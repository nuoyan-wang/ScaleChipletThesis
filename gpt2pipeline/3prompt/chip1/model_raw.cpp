
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

void shared_kernel_2(ap_fixed<8, 4> arg0000[32], ap_fixed<8, 4> arg0001[1], ap_fixed<8, 4> arg0002[32]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=1 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)32; v0000 += (int)1) {
  #pragma HLS unroll factor=32
    ap_fixed<8, 4> v0001 = arg0000[v0000];
    ap_fixed<8, 4> v0002 = arg0001[0];
    ap_fixed<8, 4> v0003 = v0001 * v0002;
    arg0002[v0000] = v0003;
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
  ap_fixed<32, 8> v0000[2];
  for (int v0001 = (int)0; v0001 < (int)2; v0001 += (int)1) {
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
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)1) {
    ap_fixed<8, 4> v0001 = arg0000[0];
    arg0001[v0000] = v0001;
  }
  return;
}

void dataflow_node_5(ap_fixed<8, 4> arg0000[1][1][1024], ap_fixed<8, 4> arg0001[1024], ap_fixed<8, 4> arg0002[1024]) {
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
    ap_fixed<8, 4> v0017 = arg0000[v0007][v0012][v0016];
    ap_fixed<8, 4> v0018 = arg0001[v0000];
    ap_fixed<8, 4> v0019 = v0017 - v0018;
    arg0002[v0000] = v0019;
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

void dataflow_node_7(ap_fixed<32, 8> arg0000[1][1]) {
#pragma HLS inline off
  arg0000[(int)0][(int)0] = (ap_fixed<32, 8>)0.000000;
  return;
}

void dataflow_node_8(ap_fixed<8, 4> arg0000[1024], ap_fixed<32, 8> arg0001[1][1]) {
#pragma HLS inline off
  ap_fixed<32, 8> v0000[2];
  for (int v0001 = (int)0; v0001 < (int)2; v0001 += (int)1) {
    v0000[v0001] = (ap_fixed<32, 8>)0.000000;
  }
  for (int v0002 = (int)0; v0002 < (int)1024; v0002 += (int)1) {
    ap_fixed<8, 4> v0003 = arg0000[v0002];
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

void dataflow_node_9(ap_fixed<32, 8> arg0000[1][1], ap_fixed<8, 4> arg0001[1]) {
#pragma HLS inline off
  ap_fixed<32, 8> v0000 = arg0000[(int)0][(int)0];
  ap_fixed<8, 4> v0001 = (ap_fixed<8, 4>)v0000;
  arg0001[0] = v0001;
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
  ap_fixed<8, 4> v0001 = hls::rsqrt((float)v0000);
  arg0001[0] = v0001;
  return;
}

void dataflow_node_13(ap_fixed<8, 4> arg0000[1], ap_fixed<8, 4> arg0001[1024]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)1) {
    ap_fixed<8, 4> v0001 = arg0000[0];
    arg0001[v0000] = v0001;
  }
  return;
}

void dataflow_node_14(ap_fixed<8, 4> arg0000[1024], ap_fixed<8, 4> arg0001[1024], ap_fixed<8, 4> arg0002[1024]) {
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

void dataflow_node_15(ap_fixed<8, 4> arg0000[1024], ap_fixed<8, 4> arg0001[1024], ap_fixed<8, 4> arg0002[1024]) {
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

void dataflow_node_16(ap_fixed<8, 4> arg0000[1024], ap_fixed<8, 4> arg0001[1024], ap_fixed<8, 4> arg0002[1024]) {
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

void dataflow_node_17(ap_fixed<32, 8> arg0000[1][1][4096]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)4096; v0000 += (int)1) {
    arg0000[(int)0][(int)0][v0000] = (ap_fixed<32, 8>)0.000000;
  }
  return;
}

void dataflow_node_18(ap_fixed<8, 4> arg0000[1024], ap_fixed<8, 4> arg0001[1024][4096], ap_fixed<32, 8> arg0002[1][1][4096]) {
#pragma HLS inline off
  ap_fixed<32, 8> v0000[2];
  for (int v0001 = (int)0; v0001 < (int)4096; v0001 += (int)1) {
    for (int v0002 = (int)0; v0002 < (int)2; v0002 += (int)1) {
      v0000[v0002] = (ap_fixed<32, 8>)0.000000;
    }
    for (int v0003 = (int)0; v0003 < (int)1024; v0003 += (int)1) {
      ap_fixed<8, 4> v0004 = arg0000[v0003];
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

void dataflow_node_19(ap_fixed<32, 8> arg0000[1][1][4096], ap_fixed<8, 4> arg0001[4096]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)4096; v0000 += (int)1) {
    int v0001 = v0000 / (int)4096;
    int v0002 = v0001 * (int)4096;
    ap_int<1> v0003 = v0000 != v0002;
    ap_int<1> v0004 = v0000 < (int)0;
    ap_int<1> v0005 = v0003 & v0004;
    int v0006 = v0001 + (int)-1;
    int v0007 = v0005 ? v0006 : v0001;
    int v0008 = v0000 % (int)4096;
    ap_int<1> v0009 = v0008 < (int)0;
    int v0010 = v0008 + (int)4096;
    int v0011 = v0009 ? v0010 : v0008;
    int v0012 = v0011 / (int)4096;
    int v0013 = v0000 % (int)4096;
    ap_int<1> v0014 = v0013 < (int)0;
    int v0015 = v0013 + (int)4096;
    int v0016 = v0014 ? v0015 : v0013;
    ap_fixed<32, 8> v0017 = arg0000[v0007][v0012][v0016];
    ap_fixed<8, 4> v0018 = (ap_fixed<8, 4>)v0017;
    arg0001[v0000] = v0018;
  }
  return;
}

void dataflow_node_20(ap_fixed<8, 4> arg0000[4096], ap_fixed<8, 4> arg0001[4096], ap_fixed<8, 4> arg0002[4096]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=1 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)4096; v0000 += (int)32) {
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

void dataflow_node_21(ap_fixed<8, 4> arg0000[4096], ap_fixed<8, 4> arg0001[4096]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
  ap_fixed<8, 4> v0000[1] = {5.000000e-01};
  for (int v0001 = (int)0; v0001 < (int)4096; v0001 += (int)32) {
  #pragma HLS pipeline II=1
    ap_fixed<8, 4> v0002[32];
    #pragma HLS array_partition variable=v0002 dim=1 cyclic factor=32
    for (int v0003 = (int)0; v0003 < (int)32; v0003 += (int)1) {
    #pragma HLS unroll factor=32
      int v0004 = v0001 + v0003;
      ap_fixed<8, 4> v0005 = arg0000[v0004];
      v0002[v0003] = v0005;
    }
    ap_fixed<8, 4> v0006[32];
    #pragma HLS array_partition variable=v0006 dim=1 cyclic factor=32
    for (int v0007 = (int)0; v0007 < (int)32; v0007 += (int)1) {
    #pragma HLS unroll factor=32
      int v0008 = v0001 + v0007;
      ap_fixed<8, 4> v0009 = arg0001[v0008];
      v0006[v0007] = v0009;
    }
    shared_kernel_2(v0002, v0000, v0006);
    for (int v0010 = (int)0; v0010 < (int)32; v0010 += (int)1) {
    #pragma HLS unroll factor=32
      ap_fixed<8, 4> v0011 = v0006[v0010];
      int v0012 = v0001 + v0010;
      arg0001[v0012] = v0011;
    }
  }
  return;
}

void dataflow_node_22(ap_fixed<8, 4> arg0000[4096], ap_fixed<8, 4> arg0001[4096]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)4096; v0000 += (int)1) {
    ap_fixed<8, 4> v0001 = arg0000[v0000];
    ap_fixed<8, 4> v0002 = hls::pow((float)v0001, (float)(ap_fixed<8, 4>)3.000000);
    arg0001[v0000] = v0002;
  }
  return;
}

void dataflow_node_23(ap_fixed<8, 4> arg0000[4096], ap_fixed<8, 4> arg0001[4096]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
  ap_fixed<8, 4> v0000[1] = {4.296875e-02};
  for (int v0001 = (int)0; v0001 < (int)4096; v0001 += (int)32) {
  #pragma HLS pipeline II=1
    ap_fixed<8, 4> v0002[32];
    #pragma HLS array_partition variable=v0002 dim=1 cyclic factor=32
    for (int v0003 = (int)0; v0003 < (int)32; v0003 += (int)1) {
    #pragma HLS unroll factor=32
      int v0004 = v0001 + v0003;
      ap_fixed<8, 4> v0005 = arg0000[v0004];
      v0002[v0003] = v0005;
    }
    ap_fixed<8, 4> v0006[32];
    #pragma HLS array_partition variable=v0006 dim=1 cyclic factor=32
    for (int v0007 = (int)0; v0007 < (int)32; v0007 += (int)1) {
    #pragma HLS unroll factor=32
      int v0008 = v0001 + v0007;
      ap_fixed<8, 4> v0009 = arg0001[v0008];
      v0006[v0007] = v0009;
    }
    shared_kernel_2(v0002, v0000, v0006);
    for (int v0010 = (int)0; v0010 < (int)32; v0010 += (int)1) {
    #pragma HLS unroll factor=32
      ap_fixed<8, 4> v0011 = v0006[v0010];
      int v0012 = v0001 + v0010;
      arg0001[v0012] = v0011;
    }
  }
  return;
}

void dataflow_node_24(ap_fixed<8, 4> arg0000[4096], ap_fixed<8, 4> arg0001[4096], ap_fixed<8, 4> arg0002[4096]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=1 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)4096; v0000 += (int)32) {
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

void dataflow_node_25(ap_fixed<8, 4> arg0000[4096], ap_fixed<8, 4> arg0001[4096]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
  ap_fixed<8, 4> v0000[1] = {8.125000e-01};
  for (int v0001 = (int)0; v0001 < (int)4096; v0001 += (int)32) {
  #pragma HLS pipeline II=1
    ap_fixed<8, 4> v0002[32];
    #pragma HLS array_partition variable=v0002 dim=1 cyclic factor=32
    for (int v0003 = (int)0; v0003 < (int)32; v0003 += (int)1) {
    #pragma HLS unroll factor=32
      int v0004 = v0001 + v0003;
      ap_fixed<8, 4> v0005 = arg0000[v0004];
      v0002[v0003] = v0005;
    }
    ap_fixed<8, 4> v0006[32];
    #pragma HLS array_partition variable=v0006 dim=1 cyclic factor=32
    for (int v0007 = (int)0; v0007 < (int)32; v0007 += (int)1) {
    #pragma HLS unroll factor=32
      int v0008 = v0001 + v0007;
      ap_fixed<8, 4> v0009 = arg0001[v0008];
      v0006[v0007] = v0009;
    }
    shared_kernel_2(v0002, v0000, v0006);
    for (int v0010 = (int)0; v0010 < (int)32; v0010 += (int)1) {
    #pragma HLS unroll factor=32
      ap_fixed<8, 4> v0011 = v0006[v0010];
      int v0012 = v0001 + v0010;
      arg0001[v0012] = v0011;
    }
  }
  return;
}

void dataflow_node_26(ap_fixed<8, 4> arg0000[4096], ap_fixed<8, 4> arg0001[4096]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)4096; v0000 += (int)1) {
    ap_fixed<8, 4> v0001 = arg0000[v0000];
    ap_fixed<8, 4> v0002 = hls::tanh((float)v0001);
    arg0001[v0000] = v0002;
  }
  return;
}

