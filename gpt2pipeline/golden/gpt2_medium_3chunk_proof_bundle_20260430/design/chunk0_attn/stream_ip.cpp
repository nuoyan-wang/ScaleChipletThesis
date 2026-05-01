#define kernel_0_chip0 kernel_0_chip0_core
#include "../stream_abi_common.h"

using packed_fxp64_t = ap_uint<512>;

// Chunk0 weight layout (per bank k, in packed_fxp64_t words):
//   [0 .. 6143]    : QKV weight bank k  (128 rg x 48 col)
//   [6144 .. 8191] : Wout weight bank k (128 rg x 16 col)
// Layer metadata lives on a separate 256-bit port:
//   [0 .. 95]      : QKV bias  (3072 fxp8 / 32 = 96 words)
//   [96 .. 127]    : Wout bias (1024 fxp8 / 32 = 32 words)
//   [128 .. 159]   : LN1 beta  (1024 fxp8 / 32 = 32 words)
//   [160 .. 191]   : LN1 gamma (1024 fxp8 / 32 = 32 words)
static const int C0_QKV_BANK  = 128 * 48;   // 6144
static const int C0_WOUT_OFF  = C0_QKV_BANK;
static const int C0_BIAS_OFF  = 0;
static const int C0_WOUT_BIAS = 96;
static const int C0_LN1_BETA  = 128;
static const int C0_LN1_GAMMA = 160;
static const int C0_META_WORDS = 192;
static const int C0_BANK_WORDS = 8192;
static const int C0_NUM_LAYERS = 24;
static const int C0_NUM_PROMPTS = 3;
static const int C0_KV_SLICE_WORDS = 16 * 128 * 2;
static const int C0_KV_HALF_SLICE_WORDS = 8 * 128 * 2;

static int c0_kv_slice_index(ap_uint<5> layer_id, ap_uint<2> prompt_id) {
  return static_cast<int>(layer_id) * C0_NUM_PROMPTS + static_cast<int>(prompt_id);
}

static int clamp_live_tokens_128(int cache_position) {
  return (cache_position < 0) ? 0 :
         ((cache_position >= 127) ? 128 : (cache_position + 1));
}

static void build_token_mask_128(ap_int<1> mask[128], int cache_position) {
  const int live_tokens = clamp_live_tokens_128(cache_position);
  for (int t = 0; t < 128; ++t) {
  #pragma HLS pipeline II=1
    mask[t] = (t < live_tokens) ? (ap_int<1>)1 : (ap_int<1>)0;
  }
}

static void load_kv_prefix_128x2(const packed_fxp32_t *src,
                                 packed_fxp32_t dst[1][16][128][2],
                                 ap_int<1> token_mask[128]) {
  for (int h = 0; h < 16; ++h) {
    const int head_off = h * 128 * 2;
    for (int t = 0; t < 128; ++t) {
      const int tok_off = head_off + t * 2;
      const bool token_live = token_mask[t] != 0;
      for (int d = 0; d < 2; ++d) {
      #pragma HLS pipeline II=1
        dst[0][h][t][d] = token_live ? src[tok_off + d] : (packed_fxp32_t)0;
      }
    }
  }
}

static void store_kv_token_128x2(packed_fxp32_t *dst,
                                 packed_fxp32_t src[1][16][128][2],
                                 int cache_position) {
  const bool valid_pos = (cache_position >= 0) && (cache_position < 128);
  for (int h = 0; h < 16; ++h) {
    const int tok_off = h * 128 * 2 + cache_position * 2;
    for (int d = 0; d < 2; ++d) {
    #pragma HLS pipeline II=1
      if (valid_pos)
        dst[tok_off + d] = src[0][h][cache_position][d];
    }
  }
}

static void zero_kvt_slice_128x4(packed_fxp32_t dst[1][16][64][4]) {
  for (int h = 0; h < 16; ++h) {
    for (int d = 0; d < 64; ++d) {
      for (int w = 0; w < 4; ++w) {
      #pragma HLS pipeline II=1
        dst[0][h][d][w] = 0;
      }
    }
  }
}

static void load_kv_prefix_half_128x2(const packed_fxp32_t *src,
                                      packed_fxp32_t dst[1][8][128][2],
                                      int live_tokens) {
  for (int local_h = 0; local_h < 8; ++local_h) {
    const int head_off = local_h * 128 * 2;
    for (int t = 0; t < 128; ++t) {
      const int tok_off = head_off + t * 2;
      for (int d = 0; d < 2; ++d) {
      #pragma HLS pipeline II=1
        dst[0][local_h][t][d] = (t < live_tokens) ? src[tok_off + d] : (packed_fxp32_t)0;
      }
    }
  }
}

static void store_kv_token_half_128x2(packed_fxp32_t *dst,
                                      packed_fxp32_t src[1][8][128][2],
                                      int cache_position) {
  const bool valid_pos = (cache_position >= 0) && (cache_position < 128);
  for (int local_h = 0; local_h < 8; ++local_h) {
    const int tok_off = local_h * 128 * 2 + cache_position * 2;
    for (int d = 0; d < 2; ++d) {
    #pragma HLS pipeline II=1
      if (valid_pos)
        dst[tok_off + d] = src[0][local_h][cache_position][d];
    }
  }
}

static void store_kv_token_split_128x2(packed_fxp32_t *dst_lo,
                                       packed_fxp32_t *dst_hi,
                                       packed_fxp32_t src_lo[1][8][128][2],
                                       packed_fxp32_t src_hi[1][8][128][2],
                                       int cache_position) {
  store_kv_token_half_128x2(dst_lo, src_lo, cache_position);
  store_kv_token_half_128x2(dst_hi, src_hi, cache_position);
}

static void load_chunk0_frame_state(
    hls::stream<axis256_t> &hidden_in,
    const packed_fxp32_t *kv_k_base_lo,
    const packed_fxp32_t *kv_k_base_hi,
    const packed_fxp32_t *kv_v_base_lo,
    const packed_fxp32_t *kv_v_base_hi,
    int cache_position,
    fxp8_t hidden_local[1][1][1024],
    fxp8_t attn_bias[1][1][1][128],
    packed_fxp32_t kv_k_local_lo[1][8][128][2],
    packed_fxp32_t kv_k_local_hi[1][8][128][2],
    packed_fxp32_t kv_v_local_lo[1][8][128][2],
    packed_fxp32_t kv_v_local_hi[1][8][128][2]) {
#pragma HLS dataflow
  const int live_tokens = clamp_live_tokens_128(cache_position);
  load_kv_prefix_half_128x2(kv_k_base_lo, kv_k_local_lo, live_tokens);
  load_kv_prefix_half_128x2(kv_k_base_hi, kv_k_local_hi, live_tokens);
  load_kv_prefix_half_128x2(kv_v_base_lo, kv_v_local_lo, live_tokens);
  load_kv_prefix_half_128x2(kv_v_base_hi, kv_v_local_hi, live_tokens);
  zero_attn_bias(attn_bias);
  load_axis_tensor_1x1xN<1024>(hidden_in, hidden_local);
}

static void store_chunk0_frame_state(
    packed_fxp32_t *kv_k_base_lo,
    packed_fxp32_t *kv_k_base_hi,
    packed_fxp32_t *kv_v_base_lo,
    packed_fxp32_t *kv_v_base_hi,
    int cache_position,
    packed_fxp32_t kv_k_local_lo[1][8][128][2],
    packed_fxp32_t kv_k_local_hi[1][8][128][2],
    packed_fxp32_t kv_v_local_lo[1][8][128][2],
    packed_fxp32_t kv_v_local_hi[1][8][128][2],
    fxp8_t hidden_mid_local[1][1][1024],
    fxp8_t bypass_local[1][1][1024],
    hls::stream<axis256_t> &hidden_mid_out,
    hls::stream<axis256_t> &bypass_out) {
#pragma HLS dataflow
  store_kv_token_split_128x2(kv_k_base_lo, kv_k_base_hi, kv_k_local_lo, kv_k_local_hi, cache_position);
  store_kv_token_split_128x2(kv_v_base_lo, kv_v_base_hi, kv_v_local_lo, kv_v_local_hi, cache_position);
  store_axis_tensor_1x1xN<1024>(hidden_mid_local, hidden_mid_out);
  store_axis_tensor_1x1xN<1024>(bypass_local, bypass_out);
}

//===------------------------------------------------------------*- C++ -*-===//
//
// Automatically generated file for High-level Synthesis (HLS).
//
//===----------------------------------------------------------------------===//

#include <algorithm>
#include <ap_axi_sdata.h>
#include <ap_fixed.h>
#include <ap_float.h>
#include <ap_int.h>
#include <hls_math.h>
#include <hls_stream.h>
#include <hls_vector.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

using namespace std;

#ifndef __SYNTHESIS__
#include <cstdio>
#include <cstdlib>
#include <cstring>
static int g_c0_debug_prompt = -1;
static int g_c0_debug_layer = -1;
static void c0_debug_set_slot(int layer, int prompt) {
  g_c0_debug_layer = layer;
  g_c0_debug_prompt = prompt;
}
static void c0_debug_dump(const char *label, const ap_fixed<8,4> *data, int n) {
  const char *dir = getenv("CHUNK0_DEBUG_DIR");
  if (!dir || !dir[0]) return;
  char path[1024];
  if (g_c0_debug_prompt >= 0 && g_c0_debug_layer >= 0)
    snprintf(path, sizeof(path), "%s/%s_l%02d_p%d.bin", dir, label, g_c0_debug_layer, g_c0_debug_prompt);
  else
    snprintf(path, sizeof(path), "%s/%s.bin", dir, label);
  FILE *f = fopen(path, "wb");
  if (!f) return;
  for (int i = 0; i < n; ++i) {
    uint8_t b = (uint8_t)data[i].range(7, 0).to_uint();
    fwrite(&b, 1, 1, f);
  }
  fclose(f);
}
static void c0_debug_dump_fxp32(const char *label, const ap_fixed<32,8> *data, int n) {
  const char *dir = getenv("CHUNK0_DEBUG_DIR");
  if (!dir || !dir[0]) return;
  char path[1024];
  if (g_c0_debug_prompt >= 0 && g_c0_debug_layer >= 0)
    snprintf(path, sizeof(path), "%s/%s_l%02d_p%d.bin", dir, label, g_c0_debug_layer, g_c0_debug_prompt);
  else
    snprintf(path, sizeof(path), "%s/%s.bin", dir, label);
  FILE *f = fopen(path, "wb");
  if (!f) return;
  for (int i = 0; i < n; ++i) {
    ap_uint<32> bits = data[i].range(31, 0);
    for (int b = 0; b < 4; ++b) {
      uint8_t v = (uint8_t)bits.range(b * 8 + 7, b * 8).to_uint();
      fwrite(&v, 1, 1, f);
    }
  }
  fclose(f);
}
static void c0_debug_dump_packed512(const char *label, const ap_uint<512> *data, int nwords) {
  const char *dir = getenv("CHUNK0_DEBUG_DIR");
  if (!dir || !dir[0]) return;
  char path[1024];
  if (g_c0_debug_prompt >= 0 && g_c0_debug_layer >= 0)
    snprintf(path, sizeof(path), "%s/%s_l%02d_p%d.bin", dir, label, g_c0_debug_layer, g_c0_debug_prompt);
  else
    snprintf(path, sizeof(path), "%s/%s.bin", dir, label);
  FILE *f = fopen(path, "wb");
  if (!f) return;
  for (int i = 0; i < nwords; ++i) {
    for (int b = 0; b < 64; ++b) {
      uint8_t v = (uint8_t)data[i].range(b * 8 + 7, b * 8).to_uint();
      fwrite(&v, 1, 1, f);
    }
  }
  fclose(f);
}
#else
static void c0_debug_set_slot(int, int) {}
static void c0_debug_dump(const char*, const ap_fixed<8,4>*, int) {}
static void c0_debug_dump_fxp32(const char*, const ap_fixed<32,8>*, int) {}
static void c0_debug_dump_packed512(const char*, const ap_uint<512>*, int) {}
#endif

static ap_fixed<8, 4> bits_to_fxp8_local(ap_uint<8> bits) {
#pragma HLS inline
  ap_fixed<8, 4> value;
  value.range(7, 0) = bits;
  return value;
}

void transpose_engine_64_64(hls::stream<ap_uint<4096>> &in_stream,
                            hls::stream<ap_uint<4096>> &out_stream) {
  static constexpr int NUM_BLOCKS = 32;
  ap_uint<64> mem[64][16];

#pragma HLS ARRAY_PARTITION variable=mem complete dim=1
#pragma HLS BIND_STORAGE variable=mem type=ram_s2p impl=bram

  for (int t = 0; t < (NUM_BLOCKS + 1) * 8; t++) {
#pragma HLS PIPELINE II = 1
    int b = t >> 3;
    int cyc = t & 7;
    int wr_offset = (b & 1) << 3;
    int rd_offset = ((b - 1) & 1) << 3;

    if (b < NUM_BLOCKS) {
      ap_uint<4096> in_chunk = in_stream.read();
      ap_uint<64> in_words[8][8];
#pragma HLS ARRAY_PARTITION variable=in_words complete dim=0
      for (int r = 0; r < 8; r++) {
#pragma HLS UNROLL
        for (int w = 0; w < 8; w++) {
#pragma HLS UNROLL
          in_words[r][w] = in_chunk(r * 512 + w * 64 + 63, r * 512 + w * 64);
        }
      }
      for (int g = 0; g < 8; g++) {
#pragma HLS UNROLL
        for (int r = 0; r < 8; r++) {
#pragma HLS UNROLL
          int bank = g * 8 + r;
          int w = (g + 8 - cyc) & 7;
          mem[bank][wr_offset + cyc] = in_words[r][w];
        }
      }
    }

    if (b > 0) {
      ap_uint<64> bram_read[8][8];
#pragma HLS ARRAY_PARTITION variable=bram_read complete dim=0
      for (int g = 0; g < 8; g++) {
#pragma HLS UNROLL
        for (int r_in = 0; r_in < 8; r_in++) {
#pragma HLS UNROLL
          int bank = g * 8 + r_in;
          int i_in = (g + 8 - cyc) & 7;
          bram_read[r_in][g] = mem[bank][rd_offset + i_in];
        }
      }

      ap_uint<4096> out_chunk = 0;
      for (int tr = 0; tr < 8; tr++) {
#pragma HLS UNROLL
        for (int bc = 0; bc < 8; bc++) {
#pragma HLS UNROLL
          ap_uint<64> out_word = 0;
          for (int el = 0; el < 8; el++) {
#pragma HLS UNROLL
            int r_in = el;
            int g = (bc + cyc) & 7;
            ap_uint<64> packed_word = bram_read[r_in][g];
            out_word(el * 8 + 7, el * 8) = packed_word(tr * 8 + 7, tr * 8);
          }
          out_chunk(tr * 512 + bc * 64 + 63, tr * 512 + bc * 64) = out_word;
        }
      }
      out_stream.write(out_chunk);
    }
  }
}

void shared_kernel_5(ap_fixed<32, 8> arg0000[1][32], ap_fixed<8, 4> arg0001[1][32]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=2 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)32; v0000 += (int)1) {
  #pragma HLS unroll factor=32
    ap_fixed<32, 8> v0001 = arg0000[(int)0][v0000];
    ap_fixed<8, 4> v0002 = (ap_fixed<8, 4>)v0001;
    arg0001[(int)0][v0000] = v0002;
  }
  return;
}

