#!/usr/bin/env python3
import argparse
import json
from dataclasses import dataclass, field
from pathlib import Path

import numpy as np

K_NUM_LAYERS = 24
K_NUM_PROMPTS = 3
K_HIDDEN = 1024
K_HEADS = 16
K_HEAD_DIM = 64
K_SEQ = 128
K_VOCAB = 50257
K_FC1 = 4096
K_QKV = 3072


@dataclass
class AssetBundle:
    wte: np.ndarray
    wpe: np.ndarray
    final_ln_w: np.ndarray
    final_ln_b: np.ndarray
    lm_head_w: np.ndarray
    decoder: list[str]


@dataclass
class DecodeInfo:
    next_token: int
    best_logit: float
    top_ids: list[int]
    top_logits: list[float]


@dataclass
class PromptState:
    prompt_text: str
    prompt_ids: list[int]
    generated_ids: list[int] = field(default_factory=list)
    full_sequence: list[int] = field(default_factory=list)
    next_input_token: int = -1
    next_position: int = 0


@dataclass
class LayerWeights:
    ln1_gamma: np.ndarray
    ln1_beta: np.ndarray
    qkv_w: np.ndarray
    qkv_b: np.ndarray
    wout_w: np.ndarray
    wout_b: np.ndarray
    ln2_gamma: np.ndarray
    ln2_beta: np.ndarray
    fc1_w: np.ndarray
    fc1_b: np.ndarray
    fc2_w: np.ndarray
    fc2_b: np.ndarray


class PackedLayerLoader:
    def __init__(self, io_dir: Path):
        self.io_dir = io_dir
        self._cache: dict[int, LayerWeights] = {}

    def load(self, layer: int) -> LayerWeights:
        if layer not in self._cache:
            self._cache[layer] = self._decode_layer(layer)
        return self._cache[layer]

    def _decode_layer(self, layer: int) -> LayerWeights:
        layer_dir = self.io_dir / f"layer_{layer:02d}"
        qkv_w = np.zeros((K_HIDDEN, K_QKV), dtype=np.float32)
        wout_w = np.zeros((K_HIDDEN, K_HIDDEN), dtype=np.float32)
        qkv_b = np.zeros((K_QKV,), dtype=np.float32)
        wout_b = np.zeros((K_HIDDEN,), dtype=np.float32)
        ln1_beta = np.zeros((K_HIDDEN,), dtype=np.float32)
        ln1_gamma = np.zeros((K_HIDDEN,), dtype=np.float32)

        qkv_bank_bytes = 128 * K_QKV
        wout_bank_bytes = 128 * K_HIDDEN
        qkv_bias_bytes = K_QKV
        wout_bias_bytes = K_HIDDEN
        ln_bytes = K_HIDDEN

        for bank in range(8):
            raw = np.fromfile(layer_dir / f"c0_b{bank}.bin", dtype=np.uint8)
            off = 0
            qkv_rows = dequant_raw(raw[off:off + qkv_bank_bytes]).reshape(128, K_QKV)
            off += qkv_bank_bytes
            wout_rows = dequant_raw(raw[off:off + wout_bank_bytes]).reshape(128, K_HIDDEN)
            off += wout_bank_bytes
            qkv_w[bank::8, :] = qkv_rows
            wout_w[bank::8, :] = wout_rows
            if bank == 0:
                qkv_b = dequant_raw(raw[off:off + qkv_bias_bytes]); off += qkv_bias_bytes
                wout_b = dequant_raw(raw[off:off + wout_bias_bytes]); off += wout_bias_bytes
                ln1_beta = dequant_raw(raw[off:off + ln_bytes]); off += ln_bytes
                ln1_gamma = dequant_raw(raw[off:off + ln_bytes])

        fc1_w = np.zeros((K_HIDDEN, K_FC1), dtype=np.float32)
        for bank in range(8):
            raw = np.fromfile(layer_dir / f"c1_fc1_b{bank}.bin", dtype=np.uint8)
            if bank == 0:
                fc1_rows = dequant_raw(raw).reshape(128, K_FC1)
            else:
                fc1_rows = dequant_raw(raw).reshape(128, K_FC1)
            fc1_w[bank::8, :] = fc1_rows
        fc1_b = dequant_raw(np.fromfile(layer_dir / "c1_fc1_bias.bin", dtype=np.uint8))
        ln2_beta = dequant_raw(np.fromfile(layer_dir / "c1_ln2_beta.bin", dtype=np.uint8))
        ln2_gamma = dequant_raw(np.fromfile(layer_dir / "c1_ln2_gamma.bin", dtype=np.uint8))

        fc2_w = np.zeros((K_FC1, K_HIDDEN), dtype=np.float32)
        for bank in range(8):
            raw = np.fromfile(layer_dir / f"c2_fc2_b{bank}.bin", dtype=np.uint8)
            fc2_rows = dequant_raw(raw).reshape(512, K_HIDDEN)
            fc2_w[bank::8, :] = fc2_rows
        fc2_b = dequant_raw(np.fromfile(layer_dir / "c2_fc2_bias.bin", dtype=np.uint8))

        return LayerWeights(ln1_gamma, ln1_beta, qkv_w, qkv_b, wout_w, wout_b,
                            ln2_gamma, ln2_beta, fc1_w, fc1_b, fc2_w, fc2_b)


