#define kernel_0_chip2 kernel_0_chip2_core
#include "../stream_abi_common.h"

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

using packed_fxp64_t = ap_uint<512>;

void shared_kernel_0(ap_fixed<8, 4> arg0000[32], ap_fixed<8, 4> arg0001[32], ap_fixed<8, 4> arg0002[32]) {
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

void dataflow_node_0(ap_fixed<8, 4> arg0000[1][1][1024]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)1) {
    arg0000[(int)0][(int)0][v0000] = (ap_fixed<8, 4>)0.000000;
  }
  return;
}

static void shared_kernel_fc2_8x64(
    ap_fixed<8, 4> arg0000[1][1][8],
    ap_fixed<8, 4> arg0001[1][8][64],
    ap_fixed<32, 8> arg0002[1][1][64]);

static void shared_kernel_fc2_8x32(
    ap_fixed<8, 4> arg0000[1][1][8],
    ap_fixed<8, 4> arg0001[1][8][32],
    ap_fixed<8, 4> arg0002[1][1][32]);

// dataflow_node_1: FC2 GEMM, 8-bank HBM streaming.
// Weight layout: fc2_bk[rg*16+col] holds rows where row%8==k, rg=row/8.
// Each 512-bit beat packs two adjacent 32-lane output groups. To match the
// monolithic kernel exactly, unpack the beat into two 8x32 tiles and update
// each half with the same 8x32 reduction semantics.
void dataflow_node_1(
    ap_fixed<8, 4> arg0000[1][1][4096],
    packed_fxp64_t *fc2_b0, packed_fxp64_t *fc2_b1,
    packed_fxp64_t *fc2_b2, packed_fxp64_t *fc2_b3,
    packed_fxp64_t *fc2_b4, packed_fxp64_t *fc2_b5,
    packed_fxp64_t *fc2_b6, packed_fxp64_t *fc2_b7,
    ap_fixed<8, 4> arg0002[1][1][1024]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=8
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=64
  for (int rg = 0; rg < 512; rg++) {   // 4096/8 = 512 row groups
  #pragma HLS dependence variable=arg0002 type=inter direction=RAW dependent=false
    for (int col = 0; col < 16; col++) {  // 1024/64 = 16 output col groups
    #pragma HLS pipeline II=1
      packed_fxp64_t w0 = fc2_b0[rg * 16 + col];
      packed_fxp64_t w1 = fc2_b1[rg * 16 + col];
      packed_fxp64_t w2 = fc2_b2[rg * 16 + col];
      packed_fxp64_t w3 = fc2_b3[rg * 16 + col];
      packed_fxp64_t w4 = fc2_b4[rg * 16 + col];
      packed_fxp64_t w5 = fc2_b5[rg * 16 + col];
      packed_fxp64_t w6 = fc2_b6[rg * 16 + col];
      packed_fxp64_t w7 = fc2_b7[rg * 16 + col];
      ap_fixed<8, 4> v0002[1][1][8];
      #pragma HLS array_partition variable=v0002 dim=3 complete
      for (int k = 0; k < 8; k++) {
      #pragma HLS unroll
        v0002[0][0][k] = arg0000[0][0][rg * 8 + k];
      }
      ap_fixed<8, 4> v0004_lo[1][8][32];
      ap_fixed<8, 4> v0004_hi[1][8][32];
      #pragma HLS array_partition variable=v0004_lo dim=2 complete
      #pragma HLS array_partition variable=v0004_lo dim=3 complete
      #pragma HLS array_partition variable=v0004_hi dim=2 complete
      #pragma HLS array_partition variable=v0004_hi dim=3 complete
      packed_fxp64_t wbanks[8];
      #pragma HLS array_partition variable=wbanks complete
      wbanks[0] = w0; wbanks[1] = w1; wbanks[2] = w2; wbanks[3] = w3;
      wbanks[4] = w4; wbanks[5] = w5; wbanks[6] = w6; wbanks[7] = w7;
      for (int k = 0; k < 8; k++) {
      #pragma HLS unroll
        for (int j = 0; j < 32; j++) {
        #pragma HLS unroll
          v0004_lo[0][k][j].range(7, 0) = wbanks[k].range(j * 8 + 7, j * 8);
          v0004_hi[0][k][j].range(7, 0) = wbanks[k].range((j + 32) * 8 + 7, (j + 32) * 8);
        }
      }
      ap_fixed<8, 4> v0010_lo[1][1][32];
      ap_fixed<8, 4> v0010_hi[1][1][32];
      #pragma HLS array_partition variable=v0010_lo dim=3 complete
      #pragma HLS array_partition variable=v0010_hi dim=3 complete
      for (int j = 0; j < 32; j++) {
      #pragma HLS unroll
        v0010_lo[0][0][j] = arg0002[0][0][col * 64 + j];
        v0010_hi[0][0][j] = arg0002[0][0][col * 64 + 32 + j];
      }
      shared_kernel_fc2_8x32(v0002, v0004_lo, v0010_lo);
      shared_kernel_fc2_8x32(v0002, v0004_hi, v0010_hi);
      for (int j = 0; j < 32; j++) {
      #pragma HLS unroll
        arg0002[0][0][col * 64 + j] = v0010_lo[0][0][j];
        arg0002[0][0][col * 64 + 32 + j] = v0010_hi[0][0][j];
      }
    }
  }
  return;
}