void shared_kernel_4(ap_int<1> arg0000[2], ap_fixed<8, 4> arg0001[1][2][32], ap_fixed<8, 4> arg0002[1][2][32], ap_fixed<8, 4> arg0003[1][2][32]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=2
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=2
#pragma HLS array_partition variable=arg0001 dim=3 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=2 cyclic factor=2
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=32
#pragma HLS array_partition variable=arg0003 dim=2 cyclic factor=2
#pragma HLS array_partition variable=arg0003 dim=3 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)2; v0000 += (int)1) {
  #pragma HLS unroll factor=2
    for (int v0001 = (int)0; v0001 < (int)32; v0001 += (int)1) {
    #pragma HLS unroll factor=32
      ap_int<1> v0002 = arg0000[v0000];
      ap_fixed<8, 4> v0003 = arg0001[(int)0][v0000][v0001];
      ap_fixed<8, 4> v0004 = arg0002[(int)0][v0000][v0001];
      ap_fixed<8, 4> v0005 = v0002 ? v0003 : v0004;
      arg0003[(int)0][v0000][v0001] = v0005;
    }
  }
  return;
}

void shared_kernel_4_attn(ap_fixed<8, 4> arg0000[1][1][8],
                          ap_fixed<8, 4> arg0001[1][8][64],
                          ap_fixed<8, 4> arg0002[1][1][64]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=8
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=8
#pragma HLS array_reshape variable=arg0001 dim=3 cyclic factor=64
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=64
  ap_fixed<8, 4> lanes[4];
#pragma HLS array_partition variable=lanes complete
  for (int col = 0; col < 64; ++col) {
#pragma HLS unroll factor=64
    for (int lane = 0; lane < 4; ++lane) {
#pragma HLS unroll
      lanes[lane] = (ap_fixed<8, 4>)0.0;
    }
    for (int k = 0; k < 8; ++k) {
#pragma HLS unroll factor=8
      ap_fixed<8, 4> prod = arg0000[0][0][k] * arg0001[0][k][col];
#pragma HLS BIND_OP variable=prod op=mul impl=dsp
      int lane = k & 3;
      lanes[lane] = lanes[lane] + prod;
    }
    arg0002[0][0][col] =
        arg0002[0][0][col] + (lanes[0] + lanes[1]) + (lanes[2] + lanes[3]);
  }
}

void shared_kernel_3(ap_fixed<32, 8> arg0000[32], ap_fixed<8, 4> arg0001[32]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)32; v0000 += (int)1) {
  #pragma HLS unroll factor=32
    ap_fixed<32, 8> v0001 = arg0000[v0000];
    ap_fixed<8, 4> v0002 = (ap_fixed<8, 4>)v0001;
    arg0001[v0000] = v0002;
  }
  return;
}

void shared_kernel_2(ap_fixed<8, 4> arg0000[1][1][8], ap_fixed<8, 4> arg0001[1][8][32], ap_fixed<32, 8> arg0002[1][1][32]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=8
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=8
#pragma HLS array_partition variable=arg0001 dim=3 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=32
  ap_fixed<32, 8> v0000[4];
  #pragma HLS array_partition variable=v0000 dim=1 complete
  for (int v0001 = 0; v0001 < 32; ++v0001) {
  #pragma HLS unroll factor=32
    for (int v0002 = 0; v0002 < 4; ++v0002) {
    #pragma HLS unroll factor=4
      v0000[v0002] = (ap_fixed<32, 8>)0.000000;
    }
    for (int v0003 = 0; v0003 < 8; ++v0003) {
    #pragma HLS unroll factor=8
      ap_fixed<8, 4> v0004 = arg0000[0][0][v0003];
      ap_fixed<8, 4> v0005 = arg0001[0][v0003][v0001];
      int lane_grp = v0003 & 3;
      ap_fixed<16, 8> v0013_prod = v0004 * v0005;
      #pragma HLS BIND_OP variable=v0013_prod op=mul impl=dsp
      v0000[lane_grp] = v0000[lane_grp] + (ap_fixed<32, 8>)v0013_prod;
    }
    ap_fixed<32, 8> v0019 = arg0002[0][0][v0001];
    ap_fixed<32, 8> v0022 = (v0000[0] + v0000[1]) + (v0000[2] + v0000[3]);
    arg0002[0][0][v0001] = v0022 + v0019;
  }
  return;
}

void shared_kernel_1(ap_fixed<8, 4> arg0000[32], ap_fixed<8, 4> arg0001[32], ap_fixed<8, 4> arg0002[32]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=1 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)32; v0000 += (int)1) {
  #pragma HLS unroll factor=32
    ap_fixed<8, 4> v0001 = arg0000[v0000];
    ap_fixed<8, 4> v0002 = arg0001[v0000];
    ap_fixed<8, 4> v0003 = v0001 + v0002;
    arg0002[v0000] = v0003;
  }
  return;
}

void shared_kernel_reduce64(ap_fixed<8, 4> arg0000[1][1][64], ap_fixed<8, 4> arg0001[1][1]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=64
  ap_fixed<8, 4> v0000[4];
  #pragma HLS array_partition variable=v0000 dim=1 cyclic factor=4
  for (int arg0002 = (int)0; arg0002 < (int)4; arg0002 += (int)1) {
  #pragma HLS unroll factor=4
    v0000[arg0002] = (ap_fixed<8, 4>)0.000000;
  }
  for (int arg0003 = (int)0; arg0003 < (int)64; arg0003 += (int)1) {
  #pragma HLS unroll factor=64
    ap_fixed<8, 4> v0001 = arg0000[(int)0][(int)0][arg0003];
    int v0002 = arg0003 % (int)4;
    ap_int<1> v0003 = v0002 < (int)0;
    int v0004 = v0002 + (int)4;
    int v0005 = v0003 ? v0004 : v0002;
    ap_fixed<8, 4> v0006 = v0000[v0005];
    ap_fixed<8, 4> v0007 = v0001 + v0006;
    int v0008 = arg0003 % (int)4;
    ap_int<1> v0009 = v0008 < (int)0;
    int v0010 = v0008 + (int)4;
    int v0011 = v0009 ? v0010 : v0008;
    v0000[v0011] = v0007;
  }
  ap_fixed<8, 4> v0012 = arg0001[(int)0][(int)0];
  ap_fixed<8, 4> v0013 = v0000[(int)0];
  ap_fixed<8, 4> v0014 = v0000[(int)1];
  ap_fixed<8, 4> v0015 = v0000[(int)2];
  ap_fixed<8, 4> v0016 = v0000[(int)3];
  ap_fixed<8, 4> v0017 = v0013 + v0014;
  ap_fixed<8, 4> v0018 = v0015 + v0016;
  ap_fixed<8, 4> v0019 = v0017 + v0018;
  ap_fixed<8, 4> v0020 = v0019 + v0012;
  arg0001[(int)0][(int)0] = v0020;
  return;
}

void shared_kernel_mul64(ap_fixed<8, 4> arg0000[64], ap_fixed<8, 4> arg0001[64], ap_fixed<8, 4> arg0002[64]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=64
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=64
#pragma HLS array_partition variable=arg0002 dim=1 cyclic factor=64
  for (int arg0003 = (int)0; arg0003 < (int)64; arg0003 += (int)1) {
  #pragma HLS unroll factor=64
    ap_fixed<8, 4> v0000 = arg0000[arg0003];
    ap_fixed<8, 4> v0001 = arg0001[arg0003];
    ap_fixed<8, 4> v0002 = v0000 * v0001;
    #pragma HLS BIND_OP variable=v0002 op=mul impl=dsp
    arg0002[arg0003] = v0002;
  }
  return;
}

void shared_kernel_add64(ap_fixed<8, 4> arg0000[64], ap_fixed<8, 4> arg0001[64], ap_fixed<8, 4> arg0002[64]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=64
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=64
#pragma HLS array_partition variable=arg0002 dim=1 cyclic factor=64
  for (int arg0003 = (int)0; arg0003 < (int)64; arg0003 += (int)1) {
  #pragma HLS unroll factor=64
    ap_fixed<8, 4> v0000 = arg0000[arg0003];
    ap_fixed<8, 4> v0001 = arg0001[arg0003];
    ap_fixed<8, 4> v0002 = v0000 + v0001;
    arg0002[arg0003] = v0002;
  }
  return;
}

void shared_kernel_0(ap_fixed<8, 4> arg0000[32], ap_fixed<8, 4> arg0001[32], ap_fixed<8, 4> arg0002[32]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=1 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)32; v0000 += (int)1) {
  #pragma HLS unroll factor=32
    ap_fixed<8, 4> v0001 = arg0000[v0000];
    ap_fixed<8, 4> v0002 = arg0001[v0000];
    ap_fixed<8, 4> v0003 = v0001 * v0002;
    arg0002[v0000] = v0003;
  }
  return;
}

void dataflow_node_0(ap_fixed<32, 8> arg0000[1][1]) {
#pragma HLS inline off
  arg0000[(int)0][(int)0] = (ap_fixed<32, 8>)0.000000;
  return;
}

void dataflow_node_1(ap_fixed<8, 4> arg0000[1][1][1024], ap_fixed<32, 8> arg0001[1][1]) {
#pragma HLS inline off
  ap_fixed<32, 8> v0000[4];
  for (int v0001 = (int)0; v0001 < (int)4; v0001 += (int)1) {
    v0000[v0001] = (ap_fixed<32, 8>)0.000000;
  }
  for (int v0002 = (int)0; v0002 < (int)1024; v0002 += (int)1) {
    ap_fixed<8, 4> v0003 = arg0000[(int)0][(int)0][v0002];
    int v0004 = v0002 % (int)4;
    ap_int<1> v0005 = v0004 < (int)0;
    int v0006 = v0004 + (int)4;
    int v0007 = v0005 ? v0006 : v0004;
    ap_fixed<32, 8> v0008 = v0000[v0007];
    ap_fixed<32, 8> v0009 = (ap_fixed<32, 8>)v0003;
    ap_fixed<32, 8> v0010 = v0009 + v0008;
    int v0011 = v0002 % (int)4;
    ap_int<1> v0012 = v0011 < (int)0;
    int v0013 = v0011 + (int)4;
    int v0014 = v0012 ? v0013 : v0011;
    v0000[v0014] = v0010;
  }
  ap_fixed<32, 8> v0015 = arg0001[(int)0][(int)0];
  ap_fixed<32, 8> v0016 = v0000[(int)0];
  ap_fixed<32, 8> v0017 = v0000[(int)1];
  ap_fixed<32, 8> v0018 = v0016 + v0017;
  ap_fixed<32, 8> v0019 = v0018 + v0015;
  arg0001[(int)0][(int)0] = v0019;
  return;
}

void dataflow_node_2(ap_fixed<32, 8> arg0000[1][1], ap_fixed<8, 4> arg0001[1]) {
#pragma HLS inline off
  ap_fixed<32, 8> v0000 = arg0000[(int)0][(int)0];
  ap_fixed<8, 4> v0001 = (ap_fixed<8, 4>)v0000;
  arg0001[0] = v0001;
  return;
}

void dataflow_node_3(ap_fixed<8, 4> arg0000[1], ap_fixed<8, 4> arg0001[1]) {
#pragma HLS inline off
  ap_fixed<8, 4> v0000 = arg0000[0];
  ap_fixed<8, 4> v0001 = v0000 * (ap_fixed<8, 4>)0.000000;
  arg0001[0] = v0001;
  return;
}

void dataflow_node_4(ap_fixed<8, 4> arg0000[1], ap_fixed<8, 4> arg0001[1024]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
  ap_fixed<8, 4> v0000 = arg0000[0];
  for (int v0001 = (int)0; v0001 < (int)1024; v0001 += (int)32) {
  #pragma HLS pipeline II=1
    for (int v0002 = (int)0; v0002 < (int)32; v0002 += (int)1) {
    #pragma HLS unroll factor=32
      int v0003 = v0001 + v0002;
      arg0001[v0003] = v0000;
    }
  }
  return;
}

void dataflow_node_5(ap_fixed<8, 4> arg0000[1][1][1024], ap_fixed<8, 4> arg0001[1024], ap_fixed<8, 4> arg0002[1024]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=1 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)32) {
  #pragma HLS pipeline II=1
    for (int v0001 = (int)0; v0001 < (int)32; v0001 += (int)1) {
    #pragma HLS unroll factor=32
      int v0002 = v0000 + v0001;
      arg0002[v0002] = arg0000[(int)0][(int)0][v0002] - arg0001[v0002];
    }
  }
  return;
}

void dataflow_node_6(ap_fixed<8, 4> arg0000[1024], ap_fixed<8, 4> arg0001[1024], ap_fixed<8, 4> arg0002[1024]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=1 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)32) {
  #pragma HLS pipeline II=1
    ap_fixed<8, 4> v0001[32];
    #pragma HLS array_partition variable=v0001 dim=1 cyclic factor=32
    for (int v0002 = (int)0; v0002 < (int)32; v0002 += (int)1) {
    #pragma HLS unroll factor=32
      int v0003 = v0000 + v0002;
      ap_fixed<8, 4> v0004 = arg0001[v0003];
      v0001[v0002] = v0004;
    }
    ap_fixed<8, 4> v0005[32];
    #pragma HLS array_partition variable=v0005 dim=1 cyclic factor=32
    for (int v0006 = (int)0; v0006 < (int)32; v0006 += (int)1) {
    #pragma HLS unroll factor=32
      int v0007 = v0000 + v0006;
      ap_fixed<8, 4> v0008 = arg0001[v0007];
      v0005[v0006] = v0008;
    }
    ap_fixed<8, 4> v0009[32];
    #pragma HLS array_partition variable=v0009 dim=1 cyclic factor=32
    for (int v0010 = (int)0; v0010 < (int)32; v0010 += (int)1) {
    #pragma HLS unroll factor=32
      int v0011 = v0000 + v0010;
      ap_fixed<8, 4> v0012 = arg0002[v0011];
      v0009[v0010] = v0012;
    }
    shared_kernel_0(v0001, v0005, v0009);
    for (int v0013 = (int)0; v0013 < (int)32; v0013 += (int)1) {
    #pragma HLS unroll factor=32
      ap_fixed<8, 4> v0014 = v0009[v0013];
      int v0015 = v0000 + v0013;
      arg0002[v0015] = v0014;
    }
  }
  return;
}

void dataflow_node_7(ap_fixed<8, 4> arg0000[1][1]) {
#pragma HLS inline off
  arg0000[(int)0][(int)0] = (ap_fixed<8, 4>)0.000000;
  return;
}

void dataflow_node_8(ap_fixed<8, 4> arg0000[1024], ap_fixed<8, 4> arg0001[1][1]) {
#pragma HLS inline off
  #pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=64
  for (int arg0002 = (int)0; arg0002 < (int)1024; arg0002 += (int)64) {
  #pragma HLS pipeline II=1
    ap_fixed<8, 4> v0000[1][1][64];
    #pragma HLS array_partition variable=v0000 dim=3 cyclic factor=64
    for (int arg0003 = (int)0; arg0003 < (int)64; arg0003 += (int)1) {
    #pragma HLS unroll factor=64
      int v0001 = arg0002 + arg0003;
      ap_fixed<8, 4> v0002 = arg0000[v0001];
      v0000[(int)0][(int)0][arg0003] = v0002;
    }
    ap_fixed<8, 4> v0003[1][1];
    ap_fixed<8, 4> v0004 = arg0001[(int)0][(int)0];
    v0003[(int)0][(int)0] = v0004;
    shared_kernel_reduce64(v0000, v0003);
    ap_fixed<8, 4> v0005 = v0003[(int)0][(int)0];
    arg0001[(int)0][(int)0] = v0005;
  }
  return;
}

void dataflow_node_9(ap_fixed<8, 4> arg0000[1][1], ap_fixed<8, 4> arg0001[1]) {
#pragma HLS inline off
  ap_fixed<8, 4> v0000 = arg0000[(int)0][(int)0];
  arg0001[0] = v0000;
  return;
}

void dataflow_node_10(ap_fixed<8, 4> arg0000[1], ap_fixed<8, 4> arg0001[1]) {
#pragma HLS inline off
  ap_fixed<8, 4> v0000 = arg0000[0];
  ap_fixed<8, 4> v0001 = v0000 * (ap_fixed<8, 4>)0.000000;
  arg0001[0] = v0001;
  return;
}

