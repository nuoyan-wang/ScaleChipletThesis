module {
  func.func @kernel_0_chip0(%arg0: tensor<1x1x1024xbf16>, %arg1: tensor<1xi32>, %arg2: tensor<1x16x128x64xbf16>, %arg3: tensor<1x16x128x64xbf16>, %arg4: tensor<1x1x1024x1024xi1>, %arg5: tensor<1x1x1x128xbf16>) -> (tensor<1x16x128x64xbf16>, tensor<1x16x128x64xbf16>, tensor<1x1x1024xbf16> {chiplet.bytes = 2048 : i64, chiplet.comm_dir = "send", chiplet.comm_kind = "axis", chiplet.receiver_id = 1 : i64}, tensor<1x1x1024xbf16> {chiplet.bytes = 2048 : i64, chiplet.comm_dir = "send", chiplet.comm_kind = "axis", chiplet.receiver_id = 2 : i64}) {
    %0 = "tosa.const"() {value = dense<9.765620e-04> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %1 = "tosa.const"() {value = dense<1.001360e-05> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %2 = "tosa.const"() {value = dense_resource<torch_tensor_1024_torch.bfloat16> : tensor<1024xbf16>} : () -> tensor<1024xbf16>
    %3 = "tosa.const"() {value = dense_resource<torch_tensor_1024_torch.bfloat16_1> : tensor<1024xbf16>} : () -> tensor<1024xbf16>
    %4 = "tosa.const"() {value = dense_resource<torch_tensor_1024_3072_torch.bfloat16> : tensor<1024x3072xbf16>} : () -> tensor<1024x3072xbf16>
    %5 = "tosa.const"() {value = dense_resource<torch_tensor_3072_torch.bfloat16> : tensor<3072xbf16>} : () -> tensor<3072xbf16>
    %6 = "tosa.const"() {value = dense<"0x00000000000000000100000000000000020000000000000003000000000000000400000000000000050000000000000006000000000000000700000000000000080000000000000009000000000000000A000000000000000B000000000000000C000000000000000D000000000000000E000000000000000F0000000000000010000000000000001100000000000000120000000000000013000000000000001400000000000000150000000000000016000000000000001700000000000000180000000000000019000000000000001A000000000000001B000000000000001C000000000000001D000000000000001E000000000000001F0000000000000020000000000000002100000000000000220000000000000023000000000000002400000000000000250000000000000026000000000000002700000000000000280000000000000029000000000000002A000000000000002B000000000000002C000000000000002D000000000000002E000000000000002F0000000000000030000000000000003100000000000000320000000000000033000000000000003400000000000000350000000000000036000000000000003700000000000000380000000000000039000000000000003A000000000000003B000000000000003C000000000000003D000000000000003E000000000000003F0000000000000040000000000000004100000000000000420000000000000043000000000000004400000000000000450000000000000046000000000000004700000000000000480000000000000049000000000000004A000000000000004B000000000000004C000000000000004D000000000000004E000000000000004F0000000000000050000000000000005100000000000000520000000000000053000000000000005400000000000000550000000000000056000000000000005700000000000000580000000000000059000000000000005A000000000000005B000000000000005C000000000000005D000000000000005E000000000000005F0000000000000060000000000000006100000000000000620000000000000063000000000000006400000000000000650000000000000066000000000000006700000000000000680000000000000069000000000000006A000000000000006B000000000000006C000000000000006D000000000000006E000000000000006F0000000000000070000000000000007100000000000000720000000000000073000000000000007400000000000000750000000000000076000000000000007700000000000000780000000000000079000000000000007A000000000000007B000000000000007C000000000000007D000000000000007E000000000000007F00000000000000"> : tensor<1x1x128x1xi64>} : () -> tensor<1x1x128x1xi64>
    %7 = "tosa.const"() {value = dense<[0, 1, 3, 2]> : tensor<4xi32>} : () -> tensor<4xi32>
    %8 = "tosa.const"() {value = dense<1.250000e-01> : tensor<1x1x1x1xbf16>} : () -> tensor<1x1x1x1xbf16>
    %9 = "tosa.const"() {value = dense<-3.389530e+38> : tensor<1x1x1x1xbf16>} : () -> tensor<1x1x1x1xbf16>
    %10 = "tosa.const"() {value = dense_resource<torch_tensor_1024_1024_torch.bfloat16> : tensor<1024x1024xbf16>} : () -> tensor<1024x1024xbf16>
    %11 = "tosa.const"() {value = dense_resource<torch_tensor_1024_torch.bfloat16_2> : tensor<1024xbf16>} : () -> tensor<1024xbf16>
    %12 = "tosa.reduce_sum"(%arg0) {axis = 2 : i64} : (tensor<1x1x1024xbf16>) -> tensor<1x1x1xbf16>
    %13 = "tosa.mul"(%12, %0) {shift = 0 : i32} : (tensor<1x1x1xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %14 = "tosa.tile"(%13) {multiples = [1, 1, 1024]} : (tensor<1x1x1xbf16>) -> tensor<1x1x1024xbf16>
    %15 = "tosa.sub"(%arg0, %14) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %16 = "tosa.mul"(%15, %15) {shift = 0 : i32} : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %17 = "tosa.reduce_sum"(%16) {axis = 2 : i64} : (tensor<1x1x1024xbf16>) -> tensor<1x1x1xbf16>
    %18 = "tosa.mul"(%17, %0) {shift = 0 : i32} : (tensor<1x1x1xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %19 = "tosa.add"(%18, %1) : (tensor<1x1x1xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %20 = "tosa.rsqrt"(%19) : (tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %21 = "tosa.tile"(%20) {multiples = [1, 1, 1024]} : (tensor<1x1x1xbf16>) -> tensor<1x1x1024xbf16>
    %22 = "tosa.mul"(%15, %21) {shift = 0 : i32} : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %23 = "tosa.reshape"(%2) {new_shape = [1, 1, 1024]} : (tensor<1024xbf16>) -> tensor<1x1x1024xbf16>
    %24 = "tosa.mul"(%22, %23) {shift = 0 : i32} : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %25 = "tosa.reshape"(%3) {new_shape = [1, 1, 1024]} : (tensor<1024xbf16>) -> tensor<1x1x1024xbf16>
    %26 = "tosa.add"(%24, %25) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %27 = "tosa.reshape"(%4) {new_shape = [1, 1024, 3072]} : (tensor<1024x3072xbf16>) -> tensor<1x1024x3072xbf16>
    %28 = "tosa.matmul"(%26, %27) : (tensor<1x1x1024xbf16>, tensor<1x1024x3072xbf16>) -> tensor<1x1x3072xf32>
    %29 = "tosa.reshape"(%28) {new_shape = [1, 3072]} : (tensor<1x1x3072xf32>) -> tensor<1x3072xf32>
    %30 = "tosa.cast"(%29) : (tensor<1x3072xf32>) -> tensor<1x3072xbf16>
    %31 = "tosa.reshape"(%5) {new_shape = [1, 3072]} : (tensor<3072xbf16>) -> tensor<1x3072xbf16>
    %32 = "tosa.add"(%30, %31) : (tensor<1x3072xbf16>, tensor<1x3072xbf16>) -> tensor<1x3072xbf16>
    %33 = "tosa.reshape"(%32) {new_shape = [1, 1, 3072]} : (tensor<1x3072xbf16>) -> tensor<1x1x3072xbf16>
    %34 = "tosa.slice"(%33) {size = [1, 1, 1024], start = [0, 0, 0]} : (tensor<1x1x3072xbf16>) -> tensor<1x1x1024xbf16>
    %35 = "tosa.slice"(%33) {size = [1, 1, 1024], start = [0, 0, 1024]} : (tensor<1x1x3072xbf16>) -> tensor<1x1x1024xbf16>
    %36 = "tosa.slice"(%33) {size = [1, 1, 1024], start = [0, 0, 2048]} : (tensor<1x1x3072xbf16>) -> tensor<1x1x1024xbf16>
    %37 = "tosa.reshape"(%35) {new_shape = [1, 16, 1, 64]} : (tensor<1x1x1024xbf16>) -> tensor<1x16x1x64xbf16>
    %38 = "tosa.reshape"(%36) {new_shape = [1, 16, 1, 64]} : (tensor<1x1x1024xbf16>) -> tensor<1x16x1x64xbf16>
    %39 = "tosa.reshape"(%arg1) {new_shape = [1, 1, 1, 1]} : (tensor<1xi32>) -> tensor<1x1x1x1xi32>
    %40 = "tosa.cast"(%39) : (tensor<1x1x1x1xi32>) -> tensor<1x1x1x1xi64>
    %41 = "tosa.equal"(%40, %6) : (tensor<1x1x1x1xi64>, tensor<1x1x128x1xi64>) -> tensor<1x1x128x1xi1>
    %42 = "tosa.tile"(%37) {multiples = [1, 1, 128, 1]} : (tensor<1x16x1x64xbf16>) -> tensor<1x16x128x64xbf16>
    %43 = "tosa.tile"(%38) {multiples = [1, 1, 128, 1]} : (tensor<1x16x1x64xbf16>) -> tensor<1x16x128x64xbf16>
    %44 = "tosa.select"(%41, %42, %arg2) : (tensor<1x1x128x1xi1>, tensor<1x16x128x64xbf16>, tensor<1x16x128x64xbf16>) -> tensor<1x16x128x64xbf16>
    %45 = "tosa.select"(%41, %43, %arg3) : (tensor<1x1x128x1xi1>, tensor<1x16x128x64xbf16>, tensor<1x16x128x64xbf16>) -> tensor<1x16x128x64xbf16>
    %46 = "tosa.transpose"(%44, %7) : (tensor<1x16x128x64xbf16>, tensor<4xi32>) -> tensor<1x16x64x128xbf16>
    %47 = "tosa.reshape"(%34) {new_shape = [16, 1, 64]} : (tensor<1x1x1024xbf16>) -> tensor<16x1x64xbf16>
    %48 = "tosa.reshape"(%46) {new_shape = [16, 64, 128]} : (tensor<1x16x64x128xbf16>) -> tensor<16x64x128xbf16>
    %49 = "tosa.matmul"(%47, %48) : (tensor<16x1x64xbf16>, tensor<16x64x128xbf16>) -> tensor<16x1x128xf32>
    %50 = "tosa.reshape"(%49) {new_shape = [1, 16, 1, 128]} : (tensor<16x1x128xf32>) -> tensor<1x16x1x128xf32>
    %51 = "tosa.cast"(%50) : (tensor<1x16x1x128xf32>) -> tensor<1x16x1x128xbf16>
    %52 = "tosa.mul"(%51, %8) {shift = 0 : i32} : (tensor<1x16x1x128xbf16>, tensor<1x1x1x1xbf16>) -> tensor<1x16x1x128xbf16>
    %53 = "tosa.slice"(%arg4) {size = [1, 1, 1, 1024], start = [0, 0, 127, 0]} : (tensor<1x1x1024x1024xi1>) -> tensor<1x1x1x1024xi1>
    %54 = "tosa.slice"(%53) {size = [1, 1, 1, 128], start = [0, 0, 0, 0]} : (tensor<1x1x1x1024xi1>) -> tensor<1x1x1x128xi1>
    %55 = "tosa.select"(%54, %52, %9) : (tensor<1x1x1x128xi1>, tensor<1x16x1x128xbf16>, tensor<1x1x1x1xbf16>) -> tensor<1x16x1x128xbf16>
    %56 = "tosa.tile"(%arg5) {multiples = [1, 16, 1, 1]} : (tensor<1x1x1x128xbf16>) -> tensor<1x16x1x128xbf16>
    %57 = "tosa.add"(%55, %56) : (tensor<1x16x1x128xbf16>, tensor<1x16x1x128xbf16>) -> tensor<1x16x1x128xbf16>
    %58 = "tosa.cast"(%57) : (tensor<1x16x1x128xbf16>) -> tensor<1x16x1x128xf32>
    %59 = "tosa.reduce_max"(%58) {axis = 3 : i64} : (tensor<1x16x1x128xf32>) -> tensor<1x16x1x1xf32>
    %60 = "tosa.sub"(%58, %59) : (tensor<1x16x1x128xf32>, tensor<1x16x1x1xf32>) -> tensor<1x16x1x128xf32>
    %61 = "tosa.exp"(%60) : (tensor<1x16x1x128xf32>) -> tensor<1x16x1x128xf32>
    %62 = "tosa.reduce_sum"(%61) {axis = 3 : i64} : (tensor<1x16x1x128xf32>) -> tensor<1x16x1x1xf32>
    %63 = "tosa.reciprocal"(%62) : (tensor<1x16x1x1xf32>) -> tensor<1x16x1x1xf32>
    %64 = "tosa.tile"(%63) {multiples = [1, 1, 1, 128]} : (tensor<1x16x1x1xf32>) -> tensor<1x16x1x128xf32>
    %65 = "tosa.mul"(%61, %64) {shift = 0 : i32} : (tensor<1x16x1x128xf32>, tensor<1x16x1x128xf32>) -> tensor<1x16x1x128xf32>
    %66 = "tosa.cast"(%65) : (tensor<1x16x1x128xf32>) -> tensor<1x16x1x128xbf16>
    %67 = "tosa.reshape"(%66) {new_shape = [16, 1, 128]} : (tensor<1x16x1x128xbf16>) -> tensor<16x1x128xbf16>
    %68 = "tosa.reshape"(%45) {new_shape = [16, 128, 64]} : (tensor<1x16x128x64xbf16>) -> tensor<16x128x64xbf16>
    %69 = "tosa.matmul"(%67, %68) : (tensor<16x1x128xbf16>, tensor<16x128x64xbf16>) -> tensor<16x1x64xf32>
    %70 = "tosa.reshape"(%69) {new_shape = [1, 16, 1, 64]} : (tensor<16x1x64xf32>) -> tensor<1x16x1x64xf32>
    %71 = "tosa.cast"(%70) : (tensor<1x16x1x64xf32>) -> tensor<1x16x1x64xbf16>
    %72 = "tosa.reshape"(%71) {new_shape = [1, 1, 1024]} : (tensor<1x16x1x64xbf16>) -> tensor<1x1x1024xbf16>
    %73 = "tosa.reshape"(%10) {new_shape = [1, 1024, 1024]} : (tensor<1024x1024xbf16>) -> tensor<1x1024x1024xbf16>
    %74 = "tosa.matmul"(%72, %73) : (tensor<1x1x1024xbf16>, tensor<1x1024x1024xbf16>) -> tensor<1x1x1024xf32>
    %75 = "tosa.reshape"(%74) {new_shape = [1, 1024]} : (tensor<1x1x1024xf32>) -> tensor<1x1024xf32>
    %76 = "tosa.cast"(%75) : (tensor<1x1024xf32>) -> tensor<1x1024xbf16>
    %77 = "tosa.reshape"(%11) {new_shape = [1, 1024]} : (tensor<1024xbf16>) -> tensor<1x1024xbf16>
    %78 = "tosa.add"(%76, %77) : (tensor<1x1024xbf16>, tensor<1x1024xbf16>) -> tensor<1x1024xbf16>
    %79 = "tosa.reshape"(%78) {new_shape = [1, 1, 1024]} : (tensor<1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %80 = "tosa.add"(%79, %arg0) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %81 = "tosa.const"() {value = dense<0.000000e+00> : tensor<1x16x128x64xbf16>} : () -> tensor<1x16x128x64xbf16>
    %82 = "tosa.add"(%44, %81) : (tensor<1x16x128x64xbf16>, tensor<1x16x128x64xbf16>) -> tensor<1x16x128x64xbf16>
    %83 = "tosa.add"(%45, %81) : (tensor<1x16x128x64xbf16>, tensor<1x16x128x64xbf16>) -> tensor<1x16x128x64xbf16>
    %84 = "tosa.const"() {value = dense<0.000000e+00> : tensor<1x1x1024xbf16>} : () -> tensor<1x1x1024xbf16>
    %85 = "tosa.add"(%80, %84) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %86 = "tosa.add"(%80, %84) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    return %82, %83, %85, %86 : tensor<1x16x128x64xbf16>, tensor<1x16x128x64xbf16>, tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>
  }
}
