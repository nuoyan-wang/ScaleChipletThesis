module {
  func.func @shared_kernel_2(%arg0: memref<32xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg1: memref<f8E4M3>, %arg2: memref<32xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) attributes {hls.initiation_interval = #hls.initiation_interval<1>, hls.kernel, hls.latency = #hls.latency<0>, hls.resource = #hls.resource<bram : 0>} {
    %c0 = arith.constant 0 : index
    %c32 = arith.constant 32 : index
    %c1 = arith.constant 1 : index
    scf.for %arg3 = %c0 to %c32 step %c1 {
      %0 = memref.load %arg0[%arg3] : memref<32xf8E4M3>
      %1 = memref.load %arg1[] : memref<f8E4M3>
      %2 = arith.mulf %0, %1 : f8E4M3
      memref.store %2, %arg2[%arg3] : memref<32xf8E4M3>
    } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
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
  memref.global "private" constant @__constant_xbf16_f9143c17 : memref<f8E4M3> = dense<1.000000e+00>
  memref.global "private" constant @__constant_xbf16_c9596378 : memref<f8E4M3> = dense<8.125000e-01>
  memref.global "private" constant @__constant_xbf16_c4ae1415 : memref<f8E4M3> = dense<0.000000e+00>
  memref.global "private" constant @__constant_xbf16_aae29fdb : memref<f8E4M3> = dense<4.296880e-02>
  memref.global "private" constant @__constant_xbf16_2058c65b : memref<f8E4M3> = dense<3.000000e+00>
  memref.global "private" constant @__constant_xbf16_1b759226 : memref<f8E4M3> = dense<5.000000e-01>
  memref.global "private" constant @__constant_4096xbf16_32e35d8c : memref<4096xf8E4M3> = dense_resource<torch_tensor_4096_torch.bfloat16>
  memref.global "private" constant @__constant_1024xbf16_fc6692a9 : memref<1024xf8E4M3> = dense_resource<torch_tensor_1024_torch.bfloat16_4>
  memref.global "private" constant @__constant_1024xbf16_4d0f539d : memref<1024xf8E4M3> = dense_resource<torch_tensor_1024_torch.bfloat16_3>
  memref.global "private" constant @__constant_1024x4096xbf16_c14f6a72 : memref<1024x4096xf8E4M3> = dense_resource<torch_tensor_1024_4096_torch.bfloat16>
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
  func.func private @dataflow_node_17(%arg0: memref<1x1x4096xf32>) {
    %c4096 = arith.constant 4096 : index
    %cst = arith.constant 0.000000e+00 : f32
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    scf.for %arg1 = %c0 to %c4096 step %c1 {
      memref.store %cst, %arg0[%c0, %c0, %arg1] : memref<1x1x4096xf32>
    } {hls.latency = #hls.latency<4096>}
    return
  }
  func.func private @dataflow_node_18(%arg0: memref<1024xf8E4M3>, %arg1: memref<1024x4096xf8E4M3>, %arg2: memref<1x1x4096xf32>) {
    %c4 = arith.constant 4 : index
    %c1024 = arith.constant 1024 : index
    %c2 = arith.constant 2 : index
    %c4096 = arith.constant 4096 : index
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    %cst = arith.constant 0.000000e+00 : f32
    %alloc = memref.alloc() : memref<2xf32>
    scf.for %arg3 = %c0 to %c4096 step %c1 {
      scf.for %arg4 = %c0 to %c2 step %c1 {
        memref.store %cst, %alloc[%arg4] : memref<2xf32>
      } {hls.latency = #hls.latency<2>}
      scf.for %arg4 = %c0 to %c1024 step %c1 {
        %5 = memref.load %arg0[%arg4] : memref<1024xf8E4M3>
        %6 = memref.load %arg1[%arg4, %arg3] : memref<1024x4096xf8E4M3>
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
      } {hls.latency = #hls.latency<1024>}
      %0 = memref.load %arg2[%c0, %c0, %arg3] : memref<1x1x4096xf32>
      %1 = memref.load %alloc[%c0] : memref<2xf32>
      %2 = memref.load %alloc[%c1] : memref<2xf32>
      %3 = arith.addf %1, %2 : f32
      %4 = arith.addf %3, %0 : f32
      memref.store %4, %arg2[%c0, %c0, %arg3] : memref<1x1x4096xf32>
    } {hls.latency = #hls.latency<4202496>}
    return
  }
  func.func private @dataflow_node_19(%arg0: memref<1x1x4096xf32>, %arg1: memref<4096xf8E4M3>) {
    %c-1 = arith.constant -1 : index
    %c1 = arith.constant 1 : index
    %c4096 = arith.constant 4096 : index
    %c0 = arith.constant 0 : index
    scf.for %arg2 = %c0 to %c4096 step %c1 {
      %0 = arith.divsi %arg2, %c4096 : index
      %1 = arith.muli %0, %c4096 : index
      %2 = arith.cmpi ne, %arg2, %1 : index
      %3 = arith.cmpi slt, %arg2, %c0 : index
      %4 = arith.andi %2, %3 : i1
      %5 = arith.addi %0, %c-1 : index
      %6 = arith.select %4, %5, %0 : index
      %7 = arith.remsi %arg2, %c4096 : index
      %8 = arith.cmpi slt, %7, %c0 : index
      %9 = arith.addi %7, %c4096 overflow<nsw> : index
      %10 = arith.select %8, %9, %7 : index
      %11 = arith.divsi %10, %c4096 : index
      %12 = arith.remsi %arg2, %c4096 : index
      %13 = arith.cmpi slt, %12, %c0 : index
      %14 = arith.addi %12, %c4096 overflow<nsw> : index
      %15 = arith.select %13, %14, %12 : index
      %16 = memref.load %arg0[%6, %11, %15] : memref<1x1x4096xf32>
      %17 = arith.truncf %16 : f32 to f8E4M3
      memref.store %17, %arg1[%arg2] : memref<4096xf8E4M3>
    } {hls.latency = #hls.latency<4096>}
    return
  }
  func.func private @dataflow_node_20(%arg0: memref<4096xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg1: memref<4096xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg2: memref<4096xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) {
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    %c4096 = arith.constant 4096 : index
    %c32 = arith.constant 32 : index
    scf.for %arg3 = %c0 to %c4096 step %c32 {
      %alloc = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg0[%0] : memref<4096xf8E4M3>
        memref.store %1, %alloc[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg1[%0] : memref<4096xf8E4M3>
        memref.store %1, %alloc_0[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_1 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg2[%0] : memref<4096xf8E4M3>
        memref.store %1, %alloc_1[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      func.call @shared_kernel_1(%alloc, %alloc_0, %alloc_1) : (memref<32xf8E4M3>, memref<32xf8E4M3>, memref<32xf8E4M3>) -> ()
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = memref.load %alloc_1[%arg4] : memref<32xf8E4M3>
        %1 = arith.addi %arg3, %arg4 : index
        memref.store %0, %arg2[%1] : memref<4096xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<128>}
    return
  }
  func.func private @dataflow_node_21(%arg0: memref<4096xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg1: memref<4096xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) {
    %c1 = arith.constant 1 : index
    %c32 = arith.constant 32 : index
    %c4096 = arith.constant 4096 : index
    %c0 = arith.constant 0 : index
    %0 = memref.get_global @__constant_xbf16_1b759226 : memref<f8E4M3>
    scf.for %arg2 = %c0 to %c4096 step %c32 {
      %alloc = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg3 = %c0 to %c32 step %c1 {
        %1 = arith.addi %arg2, %arg3 : index
        %2 = memref.load %arg0[%1] : memref<4096xf8E4M3>
        memref.store %2, %alloc[%arg3] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg3 = %c0 to %c32 step %c1 {
        %1 = arith.addi %arg2, %arg3 : index
        %2 = memref.load %arg1[%1] : memref<4096xf8E4M3>
        memref.store %2, %alloc_0[%arg3] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      func.call @shared_kernel_2(%alloc, %0, %alloc_0) : (memref<32xf8E4M3>, memref<f8E4M3>, memref<32xf8E4M3>) -> ()
      scf.for %arg3 = %c0 to %c32 step %c1 {
        %1 = memref.load %alloc_0[%arg3] : memref<32xf8E4M3>
        %2 = arith.addi %arg2, %arg3 : index
        memref.store %1, %arg1[%2] : memref<4096xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<128>}
    return
  }
  func.func private @dataflow_node_22(%arg0: memref<4096xf8E4M3>, %arg1: memref<4096xf8E4M3>) {
    %cst = arith.constant 3.000000e+00 : f8E4M3
    %c0 = arith.constant 0 : index
    %c4096 = arith.constant 4096 : index
    %c1 = arith.constant 1 : index
    scf.for %arg2 = %c0 to %c4096 step %c1 {
      %0 = memref.load %arg0[%arg2] : memref<4096xf8E4M3>
      %1 = math.powf %0, %cst : f8E4M3
      memref.store %1, %arg1[%arg2] : memref<4096xf8E4M3>
    } {hls.latency = #hls.latency<4096>}
    return
  }
  func.func private @dataflow_node_23(%arg0: memref<4096xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg1: memref<4096xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) {
    %c1 = arith.constant 1 : index
    %c32 = arith.constant 32 : index
    %c4096 = arith.constant 4096 : index
    %c0 = arith.constant 0 : index
    %0 = memref.get_global @__constant_xbf16_aae29fdb : memref<f8E4M3>
    scf.for %arg2 = %c0 to %c4096 step %c32 {
      %alloc = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg3 = %c0 to %c32 step %c1 {
        %1 = arith.addi %arg2, %arg3 : index
        %2 = memref.load %arg0[%1] : memref<4096xf8E4M3>
        memref.store %2, %alloc[%arg3] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg3 = %c0 to %c32 step %c1 {
        %1 = arith.addi %arg2, %arg3 : index
        %2 = memref.load %arg1[%1] : memref<4096xf8E4M3>
        memref.store %2, %alloc_0[%arg3] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      func.call @shared_kernel_2(%alloc, %0, %alloc_0) : (memref<32xf8E4M3>, memref<f8E4M3>, memref<32xf8E4M3>) -> ()
      scf.for %arg3 = %c0 to %c32 step %c1 {
        %1 = memref.load %alloc_0[%arg3] : memref<32xf8E4M3>
        %2 = arith.addi %arg2, %arg3 : index
        memref.store %1, %arg1[%2] : memref<4096xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<128>}
    return
  }
  func.func private @dataflow_node_24(%arg0: memref<4096xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg1: memref<4096xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg2: memref<4096xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) {
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    %c4096 = arith.constant 4096 : index
    %c32 = arith.constant 32 : index
    scf.for %arg3 = %c0 to %c4096 step %c32 {
      %alloc = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg0[%0] : memref<4096xf8E4M3>
        memref.store %1, %alloc[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg1[%0] : memref<4096xf8E4M3>
        memref.store %1, %alloc_0[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_1 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg2[%0] : memref<4096xf8E4M3>
        memref.store %1, %alloc_1[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      func.call @shared_kernel_1(%alloc, %alloc_0, %alloc_1) : (memref<32xf8E4M3>, memref<32xf8E4M3>, memref<32xf8E4M3>) -> ()
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = memref.load %alloc_1[%arg4] : memref<32xf8E4M3>
        %1 = arith.addi %arg3, %arg4 : index
        memref.store %0, %arg2[%1] : memref<4096xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<128>}
    return
  }
  func.func private @dataflow_node_25(%arg0: memref<4096xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg1: memref<4096xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) {
    %c1 = arith.constant 1 : index
    %c32 = arith.constant 32 : index
    %c4096 = arith.constant 4096 : index
    %c0 = arith.constant 0 : index
    %0 = memref.get_global @__constant_xbf16_c9596378 : memref<f8E4M3>
    scf.for %arg2 = %c0 to %c4096 step %c32 {
      %alloc = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg3 = %c0 to %c32 step %c1 {
        %1 = arith.addi %arg2, %arg3 : index
        %2 = memref.load %arg0[%1] : memref<4096xf8E4M3>
        memref.store %2, %alloc[%arg3] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg3 = %c0 to %c32 step %c1 {
        %1 = arith.addi %arg2, %arg3 : index
        %2 = memref.load %arg1[%1] : memref<4096xf8E4M3>
        memref.store %2, %alloc_0[%arg3] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      func.call @shared_kernel_2(%alloc, %0, %alloc_0) : (memref<32xf8E4M3>, memref<f8E4M3>, memref<32xf8E4M3>) -> ()
      scf.for %arg3 = %c0 to %c32 step %c1 {
        %1 = memref.load %alloc_0[%arg3] : memref<32xf8E4M3>
        %2 = arith.addi %arg2, %arg3 : index
        memref.store %1, %arg1[%2] : memref<4096xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<128>}
    return
  }
  func.func private @dataflow_node_26(%arg0: memref<4096xf8E4M3>, %arg1: memref<4096xf8E4M3>) {
    %c0 = arith.constant 0 : index
    %c4096 = arith.constant 4096 : index
    %c1 = arith.constant 1 : index
    scf.for %arg2 = %c0 to %c4096 step %c1 {
      %0 = memref.load %arg0[%arg2] : memref<4096xf8E4M3>
      %1 = math.tanh %0 : f8E4M3
      memref.store %1, %arg1[%arg2] : memref<4096xf8E4M3>
    } {hls.latency = #hls.latency<4096>}
    return
  }
  func.func private @dataflow_node_27(%arg0: memref<4096xf8E4M3>, %arg1: memref<4096xf8E4M3>) {
    %cst = arith.constant 1.000000e+00 : f8E4M3
    %c0 = arith.constant 0 : index
    %c4096 = arith.constant 4096 : index
    %c1 = arith.constant 1 : index
    scf.for %arg2 = %c0 to %c4096 step %c1 {
      %0 = memref.load %arg0[%arg2] : memref<4096xf8E4M3>
      %1 = arith.addf %0, %cst : f8E4M3
      memref.store %1, %arg1[%arg2] : memref<4096xf8E4M3>
    } {hls.latency = #hls.latency<4096>}
    return
  }
  func.func private @dataflow_node_28(%arg0: memref<4096xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg1: memref<4096xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg2: memref<4096xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) {
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    %c4096 = arith.constant 4096 : index
    %c32 = arith.constant 32 : index
    scf.for %arg3 = %c0 to %c4096 step %c32 {
      %alloc = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg0[%0] : memref<4096xf8E4M3>
        memref.store %1, %alloc[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg1[%0] : memref<4096xf8E4M3>
        memref.store %1, %alloc_0[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      %alloc_1 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = arith.addi %arg3, %arg4 : index
        %1 = memref.load %arg2[%0] : memref<4096xf8E4M3>
        memref.store %1, %alloc_1[%arg4] : memref<32xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
      func.call @shared_kernel_0(%alloc, %alloc_0, %alloc_1) : (memref<32xf8E4M3>, memref<32xf8E4M3>, memref<32xf8E4M3>) -> ()
      scf.for %arg4 = %c0 to %c32 step %c1 {
        %0 = memref.load %alloc_1[%arg4] : memref<32xf8E4M3>
        %1 = arith.addi %arg3, %arg4 : index
        memref.store %0, %arg2[%1] : memref<4096xf8E4M3>
      } {hls.latency = #hls.latency<1>, hls.unroll = #hls.unroll<32>}
    } {hls.initiation_interval = #hls.initiation_interval<1>, hls.latency = #hls.latency<128>}
    return
  }
  func.func private @dataflow_node_29(%arg0: memref<4096xf8E4M3>, %arg1: memref<1x1x4096xf8E4M3>) {
    %c4096 = arith.constant 4096 : index
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    scf.for %arg2 = %c0 to %c4096 step %c1 {
      %0 = memref.load %arg0[%arg2] : memref<4096xf8E4M3>
      memref.store %0, %arg1[%c0, %c0, %arg2] : memref<1x1x4096xf8E4M3>
    } {hls.latency = #hls.latency<4096>}
    return
  }
  func.func @kernel_0_chip1(%arg0: memref<1x1x1024xf8E4M3> {chiplet.bytes = 2048 : i64, chiplet.comm_dir = "recv", chiplet.comm_kind = "axis", chiplet.sender_id = 0 : i64}, %arg1: memref<1x1x1024xf8E4M3> {chiplet.bytes = 2048 : i64, chiplet.comm_dir = "recv", chiplet.comm_kind = "axis", chiplet.sender_id = 0 : i64}, %arg2: memref<1x1x4096xf8E4M3> {llt.output}, %arg3: memref<1x1x1024xf8E4M3> {llt.output}, %arg4: memref<4096xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>, memref.values = @__constant_4096xbf16_32e35d8c}, %arg5: memref<1024x4096xf8E4M3> {memref.values = @__constant_1024x4096xbf16_c14f6a72}, %arg6: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>, memref.values = @__constant_1024xbf16_fc6692a9}, %arg7: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>, memref.values = @__constant_1024xbf16_4d0f539d}) attributes {hls.latency = #hls.latency<4234147>, hls.resource = #hls.resource<bram : 141>, hls.top} {
    %alloc = memref.alloc() {hls.resource = #hls.resource<bram : 0>} : memref<1x1xf32>
    call @dataflow_node_0(%alloc) : (memref<1x1xf32>) -> ()
    call @dataflow_node_1(%arg0, %alloc) : (memref<1x1x1024xf8E4M3>, memref<1x1xf32>) -> ()
    %alloc_0 = memref.alloc() {hls.resource = #hls.resource<bram : 0>} : memref<f8E4M3>
    call @dataflow_node_2(%alloc, %alloc_0) : (memref<1x1xf32>, memref<f8E4M3>) -> ()
    %alloc_1 = memref.alloc() {hls.resource = #hls.resource<bram : 0>} : memref<f8E4M3>
    call @dataflow_node_3(%alloc_0, %alloc_1) : (memref<f8E4M3>, memref<f8E4M3>) -> ()
    %alloc_2 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>, hls.resource = #hls.resource<bram : 0>} : memref<1024xf8E4M3>
    call @dataflow_node_4(%alloc_1, %alloc_2) : (memref<f8E4M3>, memref<1024xf8E4M3>) -> ()
    %alloc_3 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>, hls.resource = #hls.resource<bram : 0>} : memref<1024xf8E4M3>
    call @dataflow_node_5(%arg0, %alloc_2, %alloc_3) : (memref<1x1x1024xf8E4M3>, memref<1024xf8E4M3>, memref<1024xf8E4M3>) -> ()
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
    call @dataflow_node_15(%alloc_5, %arg7, %alloc_2) : (memref<1024xf8E4M3>, memref<1024xf8E4M3>, memref<1024xf8E4M3>) -> ()
    call @dataflow_node_16(%alloc_2, %arg6, %alloc_3) : (memref<1024xf8E4M3>, memref<1024xf8E4M3>, memref<1024xf8E4M3>) -> ()
    %alloc_6 = memref.alloc() {hls.resource = #hls.resource<bram : 8>} : memref<1x1x4096xf32>
    call @dataflow_node_17(%alloc_6) : (memref<1x1x4096xf32>) -> ()
    call @dataflow_node_18(%alloc_3, %arg5, %alloc_6) : (memref<1024xf8E4M3>, memref<1024x4096xf8E4M3>, memref<1x1x4096xf32>) -> ()
    %alloc_7 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>, hls.resource = #hls.resource<bram : 32>} : memref<4096xf8E4M3>
    call @dataflow_node_19(%alloc_6, %alloc_7) : (memref<1x1x4096xf32>, memref<4096xf8E4M3>) -> ()
    %alloc_8 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>, hls.resource = #hls.resource<bram : 32>} : memref<4096xf8E4M3>
    call @dataflow_node_20(%alloc_7, %arg4, %alloc_8) : (memref<4096xf8E4M3>, memref<4096xf8E4M3>, memref<4096xf8E4M3>) -> ()
    call @dataflow_node_21(%alloc_8, %alloc_7) : (memref<4096xf8E4M3>, memref<4096xf8E4M3>) -> ()
    %alloc_9 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>, hls.resource = #hls.resource<bram : 32>} : memref<4096xf8E4M3>
    call @dataflow_node_22(%alloc_8, %alloc_9) : (memref<4096xf8E4M3>, memref<4096xf8E4M3>) -> ()
    %alloc_10 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>, hls.resource = #hls.resource<bram : 32>} : memref<4096xf8E4M3>
    call @dataflow_node_23(%alloc_9, %alloc_10) : (memref<4096xf8E4M3>, memref<4096xf8E4M3>) -> ()
    call @dataflow_node_24(%alloc_8, %alloc_10, %alloc_9) : (memref<4096xf8E4M3>, memref<4096xf8E4M3>, memref<4096xf8E4M3>) -> ()
    call @dataflow_node_25(%alloc_9, %alloc_8) : (memref<4096xf8E4M3>, memref<4096xf8E4M3>) -> ()
    call @dataflow_node_26(%alloc_8, %alloc_9) : (memref<4096xf8E4M3>, memref<4096xf8E4M3>) -> ()
    call @dataflow_node_27(%alloc_9, %alloc_8) : (memref<4096xf8E4M3>, memref<4096xf8E4M3>) -> ()
    call @dataflow_node_28(%alloc_7, %alloc_8, %alloc_9) : (memref<4096xf8E4M3>, memref<4096xf8E4M3>, memref<4096xf8E4M3>) -> ()
    call @dataflow_node_29(%alloc_9, %arg2) : (memref<4096xf8E4M3>, memref<1x1x4096xf8E4M3>) -> ()
    return
  }
}