void dataflow_node_11(ap_fixed<8, 4> arg0000[1], ap_fixed<8, 4> arg0001[1]) {
#pragma HLS inline off
  ap_fixed<8, 4> v0000 = arg0000[0];
  ap_fixed<8, 4> v0001 = v0000 + (ap_fixed<8, 4>)0.000000;
  arg0001[0] = v0001;
  return;
}

void dataflow_node_12(ap_fixed<8, 4> arg0000[1], ap_fixed<8, 4> arg0001[1]) {
#pragma HLS inline off
  ap_fixed<8, 4> v0000 = arg0000[0];
  ap_fixed<8, 4> v0001 = hls::rsqrt(v0000);
  arg0001[0] = v0001;
  return;
}

void dataflow_node_13(ap_fixed<8, 4> arg0000[1], ap_fixed<8, 4> arg0001[1024]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=64
  ap_fixed<8, 4> v0000 = arg0000[0];
  for (int v0001 = (int)0; v0001 < (int)1024; v0001 += (int)1) {
  #pragma HLS unroll factor=64
    arg0001[v0001] = v0000;
  }
  return;
}

void dataflow_node_14(ap_fixed<8, 4> arg0000[1024], ap_fixed<8, 4> arg0001[1024], ap_fixed<8, 4> arg0002[1024]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=64
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=64
#pragma HLS array_partition variable=arg0002 dim=1 cyclic factor=64
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)64) {
  #pragma HLS pipeline II=1
    ap_fixed<8, 4> v0001[64];
    #pragma HLS array_partition variable=v0001 dim=1 cyclic factor=64
    for (int v0002 = (int)0; v0002 < (int)64; v0002 += (int)1) {
    #pragma HLS unroll factor=64
      int v0003 = v0000 + v0002;
      ap_fixed<8, 4> v0004 = arg0000[v0003];
      v0001[v0002] = v0004;
    }
    ap_fixed<8, 4> v0005[64];
    #pragma HLS array_partition variable=v0005 dim=1 cyclic factor=64
    for (int v0006 = (int)0; v0006 < (int)64; v0006 += (int)1) {
    #pragma HLS unroll factor=64
      int v0007 = v0000 + v0006;
      ap_fixed<8, 4> v0008 = arg0001[v0007];
      v0005[v0006] = v0008;
    }
    ap_fixed<8, 4> v0009[64];
    #pragma HLS array_partition variable=v0009 dim=1 cyclic factor=64
    for (int v0010 = (int)0; v0010 < (int)64; v0010 += (int)1) {
    #pragma HLS unroll factor=64
      int v0011 = v0000 + v0010;
      ap_fixed<8, 4> v0012 = arg0002[v0011];
      v0009[v0010] = v0012;
    }
    shared_kernel_mul64(v0001, v0005, v0009);
    for (int v0013 = (int)0; v0013 < (int)64; v0013 += (int)1) {
    #pragma HLS unroll factor=64
      ap_fixed<8, 4> v0014 = v0009[v0013];
      int v0015 = v0000 + v0013;
      arg0002[v0015] = v0014;
    }
  }
  return;
}

void dataflow_node_15(ap_fixed<8, 4> arg0000[1024], ap_fixed<8, 4> arg0001[1024], ap_fixed<8, 4> arg0002[1024]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=64
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=64
#pragma HLS array_partition variable=arg0002 dim=1 cyclic factor=64
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)64) {
  #pragma HLS pipeline II=1
    ap_fixed<8, 4> v0001[64];
    #pragma HLS array_partition variable=v0001 dim=1 cyclic factor=64
    for (int v0002 = (int)0; v0002 < (int)64; v0002 += (int)1) {
    #pragma HLS unroll factor=64
      int v0003 = v0000 + v0002;
      ap_fixed<8, 4> v0004 = arg0000[v0003];
      v0001[v0002] = v0004;
    }
    ap_fixed<8, 4> v0005[64];
    #pragma HLS array_partition variable=v0005 dim=1 cyclic factor=64
    for (int v0006 = (int)0; v0006 < (int)64; v0006 += (int)1) {
    #pragma HLS unroll factor=64
      int v0007 = v0000 + v0006;
      ap_fixed<8, 4> v0008 = arg0001[v0007];
      v0005[v0006] = v0008;
    }
    ap_fixed<8, 4> v0009[64];
    #pragma HLS array_partition variable=v0009 dim=1 cyclic factor=64
    for (int v0010 = (int)0; v0010 < (int)64; v0010 += (int)1) {
    #pragma HLS unroll factor=64
      int v0011 = v0000 + v0010;
      ap_fixed<8, 4> v0012 = arg0002[v0011];
      v0009[v0010] = v0012;
    }
    shared_kernel_mul64(v0001, v0005, v0009);
    for (int v0013 = (int)0; v0013 < (int)64; v0013 += (int)1) {
    #pragma HLS unroll factor=64
      ap_fixed<8, 4> v0014 = v0009[v0013];
      int v0015 = v0000 + v0013;
      arg0002[v0015] = v0014;
    }
  }
  return;
}

void dataflow_node_16(ap_fixed<8, 4> arg0000[1024], ap_fixed<8, 4> arg0001[1024], ap_fixed<8, 4> arg0002[1024]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=64
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=64
#pragma HLS array_partition variable=arg0002 dim=1 cyclic factor=64
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)64) {
  #pragma HLS pipeline II=1
    ap_fixed<8, 4> v0001[64];
    #pragma HLS array_partition variable=v0001 dim=1 cyclic factor=64
    for (int v0002 = (int)0; v0002 < (int)64; v0002 += (int)1) {
    #pragma HLS unroll factor=64
      int v0003 = v0000 + v0002;
      ap_fixed<8, 4> v0004 = arg0000[v0003];
      v0001[v0002] = v0004;
    }
    ap_fixed<8, 4> v0005[64];
    #pragma HLS array_partition variable=v0005 dim=1 cyclic factor=64
    for (int v0006 = (int)0; v0006 < (int)64; v0006 += (int)1) {
    #pragma HLS unroll factor=64
      int v0007 = v0000 + v0006;
      ap_fixed<8, 4> v0008 = arg0001[v0007];
      v0005[v0006] = v0008;
    }
    ap_fixed<8, 4> v0009[64];
    #pragma HLS array_partition variable=v0009 dim=1 cyclic factor=64
    for (int v0010 = (int)0; v0010 < (int)64; v0010 += (int)1) {
    #pragma HLS unroll factor=64
      int v0011 = v0000 + v0010;
      ap_fixed<8, 4> v0012 = arg0002[v0011];
      v0009[v0010] = v0012;
    }
    shared_kernel_add64(v0001, v0005, v0009);
    for (int v0013 = (int)0; v0013 < (int)64; v0013 += (int)1) {
    #pragma HLS unroll factor=64
      ap_fixed<8, 4> v0014 = v0009[v0013];
      int v0015 = v0000 + v0013;
      arg0002[v0015] = v0014;
    }
  }
  return;
}

void dataflow_node_17(ap_fixed<8, 4> arg0000[1][1][3072]) {
#pragma HLS inline off
  for (int v0000 = 0; v0000 < 3072; v0000 += 32) {
  #pragma HLS pipeline II=1
    for (int v0001 = 0; v0001 < 32; ++v0001) {
    #pragma HLS unroll factor=32
      arg0000[0][0][v0000 + v0001] = (ap_fixed<8, 4>)0.0;
    }
  }
  return;
}

static void shared_kernel_3_local_qkv(
    ap_fixed<8, 4> arg0000[1][1][16],
    ap_fixed<8, 4> arg0001[1][16][32],
    ap_fixed<32, 8> arg0002[1][1][32]);

static void shared_kernel_3_local(
    ap_fixed<8, 4> arg0000[1][1][8],
    ap_fixed<8, 4> arg0001[1][8][32],
    ap_fixed<32, 8> arg0002[1][1][32]);

static void shared_kernel_2_local_32fx8(
    ap_fixed<8, 4> arg0000[1][1][8],
    ap_fixed<8, 4> arg0001[1][8][32],
    ap_fixed<8, 4> arg0002[1][1][32]);

static void shared_kernel_3_local_32fx8(
    ap_fixed<8, 4> arg0000[1][1][8],
    ap_fixed<8, 4> arg0001[1][8][32],
    ap_fixed<8, 4> arg0002[1][1][32]);

static void shared_kernel_2_local_64(
    ap_fixed<8, 4> arg0000[1][1][8],
    ap_fixed<8, 4> arg0001[1][8][64],
    ap_fixed<8, 4> arg0002[1][1][64]);

static void shared_kernel_3_local_64(
    ap_fixed<8, 4> arg0000[1][1][8],
    ap_fixed<8, 4> arg0001[1][8][64],
    ap_fixed<8, 4> arg0002[1][1][64]);

// dataflow_node_18: QKV GEMM, 8-bank HBM streaming.
// Weight layout: qkv_bk[rg*48+col] stores one 64-lane output group
// per 512-bit beat. Each inner iteration updates one 64-lane QKV column group.
void dataflow_node_18(
    ap_fixed<8, 4> arg0000[1024],
    packed_fxp64_t *qkv_b0, packed_fxp64_t *qkv_b1,
    packed_fxp64_t *qkv_b2, packed_fxp64_t *qkv_b3,
    packed_fxp64_t *qkv_b4, packed_fxp64_t *qkv_b5,
    packed_fxp64_t *qkv_b6, packed_fxp64_t *qkv_b7,
    ap_fixed<8, 4> arg0002[1][1][3072]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=8
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=64
  for (int rg = 0; rg < 128; rg++) {
    for (int col = 0; col < 48; col++) {
    #pragma HLS pipeline II=1
      packed_fxp64_t w0 = qkv_b0[rg * 48 + col];
      packed_fxp64_t w1 = qkv_b1[rg * 48 + col];
      packed_fxp64_t w2 = qkv_b2[rg * 48 + col];
      packed_fxp64_t w3 = qkv_b3[rg * 48 + col];
      packed_fxp64_t w4 = qkv_b4[rg * 48 + col];
      packed_fxp64_t w5 = qkv_b5[rg * 48 + col];
      packed_fxp64_t w6 = qkv_b6[rg * 48 + col];
      packed_fxp64_t w7 = qkv_b7[rg * 48 + col];
      ap_fixed<8, 4> v0002[1][1][8];
      #pragma HLS array_partition variable=v0002 dim=3 complete
      for (int k = 0; k < 8; k++) {
      #pragma HLS unroll
        v0002[0][0][k] = arg0000[rg * 8 + k];
      }
      ap_fixed<8, 4> v0004[1][8][64];
      #pragma HLS array_partition variable=v0004 dim=2 complete
      #pragma HLS array_partition variable=v0004 dim=3 complete
      ap_fixed<8, 4> v0010[1][1][64];
      #pragma HLS array_partition variable=v0010 dim=3 complete
      packed_fxp64_t wbanks[8];
      #pragma HLS array_partition variable=wbanks complete
      wbanks[0] = w0; wbanks[1] = w1; wbanks[2] = w2; wbanks[3] = w3;
      wbanks[4] = w4; wbanks[5] = w5; wbanks[6] = w6; wbanks[7] = w7;
      for (int k = 0; k < 8; k++) {
      #pragma HLS unroll
        for (int j = 0; j < 64; j++) {
        #pragma HLS unroll
          v0004[0][k][j].range(7, 0) = wbanks[k].range(j * 8 + 7, j * 8);
        }
      }
      for (int j = 0; j < 64; ++j) {
      #pragma HLS unroll factor=64
        v0010[0][0][j] = arg0002[0][0][col * 64 + j];
      }
      shared_kernel_3_local_64(v0002, v0004, v0010);
      for (int j = 0; j < 64; ++j) {
      #pragma HLS unroll factor=64
        arg0002[0][0][col * 64 + j] = v0010[0][0][j];
      }
    }
  }
  return;
}

void dataflow_node_19(ap_fixed<8, 4> arg0000[1][1][3072], ap_fixed<8, 4> arg0001[3072]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)3072; v0000 += (int)32) {
  #pragma HLS pipeline II=1
    for (int v0002 = (int)0; v0002 < (int)32; v0002 += (int)1) {
    #pragma HLS unroll factor=32
      int v0003 = v0000 + v0002;
      arg0001[v0003] = arg0000[0][0][v0003];
    }
  }
  return;
}

void dataflow_node_20(ap_fixed<8, 4> arg0000[3072], ap_fixed<8, 4> arg0001[3072], ap_fixed<8, 4> arg0002[3072]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=1 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)3072; v0000 += (int)32) {
  #pragma HLS pipeline II=1
    ap_fixed<8, 4> v0001[32];
    #pragma HLS array_partition variable=v0001 dim=1 cyclic factor=32
    for (int v0002 = (int)0; v0002 < (int)32; v0002 += (int)1) {
    #pragma HLS unroll factor=32
      int v0003 = v0000 + v0002;
      ap_fixed<8, 4> v0004 = arg0000[v0003];
      v0001[v0002] = v0004;
    }
    ap_fixed<8, 4> v0005[32];
    #pragma HLS array_partition variable=v0005 dim=1 cyclic factor=32
    for (int v0006 = (int)0; v0006 < (int)32; v0006 += (int)1) {
    #pragma HLS unroll factor=32
      int v0007 = v0000 + v0006;
      ap_fixed<8, 4> v0008 = arg0001[v0007];
      v0005[v0006] = v0008;
    }
    ap_fixed<8, 4> v0009[32];
    #pragma HLS array_partition variable=v0009 dim=1 cyclic factor=32
    for (int v0010 = (int)0; v0010 < (int)32; v0010 += (int)1) {
    #pragma HLS unroll factor=32
      int v0011 = v0000 + v0010;
      ap_fixed<8, 4> v0012 = arg0002[v0011];
      v0009[v0010] = v0012;
    }
    shared_kernel_1(v0001, v0005, v0009);
    for (int v0013 = (int)0; v0013 < (int)32; v0013 += (int)1) {
    #pragma HLS unroll factor=32
      ap_fixed<8, 4> v0014 = v0009[v0013];
      int v0015 = v0000 + v0013;
      arg0002[v0015] = v0014;
    }
  }
  return;
}

void dataflow_node_21(ap_int<32> arg0000[1], ap_int<64> arg0001[1]) {
#pragma HLS inline off
  ap_int<32> v0000 = arg0000[(int)0];
  ap_int<64> v0001 = (ap_int<64>)v0000;
  arg0001[0] = v0001;
  return;
}

void dataflow_node_22(ap_int<64> arg0000[1], ap_int<1> arg0001[128]) {
#pragma HLS inline off
  ap_int<64> v0000[128] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127};
  for (int v0001 = (int)0; v0001 < (int)128; v0001 += (int)1) {
    ap_int<64> v0002 = arg0000[0];
    ap_int<64> v0003 = v0000[v0001];
    ap_int<1> v0004 = v0002 == v0003;
    arg0001[v0001] = v0004;
  }
  return;
}

void dataflow_node_23(ap_fixed<8, 4> arg0000[3072], ap_uint<512> arg0001[16][128][1]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=64
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=8
  for (int head = 0; head < 16; ++head) {
    for (int tok = 0; tok < 128; ++tok) {
#pragma HLS unroll factor=8
      ap_uint<512> packed = 0;
      int base = 1024 + head * 64;
      for (int lane = 0; lane < 64; ++lane) {
#pragma HLS unroll factor=64
        packed.range(lane * 8 + 7, lane * 8) = arg0000[base + lane].range(7, 0);
      }
      arg0001[head][tok][0] = packed;
    }
  }
}

