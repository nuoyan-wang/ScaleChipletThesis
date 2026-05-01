#define kernel_0_chip1 kernel_0_chip1_core
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

void dataflow_node_17(ap_fixed<32, 8> arg0000[1][1][4096]) {
#pragma HLS inline off
  (void)arg0000;
  return;
}

static void shared_kernel_fc1(
    ap_fixed<8, 4> arg0000[1][1][16],
    ap_fixed<8, 4> arg0001[1][16][16],
    ap_fixed<32, 8> arg0002[1][1][16]);

void dataflow_node_18(ap_fixed<8, 4> arg0000[1024], ap_uint<256> arg0001[1024][128], ap_fixed<32, 8> arg0002[1][1][4096]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=32
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=16
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=16
  for (int v0000 = (int)0; v0000 < (int)1024; v0000 += (int)16) {
    for (int v0001 = (int)0; v0001 < (int)4096; v0001 += (int)16) {
    #pragma HLS pipeline II=1
      ap_fixed<8, 4> v0002[1][1][16];
      #pragma HLS array_partition variable=v0002 dim=3 cyclic factor=16
      for (int v0003 = (int)0; v0003 < (int)16; v0003 += (int)1) {
      #pragma HLS unroll factor=16
        v0002[(int)0][(int)0][v0003] = arg0000[v0000 + v0003];
      }
      ap_fixed<8, 4> v0004[1][16][16];
      #pragma HLS array_partition variable=v0004 dim=2 cyclic factor=16
      #pragma HLS array_partition variable=v0004 dim=3 cyclic factor=16
      int v0005 = v0001 / (int)32;
      int v0006_base = v0001 & (int)31;
      for (int v0006 = (int)0; v0006 < (int)16; v0006 += (int)1) {
      #pragma HLS unroll factor=16
        ap_uint<256> v0007 = arg0001[v0000 + v0006][v0005];
        for (int v0008 = (int)0; v0008 < (int)16; v0008 += (int)1) {
        #pragma HLS unroll factor=16
          ap_fixed<8, 4> v0009;
          int v0010 = v0006_base + v0008;
          v0009.range(7, 0) = v0007.range(v0010 * 8 + 7, v0010 * 8);
          v0004[(int)0][v0006][v0008] = v0009;
        }
      }
      ap_fixed<32, 8> v0011[1][1][16];
      #pragma HLS array_partition variable=v0011 dim=3 cyclic factor=16
      for (int v0012 = (int)0; v0012 < (int)16; v0012 += (int)1) {
      #pragma HLS unroll factor=16
        v0011[(int)0][(int)0][v0012] = arg0002[(int)0][(int)0][v0001 + v0012];
      }
      shared_kernel_fc1(v0002, v0004, v0011);
      for (int v0013 = (int)0; v0013 < (int)16; v0013 += (int)1) {
      #pragma HLS unroll factor=16
        arg0002[(int)0][(int)0][v0001 + v0013] = v0011[(int)0][(int)0][v0013];
      }
    }
  }
  return;
}