void dataflow_node_27(ap_fixed<8, 4> arg0000[4096], ap_fixed<8, 4> arg0001[4096]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)4096; v0000 += (int)1) {
    ap_fixed<8, 4> v0001 = arg0000[v0000];
    ap_fixed<8, 4> v0002 = v0001 + (ap_fixed<8, 4>)1.000000;
    arg0001[v0000] = v0002;
  }
  return;
}

void dataflow_node_28(ap_fixed<8, 4> arg0000[4096], ap_fixed<8, 4> arg0001[4096], ap_fixed<8, 4> arg0002[4096]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=1 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)4096; v0000 += (int)32) {
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

void dataflow_node_29(ap_fixed<8, 4> arg0000[4096], ap_fixed<8, 4> arg0001[1][1][4096]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)4096; v0000 += (int)1) {
    ap_fixed<8, 4> v0001 = arg0000[v0000];
    arg0001[(int)0][(int)0][v0000] = v0001;
  }
  return;
}

void kernel_0_chip1(ap_fixed<8, 4> arg0000[1][1][1024], ap_fixed<8, 4> arg0001[1][1][1024], ap_fixed<8, 4> arg0002[1][1][4096], ap_fixed<8, 4> arg0003[1][1][1024], ap_fixed<8, 4> arg0004[4096], ap_fixed<8, 4> arg0005[1024][4096], ap_fixed<8, 4> arg0006[1024], ap_fixed<8, 4> arg0007[1024]) {
#pragma HLS interface s_axilite port=return bundle=control
#pragma HLS interface m_axi port=arg0000 offset=slave bundle=gmem0
#pragma HLS interface s_axilite port=arg0000 bundle=control
#pragma HLS interface m_axi port=arg0001 offset=slave bundle=gmem0
#pragma HLS interface s_axilite port=arg0001 bundle=control
#pragma HLS interface m_axi port=arg0002 offset=slave bundle=gmem0
#pragma HLS interface s_axilite port=arg0002 bundle=control
#pragma HLS interface m_axi port=arg0003 offset=slave bundle=gmem0
#pragma HLS interface s_axilite port=arg0003 bundle=control
#pragma HLS array_partition variable=arg0004 dim=1 cyclic factor=32
#pragma HLS interface m_axi port=arg0004 offset=slave bundle=gmem0
#pragma HLS interface s_axilite port=arg0004 bundle=control
#pragma HLS interface m_axi port=arg0005 offset=slave bundle=gmem0
#pragma HLS interface s_axilite port=arg0005 bundle=control
#pragma HLS array_partition variable=arg0006 dim=1 cyclic factor=32
#pragma HLS interface m_axi port=arg0006 offset=slave bundle=gmem0
#pragma HLS interface s_axilite port=arg0006 bundle=control
#pragma HLS array_partition variable=arg0007 dim=1 cyclic factor=32
#pragma HLS interface m_axi port=arg0007 offset=slave bundle=gmem0
#pragma HLS interface s_axilite port=arg0007 bundle=control
  ap_fixed<32, 8> v0000[1][1];
  dataflow_node_0(v0000);
  dataflow_node_1(arg0000, v0000);
  ap_fixed<8, 4> v0001[1];
  dataflow_node_2(v0000, v0001);
  ap_fixed<8, 4> v0002[1];
  dataflow_node_3(v0001, v0002);
  ap_fixed<8, 4> v0003[1024];
  #pragma HLS array_partition variable=v0003 dim=1 cyclic factor=32
  dataflow_node_4(v0002, v0003);
  ap_fixed<8, 4> v0004[1024];
  #pragma HLS array_partition variable=v0004 dim=1 cyclic factor=32
  dataflow_node_5(arg0000, v0003, v0004);
  dataflow_node_6(v0004, v0004, v0003);
  ap_fixed<32, 8> v0005[1][1];
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
  dataflow_node_15(v0006, arg0007, v0003);
  dataflow_node_16(v0003, arg0006, v0004);
  ap_fixed<32, 8> v0007[1][1][4096];
  // #pragma HLS bind_storage variable=v0007 type=ram_2p impl=bram latency=2
  dataflow_node_17(v0007);
  dataflow_node_18(v0004, arg0005, v0007);
  ap_fixed<8, 4> v0008[4096];
  #pragma HLS array_partition variable=v0008 dim=1 cyclic factor=32
  // #pragma HLS bind_storage variable=v0008 type=ram_2p impl=bram latency=2
  dataflow_node_19(v0007, v0008);
  ap_fixed<8, 4> v0009[4096];
  #pragma HLS array_partition variable=v0009 dim=1 cyclic factor=32
  // #pragma HLS bind_storage variable=v0009 type=ram_2p impl=bram latency=2
  dataflow_node_20(v0008, arg0004, v0009);
  dataflow_node_21(v0009, v0008);
  ap_fixed<8, 4> v0010[4096];
  #pragma HLS array_partition variable=v0010 dim=1 cyclic factor=32
  // #pragma HLS bind_storage variable=v0010 type=ram_2p impl=bram latency=2
  dataflow_node_22(v0009, v0010);
  ap_fixed<8, 4> v0011[4096];
  #pragma HLS array_partition variable=v0011 dim=1 cyclic factor=32
  // #pragma HLS bind_storage variable=v0011 type=ram_2p impl=bram latency=2
  dataflow_node_23(v0010, v0011);
  dataflow_node_24(v0009, v0011, v0010);
  dataflow_node_25(v0010, v0009);
  dataflow_node_26(v0009, v0010);
  dataflow_node_27(v0010, v0009);
  dataflow_node_28(v0008, v0009, v0010);
  dataflow_node_29(v0010, arg0002);
  return;
}