void dataflow_node_24(ap_fixed<8, 4> arg0000[3072], ap_uint<512> arg0001[16][128][1]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=64
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=8
  for (int head = 0; head < 16; ++head) {
    for (int tok = 0; tok < 128; ++tok) {
#pragma HLS unroll factor=8
      ap_uint<512> packed = 0;
      int base = 2048 + head * 64;
      for (int lane = 0; lane < 64; ++lane) {
#pragma HLS unroll factor=64
        packed.range(lane * 8 + 7, lane * 8) = arg0000[base + lane].range(7, 0);
      }
      arg0001[head][tok][0] = packed;
    }
  }
}

static void dataflow_node_25_half(ap_int<1> arg0000[128], ap_uint<512> arg0001[16][128][1],
                                  ap_uint<256> arg0002[1][8][128][2],
                                  ap_fixed<8, 4> arg0003[16][128][64],
                                  int head_base) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=64
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=8
#pragma HLS array_partition variable=arg0003 dim=2 cyclic factor=8
#pragma HLS array_reshape variable=arg0003 dim=3 cyclic factor=64
  for (int local_h = 0; local_h < 8; ++local_h) {
    const int head = head_base + local_h;
    for (int tok = 0; tok < 128; ++tok) {
#pragma HLS pipeline II=1
      ap_uint<512> packed = arg0001[head][tok][0];
      ap_uint<256> lo = packed.range(255, 0);
      ap_uint<256> hi = packed.range(511, 256);
      if (arg0000[tok]) {
        arg0002[0][local_h][tok][0] = lo;
        arg0002[0][local_h][tok][1] = hi;
      } else {
        lo = arg0002[0][local_h][tok][0];
        hi = arg0002[0][local_h][tok][1];
      }
      for (int lane = 0; lane < 32; ++lane) {
#pragma HLS unroll factor=32
        arg0003[head][tok][lane] = bits_to_fxp8_local(lo.range(lane * 8 + 7, lane * 8));
        arg0003[head][tok][lane + 32] =
            bits_to_fxp8_local(hi.range(lane * 8 + 7, lane * 8));
      }
    }
  }
}

static void dataflow_node_26_half(ap_int<1> arg0000[128], ap_uint<512> arg0001[16][128][1],
                                  ap_uint<256> arg0002[1][8][128][2],
                                  ap_fixed<8, 4> arg0003[16][128][64],
                                  int head_base) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=64
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=8
#pragma HLS array_partition variable=arg0003 dim=2 cyclic factor=8
#pragma HLS array_reshape variable=arg0003 dim=3 cyclic factor=64
  for (int local_h = 0; local_h < 8; ++local_h) {
    const int head = head_base + local_h;
    for (int tok = 0; tok < 128; ++tok) {
#pragma HLS pipeline II=1
      ap_uint<512> packed = arg0001[head][tok][0];
      ap_uint<256> lo = packed.range(255, 0);
      ap_uint<256> hi = packed.range(511, 256);
      if (arg0000[tok]) {
        arg0002[0][local_h][tok][0] = lo;
        arg0002[0][local_h][tok][1] = hi;
      } else {
        lo = arg0002[0][local_h][tok][0];
        hi = arg0002[0][local_h][tok][1];
      }
      for (int lane = 0; lane < 32; ++lane) {
#pragma HLS unroll factor=32
        arg0003[head][tok][lane] = bits_to_fxp8_local(lo.range(lane * 8 + 7, lane * 8));
        arg0003[head][tok][lane + 32] =
            bits_to_fxp8_local(hi.range(lane * 8 + 7, lane * 8));
      }
    }
  }
}

void dataflow_node_27(ap_fixed<8, 4> arg0000[16][128][64], ap_uint<512> arg0001[1][16][64][2]) {
#pragma HLS inline off
#pragma HLS dataflow
#pragma HLS array_partition variable=arg0000 dim=2 cyclic factor=8
#pragma HLS array_reshape variable=arg0000 dim=3 cyclic factor=64
#pragma HLS array_partition variable=arg0001 dim=3 cyclic factor=8
  hls::stream<ap_uint<4096>> v0000;
  hls::stream<ap_uint<4096>> v0001;
  for (int head = 0; head < 16; ++head) {
    for (int tok_base = 0; tok_base < 128; tok_base += 64) {
      for (int row_base = 0; row_base < 64; row_base += 8) {
        ap_uint<4096> chunk = 0;
        for (int row = 0; row < 8; ++row) {
#pragma HLS unroll factor=8
          for (int col = 0; col < 64; ++col) {
#pragma HLS unroll factor=64
            int tok = tok_base + row_base + row;
            int bit = (row * 64 + col) * 8;
            chunk.range(bit + 7, bit) = arg0000[head][tok][col].range(7, 0);
          }
        }
        v0000.write(chunk);
      }
    }
  }
  transpose_engine_64_64(v0000, v0001);
  for (int head = 0; head < 16; ++head) {
    for (int tok_base = 0; tok_base < 128; tok_base += 64) {
      for (int row_base = 0; row_base < 64; row_base += 8) {
        ap_uint<4096> chunk = v0001.read();
        for (int row = 0; row < 8; ++row) {
#pragma HLS unroll factor=8
          ap_uint<512> packed = 0;
          for (int col = 0; col < 64; ++col) {
#pragma HLS unroll factor=64
            int idx = (row * 64 + col) * 8;
            packed.range(col * 8 + 7, col * 8) = chunk.range(idx + 7, idx);
          }
          int q = row_base + row;
          int blk = tok_base / 64;
          arg0001[0][head][q][blk] = packed;
        }
      }
    }
  }
}

void dataflow_node_28(ap_fixed<8, 4> arg0000[16][1][128]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=64
  for (int head = 0; head < 16; ++head) {
    for (int tok = 0; tok < 128; ++tok) {
#pragma HLS unroll factor=64
      arg0000[head][0][tok] = (ap_fixed<8, 4>)0.0;
    }
  }
}

void dataflow_node_29(ap_fixed<8, 4> arg0000[3072], ap_uint<512> arg0001[1][16][64][2],
                      ap_fixed<8, 4> arg0002[16][1][128]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=64
#pragma HLS array_partition variable=arg0001 dim=3 cyclic factor=8
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=64
  for (int head = 0; head < 16; ++head) {
    for (int q_base = 0; q_base < 64; q_base += 8) {
      for (int tok_base = 0; tok_base < 128; tok_base += 64) {
#pragma HLS pipeline II=1
        ap_fixed<8, 4> q[1][1][8];
#pragma HLS array_partition variable=q dim=3 cyclic factor=8
        for (int i = 0; i < 8; ++i) {
#pragma HLS unroll factor=8
          q[0][0][i] = arg0000[head * 64 + q_base + i];
        }
        ap_fixed<8, 4> kt[1][8][64];
#pragma HLS array_partition variable=kt dim=2 cyclic factor=8
#pragma HLS array_reshape variable=kt dim=3 cyclic factor=64
        int blk = tok_base / 64;
        for (int i = 0; i < 8; ++i) {
#pragma HLS unroll factor=8
          ap_uint<512> packed = arg0001[0][head][q_base + i][blk];
          for (int tok = 0; tok < 64; ++tok) {
#pragma HLS unroll factor=64
            kt[0][i][tok] = bits_to_fxp8_local(packed.range(tok * 8 + 7, tok * 8));
          }
        }
        ap_fixed<8, 4> acc[1][1][64];
#pragma HLS array_partition variable=acc dim=3 cyclic factor=64
        for (int tok = 0; tok < 64; ++tok) {
#pragma HLS unroll factor=64
          acc[0][0][tok] = arg0002[head][0][tok_base + tok];
        }
        shared_kernel_4_attn(q, kt, acc);
        for (int tok = 0; tok < 64; ++tok) {
#pragma HLS unroll factor=64
          arg0002[head][0][tok_base + tok] = acc[0][0][tok];
        }
      }
    }
  }
}

void dataflow_node_30(ap_fixed<8, 4> arg0000[16][1][128], ap_fixed<8, 4> arg0001[16][128]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=64
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=64
  for (int head = 0; head < 16; ++head) {
    for (int tok = 0; tok < 128; ++tok) {
#pragma HLS unroll factor=64
      arg0001[head][tok] = arg0000[head][0][tok];
    }
  }
}

void dataflow_node_31(ap_fixed<8, 4> arg0000[16][128], ap_fixed<8, 4> arg0001[16][128]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    for (int v0001 = (int)0; v0001 < (int)128; v0001 += (int)1) {
      ap_fixed<8, 4> v0002 = arg0000[v0000][v0001];
      ap_fixed<8, 4> v0003 = v0002 * (ap_fixed<8, 4>)0.125000;
      arg0001[v0000][v0001] = v0003;
    }
  }
  return;
}

void dataflow_node_32(ap_int<1> arg0000[1][1][1024][1024], ap_fixed<8, 4> arg0001[16][128], ap_fixed<8, 4> arg0002[16][128]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    for (int v0001 = (int)0; v0001 < (int)128; v0001 += (int)1) {
      ap_int<1> v0002 = arg0000[(int)0][(int)0][(int)127][v0001];
      ap_fixed<8, 4> v0003 = arg0001[v0000][v0001];
      ap_fixed<8, 4> v0004 = v0002 ? v0003 : (ap_fixed<8, 4>)-INFINITY;
      arg0002[v0000][v0001] = v0004;
    }
  }
  return;
}

void dataflow_node_33(ap_fixed<8, 4> arg0000[16][128], ap_fixed<8, 4> arg0001[1][1][1][128], ap_fixed<8, 4> arg0002[16][128]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    for (int v0001 = (int)0; v0001 < (int)128; v0001 += (int)1) {
      ap_fixed<8, 4> v0002 = arg0000[v0000][v0001];
      int v0003 = v0001 / (int)128;
      int v0004 = v0003 * (int)128;
      ap_int<1> v0005 = v0001 != v0004;
      ap_int<1> v0006 = v0001 < (int)0;
      ap_int<1> v0007 = v0005 & v0006;
      int v0008 = v0003 + (int)-1;
      int v0009 = v0007 ? v0008 : v0003;
      int v0010 = v0001 % (int)128;
      ap_int<1> v0011 = v0010 < (int)0;
      int v0012 = v0010 + (int)128;
      int v0013 = v0011 ? v0012 : v0010;
      int v0014 = v0013 / (int)128;
      int v0015 = v0001 % (int)128;
      ap_int<1> v0016 = v0015 < (int)0;
      int v0017 = v0015 + (int)128;
      int v0018 = v0016 ? v0017 : v0015;
      int v0019 = v0018 / (int)128;
      int v0020 = v0001 % (int)128;
      ap_int<1> v0021 = v0020 < (int)0;
      int v0022 = v0020 + (int)128;
      int v0023 = v0021 ? v0022 : v0020;
      ap_fixed<8, 4> v0024 = arg0001[v0009][v0014][v0019][v0023];
      ap_fixed<8, 4> v0025 = v0002 + v0024;
      arg0002[v0000][v0001] = v0025;
    }
  }
  return;
}

void dataflow_node_34(ap_fixed<8, 4> arg0000[16][128], ap_fixed<32, 8> arg0001[16][128]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    for (int v0001 = (int)0; v0001 < (int)128; v0001 += (int)1) {
      ap_fixed<8, 4> v0002 = arg0000[v0000][v0001];
      ap_fixed<32, 8> v0003 = (ap_fixed<32, 8>)v0002;
      arg0001[v0000][v0001] = v0003;
    }
  }
  return;
}

void dataflow_node_35(ap_fixed<32, 8> arg0000[1][16][1]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    arg0000[(int)0][v0000][(int)0] = (ap_fixed<32, 8>)-340282346638528859811704183484516925440.000000;
  }
  return;
}

void dataflow_node_36(ap_fixed<32, 8> arg0000[16][128], ap_fixed<32, 8> arg0001[1][16][1]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    ap_fixed<32, 8> v0001[4];
    #pragma HLS array_partition variable=v0001 complete
    for (int v0002 = (int)0; v0002 < (int)4; v0002 += (int)1) {
    #pragma HLS unroll
      v0001[v0002] = (ap_fixed<32, 8>)-340282346638528859811704183484516925440.000000;
    }
    for (int v0003 = (int)0; v0003 < (int)128; v0003 += (int)1) {
    #pragma HLS pipeline II=1
      ap_fixed<32, 8> v0004 = arg0000[v0000][v0003];
      int v0005 = v0003 & (int)3;
      ap_fixed<32, 8> v0006 = v0001[v0005];
      v0001[v0005] = (v0004 > v0006) ? v0004 : v0006;
    }
    ap_fixed<32, 8> v0007 = (v0001[0] > v0001[1]) ? v0001[0] : v0001[1];
    ap_fixed<32, 8> v0008 = (v0001[2] > v0001[3]) ? v0001[2] : v0001[3];
    ap_fixed<32, 8> v0009 = (v0007 > v0008) ? v0007 : v0008;
    ap_fixed<32, 8> v0010 = arg0001[(int)0][v0000][(int)0];
    arg0001[(int)0][v0000][(int)0] = (v0009 > v0010) ? v0009 : v0010;
  }
  return;
}

void dataflow_node_37(ap_fixed<32, 8> arg0000[16][128], ap_fixed<32, 8> arg0001[1][16][1], ap_fixed<32, 8> arg0002[16][128]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    int v0001 = v0000 / (int)16;
    int v0002 = v0001 * (int)16;
    ap_int<1> v0003 = v0000 != v0002;
    ap_int<1> v0004 = v0000 < (int)0;
    ap_int<1> v0005 = v0003 & v0004;
    int v0006 = v0001 + (int)-1;
    int v0007 = v0005 ? v0006 : v0001;
    int v0008 = v0000 % (int)16;
    ap_int<1> v0009 = v0008 < (int)0;
    int v0010 = v0008 + (int)16;
    int v0011 = v0009 ? v0010 : v0008;
    for (int v0012 = (int)0; v0012 < (int)128; v0012 += (int)1) {
      ap_fixed<32, 8> v0013 = arg0000[v0000][v0012];
      ap_fixed<32, 8> v0014 = arg0001[v0007][v0011][(int)0];
      ap_fixed<32, 8> v0015 = v0013 - v0014;
      arg0002[v0000][v0012] = v0015;
    }
  }
  return;
}

void dataflow_node_38(ap_fixed<32, 8> arg0000[16][128], ap_fixed<32, 8> arg0001[16][128]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    for (int v0001 = (int)0; v0001 < (int)128; v0001 += (int)1) {
    #pragma HLS pipeline II=1
      ap_fixed<32, 8> v0002 = arg0000[v0000][v0001];
      ap_fixed<32, 8> v0003 = hls::exp((float)v0002);
      arg0001[v0000][v0001] = v0003;
    }
  }
  return;
}

void dataflow_node_39(ap_fixed<32, 8> arg0000[1][16][1]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    arg0000[(int)0][v0000][(int)0] = (ap_fixed<32, 8>)0.000000;
  }
  return;
}

void dataflow_node_40(ap_fixed<32, 8> arg0000[16][128], ap_fixed<32, 8> arg0001[1][16][1]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    ap_fixed<32, 8> v0001[4];
    #pragma HLS array_partition variable=v0001 complete
    for (int v0002 = (int)0; v0002 < (int)4; v0002 += (int)1) {
    #pragma HLS unroll
      v0001[v0002] = (ap_fixed<32, 8>)0.000000;
    }
    for (int v0003 = (int)0; v0003 < (int)128; v0003 += (int)1) {
    #pragma HLS pipeline II=1
      ap_fixed<32, 8> v0004 = arg0000[v0000][v0003];
      int v0005 = v0003 & (int)3;
      v0001[v0005] = v0001[v0005] + v0004;
    }
    arg0001[(int)0][v0000][(int)0] =
        arg0001[(int)0][v0000][(int)0] + v0001[0] + v0001[1] + v0001[2] + v0001[3];
  }
  return;
}

