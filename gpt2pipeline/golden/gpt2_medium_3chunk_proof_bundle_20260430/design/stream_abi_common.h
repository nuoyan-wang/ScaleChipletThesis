#pragma once

#include <ap_int.h>
#include <ap_fixed.h>
#include <hls_stream.h>

using axis256_t = ap_uint<256>;
using fxp8_t = ap_fixed<8, 4>;
using packed_fxp32_t = ap_uint<256>;

static fxp8_t fxp8_from_bits(ap_uint<8> bits) {
  fxp8_t value;
  value.range(7, 0) = bits;
  return value;
}

static ap_uint<8> fxp8_to_bits(fxp8_t value) {
  return value.range(7, 0);
}

template <int N>
static void load_axis_tensor_1x1xN(hls::stream<axis256_t> &in, fxp8_t tensor[1][1][N]) {
  for (int beat = 0; beat < N / 32; ++beat) {
  #pragma HLS pipeline II=1
    axis256_t word = in.read();
    for (int lane = 0; lane < 32; ++lane) {
    #pragma HLS unroll
      tensor[0][0][beat * 32 + lane] = fxp8_from_bits(word.range(lane * 8 + 7, lane * 8));
    }
  }
}

template <int N>
static void store_axis_tensor_1x1xN(fxp8_t tensor[1][1][N], hls::stream<axis256_t> &out) {
  for (int beat = 0; beat < N / 32; ++beat) {
  #pragma HLS pipeline II=1
    axis256_t word = 0;
    for (int lane = 0; lane < 32; ++lane) {
    #pragma HLS unroll
      word.range(lane * 8 + 7, lane * 8) = fxp8_to_bits(tensor[0][0][beat * 32 + lane]);
    }
    out.write(word);
  }
}

template <int N>
static void store_axis_vector_N(fxp8_t tensor[N], hls::stream<axis256_t> &out) {
  for (int beat = 0; beat < N / 32; ++beat) {
  #pragma HLS pipeline II=1
    axis256_t word = 0;
    for (int lane = 0; lane < 32; ++lane) {
    #pragma HLS unroll
      word.range(lane * 8 + 7, lane * 8) = fxp8_to_bits(tensor[beat * 32 + lane]);
    }
    out.write(word);
  }
}

static void zero_attn_bias(fxp8_t attn_bias[1][1][1][128]) {
  for (int i = 0; i < 128; ++i) {
  #pragma HLS pipeline II=1
    attn_bias[0][0][0][i] = (fxp8_t)0;
  }
}

static void update_causal_row(ap_int<1> causal_mask[1][1][1024][1024], ap_uint<8> cache_position) {
  for (int i = 0; i < 128; ++i) {
  #pragma HLS pipeline II=1
    causal_mask[0][0][127][i] = (i <= (int)cache_position) ? 1 : 0;
  }
}

static void copy_kv_bank(
    packed_fxp32_t src[1][16][128][2],
    packed_fxp32_t dst[1][16][128][2]) {
  for (int h = 0; h < 16; ++h) {
    for (int t = 0; t < 128; ++t) {
      for (int d = 0; d < 2; ++d) {
      #pragma HLS pipeline II=1
        dst[0][h][t][d] = src[0][h][t][d];
      }
    }
  }
}

// Load N fxp8 values from an HBM m_axi pointer (packed 32-per-beat) into a local array.
// N must be a multiple of 32.
template <int N>
static void load_hbm_bias(const packed_fxp32_t *src, fxp8_t dst[N]) {
  for (int beat = 0; beat < N / 32; ++beat) {
  #pragma HLS pipeline II=1
    packed_fxp32_t word = src[beat];
    for (int lane = 0; lane < 32; ++lane) {
    #pragma HLS unroll
      dst[beat * 32 + lane] = fxp8_from_bits(word.range(lane * 8 + 7, lane * 8));
    }
  }
}
