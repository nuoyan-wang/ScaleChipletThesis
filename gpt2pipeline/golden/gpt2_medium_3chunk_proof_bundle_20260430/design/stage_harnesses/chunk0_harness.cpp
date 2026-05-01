#include "stage_io_common.h"

using packed_fxp64_t = ap_uint<512>;

void chunk0_stream_ip(
    hls::stream<axis256_t> &hidden_in,
    hls::stream<axis256_t> &hidden_mid_out,
    hls::stream<axis256_t> &bypass_out,
    ap_uint<2> prompt_base,
    ap_uint<2> num_prompts,
    ap_uint<8> num_layers,
    ap_uint<8> cache_position_p0,
    ap_uint<8> cache_position_p1,
    ap_uint<8> cache_position_p2,
    packed_fxp64_t *b0,
    packed_fxp64_t *b1,
    packed_fxp64_t *b2,
    packed_fxp64_t *b3,
    packed_fxp64_t *b4,
    packed_fxp64_t *b5,
    packed_fxp64_t *b6,
    packed_fxp64_t *b7,
    packed_fxp32_t *meta_hbm,
    packed_fxp32_t *kv_k_hbm_lo,
    packed_fxp32_t *kv_v_hbm_lo,
    packed_fxp32_t *kv_k_hbm_hi,
    packed_fxp32_t *kv_v_hbm_hi);

int main(int argc, char** argv) {
  if (argc != 10) {
    std::cerr << "Usage: chunk0_harness <io_dir> <scratch_dir> <layer_idx> <prompt_base> <num_prompts> <cache0> <cache1> <cache2> <num_layers>\n";
    return 1;
  }
  const fs::path io_dir = fs::absolute(argv[1]);
  const fs::path scratch = fs::absolute(argv[2]);
  const int layer_idx = std::stoi(argv[3]);
  const int prompt_base = std::stoi(argv[4]);
  const int num_prompts = std::stoi(argv[5]);
  const int cache0 = std::stoi(argv[6]);
  const int cache1 = std::stoi(argv[7]);
  const int cache2 = std::stoi(argv[8]);
  const int num_layers = std::stoi(argv[9]);
  const fs::path layer_dir = io_dir / ("layer_" + std::string(layer_idx < 10 ? "0" : "") + std::to_string(layer_idx));

  auto in_bytes = read_exact_bytes(scratch / "hidden_to_c0.bin", static_cast<size_t>(num_prompts) * SH_HIDDEN * static_cast<size_t>(num_layers));
  hls::stream<axis256_t> in, mid, bypass;
  bytes_to_stream(in_bytes, in);

  auto b0_full = read_exact_bytes(layer_dir / "c0_b0.bin", 530432);
  auto b0_bytes = std::vector<uint8_t>(b0_full.begin(), b0_full.begin() + 524288);
  auto meta_bytes = std::vector<uint8_t>(b0_full.begin() + 524288, b0_full.end());
  auto b0 = bytes_to_packed<packed_fxp64_t>(b0_bytes);
  auto meta = bytes_to_packed<packed_fxp32_t>(meta_bytes);
  auto b1 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c0_b1.bin", 524288));
  auto b2 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c0_b2.bin", 524288));
  auto b3 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c0_b3.bin", 524288));
  auto b4 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c0_b4.bin", 524288));
  auto b5 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c0_b5.bin", 524288));
  auto b6 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c0_b6.bin", 524288));
  auto b7 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c0_b7.bin", 524288));

  auto kv_k_lo = bytes_to_packed<packed_fxp32_t>(read_exact_bytes(scratch / "kv_k_lo.bin", SH_C0_KV_SLICES * SH_C0_KV_HALF_SLICE_WORDS * SH_WORD_BYTES_256));
  auto kv_v_lo = bytes_to_packed<packed_fxp32_t>(read_exact_bytes(scratch / "kv_v_lo.bin", SH_C0_KV_SLICES * SH_C0_KV_HALF_SLICE_WORDS * SH_WORD_BYTES_256));
  auto kv_k_hi = bytes_to_packed<packed_fxp32_t>(read_exact_bytes(scratch / "kv_k_hi.bin", SH_C0_KV_SLICES * SH_C0_KV_HALF_SLICE_WORDS * SH_WORD_BYTES_256));
  auto kv_v_hi = bytes_to_packed<packed_fxp32_t>(read_exact_bytes(scratch / "kv_v_hi.bin", SH_C0_KV_SLICES * SH_C0_KV_HALF_SLICE_WORDS * SH_WORD_BYTES_256));

  const size_t kv_layer_off_half = static_cast<size_t>(layer_idx) * 3 * SH_C0_KV_HALF_SLICE_WORDS;
  run_with_large_stack([&]() {
    chunk0_stream_ip(
        in, mid, bypass,
        static_cast<ap_uint<2>>(prompt_base), static_cast<ap_uint<2>>(num_prompts), static_cast<ap_uint<8>>(num_layers),
        static_cast<ap_uint<8>>(cache0), static_cast<ap_uint<8>>(cache1), static_cast<ap_uint<8>>(cache2),
        b0.data(), b1.data(), b2.data(), b3.data(), b4.data(), b5.data(), b6.data(), b7.data(), meta.data(),
        kv_k_lo.data() + kv_layer_off_half, kv_v_lo.data() + kv_layer_off_half,
        kv_k_hi.data() + kv_layer_off_half, kv_v_hi.data() + kv_layer_off_half);
  });

  write_file(scratch / "hidden_mid_out.bin", stream_to_bytes(mid, static_cast<size_t>(num_prompts) * SH_HIDDEN_BEATS * static_cast<size_t>(num_layers)));
  write_file(scratch / "bypass0_out.bin", stream_to_bytes(bypass, static_cast<size_t>(num_prompts) * SH_HIDDEN_BEATS * static_cast<size_t>(num_layers)));
  write_file(scratch / "kv_k_lo.bin", packed_to_bytes(kv_k_lo));
  write_file(scratch / "kv_v_lo.bin", packed_to_bytes(kv_v_lo));
  write_file(scratch / "kv_k_hi.bin", packed_to_bytes(kv_k_hi));
  write_file(scratch / "kv_v_hi.bin", packed_to_bytes(kv_v_hi));
  return 0;
}