void dataflow_node_2(ap_fixed<8, 4> arg0000[1][1][1024], ap_fixed<8, 4> arg0001[1024]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)32) {
  #pragma HLS pipeline II=1
    for (int v0001 = (int)0; v0001 < (int)32; v0001 += (int)1) {
    #pragma HLS unroll factor=32
      int v0002 = v0000 + v0001;
      arg0001[v0002] = arg0000[(int)0][(int)0][v0002];
    }
  }
  return;
}

void dataflow_node_3(ap_fixed<8, 4> arg0000[1024], ap_fixed<8, 4> arg0001[1024], ap_fixed<8, 4> arg0002[1024]) {
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

void dataflow_node_4(ap_fixed<8, 4> arg0000[1][1][1024], ap_fixed<8, 4> arg0001[1024], ap_fixed<8, 4> arg0002[1024]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=1 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)32) {
  #pragma HLS pipeline II=1
    ap_fixed<8, 4> v0001[32];
    #pragma HLS array_partition variable=v0001 dim=1 cyclic factor=32
    for (int v0002 = (int)0; v0002 < (int)32; v0002 += (int)1) {
    #pragma HLS unroll factor=32
      int v0003 = v0000 + v0002;
      int v0004 = v0003 / (int)1024;
      int v0005 = v0004 * (int)1024;
      ap_int<1> v0006 = v0003 != v0005;
      ap_int<1> v0007 = v0003 < (int)0;
      ap_int<1> v0008 = v0006 & v0007;
      int v0009 = v0004 + (int)-1;
      int v0010 = v0008 ? v0009 : v0004;
      int v0011 = v0003 % (int)1024;
      ap_int<1> v0012 = v0011 < (int)0;
      int v0013 = v0011 + (int)1024;
      int v0014 = v0012 ? v0013 : v0011;
      int v0015 = v0014 / (int)1024;
      int v0016 = v0003 % (int)1024;
      ap_int<1> v0017 = v0016 < (int)0;
      int v0018 = v0016 + (int)1024;
      int v0019 = v0017 ? v0018 : v0016;
      ap_fixed<8, 4> v0020 = arg0000[v0010][v0015][v0019];
      v0001[v0002] = v0020;
    }
    ap_fixed<8, 4> v0021[32];
    #pragma HLS array_partition variable=v0021 dim=1 cyclic factor=32
    for (int v0022 = (int)0; v0022 < (int)32; v0022 += (int)1) {
    #pragma HLS unroll factor=32
      int v0023 = v0000 + v0022;
      ap_fixed<8, 4> v0024 = arg0001[v0023];
      v0021[v0022] = v0024;
    }
    ap_fixed<8, 4> v0025[32];
    #pragma HLS array_partition variable=v0025 dim=1 cyclic factor=32
    for (int v0026 = (int)0; v0026 < (int)32; v0026 += (int)1) {
    #pragma HLS unroll factor=32
      int v0027 = v0000 + v0026;
      ap_fixed<8, 4> v0028 = arg0002[v0027];
      v0025[v0026] = v0028;
    }
    shared_kernel_0(v0001, v0021, v0025);
    for (int v0029 = (int)0; v0029 < (int)32; v0029 += (int)1) {
    #pragma HLS unroll factor=32
      ap_fixed<8, 4> v0030 = v0025[v0029];
      int v0031 = v0000 + v0029;
      arg0002[v0031] = v0030;
    }
  }
  return;
}