void dataflow_node_41(ap_fixed<32, 8> arg0000[1][16][1], ap_fixed<32, 8> arg0001[16]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
  #pragma HLS pipeline II=1
    int v0001 = v0000 / (int)16;
    int v0002 = v0001 * (int)16;
    ap_int<1> v0003 = v0000 != v0002;
    ap_int<1> v0004 = v0000 < (int)0;
    ap_int<1> v0005 = v0003 & v0004;
    int v0006 = v0001 + (int)-1;
    int v0007 = v0005 ? v0006 : v0001;
    int v0008 = v0000 % (int)16;
    ap_int<1> v0009 = v0008 < (int)0;
    int v0010 = v0008 + (int)16;
    int v0011 = v0009 ? v0010 : v0008;
    ap_fixed<32, 8> v0012 = arg0000[v0007][v0011][(int)0];
    ap_fixed<32, 8> v0013 =
        (v0012 == (ap_fixed<32, 8>)0.000000)
            ? (ap_fixed<32, 8>)0.000000
            : (ap_fixed<32, 8>)hls::divide((float)(ap_fixed<32, 8>)1.000000,
                                           (float)v0012);
    arg0001[v0000] = v0013;
  }
  return;
}

void dataflow_node_42(ap_fixed<32, 8> arg0000[16][128], ap_fixed<32, 8> arg0001[16], ap_fixed<32, 8> arg0002[16][128]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    for (int v0001 = (int)0; v0001 < (int)128; v0001 += (int)1) {
      ap_fixed<32, 8> v0002 = arg0000[v0000][v0001];
      ap_fixed<32, 8> v0003 = arg0001[v0000];
      ap_fixed<32, 8> v0004 = v0002 * v0003;
      arg0002[v0000][v0001] = v0004;
    }
  }
  return;
}

void dataflow_node_43(ap_fixed<8, 4> arg0000[16][128], ap_fixed<8, 4> arg0001[16][128]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=2 cyclic factor=64
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=64
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    for (int v0001 = (int)0; v0001 < (int)128; v0001 += (int)1) {
    #pragma HLS unroll factor=64
      arg0001[v0000][v0001] = arg0000[v0000][v0001];
    }
  }
  return;
}

void dataflow_node_44(ap_fixed<8, 4> arg0000[16][1][64]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=64
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    for (int v0001 = (int)0; v0001 < (int)64; v0001 += (int)1) {
    #pragma HLS unroll factor=64
      arg0000[v0000][(int)0][v0001] = (ap_fixed<8, 4>)0.000000;
    }
  }
  return;
}

void dataflow_node_45(ap_fixed<8, 4> arg0000[16][128], ap_fixed<8, 4> arg0001[16][128][64], ap_fixed<8, 4> arg0002[16][1][64]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=2 cyclic factor=8
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=8
#pragma HLS array_reshape variable=arg0001 dim=3 cyclic factor=64
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=64
  for (int v0000 = (int)0; v0000 < (int)128; v0000 += (int)8) {
    for (int v0001 = (int)0; v0001 < (int)16; v0001 += (int)1) {
    #pragma HLS pipeline II=1
      ap_fixed<8, 4> v0003[1][1][8];
      #pragma HLS array_partition variable=v0003 dim=3 cyclic factor=8
      for (int v0004 = (int)0; v0004 < (int)8; v0004 += (int)1) {
      #pragma HLS unroll factor=8
        v0003[(int)0][(int)0][v0004] = arg0000[v0001][v0000 + v0004];
      }
      ap_fixed<8, 4> v0007[1][8][64];
      #pragma HLS array_partition variable=v0007 dim=2 cyclic factor=8
      #pragma HLS array_reshape variable=v0007 dim=3 cyclic factor=64
      for (int v0008 = (int)0; v0008 < (int)8; v0008 += (int)1) {
      #pragma HLS unroll factor=8
        for (int v0010 = (int)0; v0010 < (int)64; v0010 += (int)1) {
        #pragma HLS unroll factor=64
          v0007[(int)0][v0008][v0010] = arg0001[v0001][v0000 + v0008][v0010];
        }
      }
      ap_fixed<8, 4> v0013[1][1][64];
      #pragma HLS array_partition variable=v0013 dim=3 cyclic factor=64
      for (int v0014 = (int)0; v0014 < (int)64; v0014 += (int)1) {
      #pragma HLS unroll factor=64
        v0013[(int)0][(int)0][v0014] = arg0002[v0001][(int)0][v0014];
      }
      shared_kernel_4_attn(v0003, v0007, v0013);
      for (int v0017 = (int)0; v0017 < (int)64; v0017 += (int)1) {
      #pragma HLS unroll factor=64
        arg0002[v0001][(int)0][v0017] = v0013[(int)0][(int)0][v0017];
      }
    }
  }
  return;
}

void dataflow_node_46(ap_fixed<8, 4> arg0000[16][1][64], ap_fixed<8, 4> arg0001[16][64]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=64
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=64
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    for (int v0001 = (int)0; v0001 < (int)64; v0001 += (int)1) {
    #pragma HLS unroll factor=64
      arg0001[v0000][v0001] = arg0000[v0000][(int)0][v0001];
    }
  }
  return;
}

void dataflow_node_47(ap_fixed<8, 4> arg0000[1][1][1024]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)32) {
  #pragma HLS pipeline II=1
    for (int v0001 = (int)0; v0001 < (int)32; v0001 += (int)1) {
    #pragma HLS unroll factor=32
      int v0002 = v0000 + v0001;
      arg0000[(int)0][(int)0][v0002] = (ap_fixed<8, 4>)0.000000;
    }
  }
  return;
}

// dataflow_node_48: Wout GEMM, 8-bank HBM streaming.
// Weight layout: wout_bk[rg*16+col] stores one 64-lane output group
// per 512-bit beat.
void dataflow_node_48(
    ap_fixed<8, 4> arg0000[16][64],
    packed_fxp64_t *wout_b0, packed_fxp64_t *wout_b1,
    packed_fxp64_t *wout_b2, packed_fxp64_t *wout_b3,
    packed_fxp64_t *wout_b4, packed_fxp64_t *wout_b5,
    packed_fxp64_t *wout_b6, packed_fxp64_t *wout_b7,
    ap_fixed<8, 4> arg0002[1][1][1024]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=64
  for (int rg = 0; rg < 128; rg++) {
    for (int col = 0; col < 16; col++) {
    #pragma HLS pipeline II=1
      packed_fxp64_t w0 = wout_b0[rg * 16 + col];
      packed_fxp64_t w1 = wout_b1[rg * 16 + col];
      packed_fxp64_t w2 = wout_b2[rg * 16 + col];
      packed_fxp64_t w3 = wout_b3[rg * 16 + col];
      packed_fxp64_t w4 = wout_b4[rg * 16 + col];
      packed_fxp64_t w5 = wout_b5[rg * 16 + col];
      packed_fxp64_t w6 = wout_b6[rg * 16 + col];
      packed_fxp64_t w7 = wout_b7[rg * 16 + col];
      ap_fixed<8, 4> v0002[1][1][8];
      #pragma HLS array_partition variable=v0002 dim=3 complete
      for (int k = 0; k < 8; k++) {
      #pragma HLS unroll
        v0002[0][0][k] = arg0000[(rg * 8 + k) / 64][(rg * 8 + k) % 64];
      }
      ap_fixed<8, 4> v0005[1][8][64];
      #pragma HLS array_partition variable=v0005 dim=2 complete
      #pragma HLS array_partition variable=v0005 dim=3 complete
      ap_fixed<8, 4> v0011[1][1][64];
      #pragma HLS array_partition variable=v0011 dim=3 complete
      packed_fxp64_t wbanks[8];
      #pragma HLS array_partition variable=wbanks complete
      wbanks[0] = w0; wbanks[1] = w1; wbanks[2] = w2; wbanks[3] = w3;
      wbanks[4] = w4; wbanks[5] = w5; wbanks[6] = w6; wbanks[7] = w7;
      for (int k = 0; k < 8; k++) {
      #pragma HLS unroll
        for (int j = 0; j < 64; j++) {
        #pragma HLS unroll
          v0005[0][k][j].range(7, 0) = wbanks[k].range(j * 8 + 7, j * 8);
        }
      }
      for (int j = 0; j < 64; j++) {
      #pragma HLS unroll factor=64
        v0011[0][0][j] = arg0002[0][0][col * 64 + j];
      }
      shared_kernel_2_local_64(v0002, v0005, v0011);
      for (int j = 0; j < 64; j++) {
      #pragma HLS unroll factor=64
        arg0002[0][0][col * 64 + j] = v0011[0][0][j];
      }
    }
  }
  return;
}

void dataflow_node_49(ap_fixed<8, 4> arg0000[1][1][1024], ap_fixed<8, 4> arg0001[1024]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)32) {
  #pragma HLS pipeline II=1
    for (int v0002 = (int)0; v0002 < (int)32; v0002 += (int)1) {
    #pragma HLS unroll factor=32
      int v0003 = v0000 + v0002;
      arg0001[v0003] = arg0000[0][0][v0003];
    }
  }
  return;
}

void dataflow_node_50(ap_fixed<8, 4> arg0000[1024], ap_fixed<8, 4> arg0001[1024], ap_fixed<8, 4> arg0002[1024]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=1 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)32) {
  #pragma HLS pipeline II=1
    ap_fixed<8, 4> v0001[32];
    #pragma HLS array_partition variable=v0001 dim=1 cyclic factor=32
    for (int v0002 = (int)0; v0002 < (int)32; v0002 += (int)1) {
    #pragma HLS unroll factor=32
      int v0003 = v0000 + v0002;
      ap_fixed<8, 4> v0004 = arg0000[v0003];
      v0001[v0002] = v0004;
    }
    ap_fixed<8, 4> v0005[32];
    #pragma HLS array_partition variable=v0005 dim=1 cyclic factor=32
    for (int v0006 = (int)0; v0006 < (int)32; v0006 += (int)1) {
    #pragma HLS unroll factor=32
      int v0007 = v0000 + v0006;
      ap_fixed<8, 4> v0008 = arg0001[v0007];
      v0005[v0006] = v0008;
    }
    ap_fixed<8, 4> v0009[32];
    #pragma HLS array_partition variable=v0009 dim=1 cyclic factor=32
    for (int v0010 = (int)0; v0010 < (int)32; v0010 += (int)1) {
    #pragma HLS unroll factor=32
      int v0011 = v0000 + v0010;
      ap_fixed<8, 4> v0012 = arg0002[v0011];
      v0009[v0010] = v0012;
    }
    shared_kernel_1(v0001, v0005, v0009);
    for (int v0013 = (int)0; v0013 < (int)32; v0013 += (int)1) {
    #pragma HLS unroll factor=32
      ap_fixed<8, 4> v0014 = v0009[v0013];
      int v0015 = v0000 + v0013;
      arg0002[v0015] = v0014;
    }
  }
  return;
}

void dataflow_node_51(ap_fixed<8, 4> arg0000[1024], ap_fixed<8, 4> arg0001[1][1][1024], ap_fixed<8, 4> arg0002[1024]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=1 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)32) {
  #pragma HLS pipeline II=1
    ap_fixed<8, 4> v0001[32];
    #pragma HLS array_partition variable=v0001 dim=1 cyclic factor=32
    for (int v0002 = (int)0; v0002 < (int)32; v0002 += (int)1) {
    #pragma HLS unroll factor=32
      int v0003 = v0000 + v0002;
      ap_fixed<8, 4> v0004 = arg0000[v0003];
      v0001[v0002] = v0004;
    }
    ap_fixed<8, 4> v0005[32];
    #pragma HLS array_partition variable=v0005 dim=1 cyclic factor=32
    for (int v0006 = (int)0; v0006 < (int)32; v0006 += (int)1) {
    #pragma HLS unroll factor=32
      int v0007 = v0000 + v0006;
      int v0008 = v0007 / (int)1024;
      int v0009 = v0008 * (int)1024;
      ap_int<1> v0010 = v0007 != v0009;
      ap_int<1> v0011 = v0007 < (int)0;
      ap_int<1> v0012 = v0010 & v0011;
      int v0013 = v0008 + (int)-1;
      int v0014 = v0012 ? v0013 : v0008;
      int v0015 = v0007 % (int)1024;
      ap_int<1> v0016 = v0015 < (int)0;
      int v0017 = v0015 + (int)1024;
      int v0018 = v0016 ? v0017 : v0015;
      int v0019 = v0018 / (int)1024;
      int v0020 = v0007 % (int)1024;
      ap_int<1> v0021 = v0020 < (int)0;
      int v0022 = v0020 + (int)1024;
      int v0023 = v0021 ? v0022 : v0020;
      ap_fixed<8, 4> v0024 = arg0001[v0014][v0019][v0023];
      v0005[v0006] = v0024;
    }
    ap_fixed<8, 4> v0025[32];
    #pragma HLS array_partition variable=v0025 dim=1 cyclic factor=32
    for (int v0026 = (int)0; v0026 < (int)32; v0026 += (int)1) {
    #pragma HLS unroll factor=32
      int v0027 = v0000 + v0026;
      ap_fixed<8, 4> v0028 = arg0002[v0027];
      v0025[v0026] = v0028;
    }
    shared_kernel_1(v0001, v0005, v0025);
    for (int v0029 = (int)0; v0029 < (int)32; v0029 += (int)1) {
    #pragma HLS unroll factor=32
      ap_fixed<8, 4> v0030 = v0025[v0029];
      int v0031 = v0000 + v0029;
      arg0002[v0031] = v0030;
    }
  }
  return;
}

void dataflow_node_52(ap_fixed<8, 4> arg0000[1024], ap_fixed<8, 4> arg0001[1][1][1024]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=3 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)32) {
  #pragma HLS pipeline II=1
    for (int v0001 = (int)0; v0001 < (int)32; v0001 += (int)1) {
    #pragma HLS unroll factor=32
      int v0002 = v0000 + v0001;
      arg0001[(int)0][(int)0][v0002] = arg0000[v0002];
    }
  }
  return;
}

void dataflow_node_53(ap_fixed<8, 4> arg0000[16][128][64], ap_fixed<8, 4> arg0001[1][16][128][64]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    for (int v0001 = (int)0; v0001 < (int)128; v0001 += (int)1) {
      for (int v0002 = (int)0; v0002 < (int)64; v0002 += (int)1) {
        ap_fixed<8, 4> v0003 = arg0000[v0000][v0001][v0002];
        arg0001[(int)0][v0000][v0001][v0002] = v0003;
      }
    }
  }
  return;
}

void dataflow_node_54(ap_fixed<8, 4> arg0000[16][128][64], ap_fixed<8, 4> arg0001[1][16][128][64]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    for (int v0001 = (int)0; v0001 < (int)128; v0001 += (int)1) {
      for (int v0002 = (int)0; v0002 < (int)64; v0002 += (int)1) {
        ap_fixed<8, 4> v0003 = arg0000[v0000][v0001][v0002];
        arg0001[(int)0][v0000][v0001][v0002] = v0003;
      }
    }
  }
  return;
}


static ap_fixed<8, 4> fxp8_from_bits_local(ap_uint<8> bits) {
  ap_fixed<8, 4> value;
  value.range(7, 0) = bits;
  return value;
}

static ap_uint<8> fxp8_to_bits_local(ap_fixed<8, 4> value) {
  return value.range(7, 0);
}

