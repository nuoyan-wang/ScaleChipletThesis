#include "stage_io_common.h"

extern "C" void c2_router(
    hls::stream<axis256_t> &hidden_from_c2,
    hls::stream<axis256_t> &feedback_to_c0_p0,
    hls::stream<axis256_t> &feedback_to_c0_p1,
    hls::stream<axis256_t> &feedback_to_c0_p2,
    packed_fxp32_t *final_hidden_out,
    ap_uint<2> prompt_base,
    ap_uint<2> num_prompts,
    ap_uint<8> num_layers);

int main(int argc, char** argv) {
  if (argc != 5) {
    std::cerr << "Usage: router2_harness <scratch_dir> <prompt_base> <num_prompts> <num_layers>\n";
    return 1;
  }
  const fs::path scratch = fs::absolute(argv[1]);
  const int prompt_base = std::stoi(argv[2]);
  const int num_prompts = std::stoi(argv[3]);
  const int num_layers = std::stoi(argv[4]);

  auto in_bytes = read_exact_bytes(scratch / "hidden_from_c2.bin", static_cast<size_t>(num_prompts) * SH_HIDDEN * static_cast<size_t>(num_layers));
  hls::stream<axis256_t> in, fb0, fb1, fb2;
  bytes_to_stream(in_bytes, in);
  std::vector<packed_fxp32_t> final_words(static_cast<size_t>(SH_NUM_PROMPTS) * SH_HIDDEN_BEATS, 0);
  c2_router(in, fb0, fb1, fb2, final_words.data(),
            static_cast<ap_uint<2>>(prompt_base),
            static_cast<ap_uint<2>>(num_prompts),
            static_cast<ap_uint<8>>(num_layers));
  auto final_bytes = packed_to_bytes(final_words);
  final_bytes.resize(SH_NUM_PROMPTS * SH_HIDDEN);
  write_file(scratch / "final_hidden_out.bin", final_bytes);
  const size_t feedback_beats = (num_layers > 1) ? (static_cast<size_t>(num_layers - 1) * SH_HIDDEN_BEATS) : 0;
  write_file(scratch / "feedback_to_c0_p0.bin", stream_to_bytes(fb0, feedback_beats));
  write_file(scratch / "feedback_to_c0_p1.bin", stream_to_bytes(fb1, feedback_beats));
  write_file(scratch / "feedback_to_c0_p2.bin", stream_to_bytes(fb2, feedback_beats));
  return 0;
}
