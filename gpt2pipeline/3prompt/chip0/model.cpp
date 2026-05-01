
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
  ap_fixed<32, 8> v0000[2];
  #pragma HLS array_partition variable=v0000 dim=1 cyclic factor=2
  for (int v0001 = (int)0; v0001 < (int)32; v0001 += (int)1) {
  #pragma HLS unroll factor=32
    for (int v0002 = (int)0; v0002 < (int)2; v0002 += (int)1) {
    #pragma HLS unroll factor=2
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
      ap_fixed<32, 8> v0010 = v0000[v0009];
      ap_fixed<16, 8> v0013_prod = v0004 * v0005;
      #pragma HLS BIND_OP variable=v0013_prod op=mul impl=dsp
      ap_fixed<32, 8> v0013 = v0013_prod;
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

void dataflow_node_17(ap_fixed<32, 8> arg0000[1][1][3072]) {
#pragma HLS inline off
  (void)arg0000;
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


void dataflow_node_18(ap_fixed<8, 4> arg0000[1024], ap_uint<256> arg0001[1024][96], ap_fixed<32, 8> arg0002[1][1][3072]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=16
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)16) {
    for (int v0001 = (int)0; v0001 < (int)3072; v0001 += (int)32) {
    #pragma HLS pipeline II=1
      ap_fixed<8, 4> v0002[1][1][16];
      #pragma HLS array_partition variable=v0002 dim=3 cyclic factor=16
      for (int v0003 = (int)0; v0003 < (int)16; v0003 += (int)1) {
      #pragma HLS unroll factor=16
        v0002[(int)0][(int)0][v0003] = arg0000[v0000 + v0003];
      }
      ap_fixed<8, 4> v0004[1][16][32];
      #pragma HLS array_partition variable=v0004 dim=2 cyclic factor=16
      #pragma HLS array_partition variable=v0004 dim=3 cyclic factor=32
      int v0005 = v0001 / (int)32;
      for (int v0006 = (int)0; v0006 < (int)16; v0006 += (int)1) {
      #pragma HLS unroll factor=16
        ap_uint<256> v0007 = arg0001[v0000 + v0006][v0005];
        for (int v0008 = (int)0; v0008 < (int)32; v0008 += (int)1) {
        #pragma HLS unroll factor=32
          ap_fixed<8, 4> v0009;
          v0009.range(7, 0) = v0007.range(v0008 * 8 + 7, v0008 * 8);
          v0004[(int)0][v0006][v0008] = v0009;
        }
      }
      ap_fixed<32, 8> v0010[1][1][32];
      #pragma HLS array_partition variable=v0010 dim=3 cyclic factor=32
      for (int v0011 = (int)0; v0011 < (int)32; v0011 += (int)1) {
      #pragma HLS unroll factor=32
        v0010[(int)0][(int)0][v0011] = arg0002[(int)0][(int)0][v0001 + v0011];
      }
      shared_kernel_3_local_qkv(v0002, v0004, v0010);
      for (int v0012 = (int)0; v0012 < (int)32; v0012 += (int)1) {
      #pragma HLS unroll factor=32
        arg0002[(int)0][(int)0][v0001 + v0012] = v0010[(int)0][(int)0][v0012];
      }
    }
  }
  return;
}

void dataflow_node_19(ap_fixed<32, 8> arg0000[1][1][3072], ap_fixed<8, 4> arg0001[3072]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)3072; v0000 += (int)32) {
  #pragma HLS pipeline II=1
    ap_fixed<32, 8> v0001[32];
    #pragma HLS array_partition variable=v0001 dim=1 cyclic factor=32
    for (int v0002 = (int)0; v0002 < (int)32; v0002 += (int)1) {
    #pragma HLS unroll factor=32
      int v0003 = v0000 + v0002;
      int v0004 = v0003 / (int)3072;
      int v0005 = v0004 * (int)3072;
      ap_int<1> v0006 = v0003 != v0005;
      ap_int<1> v0007 = v0003 < (int)0;
      ap_int<1> v0008 = v0006 & v0007;
      int v0009 = v0004 + (int)-1;
      int v0010 = v0008 ? v0009 : v0004;
      int v0011 = v0003 % (int)3072;
      ap_int<1> v0012 = v0011 < (int)0;
      int v0013 = v0011 + (int)3072;
      int v0014 = v0012 ? v0013 : v0011;
      int v0015 = v0014 / (int)3072;
      int v0016 = v0003 % (int)3072;
      ap_int<1> v0017 = v0016 < (int)0;
      int v0018 = v0016 + (int)3072;
      int v0019 = v0017 ? v0018 : v0016;
      ap_fixed<32, 8> v0020 = arg0000[v0010][v0015][v0019];
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
    shared_kernel_3(v0001, v0021);
    for (int v0025 = (int)0; v0025 < (int)32; v0025 += (int)1) {
    #pragma HLS unroll factor=32
      ap_fixed<8, 4> v0026 = v0021[v0025];
      int v0027 = v0000 + v0025;
      arg0001[v0027] = v0026;
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

void dataflow_node_23(ap_fixed<8, 4> arg0000[3072], ap_fixed<8, 4> arg0001[16][128][64]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    int v0001 = v0000 * (int)64;
    for (int v0002 = (int)0; v0002 < (int)128; v0002 += (int)1) {
      for (int v0003 = (int)0; v0003 < (int)64; v0003 += (int)1) {
        int v0004 = v0001 + v0003;
        int v0005 = v0004 + (int)1024;
        ap_fixed<8, 4> v0006 = arg0000[v0005];
        arg0001[v0000][v0002][v0003] = v0006;
      }
    }
  }
  return;
}

void dataflow_node_24(ap_fixed<8, 4> arg0000[3072], ap_fixed<8, 4> arg0001[16][128][64]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    int v0001 = v0000 * (int)64;
    for (int v0002 = (int)0; v0002 < (int)128; v0002 += (int)1) {
      for (int v0003 = (int)0; v0003 < (int)64; v0003 += (int)1) {
        int v0004 = v0001 + v0003;
        int v0005 = v0004 + (int)2048;
        ap_fixed<8, 4> v0006 = arg0000[v0005];
        arg0001[v0000][v0002][v0003] = v0006;
      }
    }
  }
  return;
}

void dataflow_node_25(ap_int<1> arg0000[128], ap_fixed<8, 4> arg0001[16][128][64], ap_fixed<8, 4> arg0002[1][16][128][64], ap_fixed<8, 4> arg0003[16][128][64]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=2
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=2
#pragma HLS array_partition variable=arg0001 dim=3 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=2
#pragma HLS array_partition variable=arg0002 dim=4 cyclic factor=32
#pragma HLS array_partition variable=arg0003 dim=2 cyclic factor=2
#pragma HLS array_partition variable=arg0003 dim=3 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)64; v0000 += (int)32) {
    for (int v0001 = (int)0; v0001 < (int)16; v0001 += (int)1) {
      int v0002 = v0001 / (int)16;
      int v0003 = v0002 * (int)16;
      ap_int<1> v0004 = v0001 != v0003;
      ap_int<1> v0005 = v0001 < (int)0;
      ap_int<1> v0006 = v0004 & v0005;
      int v0007 = v0002 + (int)-1;
      int v0008 = v0006 ? v0007 : v0002;
      int v0009 = v0001 % (int)16;
      ap_int<1> v0010 = v0009 < (int)0;
      int v0011 = v0009 + (int)16;
      int v0012 = v0010 ? v0011 : v0009;
      for (int v0013 = (int)0; v0013 < (int)128; v0013 += (int)2) {
      #pragma HLS pipeline II=1
        ap_int<1> v0014[2];
        #pragma HLS array_partition variable=v0014 dim=1 cyclic factor=2
        for (int v0015 = (int)0; v0015 < (int)2; v0015 += (int)1) {
        #pragma HLS unroll factor=2
          int v0016 = v0013 + v0015;
          ap_int<1> v0017 = arg0000[v0016];
          v0014[v0015] = v0017;
        }
        ap_fixed<8, 4> v0018[1][2][32];
        #pragma HLS array_partition variable=v0018 dim=2 cyclic factor=2
        #pragma HLS array_partition variable=v0018 dim=3 cyclic factor=32
        for (int v0019 = (int)0; v0019 < (int)2; v0019 += (int)1) {
        #pragma HLS unroll factor=2
          int v0020 = v0013 + v0019;
          for (int v0021 = (int)0; v0021 < (int)32; v0021 += (int)1) {
          #pragma HLS unroll factor=32
            int v0022 = v0000 + v0021;
            ap_fixed<8, 4> v0023 = arg0001[v0001][v0020][v0022];
            v0018[(int)0][v0019][v0021] = v0023;
          }
        }
        ap_fixed<8, 4> v0024[1][2][32];
        #pragma HLS array_partition variable=v0024 dim=2 cyclic factor=2
        #pragma HLS array_partition variable=v0024 dim=3 cyclic factor=32
        for (int v0025 = (int)0; v0025 < (int)2; v0025 += (int)1) {
        #pragma HLS unroll factor=2
          int v0026 = v0013 + v0025;
          for (int v0027 = (int)0; v0027 < (int)32; v0027 += (int)1) {
          #pragma HLS unroll factor=32
            int v0028 = v0000 + v0027;
            ap_fixed<8, 4> v0029 = arg0002[v0008][v0012][v0026][v0028];
            v0024[(int)0][v0025][v0027] = v0029;
          }
        }
        ap_fixed<8, 4> v0030[1][2][32];
        #pragma HLS array_partition variable=v0030 dim=2 cyclic factor=2
        #pragma HLS array_partition variable=v0030 dim=3 cyclic factor=32
        for (int v0031 = (int)0; v0031 < (int)2; v0031 += (int)1) {
        #pragma HLS unroll factor=2
          int v0032 = v0013 + v0031;
          for (int v0033 = (int)0; v0033 < (int)32; v0033 += (int)1) {
          #pragma HLS unroll factor=32
            int v0034 = v0000 + v0033;
            ap_fixed<8, 4> v0035 = arg0003[v0001][v0032][v0034];
            v0030[(int)0][v0031][v0033] = v0035;
          }
        }
        shared_kernel_4(v0014, v0018, v0024, v0030);
        for (int v0036 = (int)0; v0036 < (int)2; v0036 += (int)1) {
        #pragma HLS unroll factor=2
          int v0037 = v0013 + v0036;
          for (int v0038 = (int)0; v0038 < (int)32; v0038 += (int)1) {
          #pragma HLS unroll factor=32
            ap_fixed<8, 4> v0039 = v0030[(int)0][v0036][v0038];
            int v0040 = v0000 + v0038;
            arg0003[v0001][v0037][v0040] = v0039;
          }
        }
      }
    }
  }
  return;
}