void shared_kernel_3_local_qkv(
    ap_fixed<8, 4> arg0000[1][1][16],
    ap_fixed<8, 4> arg0001[1][16][32],
    ap_fixed<32, 8> arg0002[1][1][32]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=16
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=16
#pragma HLS array_partition variable=arg0001 dim=3 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=32
  ap_fixed<32, 8> v0000[8];
  #pragma HLS array_partition variable=v0000 complete
  for (int v0001 = (int)0; v0001 < (int)32; v0001 += (int)1) {
  #pragma HLS unroll factor=32
    for (int v0002 = (int)0; v0002 < (int)8; v0002 += (int)1) {
    #pragma HLS unroll factor=8
      v0000[v0002] = (ap_fixed<32, 8>)0.000000;
    }
    for (int v0003 = (int)0; v0003 < (int)16; v0003 += (int)1) {
    #pragma HLS unroll factor=16
      ap_fixed<8, 4> v0004 = arg0000[(int)0][(int)0][v0003];
      ap_fixed<8, 4> v0005 = arg0001[(int)0][v0003][v0001];
      int v0006 = v0003 & (int)7;
      ap_fixed<32, 8> v0007 = v0000[v0006];
      ap_fixed<32, 8> v0008 = v0004 * v0005;
      #pragma HLS BIND_OP variable=v0008 op=mul impl=dsp
      v0000[v0006] = v0007 + v0008;
    }
    ap_fixed<32, 8> v0009 = arg0002[(int)0][(int)0][v0001];
    ap_fixed<32, 8> v0010 = v0000[(int)0] + v0000[(int)1];
    #pragma HLS BIND_OP variable=v0010 op=add impl=fabric latency=1
    ap_fixed<32, 8> v0011 = v0000[(int)2] + v0000[(int)3];
    #pragma HLS BIND_OP variable=v0011 op=add impl=fabric latency=1
    ap_fixed<32, 8> v0012 = v0000[(int)4] + v0000[(int)5];
    #pragma HLS BIND_OP variable=v0012 op=add impl=fabric latency=1
    ap_fixed<32, 8> v0013 = v0000[(int)6] + v0000[(int)7];
    #pragma HLS BIND_OP variable=v0013 op=add impl=fabric latency=1
    ap_fixed<32, 8> v0014 = v0010 + v0011;
    #pragma HLS BIND_OP variable=v0014 op=add impl=fabric latency=1
    ap_fixed<32, 8> v0015 = v0012 + v0013;
    #pragma HLS BIND_OP variable=v0015 op=add impl=fabric latency=1
    ap_fixed<32, 8> v0016 = v0014 + v0015;
    #pragma HLS BIND_OP variable=v0016 op=add impl=fabric latency=1
    ap_fixed<32, 8> v0017 = v0016 + v0009;
    #pragma HLS BIND_OP variable=v0017 op=add impl=fabric latency=1
    arg0002[(int)0][(int)0][v0001] = v0017;
  }
  return;
}

void shared_kernel_3_local(
    ap_fixed<8, 4> arg0000[1][1][8],
    ap_fixed<8, 4> arg0001[1][8][32],
    ap_fixed<32, 8> arg0002[1][1][32]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=8
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=8
#pragma HLS array_partition variable=arg0001 dim=3 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=32
  ap_fixed<32, 8> v0000[4];
  #pragma HLS array_partition variable=v0000 complete
  for (int v0001 = (int)0; v0001 < (int)32; v0001 += (int)1) {
  #pragma HLS unroll factor=32
    for (int v0002 = (int)0; v0002 < (int)4; v0002 += (int)1) {
    #pragma HLS unroll factor=4
      v0000[v0002] = (ap_fixed<32, 8>)0.000000;
    }
    for (int v0003 = (int)0; v0003 < (int)8; v0003 += (int)1) {
    #pragma HLS unroll factor=8
      ap_fixed<8, 4> v0004 = arg0000[(int)0][(int)0][v0003];
      ap_fixed<8, 4> v0005 = arg0001[(int)0][v0003][v0001];
      int v0006 = v0003 & (int)3;
      ap_fixed<32, 8> v0007 = v0000[v0006];
      ap_fixed<32, 8> v0008 = v0004 * v0005;
      #pragma HLS BIND_OP variable=v0008 op=mul impl=dsp
      v0000[v0006] = v0007 + v0008;
    }
    ap_fixed<32, 8> v0009 = arg0002[(int)0][(int)0][v0001];
    ap_fixed<32, 8> v0010 = v0000[(int)0] + v0000[(int)1];
    #pragma HLS BIND_OP variable=v0010 op=add impl=fabric latency=1
    ap_fixed<32, 8> v0011 = v0000[(int)2] + v0000[(int)3];
    #pragma HLS BIND_OP variable=v0011 op=add impl=fabric latency=1
    ap_fixed<32, 8> v0012 = v0010 + v0011;
    #pragma HLS BIND_OP variable=v0012 op=add impl=fabric latency=1
    ap_fixed<32, 8> v0013 = v0012 + v0009;
    #pragma HLS BIND_OP variable=v0013 op=add impl=fabric latency=1
    arg0002[(int)0][(int)0][v0001] = v0013;
  }
  return;
}

void shared_kernel_2_local_32fx8(
    ap_fixed<8, 4> arg0000[1][1][8],
    ap_fixed<8, 4> arg0001[1][8][32],
    ap_fixed<8, 4> arg0002[1][1][32]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=8
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=8
#pragma HLS array_partition variable=arg0001 dim=3 complete
#pragma HLS array_partition variable=arg0002 dim=3 complete
  ap_fixed<8, 4> v0000[4];
  #pragma HLS array_partition variable=v0000 complete
  for (int v0001 = 0; v0001 < 32; ++v0001) {
  #pragma HLS unroll factor=32
    for (int v0002 = 0; v0002 < 4; ++v0002) {
    #pragma HLS unroll factor=4
      v0000[v0002] = (ap_fixed<8, 4>)0.000000;
    }
    for (int v0003 = 0; v0003 < 8; ++v0003) {
    #pragma HLS unroll factor=8
      ap_fixed<8, 4> v0004 = arg0000[0][0][v0003];
      ap_fixed<8, 4> v0005 = arg0001[0][v0003][v0001];
      int lane_grp = v0003 & 3;
      ap_fixed<8, 4> v0013_prod = v0004 * v0005;
      #pragma HLS BIND_OP variable=v0013_prod op=mul impl=dsp
      v0000[lane_grp] = v0000[lane_grp] + v0013_prod;
    }
    ap_fixed<8, 4> v0019 = arg0002[0][0][v0001];
    ap_fixed<8, 4> v0022 = (v0000[0] + v0000[1]) + (v0000[2] + v0000[3]);
    arg0002[0][0][v0001] = v0022 + v0019;
  }
  return;
}

void shared_kernel_3_local_32fx8(
    ap_fixed<8, 4> arg0000[1][1][8],
    ap_fixed<8, 4> arg0001[1][8][32],
    ap_fixed<8, 4> arg0002[1][1][32]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=8
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=8
#pragma HLS array_partition variable=arg0001 dim=3 complete
#pragma HLS array_partition variable=arg0002 dim=3 complete
  ap_fixed<8, 4> v0000[4];
  #pragma HLS array_partition variable=v0000 dim=1 cyclic factor=4
  for (int v0001 = 0; v0001 < 32; ++v0001) {
  #pragma HLS unroll factor=32
    for (int v0002 = 0; v0002 < 4; ++v0002) {
    #pragma HLS unroll factor=4
      v0000[v0002] = (ap_fixed<8, 4>)0.000000;
    }
    for (int v0003 = 0; v0003 < 8; ++v0003) {
    #pragma HLS unroll factor=8
      ap_fixed<8, 4> v0004 = arg0000[0][0][v0003];
      ap_fixed<8, 4> v0005 = arg0001[0][v0003][v0001];
      int v0006 = v0003 % 4;
      ap_int<1> v0007n = v0006 < 0;
      int v0007 = v0006 + 4;
      int v0008 = v0007n ? v0007 : v0006;
      ap_fixed<8, 4> v0009 = v0000[v0008];
      ap_fixed<8, 4> v0010 = v0004 * v0005;
      #pragma HLS BIND_OP variable=v0010 op=mul impl=dsp
      ap_fixed<8, 4> v0011 = v0009 + v0010;
      int v0012 = v0003 % 4;
      ap_int<1> v0013n = v0012 < 0;
      int v0013 = v0012 + 4;
      int v0014 = v0013n ? v0013 : v0012;
      v0000[v0014] = v0011;
    }
    ap_fixed<8, 4> v0015 = arg0002[0][0][v0001];
    ap_fixed<8, 4> v0016 = v0000[0];
    ap_fixed<8, 4> v0017 = v0000[1];
    ap_fixed<8, 4> v0018 = v0016 + v0017;
    ap_fixed<8, 4> v0019 = v0000[2];
    ap_fixed<8, 4> v0020 = v0000[3];
    ap_fixed<8, 4> v0021 = v0019 + v0020;
    ap_fixed<8, 4> v0022 = v0018 + v0021;
    ap_fixed<8, 4> v0023 = v0022 + v0015;
    arg0002[0][0][v0001] = v0023;
  }
  return;
}

void shared_kernel_2_local_64(
    ap_fixed<8, 4> arg0000[1][1][8],
    ap_fixed<8, 4> arg0001[1][8][64],
    ap_fixed<8, 4> arg0002[1][1][64]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=8
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=8
#pragma HLS array_partition variable=arg0001 dim=3 complete
#pragma HLS array_partition variable=arg0002 dim=3 complete
  ap_fixed<8, 4> v0000[4];
  #pragma HLS array_partition variable=v0000 complete
  for (int v0001 = 0; v0001 < 64; ++v0001) {
  #pragma HLS unroll
    for (int v0002 = 0; v0002 < 4; ++v0002) {
    #pragma HLS unroll
      v0000[v0002] = (ap_fixed<8, 4>)0.000000;
    }
    for (int v0003 = 0; v0003 < 8; ++v0003) {
    #pragma HLS unroll
      ap_fixed<8, 4> v0004 = arg0000[0][0][v0003];
      ap_fixed<8, 4> v0005 = arg0001[0][v0003][v0001];
      int lane_grp = v0003 & 3;
      ap_fixed<8, 4> v0013_prod = v0004 * v0005;
      #pragma HLS BIND_OP variable=v0013_prod op=mul impl=dsp
      v0000[lane_grp] = v0000[lane_grp] + v0013_prod;
    }
    ap_fixed<8, 4> v0019 = arg0002[0][0][v0001];
    ap_fixed<8, 4> v0022 = (v0000[0] + v0000[1]) + (v0000[2] + v0000[3]);
    arg0002[0][0][v0001] = v0022 + v0019;
  }
  return;
}

void shared_kernel_3_local_64(
    ap_fixed<8, 4> arg0000[1][1][8],
    ap_fixed<8, 4> arg0001[1][8][64],
    ap_fixed<8, 4> arg0002[1][1][64]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=8
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=8
#pragma HLS array_partition variable=arg0001 dim=3 complete
#pragma HLS array_partition variable=arg0002 dim=3 complete
  ap_fixed<8, 4> v0000[4];
  #pragma HLS array_partition variable=v0000 dim=1 cyclic factor=4
  for (int v0001 = 0; v0001 < 64; ++v0001) {
  #pragma HLS unroll factor=64
    for (int v0002 = 0; v0002 < 4; ++v0002) {
    #pragma HLS unroll factor=4
      v0000[v0002] = (ap_fixed<8, 4>)0.000000;
    }
    for (int v0003 = 0; v0003 < 8; ++v0003) {
    #pragma HLS unroll factor=8
      ap_fixed<8, 4> v0004 = arg0000[0][0][v0003];
      ap_fixed<8, 4> v0005 = arg0001[0][v0003][v0001];
      int v0006 = v0003 % 4;
      ap_int<1> v0007n = v0006 < 0;
      int v0007 = v0006 + 4;
      int v0008 = v0007n ? v0007 : v0006;
      ap_fixed<8, 4> v0009 = v0000[v0008];
      ap_fixed<8, 4> v0010 = v0004 * v0005;
      #pragma HLS BIND_OP variable=v0010 op=mul impl=dsp
      ap_fixed<8, 4> v0011 = v0009 + v0010;
      int v0012 = v0003 % 4;
      ap_int<1> v0013n = v0012 < 0;
      int v0013 = v0012 + 4;
      int v0014 = v0013n ? v0013 : v0012;
      v0000[v0014] = v0011;
    }
    ap_fixed<8, 4> v0015 = arg0002[0][0][v0001];
    ap_fixed<8, 4> v0016 = v0000[0];
    ap_fixed<8, 4> v0017 = v0000[1];
    ap_fixed<8, 4> v0018 = v0000[2];
    ap_fixed<8, 4> v0019 = v0000[3];
    ap_fixed<8, 4> v0020 = v0016 + v0017;
    ap_fixed<8, 4> v0021 = v0018 + v0019;
    ap_fixed<8, 4> v0022 = v0020 + v0021;
    ap_fixed<8, 4> v0023 = v0022 + v0015;
    arg0002[0][0][v0001] = v0023;
  }
  return;
}

void dataflow_node_23_fast(ap_fixed<8, 4> arg0000[3072], ap_fixed<8, 4> arg0001[16][64]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    int v0001 = v0000 * (int)64;
    for (int v0002 = (int)0; v0002 < (int)64; v0002 += (int)1) {
    #pragma HLS pipeline II=1
      int v0003 = v0001 + v0002;
      int v0004 = v0003 + (int)1024;
      arg0001[v0000][v0002] = arg0000[v0004];
    }
  }
  return;
}

void dataflow_node_24_fast(ap_fixed<8, 4> arg0000[3072], ap_fixed<8, 4> arg0001[16][64]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    int v0001 = v0000 * (int)64;
    for (int v0002 = (int)0; v0002 < (int)64; v0002 += (int)1) {
    #pragma HLS pipeline II=1
      int v0003 = v0001 + v0002;
      int v0004 = v0003 + (int)2048;
      arg0001[v0000][v0002] = arg0000[v0004];
    }
  }
  return;
}

void dataflow_node_25_fast(
    ap_int<32> arg0000[1],
    ap_fixed<8, 4> arg0001[16][64],
    ap_uint<256> arg0002[1][16][128][2],
    ap_uint<256> arg0003[1][16][64][4]) {
#pragma HLS inline off
  int v0000 = arg0000[(int)0];
  int v0001 = v0000 >> 5;
  int v0002 = v0000 & (int)31;
  for (int v0003 = (int)0; v0003 < (int)16; v0003 += (int)1) {
    ap_uint<256> v0004 = 0;
    ap_uint<256> v0005 = 0;
    for (int v0006 = (int)0; v0006 < (int)32; v0006 += (int)1) {
    #pragma HLS unroll
      v0004.range(v0006 * 8 + 7, v0006 * 8) = fxp8_to_bits_local(arg0001[v0003][v0006]);
    }
    for (int v0007 = (int)0; v0007 < (int)32; v0007 += (int)1) {
    #pragma HLS unroll
      int v0008 = v0007 + (int)32;
      v0005.range(v0007 * 8 + 7, v0007 * 8) = fxp8_to_bits_local(arg0001[v0003][v0008]);
    }
    arg0002[(int)0][v0003][v0000][(int)0] = v0004;
    arg0002[(int)0][v0003][v0000][(int)1] = v0005;
    for (int v0009 = (int)0; v0009 < (int)64; v0009 += (int)1) {
    #pragma HLS pipeline II=1
      ap_uint<256> v0010 = arg0003[(int)0][v0003][v0009][v0001];
      v0010.range(v0002 * 8 + 7, v0002 * 8) = fxp8_to_bits_local(arg0001[v0003][v0009]);
      arg0003[(int)0][v0003][v0009][v0001] = v0010;
    }
  }
  return;
}

