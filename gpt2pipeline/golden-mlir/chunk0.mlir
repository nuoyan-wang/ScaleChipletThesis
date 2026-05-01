module {
  func.func @kernel_0_chip0(%arg0: tensor<1x1x1024xbf16>, %arg1: tensor<1xi32>, %arg2: tensor<1x16x128x64xbf16>, %arg3: tensor<1x16x128x64xbf16>, %arg4: tensor<1x1x1024x1024xi1>, %arg5: tensor<1x1x1x128xbf16>) -> (tensor<1x16x128x64xbf16>, tensor<1x16x128x64xbf16>, tensor<1x1x1024xbf16> {chiplet.bytes = 2048 : i64, chiplet.comm_dir = "send", chiplet.comm_kind = "axis", chiplet.receiver_id = 1 : i64}, tensor<1x1x1024xbf16> {chiplet.bytes = 2048 : i64, chiplet.comm_dir = "send", chiplet.comm_kind = "axis", chiplet.receiver_id = 2 : i64}) {
    %_tf_zero_i8 = "tosa.const"() {values = dense<0> : tensor<1xi8>} : () -> tensor<1xi8>
    %_tf_zero_bf16 = "tosa.const"() {values = dense<0.000000e+00> : tensor<1xbf16>} : () -> tensor<1xbf16>
    %0 = "tosa.const"() {values = dense<9.765620e-04> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %1 = "tosa.const"() {values = dense<1.001360e-05> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %2 = "tosa.const"() {values = dense_resource<torch_tensor_1024_torch.bfloat16> : tensor<1024xbf16>} : () -> tensor<1024xbf16>
    %3 = "tosa.const"() {values = dense_resource<torch_tensor_1024_torch.bfloat16_1> : tensor<1024xbf16>} : () -> tensor<1024xbf16>
    %4 = "tosa.const"() {values = dense_resource<torch_tensor_1024_3072_torch.bfloat16> : tensor<1024x3072xbf16>} : () -> tensor<1024x3072xbf16>
    %5 = "tosa.const"() {values = dense_resource<torch_tensor_3072_torch.bfloat16> : tensor<3072xbf16>} : () -> tensor<3072xbf16>
    %6 = "tosa.const"() {values = dense<"0x00000000000000000100000000000000020000000000000003000000000000000400000000000000050000000000000006000000000000000700000000000000080000000000000009000000000000000A000000000000000B000000000000000C000000000000000D000000000000000E000000000000000F0000000000000010000000000000001100000000000000120000000000000013000000000000001400000000000000150000000000000016000000000000001700000000000000180000000000000019000000000000001A000000000000001B000000000000001C000000000000001D000000000000001E000000000000001F0000000000000020000000000000002100000000000000220000000000000023000000000000002400000000000000250000000000000026000000000000002700000000000000280000000000000029000000000000002A000000000000002B000000000000002C000000000000002D000000000000002E000000000000002F0000000000000030000000000000003100000000000000320000000000000033000000000000003400000000000000350000000000000036000000000000003700000000000000380000000000000039000000000000003A000000000000003B000000000000003C000000000000003D000000000000003E000000000000003F0000000000000040000000000000004100000000000000420000000000000043000000000000004400000000000000450000000000000046000000000000004700000000000000480000000000000049000000000000004A000000000000004B000000000000004C000000000000004D000000000000004E000000000000004F0000000000000050000000000000005100000000000000520000000000000053000000000000005400000000000000550000000000000056000000000000005700000000000000580000000000000059000000000000005A000000000000005B000000000000005C000000000000005D000000000000005E000000000000005F0000000000000060000000000000006100000000000000620000000000000063000000000000006400000000000000650000000000000066000000000000006700000000000000680000000000000069000000000000006A000000000000006B000000000000006C000000000000006D000000000000006E000000000000006F0000000000000070000000000000007100000000000000720000000000000073000000000000007400000000000000750000000000000076000000000000007700000000000000780000000000000079000000000000007A000000000000007B000000000000007C000000000000007D000000000000007E000000000000007F00000000000000"> : tensor<1x1x128x1xi64>} : () -> tensor<1x1x128x1xi64>
    %7 = "tosa.const"() {values = dense<[0, 1, 3, 2]> : tensor<4xi32>} : () -> tensor<4xi32>
    %8 = "tosa.const"() {values = dense<1.250000e-01> : tensor<1x1x1x1xbf16>} : () -> tensor<1x1x1x1xbf16>
    %9 = "tosa.const"() {values = dense<-3.389530e+38> : tensor<1x1x1x1xbf16>} : () -> tensor<1x1x1x1xbf16>
    %10 = "tosa.const"() {values = dense_resource<torch_tensor_1024_1024_torch.bfloat16> : tensor<1024x1024xbf16>} : () -> tensor<1024x1024xbf16>
    %11 = "tosa.const"() {values = dense_resource<torch_tensor_1024_torch.bfloat16_2> : tensor<1024xbf16>} : () -> tensor<1024xbf16>
    %12 = "tosa.reduce_sum"(%arg0) {axis = 2 : i32} : (tensor<1x1x1024xbf16>) -> tensor<1x1x1xbf16>
    %13 = "tosa.mul" (%12, %0, %_tf_zero_i8) : (tensor<1x1x1xbf16>, tensor<1x1x1xbf16>, tensor<1xi8>) -> tensor<1x1x1xbf16>
    %_tf_tile_shape_0 = tosa.const_shape  {values = dense<[1, 1, 1024]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %14 = "tosa.tile" (%13, %_tf_tile_shape_0) : (tensor<1x1x1xbf16>, !tosa.shape<3>) -> tensor<1x1x1024xbf16>
    %15 = "tosa.sub"(%arg0, %14) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %16 = "tosa.mul" (%15, %15, %_tf_zero_i8) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>, tensor<1xi8>) -> tensor<1x1x1024xbf16>
    %17 = "tosa.reduce_sum"(%16) {axis = 2 : i32} : (tensor<1x1x1024xbf16>) -> tensor<1x1x1xbf16>
    %18 = "tosa.mul" (%17, %0, %_tf_zero_i8) : (tensor<1x1x1xbf16>, tensor<1x1x1xbf16>, tensor<1xi8>) -> tensor<1x1x1xbf16>
    %19 = "tosa.add"(%18, %1) : (tensor<1x1x1xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %20 = "tosa.rsqrt"(%19) : (tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %_tf_tile_shape_1 = tosa.const_shape  {values = dense<[1, 1, 1024]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %21 = "tosa.tile" (%20, %_tf_tile_shape_1) : (tensor<1x1x1xbf16>, !tosa.shape<3>) -> tensor<1x1x1024xbf16>
    %22 = "tosa.mul" (%15, %21, %_tf_zero_i8) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>, tensor<1xi8>) -> tensor<1x1x1024xbf16>
    %_tf_reshape_shape_0 = tosa.const_shape  {values = dense<[1, 1, 1024]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %23 = "tosa.reshape" (%2, %_tf_reshape_shape_0) : (tensor<1024xbf16>, !tosa.shape<3>) -> tensor<1x1x1024xbf16>
    %24 = "tosa.mul" (%22, %23, %_tf_zero_i8) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>, tensor<1xi8>) -> tensor<1x1x1024xbf16>
    %_tf_reshape_shape_1 = tosa.const_shape  {values = dense<[1, 1, 1024]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %25 = "tosa.reshape" (%3, %_tf_reshape_shape_1) : (tensor<1024xbf16>, !tosa.shape<3>) -> tensor<1x1x1024xbf16>
    %26 = "tosa.add"(%24, %25) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %_tf_reshape_shape_2 = tosa.const_shape  {values = dense<[1, 1024, 3072]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %27 = "tosa.reshape" (%4, %_tf_reshape_shape_2) : (tensor<1024x3072xbf16>, !tosa.shape<3>) -> tensor<1x1024x3072xbf16>
    %28 = "tosa.matmul" (%26, %27, %_tf_zero_bf16, %_tf_zero_bf16) : (tensor<1x1x1024xbf16>, tensor<1x1024x3072xbf16>, tensor<1xbf16>, tensor<1xbf16>) -> tensor<1x1x3072xf32>
    %_tf_reshape_shape_3 = tosa.const_shape  {values = dense<[1, 3072]> : tensor<2xindex>} : () -> !tosa.shape<2>
    %29 = "tosa.reshape" (%28, %_tf_reshape_shape_3) : (tensor<1x1x3072xf32>, !tosa.shape<2>) -> tensor<1x3072xf32>
    %30 = "tosa.cast"(%29) : (tensor<1x3072xf32>) -> tensor<1x3072xbf16>
    %_tf_reshape_shape_4 = tosa.const_shape  {values = dense<[1, 3072]> : tensor<2xindex>} : () -> !tosa.shape<2>
    %31 = "tosa.reshape" (%5, %_tf_reshape_shape_4) : (tensor<3072xbf16>, !tosa.shape<2>) -> tensor<1x3072xbf16>
    %32 = "tosa.add"(%30, %31) : (tensor<1x3072xbf16>, tensor<1x3072xbf16>) -> tensor<1x3072xbf16>
    %_tf_reshape_shape_5 = tosa.const_shape  {values = dense<[1, 1, 3072]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %33 = "tosa.reshape" (%32, %_tf_reshape_shape_5) : (tensor<1x3072xbf16>, !tosa.shape<3>) -> tensor<1x1x3072xbf16>
    %_tf_slice_start_0 = tosa.const_shape  {values = dense<[0, 0, 0]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %_tf_slice_size_0 = tosa.const_shape  {values = dense<[1, 1, 1024]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %34 = "tosa.slice" (%33, %_tf_slice_start_0, %_tf_slice_size_0) : (tensor<1x1x3072xbf16>, !tosa.shape<3>, !tosa.shape<3>) -> tensor<1x1x1024xbf16>
    %_tf_slice_start_1 = tosa.const_shape  {values = dense<[0, 0, 1024]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %_tf_slice_size_1 = tosa.const_shape  {values = dense<[1, 1, 1024]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %35 = "tosa.slice" (%33, %_tf_slice_start_1, %_tf_slice_size_1) : (tensor<1x1x3072xbf16>, !tosa.shape<3>, !tosa.shape<3>) -> tensor<1x1x1024xbf16>
    %_tf_slice_start_2 = tosa.const_shape  {values = dense<[0, 0, 2048]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %_tf_slice_size_2 = tosa.const_shape  {values = dense<[1, 1, 1024]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %36 = "tosa.slice" (%33, %_tf_slice_start_2, %_tf_slice_size_2) : (tensor<1x1x3072xbf16>, !tosa.shape<3>, !tosa.shape<3>) -> tensor<1x1x1024xbf16>
    %_tf_reshape_shape_6 = tosa.const_shape  {values = dense<[1, 16, 1, 64]> : tensor<4xindex>} : () -> !tosa.shape<4>
    %37 = "tosa.reshape" (%35, %_tf_reshape_shape_6) : (tensor<1x1x1024xbf16>, !tosa.shape<4>) -> tensor<1x16x1x64xbf16>
    %_tf_reshape_shape_7 = tosa.const_shape  {values = dense<[1, 16, 1, 64]> : tensor<4xindex>} : () -> !tosa.shape<4>
    %38 = "tosa.reshape" (%36, %_tf_reshape_shape_7) : (tensor<1x1x1024xbf16>, !tosa.shape<4>) -> tensor<1x16x1x64xbf16>
    %_tf_reshape_shape_8 = tosa.const_shape  {values = dense<[1, 1, 1, 1]> : tensor<4xindex>} : () -> !tosa.shape<4>
    %39 = "tosa.reshape" (%arg1, %_tf_reshape_shape_8) : (tensor<1xi32>, !tosa.shape<4>) -> tensor<1x1x1x1xi32>
    %40 = "tosa.cast"(%39) : (tensor<1x1x1x1xi32>) -> tensor<1x1x1x1xi64>
    %41 = "tosa.equal"(%40, %6) : (tensor<1x1x1x1xi64>, tensor<1x1x128x1xi64>) -> tensor<1x1x128x1xi1>
    %_tf_tile_shape_2 = tosa.const_shape  {values = dense<[1, 1, 128, 1]> : tensor<4xindex>} : () -> !tosa.shape<4>
    %42 = "tosa.tile" (%37, %_tf_tile_shape_2) : (tensor<1x16x1x64xbf16>, !tosa.shape<4>) -> tensor<1x16x128x64xbf16>
    %_tf_tile_shape_3 = tosa.const_shape  {values = dense<[1, 1, 128, 1]> : tensor<4xindex>} : () -> !tosa.shape<4>
    %43 = "tosa.tile" (%38, %_tf_tile_shape_3) : (tensor<1x16x1x64xbf16>, !tosa.shape<4>) -> tensor<1x16x128x64xbf16>
    %44 = "tosa.select"(%41, %42, %arg2) : (tensor<1x1x128x1xi1>, tensor<1x16x128x64xbf16>, tensor<1x16x128x64xbf16>) -> tensor<1x16x128x64xbf16>
    %45 = "tosa.select"(%41, %43, %arg3) : (tensor<1x1x128x1xi1>, tensor<1x16x128x64xbf16>, tensor<1x16x128x64xbf16>) -> tensor<1x16x128x64xbf16>
    %46 = tosa.transpose %44 {perms = array<i32: 0, 1, 3, 2>} : (tensor<1x16x128x64xbf16>) -> tensor<1x16x64x128xbf16>
    %_tf_reshape_shape_9 = tosa.const_shape  {values = dense<[16, 1, 64]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %47 = "tosa.reshape" (%34, %_tf_reshape_shape_9) : (tensor<1x1x1024xbf16>, !tosa.shape<3>) -> tensor<16x1x64xbf16>
    %_tf_reshape_shape_10 = tosa.const_shape  {values = dense<[16, 64, 128]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %48 = "tosa.reshape" (%46, %_tf_reshape_shape_10) : (tensor<1x16x64x128xbf16>, !tosa.shape<3>) -> tensor<16x64x128xbf16>
    %49 = "tosa.matmul" (%47, %48, %_tf_zero_bf16, %_tf_zero_bf16) : (tensor<16x1x64xbf16>, tensor<16x64x128xbf16>, tensor<1xbf16>, tensor<1xbf16>) -> tensor<16x1x128xf32>
    %_tf_reshape_shape_11 = tosa.const_shape  {values = dense<[1, 16, 1, 128]> : tensor<4xindex>} : () -> !tosa.shape<4>
    %50 = "tosa.reshape" (%49, %_tf_reshape_shape_11) : (tensor<16x1x128xf32>, !tosa.shape<4>) -> tensor<1x16x1x128xf32>
    %51 = "tosa.cast"(%50) : (tensor<1x16x1x128xf32>) -> tensor<1x16x1x128xbf16>
    %52 = "tosa.mul" (%51, %8, %_tf_zero_i8) : (tensor<1x16x1x128xbf16>, tensor<1x1x1x1xbf16>, tensor<1xi8>) -> tensor<1x16x1x128xbf16>
    %_tf_slice_start_3 = tosa.const_shape  {values = dense<[0, 0, 127, 0]> : tensor<4xindex>} : () -> !tosa.shape<4>
    %_tf_slice_size_3 = tosa.const_shape  {values = dense<[1, 1, 1, 1024]> : tensor<4xindex>} : () -> !tosa.shape<4>
    %53 = "tosa.slice" (%arg4, %_tf_slice_start_3, %_tf_slice_size_3) : (tensor<1x1x1024x1024xi1>, !tosa.shape<4>, !tosa.shape<4>) -> tensor<1x1x1x1024xi1>
    %_tf_slice_start_4 = tosa.const_shape  {values = dense<[0, 0, 0, 0]> : tensor<4xindex>} : () -> !tosa.shape<4>
    %_tf_slice_size_4 = tosa.const_shape  {values = dense<[1, 1, 1, 128]> : tensor<4xindex>} : () -> !tosa.shape<4>
    %54 = "tosa.slice" (%53, %_tf_slice_start_4, %_tf_slice_size_4) : (tensor<1x1x1x1024xi1>, !tosa.shape<4>, !tosa.shape<4>) -> tensor<1x1x1x128xi1>
    %55 = "tosa.select"(%54, %52, %9) : (tensor<1x1x1x128xi1>, tensor<1x16x1x128xbf16>, tensor<1x1x1x1xbf16>) -> tensor<1x16x1x128xbf16>
    %_tf_tile_shape_4 = tosa.const_shape  {values = dense<[1, 16, 1, 1]> : tensor<4xindex>} : () -> !tosa.shape<4>
    %56 = "tosa.tile" (%arg5, %_tf_tile_shape_4) : (tensor<1x1x1x128xbf16>, !tosa.shape<4>) -> tensor<1x16x1x128xbf16>
    %57 = "tosa.add"(%55, %56) : (tensor<1x16x1x128xbf16>, tensor<1x16x1x128xbf16>) -> tensor<1x16x1x128xbf16>
    %58 = "tosa.cast"(%57) : (tensor<1x16x1x128xbf16>) -> tensor<1x16x1x128xf32>
    %59 = "tosa.reduce_max"(%58) {axis = 3 : i32} : (tensor<1x16x1x128xf32>) -> tensor<1x16x1x1xf32>
    %60 = "tosa.sub"(%58, %59) : (tensor<1x16x1x128xf32>, tensor<1x16x1x1xf32>) -> tensor<1x16x1x128xf32>
    %61 = "tosa.exp"(%60) : (tensor<1x16x1x128xf32>) -> tensor<1x16x1x128xf32>
    %62 = "tosa.reduce_sum"(%61) {axis = 3 : i32} : (tensor<1x16x1x128xf32>) -> tensor<1x16x1x1xf32>
    %63 = "tosa.reciprocal"(%62) : (tensor<1x16x1x1xf32>) -> tensor<1x16x1x1xf32>
    %_tf_tile_shape_5 = tosa.const_shape  {values = dense<[1, 1, 1, 128]> : tensor<4xindex>} : () -> !tosa.shape<4>
    %64 = "tosa.tile" (%63, %_tf_tile_shape_5) : (tensor<1x16x1x1xf32>, !tosa.shape<4>) -> tensor<1x16x1x128xf32>
    %65 = "tosa.mul" (%61, %64, %_tf_zero_i8) : (tensor<1x16x1x128xf32>, tensor<1x16x1x128xf32>, tensor<1xi8>) -> tensor<1x16x1x128xf32>
    %66 = "tosa.cast"(%65) : (tensor<1x16x1x128xf32>) -> tensor<1x16x1x128xbf16>
    %_tf_reshape_shape_12 = tosa.const_shape  {values = dense<[16, 1, 128]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %67 = "tosa.reshape" (%66, %_tf_reshape_shape_12) : (tensor<1x16x1x128xbf16>, !tosa.shape<3>) -> tensor<16x1x128xbf16>
    %_tf_reshape_shape_13 = tosa.const_shape  {values = dense<[16, 128, 64]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %68 = "tosa.reshape" (%45, %_tf_reshape_shape_13) : (tensor<1x16x128x64xbf16>, !tosa.shape<3>) -> tensor<16x128x64xbf16>
    %69 = "tosa.matmul" (%67, %68, %_tf_zero_bf16, %_tf_zero_bf16) : (tensor<16x1x128xbf16>, tensor<16x128x64xbf16>, tensor<1xbf16>, tensor<1xbf16>) -> tensor<16x1x64xf32>
    %_tf_reshape_shape_14 = tosa.const_shape  {values = dense<[1, 16, 1, 64]> : tensor<4xindex>} : () -> !tosa.shape<4>
    %70 = "tosa.reshape" (%69, %_tf_reshape_shape_14) : (tensor<16x1x64xf32>, !tosa.shape<4>) -> tensor<1x16x1x64xf32>
    %71 = "tosa.cast"(%70) : (tensor<1x16x1x64xf32>) -> tensor<1x16x1x64xbf16>
    %_tf_reshape_shape_15 = tosa.const_shape  {values = dense<[1, 1, 1024]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %72 = "tosa.reshape" (%71, %_tf_reshape_shape_15) : (tensor<1x16x1x64xbf16>, !tosa.shape<3>) -> tensor<1x1x1024xbf16>
    %_tf_reshape_shape_16 = tosa.const_shape  {values = dense<[1, 1024, 1024]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %73 = "tosa.reshape" (%10, %_tf_reshape_shape_16) : (tensor<1024x1024xbf16>, !tosa.shape<3>) -> tensor<1x1024x1024xbf16>
    %74 = "tosa.matmul" (%72, %73, %_tf_zero_bf16, %_tf_zero_bf16) : (tensor<1x1x1024xbf16>, tensor<1x1024x1024xbf16>, tensor<1xbf16>, tensor<1xbf16>) -> tensor<1x1x1024xf32>
    %_tf_reshape_shape_17 = tosa.const_shape  {values = dense<[1, 1024]> : tensor<2xindex>} : () -> !tosa.shape<2>
    %75 = "tosa.reshape" (%74, %_tf_reshape_shape_17) : (tensor<1x1x1024xf32>, !tosa.shape<2>) -> tensor<1x1024xf32>
    %76 = "tosa.cast"(%75) : (tensor<1x1024xf32>) -> tensor<1x1024xbf16>
    %_tf_reshape_shape_18 = tosa.const_shape  {values = dense<[1, 1024]> : tensor<2xindex>} : () -> !tosa.shape<2>
    %77 = "tosa.reshape" (%11, %_tf_reshape_shape_18) : (tensor<1024xbf16>, !tosa.shape<2>) -> tensor<1x1024xbf16>
    %78 = "tosa.add"(%76, %77) : (tensor<1x1024xbf16>, tensor<1x1024xbf16>) -> tensor<1x1024xbf16>
    %_tf_reshape_shape_19 = tosa.const_shape  {values = dense<[1, 1, 1024]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %79 = "tosa.reshape" (%78, %_tf_reshape_shape_19) : (tensor<1x1024xbf16>, !tosa.shape<3>) -> tensor<1x1x1024xbf16>
    %80 = "tosa.add"(%79, %arg0) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %81 = "tosa.const"() {values = dense<0.000000e+00> : tensor<1x16x128x64xbf16>} : () -> tensor<1x16x128x64xbf16>
    %82 = "tosa.add"(%44, %81) : (tensor<1x16x128x64xbf16>, tensor<1x16x128x64xbf16>) -> tensor<1x16x128x64xbf16>
    %83 = "tosa.add"(%45, %81) : (tensor<1x16x128x64xbf16>, tensor<1x16x128x64xbf16>) -> tensor<1x16x128x64xbf16>
    %84 = "tosa.const"() {values = dense<0.000000e+00> : tensor<1x1x1024xbf16>} : () -> tensor<1x1x1024xbf16>
    %85 = "tosa.add"(%80, %84) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %86 = "tosa.add"(%80, %84) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    return %82, %83, %85, %86 : tensor<1x16x128x64xbf16>, tensor<1x16x128x64xbf16>, tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>
  }
}