void dataflow_node_26(ap_int<1> arg0000[128], ap_fixed<8, 4> arg0001[16][128][64], ap_fixed<8, 4> arg0002[1][16][128][64], ap_fixed<8, 4> arg0003[16][128][64]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=2
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=2
#pragma HLS array_partition variable=arg0001 dim=3 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=2
#pragma HLS array_partition variable=arg0002 dim=4 cyclic factor=32
#pragma HLS array_partition variable=arg0003 dim=2 cyclic factor=2
#pragma HLS array_partition variable=arg0003 dim=3 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)64; v0000 += (int)32) {
    for (int v0001 = (int)0; v0001 < (int)16; v0001 += (int)1) {
      int v0002 = v0001 / (int)16;
      int v0003 = v0002 * (int)16;
      ap_int<1> v0004 = v0001 != v0003;
      ap_int<1> v0005 = v0001 < (int)0;
      ap_int<1> v0006 = v0004 & v0005;
      int v0007 = v0002 + (int)-1;
      int v0008 = v0006 ? v0007 : v0002;
      int v0009 = v0001 % (int)16;
      ap_int<1> v0010 = v0009 < (int)0;
      int v0011 = v0009 + (int)16;
      int v0012 = v0010 ? v0011 : v0009;
      for (int v0013 = (int)0; v0013 < (int)128; v0013 += (int)2) {
      #pragma HLS pipeline II=1
        ap_int<1> v0014[2];
        #pragma HLS array_partition variable=v0014 dim=1 cyclic factor=2
        for (int v0015 = (int)0; v0015 < (int)2; v0015 += (int)1) {
        #pragma HLS unroll factor=2
          int v0016 = v0013 + v0015;
          ap_int<1> v0017 = arg0000[v0016];
          v0014[v0015] = v0017;
        }
        ap_fixed<8, 4> v0018[1][2][32];
        #pragma HLS array_partition variable=v0018 dim=2 cyclic factor=2
        #pragma HLS array_partition variable=v0018 dim=3 cyclic factor=32
        for (int v0019 = (int)0; v0019 < (int)2; v0019 += (int)1) {
        #pragma HLS unroll factor=2
          int v0020 = v0013 + v0019;
          for (int v0021 = (int)0; v0021 < (int)32; v0021 += (int)1) {
          #pragma HLS unroll factor=32
            int v0022 = v0000 + v0021;
            ap_fixed<8, 4> v0023 = arg0001[v0001][v0020][v0022];
            v0018[(int)0][v0019][v0021] = v0023;
          }
        }
        ap_fixed<8, 4> v0024[1][2][32];
        #pragma HLS array_partition variable=v0024 dim=2 cyclic factor=2
        #pragma HLS array_partition variable=v0024 dim=3 cyclic factor=32
        for (int v0025 = (int)0; v0025 < (int)2; v0025 += (int)1) {
        #pragma HLS unroll factor=2
          int v0026 = v0013 + v0025;
          for (int v0027 = (int)0; v0027 < (int)32; v0027 += (int)1) {
          #pragma HLS unroll factor=32
            int v0028 = v0000 + v0027;
            ap_fixed<8, 4> v0029 = arg0002[v0008][v0012][v0026][v0028];
            v0024[(int)0][v0025][v0027] = v0029;
          }
        }
        ap_fixed<8, 4> v0030[1][2][32];
        #pragma HLS array_partition variable=v0030 dim=2 cyclic factor=2
        #pragma HLS array_partition variable=v0030 dim=3 cyclic factor=32
        for (int v0031 = (int)0; v0031 < (int)2; v0031 += (int)1) {
        #pragma HLS unroll factor=2
          int v0032 = v0013 + v0031;
          for (int v0033 = (int)0; v0033 < (int)32; v0033 += (int)1) {
          #pragma HLS unroll factor=32
            int v0034 = v0000 + v0033;
            ap_fixed<8, 4> v0035 = arg0003[v0001][v0032][v0034];
            v0030[(int)0][v0031][v0033] = v0035;
          }
        }
        shared_kernel_4(v0014, v0018, v0024, v0030);
        for (int v0036 = (int)0; v0036 < (int)2; v0036 += (int)1) {
        #pragma HLS unroll factor=2
          int v0037 = v0013 + v0036;
          for (int v0038 = (int)0; v0038 < (int)32; v0038 += (int)1) {
          #pragma HLS unroll factor=32
            ap_fixed<8, 4> v0039 = v0030[(int)0][v0036][v0038];
            int v0040 = v0000 + v0038;
            arg0003[v0001][v0037][v0040] = v0039;
          }
        }
      }
    }
  }
  return;
}