def parse_args():
    ap = argparse.ArgumentParser(description="Software reference round traces for temp 3-prompt GPT2-medium bundle.")
    ap.add_argument("io_dir", help="Path to gpt2_medium_3chunk_generate_io bundle")
    ap.add_argument("--max-new-tokens", type=int, default=None)
    return ap.parse_args()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def read_vector_exact(path: Path, dtype) -> np.ndarray:
    return np.fromfile(path, dtype=dtype)


def decode_hex_string(hex_string: str) -> str:
    if len(hex_string) % 2:
      return ""
    return bytes.fromhex(hex_string).decode("utf-8", errors="replace")


def load_decoder(path: Path) -> list[str]:
    decoder = [f"<{i}>" for i in range(K_VOCAB)]
    with path.open("r", encoding="utf-8") as f:
        for line in f:
            line = line.rstrip("\n")
            if "\t" not in line:
                continue
            tok_str, hex_string = line.split("\t", 1)
            tok = int(tok_str)
            if 0 <= tok < K_VOCAB:
                decoder[tok] = decode_hex_string(hex_string)
    return decoder


def quantize_raw(values: np.ndarray) -> np.ndarray:
    scaled = np.rint(values.astype(np.float32) * 16.0)
    clipped = np.clip(scaled, -128.0, 127.0).astype(np.int16)
    return clipped.astype(np.int8)


def dequant_raw(raw: np.ndarray) -> np.ndarray:
    return raw.view(np.int8).astype(np.float32) / 16.0


def qdq(values: np.ndarray) -> np.ndarray:
    return quantize_raw(values).astype(np.float32) / 16.0


