#!/usr/bin/env python3
import argparse
import os
import shutil
import subprocess
from pathlib import Path

import numpy as np


ROOT = Path(__file__).resolve().parent
BIN = ROOT / "bin"
IO_DEFAULT = ROOT / "golden_io"

NUM_PROMPTS = 3
GOLDEN_KV_WORDS = 16 * 128
GOLDEN_KV_BYTES = GOLDEN_KV_WORDS * 64
SPLIT_KV_HALF_BYTES = 72 * 2048 * 32
SPLIT_KVT_BYTES = 72 * 4096 * 32
HIDDEN_BYTES = 1024


def run(cmd, env=None):
    subprocess.run(cmd, check=True, env=env)


def fnv1a(data: bytes) -> int:
    acc = 1469598103934665603
    for b in data:
        acc ^= b
        acc = (acc * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return acc


def ensure_zero_file(path: Path, nbytes: int):
    path.write_bytes(bytes(nbytes))


def read_prompt_ids(path: Path):
    return np.fromfile(path, dtype=np.int32).tolist()


def decode_next_token(hidden_bytes: bytes, final_ln_w: np.ndarray, final_ln_b: np.ndarray,
                      lm_head_w: np.ndarray) -> int:
    raw = np.frombuffer(hidden_bytes, dtype=np.uint8)
    if raw.size != 1024:
        raise RuntimeError("unexpected hidden size")
    hidden = raw.view(np.int8).astype(np.float32) / 16.0
    mean = float(hidden.mean())
    var = float(((hidden - mean) ** 2).mean())
    inv_std = 1.0 / np.sqrt(var + 1.0e-5)
    norm = (hidden - mean) * inv_std * final_ln_w + final_ln_b
    logits = lm_head_w @ norm
    return int(np.argmax(logits))


def compare_bytes(label: str, golden: bytes, actual: bytes):
    if golden == actual:
        return None
    first = next((i for i, (a, b) in enumerate(zip(golden, actual)) if a != b), None)
    return {
        "label": label,
        "first_diff": first,
        "golden": fnv1a(golden),
        "actual": fnv1a(actual),
    }


def concat_slot_chunks(chunks_by_slot):
    return b"".join(chunks_by_slot)


def build_expected_feedback(hidden_out_by_layer, prompt_slot: int):
    return b"".join(hidden_out_by_layer[layer][prompt_slot] for layer in range(len(hidden_out_by_layer) - 1))


def build_expected_hidden_to_c0(seed_by_slot, hidden_out_by_layer):
    parts = [concat_slot_chunks(seed_by_slot)]
    for layer in range(len(hidden_out_by_layer) - 1):
        parts.append(concat_slot_chunks(hidden_out_by_layer[layer]))
    return b"".join(parts)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--io-dir", default=str(IO_DEFAULT))
    ap.add_argument("--prompts", type=int, nargs=3, default=[0, 1, 2],
                    help="Three prompt indices to interleave as slots P0/P1/P2")
    ap.add_argument("--rounds", type=int, default=1)
    ap.add_argument("--layers", type=int, default=24)
    ap.add_argument("--scratch-dir", default="/tmp/newest_routered_stage_seq_triprompt_distinct_bundle")
    ap.add_argument("--stop-on-first-mismatch", action="store_true")
    args = ap.parse_args()

    io_dir = Path(args.io_dir).resolve()
    scratch = Path(args.scratch_dir).resolve()
    golden_root = scratch / "golden"
    split = scratch / "split"
    golden_kv = scratch / "golden_kv"

    shutil.rmtree(scratch, ignore_errors=True)
    golden_root.mkdir(parents=True)
    split.mkdir(parents=True)
    golden_kv.mkdir(parents=True)

    prompt_indices = list(args.prompts)
    prompt_ids = [read_prompt_ids(io_dir / f"prompt_{prompt_idx}_token_ids.bin")
                  for prompt_idx in prompt_indices]
    final_ln_w = np.fromfile(io_dir / "final_ln_w_f32.bin", dtype=np.float32)
    final_ln_b = np.fromfile(io_dir / "final_ln_b_f32.bin", dtype=np.float32)
    lm_head_w = np.fromfile(io_dir / "lm_head_w_f32.bin", dtype=np.float32).reshape(-1, 1024)
    next_tokens = [None] * NUM_PROMPTS

    ensure_zero_file(split / "kv_k_lo.bin", SPLIT_KV_HALF_BYTES)
    ensure_zero_file(split / "kv_v_lo.bin", SPLIT_KV_HALF_BYTES)
    ensure_zero_file(split / "kv_k_hi.bin", SPLIT_KV_HALF_BYTES)
    ensure_zero_file(split / "kv_v_hi.bin", SPLIT_KV_HALF_BYTES)
    ensure_zero_file(split / "kv_k_t.bin", SPLIT_KVT_BYTES)

    golden_kv_k = [[None] * args.layers for _ in range(NUM_PROMPTS)]
    golden_kv_v = [[None] * args.layers for _ in range(NUM_PROMPTS)]
    for prompt_slot in range(NUM_PROMPTS):
        slot_dir = golden_kv / f"slot_{prompt_slot}"
        slot_dir.mkdir(parents=True, exist_ok=True)
        for layer in range(args.layers):
            k = slot_dir / f"layer_{layer:02d}_k.bin"
            v = slot_dir / f"layer_{layer:02d}_v.bin"
            ensure_zero_file(k, GOLDEN_KV_BYTES)
            ensure_zero_file(v, GOLDEN_KV_BYTES)
            golden_kv_k[prompt_slot][layer] = k
            golden_kv_v[prompt_slot][layer] = v

    total = 0
    mismatch_count = 0
    first_mismatch = None

    for position in range(args.rounds):
        current_tokens = []
        for prompt_slot in range(NUM_PROMPTS):
            slot_prompt_ids = prompt_ids[prompt_slot]
            if position < len(slot_prompt_ids):
                current_tokens.append(int(slot_prompt_ids[position]))
            else:
                if next_tokens[prompt_slot] is None:
                    raise RuntimeError(f"missing generated next_token for slot {prompt_slot}")
                current_tokens.append(int(next_tokens[prompt_slot]))

        prev_golden_hidden = [None] * NUM_PROMPTS
        golden_seed_by_slot = None
        golden_hidden_out_by_layer = []
        actual_hidden_out_by_layer = []

        for layer in range(args.layers):
            golden_dirs = []
            for prompt_slot in range(NUM_PROMPTS):
                prompt_idx = prompt_indices[prompt_slot]
                golden_dir = golden_root / f"p{position:02d}_slot{prompt_slot}_prompt{prompt_idx}_l{layer:02d}"
                golden_dir.mkdir(parents=True, exist_ok=True)
                golden_dirs.append(golden_dir)

                env = os.environ.copy()
                env["MODEL_DUMP_DIR"] = str(golden_dir)
                env["MODEL_KV_K_IN_FILE"] = str(golden_kv_k[prompt_slot][layer])
                env["MODEL_KV_V_IN_FILE"] = str(golden_kv_v[prompt_slot][layer])
                env["MODEL_KV_K_OUT_FILE"] = str(golden_kv_k[prompt_slot][layer])
                env["MODEL_KV_V_OUT_FILE"] = str(golden_kv_v[prompt_slot][layer])
                env["MODEL_TOKEN_ID"] = str(current_tokens[prompt_slot])
                env["MODEL_QUIET"] = "1"
                if prev_golden_hidden[prompt_slot] is not None:
                    env["MODEL_HIDDEN_IN_FILE"] = str(prev_golden_hidden[prompt_slot])
                else:
                    env.pop("MODEL_HIDDEN_IN_FILE", None)

                run(
                    [
                        str(BIN / "tb_newest_layer_boundary"),
                        str(io_dir),
                        str(layer),
                        str(prompt_idx),
                        str(position),
                    ],
                    env=env,
                )

            if layer == 0:
                golden_seed_by_slot = [(gd / "seed_hidden.bin").read_bytes() for gd in golden_dirs]
                (split / "hidden_to_c0.bin").write_bytes(concat_slot_chunks(golden_seed_by_slot))
            else:
                (split / "hidden_to_c0.bin").write_bytes(
                    (split / "hidden_from_c2.bin").read_bytes()[: NUM_PROMPTS * HIDDEN_BYTES]
                )

            run(
                [
                    str(BIN / "chunk0_harness"),
                    str(io_dir),
                    str(split),
                    str(layer),
                    "0",
                    str(NUM_PROMPTS),
                    str(position),
                    str(position),
                    str(position),
                    "1",
                ]
            )

            run(
                [
                    str(BIN / "chunk1_harness"),
                    str(io_dir),
                    str(split),
                    str(layer),
                    str(NUM_PROMPTS),
                    "1",
                ],
                env=dict(os.environ, CHUNK1_QUIET="1"),
            )

            run(
                [
                    str(BIN / "chunk2_harness"),
                    str(io_dir),
                    str(split),
                    str(layer),
                    str(NUM_PROMPTS),
                    "1",
                ]
            )

            hidden_out_bytes = (split / "hidden_from_c2.bin").read_bytes()[: NUM_PROMPTS * HIDDEN_BYTES]
            slot_hidden_out = [
                hidden_out_bytes[p * HIDDEN_BYTES:(p + 1) * HIDDEN_BYTES] for p in range(NUM_PROMPTS)
            ]
            golden_hidden_out_by_layer.append([(gd / "hidden_out.bin").read_bytes() for gd in golden_dirs])
            actual_hidden_out_by_layer.append(slot_hidden_out)

            for prompt_slot in range(NUM_PROMPTS):
                prev_golden_hidden[prompt_slot] = golden_dirs[prompt_slot] / "hidden_out.bin"

        actual_hidden_from_c2 = b"".join(
            b"".join(actual_hidden_out_by_layer[layer][slot] for slot in range(NUM_PROMPTS))
            for layer in range(args.layers)
        )
        (split / "hidden_from_c2.bin").write_bytes(actual_hidden_from_c2)
        (split / "seed_hidden_in.bin").write_bytes(concat_slot_chunks(golden_seed_by_slot))

        run([str(BIN / "router2_harness"), str(split), "0", str(NUM_PROMPTS), str(args.layers)])

        router2_checks = []
        expected_final_hidden = concat_slot_chunks(golden_hidden_out_by_layer[-1])
        router2_checks.append(compare_bytes(
            "router2.final_hidden_out",
            expected_final_hidden,
            (split / "final_hidden_out.bin").read_bytes()[: len(expected_final_hidden)],
        ))
        for slot in range(NUM_PROMPTS):
            expected_fb = build_expected_feedback(golden_hidden_out_by_layer, slot)
            router2_checks.append(compare_bytes(
                f"router2.feedback_p{slot}",
                expected_fb,
                (split / f"feedback_to_c0_p{slot}.bin").read_bytes()[: len(expected_fb)],
            ))

        shutil.copyfile(split / "feedback_to_c0_p0.bin", split / "feedback_in_p0.bin")
        shutil.copyfile(split / "feedback_to_c0_p1.bin", split / "feedback_in_p1.bin")
        shutil.copyfile(split / "feedback_to_c0_p2.bin", split / "feedback_in_p2.bin")
        run([str(BIN / "router0_harness"), str(split), "0", str(NUM_PROMPTS), str(args.layers)])

        expected_hidden_to_c0 = build_expected_hidden_to_c0(golden_seed_by_slot, golden_hidden_out_by_layer)
        router0_check = compare_bytes(
            "router0.hidden_to_c0",
            expected_hidden_to_c0,
            (split / "hidden_to_c0.bin").read_bytes()[: len(expected_hidden_to_c0)],
        )

        total += 1
        mismatches = [m for m in router2_checks if m is not None]
        if router0_check is not None:
            mismatches.append(router0_check)

        if mismatches:
            mismatch_count += 1
            if first_mismatch is None:
                first_mismatch = (position, mismatches)
            print(f"position={position} router_mismatch")
            for info in mismatches:
                print(
                    f"  {info['label']}: first_diff={info['first_diff']} "
                    f"golden=0x{info['golden']:016x} actual=0x{info['actual']:016x}"
                )
            if args.stop_on_first_mismatch:
                print("stopping on first mismatch")
                print(f"checked={total} mismatching_rounds={mismatch_count}")
                return
        else:
            print(f"position={position} router_match")

        split_final = (split / "final_hidden_out.bin").read_bytes()[: NUM_PROMPTS * HIDDEN_BYTES]
        for prompt_slot in range(NUM_PROMPTS):
            start = prompt_slot * HIDDEN_BYTES
            end = start + HIDDEN_BYTES
            next_tokens[prompt_slot] = decode_next_token(
                split_final[start:end], final_ln_w, final_ln_b, lm_head_w
            )

    print(f"checked={total} mismatching_rounds={mismatch_count}")
    if first_mismatch is None:
        print("all_router_rounds_match=True")
    else:
        pos, _ = first_mismatch
        print(f"all_router_rounds_match=False first_mismatch_position={pos}")


if __name__ == "__main__":
    main()