void dataflow_node_27(ap_fixed<8, 4> arg0000[16][128][64], ap_fixed<8, 4> arg0001[1][16][64][128]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    for (int v0001 = (int)0; v0001 < (int)64; v0001 += (int)1) {
      for (int v0002 = (int)0; v0002 < (int)128; v0002 += (int)1) {
        ap_fixed<8, 4> v0003 = arg0000[v0000][v0002][v0001];
        arg0001[(int)0][v0000][v0001][v0002] = v0003;
      }
    }
  }
  return;
}

void dataflow_node_28(ap_fixed<32, 8> arg0000[16][1][128]) {
#pragma HLS inline off
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    for (int v0001 = (int)0; v0001 < (int)128; v0001 += (int)1) {
      arg0000[v0000][(int)0][v0001] = (ap_fixed<32, 8>)0.000000;
    }
  }
  return;
}

void dataflow_node_29(ap_fixed<8, 4> arg0000[3072], ap_fixed<8, 4> arg0001[1][16][64][128], ap_fixed<32, 8> arg0002[16][1][128]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0001 dim=3 cyclic factor=8
#pragma HLS array_partition variable=arg0001 dim=4 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)64; v0000 += (int)8) {
    for (int v0001 = (int)0; v0001 < (int)16; v0001 += (int)1) {
      int v0002 = v0001 * (int)64;
      int v0003 = v0001 / (int)16;
      int v0004 = v0003 * (int)16;
      ap_int<1> v0005 = v0001 != v0004;
      ap_int<1> v0006 = v0001 < (int)0;
      ap_int<1> v0007 = v0005 & v0006;
      int v0008 = v0003 + (int)-1;
      int v0009 = v0007 ? v0008 : v0003;
      int v0010 = v0001 % (int)16;
      ap_int<1> v0011 = v0010 < (int)0;
      int v0012 = v0010 + (int)16;
      int v0013 = v0011 ? v0012 : v0010;
      for (int v0014 = (int)0; v0014 < (int)128; v0014 += (int)32) {
      #pragma HLS pipeline II=1
        ap_fixed<8, 4> v0015[1][1][8];
        #pragma HLS array_partition variable=v0015 dim=3 cyclic factor=8
        for (int v0016 = (int)0; v0016 < (int)8; v0016 += (int)1) {
        #pragma HLS unroll factor=8
          int v0017 = v0000 + v0016;
          int v0018 = v0002 + v0017;
          ap_fixed<8, 4> v0019 = arg0000[v0018];
          v0015[(int)0][(int)0][v0016] = v0019;
        }
        ap_fixed<8, 4> v0020[1][8][32];
        #pragma HLS array_partition variable=v0020 dim=2 cyclic factor=8
        #pragma HLS array_partition variable=v0020 dim=3 cyclic factor=32
        for (int v0021 = (int)0; v0021 < (int)8; v0021 += (int)1) {
        #pragma HLS unroll factor=8
          int v0022 = v0000 + v0021;
          for (int v0023 = (int)0; v0023 < (int)32; v0023 += (int)1) {
          #pragma HLS unroll factor=32
            int v0024 = v0014 + v0023;
            ap_fixed<8, 4> v0025 = arg0001[v0009][v0013][v0022][v0024];
            v0020[(int)0][v0021][v0023] = v0025;
          }
        }
        ap_fixed<32, 8> v0026[1][1][32];
        #pragma HLS array_partition variable=v0026 dim=3 cyclic factor=32
        for (int v0027 = (int)0; v0027 < (int)32; v0027 += (int)1) {
        #pragma HLS unroll factor=32
          int v0028 = v0014 + v0027;
          ap_fixed<32, 8> v0029 = arg0002[v0001][(int)0][v0028];
          v0026[(int)0][(int)0][v0027] = v0029;
        }
        shared_kernel_2(v0015, v0020, v0026);
        for (int v0030 = (int)0; v0030 < (int)32; v0030 += (int)1) {
        #pragma HLS unroll factor=32
          ap_fixed<32, 8> v0031 = v0026[(int)0][(int)0][v0030];
          int v0032 = v0014 + v0030;
          arg0002[v0001][(int)0][v0032] = v0031;
        }
      }
    }
  }
  return;
}