void dataflow_node_5(ap_fixed<8, 4> arg0000[1024], ap_fixed<8, 4> arg0001[1][1][1024]) {
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

void shared_kernel_fc2(
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
      ap_fixed<16, 8> v0008_prod = v0004 * v0005;
      #pragma HLS BIND_OP variable=v0008_prod op=mul impl=dsp
      ap_fixed<32, 8> v0008 = v0008_prod;
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

void shared_kernel_fc2_8x32(
    ap_fixed<8, 4> arg0000[1][1][8],
    ap_fixed<8, 4> arg0001[1][8][32],
    ap_fixed<8, 4> arg0002[1][1][32]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=8
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=8
#pragma HLS array_partition variable=arg0001 dim=3 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=32
  ap_fixed<8, 4> v0000[4];
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
      int v0006 = v0003 % (int)4;
      ap_int<1> v0007 = v0006 < (int)0;
      int v0008 = v0006 + (int)4;
      int v0009 = v0007 ? v0008 : v0006;
      ap_fixed<8, 4> v0010 = v0000[v0009];
      ap_fixed<8, 4> v0011 = v0004 * v0005;
      #pragma HLS BIND_OP variable=v0011 op=mul impl=dsp
      v0000[v0009] = v0010 + v0011;
    }
    ap_fixed<8, 4> v0012 = arg0002[(int)0][(int)0][v0001];
    ap_fixed<8, 4> v0013 = v0000[(int)0] + v0000[(int)1];
    ap_fixed<8, 4> v0014 = v0000[(int)2] + v0000[(int)3];
    ap_fixed<8, 4> v0015 = v0013 + v0014;
    arg0002[(int)0][(int)0][v0001] = v0015 + v0012;
  }
  return;
}

// 8-input × 32-output MAC kernel for FC2 GEMM (bank-oriented, 4 accumulators).
void shared_kernel_fc2_8x64(
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
  for (int v0001 = (int)0; v0001 < (int)64; v0001 += (int)1) {
  #pragma HLS unroll
    for (int v0002 = (int)0; v0002 < (int)4; v0002 += (int)1) {
    #pragma HLS unroll factor=4
      v0000[v0002] = (ap_fixed<32, 8>)0.000000;
    }
    for (int v0003 = (int)0; v0003 < (int)8; v0003 += (int)1) {
    #pragma HLS unroll factor=8
      ap_fixed<8, 4> v0004 = arg0000[(int)0][(int)0][v0003];
      ap_fixed<8, 4> v0005 = arg0001[(int)0][v0003][v0001];
      int v0006 = v0003 & (int)3;
      ap_fixed<8, 4> v0007 = v0000[v0006];
      ap_fixed<8, 4> v0008 = v0004 * v0005;
      #pragma HLS BIND_OP variable=v0008 op=mul impl=dsp
      v0000[v0006] = v0007 + v0008;
    }
    ap_fixed<8, 4> v0009 = arg0002[(int)0][(int)0][v0001];
    ap_fixed<8, 4> v0010 = v0000[(int)0] + v0000[(int)1];
    ap_fixed<8, 4> v0011 = v0000[(int)2] + v0000[(int)3];
    ap_fixed<8, 4> v0012 = v0010 + v0011;
    arg0002[(int)0][(int)0][v0001] = v0012 + v0009;
  }
  return;
}