void dataflow_node_19(ap_fixed<32, 8> arg0000[1][1][4096], ap_fixed<8, 4> arg0001[4096]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=16
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=16
  for (int v0000 = (int)0; v0000 < (int)4096; v0000 += (int)16) {
  #pragma HLS pipeline II=1
    for (int v0001 = (int)0; v0001 < (int)16; v0001 += (int)1) {
    #pragma HLS unroll factor=16
      int v0002 = v0000 + v0001;
      arg0001[v0002] = (ap_fixed<8, 4>)arg0000[(int)0][(int)0][v0002];
    }
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
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=256
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=256
  for (int v0000 = (int)0; v0000 < (int)4096; v0000 += (int)256) {
  #pragma HLS pipeline II=1
    for (int v0001 = (int)0; v0001 < (int)256; v0001 += (int)1) {
    #pragma HLS unroll
      int v0002 = v0000 + v0001;
      ap_fixed<8, 4> x = arg0000[v0002];
      arg0001[v0002] = x * x * x;
    }
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
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=256
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=256
  for (int v0000 = (int)0; v0000 < (int)4096; v0000 += (int)256) {
  #pragma HLS pipeline II=1
    for (int v0001 = (int)0; v0001 < (int)256; v0001 += (int)1) {
    #pragma HLS unroll
      int v0002 = v0000 + v0001;
      arg0001[v0002] = arg0000[v0002] + (ap_fixed<8, 4>)1.000000;
    }
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
  for (int v0000 = (int)0; v0000 < (int)4096; v0000 += (int)256) {
  #pragma HLS pipeline II=1
    for (int v0001 = (int)0; v0001 < (int)256; v0001 += (int)1) {
    #pragma HLS unroll
      int v0002 = v0000 + v0001;
      arg0001[(int)0][(int)0][v0002] = arg0000[v0002];
    }
  }
  return;
}


void shared_kernel_fc1(
    ap_fixed<8, 4> arg0000[1][1][16],
    ap_fixed<8, 4> arg0001[1][16][16],
    ap_fixed<32, 8> arg0002[1][1][16]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=16
#pragma HLS array_partition variable=arg0001 dim=2 cyclic factor=16
#pragma HLS array_partition variable=arg0001 dim=3 cyclic factor=16
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=16
  ap_fixed<32, 8> v0000[4];
  #pragma HLS array_partition variable=v0000 complete
  for (int v0001 = (int)0; v0001 < (int)16; v0001 += (int)1) {
  #pragma HLS unroll factor=16
    for (int v0002 = (int)0; v0002 < (int)4; v0002 += (int)1) {
    #pragma HLS unroll factor=4
      v0000[v0002] = (ap_fixed<32, 8>)0.000000;
    }
    for (int v0003 = (int)0; v0003 < (int)16; v0003 += (int)1) {
    #pragma HLS unroll factor=16
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

void kernel_0_chip1(ap_fixed<8, 4> arg0000[1][1][1024], hls::stream<axis256_t> &arg0002, ap_fixed<8, 4> arg0004[4096], ap_uint<256> arg0005[1024][128], ap_fixed<8, 4> arg0006[1024], ap_fixed<8, 4> arg0007[1024]) {
#pragma HLS interface s_axilite port=return bundle=control
#pragma HLS array_partition variable=arg0004 dim=1 cyclic factor=32
#pragma HLS interface ap_memory port=arg0004
#pragma HLS interface ap_memory port=arg0005
#pragma HLS array_partition variable=arg0006 dim=1 cyclic factor=32
#pragma HLS interface ap_memory port=arg0006
#pragma HLS array_partition variable=arg0007 dim=1 cyclic factor=32
#pragma HLS interface ap_memory port=arg0007
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
  store_axis_vector_N<4096>(v0010, arg0002);
  return;
}

#undef kernel_0_chip1

void chunk1_stream_ip(
    hls::stream<axis256_t> &hidden_mid_in,
    hls::stream<axis256_t> &bypass_in,
    hls::stream<axis256_t> &gelu_out,
    hls::stream<axis256_t> &bypass_out,
    fxp8_t fc1_bias[4096],
    packed_fxp32_t fc1_weight[1024][128],
    fxp8_t ln2_beta[1024],
    fxp8_t ln2_gamma[1024]) {
#pragma HLS interface axis port=hidden_mid_in
#pragma HLS interface axis port=bypass_in
#pragma HLS interface axis port=gelu_out
#pragma HLS interface axis port=bypass_out
#pragma HLS interface s_axilite port=return bundle=control
#pragma HLS interface ap_memory port=fc1_bias
#pragma HLS interface ap_memory port=fc1_weight
#pragma HLS interface ap_memory port=ln2_beta
#pragma HLS interface ap_memory port=ln2_gamma

  fxp8_t hidden_mid_local[1][1][1024];

  load_axis_tensor_1x1xN<1024>(hidden_mid_in, hidden_mid_local);
  kernel_0_chip1_core(
      hidden_mid_local, gelu_out,
      fc1_bias, fc1_weight, ln2_beta, ln2_gamma);
  for (int i = 0; i < 32; ++i) {
  #pragma HLS pipeline II=1
    bypass_out.write(bypass_in.read());
  }
}