void dataflow_node_30(ap_fixed<32, 8> arg0000[16][1][128], ap_fixed<8, 4> arg0001[16][128]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)128; v0000 += (int)32) {
    for (int v0001 = (int)0; v0001 < (int)16; v0001 += (int)1) {
    #pragma HLS pipeline II=1
      ap_fixed<32, 8> v0002[1][32];
      #pragma HLS array_partition variable=v0002 dim=2 cyclic factor=32
      for (int v0003 = (int)0; v0003 < (int)32; v0003 += (int)1) {
      #pragma HLS unroll factor=32
        int v0004 = v0000 + v0003;
        ap_fixed<32, 8> v0005 = arg0000[v0001][(int)0][v0004];
        v0002[(int)0][v0003] = v0005;
      }
      ap_fixed<8, 4> v0006[1][32];
      #pragma HLS array_partition variable=v0006 dim=2 cyclic factor=32
      for (int v0007 = (int)0; v0007 < (int)32; v0007 += (int)1) {
      #pragma HLS unroll factor=32
        int v0008 = v0000 + v0007;
        ap_fixed<8, 4> v0009 = arg0001[v0001][v0008];
        v0006[(int)0][v0007] = v0009;
      }
      shared_kernel_5(v0002, v0006);
      for (int v0010 = (int)0; v0010 < (int)32; v0010 += (int)1) {
      #pragma HLS unroll factor=32
        ap_fixed<8, 4> v0011 = v0006[(int)0][v0010];
        int v0012 = v0000 + v0010;
        arg0001[v0001][v0012] = v0011;
      }
    }
  }
  return;
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
    ap_fixed<32, 8> v0013 = hls::divide((float)(ap_fixed<32, 8>)1.000000, (float)v0012);
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

