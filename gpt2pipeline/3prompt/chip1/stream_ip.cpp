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
#ifndef __SYNTHESIS__
#include <stdio.h>
#include <stdlib.h>
#endif

using namespace std;

using packed_fxp64_t = ap_uint<512>;

#ifndef __SYNTHESIS__
static unsigned long long debug_fnv1a_bytes(const unsigned char *data, int n) {
  unsigned long long acc = 1469598103934665603ull;
  for (int i = 0; i < n; ++i) {
    acc ^= (unsigned long long)data[i];
    acc *= 1099511628211ull;
  }
  return acc;
}

static void debug_dump_fxp8_vector(const char *label, ap_fixed<8, 4> *data, int n, int head = 8) {
  unsigned char raw[4096];
  const char *quiet = getenv("CHUNK1_QUIET");
  if (n > 4096) n = 4096;
  for (int i = 0; i < n; ++i) {
    ap_uint<8> bits = data[i].range(7, 0);
    raw[i] = (unsigned char)bits.to_uint();
  }
  unsigned long long ck = debug_fnv1a_bytes(raw, n);
  if (!(quiet && quiet[0])) {
  printf("chunk1_boundary %s checksum=0x%016llx head0=[", label, ck);
  int limit = head < n ? head : n;
  for (int i = 0; i < limit; ++i) {
    if (i) printf(", ");
    printf("%.4f", (float)data[i]);
  }
  printf("]");
  if (n >= 40) {
    printf(" head32=[");
    int start = 32;
    int end = start + head;
    if (end > n) end = n;
    for (int i = start; i < end; ++i) {
      if (i != start) printf(", ");
      printf("%.4f", (float)data[i]);
    }
    printf("]");
  }
  printf("\n");
  fflush(stdout);
  }

  const char *dump_dir = getenv("CHUNK1_DUMP_DIR");
  char path[4096];
  if (dump_dir && dump_dir[0]) {
    snprintf(path, sizeof(path), "%s/%s.bin", dump_dir, label);
  } else {
    snprintf(path, sizeof(path), "/tmp/%s.bin", label);
  }
  FILE *f = fopen(path, "wb");
  if (f) {
    fwrite(raw, 1, (size_t)n, f);
    fclose(f);
  } else {
    printf("chunk1_boundary dump_open_failed path=%s\n", path);
    fflush(stdout);
  }
}
#endif

static ap_fixed<8, 4> clipped_tanh(ap_fixed<8, 4> x) {
#pragma HLS inline
  if (x > (ap_fixed<8, 4>)1.0) {
    return (ap_fixed<8, 4>)1.0;
  } else if (x < (ap_fixed<8, 4>)-1.0) {
    return (ap_fixed<8, 4>)-1.0;
  } else {
    return x;
  }
}

static ap_fixed<8, 4> pow3_fxp8(ap_fixed<8, 4> x) {
#pragma HLS inline
  return x * x * x;
}

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

void dataflow_node_7(ap_fixed<32, 8> arg0000[1][1]) {
#pragma HLS inline off
  arg0000[(int)0][(int)0] = (ap_fixed<32, 8>)0.000000;
  return;
}

