#include "stage_io_common.h"

void chunk1_stream_ip(
    hls::stream<axis256_t> &hidden_mid_in,
    hls::stream<axis256_t> &bypass_in,
    hls::stream<axis256_t> &gelu_out,
    hls::stream<axis256_t> &bypass_out,
    ap_uint<2> num_prompts,
    ap_uint<8> num_layers,
    packed_fxp64_t *fc1_b0,
    packed_fxp32_t *fc1_bias,
    packed_fxp32_t *ln2_beta,
    packed_fxp32_t *ln2_gamma,
    packed_fxp64_t *fc1_b1,
    packed_fxp64_t *fc1_b2,
    packed_fxp64_t *fc1_b3,
    packed_fxp64_t *fc1_b4,
    packed_fxp64_t *fc1_b5,
    packed_fxp64_t *fc1_b6,
    packed_fxp64_t *fc1_b7);

int main(int argc, char** argv) {
  if (argc != 6) {
    std::cerr << "Usage: chunk1_harness <io_dir> <scratch_dir> <layer_idx> <num_prompts> <num_layers>\n";
    return 1;
  }
  const fs::path io_dir = fs::absolute(argv[1]);
  const fs::path scratch = fs::absolute(argv[2]);
  const int layer_idx = std::stoi(argv[3]);
  const int num_prompts = std::stoi(argv[4]);
  const int num_layers = std::stoi(argv[5]);
  const fs::path layer_dir = io_dir / ("layer_" + std::string(layer_idx < 10 ? "0" : "") + std::to_string(layer_idx));

  hls::stream<axis256_t> hidden_mid_in, bypass_in, gelu_out, bypass_out;
  bytes_to_stream(read_exact_bytes(scratch / "hidden_mid_out.bin", static_cast<size_t>(num_prompts) * SH_HIDDEN * static_cast<size_t>(num_layers)), hidden_mid_in);
  bytes_to_stream(read_exact_bytes(scratch / "bypass0_out.bin", static_cast<size_t>(num_prompts) * SH_HIDDEN * static_cast<size_t>(num_layers)), bypass_in);

  auto fc1_b0 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c1_fc1_b0.bin", 524288));
  auto fc1_b1 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c1_fc1_b1.bin", 524288));
  auto fc1_b2 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c1_fc1_b2.bin", 524288));
  auto fc1_b3 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c1_fc1_b3.bin", 524288));
  auto fc1_b4 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c1_fc1_b4.bin", 524288));
  auto fc1_b5 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c1_fc1_b5.bin", 524288));
  auto fc1_b6 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c1_fc1_b6.bin", 524288));
  auto fc1_b7 = bytes_to_packed<packed_fxp64_t>(read_exact_bytes(layer_dir / "c1_fc1_b7.bin", 524288));
  auto fc1_bias = bytes_to_packed<packed_fxp32_t>(read_exact_bytes(layer_dir / "c1_fc1_bias.bin", 4096));
  auto ln2_beta = bytes_to_packed<packed_fxp32_t>(read_exact_bytes(layer_dir / "c1_ln2_beta.bin", 1024));
  auto ln2_gamma = bytes_to_packed<packed_fxp32_t>(read_exact_bytes(layer_dir / "c1_ln2_gamma.bin", 1024));

  run_with_large_stack([&]() {
    chunk1_stream_ip(hidden_mid_in, bypass_in, gelu_out, bypass_out,
                     static_cast<ap_uint<2>>(num_prompts), static_cast<ap_uint<8>>(num_layers),
                     fc1_b0.data(), fc1_bias.data(), ln2_beta.data(), ln2_gamma.data(),
                     fc1_b1.data(), fc1_b2.data(), fc1_b3.data(), fc1_b4.data(),
                     fc1_b5.data(), fc1_b6.data(), fc1_b7.data());
  });

  write_file(scratch / "gelu_out.bin", stream_to_bytes(gelu_out, static_cast<size_t>(num_prompts) * SH_FC1_BEATS * static_cast<size_t>(num_layers)));
  write_file(scratch / "bypass1_out.bin", stream_to_bytes(bypass_out, static_cast<size_t>(num_prompts) * SH_HIDDEN_BEATS * static_cast<size_t>(num_layers)));
  return 0;
}