def fnv1a_u8(raw_u8: np.ndarray) -> int:
    acc = 1469598103934665603
    for v in raw_u8.astype(np.uint64, copy=False):
        acc ^= int(v)
        acc = (acc * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return acc


def raw_checksum_from_fxp8(raw_i8: np.ndarray) -> int:
    return fnv1a_u8(raw_i8.view(np.uint8))


def summarize_head(values: np.ndarray, count: int = 8) -> str:
    head = values[: min(count, values.shape[0])]
    return "[" + ", ".join(f"{float(v):.4f}" for v in head) + "]"


def summarize_topk(assets: AssetBundle, info: DecodeInfo) -> str:
    parts = []
    for tok, logit in zip(info.top_ids, info.top_logits):
        text = assets.decoder[tok] if 0 <= tok < K_VOCAB else "<invalid>"
        parts.append(f"{tok}:{logit:.3f}:{text}")
    return " | ".join(parts)


def layernorm(x: np.ndarray, gamma: np.ndarray, beta: np.ndarray) -> np.ndarray:
    mean = np.float32(np.mean(x, dtype=np.float64))
    var = np.float32(np.var(x, dtype=np.float64))
    inv_std = np.float32(1.0 / np.sqrt(float(var) + 1.0e-5))
    return qdq((x - mean) * inv_std * gamma + beta)


def decode_next(assets: AssetBundle, hidden: np.ndarray) -> DecodeInfo:
    mean = np.float32(np.mean(hidden, dtype=np.float64))
    var = np.float32(np.var(hidden, dtype=np.float64))
    inv_std = np.float32(1.0 / np.sqrt(float(var) + 1.0e-5))
    norm = (hidden - mean) * inv_std * assets.final_ln_w + assets.final_ln_b
    logits = assets.lm_head_w @ norm
    top_idx = np.argpartition(logits, -5)[-5:]
    top_idx = top_idx[np.argsort(logits[top_idx])[::-1]]
    return DecodeInfo(
        next_token=int(top_idx[0]),
        best_logit=float(logits[top_idx[0]]),
        top_ids=[int(i) for i in top_idx.tolist()],
        top_logits=[float(logits[i]) for i in top_idx.tolist()],
    )


def load_assets(io_dir: Path) -> AssetBundle:
    return AssetBundle(
        wte=read_vector_exact(io_dir / "wte_f32.bin", "<f4").reshape(K_VOCAB, K_HIDDEN),
        wpe=read_vector_exact(io_dir / "wpe_f32.bin", "<f4").reshape(-1, K_HIDDEN),
        final_ln_w=read_vector_exact(io_dir / "final_ln_w_f32.bin", "<f4"),
        final_ln_b=read_vector_exact(io_dir / "final_ln_b_f32.bin", "<f4"),
        lm_head_w=read_vector_exact(io_dir / "lm_head_w_f32.bin", "<f4").reshape(K_VOCAB, K_HIDDEN),
        decoder=load_decoder(io_dir / "token_decoder_hex.tsv"),
    )


def load_prompts(io_dir: Path) -> list[PromptState]:
    prompts: list[PromptState] = []
    for i in range(K_NUM_PROMPTS):
        prompt_ids = read_vector_exact(io_dir / f"prompt_{i}_token_ids.bin", "<i4").astype(np.int32)
        prompt = PromptState(
            prompt_text=read_text(io_dir / f"prompt_{i}.txt"),
            prompt_ids=[int(v) for v in prompt_ids.tolist()],
        )
        prompt.full_sequence = list(prompt.prompt_ids)
        prompts.append(prompt)
    return prompts


def gelu_tanh_fxp8(x: np.ndarray) -> np.ndarray:
    halfx = qdq(x * np.float32(0.5))
    x3 = qdq(x * x * x)
    scaled_x3 = qdq(x3 * np.float32(4.296875e-02))
    inner = qdq(x + scaled_x3)
    tanh_arg = qdq(inner * np.float32(8.125000e-01))
    tanh_val = qdq(np.tanh(tanh_arg.astype(np.float32)))
    one_plus = qdq(tanh_val + np.float32(1.0))
    return qdq(halfx * one_plus)


def run_block_round(hidden_in: np.ndarray, prompt_idx: int, cache_position: int,
                    layer: LayerWeights, kv_k_raw: np.ndarray, kv_v_raw: np.ndarray):
    ln1 = layernorm(hidden_in, layer.ln1_gamma, layer.ln1_beta)
    qkv = qdq(qdq(ln1 @ layer.qkv_w) + layer.qkv_b)
    q = qkv[:K_HIDDEN].reshape(K_HEADS, K_HEAD_DIM)
    k = qkv[K_HIDDEN: 2 * K_HIDDEN].reshape(K_HEADS, K_HEAD_DIM)
    v = qkv[2 * K_HIDDEN:].reshape(K_HEADS, K_HEAD_DIM)
    kv_k_raw[prompt_idx, cache_position] = quantize_raw(k)
    kv_v_raw[prompt_idx, cache_position] = quantize_raw(v)

    k_valid = dequant_raw(kv_k_raw[prompt_idx, : cache_position + 1].view(np.uint8)).reshape(
        cache_position + 1, K_HEADS, K_HEAD_DIM)
    v_valid = dequant_raw(kv_v_raw[prompt_idx, : cache_position + 1].view(np.uint8)).reshape(
        cache_position + 1, K_HEADS, K_HEAD_DIM)
    scores = np.einsum("hd,thd->ht", q, k_valid, dtype=np.float32) * np.float32(0.125)
    scores = scores - np.max(scores, axis=1, keepdims=True)
    probs = np.exp(scores.astype(np.float32))
    probs = probs / np.sum(probs, axis=1, keepdims=True, dtype=np.float32)
    ctx = np.einsum("ht,thd->hd", probs, v_valid, dtype=np.float32).reshape(K_HIDDEN)

    attn_proj = qdq(qdq(ctx @ layer.wout_w) + layer.wout_b)
    hidden_mid = qdq(attn_proj + hidden_in)
    bypass = hidden_mid.copy()
    ln2 = layernorm(hidden_mid, layer.ln2_gamma, layer.ln2_beta)
    fc1 = qdq(qdq(ln2 @ layer.fc1_w) + layer.fc1_b)
    gelu = gelu_tanh_fxp8(fc1)
    fc2 = qdq(qdq(gelu @ layer.fc2_w) + layer.fc2_b)
    hidden_out = qdq(fc2 + bypass)
    return hidden_mid, gelu, hidden_out


def print_ref_round_trace(assets: AssetBundle, prompt_id: int, cache_position: int, input_token: int,
                          seed_raw: np.ndarray, chunk0_raw: np.ndarray, chunk1_raw: np.ndarray,
                          hidden_raw: np.ndarray, hidden: np.ndarray, info: DecodeInfo):
    print(
        "ref_round_trace"
        f" prompt={prompt_id}"
        f" cache_position={cache_position}"
        f" input_token={input_token}"
        f" seed_checksum=0x{raw_checksum_from_fxp8(seed_raw):016x}"
        f" chunk0_checksum=0x{raw_checksum_from_fxp8(chunk0_raw):016x}"
        f" chunk1_checksum=0x{raw_checksum_from_fxp8(chunk1_raw):016x}"
        f" hidden_checksum=0x{raw_checksum_from_fxp8(hidden_raw):016x}"
        f" next_token={info.next_token}"
        f" next_text={assets.decoder[info.next_token] if 0 <= info.next_token < K_VOCAB else '<invalid>'}"
        f" seed_head={summarize_head(seed_raw.astype(np.float32) / 16.0)}"
        f" chunk0_head={summarize_head(chunk0_raw.astype(np.float32) / 16.0)}"
        f" chunk1_head={summarize_head(chunk1_raw.astype(np.float32) / 16.0)}"
        f" hidden_head={summarize_head(hidden)}"
        f" top5=[{summarize_topk(assets, info)}]"
    )


def decode_tokens(decoder: list[str], token_ids: list[int]) -> str:
    return "".join(decoder[tok] for tok in token_ids if 0 <= tok < K_VOCAB)


def main():
    args = parse_args()
    io_dir = Path(args.io_dir).resolve()
    metadata = json.loads(read_text(io_dir / "metadata.json"))
    max_new_tokens = int(metadata["max_new_tokens"]) if args.max_new_tokens is None else args.max_new_tokens

    assets = load_assets(io_dir)
    prompts = load_prompts(io_dir)
    for prompt in prompts:
        if len(prompt.prompt_ids) + max_new_tokens > K_SEQ:
            raise RuntimeError("prompt length + requested generation exceeds cache length 128")

    layers = PackedLayerLoader(io_dir)
    kv_k_raw = np.zeros((K_NUM_PROMPTS, K_NUM_LAYERS, K_SEQ, K_HEADS, K_HEAD_DIM), dtype=np.int8)
    kv_v_raw = np.zeros_like(kv_k_raw)

    def run_prompt_token(prompt_idx: int, token_id: int, position: int):
        seed_raw = quantize_raw(assets.wte[token_id] + assets.wpe[position])
        hidden = seed_raw.astype(np.float32) / 16.0
        last_chunk0_raw = np.zeros((K_HIDDEN,), dtype=np.int8)
        last_chunk1_raw = np.zeros((K_FC1,), dtype=np.int8)
        hidden_raw = seed_raw.copy()
        for layer_idx in range(K_NUM_LAYERS):
            layer = layers.load(layer_idx)
            hidden_mid, gelu, hidden = run_block_round(hidden, prompt_idx, position, layer,
                                                       kv_k_raw[:, layer_idx], kv_v_raw[:, layer_idx])
            last_chunk0_raw = quantize_raw(hidden_mid)
            last_chunk1_raw = quantize_raw(gelu)
            hidden_raw = quantize_raw(hidden)
            hidden = hidden_raw.astype(np.float32) / 16.0
        return seed_raw, last_chunk0_raw, last_chunk1_raw, hidden_raw

    for prompt_idx in range(K_NUM_PROMPTS):
        prompt = prompts[prompt_idx]
        for pos, token_id in enumerate(prompt.prompt_ids):
            seed_raw, chunk0_raw, chunk1_raw, hidden_raw = run_prompt_token(prompt_idx, token_id, pos)
        hidden = hidden_raw.astype(np.float32) / 16.0
        info = decode_next(assets, hidden)
        print_ref_round_trace(assets, prompt_idx, len(prompt.prompt_ids) - 1, prompt.prompt_ids[-1],
                              seed_raw, chunk0_raw, chunk1_raw, hidden_raw, hidden, info)
        prompt.generated_ids.append(info.next_token)
        prompt.full_sequence.append(info.next_token)
        prompt.next_input_token = info.next_token
        prompt.next_position = len(prompt.prompt_ids)

    for _gen_round in range(1, max_new_tokens):
        for prompt_idx in range(K_NUM_PROMPTS):
            prompt = prompts[prompt_idx]
            seed_raw, chunk0_raw, chunk1_raw, hidden_raw = run_prompt_token(
                prompt_idx, prompt.next_input_token, prompt.next_position)
            hidden = hidden_raw.astype(np.float32) / 16.0
            info = decode_next(assets, hidden)
            print_ref_round_trace(assets, prompt_idx, prompt.next_position, prompt.next_input_token,
                                  seed_raw, chunk0_raw, chunk1_raw, hidden_raw, hidden, info)
            prompt.generated_ids.append(info.next_token)
            prompt.full_sequence.append(info.next_token)
            prompt.next_input_token = info.next_token
            prompt.next_position += 1

    for i, prompt in enumerate(prompts):
        print(f"\n[prompt {i}]")
        print(f"input:  {prompt.prompt_text}")
        print(f"output: {decode_tokens(assets.decoder, prompt.generated_ids)}")


if __name__ == "__main__":
    main()