void kernel_0_chip2(
    ap_fixed<8, 4> arg0000[1][1][4096],
    ap_fixed<8, 4> arg0001[1][1][1024],
    ap_fixed<8, 4> arg0002[1][1][1024],
    ap_fixed<8, 4> arg0003[1024],
    packed_fxp64_t *fc2_b0, packed_fxp64_t *fc2_b1,
    packed_fxp64_t *fc2_b2, packed_fxp64_t *fc2_b3,
    packed_fxp64_t *fc2_b4, packed_fxp64_t *fc2_b5,
    packed_fxp64_t *fc2_b6, packed_fxp64_t *fc2_b7) {
#pragma HLS interface s_axilite port=return bundle=control
#pragma HLS interface ap_memory port=arg0000
#pragma HLS interface ap_memory port=arg0001
#pragma HLS interface ap_memory port=arg0002
#pragma HLS array_partition variable=arg0003 dim=1 cyclic factor=32
#pragma HLS interface ap_memory port=arg0003
  ap_fixed<8, 4> v0000[1][1][1024];
  // #pragma HLS bind_storage variable=v0000 type=ram_2p impl=bram latency=2
  dataflow_node_0(v0000);
  dataflow_node_1(arg0000, fc2_b0, fc2_b1, fc2_b2, fc2_b3,
                           fc2_b4, fc2_b5, fc2_b6, fc2_b7, v0000);
  ap_fixed<8, 4> v0001[1024];
  #pragma HLS array_partition variable=v0001 dim=1 cyclic factor=32
  dataflow_node_2(v0000, v0001);
  ap_fixed<8, 4> v0002[1024];
  #pragma HLS array_partition variable=v0002 dim=1 cyclic factor=32
  dataflow_node_3(v0001, arg0003, v0002);
  dataflow_node_4(arg0001, v0002, v0001);
  dataflow_node_5(v0001, arg0002);
  return;
}
#undef kernel_0_chip2

static void load_chunk2_frame_state(
    hls::stream<axis256_t> &gelu_in,
    hls::stream<axis256_t> &bypass_in,
    fxp8_t gelu_local[1][1][4096],
    fxp8_t bypass_local[1][1][1024]) {
#pragma HLS dataflow
  load_axis_tensor_1x1xN<4096>(gelu_in, gelu_local);
  load_axis_tensor_1x1xN<1024>(bypass_in, bypass_local);
}