void dataflow_node_26_fast(ap_int<32> arg0000[1], ap_fixed<8, 4> arg0001[16][64], ap_uint<256> arg0002[1][16][128][2]) {
#pragma HLS inline off
  int v0000 = arg0000[(int)0];
  for (int v0001 = (int)0; v0001 < (int)16; v0001 += (int)1) {
    ap_uint<256> v0002 = 0;
    ap_uint<256> v0003 = 0;
    for (int v0004 = (int)0; v0004 < (int)32; v0004 += (int)1) {
    #pragma HLS unroll
      v0002.range(v0004 * 8 + 7, v0004 * 8) = fxp8_to_bits_local(arg0001[v0001][v0004]);
    }
    for (int v0005 = (int)0; v0005 < (int)32; v0005 += (int)1) {
    #pragma HLS unroll
      int v0006 = v0005 + (int)32;
      v0003.range(v0005 * 8 + 7, v0005 * 8) = fxp8_to_bits_local(arg0001[v0001][v0006]);
    }
    arg0002[(int)0][v0001][v0000][(int)0] = v0002;
    arg0002[(int)0][v0001][v0000][(int)1] = v0003;
  }
  return;
}

void dataflow_node_29_fast(ap_fixed<8, 4> arg0000[3072], ap_uint<256> arg0001[1][16][64][4], ap_fixed<32, 8> arg0002[16][1][128]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=3 cyclic factor=8
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)64; v0000 += (int)8) {
    for (int v0001 = (int)0; v0001 < (int)16; v0001 += (int)1) {
      for (int v0002 = (int)0; v0002 < (int)128; v0002 += (int)32) {
      #pragma HLS pipeline II=1
        ap_fixed<8, 4> v0003[1][1][8];
        #pragma HLS array_partition variable=v0003 dim=3 cyclic factor=8
        for (int v0004 = (int)0; v0004 < (int)8; v0004 += (int)1) {
        #pragma HLS unroll factor=8
          int v0005 = v0000 + v0004;
          v0003[(int)0][(int)0][v0004] = arg0000[v0001 * (int)64 + v0005];
        }
        ap_fixed<8, 4> v0006[1][8][32];
        #pragma HLS array_partition variable=v0006 dim=2 cyclic factor=8
        #pragma HLS array_partition variable=v0006 dim=3 cyclic factor=32
        int v0007 = v0002 / (int)32;
        for (int v0008 = (int)0; v0008 < (int)8; v0008 += (int)1) {
        #pragma HLS unroll factor=8
          int v0009 = v0000 + v0008;
          ap_uint<256> v0010 = arg0001[(int)0][v0001][v0009][v0007];
          for (int v0011 = (int)0; v0011 < (int)32; v0011 += (int)1) {
          #pragma HLS unroll factor=32
            ap_fixed<8, 4> v0012;
            v0012.range(7, 0) = v0010.range(v0011 * 8 + 7, v0011 * 8);
            v0006[(int)0][v0008][v0011] = v0012;
          }
        }
        ap_fixed<32, 8> v0013[1][1][32];
        #pragma HLS array_partition variable=v0013 dim=3 cyclic factor=32
        for (int v0014 = (int)0; v0014 < (int)32; v0014 += (int)1) {
        #pragma HLS unroll factor=32
          v0013[(int)0][(int)0][v0014] = arg0002[v0001][(int)0][v0002 + v0014];
        }
        shared_kernel_3_local(v0003, v0006, v0013);
        for (int v0015 = (int)0; v0015 < (int)32; v0015 += (int)1) {
        #pragma HLS unroll factor=32
          arg0002[v0001][(int)0][v0002 + v0015] = v0013[(int)0][(int)0][v0015];
        }
      }
    }
  }
  return;
}

void dataflow_node_45_fast(ap_fixed<8, 4> arg0000[16][128], ap_uint<256> arg0001[1][16][128][2], ap_fixed<32, 8> arg0002[16][1][64]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=2 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=3 cyclic factor=8
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)128; v0000 += (int)8) {
    for (int v0001 = (int)0; v0001 < (int)16; v0001 += (int)1) {
      for (int v0002 = (int)0; v0002 < (int)64; v0002 += (int)32) {
      #pragma HLS pipeline II=1
        ap_fixed<8, 4> v0003[1][1][8];
        #pragma HLS array_partition variable=v0003 dim=3 cyclic factor=8
        for (int v0004 = (int)0; v0004 < (int)8; v0004 += (int)1) {
        #pragma HLS unroll factor=8
          v0003[(int)0][(int)0][v0004] = arg0000[v0001][v0000 + v0004];
        }
        ap_fixed<8, 4> v0005[1][8][32];
        #pragma HLS array_partition variable=v0005 dim=2 cyclic factor=8
        #pragma HLS array_partition variable=v0005 dim=3 cyclic factor=32
        int v0006 = v0002 / (int)32;
        for (int v0007 = (int)0; v0007 < (int)8; v0007 += (int)1) {
        #pragma HLS unroll factor=8
          ap_uint<256> v0008 = arg0001[(int)0][v0001][v0000 + v0007][v0006];
          for (int v0009 = (int)0; v0009 < (int)32; v0009 += (int)1) {
          #pragma HLS unroll factor=32
            ap_fixed<8, 4> v0010;
            v0010.range(7, 0) = v0008.range(v0009 * 8 + 7, v0009 * 8);
            v0005[(int)0][v0007][v0009] = v0010;
          }
        }
        ap_fixed<32, 8> v0011[1][1][32];
        #pragma HLS array_partition variable=v0011 dim=3 cyclic factor=32
        for (int v0012 = (int)0; v0012 < (int)32; v0012 += (int)1) {
        #pragma HLS unroll factor=32
          v0011[(int)0][(int)0][v0012] = arg0002[v0001][(int)0][v0002 + v0012];
        }
        shared_kernel_3_local(v0003, v0005, v0011);
        for (int v0013 = (int)0; v0013 < (int)32; v0013 += (int)1) {
        #pragma HLS unroll factor=32
          arg0002[v0001][(int)0][v0002 + v0013] = v0011[(int)0][(int)0][v0013];
        }
      }
    }
  }
  return;
}

void dataflow_softmax_pass1(
    ap_int<1> arg0000[1][1][1024][1024],
    ap_fixed<8, 4> arg0001[16][128],
    ap_fixed<8, 4> arg0002[1][1][1][128],
    ap_fixed<8, 4> arg0003[16][128],
    ap_fixed<8, 4> arg0004[1][16][1]) {
#pragma HLS inline off
  for (int v0000 = 0; v0000 < 16; ++v0000) {
    ap_fixed<8, 4> v0001[4];
    #pragma HLS array_partition variable=v0001 complete
    for (int v0002 = 0; v0002 < 4; ++v0002) {
    #pragma HLS unroll
      v0001[v0002] = (ap_fixed<8, 4>)0.000000;
    }
    for (int v0003 = 0; v0003 < 128; ++v0003) {
    #pragma HLS pipeline II=1
      ap_fixed<8, 4> v0004 = arg0001[v0000][v0003];
      ap_fixed<8, 4> v0005 = v0004 * (ap_fixed<8, 4>)0.125000;
      ap_int<1> v0006 = arg0000[0][0][127][v0003];
      ap_fixed<8, 4> v0007 = v0006 ? v0005 : (ap_fixed<8, 4>)-INFINITY;
      ap_fixed<8, 4> v0008 = v0007 + arg0002[0][0][0][v0003];
      arg0003[v0000][v0003] = v0008;
      int v0010 = v0003 & 3;
      ap_fixed<8, 4> v0011 = v0001[v0010];
      v0001[v0010] = (v0008 > v0011) ? v0008 : v0011;
    }
    ap_fixed<8, 4> v0012 = (v0001[0] > v0001[1]) ? v0001[0] : v0001[1];
    ap_fixed<8, 4> v0013 = (v0001[2] > v0001[3]) ? v0001[2] : v0001[3];
    arg0004[0][v0000][0] = (v0012 > v0013) ? v0012 : v0013;
  }
  return;
}

void dataflow_softmax_pass2(
    ap_fixed<8, 4> arg0000[16][128],
    ap_fixed<8, 4> arg0001[1][16][1],
    ap_fixed<8, 4> arg0002[16][128],
    ap_fixed<8, 4> arg0003[1][16][1]) {
#pragma HLS inline off
  for (int v0000 = 0; v0000 < 16; ++v0000) {
    ap_fixed<8, 4> max_val = arg0001[0][v0000][0];
    // exp(score - max) in fxp8
    ap_fixed<8, 4> exp_buf[128];
    #pragma HLS array_partition variable=exp_buf dim=1 cyclic factor=64
    ap_fixed<8, 4> sum_acc[4];
    #pragma HLS array_partition variable=sum_acc complete
    for (int v0002 = 0; v0002 < 4; ++v0002) {
    #pragma HLS unroll
      sum_acc[v0002] = (ap_fixed<8, 4>)0.000000;
    }
    for (int v0004 = 0; v0004 < 128; ++v0004) {
    #pragma HLS pipeline II=1
      ap_fixed<8, 4> shifted = arg0000[v0000][v0004] - max_val;
      ap_fixed<8, 4> e = hls::exp(shifted);
      exp_buf[v0004] = e;
      int lane = v0004 & 3;
      sum_acc[lane] = sum_acc[lane] + e;
    }
    ap_fixed<8, 4> total = (sum_acc[0] + sum_acc[1]) + (sum_acc[2] + sum_acc[3]);
    ap_fixed<8, 4> inv_sum =
        (total == (ap_fixed<8, 4>)0.000000)
            ? (ap_fixed<8, 4>)0.000000
            : hls::divide((ap_fixed<8, 4>)1.000000, total);
    for (int v0004 = 0; v0004 < 128; ++v0004) {
    #pragma HLS pipeline II=1
      ap_fixed<8, 4> w = exp_buf[v0004] * inv_sum;
      #pragma HLS BIND_OP variable=w op=mul impl=dsp
      arg0002[v0000][v0004] = w;
    }
    arg0003[0][v0000][0] = total;
  }
  return;
}

void dataflow_softmax_pass3(
    ap_fixed<32, 8> arg0000[16][128],
    ap_fixed<32, 8> arg0001[1][16][1],
    ap_fixed<8, 4> arg0002[16][128]) {
#pragma HLS inline off
  for (int v0000 = 0; v0000 < 16; ++v0000) {
    ap_fixed<32, 8> v0001 =
        (arg0001[0][v0000][0] == (ap_fixed<32, 8>)0.000000)
            ? (ap_fixed<32, 8>)0.000000
            : (ap_fixed<32, 8>)hls::divide((float)(ap_fixed<32, 8>)1.000000,
                                           (float)arg0001[0][v0000][0]);
    for (int v0002 = 0; v0002 < 128; ++v0002) {
    #pragma HLS pipeline II=1
      ap_fixed<32, 8> v0003 = arg0000[v0000][v0002] * v0001;
      arg0002[v0000][v0002] = (ap_fixed<8, 4>)v0003;
    }
  }
  return;
}

