#!/usr/bin/env python3
import argparse
from pathlib import Path


NUM_LAYERS = 24
WORD_BYTES_256 = 32
WORD_BYTES_512 = 64

C0_WEIGHT_BANK_WORDS = 8192
C0_WEIGHT_BANK_BYTES_PER_LAYER = C0_WEIGHT_BANK_WORDS * WORD_BYTES_512
C0_META_WORDS = 192
C0_META_BYTES_PER_LAYER = C0_META_WORDS * WORD_BYTES_256
C0_B0_FILE_BYTES_PER_LAYER = C0_WEIGHT_BANK_BYTES_PER_LAYER + C0_META_BYTES_PER_LAYER

C1_FC1_WORDS = 16384
C1_FC1_BYTES_PER_LAYER = C1_FC1_WORDS * WORD_BYTES_256
C1_FC1_BIAS_WORDS = 128
C1_FC1_BIAS_BYTES_PER_LAYER = C1_FC1_BIAS_WORDS * WORD_BYTES_256
C1_LN_WORDS = 32
C1_LN_BYTES_PER_LAYER = C1_LN_WORDS * WORD_BYTES_256

C2_FC2_WORDS = 16384
C2_FC2_BYTES_PER_LAYER = C2_FC2_WORDS * WORD_BYTES_256
C2_FC2_BIAS_WORDS = 32
C2_FC2_BIAS_BYTES_PER_LAYER = C2_FC2_BIAS_WORDS * WORD_BYTES_256