void dataflow_node_43(ap_fixed<32, 8> arg0000[16][128], ap_fixed<8, 4> arg0001[16][128]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=2 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)128; v0000 += (int)32) {
    for (int v0001 = (int)0; v0001 < (int)16; v0001 += (int)1) {
    #pragma HLS pipeline II=1
      ap_fixed<32, 8> v0002[1][32];
      #pragma HLS array_partition variable=v0002 dim=2 cyclic factor=32
      for (int v0003 = (int)0; v0003 < (int)32; v0003 += (int)1) {
      #pragma HLS unroll factor=32
        int v0004 = v0000 + v0003;
        ap_fixed<32, 8> v0005 = arg0000[v0001][v0004];
        v0002[(int)0][v0003] = v0005;
      }
      ap_fixed<8, 4> v0006[1][32];
      #pragma HLS array_partition variable=v0006 dim=2 cyclic factor=32
      for (int v0007 = (int)0; v0007 < (int)32; v0007 += (int)1) {
      #pragma HLS unroll factor=32
        int v0008 = v0000 + v0007;
        ap_fixed<8, 4> v0009 = arg0001[v0001][v0008];
        v0006[(int)0][v0007] = v0009;
      }
      shared_kernel_5(v0002, v0006);
      for (int v0010 = (int)0; v0010 < (int)32; v0010 += (int)1) {
      #pragma HLS unroll factor=32
        ap_fixed<8, 4> v0011 = v0006[(int)0][v0010];
        int v0012 = v0000 + v0010;
        arg0001[v0001][v0012] = v0011;
      }
    }
  }
  return;
}

void dataflow_node_44(ap_fixed<32, 8> arg0000[16][1][64]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)16; v0000 += (int)1) {
    for (int v0001 = (int)0; v0001 < (int)64; v0001 += (int)32) {
    #pragma HLS pipeline II=1
      for (int v0002 = (int)0; v0002 < (int)32; v0002 += (int)1) {
      #pragma HLS unroll factor=32
        arg0000[v0000][(int)0][v0001 + v0002] = (ap_fixed<32, 8>)0.000000;
      }
    }
  }
  return;
}

void dataflow_node_45(ap_fixed<8, 4> arg0000[16][128], ap_fixed<8, 4> arg0001[16][128][64], ap_fixed<32, 8> arg0002[16][1][64]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=2 cyclic factor=8
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=8
#pragma HLS array_partition variable=arg0001 dim=3 cyclic factor=32
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)128; v0000 += (int)8) {
    for (int v0001 = (int)0; v0001 < (int)16; v0001 += (int)1) {
      for (int v0002 = (int)0; v0002 < (int)64; v0002 += (int)32) {
      #pragma HLS pipeline II=1
        ap_fixed<8, 4> v0003[1][1][8];
        #pragma HLS array_partition variable=v0003 dim=3 cyclic factor=8
        for (int v0004 = (int)0; v0004 < (int)8; v0004 += (int)1) {
        #pragma HLS unroll factor=8
          int v0005 = v0000 + v0004;
          ap_fixed<8, 4> v0006 = arg0000[v0001][v0005];
          v0003[(int)0][(int)0][v0004] = v0006;
        }
        ap_fixed<8, 4> v0007[1][8][32];
        #pragma HLS array_partition variable=v0007 dim=2 cyclic factor=8
        #pragma HLS array_partition variable=v0007 dim=3 cyclic factor=32
        for (int v0008 = (int)0; v0008 < (int)8; v0008 += (int)1) {
        #pragma HLS unroll factor=8
          int v0009 = v0000 + v0008;
          for (int v0010 = (int)0; v0010 < (int)32; v0010 += (int)1) {
          #pragma HLS unroll factor=32
            int v0011 = v0002 + v0010;
            ap_fixed<8, 4> v0012 = arg0001[v0001][v0009][v0011];
            v0007[(int)0][v0008][v0010] = v0012;
          }
        }
        ap_fixed<32, 8> v0013[1][1][32];
        #pragma HLS array_partition variable=v0013 dim=3 cyclic factor=32
        for (int v0014 = (int)0; v0014 < (int)32; v0014 += (int)1) {
        #pragma HLS unroll factor=32
          int v0015 = v0002 + v0014;
          ap_fixed<32, 8> v0016 = arg0002[v0001][(int)0][v0015];
          v0013[(int)0][(int)0][v0014] = v0016;
        }
        shared_kernel_2(v0003, v0007, v0013);
        for (int v0017 = (int)0; v0017 < (int)32; v0017 += (int)1) {
        #pragma HLS unroll factor=32
          ap_fixed<32, 8> v0018 = v0013[(int)0][(int)0][v0017];
          int v0019 = v0002 + v0017;
          arg0002[v0001][(int)0][v0019] = v0018;
        }
      }
    }
  }
  return;
}

