#include "stage_io_common.h"

extern "C" void c0_router(
    packed_fxp32_t *seed_hidden_in,
    hls::stream<axis256_t> &feedback_in_p0,
    hls::stream<axis256_t> &feedback_in_p1,
    hls::stream<axis256_t> &feedback_in_p2,
    hls::stream<axis256_t> &hidden_to_c0,
    ap_uint<2> prompt_base,
    ap_uint<2> num_prompts,
    ap_uint<8> num_layers);

int main(int argc, char** argv) {
  if (argc != 5) {
    std::cerr << "Usage: router0_harness <scratch_dir> <prompt_base> <num_prompts> <num_layers>\n";
    return 1;
  }
  const fs::path scratch = fs::absolute(argv[1]);
  const int prompt_base = std::stoi(argv[2]);
  const int num_prompts = std::stoi(argv[3]);
  const int num_layers = std::stoi(argv[4]);

  auto seed_bytes = read_exact_bytes(scratch / "seed_hidden_in.bin", SH_NUM_PROMPTS * SH_HIDDEN);
  auto seed_words = bytes_to_packed<packed_fxp32_t>(seed_bytes);
  hls::stream<axis256_t> fb0, fb1, fb2, out;
  const fs::path fb0_path = scratch / "feedback_in_p0.bin";
  const fs::path fb1_path = scratch / "feedback_in_p1.bin";
  const fs::path fb2_path = scratch / "feedback_in_p2.bin";
  if (fs::exists(fb0_path))
    bytes_to_stream(read_file(fb0_path), fb0);
  if (fs::exists(fb1_path))
    bytes_to_stream(read_file(fb1_path), fb1);
  if (fs::exists(fb2_path))
    bytes_to_stream(read_file(fb2_path), fb2);
  c0_router(seed_words.data(), fb0, fb1, fb2, out,
            static_cast<ap_uint<2>>(prompt_base),
            static_cast<ap_uint<2>>(num_prompts),
            static_cast<ap_uint<8>>(num_layers));
  auto out_bytes = stream_to_bytes(out, static_cast<size_t>(num_prompts) * SH_HIDDEN_BEATS * static_cast<size_t>(num_layers));
  write_file(scratch / "hidden_to_c0.bin", out_bytes);
  return 0;
}
