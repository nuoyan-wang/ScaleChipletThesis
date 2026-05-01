#include "stage_io_common.h"

void chunk2_stream_ip(
    hls::stream<axis256_t> &gelu_in,
    hls::stream<axis256_t> &bypass_in,
    hls::stream<axis256_t> &hidden_out,
    ap_uint<2> num_prompts,
    ap_uint<8> num_layers,
    packed_fxp64_t *fc2_b0,
    packed_fxp32_t *fc2_bias,
    packed_fxp64_t *fc2_b1,
    packed_fxp64_t *fc2_b2,
    packed_fxp64_t *fc2_b3,
    packed_fxp64_t *fc2_b4,
    packed_fxp64_t *fc2_b5,
    packed_fxp64_t *fc2_b6,
    packed_fxp64_t *fc2_b7);

int main(int argc, char** argv) {
  if (argc != 6) {
    std::cerr << "Usage: chunk2_harness <io_dir> <scratch_dir> <layer_idx> <num_prompts> <num_layers>\n";
    return 1;
  }
  const fs::path io_dir = fs::absolute(argv[1]);
  const fs::path scratch = fs::absolute(argv[2]);
  const int layer_idx = std::stoi(argv[3]);
  const int num_prompts = std::stoi(argv[4]);
  const int num_layers = std::stoi(argv[5]);
  const fs::path layer_dir = io_dir / ("layer_" + std::string(layer_idx < 10 ? "0" : "") + std::to_string(layer_idx));

  hls::stream<axis256_t> gelu_in, bypass_in, hidden_out;
  bytes_to_stream(read_exact_bytes(scratch / "gelu_out.bin", static_cast<size_t>(num_prompts) * SH_FC1 * static_cast<size_t>(num_layers)), gelu_in);
  bytes_to_stream(read_exact_bytes(scratch / "bypass1_out.bin", static_cast<size_t>(num_prompts) * SH_HIDDEN * static_cast<size_t>(num_layers)), bypass_in);

  auto fc2_b0 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c2_fc2_b0.bin", 524288));
  auto fc2_b1 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c2_fc2_b1.bin", 524288));
  auto fc2_b2 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c2_fc2_b2.bin", 524288));
  auto fc2_b3 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c2_fc2_b3.bin", 524288));
  auto fc2_b4 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c2_fc2_b4.bin", 524288));
  auto fc2_b5 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c2_fc2_b5.bin", 524288));
  auto fc2_b6 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c2_fc2_b6.bin", 524288));
  auto fc2_b7 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c2_fc2_b7.bin", 524288));
  auto fc2_bias = bytes_to_packed<packed_fxp32_t>(read_exact_bytes(layer_dir / "c2_fc2_bias.bin", 1024));

  run_with_large_stack([&]() {
    chunk2_stream_ip(gelu_in, bypass_in, hidden_out,
                     static_cast<ap_uint<2>>(num_prompts), static_cast<ap_uint<8>>(num_layers),
                     fc2_b0.data(), fc2_bias.data(), fc2_b1.data(), fc2_b2.data(),
                     fc2_b3.data(), fc2_b4.data(), fc2_b5.data(), fc2_b6.data(), fc2_b7.data());
  });

  write_file(scratch / "hidden_from_c2.bin", stream_to_bytes(hidden_out, static_cast<size_t>(num_prompts) * SH_HIDDEN_BEATS * static_cast<size_t>(num_layers)));
  return 0;
}