void dataflow_node_46(ap_fixed<32, 8> arg0000[16][1][64], ap_fixed<8, 4> arg0001[16][64]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)64; v0000 += (int)32) {
    for (int v0001 = (int)0; v0001 < (int)16; v0001 += (int)1) {
    #pragma HLS pipeline II=1
      ap_fixed<32, 8> v0002[1][32];
      #pragma HLS array_partition variable=v0002 dim=2 cyclic factor=32
      for (int v0003 = (int)0; v0003 < (int)32; v0003 += (int)1) {
      #pragma HLS unroll factor=32
        int v0004 = v0000 + v0003;
        ap_fixed<32, 8> v0005 = arg0000[v0001][(int)0][v0004];
        v0002[(int)0][v0003] = v0005;
      }
      ap_fixed<8, 4> v0006[1][32];
      #pragma HLS array_partition variable=v0006 dim=2 cyclic factor=32
      for (int v0007 = (int)0; v0007 < (int)32; v0007 += (int)1) {
      #pragma HLS unroll factor=32
        int v0008 = v0000 + v0007;
        ap_fixed<8, 4> v0009 = arg0001[v0001][v0008];
        v0006[(int)0][v0007] = v0009;
      }
      shared_kernel_5(v0002, v0006);
      for (int v0010 = (int)0; v0010 < (int)32; v0010 += (int)1) {
      #pragma HLS unroll factor=32
        ap_fixed<8, 4> v0011 = v0006[(int)0][v0010];
        int v0012 = v0000 + v0010;
        arg0001[v0001][v0012] = v0011;
      }
    }
  }
  return;
}

void dataflow_node_47(ap_fixed<32, 8> arg0000[1][1][1024]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)32) {
  #pragma HLS pipeline II=1
    for (int v0001 = (int)0; v0001 < (int)32; v0001 += (int)1) {
    #pragma HLS unroll factor=32
      int v0002 = v0000 + v0001;
      arg0000[(int)0][(int)0][v0002] = (ap_fixed<32, 8>)0.000000;
    }
  }
  return;
}

void dataflow_node_48(ap_fixed<8, 4> arg0000[16][64], ap_uint<256> arg0001[1024][32], ap_fixed<32, 8> arg0002[1][1][1024]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=8
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)8) {
    for (int v0001 = (int)0; v0001 < (int)1024; v0001 += (int)32) {
    #pragma HLS pipeline II=1
      ap_fixed<8, 4> v0002[1][1][8];
      #pragma HLS array_partition variable=v0002 dim=3 cyclic factor=8
      for (int v0003 = (int)0; v0003 < (int)8; v0003 += (int)1) {
      #pragma HLS unroll factor=8
        int v0004 = v0000 + v0003;
        v0002[(int)0][(int)0][v0003] = arg0000[v0004 >> 6][v0004 & (int)63];
      }
      ap_fixed<8, 4> v0005[1][8][32];
      #pragma HLS array_partition variable=v0005 dim=2 cyclic factor=8
      #pragma HLS array_partition variable=v0005 dim=3 cyclic factor=32
      int v0006 = v0001 / (int)32;
      for (int v0007 = (int)0; v0007 < (int)8; v0007 += (int)1) {
      #pragma HLS unroll factor=8
        ap_uint<256> v0008 = arg0001[v0000 + v0007][v0006];
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
        v0011[(int)0][(int)0][v0012] = arg0002[(int)0][(int)0][v0001 + v0012];
      }
      shared_kernel_2(v0002, v0005, v0011);
      for (int v0013 = (int)0; v0013 < (int)32; v0013 += (int)1) {
      #pragma HLS unroll factor=32
        arg0002[(int)0][(int)0][v0001 + v0013] = v0011[(int)0][(int)0][v0013];
      }
    }
  }
  return;
}

