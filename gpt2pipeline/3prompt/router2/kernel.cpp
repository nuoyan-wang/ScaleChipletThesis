#include "../stream_abi_common.h"

static const int HIDDEN_BEATS = 1024 / 32;
static const int ROUTER_NUM_PROMPTS = 3;
static const int ROUTER_NUM_LAYERS = 24;

extern "C" void c2_router(
    hls::stream<axis256_t> &hidden_from_c2,
    hls::stream<axis256_t> &feedback_to_c0_p0,
    hls::stream<axis256_t> &feedback_to_c0_p1,
    hls::stream<axis256_t> &feedback_to_c0_p2,
    packed_fxp32_t *final_hidden_out,
    ap_uint<2> prompt_base,
    ap_uint<2> num_prompts,
    ap_uint<8> num_layers) {
#pragma HLS interface axis port=hidden_from_c2
#pragma HLS interface axis port=feedback_to_c0_p0
#pragma HLS interface axis port=feedback_to_c0_p1
#pragma HLS interface axis port=feedback_to_c0_p2
#pragma HLS interface m_axi port=final_hidden_out offset=slave bundle=FINAL_HBM max_write_burst_length=32 num_write_outstanding=4
#pragma HLS interface s_axilite port=final_hidden_out bundle=control
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
      const int out_base_word = prompt_id * HIDDEN_BEATS;
      const bool write_final = (layer == active_num_layers - 1);
      if (write_final) {
        for (int beat = 0; beat < HIDDEN_BEATS; ++beat) {
        #pragma HLS pipeline II=1
          final_hidden_out[out_base_word + beat] = hidden_from_c2.read();
        }
      } else {
        for (int beat = 0; beat < HIDDEN_BEATS; ++beat) {
        #pragma HLS pipeline II=1
          axis256_t beat_data = hidden_from_c2.read();
          if (prompt_id == 0)
            feedback_to_c0_p0.write(beat_data);
          else if (prompt_id == 1)
            feedback_to_c0_p1.write(beat_data);
          else
            feedback_to_c0_p2.write(beat_data);
        }
      }
    }
  }
}
