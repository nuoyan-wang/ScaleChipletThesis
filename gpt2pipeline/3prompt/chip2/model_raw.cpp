
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

void dataflow_node_0(ap_fixed<32, 8> arg0000[1][1][1024]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)1) {
    arg0000[(int)0][(int)0][v0000] = (ap_fixed<32, 8>)0.000000;
  }
  return;
}

void dataflow_node_1(ap_fixed<8, 4> arg0000[1][1][4096], ap_fixed<8, 4> arg0001[4096][1024], ap_fixed<32, 8> arg0002[1][1][1024]) {
#pragma HLS inline off
  ap_fixed<32, 8> v0000[2];
  for (int v0001 = (int)0; v0001 < (int)1024; v0001 += (int)1) {
    for (int v0002 = (int)0; v0002 < (int)2; v0002 += (int)1) {
      v0000[v0002] = (ap_fixed<32, 8>)0.000000;
    }
    for (int v0003 = (int)0; v0003 < (int)4096; v0003 += (int)1) {
      ap_fixed<8, 4> v0004 = arg0000[(int)0][(int)0][v0003];
      ap_fixed<8, 4> v0005 = arg0001[v0003][v0001];
      int v0006 = v0003 % (int)4;
      ap_int<1> v0007 = v0006 < (int)0;
      int v0008 = v0006 + (int)4;
      int v0009 = v0007 ? v0008 : v0006;
      ap_fixed<32, 8> v0010 = v0000[v0009];
      ap_fixed<32, 8> v0011 = (ap_fixed<32, 8>)v0004;
      ap_fixed<32, 8> v0012 = (ap_fixed<32, 8>)v0005;
      ap_fixed<32, 8> v0013 = v0011 * v0012;
      ap_fixed<32, 8> v0014 = v0010 + v0013;
      int v0015 = v0003 % (int)4;
      ap_int<1> v0016 = v0015 < (int)0;
      int v0017 = v0015 + (int)4;
      int v0018 = v0016 ? v0017 : v0015;
      v0000[v0018] = v0014;
    }
    ap_fixed<32, 8> v0019 = arg0002[(int)0][(int)0][v0001];
    ap_fixed<32, 8> v0020 = v0000[(int)0];
    ap_fixed<32, 8> v0021 = v0000[(int)1];
    ap_fixed<32, 8> v0022 = v0020 + v0021;
    ap_fixed<32, 8> v0023 = v0022 + v0019;
    arg0002[(int)0][(int)0][v0001] = v0023;
  }
  return;
}

void dataflow_node_2(ap_fixed<32, 8> arg0000[1][1][1024], ap_fixed<8, 4> arg0001[1024]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)1) {
    int v0001 = v0000 / (int)1024;
    int v0002 = v0001 * (int)1024;
    ap_int<1> v0003 = v0000 != v0002;
    ap_int<1> v0004 = v0000 < (int)0;
    ap_int<1> v0005 = v0003 & v0004;
    int v0006 = v0001 + (int)-1;
    int v0007 = v0005 ? v0006 : v0001;
    int v0008 = v0000 % (int)1024;
    ap_int<1> v0009 = v0008 < (int)0;
    int v0010 = v0008 + (int)1024;
    int v0011 = v0009 ? v0010 : v0008;
    int v0012 = v0011 / (int)1024;
    int v0013 = v0000 % (int)1024;
    ap_int<1> v0014 = v0013 < (int)0;
    int v0015 = v0013 + (int)1024;
    int v0016 = v0014 ? v0015 : v0013;
    ap_fixed<32, 8> v0017 = arg0000[v0007][v0012][v0016];
    ap_fixed<8, 4> v0018 = (ap_fixed<8, 4>)v0017;
    arg0001[v0000] = v0018;
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
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)1) {
    ap_fixed<8, 4> v0001 = arg0000[v0000];
    arg0001[(int)0][(int)0][v0000] = v0001;
  }
  return;
}

void kernel_0_chip2(ap_fixed<8, 4> arg0000[1][1][4096], ap_fixed<8, 4> arg0001[1][1][1024], ap_fixed<8, 4> arg0002[1][1][1024], ap_fixed<8, 4> arg0003[1024], ap_fixed<8, 4> arg0004[4096][1024]) {
#pragma HLS interface s_axilite port=return bundle=control
#pragma HLS interface m_axi port=arg0000 offset=slave bundle=gmem0
#pragma HLS interface s_axilite port=arg0000 bundle=control
#pragma HLS interface m_axi port=arg0001 offset=slave bundle=gmem0
#pragma HLS interface s_axilite port=arg0001 bundle=control
#pragma HLS interface m_axi port=arg0002 offset=slave bundle=gmem0
#pragma HLS interface s_axilite port=arg0002 bundle=control
#pragma HLS array_partition variable=arg0003 dim=1 cyclic factor=32
#pragma HLS interface m_axi port=arg0003 offset=slave bundle=gmem0
#pragma HLS interface s_axilite port=arg0003 bundle=control
#pragma HLS interface m_axi port=arg0004 offset=slave bundle=gmem0
#pragma HLS interface s_axilite port=arg0004 bundle=control
  ap_fixed<32, 8> v0000[1][1][1024];
  // #pragma HLS bind_storage variable=v0000 type=ram_2p impl=bram latency=2
  dataflow_node_0(v0000);
  dataflow_node_1(arg0000, arg0004, v0000);
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