void dataflow_node_49(ap_fixed<32, 8> arg0000[1][1][1024], ap_fixed<8, 4> arg0001[1024]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=32
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)32) {
  #pragma HLS pipeline II=1
    ap_fixed<32, 8> v0001[32];
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
      ap_fixed<32, 8> v0020 = arg0000[v0010][v0015][v0019];
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
    shared_kernel_3(v0001, v0021);
    for (int v0025 = (int)0; v0025 < (int)32; v0025 += (int)1) {
    #pragma HLS unroll factor=32
      ap_fixed<8, 4> v0026 = v0021[v0025];
      int v0027 = v0000 + v0025;
      arg0001[v0027] = v0026;
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
    ap_fixed<32, 8> arg0003[16][128],
    ap_fixed<32, 8> arg0004[1][16][1]) {
#pragma HLS inline off
  for (int v0000 = 0; v0000 < 16; ++v0000) {
    ap_fixed<32, 8> v0001[4];
    #pragma HLS array_partition variable=v0001 complete
    for (int v0002 = 0; v0002 < 4; ++v0002) {
    #pragma HLS unroll
      v0001[v0002] = (ap_fixed<32, 8>)-340282346638528859811704183484516925440.000000;
    }
    for (int v0003 = 0; v0003 < 128; ++v0003) {
    #pragma HLS pipeline II=1
      ap_fixed<8, 4> v0004 = arg0001[v0000][v0003];
      ap_fixed<8, 4> v0005 = v0004 * (ap_fixed<8, 4>)0.125000;
      ap_int<1> v0006 = arg0000[0][0][127][v0003];
      ap_fixed<8, 4> v0007 = v0006 ? v0005 : (ap_fixed<8, 4>)-INFINITY;
      ap_fixed<8, 4> v0008 = v0007 + arg0002[0][0][0][v0003];
      ap_fixed<32, 8> v0009 = (ap_fixed<32, 8>)v0008;
      arg0003[v0000][v0003] = v0009;
      int v0010 = v0003 & 3;
      ap_fixed<32, 8> v0011 = v0001[v0010];
      v0001[v0010] = (v0009 > v0011) ? v0009 : v0011;
    }
    ap_fixed<32, 8> v0012 = (v0001[0] > v0001[1]) ? v0001[0] : v0001[1];
    ap_fixed<32, 8> v0013 = (v0001[2] > v0001[3]) ? v0001[2] : v0001[3];
    arg0004[0][v0000][0] = (v0012 > v0013) ? v0012 : v0013;
  }
  return;
}

void dataflow_softmax_pass2(
    ap_fixed<32, 8> arg0000[16][128],
    ap_fixed<32, 8> arg0001[1][16][1],
    ap_fixed<32, 8> arg0002[16][128],
    ap_fixed<32, 8> arg0003[1][16][1]) {
#pragma HLS inline off
  for (int v0000 = 0; v0000 < 16; ++v0000) {
    ap_fixed<32, 8> v0001[4];
    #pragma HLS array_partition variable=v0001 complete
    for (int v0002 = 0; v0002 < 4; ++v0002) {
    #pragma HLS unroll
      v0001[v0002] = (ap_fixed<32, 8>)0.000000;
    }
    ap_fixed<32, 8> v0003 = arg0001[0][v0000][0];
    for (int v0004 = 0; v0004 < 128; ++v0004) {
    #pragma HLS pipeline II=1
      ap_fixed<32, 8> v0005 = arg0000[v0000][v0004] - v0003;
      ap_fixed<32, 8> v0006 = hls::exp((float)v0005);
      arg0002[v0000][v0004] = v0006;
      int v0007 = v0004 & 3;
      v0001[v0007] = v0001[v0007] + v0006;
    }
    arg0003[0][v0000][0] = v0001[0] + v0001[1] + v0001[2] + v0001[3];
  }
  return;
}

void dataflow_softmax_pass3(
    ap_fixed<32, 8> arg0000[16][128],
    ap_fixed<32, 8> arg0001[1][16][1],
    ap_fixed<8, 4> arg0002[16][128]) {
#pragma HLS inline off
  for (int v0000 = 0; v0000 < 16; ++v0000) {
    ap_fixed<32, 8> v0001 = hls::divide((float)(ap_fixed<32, 8>)1.000000, (float)arg0001[0][v0000][0]);
    for (int v0002 = 0; v0002 < 128; ++v0002) {
    #pragma HLS pipeline II=1
      ap_fixed<32, 8> v0003 = arg0000[v0000][v0002] * v0001;
      arg0002[v0000][v0002] = (ap_fixed<8, 4>)v0003;
    }
  }
  return;
}


void kernel_0_chip0(ap_int<1> arg0000[1][1][1024][1024], ap_fixed<8, 4> arg0001[1], ap_fixed<8, 4> arg0048[1][1][1024], ap_int<32> arg0049[1], ap_fixed<8, 4> arg0050[1][1][1][128], ap_uint<256> arg0051[1][16][128][2], ap_uint<256> arg0052[1][16][128][2], ap_fixed<8, 4> arg0053[1][1][1024], ap_fixed<8, 4> arg0054[1][1][1024], ap_uint<256> arg0055[1][16][64][4], ap_fixed<8, 4> arg0057[1024], ap_uint<256> arg0058[1024][32], ap_fixed<8, 4> arg0059[3072], ap_uint<256> arg0060[1024][96], ap_fixed<8, 4> arg0061[1024], ap_fixed<8, 4> arg0062[1024]) {
#pragma HLS interface s_axilite port=return bundle=control
#pragma HLS interface ap_memory port=arg0048
#pragma HLS interface ap_memory port=arg0050
#pragma HLS interface ap_memory port=arg0051
#pragma HLS interface ap_memory port=arg0052
#pragma HLS interface ap_memory port=arg0055
#pragma HLS interface ap_memory port=arg0053
#pragma HLS interface ap_memory port=arg0054
#pragma HLS interface ap_memory port=arg0054
#pragma HLS interface ap_memory port=arg0054
#pragma HLS array_partition variable=arg0057 dim=1 cyclic factor=32
#pragma HLS interface ap_memory port=arg0057
#pragma HLS array_partition variable=arg0058 dim=1 cyclic factor=8
#pragma HLS interface ap_memory port=arg0058
#pragma HLS array_partition variable=arg0059 dim=1 cyclic factor=32
#pragma HLS interface ap_memory port=arg0059
#pragma HLS array_partition variable=arg0060 dim=1 cyclic factor=16
#pragma HLS interface ap_memory port=arg0060
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
  dataflow_node_15(v0006, arg0062, v0003);
  dataflow_node_16(v0003, arg0061, v0004);
  ap_fixed<32, 8> v0007[1][1][3072];
  #pragma HLS array_partition variable=v0007 dim=3 cyclic factor=32
  // #pragma HLS bind_storage variable=v0007 type=ram_2p impl=bram latency=2
  dataflow_node_17(v0007);
  dataflow_node_18(v0004, arg0060, v0007);
  ap_fixed<8, 4> v0008[3072];
  #pragma HLS array_partition variable=v0008 dim=1 cyclic factor=32
  // #pragma HLS bind_storage variable=v0008 type=ram_2p impl=bram latency=2
  dataflow_node_19(v0007, v0008);
  ap_fixed<8, 4> v0009[3072];
  #pragma HLS array_partition variable=v0009 dim=1 cyclic factor=32
  // #pragma HLS bind_storage variable=v0009 type=ram_2p impl=bram latency=2
  dataflow_node_20(v0008, arg0059, v0009);
  ap_fixed<8, 4> v0012_fast[16][64];
  #pragma HLS array_partition variable=v0012_fast dim=2 cyclic factor=8
  dataflow_node_23_fast(v0009, v0012_fast);
  ap_fixed<8, 4> v0013_fast[16][64];
  #pragma HLS array_partition variable=v0013_fast dim=2 cyclic factor=8
  dataflow_node_24_fast(v0009, v0013_fast);
  dataflow_node_25_fast(arg0049, v0012_fast, arg0051, arg0055);
  dataflow_node_26_fast(arg0049, v0013_fast, arg0052);
  ap_fixed<32, 8> v0016[16][1][128];
  #pragma HLS array_partition variable=v0016 dim=3 cyclic factor=32
  // #pragma HLS bind_storage variable=v0016 type=ram_2p impl=bram latency=2
  dataflow_node_28(v0016);
  dataflow_node_29_fast(v0009, arg0055, v0016);
  ap_fixed<8, 4> v0017[16][128];
  #pragma HLS array_partition variable=v0017 dim=2 cyclic factor=32
  // #pragma HLS bind_storage variable=v0017 type=ram_2p impl=bram latency=2
  dataflow_node_30(v0016, v0017);
  ap_fixed<32, 8> v0019[16][128];
  // #pragma HLS bind_storage variable=v0019 type=ram_2p impl=bram latency=2
  ap_fixed<32, 8> v0020[1][16][1];
  dataflow_softmax_pass1(arg0000, v0017, arg0050, v0019, v0020);
  ap_fixed<32, 8> v0021[16][128];
  #pragma HLS array_partition variable=v0021 dim=2 cyclic factor=32
  // #pragma HLS bind_storage variable=v0021 type=ram_2p impl=bram latency=2
  ap_fixed<32, 8> v0022[1][16][1];
  dataflow_softmax_pass2(v0019, v0020, v0021, v0022);
  dataflow_softmax_pass3(v0021, v0022, v0017);
  ap_fixed<32, 8> v0024[16][1][64];
  #pragma HLS array_partition variable=v0024 dim=3 cyclic factor=32
  dataflow_node_44(v0024);
  dataflow_node_45_fast(v0017, arg0052, v0024);
  ap_fixed<8, 4> v0025[16][64];
  #pragma HLS array_partition variable=v0025 dim=2 cyclic factor=32
  dataflow_node_46(v0024, v0025);
  ap_fixed<32, 8> v0026[1][1][1024];
  #pragma HLS array_partition variable=v0026 dim=3 cyclic factor=32
  dataflow_node_47(v0026);
  dataflow_node_48(v0025, arg0058, v0026);
  dataflow_node_49(v0026, v0003);
  dataflow_node_50(v0003, arg0057, v0004);
  dataflow_node_51(v0004, arg0048, v0003);
  dataflow_node_52(v0003, arg0053);
  // KV banks are updated in place; no explicit full-bank output copy.
  return;
}
