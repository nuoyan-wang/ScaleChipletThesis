module {
  func.func @shared_kernel_5(%arg0: memref<1x32xf32> {hls.partition = #hls.partition<[none, cyclic], [0, 32]>}, %arg1: memref<1x32xf8E4M3> {hls.partition = #hls.partition<[none, cyclic], [0, 32]>}) attributes {hls.initiation_interval = #hls.initiation_interval<1>, hls.kernel, hls.latency = #hls.latency<0>, hls.resource = #hls.resource<bram : 0>} {
    %c32 = arith.constant 32 : index
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    scf.for %arg2 = %c0 to %c32 step %c1 {
      %0 = memref.load %arg0[%c0, %arg2] : memref<1x32xf32>
      %1 = arith.truncf %0 : f32 to f8E4M3
      memref.store %1, %arg1[%c0, %arg2] : memref<1x32xf8E4M3>
    } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    return
  }
  func.func @shared_kernel_4(%arg0: memref<2xi1> {hls.partition = #hls.partition<[cyclic], [2]>}, %arg1: memref<1x2x32xf8E4M3> {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 2, 32]>}, %arg2: memref<1x2x32xf8E4M3> {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 2, 32]>}, %arg3: memref<1x2x32xf8E4M3> {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 2, 32]>}) attributes {hls.initiation_interval = #hls.initiation_interval<1>, hls.kernel, hls.latency = #hls.latency<0>, hls.resource = #hls.resource<bram : 0>} {
    %c32 = arith.constant 32 : index
    %c2 = arith.constant 2 : index
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    scf.for %arg4 = %c0 to %c2 step %c1 {
      scf.for %arg5 = %c0 to %c32 step %c1 {
        %0 = memref.load %arg0[%arg4] : memref<2xi1>
        %1 = memref.load %arg1[%c0, %arg4, %arg5] : memref<1x2x32xf8E4M3>
        %2 = memref.load %arg2[%c0, %arg4, %arg5] : memref<1x2x32xf8E4M3>
        %3 = arith.select %0, %1, %2 : f8E4M3
        memref.store %3, %arg3[%c0, %arg4, %arg5] : memref<1x2x32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<2>}
    return
  }
  func.func @shared_kernel_3(%arg0: memref<32xf32> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg1: memref<32xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) attributes {hls.initiation_interval = #hls.initiation_interval<1>, hls.kernel, hls.latency = #hls.latency<0>, hls.resource = #hls.resource<bram : 0>} {
    %c0 = arith.constant 0 : index
    %c32 = arith.constant 32 : index
    %c1 = arith.constant 1 : index
    scf.for %arg2 = %c0 to %c32 step %c1 {
      %0 = memref.load %arg0[%arg2] : memref<32xf32>
      %1 = arith.truncf %0 : f32 to f8E4M3
      memref.store %1, %arg1[%arg2] : memref<32xf8E4M3>
    } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    return
  }
  func.func @shared_kernel_2(%arg0: memref<1x1x8xf8E4M3> {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 8]>}, %arg1: memref<1x8x32xf8E4M3> {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 8, 32]>}, %arg2: memref<1x1x32xf32> {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 32]>}) attributes {hls.initiation_interval = #hls.initiation_interval<1>, hls.kernel, hls.latency = #hls.latency<0>, hls.resource = #hls.resource<bram : 0>} {
    %c4 = arith.constant 4 : index
    %c8 = arith.constant 8 : index
    %c2 = arith.constant 2 : index
    %c32 = arith.constant 32 : index
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    %cst = arith.constant 0.000000e+00 : f32
    %alloc = memref.alloc() {hls.partition = #hls.partition<[cyclic], [2]>, hls.resource = #hls.resource<bram : 0>} : memref<2xf32>
    scf.for %arg3 = %c0 to %c32 step %c1 {
      scf.for %arg4 = %c0 to %c2 step %c1 {
        memref.store %cst, %alloc[%arg4] : memref<2xf32>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<2>}
      scf.for %arg4 = %c0 to %c8 step %c1 {
        %5 = memref.load %arg0[%c0, %c0, %arg4] : memref<1x1x8xf8E4M3>
        %6 = memref.load %arg1[%c0, %arg4, %arg3] : memref<1x8x32xf8E4M3>
        %7 = arith.remsi %arg4, %c4 : index
        %8 = arith.cmpi slt, %7, %c0 : index
        %9 = arith.addi %7, %c4 : index
        %10 = arith.select %8, %9, %7 : index
        %11 = memref.load %alloc[%10] : memref<2xf32>
        %12 = arith.extf %5 : f8E4M3 to f32
        %13 = arith.extf %6 : f8E4M3 to f32
        %14 = arith.mulf %12, %13 : f32
        %15 = arith.addf %11, %14 : f32
        %16 = arith.remsi %arg4, %c4 : index
        %17 = arith.cmpi slt, %16, %c0 : index
        %18 = arith.addi %16, %c4 : index
        %19 = arith.select %17, %18, %16 : index
        memref.store %15, %alloc[%19] : memref<2xf32>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<8>}
      %0 = memref.load %arg2[%c0, %c0, %arg3] : memref<1x1x32xf32>
      %1 = memref.load %alloc[%c0] : memref<2xf32>
      %2 = memref.load %alloc[%c1] : memref<2xf32>
      %3 = arith.addf %1, %2 : f32
      %4 = arith.addf %3, %0 : f32
      memref.store %4, %arg2[%c0, %c0, %arg3] : memref<1x1x32xf32>
    } {hls.latency = #hls.latency<2>, hls.unroll = #hls.unroll<32>}
    return
  }
  func.func @shared_kernel_1(%arg0: memref<32xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg1: memref<32xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg2: memref<32xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) attributes {hls.initiation_interval = #hls.initiation_interval<1>, hls.kernel, hls.latency = #hls.latency<0>, hls.resource = #hls.resource<bram : 0>} {
    %c0 = arith.constant 0 : index
    %c32 = arith.constant 32 : index
    %c1 = arith.constant 1 : index
    scf.for %arg3 = %c0 to %c32 step %c1 {
      %0 = memref.load %arg0[%arg3] : memref<32xf8E4M3>
      %1 = memref.load %arg1[%arg3] : memref<32xf8E4M3>
      %2 = arith.addf %0, %1 : f8E4M3
      memref.store %2, %arg2[%arg3] : memref<32xf8E4M3>
    } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    return
  }
  func.func @shared_kernel_0(%arg0: memref<32xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg1: memref<32xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg2: memref<32xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) attributes {hls.initiation_interval = #hls.initiation_interval<1>, hls.kernel, hls.latency = #hls.latency<0>, hls.resource = #hls.resource<bram : 0>} {
    %c0 = arith.constant 0 : index
    %c32 = arith.constant 32 : index
    %c1 = arith.constant 1 : index
    scf.for %arg3 = %c0 to %c32 step %c1 {
      %0 = memref.load %arg0[%arg3] : memref<32xf8E4M3>
      %1 = memref.load %arg1[%arg3] : memref<32xf8E4M3>
      %2 = arith.mulf %0, %1 : f8E4M3
      memref.store %2, %arg2[%arg3] : memref<32xf8E4M3>
    } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    return
  }
  memref.global "private" constant @__constant_xbf16_ffe646c1 : memref<f8E4M3> = dense<0.000000e+00>
  memref.global "private" constant @__constant_xbf16_c4ae1415 : memref<f8E4M3> = dense<0.000000e+00>
  memref.global "private" constant @__constant_xbf16_b8114a9c : memref<f8E4M3> = dense<0xF8>
  memref.global "private" constant @__constant_xbf16_60f0933a : memref<f8E4M3> = dense<1.250000e-01>
  memref.global "private" constant @__constant_3072xbf16_35cfe092 : memref<3072xf8E4M3> = dense_resource<torch_tensor_3072_torch.bfloat16>
  memref.global "private" constant @__constant_128xi64_7f1a85be : memref<128xi64> = dense<"0x00000000000000000100000000000000020000000000000003000000000000000400000000000000050000000000000006000000000000000700000000000000080000000000000009000000000000000A000000000000000B000000000000000C000000000000000D000000000000000E000000000000000F0000000000000010000000000000001100000000000000120000000000000013000000000000001400000000000000150000000000000016000000000000001700000000000000180000000000000019000000000000001A000000000000001B000000000000001C000000000000001D000000000000001E000000000000001F0000000000000020000000000000002100000000000000220000000000000023000000000000002400000000000000250000000000000026000000000000002700000000000000280000000000000029000000000000002A000000000000002B000000000000002C000000000000002D000000000000002E000000000000002F0000000000000030000000000000003100000000000000320000000000000033000000000000003400000000000000350000000000000036000000000000003700000000000000380000000000000039000000000000003A000000000000003B000000000000003C000000000000003D000000000000003E000000000000003F0000000000000040000000000000004100000000000000420000000000000043000000000000004400000000000000450000000000000046000000000000004700000000000000480000000000000049000000000000004A000000000000004B000000000000004C000000000000004D000000000000004E000000000000004F0000000000000050000000000000005100000000000000520000000000000053000000000000005400000000000000550000000000000056000000000000005700000000000000580000000000000059000000000000005A000000000000005B000000000000005C000000000000005D000000000000005E000000000000005F0000000000000060000000000000006100000000000000620000000000000063000000000000006400000000000000650000000000000066000000000000006700000000000000680000000000000069000000000000006A000000000000006B000000000000006C000000000000006D000000000000006E000000000000006F0000000000000070000000000000007100000000000000720000000000000073000000000000007400000000000000750000000000000076000000000000007700000000000000780000000000000079000000000000007A000000000000007B000000000000007C000000000000007D000000000000007E000000000000007F00000000000000">
  memref.global "private" constant @__constant_1024xbf16_afdc2952 : memref<1024xf8E4M3> = dense_resource<torch_tensor_1024_torch.bfloat16_2>
  memref.global "private" constant @__constant_1024xbf16_78fbe581 : memref<1024xf8E4M3> = dense_resource<torch_tensor_1024_torch.bfloat16>
  memref.global "private" constant @__constant_1024xbf16_113d1edf : memref<1024xf8E4M3> = dense_resource<torch_tensor_1024_torch.bfloat16_1>
  memref.global "private" constant @__constant_1024x3072xbf16_1afe0d7a : memref<1024x3072xf8E4M3> = dense_resource<torch_tensor_1024_3072_torch.bfloat16>
  memref.global "private" constant @__constant_1024x1024xbf16_bf2043af : memref<1024x1024xf8E4M3> = dense_resource<torch_tensor_1024_1024_torch.bfloat16>
  func.func private @dataflow_node_0(%arg0: memref<1x1xf32>) {
    %cst = arith.constant 0.000000e+00 : f32
    %c0 = arith.constant 0 : index
    memref.store %cst, %arg0[%c0, %c0] : memref<1x1xf32>
    return
  }
  func.func private @dataflow_node_1(%arg0: memref<1x1x1024xf8E4M3>, %arg1: memref<1x1xf32>) {
    %c4 = arith.constant 4 : index
    %c1024 = arith.constant 1024 : index
    %c2 = arith.constant 2 : index
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    %cst = arith.constant 0.000000e+00 : f32
    %alloc = memref.alloc() : memref<2xf32>
    scf.for %arg2 = %c0 to %c2 step %c1 {
      memref.store %cst, %alloc[%arg2] : memref<2xf32>
    } {hls.latency = #hls.latency<2>}
    scf.for %arg2 = %c0 to %c1024 step %c1 {
      %5 = memref.load %arg0[%c0, %c0, %arg2] : memref<1x1x1024xf8E4M3>
      %6 = arith.remsi %arg2, %c4 : index
      %7 = arith.cmpi slt, %6, %c0 : index
      %8 = arith.addi %6, %c4 : index
      %9 = arith.select %7, %8, %6 : index
      %10 = memref.load %alloc[%9] : memref<2xf32>
      %11 = arith.extf %5 : f8E4M3 to f32
      %12 = arith.addf %11, %10 : f32
      %13 = arith.remsi %arg2, %c4 : index
      %14 = arith.cmpi slt, %13, %c0 : index
      %15 = arith.addi %13, %c4 : index
      %16 = arith.select %14, %15, %13 : index
      memref.store %12, %alloc[%16] : memref<2xf32>
    } {hls.latency = #hls.latency<1024>}
    %0 = memref.load %arg1[%c0, %c0] : memref<1x1xf32>
    %1 = memref.load %alloc[%c0] : memref<2xf32>
    %2 = memref.load %alloc[%c1] : memref<2xf32>
    %3 = arith.addf %1, %2 : f32
    %4 = arith.addf %3, %0 : f32
    memref.store %4, %arg1[%c0, %c0] : memref<1x1xf32>
    return
  }
  func.func private @dataflow_node_2(%arg0: memref<1x1xf32>, %arg1: memref<f8E4M3>) {
    %c0 = arith.constant 0 : index
    %0 = memref.load %arg0[%c0, %c0] : memref<1x1xf32>
    %1 = arith.truncf %0 : f32 to f8E4M3
    memref.store %1, %arg1[] : memref<f8E4M3>
    return
  }
  func.func private @dataflow_node_3(%arg0: memref<f8E4M3>, %arg1: memref<f8E4M3>) {
    %cst = arith.constant 0.000000e+00 : f8E4M3
    %0 = memref.load %arg0[] : memref<f8E4M3>
    %1 = arith.mulf %0, %cst : f8E4M3
    memref.store %1, %arg1[] : memref<f8E4M3>
    return
  }
  func.func private @dataflow_node_4(%arg0: memref<f8E4M3>, %arg1: memref<1024xf8E4M3>) {
    %c0 = arith.constant 0 : index
    %c1024 = arith.constant 1024 : index
    %c1 = arith.constant 1 : index
    scf.for %arg2 = %c0 to %c1024 step %c1 {
      %0 = memref.load %arg0[] : memref<f8E4M3>
      memref.store %0, %arg1[%arg2] : memref<1024xf8E4M3>
    } {hls.latency = #hls.latency<1024>}
    return
  }
  func.func private @dataflow_node_5(%arg0: memref<1x1x1024xf8E4M3>, %arg1: memref<1024xf8E4M3>, %arg2: memref<1024xf8E4M3>) {
    %c-1 = arith.constant -1 : index
    %c1 = arith.constant 1 : index
    %c1024 = arith.constant 1024 : index
    %c0 = arith.constant 0 : index
    scf.for %arg3 = %c0 to %c1024 step %c1 {
      %0 = arith.divsi %arg3, %c1024 : index
      %1 = arith.muli %0, %c1024 : index
      %2 = arith.cmpi ne, %arg3, %1 : index
      %3 = arith.cmpi slt, %arg3, %c0 : index
      %4 = arith.andi %2, %3 : i1
      %5 = arith.addi %0, %c-1 : index
      %6 = arith.select %4, %5, %0 : index
      %7 = arith.remsi %arg3, %c1024 : index
      %8 = arith.cmpi slt, %7, %c0 : index
      %9 = arith.addi %7, %c1024 overflow<nsw> : index
      %10 = arith.select %8, %9, %7 : index
      %11 = arith.divsi %10, %c1024 : index
      %12 = arith.remsi %arg3, %c1024 : index
      %13 = arith.cmpi slt, %12, %c0 : index
      %14 = arith.addi %12, %c1024 overflow<nsw> : index
      %15 = arith.select %13, %14, %12 : index
      %16 = memref.load %arg0[%6, %11, %15] : memref<1x1x1024xf8E4M3>
      %17 = memref.load %arg1[%arg3] : memref<1024xf8E4M3>
      %18 = arith.subf %16, %17 : f8E4M3
      memref.store %18, %arg2[%arg3] : memref<1024xf8E4M3>
    } {hls.latency = #hls.latency<1024>}
    return
  }
  func.func private @dataflow_node_6(%arg0: memref<1024xf8E4M3>, %arg1: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg2: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) {
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    %c1024 = arith.constant 1024 : index
    %c32 = arith.constant 32 : index
    scf.for %arg3 = %c0 to %c1024 step %c32 {
      %alloc = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg1[%0] : memref<1024xf8E4M3>
        memref.store %1, %alloc[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg1[%0] : memref<1024xf8E4M3>
        memref.store %1, %alloc_0[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_1 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg2[%0] : memref<1024xf8E4M3>
        memref.store %1, %alloc_1[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      func.call @shared_kernel_0(%alloc, %alloc_0, %alloc_1) : (memref<32xf8E4M3>, memref<32xf8E4M3>, memref<32xf8E4M3>) -> ()
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = memref.load %alloc_1[%arg4] : memref<32xf8E4M3>
        %1 = arith.addi %arg3, %arg4 : index
        memref.store %0, %arg2[%1] : memref<1024xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<32>}
    return
  }
  func.func private @dataflow_node_7(%arg0: memref<1x1xf32>) {
    %cst = arith.constant 0.000000e+00 : f32
    %c0 = arith.constant 0 : index
    memref.store %cst, %arg0[%c0, %c0] : memref<1x1xf32>
    return
  }
  func.func private @dataflow_node_8(%arg0: memref<1024xf8E4M3>, %arg1: memref<1x1xf32>) {
    %c4 = arith.constant 4 : index
    %c1024 = arith.constant 1024 : index
    %c2 = arith.constant 2 : index
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    %cst = arith.constant 0.000000e+00 : f32
    %alloc = memref.alloc() : memref<2xf32>
    scf.for %arg2 = %c0 to %c2 step %c1 {
      memref.store %cst, %alloc[%arg2] : memref<2xf32>
    } {hls.latency = #hls.latency<2>}
    scf.for %arg2 = %c0 to %c1024 step %c1 {
      %5 = memref.load %arg0[%arg2] : memref<1024xf8E4M3>
      %6 = arith.remsi %arg2, %c4 : index
      %7 = arith.cmpi slt, %6, %c0 : index
      %8 = arith.addi %6, %c4 : index
      %9 = arith.select %7, %8, %6 : index
      %10 = memref.load %alloc[%9] : memref<2xf32>
      %11 = arith.extf %5 : f8E4M3 to f32
      %12 = arith.addf %11, %10 : f32
      %13 = arith.remsi %arg2, %c4 : index
      %14 = arith.cmpi slt, %13, %c0 : index
      %15 = arith.addi %13, %c4 : index
      %16 = arith.select %14, %15, %13 : index
      memref.store %12, %alloc[%16] : memref<2xf32>
    } {hls.latency = #hls.latency<1024>}
    %0 = memref.load %arg1[%c0, %c0] : memref<1x1xf32>
    %1 = memref.load %alloc[%c0] : memref<2xf32>
    %2 = memref.load %alloc[%c1] : memref<2xf32>
    %3 = arith.addf %1, %2 : f32
    %4 = arith.addf %3, %0 : f32
    memref.store %4, %arg1[%c0, %c0] : memref<1x1xf32>
    return
  }
  func.func private @dataflow_node_9(%arg0: memref<1x1xf32>, %arg1: memref<f8E4M3>) {
    %c0 = arith.constant 0 : index
    %0 = memref.load %arg0[%c0, %c0] : memref<1x1xf32>
    %1 = arith.truncf %0 : f32 to f8E4M3
    memref.store %1, %arg1[] : memref<f8E4M3>
    return
  }
  func.func private @dataflow_node_10(%arg0: memref<f8E4M3>, %arg1: memref<f8E4M3>) {
    %cst = arith.constant 0.000000e+00 : f8E4M3
    %0 = memref.load %arg0[] : memref<f8E4M3>
    %1 = arith.mulf %0, %cst : f8E4M3
    memref.store %1, %arg1[] : memref<f8E4M3>
    return
  }
  func.func private @dataflow_node_11(%arg0: memref<f8E4M3>, %arg1: memref<f8E4M3>) {
    %cst = arith.constant 0.000000e+00 : f8E4M3
    %0 = memref.load %arg0[] : memref<f8E4M3>
    %1 = arith.addf %0, %cst : f8E4M3
    memref.store %1, %arg1[] : memref<f8E4M3>
    return
  }
  func.func private @dataflow_node_12(%arg0: memref<f8E4M3>, %arg1: memref<f8E4M3>) {
    %0 = memref.load %arg0[] : memref<f8E4M3>
    %1 = math.rsqrt %0 : f8E4M3
    memref.store %1, %arg1[] : memref<f8E4M3>
    return
  }
  func.func private @dataflow_node_13(%arg0: memref<f8E4M3>, %arg1: memref<1024xf8E4M3>) {
    %c0 = arith.constant 0 : index
    %c1024 = arith.constant 1024 : index
    %c1 = arith.constant 1 : index
    scf.for %arg2 = %c0 to %c1024 step %c1 {
      %0 = memref.load %arg0[] : memref<f8E4M3>
      memref.store %0, %arg1[%arg2] : memref<1024xf8E4M3>
    } {hls.latency = #hls.latency<1024>}
    return
  }
  func.func private @dataflow_node_14(%arg0: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg1: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg2: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) {
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    %c1024 = arith.constant 1024 : index
    %c32 = arith.constant 32 : index
    scf.for %arg3 = %c0 to %c1024 step %c32 {
      %alloc = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg0[%0] : memref<1024xf8E4M3>
        memref.store %1, %alloc[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg1[%0] : memref<1024xf8E4M3>
        memref.store %1, %alloc_0[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_1 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg2[%0] : memref<1024xf8E4M3>
        memref.store %1, %alloc_1[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      func.call @shared_kernel_0(%alloc, %alloc_0, %alloc_1) : (memref<32xf8E4M3>, memref<32xf8E4M3>, memref<32xf8E4M3>) -> ()
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = memref.load %alloc_1[%arg4] : memref<32xf8E4M3>
        %1 = arith.addi %arg3, %arg4 : index
        memref.store %0, %arg2[%1] : memref<1024xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<32>}
    return
  }
  func.func private @dataflow_node_15(%arg0: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg1: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg2: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) {
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    %c1024 = arith.constant 1024 : index
    %c32 = arith.constant 32 : index
    scf.for %arg3 = %c0 to %c1024 step %c32 {
      %alloc = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg0[%0] : memref<1024xf8E4M3>
        memref.store %1, %alloc[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg1[%0] : memref<1024xf8E4M3>
        memref.store %1, %alloc_0[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_1 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg2[%0] : memref<1024xf8E4M3>
        memref.store %1, %alloc_1[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      func.call @shared_kernel_0(%alloc, %alloc_0, %alloc_1) : (memref<32xf8E4M3>, memref<32xf8E4M3>, memref<32xf8E4M3>) -> ()
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = memref.load %alloc_1[%arg4] : memref<32xf8E4M3>
        %1 = arith.addi %arg3, %arg4 : index
        memref.store %0, %arg2[%1] : memref<1024xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<32>}
    return
  }
  func.func private @dataflow_node_16(%arg0: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg1: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg2: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) {
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    %c1024 = arith.constant 1024 : index
    %c32 = arith.constant 32 : index
    scf.for %arg3 = %c0 to %c1024 step %c32 {
      %alloc = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg0[%0] : memref<1024xf8E4M3>
        memref.store %1, %alloc[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg1[%0] : memref<1024xf8E4M3>
        memref.store %1, %alloc_0[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_1 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg2[%0] : memref<1024xf8E4M3>
        memref.store %1, %alloc_1[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      func.call @shared_kernel_1(%alloc, %alloc_0, %alloc_1) : (memref<32xf8E4M3>, memref<32xf8E4M3>, memref<32xf8E4M3>) -> ()
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = memref.load %alloc_1[%arg4] : memref<32xf8E4M3>
        %1 = arith.addi %arg3, %arg4 : index
        memref.store %0, %arg2[%1] : memref<1024xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<32>}
    return
  }
  func.func private @dataflow_node_17(%arg0: memref<1x1x3072xf32>) {
    %c3072 = arith.constant 3072 : index
    %cst = arith.constant 0.000000e+00 : f32
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    scf.for %arg1 = %c0 to %c3072 step %c1 {
      memref.store %cst, %arg0[%c0, %c0, %arg1] : memref<1x1x3072xf32>
    } {hls.latency = #hls.latency<3072>}
    return
  }
  func.func private @dataflow_node_18(%arg0: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [8]>}, %arg1: memref<1024x3072xf8E4M3> {hls.partition = #hls.partition<[cyclic, cyclic], [8, 32]>}, %arg2: memref<1x1x3072xf32> {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 32]>}) {
    %c32 = arith.constant 32 : index
    %c8 = arith.constant 8 : index
    %c3072 = arith.constant 3072 : index
    %c1 = arith.constant 1 : index
    %c1024 = arith.constant 1024 : index
    %c0 = arith.constant 0 : index
    scf.for %arg3 = %c0 to %c1024 step %c8 {
      scf.for %arg4 = %c0 to %c3072 step %c32 {
        %alloc = memref.alloc() {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 8]>} : memref<1x1x8xf8E4M3>
        scf.for %arg5 = %c0 to %c8 step %c1 {
          %0 = arith.addi %arg3, %arg5 : index
          %1 = memref.load %arg0[%0] : memref<1024xf8E4M3>
          memref.store %1, %alloc[%c0, %c0, %arg5] : memref<1x1x8xf8E4M3>
        } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<8>}
        %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 8, 32]>} : memref<1x8x32xf8E4M3>
        scf.for %arg5 = %c0 to %c8 step %c1 {
          %0 = arith.addi %arg3, %arg5 : index
          scf.for %arg6 = %c0 to %c32 step %c1 {
            %1 = arith.addi %arg4, %arg6 : index
            %2 = memref.load %arg1[%0, %1] : memref<1024x3072xf8E4M3>
            memref.store %2, %alloc_0[%c0, %arg5, %arg6] : memref<1x8x32xf8E4M3>
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
        } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<8>}
        %alloc_1 = memref.alloc() {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 32]>} : memref<1x1x32xf32>
        scf.for %arg5 = %c0 to %c32 step %c1 {
          %0 = arith.addi %arg4, %arg5 : index
          %1 = memref.load %arg2[%c0, %c0, %0] : memref<1x1x3072xf32>
          memref.store %1, %alloc_1[%c0, %c0, %arg5] : memref<1x1x32xf32>
        } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
        func.call @shared_kernel_2(%alloc, %alloc_0, %alloc_1) : (memref<1x1x8xf8E4M3>, memref<1x8x32xf8E4M3>, memref<1x1x32xf32>) -> ()
        scf.for %arg5 = %c0 to %c32 step %c1 {
          %0 = memref.load %alloc_1[%c0, %c0, %arg5] : memref<1x1x32xf32>
          %1 = arith.addi %arg4, %arg5 : index
          memref.store %0, %arg2[%c0, %c0, %1] : memref<1x1x3072xf32>
        } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<96>}
    } {hls.latency = #hls.latency<12288>}
    return
  }
  func.func private @dataflow_node_19(%arg0: memref<1x1x3072xf32>, %arg1: memref<3072xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) {
    %c-1 = arith.constant -1 : index
    %c1 = arith.constant 1 : index
    %c32 = arith.constant 32 : index
    %c3072 = arith.constant 3072 : index
    %c0 = arith.constant 0 : index
    scf.for %arg2 = %c0 to %c3072 step %c32 {
      %alloc = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf32>
      scf.for %arg3 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg2, %arg3 : index
        %1 = arith.divsi %0, %c3072 : index
        %2 = arith.muli %1, %c3072 : index
        %3 = arith.cmpi ne, %0, %2 : index
        %4 = arith.cmpi slt, %0, %c0 : index
        %5 = arith.andi %3, %4 : i1
        %6 = arith.addi %1, %c-1 : index
        %7 = arith.select %5, %6, %1 : index
        %8 = arith.remsi %0, %c3072 : index
        %9 = arith.cmpi slt, %8, %c0 : index
        %10 = arith.addi %8, %c3072 overflow<nsw> : index
        %11 = arith.select %9, %10, %8 : index
        %12 = arith.divsi %11, %c3072 : index
        %13 = arith.remsi %0, %c3072 : index
        %14 = arith.cmpi slt, %13, %c0 : index
        %15 = arith.addi %13, %c3072 overflow<nsw> : index
        %16 = arith.select %14, %15, %13 : index
        %17 = memref.load %arg0[%7, %12, %16] : memref<1x1x3072xf32>
        memref.store %17, %alloc[%arg3] : memref<32xf32>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg3 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg2, %arg3 : index
        %1 = memref.load %arg1[%0] : memref<3072xf8E4M3>
        memref.store %1, %alloc_0[%arg3] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      func.call @shared_kernel_3(%alloc, %alloc_0) : (memref<32xf32>, memref<32xf8E4M3>) -> ()
      scf.for %arg3 = %c0 to %c32 step %c1 {
        %0 = memref.load %alloc_0[%arg3] : memref<32xf8E4M3>
        %1 = arith.addi %arg2, %arg3 : index
        memref.store %0, %arg1[%1] : memref<3072xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<96>}
    return
  }
  func.func private @dataflow_node_20(%arg0: memref<3072xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg1: memref<3072xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg2: memref<3072xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) {
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    %c3072 = arith.constant 3072 : index
    %c32 = arith.constant 32 : index
    scf.for %arg3 = %c0 to %c3072 step %c32 {
      %alloc = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg0[%0] : memref<3072xf8E4M3>
        memref.store %1, %alloc[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg1[%0] : memref<3072xf8E4M3>
        memref.store %1, %alloc_0[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_1 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg2[%0] : memref<3072xf8E4M3>
        memref.store %1, %alloc_1[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      func.call @shared_kernel_1(%alloc, %alloc_0, %alloc_1) : (memref<32xf8E4M3>, memref<32xf8E4M3>, memref<32xf8E4M3>) -> ()
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = memref.load %alloc_1[%arg4] : memref<32xf8E4M3>
        %1 = arith.addi %arg3, %arg4 : index
        memref.store %0, %arg2[%1] : memref<3072xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<96>}
    return
  }
  func.func private @dataflow_node_21(%arg0: memref<1xi32>, %arg1: memref<i64>) {
    %c0 = arith.constant 0 : index
    %0 = memref.load %arg0[%c0] : memref<1xi32>
    %1 = arith.extsi %0 : i32 to i64
    memref.store %1, %arg1[] : memref<i64>
    return
  }
  func.func private @dataflow_node_22(%arg0: memref<i64>, %arg1: memref<128xi1>) {
    %c1 = arith.constant 1 : index
    %c128 = arith.constant 128 : index
    %c0 = arith.constant 0 : index
    %0 = memref.get_global @__constant_128xi64_7f1a85be : memref<128xi64>
    scf.for %arg2 = %c0 to %c128 step %c1 {
      %1 = memref.load %arg0[] : memref<i64>
      %2 = memref.load %0[%arg2] : memref<128xi64>
      %3 = arith.cmpi eq, %1, %2 : i64
      memref.store %3, %arg1[%arg2] : memref<128xi1>
    } {hls.latency = #hls.latency<128>}
    return
  }
  func.func private @dataflow_node_23(%arg0: memref<3072xf8E4M3>, %arg1: memref<16x128x64xf8E4M3>) {
    %c1024 = arith.constant 1024 : index
    %c64 = arith.constant 64 : index
    %c128 = arith.constant 128 : index
    %c1 = arith.constant 1 : index
    %c16 = arith.constant 16 : index
    %c0 = arith.constant 0 : index
    scf.for %arg2 = %c0 to %c16 step %c1 {
      %0 = arith.muli %arg2, %c64 overflow<nsw> : index
      scf.for %arg3 = %c0 to %c128 step %c1 {
        scf.for %arg4 = %c0 to %c64 step %c1 {
          %1 = arith.addi %0, %arg4 overflow<nsw> : index
          %2 = arith.addi %1, %c1024 : index
          %3 = memref.load %arg0[%2] : memref<3072xf8E4M3>
          memref.store %3, %arg1[%arg2, %arg3, %arg4] : memref<16x128x64xf8E4M3>
        } {hls.latency = #hls.latency<64>}
      } {hls.latency = #hls.latency<8192>}
    } {hls.latency = #hls.latency<131072>}
    return
  }
  func.func private @dataflow_node_24(%arg0: memref<3072xf8E4M3>, %arg1: memref<16x128x64xf8E4M3>) {
    %c2048 = arith.constant 2048 : index
    %c64 = arith.constant 64 : index
    %c128 = arith.constant 128 : index
    %c1 = arith.constant 1 : index
    %c16 = arith.constant 16 : index
    %c0 = arith.constant 0 : index
    scf.for %arg2 = %c0 to %c16 step %c1 {
      %0 = arith.muli %arg2, %c64 overflow<nsw> : index
      scf.for %arg3 = %c0 to %c128 step %c1 {
        scf.for %arg4 = %c0 to %c64 step %c1 {
          %1 = arith.addi %0, %arg4 overflow<nsw> : index
          %2 = arith.addi %1, %c2048 : index
          %3 = memref.load %arg0[%2] : memref<3072xf8E4M3>
          memref.store %3, %arg1[%arg2, %arg3, %arg4] : memref<16x128x64xf8E4M3>
        } {hls.latency = #hls.latency<64>}
      } {hls.latency = #hls.latency<8192>}
    } {hls.latency = #hls.latency<131072>}
    return
  }
  func.func private @dataflow_node_25(%arg0: memref<128xi1> {hls.partition = #hls.partition<[cyclic], [2]>}, %arg1: memref<16x128x64xf8E4M3> {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 2, 32]>}, %arg2: memref<1x16x128x64xf8E4M3> {hls.partition = #hls.partition<[none, none, cyclic, cyclic], [0, 0, 2, 32]>}, %arg3: memref<16x128x64xf8E4M3> {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 2, 32]>}) {
    %c-1 = arith.constant -1 : index
    %c2 = arith.constant 2 : index
    %c1 = arith.constant 1 : index
    %c32 = arith.constant 32 : index
    %c128 = arith.constant 128 : index
    %c16 = arith.constant 16 : index
    %c64 = arith.constant 64 : index
    %c0 = arith.constant 0 : index
    scf.for %arg4 = %c0 to %c64 step %c32 {
      scf.for %arg5 = %c0 to %c16 step %c1 {
        %0 = arith.divsi %arg5, %c16 : index
        %1 = arith.muli %0, %c16 : index
        %2 = arith.cmpi ne, %arg5, %1 : index
        %3 = arith.cmpi slt, %arg5, %c0 : index
        %4 = arith.andi %2, %3 : i1
        %5 = arith.addi %0, %c-1 : index
        %6 = arith.select %4, %5, %0 : index
        %7 = arith.remsi %arg5, %c16 : index
        %8 = arith.cmpi slt, %7, %c0 : index
        %9 = arith.addi %7, %c16 overflow<nsw> : index
        %10 = arith.select %8, %9, %7 : index
        scf.for %arg6 = %c0 to %c128 step %c2 {
          %alloc = memref.alloc() {hls.partition = #hls.partition<[cyclic], [2]>} : memref<2xi1>
          scf.for %arg7 = %c0 to %c2 step %c1 {
            %11 = arith.addi %arg6, %arg7 : index
            %12 = memref.load %arg0[%11] : memref<128xi1>
            memref.store %12, %alloc[%arg7] : memref<2xi1>
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<2>}
          %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 2, 32]>} : memref<1x2x32xf8E4M3>
          scf.for %arg7 = %c0 to %c2 step %c1 {
            %11 = arith.addi %arg6, %arg7 : index
            scf.for %arg8 = %c0 to %c32 step %c1 {
              %12 = arith.addi %arg4, %arg8 : index
              %13 = memref.load %arg1[%arg5, %11, %12] : memref<16x128x64xf8E4M3>
              memref.store %13, %alloc_0[%c0, %arg7, %arg8] : memref<1x2x32xf8E4M3>
            } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<2>}
          %alloc_1 = memref.alloc() {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 2, 32]>} : memref<1x2x32xf8E4M3>
          scf.for %arg7 = %c0 to %c2 step %c1 {
            %11 = arith.addi %arg6, %arg7 : index
            scf.for %arg8 = %c0 to %c32 step %c1 {
              %12 = arith.addi %arg4, %arg8 : index
              %13 = memref.load %arg2[%6, %10, %11, %12] : memref<1x16x128x64xf8E4M3>
              memref.store %13, %alloc_1[%c0, %arg7, %arg8] : memref<1x2x32xf8E4M3>
            } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<2>}
          %alloc_2 = memref.alloc() {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 2, 32]>} : memref<1x2x32xf8E4M3>
          scf.for %arg7 = %c0 to %c2 step %c1 {
            %11 = arith.addi %arg6, %arg7 : index
            scf.for %arg8 = %c0 to %c32 step %c1 {
              %12 = arith.addi %arg4, %arg8 : index
              %13 = memref.load %arg3[%arg5, %11, %12] : memref<16x128x64xf8E4M3>
              memref.store %13, %alloc_2[%c0, %arg7, %arg8] : memref<1x2x32xf8E4M3>
            } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<2>}
          func.call @shared_kernel_4(%alloc, %alloc_0, %alloc_1, %alloc_2) : (memref<2xi1>, memref<1x2x32xf8E4M3>, memref<1x2x32xf8E4M3>, memref<1x2x32xf8E4M3>) -> ()
          scf.for %arg7 = %c0 to %c2 step %c1 {
            %11 = arith.addi %arg6, %arg7 : index
            scf.for %arg8 = %c0 to %c32 step %c1 {
              %12 = memref.load %alloc_2[%c0, %arg7, %arg8] : memref<1x2x32xf8E4M3>
              %13 = arith.addi %arg4, %arg8 : index
              memref.store %12, %arg3[%arg5, %11, %13] : memref<16x128x64xf8E4M3>
            } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<2>}
        } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<64>}
      } {hls.latency = #hls.latency<1024>}
    } {hls.latency = #hls.latency<2048>}
    return
  }
  func.func private @dataflow_node_26(%arg0: memref<128xi1> {hls.partition = #hls.partition<[cyclic], [2]>}, %arg1: memref<16x128x64xf8E4M3> {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 2, 32]>}, %arg2: memref<1x16x128x64xf8E4M3> {hls.partition = #hls.partition<[none, none, cyclic, cyclic], [0, 0, 2, 32]>}, %arg3: memref<16x128x64xf8E4M3> {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 2, 32]>}) {
    %c-1 = arith.constant -1 : index
    %c2 = arith.constant 2 : index
    %c1 = arith.constant 1 : index
    %c32 = arith.constant 32 : index
    %c128 = arith.constant 128 : index
    %c16 = arith.constant 16 : index
    %c64 = arith.constant 64 : index
    %c0 = arith.constant 0 : index
    scf.for %arg4 = %c0 to %c64 step %c32 {
      scf.for %arg5 = %c0 to %c16 step %c1 {
        %0 = arith.divsi %arg5, %c16 : index
        %1 = arith.muli %0, %c16 : index
        %2 = arith.cmpi ne, %arg5, %1 : index
        %3 = arith.cmpi slt, %arg5, %c0 : index
        %4 = arith.andi %2, %3 : i1
        %5 = arith.addi %0, %c-1 : index
        %6 = arith.select %4, %5, %0 : index
        %7 = arith.remsi %arg5, %c16 : index
        %8 = arith.cmpi slt, %7, %c0 : index
        %9 = arith.addi %7, %c16 overflow<nsw> : index
        %10 = arith.select %8, %9, %7 : index
        scf.for %arg6 = %c0 to %c128 step %c2 {
          %alloc = memref.alloc() {hls.partition = #hls.partition<[cyclic], [2]>} : memref<2xi1>
          scf.for %arg7 = %c0 to %c2 step %c1 {
            %11 = arith.addi %arg6, %arg7 : index
            %12 = memref.load %arg0[%11] : memref<128xi1>
            memref.store %12, %alloc[%arg7] : memref<2xi1>
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<2>}
          %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 2, 32]>} : memref<1x2x32xf8E4M3>
          scf.for %arg7 = %c0 to %c2 step %c1 {
            %11 = arith.addi %arg6, %arg7 : index
            scf.for %arg8 = %c0 to %c32 step %c1 {
              %12 = arith.addi %arg4, %arg8 : index
              %13 = memref.load %arg1[%arg5, %11, %12] : memref<16x128x64xf8E4M3>
              memref.store %13, %alloc_0[%c0, %arg7, %arg8] : memref<1x2x32xf8E4M3>
            } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<2>}
          %alloc_1 = memref.alloc() {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 2, 32]>} : memref<1x2x32xf8E4M3>
          scf.for %arg7 = %c0 to %c2 step %c1 {
            %11 = arith.addi %arg6, %arg7 : index
            scf.for %arg8 = %c0 to %c32 step %c1 {
              %12 = arith.addi %arg4, %arg8 : index
              %13 = memref.load %arg2[%6, %10, %11, %12] : memref<1x16x128x64xf8E4M3>
              memref.store %13, %alloc_1[%c0, %arg7, %arg8] : memref<1x2x32xf8E4M3>
            } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<2>}
          %alloc_2 = memref.alloc() {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 2, 32]>} : memref<1x2x32xf8E4M3>
          scf.for %arg7 = %c0 to %c2 step %c1 {
            %11 = arith.addi %arg6, %arg7 : index
            scf.for %arg8 = %c0 to %c32 step %c1 {
              %12 = arith.addi %arg4, %arg8 : index
              %13 = memref.load %arg3[%arg5, %11, %12] : memref<16x128x64xf8E4M3>
              memref.store %13, %alloc_2[%c0, %arg7, %arg8] : memref<1x2x32xf8E4M3>
            } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<2>}
          func.call @shared_kernel_4(%alloc, %alloc_0, %alloc_1, %alloc_2) : (memref<2xi1>, memref<1x2x32xf8E4M3>, memref<1x2x32xf8E4M3>, memref<1x2x32xf8E4M3>) -> ()
          scf.for %arg7 = %c0 to %c2 step %c1 {
            %11 = arith.addi %arg6, %arg7 : index
            scf.for %arg8 = %c0 to %c32 step %c1 {
              %12 = memref.load %alloc_2[%c0, %arg7, %arg8] : memref<1x2x32xf8E4M3>
              %13 = arith.addi %arg4, %arg8 : index
              memref.store %12, %arg3[%arg5, %11, %13] : memref<16x128x64xf8E4M3>
            } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<2>}
        } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<64>}
      } {hls.latency = #hls.latency<1024>}
    } {hls.latency = #hls.latency<2048>}
    return
  }
  func.func private @dataflow_node_27(%arg0: memref<16x128x64xf8E4M3>, %arg1: memref<1x16x64x128xf8E4M3>) {
    %c128 = arith.constant 128 : index
    %c64 = arith.constant 64 : index
    %c16 = arith.constant 16 : index
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    scf.for %arg2 = %c0 to %c16 step %c1 {
      scf.for %arg3 = %c0 to %c64 step %c1 {
        scf.for %arg4 = %c0 to %c128 step %c1 {
          %0 = memref.load %arg0[%arg2, %arg4, %arg3] : memref<16x128x64xf8E4M3>
          memref.store %0, %arg1[%c0, %arg2, %arg3, %arg4] : memref<1x16x64x128xf8E4M3>
        } {hls.latency = #hls.latency<128>}
      } {hls.latency = #hls.latency<8192>}
    } {hls.latency = #hls.latency<131072>}
    return
  }
  func.func private @dataflow_node_28(%arg0: memref<16x1x128xf32>) {
    %c128 = arith.constant 128 : index
    %cst = arith.constant 0.000000e+00 : f32
    %c0 = arith.constant 0 : index
    %c16 = arith.constant 16 : index
    %c1 = arith.constant 1 : index
    scf.for %arg1 = %c0 to %c16 step %c1 {
      scf.for %arg2 = %c0 to %c128 step %c1 {
        memref.store %cst, %arg0[%arg1, %c0, %arg2] : memref<16x1x128xf32>
      } {hls.latency = #hls.latency<128>}
    } {hls.latency = #hls.latency<2048>}
    return
  }
  func.func private @dataflow_node_29(%arg0: memref<3072xf8E4M3>, %arg1: memref<1x16x64x128xf8E4M3> {hls.partition = #hls.partition<[none, none, cyclic, cyclic], [0, 0, 8, 32]>}, %arg2: memref<16x1x128xf32> {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 32]>}) {
    %c-1 = arith.constant -1 : index
    %c32 = arith.constant 32 : index
    %c8 = arith.constant 8 : index
    %c128 = arith.constant 128 : index
    %c1 = arith.constant 1 : index
    %c16 = arith.constant 16 : index
    %c64 = arith.constant 64 : index
    %c0 = arith.constant 0 : index
    scf.for %arg3 = %c0 to %c64 step %c8 {
      scf.for %arg4 = %c0 to %c16 step %c1 {
        %0 = arith.muli %arg4, %c64 overflow<nsw> : index
        %1 = arith.divsi %arg4, %c16 : index
        %2 = arith.muli %1, %c16 : index
        %3 = arith.cmpi ne, %arg4, %2 : index
        %4 = arith.cmpi slt, %arg4, %c0 : index
        %5 = arith.andi %3, %4 : i1
        %6 = arith.addi %1, %c-1 : index
        %7 = arith.select %5, %6, %1 : index
        %8 = arith.remsi %arg4, %c16 : index
        %9 = arith.cmpi slt, %8, %c0 : index
        %10 = arith.addi %8, %c16 overflow<nsw> : index
        %11 = arith.select %9, %10, %8 : index
        scf.for %arg5 = %c0 to %c128 step %c32 {
          %alloc = memref.alloc() {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 8]>} : memref<1x1x8xf8E4M3>
          scf.for %arg6 = %c0 to %c8 step %c1 {
            %12 = arith.addi %arg3, %arg6 : index
            %13 = arith.addi %0, %12 overflow<nsw> : index
            %14 = memref.load %arg0[%13] : memref<3072xf8E4M3>
            memref.store %14, %alloc[%c0, %c0, %arg6] : memref<1x1x8xf8E4M3>
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<8>}
          %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 8, 32]>} : memref<1x8x32xf8E4M3>
          scf.for %arg6 = %c0 to %c8 step %c1 {
            %12 = arith.addi %arg3, %arg6 : index
            scf.for %arg7 = %c0 to %c32 step %c1 {
              %13 = arith.addi %arg5, %arg7 : index
              %14 = memref.load %arg1[%7, %11, %12, %13] : memref<1x16x64x128xf8E4M3>
              memref.store %14, %alloc_0[%c0, %arg6, %arg7] : memref<1x8x32xf8E4M3>
            } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<8>}
          %alloc_1 = memref.alloc() {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 32]>} : memref<1x1x32xf32>
          scf.for %arg6 = %c0 to %c32 step %c1 {
            %12 = arith.addi %arg5, %arg6 : index
            %13 = memref.load %arg2[%arg4, %c0, %12] : memref<16x1x128xf32>
            memref.store %13, %alloc_1[%c0, %c0, %arg6] : memref<1x1x32xf32>
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
          func.call @shared_kernel_2(%alloc, %alloc_0, %alloc_1) : (memref<1x1x8xf8E4M3>, memref<1x8x32xf8E4M3>, memref<1x1x32xf32>) -> ()
          scf.for %arg6 = %c0 to %c32 step %c1 {
            %12 = memref.load %alloc_1[%c0, %c0, %arg6] : memref<1x1x32xf32>
            %13 = arith.addi %arg5, %arg6 : index
            memref.store %12, %arg2[%arg4, %c0, %13] : memref<16x1x128xf32>
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
        } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<4>}
      } {hls.latency = #hls.latency<64>}
    } {hls.latency = #hls.latency<512>}
    return
  }
  func.func private @dataflow_node_30(%arg0: memref<16x1x128xf32> {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 32]>}, %arg1: memref<16x128xf8E4M3> {hls.partition = #hls.partition<[none, cyclic], [0, 32]>}) {
    %c1 = arith.constant 1 : index
    %c32 = arith.constant 32 : index
    %c16 = arith.constant 16 : index
    %c128 = arith.constant 128 : index
    %c0 = arith.constant 0 : index
    scf.for %arg2 = %c0 to %c128 step %c32 {
      scf.for %arg3 = %c0 to %c16 step %c1 {
        %alloc = memref.alloc() {hls.partition = #hls.partition<[none, cyclic], [0, 32]>} : memref<1x32xf32>
        scf.for %arg4 = %c0 to %c32 step %c1 {
          %0 = arith.addi %arg2, %arg4 : index
          %1 = memref.load %arg0[%arg3, %c0, %0] : memref<16x1x128xf32>
          memref.store %1, %alloc[%c0, %arg4] : memref<1x32xf32>
        } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
        %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[none, cyclic], [0, 32]>} : memref<1x32xf8E4M3>
        scf.for %arg4 = %c0 to %c32 step %c1 {
          %0 = arith.addi %arg2, %arg4 : index
          %1 = memref.load %arg1[%arg3, %0] : memref<16x128xf8E4M3>
          memref.store %1, %alloc_0[%c0, %arg4] : memref<1x32xf8E4M3>
        } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
        func.call @shared_kernel_5(%alloc, %alloc_0) : (memref<1x32xf32>, memref<1x32xf8E4M3>) -> ()
        scf.for %arg4 = %c0 to %c32 step %c1 {
          %0 = memref.load %alloc_0[%c0, %arg4] : memref<1x32xf8E4M3>
          %1 = arith.addi %arg2, %arg4 : index
          memref.store %0, %arg1[%arg3, %1] : memref<16x128xf8E4M3>
        } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<16>}
    } {hls.latency = #hls.latency<64>}
    return
  }
  func.func private @dataflow_node_31(%arg0: memref<16x128xf8E4M3>, %arg1: memref<16x128xf8E4M3>) {
    %c128 = arith.constant 128 : index
    %cst = arith.constant 1.250000e-01 : f8E4M3
    %c0 = arith.constant 0 : index
    %c16 = arith.constant 16 : index
    %c1 = arith.constant 1 : index
    scf.for %arg2 = %c0 to %c16 step %c1 {
      scf.for %arg3 = %c0 to %c128 step %c1 {
        %0 = memref.load %arg0[%arg2, %arg3] : memref<16x128xf8E4M3>
        %1 = arith.mulf %0, %cst : f8E4M3
        memref.store %1, %arg1[%arg2, %arg3] : memref<16x128xf8E4M3>
      } {hls.latency = #hls.latency<128>}
    } {hls.latency = #hls.latency<2048>}
    return
  }
  func.func private @dataflow_node_32(%arg0: memref<1x1x1024x1024xi1>, %arg1: memref<16x128xf8E4M3>, %arg2: memref<16x128xf8E4M3>) {
    %c127 = arith.constant 127 : index
    %c128 = arith.constant 128 : index
    %c1 = arith.constant 1 : index
    %c16 = arith.constant 16 : index
    %c0 = arith.constant 0 : index
    %cst = arith.constant 0xF8 : f8E4M3
    scf.for %arg3 = %c0 to %c16 step %c1 {
      scf.for %arg4 = %c0 to %c128 step %c1 {
        %0 = memref.load %arg0[%c0, %c0, %c127, %arg4] : memref<1x1x1024x1024xi1>
        %1 = memref.load %arg1[%arg3, %arg4] : memref<16x128xf8E4M3>
        %2 = arith.select %0, %1, %cst : f8E4M3
        memref.store %2, %arg2[%arg3, %arg4] : memref<16x128xf8E4M3>
      } {hls.latency = #hls.latency<128>}
    } {hls.latency = #hls.latency<2048>}
    return
  }
  func.func private @dataflow_node_33(%arg0: memref<16x128xf8E4M3>, %arg1: memref<1x1x1x128xf8E4M3>, %arg2: memref<16x128xf8E4M3>) {
    %c-1 = arith.constant -1 : index
    %c128 = arith.constant 128 : index
    %c1 = arith.constant 1 : index
    %c16 = arith.constant 16 : index
    %c0 = arith.constant 0 : index
    scf.for %arg3 = %c0 to %c16 step %c1 {
      scf.for %arg4 = %c0 to %c128 step %c1 {
        %0 = memref.load %arg0[%arg3, %arg4] : memref<16x128xf8E4M3>
        %1 = arith.divsi %arg4, %c128 : index
        %2 = arith.muli %1, %c128 : index
        %3 = arith.cmpi ne, %arg4, %2 : index
        %4 = arith.cmpi slt, %arg4, %c0 : index
        %5 = arith.andi %3, %4 : i1
        %6 = arith.addi %1, %c-1 : index
        %7 = arith.select %5, %6, %1 : index
        %8 = arith.remsi %arg4, %c128 : index
        %9 = arith.cmpi slt, %8, %c0 : index
        %10 = arith.addi %8, %c128 overflow<nsw> : index
        %11 = arith.select %9, %10, %8 : index
        %12 = arith.divsi %11, %c128 : index
        %13 = arith.remsi %arg4, %c128 : index
        %14 = arith.cmpi slt, %13, %c0 : index
        %15 = arith.addi %13, %c128 overflow<nsw> : index
        %16 = arith.select %14, %15, %13 : index
        %17 = arith.divsi %16, %c128 : index
        %18 = arith.remsi %arg4, %c128 : index
        %19 = arith.cmpi slt, %18, %c0 : index
        %20 = arith.addi %18, %c128 overflow<nsw> : index
        %21 = arith.select %19, %20, %18 : index
        %22 = memref.load %arg1[%7, %12, %17, %21] : memref<1x1x1x128xf8E4M3>
        %23 = arith.addf %0, %22 : f8E4M3
        memref.store %23, %arg2[%arg3, %arg4] : memref<16x128xf8E4M3>
      } {hls.latency = #hls.latency<128>}
    } {hls.latency = #hls.latency<2048>}
    return
  }
  func.func private @dataflow_node_34(%arg0: memref<16x128xf8E4M3>, %arg1: memref<16x128xf32>) {
    %c128 = arith.constant 128 : index
    %c0 = arith.constant 0 : index
    %c16 = arith.constant 16 : index
    %c1 = arith.constant 1 : index
    scf.for %arg2 = %c0 to %c16 step %c1 {
      scf.for %arg3 = %c0 to %c128 step %c1 {
        %0 = memref.load %arg0[%arg2, %arg3] : memref<16x128xf8E4M3>
        %1 = arith.extf %0 : f8E4M3 to f32
        memref.store %1, %arg1[%arg2, %arg3] : memref<16x128xf32>
      } {hls.latency = #hls.latency<128>}
    } {hls.latency = #hls.latency<2048>}
    return
  }
  func.func private @dataflow_node_35(%arg0: memref<1x16x1xf32>) {
    %c16 = arith.constant 16 : index
    %cst = arith.constant -3.40282347E+38 : f32
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    scf.for %arg1 = %c0 to %c16 step %c1 {
      memref.store %cst, %arg0[%c0, %arg1, %c0] : memref<1x16x1xf32>
    } {hls.latency = #hls.latency<16>}
    return
  }
  func.func private @dataflow_node_36(%arg0: memref<16x128xf32>, %arg1: memref<1x16x1xf32>) {
    %c4 = arith.constant 4 : index
    %c128 = arith.constant 128 : index
    %c2 = arith.constant 2 : index
    %c16 = arith.constant 16 : index
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    %cst = arith.constant -3.40282347E+38 : f32
    %alloc = memref.alloc() : memref<2xf32>
    scf.for %arg2 = %c0 to %c16 step %c1 {
      scf.for %arg3 = %c0 to %c2 step %c1 {
        memref.store %cst, %alloc[%arg3] : memref<2xf32>
      } {hls.latency = #hls.latency<2>}
      scf.for %arg3 = %c0 to %c128 step %c1 {
        %11 = memref.load %arg0[%arg2, %arg3] : memref<16x128xf32>
        %12 = arith.remsi %arg3, %c4 : index
        %13 = arith.cmpi slt, %12, %c0 : index
        %14 = arith.addi %12, %c4 : index
        %15 = arith.select %13, %14, %12 : index
        %16 = memref.load %alloc[%15] : memref<2xf32>
        %17 = arith.cmpf ugt, %11, %16 : f32
        %18 = arith.select %17, %11, %16 : f32
        %19 = arith.cmpf uno, %16, %16 : f32
        %20 = arith.select %19, %16, %18 : f32
        %21 = arith.remsi %arg3, %c4 : index
        %22 = arith.cmpi slt, %21, %c0 : index
        %23 = arith.addi %21, %c4 : index
        %24 = arith.select %22, %23, %21 : index
        memref.store %20, %alloc[%24] : memref<2xf32>
      } {hls.latency = #hls.latency<128>}
      %0 = memref.load %arg1[%c0, %arg2, %c0] : memref<1x16x1xf32>
      %1 = memref.load %alloc[%c0] : memref<2xf32>
      %2 = memref.load %alloc[%c1] : memref<2xf32>
      %3 = arith.cmpf ugt, %1, %2 : f32
      %4 = arith.select %3, %1, %2 : f32
      %5 = arith.cmpf uno, %2, %2 : f32
      %6 = arith.select %5, %2, %4 : f32
      %7 = arith.cmpf ugt, %6, %0 : f32
      %8 = arith.select %7, %6, %0 : f32
      %9 = arith.cmpf uno, %0, %0 : f32
      %10 = arith.select %9, %0, %8 : f32
      memref.store %10, %arg1[%c0, %arg2, %c0] : memref<1x16x1xf32>
    } {hls.latency = #hls.latency<2080>}
    return
  }
  func.func private @dataflow_node_37(%arg0: memref<16x128xf32>, %arg1: memref<1x16x1xf32>, %arg2: memref<16x128xf32>) {
    %c-1 = arith.constant -1 : index
    %c128 = arith.constant 128 : index
    %c1 = arith.constant 1 : index
    %c16 = arith.constant 16 : index
    %c0 = arith.constant 0 : index
    scf.for %arg3 = %c0 to %c16 step %c1 {
      %0 = arith.divsi %arg3, %c16 : index
      %1 = arith.muli %0, %c16 : index
      %2 = arith.cmpi ne, %arg3, %1 : index
      %3 = arith.cmpi slt, %arg3, %c0 : index
      %4 = arith.andi %2, %3 : i1
      %5 = arith.addi %0, %c-1 : index
      %6 = arith.select %4, %5, %0 : index
      %7 = arith.remsi %arg3, %c16 : index
      %8 = arith.cmpi slt, %7, %c0 : index
      %9 = arith.addi %7, %c16 overflow<nsw> : index
      %10 = arith.select %8, %9, %7 : index
      scf.for %arg4 = %c0 to %c128 step %c1 {
        %11 = memref.load %arg0[%arg3, %arg4] : memref<16x128xf32>
        %12 = memref.load %arg1[%6, %10, %c0] : memref<1x16x1xf32>
        %13 = arith.subf %11, %12 : f32
        memref.store %13, %arg2[%arg3, %arg4] : memref<16x128xf32>
      } {hls.latency = #hls.latency<128>}
    } {hls.latency = #hls.latency<2048>}
    return
  }
  func.func private @dataflow_node_38(%arg0: memref<16x128xf32>, %arg1: memref<16x128xf32>) {
    %c128 = arith.constant 128 : index
    %c0 = arith.constant 0 : index
    %c16 = arith.constant 16 : index
    %c1 = arith.constant 1 : index
    scf.for %arg2 = %c0 to %c16 step %c1 {
      scf.for %arg3 = %c0 to %c128 step %c1 {
        %0 = memref.load %arg0[%arg2, %arg3] : memref<16x128xf32>
        %1 = math.exp %0 : f32
        memref.store %1, %arg1[%arg2, %arg3] : memref<16x128xf32>
      } {hls.latency = #hls.latency<128>}
    } {hls.latency = #hls.latency<2048>}
    return
  }
  func.func private @dataflow_node_39(%arg0: memref<1x16x1xf32>) {
    %c16 = arith.constant 16 : index
    %cst = arith.constant 0.000000e+00 : f32
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    scf.for %arg1 = %c0 to %c16 step %c1 {
      memref.store %cst, %arg0[%c0, %arg1, %c0] : memref<1x16x1xf32>
    } {hls.latency = #hls.latency<16>}
    return
  }
  func.func private @dataflow_node_40(%arg0: memref<16x128xf32>, %arg1: memref<1x16x1xf32>) {
    %c4 = arith.constant 4 : index
    %c128 = arith.constant 128 : index
    %c2 = arith.constant 2 : index
    %c16 = arith.constant 16 : index
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    %cst = arith.constant 0.000000e+00 : f32
    %alloc = memref.alloc() : memref<2xf32>
    scf.for %arg2 = %c0 to %c16 step %c1 {
      scf.for %arg3 = %c0 to %c2 step %c1 {
        memref.store %cst, %alloc[%arg3] : memref<2xf32>
      } {hls.latency = #hls.latency<2>}
      scf.for %arg3 = %c0 to %c128 step %c1 {
        %5 = memref.load %arg0[%arg2, %arg3] : memref<16x128xf32>
        %6 = arith.remsi %arg3, %c4 : index
        %7 = arith.cmpi slt, %6, %c0 : index
        %8 = arith.addi %6, %c4 : index
        %9 = arith.select %7, %8, %6 : index
        %10 = memref.load %alloc[%9] : memref<2xf32>
        %11 = arith.addf %5, %10 : f32
        %12 = arith.remsi %arg3, %c4 : index
        %13 = arith.cmpi slt, %12, %c0 : index
        %14 = arith.addi %12, %c4 : index
        %15 = arith.select %13, %14, %12 : index
        memref.store %11, %alloc[%15] : memref<2xf32>
      } {hls.latency = #hls.latency<128>}
      %0 = memref.load %arg1[%c0, %arg2, %c0] : memref<1x16x1xf32>
      %1 = memref.load %alloc[%c0] : memref<2xf32>
      %2 = memref.load %alloc[%c1] : memref<2xf32>
      %3 = arith.addf %1, %2 : f32
      %4 = arith.addf %3, %0 : f32
      memref.store %4, %arg1[%c0, %arg2, %c0] : memref<1x16x1xf32>
    } {hls.latency = #hls.latency<2080>}
    return
  }
  func.func private @dataflow_node_41(%arg0: memref<1x16x1xf32>, %arg1: memref<16xf32>) {
    %c-1 = arith.constant -1 : index
    %c1 = arith.constant 1 : index
    %c16 = arith.constant 16 : index
    %c0 = arith.constant 0 : index
    %cst = arith.constant 1.000000e+00 : f32
    scf.for %arg2 = %c0 to %c16 step %c1 {
      %0 = arith.divsi %arg2, %c16 : index
      %1 = arith.muli %0, %c16 : index
      %2 = arith.cmpi ne, %arg2, %1 : index
      %3 = arith.cmpi slt, %arg2, %c0 : index
      %4 = arith.andi %2, %3 : i1
      %5 = arith.addi %0, %c-1 : index
      %6 = arith.select %4, %5, %0 : index
      %7 = arith.remsi %arg2, %c16 : index
      %8 = arith.cmpi slt, %7, %c0 : index
      %9 = arith.addi %7, %c16 overflow<nsw> : index
      %10 = arith.select %8, %9, %7 : index
      %11 = memref.load %arg0[%6, %10, %c0] : memref<1x16x1xf32>
      %12 = arith.divf %cst, %11 : f32
      memref.store %12, %arg1[%arg2] : memref<16xf32>
    } {hls.latency = #hls.latency<16>}
    return
  }
  func.func private @dataflow_node_42(%arg0: memref<16x128xf32>, %arg1: memref<16xf32>, %arg2: memref<16x128xf32>) {
    %c128 = arith.constant 128 : index
    %c0 = arith.constant 0 : index
    %c16 = arith.constant 16 : index
    %c1 = arith.constant 1 : index
    scf.for %arg3 = %c0 to %c16 step %c1 {
      scf.for %arg4 = %c0 to %c128 step %c1 {
        %0 = memref.load %arg0[%arg3, %arg4] : memref<16x128xf32>
        %1 = memref.load %arg1[%arg3] : memref<16xf32>
        %2 = arith.mulf %0, %1 : f32
        memref.store %2, %arg2[%arg3, %arg4] : memref<16x128xf32>
      } {hls.latency = #hls.latency<128>}
    } {hls.latency = #hls.latency<2048>}
    return
  }
  func.func private @dataflow_node_43(%arg0: memref<16x128xf32> {hls.partition = #hls.partition<[none, cyclic], [0, 32]>}, %arg1: memref<16x128xf8E4M3> {hls.partition = #hls.partition<[none, cyclic], [0, 32]>}) {
    %c0 = arith.constant 0 : index
    %c128 = arith.constant 128 : index
    %c16 = arith.constant 16 : index
    %c32 = arith.constant 32 : index
    %c1 = arith.constant 1 : index
    scf.for %arg2 = %c0 to %c128 step %c32 {
      scf.for %arg3 = %c0 to %c16 step %c1 {
        %alloc = memref.alloc() {hls.partition = #hls.partition<[none, cyclic], [0, 32]>} : memref<1x32xf32>
        scf.for %arg4 = %c0 to %c32 step %c1 {
          %0 = arith.addi %arg2, %arg4 : index
          %1 = memref.load %arg0[%arg3, %0] : memref<16x128xf32>
          memref.store %1, %alloc[%c0, %arg4] : memref<1x32xf32>
        } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
        %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[none, cyclic], [0, 32]>} : memref<1x32xf8E4M3>
        scf.for %arg4 = %c0 to %c32 step %c1 {
          %0 = arith.addi %arg2, %arg4 : index
          %1 = memref.load %arg1[%arg3, %0] : memref<16x128xf8E4M3>
          memref.store %1, %alloc_0[%c0, %arg4] : memref<1x32xf8E4M3>
        } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
        func.call @shared_kernel_5(%alloc, %alloc_0) : (memref<1x32xf32>, memref<1x32xf8E4M3>) -> ()
        scf.for %arg4 = %c0 to %c32 step %c1 {
          %0 = memref.load %alloc_0[%c0, %arg4] : memref<1x32xf8E4M3>
          %1 = arith.addi %arg2, %arg4 : index
          memref.store %0, %arg1[%arg3, %1] : memref<16x128xf8E4M3>
        } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<16>}
    } {hls.latency = #hls.latency<64>}
    return
  }
  func.func private @dataflow_node_44(%arg0: memref<16x1x64xf32>) {
    %c64 = arith.constant 64 : index
    %cst = arith.constant 0.000000e+00 : f32
    %c0 = arith.constant 0 : index
    %c16 = arith.constant 16 : index
    %c1 = arith.constant 1 : index
    scf.for %arg1 = %c0 to %c16 step %c1 {
      scf.for %arg2 = %c0 to %c64 step %c1 {
        memref.store %cst, %arg0[%arg1, %c0, %arg2] : memref<16x1x64xf32>
      } {hls.latency = #hls.latency<64>}
    } {hls.latency = #hls.latency<1024>}
    return
  }
  func.func private @dataflow_node_45(%arg0: memref<16x128xf8E4M3> {hls.partition = #hls.partition<[none, cyclic], [0, 8]>}, %arg1: memref<16x128x64xf8E4M3> {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 8, 32]>}, %arg2: memref<16x1x64xf32> {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 32]>}) {
    %c32 = arith.constant 32 : index
    %c8 = arith.constant 8 : index
    %c64 = arith.constant 64 : index
    %c1 = arith.constant 1 : index
    %c16 = arith.constant 16 : index
    %c128 = arith.constant 128 : index
    %c0 = arith.constant 0 : index
    scf.for %arg3 = %c0 to %c128 step %c8 {
      scf.for %arg4 = %c0 to %c16 step %c1 {
        scf.for %arg5 = %c0 to %c64 step %c32 {
          %alloc = memref.alloc() {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 8]>} : memref<1x1x8xf8E4M3>
          scf.for %arg6 = %c0 to %c8 step %c1 {
            %0 = arith.addi %arg3, %arg6 : index
            %1 = memref.load %arg0[%arg4, %0] : memref<16x128xf8E4M3>
            memref.store %1, %alloc[%c0, %c0, %arg6] : memref<1x1x8xf8E4M3>
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<8>}
          %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 8, 32]>} : memref<1x8x32xf8E4M3>
          scf.for %arg6 = %c0 to %c8 step %c1 {
            %0 = arith.addi %arg3, %arg6 : index
            scf.for %arg7 = %c0 to %c32 step %c1 {
              %1 = arith.addi %arg5, %arg7 : index
              %2 = memref.load %arg1[%arg4, %0, %1] : memref<16x128x64xf8E4M3>
              memref.store %2, %alloc_0[%c0, %arg6, %arg7] : memref<1x8x32xf8E4M3>
            } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<8>}
          %alloc_1 = memref.alloc() {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 32]>} : memref<1x1x32xf32>
          scf.for %arg6 = %c0 to %c32 step %c1 {
            %0 = arith.addi %arg5, %arg6 : index
            %1 = memref.load %arg2[%arg4, %c0, %0] : memref<16x1x64xf32>
            memref.store %1, %alloc_1[%c0, %c0, %arg6] : memref<1x1x32xf32>
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
          func.call @shared_kernel_2(%alloc, %alloc_0, %alloc_1) : (memref<1x1x8xf8E4M3>, memref<1x8x32xf8E4M3>, memref<1x1x32xf32>) -> ()
          scf.for %arg6 = %c0 to %c32 step %c1 {
            %0 = memref.load %alloc_1[%c0, %c0, %arg6] : memref<1x1x32xf32>
            %1 = arith.addi %arg5, %arg6 : index
            memref.store %0, %arg2[%arg4, %c0, %1] : memref<16x1x64xf32>
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
        } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<2>}
      } {hls.latency = #hls.latency<32>}
    } {hls.latency = #hls.latency<512>}
    return
  }
  func.func private @dataflow_node_46(%arg0: memref<16x1x64xf32> {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 32]>}, %arg1: memref<16x64xf8E4M3> {hls.partition = #hls.partition<[none, cyclic], [0, 32]>}) {
    %c1 = arith.constant 1 : index
    %c32 = arith.constant 32 : index
    %c16 = arith.constant 16 : index
    %c64 = arith.constant 64 : index
    %c0 = arith.constant 0 : index
    scf.for %arg2 = %c0 to %c64 step %c32 {
      scf.for %arg3 = %c0 to %c16 step %c1 {
        %alloc = memref.alloc() {hls.partition = #hls.partition<[none, cyclic], [0, 32]>} : memref<1x32xf32>
        scf.for %arg4 = %c0 to %c32 step %c1 {
          %0 = arith.addi %arg2, %arg4 : index
          %1 = memref.load %arg0[%arg3, %c0, %0] : memref<16x1x64xf32>
          memref.store %1, %alloc[%c0, %arg4] : memref<1x32xf32>
        } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
        %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[none, cyclic], [0, 32]>} : memref<1x32xf8E4M3>
        scf.for %arg4 = %c0 to %c32 step %c1 {
          %0 = arith.addi %arg2, %arg4 : index
          %1 = memref.load %arg1[%arg3, %0] : memref<16x64xf8E4M3>
          memref.store %1, %alloc_0[%c0, %arg4] : memref<1x32xf8E4M3>
        } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
        func.call @shared_kernel_5(%alloc, %alloc_0) : (memref<1x32xf32>, memref<1x32xf8E4M3>) -> ()
        scf.for %arg4 = %c0 to %c32 step %c1 {
          %0 = memref.load %alloc_0[%c0, %arg4] : memref<1x32xf8E4M3>
          %1 = arith.addi %arg2, %arg4 : index
          memref.store %0, %arg1[%arg3, %1] : memref<16x64xf8E4M3>
        } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<16>}
    } {hls.latency = #hls.latency<32>}
    return
  }
  func.func private @dataflow_node_47(%arg0: memref<1x1x1024xf32>) {
    %c1024 = arith.constant 1024 : index
    %cst = arith.constant 0.000000e+00 : f32
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    scf.for %arg1 = %c0 to %c1024 step %c1 {
      memref.store %cst, %arg0[%c0, %c0, %arg1] : memref<1x1x1024xf32>
    } {hls.latency = #hls.latency<1024>}
    return
  }
  func.func private @dataflow_node_48(%arg0: memref<16x64xf8E4M3>, %arg1: memref<1024x1024xf8E4M3> {hls.partition = #hls.partition<[cyclic, cyclic], [8, 32]>}, %arg2: memref<1x1x1024xf32> {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 32]>}) {
    %c-1 = arith.constant -1 : index
    %c64 = arith.constant 64 : index
    %c32 = arith.constant 32 : index
    %c8 = arith.constant 8 : index
    %c1 = arith.constant 1 : index
    %c1024 = arith.constant 1024 : index
    %c0 = arith.constant 0 : index
    scf.for %arg3 = %c0 to %c1024 step %c8 {
      scf.for %arg4 = %c0 to %c1024 step %c32 {
        %alloc = memref.alloc() {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 8]>} : memref<1x1x8xf8E4M3>
        scf.for %arg5 = %c0 to %c8 step %c1 {
          %0 = arith.addi %arg3, %arg5 : index
          %1 = arith.divsi %0, %c64 : index
          %2 = arith.muli %1, %c64 : index
          %3 = arith.cmpi ne, %0, %2 : index
          %4 = arith.cmpi slt, %0, %c0 : index
          %5 = arith.andi %3, %4 : i1
          %6 = arith.addi %1, %c-1 : index
          %7 = arith.select %5, %6, %1 : index
          %8 = arith.remsi %0, %c64 : index
          %9 = arith.cmpi slt, %8, %c0 : index
          %10 = arith.addi %8, %c64 overflow<nsw> : index
          %11 = arith.select %9, %10, %8 : index
          %12 = arith.divsi %11, %c64 : index
          %13 = arith.remsi %0, %c64 : index
          %14 = arith.cmpi slt, %13, %c0 : index
          %15 = arith.addi %13, %c64 overflow<nsw> : index
          %16 = arith.select %14, %15, %13 : index
          %17 = arith.addi %7, %12 overflow<nsw> : index
          %18 = memref.load %arg0[%17, %16] : memref<16x64xf8E4M3>
          memref.store %18, %alloc[%c0, %c0, %arg5] : memref<1x1x8xf8E4M3>
        } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<8>}
        %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 8, 32]>} : memref<1x8x32xf8E4M3>
        scf.for %arg5 = %c0 to %c8 step %c1 {
          %0 = arith.addi %arg3, %arg5 : index
          scf.for %arg6 = %c0 to %c32 step %c1 {
            %1 = arith.addi %arg4, %arg6 : index
            %2 = memref.load %arg1[%0, %1] : memref<1024x1024xf8E4M3>
            memref.store %2, %alloc_0[%c0, %arg5, %arg6] : memref<1x8x32xf8E4M3>
          } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
        } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<8>}
        %alloc_1 = memref.alloc() {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 32]>} : memref<1x1x32xf32>
        scf.for %arg5 = %c0 to %c32 step %c1 {
          %0 = arith.addi %arg4, %arg5 : index
          %1 = memref.load %arg2[%c0, %c0, %0] : memref<1x1x1024xf32>
          memref.store %1, %alloc_1[%c0, %c0, %arg5] : memref<1x1x32xf32>
        } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
        func.call @shared_kernel_2(%alloc, %alloc_0, %alloc_1) : (memref<1x1x8xf8E4M3>, memref<1x8x32xf8E4M3>, memref<1x1x32xf32>) -> ()
        scf.for %arg5 = %c0 to %c32 step %c1 {
          %0 = memref.load %alloc_1[%c0, %c0, %arg5] : memref<1x1x32xf32>
          %1 = arith.addi %arg4, %arg5 : index
          memref.store %0, %arg2[%c0, %c0, %1] : memref<1x1x1024xf32>
        } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<32>}
    } {hls.latency = #hls.latency<4096>}
    return
  }
  func.func private @dataflow_node_49(%arg0: memref<1x1x1024xf32>, %arg1: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) {
    %c-1 = arith.constant -1 : index
    %c1 = arith.constant 1 : index
    %c32 = arith.constant 32 : index
    %c1024 = arith.constant 1024 : index
    %c0 = arith.constant 0 : index
    scf.for %arg2 = %c0 to %c1024 step %c32 {
      %alloc = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf32>
      scf.for %arg3 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg2, %arg3 : index
        %1 = arith.divsi %0, %c1024 : index
        %2 = arith.muli %1, %c1024 : index
        %3 = arith.cmpi ne, %0, %2 : index
        %4 = arith.cmpi slt, %0, %c0 : index
        %5 = arith.andi %3, %4 : i1
        %6 = arith.addi %1, %c-1 : index
        %7 = arith.select %5, %6, %1 : index
        %8 = arith.remsi %0, %c1024 : index
        %9 = arith.cmpi slt, %8, %c0 : index
        %10 = arith.addi %8, %c1024 overflow<nsw> : index
        %11 = arith.select %9, %10, %8 : index
        %12 = arith.divsi %11, %c1024 : index
        %13 = arith.remsi %0, %c1024 : index
        %14 = arith.cmpi slt, %13, %c0 : index
        %15 = arith.addi %13, %c1024 overflow<nsw> : index
        %16 = arith.select %14, %15, %13 : index
        %17 = memref.load %arg0[%7, %12, %16] : memref<1x1x1024xf32>
        memref.store %17, %alloc[%arg3] : memref<32xf32>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg3 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg2, %arg3 : index
        %1 = memref.load %arg1[%0] : memref<1024xf8E4M3>
        memref.store %1, %alloc_0[%arg3] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      func.call @shared_kernel_3(%alloc, %alloc_0) : (memref<32xf32>, memref<32xf8E4M3>) -> ()
      scf.for %arg3 = %c0 to %c32 step %c1 {
        %0 = memref.load %alloc_0[%arg3] : memref<32xf8E4M3>
        %1 = arith.addi %arg2, %arg3 : index
        memref.store %0, %arg1[%1] : memref<1024xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<32>}
    return
  }
  func.func private @dataflow_node_50(%arg0: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg1: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg2: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) {
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    %c1024 = arith.constant 1024 : index
    %c32 = arith.constant 32 : index
    scf.for %arg3 = %c0 to %c1024 step %c32 {
      %alloc = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg0[%0] : memref<1024xf8E4M3>
        memref.store %1, %alloc[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg1[%0] : memref<1024xf8E4M3>
        memref.store %1, %alloc_0[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_1 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg2[%0] : memref<1024xf8E4M3>
        memref.store %1, %alloc_1[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      func.call @shared_kernel_1(%alloc, %alloc_0, %alloc_1) : (memref<32xf8E4M3>, memref<32xf8E4M3>, memref<32xf8E4M3>) -> ()
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = memref.load %alloc_1[%arg4] : memref<32xf8E4M3>
        %1 = arith.addi %arg3, %arg4 : index
        memref.store %0, %arg2[%1] : memref<1024xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<32>}
    return
  }
  func.func private @dataflow_node_51(%arg0: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg1: memref<1x1x1024xf8E4M3>, %arg2: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) {
    %c-1 = arith.constant -1 : index
    %c1 = arith.constant 1 : index
    %c32 = arith.constant 32 : index
    %c1024 = arith.constant 1024 : index
    %c0 = arith.constant 0 : index
    scf.for %arg3 = %c0 to %c1024 step %c32 {
      %alloc = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg0[%0] : memref<1024xf8E4M3>
        memref.store %1, %alloc[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = arith.divsi %0, %c1024 : index
        %2 = arith.muli %1, %c1024 : index
        %3 = arith.cmpi ne, %0, %2 : index
        %4 = arith.cmpi slt, %0, %c0 : index
        %5 = arith.andi %3, %4 : i1
        %6 = arith.addi %1, %c-1 : index
        %7 = arith.select %5, %6, %1 : index
        %8 = arith.remsi %0, %c1024 : index
        %9 = arith.cmpi slt, %8, %c0 : index
        %10 = arith.addi %8, %c1024 overflow<nsw> : index
        %11 = arith.select %9, %10, %8 : index
        %12 = arith.divsi %11, %c1024 : index
        %13 = arith.remsi %0, %c1024 : index
        %14 = arith.cmpi slt, %13, %c0 : index
        %15 = arith.addi %13, %c1024 overflow<nsw> : index
        %16 = arith.select %14, %15, %13 : index
        %17 = memref.load %arg1[%7, %12, %16] : memref<1x1x1024xf8E4M3>
        memref.store %17, %alloc_0[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_1 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg2[%0] : memref<1024xf8E4M3>
        memref.store %1, %alloc_1[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      func.call @shared_kernel_1(%alloc, %alloc_0, %alloc_1) : (memref<32xf8E4M3>, memref<32xf8E4M3>, memref<32xf8E4M3>) -> ()
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = memref.load %alloc_1[%arg4] : memref<32xf8E4M3>
        %1 = arith.addi %arg3, %arg4 : index
        memref.store %0, %arg2[%1] : memref<1024xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<32>}
    return
  }
  func.func private @dataflow_node_52(%arg0: memref<1024xf8E4M3>, %arg1: memref<1x1x1024xf8E4M3>) {
    %c1024 = arith.constant 1024 : index
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    scf.for %arg2 = %c0 to %c1024 step %c1 {
      %0 = memref.load %arg0[%arg2] : memref<1024xf8E4M3>
      memref.store %0, %arg1[%c0, %c0, %arg2] : memref<1x1x1024xf8E4M3>
    } {hls.latency = #hls.latency<1024>}
    return
  }
  func.func private @dataflow_node_53(%arg0: memref<16x128x64xf8E4M3>, %arg1: memref<1x16x128x64xf8E4M3>) {
    %c64 = arith.constant 64 : index
    %c128 = arith.constant 128 : index
    %c16 = arith.constant 16 : index
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    scf.for %arg2 = %c0 to %c16 step %c1 {
      scf.for %arg3 = %c0 to %c128 step %c1 {
        scf.for %arg4 = %c0 to %c64 step %c1 {
          %0 = memref.load %arg0[%arg2, %arg3, %arg4] : memref<16x128x64xf8E4M3>
          memref.store %0, %arg1[%c0, %arg2, %arg3, %arg4] : memref<1x16x128x64xf8E4M3>
        } {hls.latency = #hls.latency<64>}
      } {hls.latency = #hls.latency<8192>}
    } {hls.latency = #hls.latency<131072>}
    return
  }
  func.func private @dataflow_node_54(%arg0: memref<16x128x64xf8E4M3>, %arg1: memref<1x16x128x64xf8E4M3>) {
    %c64 = arith.constant 64 : index
    %c128 = arith.constant 128 : index
    %c16 = arith.constant 16 : index
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    scf.for %arg2 = %c0 to %c16 step %c1 {
      scf.for %arg3 = %c0 to %c128 step %c1 {
        scf.for %arg4 = %c0 to %c64 step %c1 {
          %0 = memref.load %arg0[%arg2, %arg3, %arg4] : memref<16x128x64xf8E4M3>
          memref.store %0, %arg1[%c0, %arg2, %arg3, %arg4] : memref<1x16x128x64xf8E4M3>
        } {hls.latency = #hls.latency<64>}
      } {hls.latency = #hls.latency<8192>}
    } {hls.latency = #hls.latency<131072>}
    return
  }
  func.func @kernel_0_chip0(%arg0: memref<1x1x1024x1024xi1> {llt.input}, %arg1: memref<f8E4M3> {llt.input}, %arg2: memref<1x1x1024x1024xi1> {llt.input}, %arg3: memref<f8E4M3> {llt.input}, %arg4: memref<1x1x1024x1024xi1> {llt.input}, %arg5: memref<f8E4M3> {llt.input}, %arg6: memref<1x1x1024x1024xi1> {llt.input}, %arg7: memref<f8E4M3> {llt.input}, %arg8: memref<1x1x1024x1024xi1> {llt.input}, %arg9: memref<f8E4M3> {llt.input}, %arg10: memref<1x1x1024x1024xi1> {llt.input}, %arg11: memref<f8E4M3> {llt.input}, %arg12: memref<1x1x1024x1024xi1> {llt.input}, %arg13: memref<f8E4M3> {llt.input}, %arg14: memref<1x1x1024x1024xi1> {llt.input}, %arg15: memref<f8E4M3> {llt.input}, %arg16: memref<1x1x1024x1024xi1> {llt.input}, %arg17: memref<f8E4M3> {llt.input}, %arg18: memref<1x1x1024x1024xi1> {llt.input}, %arg19: memref<f8E4M3> {llt.input}, %arg20: memref<1x1x1024x1024xi1> {llt.input}, %arg21: memref<f8E4M3> {llt.input}, %arg22: memref<1x1x1024x1024xi1> {llt.input}, %arg23: memref<f8E4M3> {llt.input}, %arg24: memref<1x1x1024x1024xi1> {llt.input}, %arg25: memref<f8E4M3> {llt.input}, %arg26: memref<1x1x1024x1024xi1> {llt.input}, %arg27: memref<f8E4M3> {llt.input}, %arg28: memref<1x1x1024x1024xi1> {llt.input}, %arg29: memref<f8E4M3> {llt.input}, %arg30: memref<1x1x1024x1024xi1> {llt.input}, %arg31: memref<f8E4M3> {llt.input}, %arg32: memref<1x1x1024x1024xi1> {llt.input}, %arg33: memref<f8E4M3> {llt.input}, %arg34: memref<1x1x1024x1024xi1> {llt.input}, %arg35: memref<f8E4M3> {llt.input}, %arg36: memref<1x1x1024x1024xi1> {llt.input}, %arg37: memref<f8E4M3> {llt.input}, %arg38: memref<1x1x1024x1024xi1> {llt.input}, %arg39: memref<f8E4M3> {llt.input}, %arg40: memref<1x1x1024x1024xi1> {llt.input}, %arg41: memref<f8E4M3> {llt.input}, %arg42: memref<1x1x1024x1024xi1> {llt.input}, %arg43: memref<f8E4M3> {llt.input}, %arg44: memref<1x1x1024x1024xi1> {llt.input}, %arg45: memref<f8E4M3> {llt.input}, %arg46: memref<1x1x1024x1024xi1> {llt.input}, %arg47: memref<f8E4M3> {llt.input}, %arg48: memref<1x1x1024xf8E4M3> {llt.input}, %arg49: memref<1xi32> {llt.input}, %arg50: memref<1x1x1x128xf8E4M3> {llt.input}, %arg51: memref<1x16x128x64xf8E4M3> {hls.partition = #hls.partition<[none, none, cyclic, cyclic], [0, 0, 2, 32]>, llt.input}, %arg52: memref<1x16x128x64xf8E4M3> {hls.partition = #hls.partition<[none, none, cyclic, cyclic], [0, 0, 2, 32]>, llt.input}, %arg53: memref<1x1x1024xf8E4M3> {llt.output}, %arg54: memref<1x1x1024xf8E4M3> {llt.output}, %arg55: memref<1x16x128x64xf8E4M3> {llt.output}, %arg56: memref<1x16x128x64xf8E4M3> {llt.output}, %arg57: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>, memref.values = @__constant_1024xbf16_afdc2952}, %arg58: memref<1024x1024xf8E4M3> {hls.partition = #hls.partition<[cyclic, cyclic], [8, 32]>, memref.values = @__constant_1024x1024xbf16_bf2043af}, %arg59: memref<3072xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>, memref.values = @__constant_3072xbf16_35cfe092}, %arg60: memref<1024x3072xf8E4M3> {hls.partition = #hls.partition<[cyclic, cyclic], [8, 32]>, memref.values = @__constant_1024x3072xbf16_1afe0d7a}, %arg61: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>, memref.values = @__constant_1024xbf16_113d1edf}, %arg62: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>, memref.values = @__constant_1024xbf16_78fbe581}) attributes {hls.latency = #hls.latency<710508>, hls.resource = #hls.resource<bram : 839>, hls.top} {
    %alloc = memref.alloc() {hls.resource = #hls.resource<bram : 0>} : memref<1x1xf32>
    call @dataflow_node_0(%alloc) : (memref<1x1xf32>) -> ()
    call @dataflow_node_1(%arg48, %alloc) : (memref<1x1x1024xf8E4M3>, memref<1x1xf32>) -> ()
    %alloc_0 = memref.alloc() {hls.resource = #hls.resource<bram : 0>} : memref<f8E4M3>
    call @dataflow_node_2(%alloc, %alloc_0) : (memref<1x1xf32>, memref<f8E4M3>) -> ()
    %alloc_1 = memref.alloc() {hls.resource = #hls.resource<bram : 0>} : memref<f8E4M3>
    call @dataflow_node_3(%alloc_0, %alloc_1) : (memref<f8E4M3>, memref<f8E4M3>) -> ()
    %alloc_2 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>, hls.resource = #hls.resource<bram : 0>} : memref<1024xf8E4M3>
    call @dataflow_node_4(%alloc_1, %alloc_2) : (memref<f8E4M3>, memref<1024xf8E4M3>) -> ()
    %alloc_3 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>, hls.resource = #hls.resource<bram : 0>} : memref<1024xf8E4M3>
    call @dataflow_node_5(%arg48, %alloc_2, %alloc_3) : (memref<1x1x1024xf8E4M3>, memref<1024xf8E4M3>, memref<1024xf8E4M3>) -> ()
    call @dataflow_node_6(%alloc_3, %alloc_3, %alloc_2) : (memref<1024xf8E4M3>, memref<1024xf8E4M3>, memref<1024xf8E4M3>) -> ()
    %alloc_4 = memref.alloc() {hls.resource = #hls.resource<bram : 0>} : memref<1x1xf32>
    call @dataflow_node_7(%alloc_4) : (memref<1x1xf32>) -> ()
    call @dataflow_node_8(%alloc_2, %alloc_4) : (memref<1024xf8E4M3>, memref<1x1xf32>) -> ()
    call @dataflow_node_9(%alloc_4, %alloc_0) : (memref<1x1xf32>, memref<f8E4M3>) -> ()
    call @dataflow_node_10(%alloc_0, %alloc_1) : (memref<f8E4M3>, memref<f8E4M3>) -> ()
    call @dataflow_node_11(%alloc_1, %alloc_0) : (memref<f8E4M3>, memref<f8E4M3>) -> ()
    call @dataflow_node_12(%alloc_0, %alloc_1) : (memref<f8E4M3>, memref<f8E4M3>) -> ()
    call @dataflow_node_13(%alloc_1, %alloc_2) : (memref<f8E4M3>, memref<1024xf8E4M3>) -> ()
    %alloc_5 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>, hls.resource = #hls.resource<bram : 0>} : memref<1024xf8E4M3>
    call @dataflow_node_14(%alloc_3, %alloc_2, %alloc_5) : (memref<1024xf8E4M3>, memref<1024xf8E4M3>, memref<1024xf8E4M3>) -> ()
    call @dataflow_node_15(%alloc_5, %arg62, %alloc_2) : (memref<1024xf8E4M3>, memref<1024xf8E4M3>, memref<1024xf8E4M3>) -> ()
    call @dataflow_node_16(%alloc_2, %arg61, %alloc_3) : (memref<1024xf8E4M3>, memref<1024xf8E4M3>, memref<1024xf8E4M3>) -> ()
    %alloc_6 = memref.alloc() {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 32]>, hls.resource = #hls.resource<bram : 32>} : memref<1x1x3072xf32>
    call @dataflow_node_17(%alloc_6) : (memref<1x1x3072xf32>) -> ()
    call @dataflow_node_18(%alloc_3, %arg60, %alloc_6) : (memref<1024xf8E4M3>, memref<1024x3072xf8E4M3>, memref<1x1x3072xf32>) -> ()
    %alloc_7 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>, hls.resource = #hls.resource<bram : 32>} : memref<3072xf8E4M3>
    call @dataflow_node_19(%alloc_6, %alloc_7) : (memref<1x1x3072xf32>, memref<3072xf8E4M3>) -> ()
    %alloc_8 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>, hls.resource = #hls.resource<bram : 32>} : memref<3072xf8E4M3>
    call @dataflow_node_20(%alloc_7, %arg59, %alloc_8) : (memref<3072xf8E4M3>, memref<3072xf8E4M3>, memref<3072xf8E4M3>) -> ()
    %alloc_9 = memref.alloc() {hls.resource = #hls.resource<bram : 0>} : memref<i64>
    call @dataflow_node_21(%arg49, %alloc_9) : (memref<1xi32>, memref<i64>) -> ()
    %alloc_10 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [2]>, hls.resource = #hls.resource<bram : 2>} : memref<128xi1>
    call @dataflow_node_22(%alloc_9, %alloc_10) : (memref<i64>, memref<128xi1>) -> ()
    %alloc_11 = memref.alloc() {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 8, 32]>, hls.resource = #hls.resource<bram : 256>} : memref<16x128x64xf8E4M3>
    call @dataflow_node_23(%alloc_8, %alloc_11) : (memref<3072xf8E4M3>, memref<16x128x64xf8E4M3>) -> ()
    %alloc_12 = memref.alloc() {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 2, 32]>, hls.resource = #hls.resource<bram : 64>} : memref<16x128x64xf8E4M3>
    call @dataflow_node_24(%alloc_8, %alloc_12) : (memref<3072xf8E4M3>, memref<16x128x64xf8E4M3>) -> ()
    %alloc_13 = memref.alloc() {hls.partition = #hls.partition<[none, cyclic, cyclic], [0, 2, 32]>, hls.resource = #hls.resource<bram : 64>} : memref<16x128x64xf8E4M3>
    call @dataflow_node_25(%alloc_10, %alloc_11, %arg51, %alloc_13) : (memref<128xi1>, memref<16x128x64xf8E4M3>, memref<1x16x128x64xf8E4M3>, memref<16x128x64xf8E4M3>) -> ()
    call @dataflow_node_26(%alloc_10, %alloc_12, %arg52, %alloc_11) : (memref<128xi1>, memref<16x128x64xf8E4M3>, memref<1x16x128x64xf8E4M3>, memref<16x128x64xf8E4M3>) -> ()
    %alloc_14 = memref.alloc() {hls.partition = #hls.partition<[none, none, cyclic, cyclic], [0, 0, 8, 32]>, hls.resource = #hls.resource<bram : 256>} : memref<1x16x64x128xf8E4M3>
    call @dataflow_node_27(%alloc_13, %alloc_14) : (memref<16x128x64xf8E4M3>, memref<1x16x64x128xf8E4M3>) -> ()
    %alloc_15 = memref.alloc() {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 32]>, hls.resource = #hls.resource<bram : 32>} : memref<16x1x128xf32>
    call @dataflow_node_28(%alloc_15) : (memref<16x1x128xf32>) -> ()
    call @dataflow_node_29(%alloc_8, %alloc_14, %alloc_15) : (memref<3072xf8E4M3>, memref<1x16x64x128xf8E4M3>, memref<16x1x128xf32>) -> ()
    %alloc_16 = memref.alloc() {hls.partition = #hls.partition<[none, cyclic], [0, 32]>, hls.resource = #hls.resource<bram : 32>} : memref<16x128xf8E4M3>
    call @dataflow_node_30(%alloc_15, %alloc_16) : (memref<16x1x128xf32>, memref<16x128xf8E4M3>) -> ()
    %alloc_17 = memref.alloc() {hls.resource = #hls.resource<bram : 1>} : memref<16x128xf8E4M3>
    call @dataflow_node_31(%alloc_16, %alloc_17) : (memref<16x128xf8E4M3>, memref<16x128xf8E4M3>) -> ()
    call @dataflow_node_32(%arg0, %alloc_17, %alloc_16) : (memref<1x1x1024x1024xi1>, memref<16x128xf8E4M3>, memref<16x128xf8E4M3>) -> ()
    call @dataflow_node_33(%alloc_16, %arg50, %alloc_17) : (memref<16x128xf8E4M3>, memref<1x1x1x128xf8E4M3>, memref<16x128xf8E4M3>) -> ()
    %alloc_18 = memref.alloc() {hls.resource = #hls.resource<bram : 4>} : memref<16x128xf32>
    call @dataflow_node_34(%alloc_17, %alloc_18) : (memref<16x128xf8E4M3>, memref<16x128xf32>) -> ()
    %alloc_19 = memref.alloc() {hls.resource = #hls.resource<bram : 0>} : memref<1x16x1xf32>
    call @dataflow_node_35(%alloc_19) : (memref<1x16x1xf32>) -> ()
    call @dataflow_node_36(%alloc_18, %alloc_19) : (memref<16x128xf32>, memref<1x16x1xf32>) -> ()
    %alloc_20 = memref.alloc() {hls.partition = #hls.partition<[none, cyclic], [0, 32]>, hls.resource = #hls.resource<bram : 32>} : memref<16x128xf32>
    call @dataflow_node_37(%alloc_18, %alloc_19, %alloc_20) : (memref<16x128xf32>, memref<1x16x1xf32>, memref<16x128xf32>) -> ()
    call @dataflow_node_38(%alloc_20, %alloc_18) : (memref<16x128xf32>, memref<16x128xf32>) -> ()
    %alloc_21 = memref.alloc() {hls.resource = #hls.resource<bram : 0>} : memref<1x16x1xf32>
    call @dataflow_node_39(%alloc_21) : (memref<1x16x1xf32>) -> ()
    call @dataflow_node_40(%alloc_18, %alloc_21) : (memref<16x128xf32>, memref<1x16x1xf32>) -> ()
    %alloc_22 = memref.alloc() {hls.resource = #hls.resource<bram : 0>} : memref<16xf32>
    call @dataflow_node_41(%alloc_21, %alloc_22) : (memref<1x16x1xf32>, memref<16xf32>) -> ()
    call @dataflow_node_42(%alloc_18, %alloc_22, %alloc_20) : (memref<16x128xf32>, memref<16xf32>, memref<16x128xf32>) -> ()
    call @dataflow_node_43(%alloc_20, %alloc_16) : (memref<16x128xf32>, memref<16x128xf8E4M3>) -> ()
    %alloc_23 = memref.alloc() {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 32]>, hls.resource = #hls.resource<bram : 0>} : memref<16x1x64xf32>
    call @dataflow_node_44(%alloc_23) : (memref<16x1x64xf32>) -> ()
    call @dataflow_node_45(%alloc_16, %alloc_11, %alloc_23) : (memref<16x128xf8E4M3>, memref<16x128x64xf8E4M3>, memref<16x1x64xf32>) -> ()
    %alloc_24 = memref.alloc() {hls.partition = #hls.partition<[none, cyclic], [0, 32]>, hls.resource = #hls.resource<bram : 0>} : memref<16x64xf8E4M3>
    call @dataflow_node_46(%alloc_23, %alloc_24) : (memref<16x1x64xf32>, memref<16x64xf8E4M3>) -> ()
    %alloc_25 = memref.alloc() {hls.partition = #hls.partition<[none, none, cyclic], [0, 0, 32]>, hls.resource = #hls.resource<bram : 0>} : memref<1x1x1024xf32>
    call @dataflow_node_47(%alloc_25) : (memref<1x1x1024xf32>) -> ()
    call @dataflow_node_48(%alloc_24, %arg58, %alloc_25) : (memref<16x64xf8E4M3>, memref<1024x1024xf8E4M3>, memref<1x1x1024xf32>) -> ()
    call @dataflow_node_49(%alloc_25, %alloc_2) : (memref<1x1x1024xf32>, memref<1024xf8E4M3>) -> ()
    call @dataflow_node_50(%alloc_2, %arg57, %alloc_3) : (memref<1024xf8E4M3>, memref<1024xf8E4M3>, memref<1024xf8E4M3>) -> ()
    call @dataflow_node_51(%alloc_3, %arg48, %alloc_2) : (memref<1024xf8E4M3>, memref<1x1x1024xf8E4M3>, memref<1024xf8E4M3>) -> ()
    call @dataflow_node_52(%alloc_2, %arg53) : (memref<1024xf8E4M3>, memref<1x1x1024xf8E4M3>) -> ()
    call @dataflow_node_53(%alloc_13, %arg55) : (memref<16x128x64xf8E4M3>, memref<1x16x128x64xf8E4M3>) -> ()
    call @dataflow_node_54(%alloc_11, %arg56) : (memref<16x128x64xf8E4M3>, memref<1x16x128x64xf8E4M3>) -> ()
    return
  }
}