void dataflow_softmax_pass3_attn_v(
    ap_fixed<32, 8> arg0000[16][128],
    ap_fixed<32, 8> arg0001[1][16][1],
    ap_uint<256> arg0002[1][16][128][2],
    ap_fixed<32, 8> arg0003[16][1][64]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=2 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=8
#pragma HLS array_partition variable=arg0003 dim=3 cyclic factor=32
  for (int v0000 = 0; v0000 < 16; ++v0000) {
    ap_fixed<32, 8> v0001 =
        (arg0001[0][v0000][0] == (ap_fixed<32, 8>)0.000000)
            ? (ap_fixed<32, 8>)0.000000
            : (ap_fixed<32, 8>)hls::divide((float)(ap_fixed<32, 8>)1.000000,
                                           (float)arg0001[0][v0000][0]);
    for (int v0002 = 0; v0002 < 128; v0002 += 8) {
      for (int v0003 = 0; v0003 < 64; v0003 += 32) {
      #pragma HLS pipeline II=1
        ap_fixed<8, 4> v0004[1][1][8];
        #pragma HLS array_partition variable=v0004 dim=3 cyclic factor=8
        for (int v0005 = 0; v0005 < 8; ++v0005) {
        #pragma HLS unroll factor=8
          ap_fixed<32, 8> v0006 = arg0000[v0000][v0002 + v0005] * v0001;
          v0004[0][0][v0005] = (ap_fixed<8, 4>)v0006;
        }
        ap_fixed<8, 4> v0007[1][8][32];
        #pragma HLS array_partition variable=v0007 dim=2 cyclic factor=8
        #pragma HLS array_partition variable=v0007 dim=3 cyclic factor=32
        int v0008 = v0003 / 32;
        for (int v0009 = 0; v0009 < 8; ++v0009) {
        #pragma HLS unroll factor=8
          ap_uint<256> v0010 = arg0002[0][v0000][v0002 + v0009][v0008];
          for (int v0011 = 0; v0011 < 32; ++v0011) {
          #pragma HLS unroll factor=32
            ap_fixed<8, 4> v0012;
            v0012.range(7, 0) = v0010.range(v0011 * 8 + 7, v0011 * 8);
            v0007[0][v0009][v0011] = v0012;
          }
        }
        ap_fixed<32, 8> v0013[1][1][32];
        #pragma HLS array_partition variable=v0013 dim=3 cyclic factor=32
        for (int v0014 = 0; v0014 < 32; ++v0014) {
        #pragma HLS unroll factor=32
          v0013[0][0][v0014] = arg0003[v0000][0][v0003 + v0014];
        }
        shared_kernel_3_local(v0004, v0007, v0013);
        for (int v0015 = 0; v0015 < 32; ++v0015) {
        #pragma HLS unroll factor=32
          arg0003[v0000][0][v0003 + v0015] = v0013[0][0][v0015];
        }
      }
    }
  }
  return;
}
void kernel_0_chip0(
    ap_int<1> arg0000[1][1][1024][1024],
    ap_fixed<8, 4> arg0001[1],
    ap_fixed<8, 4> arg0048[1][1][1024],
    ap_int<32> arg0049[1],
    ap_fixed<8, 4> arg0050[1][1][1][128],
    ap_uint<256> arg0051_lo[1][8][128][2],
    ap_uint<256> arg0051_hi[1][8][128][2],
    ap_uint<256> arg0052_lo[1][8][128][2],
    ap_uint<256> arg0052_hi[1][8][128][2],
    ap_fixed<8, 4> arg0053[1][1][1024],
    ap_fixed<8, 4> arg0054[1][1][1024],
    ap_fixed<8, 4> arg0057[1024],        // wout_bias (local BRAM)
    packed_fxp64_t *b0, packed_fxp64_t *b1,
    packed_fxp64_t *b2, packed_fxp64_t *b3,
    packed_fxp64_t *b4, packed_fxp64_t *b5,
    packed_fxp64_t *b6, packed_fxp64_t *b7,
    ap_fixed<8, 4> arg0059[3072],        // qkv_bias (local BRAM)
    ap_fixed<8, 4> arg0061[1024],        // ln1_gamma (local BRAM)
    ap_fixed<8, 4> arg0062[1024]) {      // ln1_beta (local BRAM)
#pragma HLS interface s_axilite port=return bundle=control
#pragma HLS interface ap_memory port=arg0048
#pragma HLS interface ap_memory port=arg0050
#pragma HLS interface ap_memory port=arg0051_lo
#pragma HLS interface ap_memory port=arg0051_hi
#pragma HLS interface ap_memory port=arg0052_lo
#pragma HLS interface ap_memory port=arg0052_hi
#pragma HLS interface ap_memory port=arg0053
#pragma HLS interface ap_memory port=arg0054
#pragma HLS array_partition variable=arg0057 dim=1 cyclic factor=32
#pragma HLS interface ap_memory port=arg0057
#pragma HLS array_partition variable=arg0059 dim=1 cyclic factor=32
#pragma HLS interface ap_memory port=arg0059
#pragma HLS array_partition variable=arg0061 dim=1 cyclic factor=32
#pragma HLS interface ap_memory port=arg0061
#pragma HLS array_partition variable=arg0062 dim=1 cyclic factor=32
#pragma HLS interface ap_memory port=arg0062
  ap_fixed<32, 8> v0000[1][1];
  dataflow_node_0(v0000);
  dataflow_node_1(arg0048, v0000);
  ap_fixed<8, 4> v0001[1];
  dataflow_node_2(v0000, v0001);
  ap_fixed<8, 4> v0002[1];
  dataflow_node_3(v0001, v0002);
  ap_fixed<8, 4> v0003[1024];
  #pragma HLS array_partition variable=v0003 dim=1 cyclic factor=32
  dataflow_node_4(v0002, v0003);
  ap_fixed<8, 4> v0004[1024];
  #pragma HLS array_partition variable=v0004 dim=1 cyclic factor=32
  dataflow_node_5(arg0048, v0003, v0004);
  dataflow_node_6(v0004, v0004, v0003);
  ap_fixed<8, 4> v0005[1][1];
  dataflow_node_7(v0005);
  dataflow_node_8(v0003, v0005);
  dataflow_node_9(v0005, v0001);
  dataflow_node_10(v0001, v0002);
  dataflow_node_11(v0002, v0001);
  dataflow_node_12(v0001, v0002);
  dataflow_node_13(v0002, v0003);
  ap_fixed<8, 4> v0006[1024];
  #pragma HLS array_partition variable=v0006 dim=1 cyclic factor=32
  dataflow_node_14(v0004, v0003, v0006);
  c0_debug_dump("ln1_mul", v0006, 1024);
  dataflow_node_15(v0006, arg0061, v0003);
  c0_debug_dump("ln1_gamma", v0003, 1024);
  dataflow_node_16(v0003, arg0062, v0004);
  c0_debug_dump("ln1_out", v0004, 1024);
  ap_fixed<8, 4> v0007[1][1][3072];
  #pragma HLS array_partition variable=v0007 dim=3 cyclic factor=64
  // #pragma HLS bind_storage variable=v0007 type=ram_2p impl=bram latency=2
  dataflow_node_17(v0007);
  // QKV GEMM: each bundle owns the first C0_QKV_BANK words of its bank.
  dataflow_node_18(v0004, b0, b1, b2, b3, b4, b5, b6, b7, v0007);
  ap_fixed<8, 4> v0008[3072];
  #pragma HLS array_partition variable=v0008 dim=1 cyclic factor=64
  // #pragma HLS bind_storage variable=v0008 type=ram_2p impl=bram latency=2
  dataflow_node_19(v0007, v0008);
  ap_fixed<8, 4> v0009[3072];
  #pragma HLS array_partition variable=v0009 dim=1 cyclic factor=64
  // #pragma HLS bind_storage variable=v0009 type=ram_2p impl=bram latency=2
  dataflow_node_20(v0008, arg0059, v0009);
  ap_int<64> v0010[1];
  dataflow_node_21(arg0049, v0010);
  ap_int<1> v0011[128];
  #pragma HLS array_partition variable=v0011 dim=1 cyclic factor=64
  dataflow_node_22(v0010, v0011);
  ap_uint<512> v0012_packed[16][128][1];
  #pragma HLS array_partition variable=v0012_packed dim=2 cyclic factor=8
  dataflow_node_23(v0009, v0012_packed);
  ap_uint<512> v0013_packed[16][128][1];
  #pragma HLS array_partition variable=v0013_packed dim=2 cyclic factor=8
  dataflow_node_24(v0009, v0013_packed);
  ap_fixed<8, 4> v0012[16][128][64];
  #pragma HLS array_partition variable=v0012 dim=2 cyclic factor=8
  #pragma HLS array_reshape variable=v0012 dim=3 cyclic factor=64
  dataflow_node_25_half(v0011, v0012_packed, arg0051_lo, v0012, 0);
  dataflow_node_25_half(v0011, v0012_packed, arg0051_hi, v0012, 8);
  ap_fixed<8, 4> v0013[16][128][64];
  #pragma HLS array_partition variable=v0013 dim=2 cyclic factor=8
  #pragma HLS array_reshape variable=v0013 dim=3 cyclic factor=64
  dataflow_node_26_half(v0011, v0013_packed, arg0052_lo, v0013, 0);
  dataflow_node_26_half(v0011, v0013_packed, arg0052_hi, v0013, 8);
  ap_uint<512> v0014[1][16][64][2];
  #pragma HLS array_partition variable=v0014 dim=3 cyclic factor=8
  dataflow_node_27(v0012, v0014);
  ap_fixed<8, 4> v0016[16][1][128];
  #pragma HLS array_partition variable=v0016 dim=3 cyclic factor=32
  dataflow_node_28(v0016);
  dataflow_node_29(v0009, v0014, v0016);
  ap_fixed<8, 4> v0017[16][128];
  #pragma HLS array_partition variable=v0017 dim=2 cyclic factor=32
  dataflow_node_30(v0016, v0017);
  ap_fixed<8, 4> v0019[16][128];
  #pragma HLS array_partition variable=v0019 dim=2 cyclic factor=64
  ap_fixed<8, 4> v0020[1][16][1];
  dataflow_softmax_pass1(arg0000, v0017, arg0050, v0019, v0020);
  ap_fixed<8, 4> v0021[16][128];
  #pragma HLS array_partition variable=v0021 dim=2 cyclic factor=64
  ap_fixed<8, 4> v0022[1][16][1];
  dataflow_softmax_pass2(v0019, v0020, v0021, v0022);
  ap_fixed<8, 4> v0023_softmax[16][128];
  #pragma HLS array_partition variable=v0023_softmax dim=2 cyclic factor=64
  dataflow_node_43(v0021, v0023_softmax);
  ap_fixed<8, 4> v0024[16][1][64];
  #pragma HLS array_partition variable=v0024 dim=3 cyclic factor=64
  dataflow_node_44(v0024);
  dataflow_node_45(v0023_softmax, v0013, v0024);
  ap_fixed<8, 4> v0025[16][64];
  #pragma HLS array_partition variable=v0025 dim=2 cyclic factor=64
  dataflow_node_46(v0024, v0025);
  ap_fixed<8, 4> v0026[1][1][1024];
  #pragma HLS array_partition variable=v0026 dim=3 cyclic factor=64
  dataflow_node_47(v0026);
  // Wout GEMM: each bundle owns words [C0_WOUT_OFF .. C0_BANK_WORDS-1].
  // Pointer arithmetic shifts the base; no simultaneous bundle ownership with QKV.
  dataflow_node_48(v0025, b0+C0_WOUT_OFF, b1+C0_WOUT_OFF, b2+C0_WOUT_OFF, b3+C0_WOUT_OFF,
                            b4+C0_WOUT_OFF, b5+C0_WOUT_OFF, b6+C0_WOUT_OFF, b7+C0_WOUT_OFF, v0026);
  dataflow_node_49(v0026, v0003);
  dataflow_node_50(v0003, arg0057, v0004);
  dataflow_node_51(v0004, arg0048, v0003);
  dataflow_node_52(v0003, arg0053);
  dataflow_node_52(v0003, arg0054);
  // KV banks are updated in place; no explicit full-bank output copy.
  return;
}
#undef kernel_0_chip0

// Working-set KV state for one active (layer, prompt) slice.
// Persistent decode-state lives in HBM and is paged into these local URAM
// arrays per invocation so the existing attention core can stay unchanged.
static ap_int<1> chunk0_causal_mask[1][1][1024][1024];
static fxp8_t chunk0_zero_scalar[1] = {(fxp8_t)0};
static fxp8_t chunk0_hidden_local[1][1][1024];
static fxp8_t chunk0_hidden_mid_local[1][1][1024];
static fxp8_t chunk0_bypass_local[1][1][1024];
static fxp8_t chunk0_attn_bias[1][1][1][128];
static ap_int<32> chunk0_cache_position_local[1];
static packed_fxp32_t chunk0_kv_k_local_lo[1][8][128][2];
static packed_fxp32_t chunk0_kv_k_local_hi[1][8][128][2];
static packed_fxp32_t chunk0_kv_v_local_lo[1][8][128][2];
static packed_fxp32_t chunk0_kv_v_local_hi[1][8][128][2];

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
    // 8 HBM weight bank pointers — each bank holds [QKV_bk | Wout_bk].
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
    packed_fxp32_t *kv_v_hbm_hi) {
#pragma HLS interface axis port=hidden_in
#pragma HLS interface axis port=hidden_mid_out
#pragma HLS interface axis port=bypass_out
#pragma HLS interface s_axilite port=prompt_base bundle=control
#pragma HLS interface s_axilite port=num_prompts bundle=control
#pragma HLS interface s_axilite port=num_layers bundle=control
#pragma HLS interface s_axilite port=cache_position_p0 bundle=control
#pragma HLS interface s_axilite port=cache_position_p1 bundle=control
#pragma HLS interface s_axilite port=cache_position_p2 bundle=control
#pragma HLS interface s_axilite port=return bundle=control
// One 512-bit m_axi adapter per HBM PC for chunk0 weights. Metadata stays separate.
#pragma HLS interface m_axi port=b0 bundle=C0_B0 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS interface m_axi port=b1 bundle=C0_B1 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS interface m_axi port=b2 bundle=C0_B2 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS interface m_axi port=b3 bundle=C0_B3 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS interface m_axi port=b4 bundle=C0_B4 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS interface m_axi port=b5 bundle=C0_B5 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS interface m_axi port=b6 bundle=C0_B6 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS interface m_axi port=b7 bundle=C0_B7 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS interface m_axi port=meta_hbm bundle=C0_META max_read_burst_length=128 num_read_outstanding=4
#pragma HLS interface m_axi port=kv_k_hbm_lo bundle=C0_KV_K_LO max_read_burst_length=128 max_write_burst_length=128 num_read_outstanding=4 num_write_outstanding=4
#pragma HLS interface m_axi port=kv_v_hbm_lo bundle=C0_KV_V_LO max_read_burst_length=128 max_write_burst_length=128 num_read_outstanding=4 num_write_outstanding=4
#pragma HLS interface m_axi port=kv_k_hbm_hi bundle=C0_KV_K_HI max_read_burst_length=128 max_write_burst_length=128 num_read_outstanding=4 num_write_outstanding=4
#pragma HLS interface m_axi port=kv_v_hbm_hi bundle=C0_KV_V_HI max_read_burst_length=128 max_write_burst_length=128 num_read_outstanding=4 num_write_outstanding=4
#pragma HLS bind_storage variable=chunk0_kv_k_local_lo type=ram_2p impl=uram
#pragma HLS array_partition variable=chunk0_kv_k_local_lo dim=4 complete
#pragma HLS bind_storage variable=chunk0_kv_k_local_hi type=ram_2p impl=uram
#pragma HLS array_partition variable=chunk0_kv_k_local_hi dim=4 complete
#pragma HLS bind_storage variable=chunk0_kv_v_local_lo type=ram_2p impl=uram
#pragma HLS array_partition variable=chunk0_kv_v_local_lo dim=4 complete
#pragma HLS bind_storage variable=chunk0_kv_v_local_hi type=ram_2p impl=uram
#pragma HLS array_partition variable=chunk0_kv_v_local_hi dim=4 complete
  const int active_num_prompts =
      ((int)num_prompts > C0_NUM_PROMPTS) ? C0_NUM_PROMPTS : (int)num_prompts;
  const int active_num_layers =
      ((int)num_layers > C0_NUM_LAYERS) ? C0_NUM_LAYERS : (int)num_layers;
  fxp8_t qkv_bias_local[3072];
  fxp8_t wout_bias_local[1024];
  fxp8_t ln1_beta_local[1024];
  fxp8_t ln1_gamma_local[1024];
  int cached_layer = -1;
  packed_fxp64_t *b0_l = b0;
  packed_fxp64_t *b1_l = b1;
  packed_fxp64_t *b2_l = b2;
  packed_fxp64_t *b3_l = b3;
  packed_fxp64_t *b4_l = b4;
  packed_fxp64_t *b5_l = b5;
  packed_fxp64_t *b6_l = b6;
  packed_fxp64_t *b7_l = b7;
  for (int layer = 0; layer < C0_NUM_LAYERS; ++layer) {
  #pragma HLS loop_tripcount min=1 max=24
    for (int prompt = 0; prompt < C0_NUM_PROMPTS; ++prompt) {
    #pragma HLS loop_tripcount min=1 max=3
      const bool active_slot = (layer < active_num_layers) && (prompt < active_num_prompts);
      if (!active_slot)
        continue;
      const int prompt_id = (int)prompt_base + prompt;
      const int cache_position =
          (prompt_id == 0) ? (int)cache_position_p0 :
          (prompt_id == 1) ? (int)cache_position_p1 :
                             (int)cache_position_p2;
      if (layer != cached_layer) {
        packed_fxp32_t *meta_l = meta_hbm + layer * C0_META_WORDS;
        b0_l = b0 + layer * C0_BANK_WORDS;
        b1_l = b1 + layer * C0_BANK_WORDS;
        b2_l = b2 + layer * C0_BANK_WORDS;
        b3_l = b3 + layer * C0_BANK_WORDS;
        b4_l = b4 + layer * C0_BANK_WORDS;
        b5_l = b5 + layer * C0_BANK_WORDS;
        b6_l = b6 + layer * C0_BANK_WORDS;
        b7_l = b7 + layer * C0_BANK_WORDS;
        load_hbm_bias<3072>(meta_l + C0_BIAS_OFF,  qkv_bias_local);
        load_hbm_bias<1024>(meta_l + C0_WOUT_BIAS, wout_bias_local);
        load_hbm_bias<1024>(meta_l + C0_LN1_BETA,  ln1_beta_local);
        load_hbm_bias<1024>(meta_l + C0_LN1_GAMMA, ln1_gamma_local);
        cached_layer = layer;
      }
      const int kv_slice = c0_kv_slice_index((ap_uint<5>)layer, (ap_uint<2>)prompt_id);
      packed_fxp32_t *kv_k_base_lo = kv_k_hbm_lo + kv_slice * C0_KV_HALF_SLICE_WORDS;
      packed_fxp32_t *kv_v_base_lo = kv_v_hbm_lo + kv_slice * C0_KV_HALF_SLICE_WORDS;
      packed_fxp32_t *kv_k_base_hi = kv_k_hbm_hi + kv_slice * C0_KV_HALF_SLICE_WORDS;
      packed_fxp32_t *kv_v_base_hi = kv_v_hbm_hi + kv_slice * C0_KV_HALF_SLICE_WORDS;
      load_chunk0_frame_state(
          hidden_in, kv_k_base_lo, kv_k_base_hi, kv_v_base_lo, kv_v_base_hi,
          cache_position,
          chunk0_hidden_local, chunk0_attn_bias,
          chunk0_kv_k_local_lo, chunk0_kv_k_local_hi,
          chunk0_kv_v_local_lo, chunk0_kv_v_local_hi);
      update_causal_row(chunk0_causal_mask, cache_position);
      c0_debug_set_slot(layer, prompt);
      chunk0_cache_position_local[0] = cache_position;
      kernel_0_chip0_core(
          chunk0_causal_mask, chunk0_zero_scalar,
          chunk0_hidden_local, chunk0_cache_position_local, chunk0_attn_bias,
          chunk0_kv_k_local_lo, chunk0_kv_k_local_hi,
          chunk0_kv_v_local_lo, chunk0_kv_v_local_hi,
          chunk0_hidden_mid_local, chunk0_bypass_local,
          wout_bias_local,
          b0_l, b1_l, b2_l, b3_l, b4_l, b5_l, b6_l, b7_l,
          qkv_bias_local,
          ln1_gamma_local, ln1_beta_local);
      store_chunk0_frame_state(
          kv_k_base_lo, kv_k_base_hi, kv_v_base_lo, kv_v_base_hi, cache_position,
          chunk0_kv_k_local_lo, chunk0_kv_k_local_hi,
          chunk0_kv_v_local_lo, chunk0_kv_v_local_hi,
          chunk0_hidden_mid_local, chunk0_bypass_local,
          hidden_mid_out, bypass_out);
    }
  }
}
