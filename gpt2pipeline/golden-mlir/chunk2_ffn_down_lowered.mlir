module {
  func.func @shared_kernel_0(%arg0: memref<32xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg1: memref<32xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg2: memref<32xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) attributes {hls.initiation_interval = #hls.initiation_interval<1>, hls.kernel, hls.latency = #hls.latency<0>, hls.resource = #hls.resource<bram : 0>} {
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
  memref.global "private" constant @__constant_4096x1024xbf16_8a298025 : memref<4096x1024xf8E4M3> = dense_resource<torch_tensor_4096_1024_torch.bfloat16>
  memref.global "private" constant @__constant_1024xbf16_aee6a6d5 : memref<1024xf8E4M3> = dense_resource<torch_tensor_1024_torch.bfloat16_5>
  func.func private @dataflow_node_0(%arg0: memref<1x1x1024xf32>) {
    %c1024 = arith.constant 1024 : index
    %cst = arith.constant 0.000000e+00 : f32
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    scf.for %arg1 = %c0 to %c1024 step %c1 {
      memref.store %cst, %arg0[%c0, %c0, %arg1] : memref<1x1x1024xf32>
    } {hls.latency = #hls.latency<1024>}
    return
  }
  func.func private @dataflow_node_1(%arg0: memref<1x1x4096xf8E4M3>, %arg1: memref<4096x1024xf8E4M3>, %arg2: memref<1x1x1024xf32>) {
    %c4 = arith.constant 4 : index
    %c4096 = arith.constant 4096 : index
    %c2 = arith.constant 2 : index
    %c1024 = arith.constant 1024 : index
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    %cst = arith.constant 0.000000e+00 : f32
    %alloc = memref.alloc() : memref<2xf32>
    scf.for %arg3 = %c0 to %c1024 step %c1 {
      scf.for %arg4 = %c0 to %c2 step %c1 {
        memref.store %cst, %alloc[%arg4] : memref<2xf32>
      } {hls.latency = #hls.latency<2>}
      scf.for %arg4 = %c0 to %c4096 step %c1 {
        %5 = memref.load %arg0[%c0, %c0, %arg4] : memref<1x1x4096xf8E4M3>
        %6 = memref.load %arg1[%arg4, %arg3] : memref<4096x1024xf8E4M3>
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
      } {hls.latency = #hls.latency<4096>}
      %0 = memref.load %arg2[%c0, %c0, %arg3] : memref<1x1x1024xf32>
      %1 = memref.load %alloc[%c0] : memref<2xf32>
      %2 = memref.load %alloc[%c1] : memref<2xf32>
      %3 = arith.addf %1, %2 : f32
      %4 = arith.addf %3, %0 : f32
      memref.store %4, %arg2[%c0, %c0, %arg3] : memref<1x1x1024xf32>
    } {hls.latency = #hls.latency<4196352>}
    return
  }
  func.func private @dataflow_node_2(%arg0: memref<1x1x1024xf32>, %arg1: memref<1024xf8E4M3>) {
    %c-1 = arith.constant -1 : index
    %c1 = arith.constant 1 : index
    %c1024 = arith.constant 1024 : index
    %c0 = arith.constant 0 : index
    scf.for %arg2 = %c0 to %c1024 step %c1 {
      %0 = arith.divsi %arg2, %c1024 : index
      %1 = arith.muli %0, %c1024 : index
      %2 = arith.cmpi ne, %arg2, %1 : index
      %3 = arith.cmpi slt, %arg2, %c0 : index
      %4 = arith.andi %2, %3 : i1
      %5 = arith.addi %0, %c-1 : index
      %6 = arith.select %4, %5, %0 : index
      %7 = arith.remsi %arg2, %c1024 : index
      %8 = arith.cmpi slt, %7, %c0 : index
      %9 = arith.addi %7, %c1024 overflow<nsw> : index
      %10 = arith.select %8, %9, %7 : index
      %11 = arith.divsi %10, %c1024 : index
      %12 = arith.remsi %arg2, %c1024 : index
      %13 = arith.cmpi slt, %12, %c0 : index
      %14 = arith.addi %12, %c1024 overflow<nsw> : index
      %15 = arith.select %13, %14, %12 : index
      %16 = memref.load %arg0[%6, %11, %15] : memref<1x1x1024xf32>
      %17 = arith.truncf %16 : f32 to f8E4M3
      memref.store %17, %arg1[%arg2] : memref<1024xf8E4M3>
    } {hls.latency = #hls.latency<1024>}
    return
  }
  func.func private @dataflow_node_3(%arg0: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg1: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg2: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) {
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
  func.func private @dataflow_node_4(%arg0: memref<1x1x1024xf8E4M3>, %arg1: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}, %arg2: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>}) {
    %c-1 = arith.constant -1 : index
    %c1 = arith.constant 1 : index
    %c32 = arith.constant 32 : index
    %c1024 = arith.constant 1024 : index
    %c0 = arith.constant 0 : index
    scf.for %arg3 = %c0 to %c1024 step %c32 {
      %alloc = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>} : memref<32xf8E4M3>
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
        %17 = memref.load %arg0[%7, %12, %16] : memref<1x1x1024xf8E4M3>
        memref.store %17, %alloc[%arg4] : memref<32xf8E4M3>
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
  func.func private @dataflow_node_5(%arg0: memref<1024xf8E4M3>, %arg1: memref<1x1x1024xf8E4M3>) {
    %c1024 = arith.constant 1024 : index
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    scf.for %arg2 = %c0 to %c1024 step %c1 {
      %0 = memref.load %arg0[%arg2] : memref<1024xf8E4M3>
      memref.store %0, %arg1[%c0, %c0, %arg2] : memref<1x1x1024xf8E4M3>
    } {hls.latency = #hls.latency<1024>}
    return
  }
  func.func @kernel_0_chip2(%arg0: memref<1x1x4096xf8E4M3> {chiplet.bytes = 8192 : i64, chiplet.comm_dir = "recv", chiplet.comm_kind = "axis", chiplet.sender_id = 1 : i64}, %arg1: memref<1x1x1024xf8E4M3> {chiplet.bytes = 2048 : i64, chiplet.comm_dir = "recv", chiplet.comm_kind = "axis", chiplet.sender_id = 1 : i64}, %arg2: memref<1x1x1024xf8E4M3> {llt.output}, %arg3: memref<1024xf8E4M3> {hls.partition = #hls.partition<[cyclic], [32]>, memref.values = @__constant_1024xbf16_aee6a6d5}, %arg4: memref<4096x1024xf8E4M3> {memref.values = @__constant_4096x1024xbf16_8a298025}) attributes {hls.latency = #hls.latency<4199494>, hls.resource = #hls.resource<bram : 2>, hls.top} {
    %alloc = memref.alloc() {hls.resource = #hls.resource<bram : 2>} : memref<1x1x1024xf32>
    call @dataflow_node_0(%alloc) : (memref<1x1x1024xf32>) -> ()
    call @dataflow_node_1(%arg0, %arg4, %alloc) : (memref<1x1x4096xf8E4M3>, memref<4096x1024xf8E4M3>, memref<1x1x1024xf32>) -> ()
    %alloc_0 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>, hls.resource = #hls.resource<bram : 0>} : memref<1024xf8E4M3>
    call @dataflow_node_2(%alloc, %alloc_0) : (memref<1x1x1024xf32>, memref<1024xf8E4M3>) -> ()
    %alloc_1 = memref.alloc() {hls.partition = #hls.partition<[cyclic], [32]>, hls.resource = #hls.resource<bram : 0>} : memref<1024xf8E4M3>
    call @dataflow_node_3(%alloc_0, %arg3, %alloc_1) : (memref<1024xf8E4M3>, memref<1024xf8E4M3>, memref<1024xf8E4M3>) -> ()
    call @dataflow_node_4(%arg1, %alloc_1, %alloc_0) : (memref<1x1x1024xf8E4M3>, memref<1024xf8E4M3>, memref<1024xf8E4M3>) -> ()
    call @dataflow_node_5(%alloc_0, %arg2) : (memref<1024xf8E4M3>, memref<1x1x1024xf8E4M3>) -> ()
    return
  }
}