void dataflow_node_8(ap_fixed<8, 4> arg0000[1024], ap_fixed<32, 8> arg0001[1][1]) {
#pragma HLS inline off
  ap_fixed<32, 8> v0000[4];
  for (int v0001 = (int)0; v0001 < (int)4; v0001 += (int)1) {
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

void dataflow_node_17(ap_fixed<8, 4> arg0000[1][1][4096]) {
#pragma HLS inline off
  #pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=64
  for (int v0000 = (int)0; v0000 < (int)4096; v0000 += (int)1) {
    arg0000[(int)0][(int)0][v0000] = (ap_fixed<8, 4>)0.000000;
  }
  return;
}

static void shared_kernel_fc1(
    ap_fixed<8, 4> arg0000[1][1][16],
    ap_fixed<8, 4> arg0001[1][16][16],
    ap_fixed<32, 8> arg0002[1][1][16]);

static void shared_kernel_fc1_8x32(
    ap_fixed<8, 4> arg0000[1][1][8],
    ap_fixed<8, 4> arg0001[1][8][32],
    ap_fixed<8, 4> arg0002[1][1][32]);

static void shared_kernel_fc1_8x64(
    ap_fixed<8, 4> arg0000[1][1][8],
    ap_fixed<8, 4> arg0001[1][8][64],
    ap_fixed<8, 4> arg0002[1][1][64]);

// dataflow_node_18: FC1 GEMM, 8-bank HBM streaming.
// Weight layout: fc1_bk[rg*64+col] holds rows where row%8==k, rg=row/8.
// Each 512-bit beat packs two adjacent 32-lane output groups. To match the
// monolithic kernel exactly, unpack the beat into two 8x32 tiles and update
// each half with the same 8x32 reduction semantics.
void dataflow_node_18(
    ap_fixed<8, 4> arg0000[1024],
    packed_fxp64_t *fc1_b0, packed_fxp64_t *fc1_b1,
    packed_fxp64_t *fc1_b2, packed_fxp64_t *fc1_b3,
    packed_fxp64_t *fc1_b4, packed_fxp64_t *fc1_b5,
    packed_fxp64_t *fc1_b6, packed_fxp64_t *fc1_b7,
    ap_fixed<8, 4> arg0002[1][1][4096]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=8
#pragma HLS array_partition variable=arg0002 dim=3 cyclic factor=64
  for (int rg = 0; rg < 128; rg++) {   // 1024/8 = 128 row groups
    for (int col = 0; col < 64; col++) {  // 4096/64 = 64 output col groups
    #pragma HLS pipeline II=1
      packed_fxp64_t w0 = fc1_b0[rg * 64 + col];
      packed_fxp64_t w1 = fc1_b1[rg * 64 + col];
      packed_fxp64_t w2 = fc1_b2[rg * 64 + col];
      packed_fxp64_t w3 = fc1_b3[rg * 64 + col];
      packed_fxp64_t w4 = fc1_b4[rg * 64 + col];
      packed_fxp64_t w5 = fc1_b5[rg * 64 + col];
      packed_fxp64_t w6 = fc1_b6[rg * 64 + col];
      packed_fxp64_t w7 = fc1_b7[rg * 64 + col];
      ap_fixed<8, 4> v0002[1][1][8];
      #pragma HLS array_partition variable=v0002 dim=3 complete
      for (int k = 0; k < 8; k++) {
      #pragma HLS unroll
        v0002[0][0][k] = arg0000[rg * 8 + k];
      }
      ap_fixed<8, 4> v0004[1][8][64];
      #pragma HLS array_partition variable=v0004 dim=2 complete
      #pragma HLS array_partition variable=v0004 dim=3 complete
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
      ap_fixed<8, 4> v0011[1][1][64];
      #pragma HLS array_partition variable=v0011 dim=3 complete
      for (int j = 0; j < 64; j++) {
      #pragma HLS unroll
        v0011[0][0][j] = arg0002[0][0][col * 64 + j];
      }
      shared_kernel_fc1_8x64(v0002, v0004, v0011);
      for (int j = 0; j < 64; j++) {
      #pragma HLS unroll
        arg0002[0][0][col * 64 + j] = v0011[0][0][j];
      }
    }
  }
  return;
}

void dataflow_node_19(ap_fixed<8, 4> arg0000[1][1][4096], ap_fixed<8, 4> arg0001[4096]) {
#pragma HLS inline off
#pragma HLS array_partition variable=arg0000 dim=3 cyclic factor=64
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=64
  for (int v0000 = (int)0; v0000 < (int)4096; v0000 += (int)64) {
  #pragma HLS pipeline II=1
    for (int v0001 = (int)0; v0001 < (int)64; v0001 += (int)1) {
    #pragma HLS unroll factor=64
      int v0002 = v0000 + v0001;
      arg0001[v0002] = arg0000[(int)0][(int)0][v0002];
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
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=64
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=64
  for (int v0000 = 0; v0000 < 4096; ++v0000) {
  #pragma HLS unroll factor=64
    arg0001[v0000] = pow3_fxp8(arg0000[v0000]);
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
#pragma HLS array_partition variable=arg0000 dim=1 cyclic factor=64
#pragma HLS array_partition variable=arg0001 dim=1 cyclic factor=64
  for (int v0000 = (int)0; v0000 < (int)4096; v0000 += (int)1) {
  #pragma HLS unroll factor=64
    arg0001[v0000] = clipped_tanh(arg0000[v0000]);
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
    ap_fixed<32, 8> v0011 = v0010 + v0009;
    #pragma HLS BIND_OP variable=v0011 op=add impl=fabric latency=1
    arg0002[(int)0][(int)0][v0001] = v0011;
  }
  return;
}

void shared_kernel_fc1_8x32(
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

// 8-input × 64-output MAC kernel for FC1 GEMM (bank-oriented, 4 accumulators).
void shared_kernel_fc1_8x64(
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

void kernel_0_chip1(
    ap_fixed<8, 4> arg0000[1][1][1024],
    hls::stream<axis256_t> &arg0002,
    ap_fixed<8, 4> arg0004[4096],
    packed_fxp64_t *fc1_b0, packed_fxp64_t *fc1_b1,
    packed_fxp64_t *fc1_b2, packed_fxp64_t *fc1_b3,
    packed_fxp64_t *fc1_b4, packed_fxp64_t *fc1_b5,
    packed_fxp64_t *fc1_b6, packed_fxp64_t *fc1_b7,
    ap_fixed<8, 4> arg0006[1024],
    ap_fixed<8, 4> arg0007[1024]) {
#pragma HLS interface s_axilite port=return bundle=control
#pragma HLS array_partition variable=arg0004 dim=1 cyclic factor=32
#pragma HLS interface ap_memory port=arg0004
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
#ifndef __SYNTHESIS__
  debug_dump_fxp8_vector("ln2_out", v0004, 1024);
#endif
  ap_fixed<8, 4> v0007[1][1][4096];
  // #pragma HLS bind_storage variable=v0007 type=ram_2p impl=bram latency=2
  dataflow_node_17(v0007);
  dataflow_node_18(v0004, fc1_b0, fc1_b1, fc1_b2, fc1_b3,
                            fc1_b4, fc1_b5, fc1_b6, fc1_b7, v0007);
  ap_fixed<8, 4> v0008[4096];
  #pragma HLS array_partition variable=v0008 dim=1 cyclic factor=32
  // #pragma HLS bind_storage variable=v0008 type=ram_2p impl=bram latency=2
  dataflow_node_19(v0007, v0008);
#ifndef __SYNTHESIS__
  debug_dump_fxp8_vector("fc1_prebias", v0008, 4096);
#endif
  ap_fixed<8, 4> v0009[4096];
  #pragma HLS array_partition variable=v0009 dim=1 cyclic factor=32
  // #pragma HLS bind_storage variable=v0009 type=ram_2p impl=bram latency=2
  dataflow_node_20(v0008, arg0004, v0009);
#ifndef __SYNTHESIS__
  debug_dump_fxp8_vector("fc1_out", v0009, 4096);
#endif
  ap_fixed<8, 4> v0010[4096];
  #pragma HLS array_partition variable=v0010 dim=1 cyclic factor=32
  // #pragma HLS bind_storage variable=v0010 type=ram_2p impl=bram latency=2
  ap_fixed<8, 4> v0011[4096];
  #pragma HLS array_partition variable=v0011 dim=1 cyclic factor=32
  ap_fixed<8, 4> v0012[4096];
  #pragma HLS array_partition variable=v0012 dim=1 cyclic factor=32
  dataflow_node_21(v0009, v0010);
  dataflow_node_22(v0009, v0011);
  dataflow_node_23(v0011, v0012);
  dataflow_node_24(v0009, v0012, v0011);
  dataflow_node_25(v0011, v0012);
  dataflow_node_26(v0012, v0011);
  dataflow_node_27(v0011, v0012);
  dataflow_node_28(v0010, v0012, v0011);
#ifndef __SYNTHESIS__
  debug_dump_fxp8_vector("gelu", v0011, 4096);
#endif
  store_axis_vector_N<4096>(v0011, arg0002);
  return;
}

#undef kernel_0_chip1

template <int N>
static void forward_axis_tensor(hls::stream<axis256_t> &in,
                                hls::stream<axis256_t> &out) {
  for (int i = 0; i < N / 32; ++i) {
  #pragma HLS pipeline II=1
    out.write(in.read());
  }
}

static void process_chunk1_frame(
    fxp8_t hidden_mid_local[1][1][1024],
    fxp8_t fc1_bias_local[4096],
    packed_fxp64_t *fc1_b0_l,
    packed_fxp64_t *fc1_b1_l,
    packed_fxp64_t *fc1_b2_l,
    packed_fxp64_t *fc1_b3_l,
    packed_fxp64_t *fc1_b4_l,
    packed_fxp64_t *fc1_b5_l,
    packed_fxp64_t *fc1_b6_l,
    packed_fxp64_t *fc1_b7_l,
    fxp8_t ln2_beta_local[1024],
    fxp8_t ln2_gamma_local[1024],
    hls::stream<axis256_t> &gelu_out,
    hls::stream<axis256_t> &bypass_in,
    hls::stream<axis256_t> &bypass_out) {
#pragma HLS dataflow
  kernel_0_chip1_core(
      hidden_mid_local, gelu_out,
      fc1_bias_local, fc1_b0_l, fc1_b1_l, fc1_b2_l, fc1_b3_l,
      fc1_b4_l, fc1_b5_l, fc1_b6_l, fc1_b7_l,
      ln2_beta_local, ln2_gamma_local);
  forward_axis_tensor<1024>(bypass_in, bypass_out);
}

void chunk1_stream_ip(
    hls::stream<axis256_t> &hidden_mid_in,
    hls::stream<axis256_t> &bypass_in,
    hls::stream<axis256_t> &gelu_out,
    hls::stream<axis256_t> &bypass_out,
    ap_uint<2> num_prompts,
    ap_uint<8> num_layers,
    // Bundle C1_B0: widened fc1_b0 weights only.
    packed_fxp64_t *fc1_b0,    // [128*64] = 8192 entries
    packed_fxp32_t *fc1_bias,  // [4096/32=128] packed, temp-only meta bundle
    packed_fxp32_t *ln2_beta,  // [1024/32=32]  packed, temp-only meta bundle
    packed_fxp32_t *ln2_gamma, // [1024/32=32]  packed, temp-only meta bundle
    // Bundles C1_B1..C1_B7: remaining weight banks
    packed_fxp64_t *fc1_b1,
    packed_fxp64_t *fc1_b2,
    packed_fxp64_t *fc1_b3,
    packed_fxp64_t *fc1_b4,
    packed_fxp64_t *fc1_b5,
    packed_fxp64_t *fc1_b6,
    packed_fxp64_t *fc1_b7) {
#pragma HLS interface axis port=hidden_mid_in
#pragma HLS interface axis port=bypass_in
#pragma HLS interface axis port=gelu_out
#pragma HLS interface axis port=bypass_out
#pragma HLS interface s_axilite port=num_prompts bundle=control
#pragma HLS interface s_axilite port=num_layers bundle=control
#pragma HLS interface s_axilite port=return bundle=control
// Bundle C1_B0: widened bank 0 weights plus FC1 metadata.
#pragma HLS interface m_axi port=fc1_b0    bundle=C1_B0 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS interface m_axi port=fc1_bias  bundle=C1_B0 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS interface m_axi port=ln2_beta  bundle=C1_B0 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS interface m_axi port=ln2_gamma bundle=C1_B0 max_read_burst_length=64 num_read_outstanding=4
// Bundles C1_B1..C1_B7: widened FC1 weight banks only, uniform config.
#pragma HLS interface m_axi port=fc1_b1   bundle=C1_B1 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS interface m_axi port=fc1_b2   bundle=C1_B2 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS interface m_axi port=fc1_b3   bundle=C1_B3 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS interface m_axi port=fc1_b4   bundle=C1_B4 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS interface m_axi port=fc1_b5   bundle=C1_B5 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS interface m_axi port=fc1_b6   bundle=C1_B6 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS interface m_axi port=fc1_b7   bundle=C1_B7 max_read_burst_length=64 num_read_outstanding=4

  fxp8_t hidden_mid_local[1][1][1024];
  fxp8_t fc1_bias_local[4096];
  fxp8_t ln2_beta_local[1024];
  fxp8_t ln2_gamma_local[1024];
  const int active_num_prompts =
      ((int)num_prompts > 3) ? 3 : (int)num_prompts;
  const int active_num_layers =
      ((int)num_layers > 24) ? 24 : (int)num_layers;
  for (int layer = 0; layer < 24; ++layer) {
  #pragma HLS loop_tripcount min=1 max=24
    // Layer-strided pointers and bias loads are shared across all prompts.
    packed_fxp64_t *fc1_b0_l = fc1_b0 + layer * 8192;
    packed_fxp64_t *fc1_b1_l = fc1_b1 + layer * 8192;
    packed_fxp64_t *fc1_b2_l = fc1_b2 + layer * 8192;
    packed_fxp64_t *fc1_b3_l = fc1_b3 + layer * 8192;
    packed_fxp64_t *fc1_b4_l = fc1_b4 + layer * 8192;
    packed_fxp64_t *fc1_b5_l = fc1_b5 + layer * 8192;
    packed_fxp64_t *fc1_b6_l = fc1_b6 + layer * 8192;
    packed_fxp64_t *fc1_b7_l = fc1_b7 + layer * 8192;
    if (layer < active_num_layers) {
      load_hbm_bias<4096>(fc1_bias  + layer * 128, fc1_bias_local);
      load_hbm_bias<1024>(ln2_beta  + layer * 32,  ln2_beta_local);
      load_hbm_bias<1024>(ln2_gamma + layer * 32,  ln2_gamma_local);
    }
    for (int prompt = 0; prompt < 3; ++prompt) {
    #pragma HLS loop_tripcount min=1 max=3
      const bool active_slot = (layer < active_num_layers) && (prompt < active_num_prompts);
      if (active_slot) {
        load_axis_tensor_1x1xN<1024>(hidden_mid_in, hidden_mid_local);
        process_chunk1_frame(
            hidden_mid_local, fc1_bias_local,
            fc1_b0_l, fc1_b1_l, fc1_b2_l, fc1_b3_l,
            fc1_b4_l, fc1_b5_l, fc1_b6_l, fc1_b7_l,
            ln2_beta_local, ln2_gamma_local,
            gelu_out, bypass_in, bypass_out);
      }
    }
  }
}