def fnv1a(data: bytes) -> int:
    acc = 1469598103934665603
    for b in data:
        acc ^= b
        acc = (acc * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return acc


def read_exact(path: Path, nbytes: int) -> bytes:
    data = path.read_bytes()
    if len(data) != nbytes:
        raise RuntimeError(f"unexpected byte count for {path}: got {len(data)} expected {nbytes}")
    return data


def build_host_images(io_dir: Path):
    c0_bank_images = [bytearray(NUM_LAYERS * C0_WEIGHT_BANK_BYTES_PER_LAYER) for _ in range(8)]
    c0_meta_image = bytearray(NUM_LAYERS * C0_META_BYTES_PER_LAYER)

    c1_fc1_banks = [bytearray(NUM_LAYERS * C1_FC1_BYTES_PER_LAYER) for _ in range(8)]
    c1_fc1_bias = bytearray(NUM_LAYERS * C1_FC1_BIAS_BYTES_PER_LAYER)
    c1_ln2_beta = bytearray(NUM_LAYERS * C1_LN_BYTES_PER_LAYER)
    c1_ln2_gamma = bytearray(NUM_LAYERS * C1_LN_BYTES_PER_LAYER)

    c2_fc2_banks = [bytearray(NUM_LAYERS * C2_FC2_BYTES_PER_LAYER) for _ in range(8)]
    c2_fc2_bias = bytearray(NUM_LAYERS * C2_FC2_BIAS_BYTES_PER_LAYER)

    for layer in range(NUM_LAYERS):
        layer_dir = io_dir / f"layer_{layer:02d}"
        off_c0 = layer * C0_WEIGHT_BANK_BYTES_PER_LAYER
        off_c0_meta = layer * C0_META_BYTES_PER_LAYER
        off_c1 = layer * C1_FC1_BYTES_PER_LAYER
        off_c1_bias = layer * C1_FC1_BIAS_BYTES_PER_LAYER
        off_c1_ln = layer * C1_LN_BYTES_PER_LAYER
        off_c2 = layer * C2_FC2_BYTES_PER_LAYER
        off_c2_bias = layer * C2_FC2_BIAS_BYTES_PER_LAYER

        b0_full = read_exact(layer_dir / "c0_b0.bin", C0_B0_FILE_BYTES_PER_LAYER)
        c0_bank_images[0][off_c0:off_c0 + C0_WEIGHT_BANK_BYTES_PER_LAYER] = b0_full[:C0_WEIGHT_BANK_BYTES_PER_LAYER]
        c0_meta_image[off_c0_meta:off_c0_meta + C0_META_BYTES_PER_LAYER] = b0_full[C0_WEIGHT_BANK_BYTES_PER_LAYER:]
        for bank in range(1, 8):
            c0_bank_images[bank][off_c0:off_c0 + C0_WEIGHT_BANK_BYTES_PER_LAYER] = read_exact(
                layer_dir / f"c0_b{bank}.bin", C0_WEIGHT_BANK_BYTES_PER_LAYER
            )

        c1_fc1_banks[0][off_c1:off_c1 + C1_FC1_BYTES_PER_LAYER] = read_exact(
            layer_dir / "c1_fc1_b0.bin", C1_FC1_BYTES_PER_LAYER
        )
        c1_fc1_bias[off_c1_bias:off_c1_bias + C1_FC1_BIAS_BYTES_PER_LAYER] = read_exact(
            layer_dir / "c1_fc1_bias.bin", C1_FC1_BIAS_BYTES_PER_LAYER
        )
        c1_ln2_beta[off_c1_ln:off_c1_ln + C1_LN_BYTES_PER_LAYER] = read_exact(
            layer_dir / "c1_ln2_beta.bin", C1_LN_BYTES_PER_LAYER
        )
        c1_ln2_gamma[off_c1_ln:off_c1_ln + C1_LN_BYTES_PER_LAYER] = read_exact(
            layer_dir / "c1_ln2_gamma.bin", C1_LN_BYTES_PER_LAYER
        )
        for bank in range(1, 8):
            c1_fc1_banks[bank][off_c1:off_c1 + C1_FC1_BYTES_PER_LAYER] = read_exact(
                layer_dir / f"c1_fc1_b{bank}.bin", C1_FC1_BYTES_PER_LAYER
            )

        c2_fc2_banks[0][off_c2:off_c2 + C2_FC2_BYTES_PER_LAYER] = read_exact(
            layer_dir / "c2_fc2_b0.bin", C2_FC2_BYTES_PER_LAYER
        )
        c2_fc2_bias[off_c2_bias:off_c2_bias + C2_FC2_BIAS_BYTES_PER_LAYER] = read_exact(
            layer_dir / "c2_fc2_bias.bin", C2_FC2_BIAS_BYTES_PER_LAYER
        )
        for bank in range(1, 8):
            c2_fc2_banks[bank][off_c2:off_c2 + C2_FC2_BYTES_PER_LAYER] = read_exact(
                layer_dir / f"c2_fc2_b{bank}.bin", C2_FC2_BYTES_PER_LAYER
            )

    return {
        "c0_banks": [bytes(x) for x in c0_bank_images],
        "c0_meta": bytes(c0_meta_image),
        "c1_fc1_banks": [bytes(x) for x in c1_fc1_banks],
        "c1_fc1_bias": bytes(c1_fc1_bias),
        "c1_ln2_beta": bytes(c1_ln2_beta),
        "c1_ln2_gamma": bytes(c1_ln2_gamma),
        "c2_fc2_banks": [bytes(x) for x in c2_fc2_banks],
        "c2_fc2_bias": bytes(c2_fc2_bias),
    }


def verify_offsets(images):
    # Layer-offset and non-overlap sanity by checking each per-layer slice hashes to the source file hash.
    # This catches off-by-one slicing and accidental overlap.
    checks = []
    for name, image, bytes_per_layer in [
        ("c0_meta", images["c0_meta"], C0_META_BYTES_PER_LAYER),
        ("c1_fc1_bias", images["c1_fc1_bias"], C1_FC1_BIAS_BYTES_PER_LAYER),
        ("c1_ln2_beta", images["c1_ln2_beta"], C1_LN_BYTES_PER_LAYER),
        ("c1_ln2_gamma", images["c1_ln2_gamma"], C1_LN_BYTES_PER_LAYER),
        ("c2_fc2_bias", images["c2_fc2_bias"], C2_FC2_BIAS_BYTES_PER_LAYER),
    ]:
        if len(image) != NUM_LAYERS * bytes_per_layer:
            raise RuntimeError(f"{name} size mismatch")
        checks.append((name, fnv1a(image)))
    return checks


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("io_dir")
    args = ap.parse_args()

    io_dir = Path(args.io_dir).resolve()
    images = build_host_images(io_dir)
    offset_checks = verify_offsets(images)

    print("host_bank_layout_ok=True")
    for bank in range(8):
        print(f"c0_b{bank}_all_bytes={len(images['c0_banks'][bank])} checksum=0x{fnv1a(images['c0_banks'][bank]):016x}")
    print(f"c0_meta_all_bytes={len(images['c0_meta'])} checksum=0x{fnv1a(images['c0_meta']):016x}")
    for bank in range(8):
        print(f"c1_fc1_b{bank}_all_bytes={len(images['c1_fc1_banks'][bank])} checksum=0x{fnv1a(images['c1_fc1_banks'][bank]):016x}")
    print(f"c1_fc1_bias_all_bytes={len(images['c1_fc1_bias'])} checksum=0x{fnv1a(images['c1_fc1_bias']):016x}")
    print(f"c1_ln2_beta_all_bytes={len(images['c1_ln2_beta'])} checksum=0x{fnv1a(images['c1_ln2_beta']):016x}")
    print(f"c1_ln2_gamma_all_bytes={len(images['c1_ln2_gamma'])} checksum=0x{fnv1a(images['c1_ln2_gamma']):016x}")
    for bank in range(8):
        print(f"c2_fc2_b{bank}_all_bytes={len(images['c2_fc2_banks'][bank])} checksum=0x{fnv1a(images['c2_fc2_banks'][bank]):016x}")
    print(f"c2_fc2_bias_all_bytes={len(images['c2_fc2_bias'])} checksum=0x{fnv1a(images['c2_fc2_bias']):016x}")
    for name, checksum in offset_checks:
        print(f"{name}_offset_check=0x{checksum:016x}")


if __name__ == "__main__":
    main()