void chunk2_stream_ip(
    hls::stream<axis256_t> &gelu_in,
    hls::stream<axis256_t> &bypass_in,
    hls::stream<axis256_t> &hidden_out,
    ap_uint<2> num_prompts,
    ap_uint<8> num_layers,
    // Bundle C2_B0: fc2_b0 weights widened to 64 fxp8 values per beat.
    packed_fxp64_t *fc2_b0,   // [512*16] = 8192 entries, HBM PC for chunk2
    packed_fxp32_t *fc2_bias, // [1024/32=32] bias packed on C2_B0
    // Bundles C2_B1..C2_B7: remaining weight banks
    packed_fxp64_t *fc2_b1,
    packed_fxp64_t *fc2_b2,
    packed_fxp64_t *fc2_b3,
    packed_fxp64_t *fc2_b4,
    packed_fxp64_t *fc2_b5,
    packed_fxp64_t *fc2_b6,
    packed_fxp64_t *fc2_b7) {
#pragma HLS interface axis port=gelu_in
#pragma HLS interface axis port=bypass_in
#pragma HLS interface axis port=hidden_out
#pragma HLS interface s_axilite port=num_prompts bundle=control
#pragma HLS interface s_axilite port=num_layers bundle=control
#pragma HLS interface s_axilite port=return bundle=control
// Bundle C2_B0: widened bank 0 weights plus bias.
#pragma HLS interface m_axi port=fc2_b0   bundle=C2_B0 max_read_burst_length=32 num_read_outstanding=4
#pragma HLS interface m_axi port=fc2_bias bundle=C2_B0 max_read_burst_length=32 num_read_outstanding=4
// Bundles C2_B1..C2_B7: widened FC2 weight banks only, uniform config.
#pragma HLS interface m_axi port=fc2_b1   bundle=C2_B1 max_read_burst_length=32 num_read_outstanding=4
#pragma HLS interface m_axi port=fc2_b2   bundle=C2_B2 max_read_burst_length=32 num_read_outstanding=4
#pragma HLS interface m_axi port=fc2_b3   bundle=C2_B3 max_read_burst_length=32 num_read_outstanding=4
#pragma HLS interface m_axi port=fc2_b4   bundle=C2_B4 max_read_burst_length=32 num_read_outstanding=4
#pragma HLS interface m_axi port=fc2_b5   bundle=C2_B5 max_read_burst_length=32 num_read_outstanding=4
#pragma HLS interface m_axi port=fc2_b6   bundle=C2_B6 max_read_burst_length=32 num_read_outstanding=4
#pragma HLS interface m_axi port=fc2_b7   bundle=C2_B7 max_read_burst_length=32 num_read_outstanding=4

  fxp8_t gelu_local[1][1][4096];
  fxp8_t bypass_local[1][1][1024];
  fxp8_t hidden_local[1][1][1024];
  fxp8_t fc2_bias_local[1024];
  const int active_num_prompts =
      ((int)num_prompts > 3) ? 3 : (int)num_prompts;
  const int active_num_layers =
      ((int)num_layers > 24) ? 24 : (int)num_layers;
  for (int layer = 0; layer < 24; ++layer) {
  #pragma HLS loop_tripcount min=1 max=24
    // Layer-strided pointers and bias load shared across all prompts.
    packed_fxp64_t *fc2_b0_l = fc2_b0 + layer * 8192;
    packed_fxp64_t *fc2_b1_l = fc2_b1 + layer * 8192;
    packed_fxp64_t *fc2_b2_l = fc2_b2 + layer * 8192;
    packed_fxp64_t *fc2_b3_l = fc2_b3 + layer * 8192;
    packed_fxp64_t *fc2_b4_l = fc2_b4 + layer * 8192;
    packed_fxp64_t *fc2_b5_l = fc2_b5 + layer * 8192;
    packed_fxp64_t *fc2_b6_l = fc2_b6 + layer * 8192;
    packed_fxp64_t *fc2_b7_l = fc2_b7 + layer * 8192;
    if (layer < active_num_layers)
      load_hbm_bias<1024>(fc2_bias + layer * 32, fc2_bias_local);
    for (int prompt = 0; prompt < 3; ++prompt) {
    #pragma HLS loop_tripcount min=1 max=3
      const bool active_slot = (layer < active_num_layers) && (prompt < active_num_prompts);
      if (active_slot) {
        load_chunk2_frame_state(gelu_in, bypass_in, gelu_local, bypass_local);
        kernel_0_chip2_core(gelu_local, bypass_local, hidden_local, fc2_bias_local,
                            fc2_b0_l, fc2_b1_l, fc2_b2_l, fc2_b3_l,
                            fc2_b4_l, fc2_b5_l, fc2_b6_l, fc2_b7_l);
        store_axis_tensor_1x1xN<1024>(hidden_local, hidden_out);
      }
    }
  }
}
