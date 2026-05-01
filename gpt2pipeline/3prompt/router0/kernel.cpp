#include "../stream_abi_common.h"

static const int HIDDEN_BEATS = 1024 / 32;
static const int ROUTER_NUM_PROMPTS = 3;
static const int ROUTER_NUM_LAYERS = 24;

extern "C" void c0_router(
    packed_fxp32_t *seed_hidden_in,
    hls::stream<axis256_t> &feedback_in_p0,
    hls::stream<axis256_t> &feedback_in_p1,
    hls::stream<axis256_t> &feedback_in_p2,
    hls::stream<axis256_t> &hidden_to_c0,
    ap_uint<2> prompt_base,
    ap_uint<2> num_prompts,
    ap_uint<8> num_layers) {
#pragma HLS interface m_axi port=seed_hidden_in offset=slave bundle=SEED_HBM max_read_burst_length=32 num_read_outstanding=4
#pragma HLS interface axis port=feedback_in_p0
#pragma HLS interface axis port=feedback_in_p1
#pragma HLS interface axis port=feedback_in_p2
#pragma HLS interface axis port=hidden_to_c0
#pragma HLS interface s_axilite port=seed_hidden_in bundle=control
#pragma HLS interface s_axilite port=prompt_base bundle=control
#pragma HLS interface s_axilite port=num_prompts bundle=control
#pragma HLS interface s_axilite port=num_layers bundle=control
#pragma HLS interface s_axilite port=return bundle=control

  const int active_num_prompts =
      ((int)num_prompts > ROUTER_NUM_PROMPTS) ? ROUTER_NUM_PROMPTS : (int)num_prompts;
  const int active_num_layers =
      ((int)num_layers > ROUTER_NUM_LAYERS) ? ROUTER_NUM_LAYERS : (int)num_layers;

  for (int layer = 0; layer < ROUTER_NUM_LAYERS; ++layer) {
  #pragma HLS loop_tripcount min=1 max=24
    if (layer >= active_num_layers)
      break;
    for (int prompt = 0; prompt < ROUTER_NUM_PROMPTS; ++prompt) {
    #pragma HLS loop_tripcount min=1 max=3
      if (prompt >= active_num_prompts)
        break;
      const int prompt_id = (int)prompt_base + prompt;
      const int seed_base_word = prompt_id * HIDDEN_BEATS;
      if (layer == 0) {
        for (int beat = 0; beat < HIDDEN_BEATS; ++beat) {
        #pragma HLS pipeline II=1
          hidden_to_c0.write(seed_hidden_in[seed_base_word + beat]);
        }
      } else {
        for (int beat = 0; beat < HIDDEN_BEATS; ++beat) {
        #pragma HLS pipeline II=1
          if (prompt_id == 0)
            hidden_to_c0.write(feedback_in_p0.read());
          else if (prompt_id == 1)
            hidden_to_c0.write(feedback_in_p1.read());
          else
            hidden_to_c0.write(feedback_in_p2.read());
        }
      }
    }
  }
}
