module {
  func.func @kernel_0(%arg0: tensor<1x1x1024x1024xi1>, %arg1: tensor<bf16>, %arg2: tensor<1x1x1024x1024xi1>, %arg3: tensor<bf16>, %arg4: tensor<1x1x1024x1024xi1>, %arg5: tensor<bf16>, %arg6: tensor<1x1x1024x1024xi1>, %arg7: tensor<bf16>, %arg8: tensor<1x1x1024x1024xi1>, %arg9: tensor<bf16>, %arg10: tensor<1x1x1024x1024xi1>, %arg11: tensor<bf16>, %arg12: tensor<1x1x1024x1024xi1>, %arg13: tensor<bf16>, %arg14: tensor<1x1x1024x1024xi1>, %arg15: tensor<bf16>, %arg16: tensor<1x1x1024x1024xi1>, %arg17: tensor<bf16>, %arg18: tensor<1x1x1024x1024xi1>, %arg19: tensor<bf16>, %arg20: tensor<1x1x1024x1024xi1>, %arg21: tensor<bf16>, %arg22: tensor<1x1x1024x1024xi1>, %arg23: tensor<bf16>, %arg24: tensor<1x1x1024x1024xi1>, %arg25: tensor<bf16>, %arg26: tensor<1x1x1024x1024xi1>, %arg27: tensor<bf16>, %arg28: tensor<1x1x1024x1024xi1>, %arg29: tensor<bf16>, %arg30: tensor<1x1x1024x1024xi1>, %arg31: tensor<bf16>, %arg32: tensor<1x1x1024x1024xi1>, %arg33: tensor<bf16>, %arg34: tensor<1x1x1024x1024xi1>, %arg35: tensor<bf16>, %arg36: tensor<1x1x1024x1024xi1>, %arg37: tensor<bf16>, %arg38: tensor<1x1x1024x1024xi1>, %arg39: tensor<bf16>, %arg40: tensor<1x1x1024x1024xi1>, %arg41: tensor<bf16>, %arg42: tensor<1x1x1024x1024xi1>, %arg43: tensor<bf16>, %arg44: tensor<1x1x1024x1024xi1>, %arg45: tensor<bf16>, %arg46: tensor<1x1x1024x1024xi1>, %arg47: tensor<bf16>, %arg48: tensor<1x1x1024xbf16>, %arg49: tensor<1xi32>, %arg50: tensor<1x1x1x128xbf16>, %arg51: tensor<1x16x128x64xbf16>, %arg52: tensor<1x16x128x64xbf16>) -> (tensor<1x1x1024xbf16>, tensor<1x16x128x64xbf16>, tensor<1x16x128x64xbf16>) {
    %0 = "tosa.const"() {value = dense_resource<torch_tensor_1024_torch.bfloat16_5> : tensor<1024xbf16>} : () -> tensor<1024xbf16>
    %1 = "tosa.const"() {value = dense_resource<torch_tensor_4096_1024_torch.bfloat16> : tensor<4096x1024xbf16>} : () -> tensor<4096x1024xbf16>
    %2 = "tosa.const"() {value = dense_resource<torch_tensor_4096_torch.bfloat16> : tensor<4096xbf16>} : () -> tensor<4096xbf16>
    %3 = "tosa.const"() {value = dense_resource<torch_tensor_1024_4096_torch.bfloat16> : tensor<1024x4096xbf16>} : () -> tensor<1024x4096xbf16>
    %4 = "tosa.const"() {value = dense_resource<torch_tensor_1024_torch.bfloat16_4> : tensor<1024xbf16>} : () -> tensor<1024xbf16>
    %5 = "tosa.const"() {value = dense_resource<torch_tensor_1024_torch.bfloat16_3> : tensor<1024xbf16>} : () -> tensor<1024xbf16>
    %6 = "tosa.const"() {value = dense_resource<torch_tensor_1024_torch.bfloat16_2> : tensor<1024xbf16>} : () -> tensor<1024xbf16>
    %7 = "tosa.const"() {value = dense_resource<torch_tensor_1024_1024_torch.bfloat16> : tensor<1024x1024xbf16>} : () -> tensor<1024x1024xbf16>
    %8 = "tosa.const"() {value = dense_resource<torch_tensor_3072_torch.bfloat16> : tensor<3072xbf16>} : () -> tensor<3072xbf16>
    %9 = "tosa.const"() {value = dense_resource<torch_tensor_1024_3072_torch.bfloat16> : tensor<1024x3072xbf16>} : () -> tensor<1024x3072xbf16>
    %10 = "tosa.const"() {value = dense_resource<torch_tensor_1024_torch.bfloat16_1> : tensor<1024xbf16>} : () -> tensor<1024xbf16>
    %11 = "tosa.const"() {value = dense_resource<torch_tensor_1024_torch.bfloat16> : tensor<1024xbf16>} : () -> tensor<1024xbf16>
    %12 = "tosa.const"() {value = dense<0> : tensor<1xi8>} : () -> tensor<1xi8>
    %15 = "tosa.const"() {value = dense<0.000000e+00> : tensor<1xbf16>} : () -> tensor<1xbf16>
    %39 = "tosa.const"() {value = dense<9.765620e-04> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %40 = "tosa.const"() {value = dense<1.001360e-05> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %41 = "tosa.const"() {value = dense<"0x00000000000000000100000000000000020000000000000003000000000000000400000000000000050000000000000006000000000000000700000000000000080000000000000009000000000000000A000000000000000B000000000000000C000000000000000D000000000000000E000000000000000F0000000000000010000000000000001100000000000000120000000000000013000000000000001400000000000000150000000000000016000000000000001700000000000000180000000000000019000000000000001A000000000000001B000000000000001C000000000000001D000000000000001E000000000000001F0000000000000020000000000000002100000000000000220000000000000023000000000000002400000000000000250000000000000026000000000000002700000000000000280000000000000029000000000000002A000000000000002B000000000000002C000000000000002D000000000000002E000000000000002F0000000000000030000000000000003100000000000000320000000000000033000000000000003400000000000000350000000000000036000000000000003700000000000000380000000000000039000000000000003A000000000000003B000000000000003C000000000000003D000000000000003E000000000000003F0000000000000040000000000000004100000000000000420000000000000043000000000000004400000000000000450000000000000046000000000000004700000000000000480000000000000049000000000000004A000000000000004B000000000000004C000000000000004D000000000000004E000000000000004F0000000000000050000000000000005100000000000000520000000000000053000000000000005400000000000000550000000000000056000000000000005700000000000000580000000000000059000000000000005A000000000000005B000000000000005C000000000000005D000000000000005E000000000000005F0000000000000060000000000000006100000000000000620000000000000063000000000000006400000000000000650000000000000066000000000000006700000000000000680000000000000069000000000000006A000000000000006B000000000000006C000000000000006D000000000000006E000000000000006F0000000000000070000000000000007100000000000000720000000000000073000000000000007400000000000000750000000000000076000000000000007700000000000000780000000000000079000000000000007A000000000000007B000000000000007C000000000000007D000000000000007E000000000000007F00000000000000"> : tensor<1x1x128x1xi64>} : () -> tensor<1x1x128x1xi64>
    %42 = "tosa.const"() {value = dense<1.250000e-01> : tensor<1x1x1x1xbf16>} : () -> tensor<1x1x1x1xbf16>
    %43 = "tosa.const"() {value = dense<-3.389530e+38> : tensor<1x1x1x1xbf16>} : () -> tensor<1x1x1x1xbf16>
    %44 = "tosa.const"() {value = dense<5.000000e-01> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %45 = "tosa.const"() {value = dense<3.000000e+00> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %46 = "tosa.const"() {value = dense<4.467770e-02> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %47 = "tosa.const"() {value = dense<7.968750e-01> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %48 = "tosa.const"() {value = dense<1.000000e+00> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %49 = "tosa.reduce_sum" (%arg48) {axis = 2 : i64} : (tensor<1x1x1024xbf16>) -> tensor<1x1x1xbf16>
    %50 = "tosa.mul" (%49, %39) {shift = 0 : i32} : (tensor<1x1x1xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %51 = "tosa.tile" (%50) {multiples = [1, 1, 1024]} : (tensor<1x1x1xbf16>) -> tensor<1x1x1024xbf16>
    %52 = "tosa.sub" (%arg48, %51) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %53 = "tosa.mul" (%52, %52) {shift = 0 : i32} : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %54 = "tosa.reduce_sum" (%53) {axis = 2 : i64} : (tensor<1x1x1024xbf16>) -> tensor<1x1x1xbf16>
    %55 = "tosa.mul" (%54, %39) {shift = 0 : i32} : (tensor<1x1x1xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %56 = "tosa.add" (%55, %40) : (tensor<1x1x1xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %57 = "tosa.rsqrt" (%56) : (tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %58 = "tosa.tile" (%57) {multiples = [1, 1, 1024]} : (tensor<1x1x1xbf16>) -> tensor<1x1x1024xbf16>
    %59 = "tosa.mul" (%52, %58) {shift = 0 : i32} : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %60 = "tosa.reshape" (%11) {new_shape = [1, 1, 1024]} : (tensor<1024xbf16>) -> tensor<1x1x1024xbf16>
    %61 = "tosa.mul" (%59, %60) {shift = 0 : i32} : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %62 = "tosa.reshape" (%10) {new_shape = [1, 1, 1024]} : (tensor<1024xbf16>) -> tensor<1x1x1024xbf16>
    %63 = "tosa.add" (%61, %62) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %64 = "tosa.reshape" (%9) {new_shape = [1, 1024, 3072]} : (tensor<1024x3072xbf16>) -> tensor<1x1024x3072xbf16>
    %65 = "tosa.matmul" (%63, %64) : (tensor<1x1x1024xbf16>, tensor<1x1024x3072xbf16>) -> tensor<1x1x3072xf32>
    %66 = "tosa.reshape" (%65) {new_shape = [1, 3072]} : (tensor<1x1x3072xf32>) -> tensor<1x3072xf32>
    %67 = "tosa.cast" (%66) : (tensor<1x3072xf32>) -> tensor<1x3072xbf16>
    %68 = "tosa.reshape" (%8) {new_shape = [1, 3072]} : (tensor<3072xbf16>) -> tensor<1x3072xbf16>
    %69 = "tosa.add" (%67, %68) : (tensor<1x3072xbf16>, tensor<1x3072xbf16>) -> tensor<1x3072xbf16>
    %70 = "tosa.reshape" (%69) {new_shape = [1, 1, 3072]} : (tensor<1x3072xbf16>) -> tensor<1x1x3072xbf16>
    %71 = "tosa.slice" (%70) {start = [0, 0, 0], size = [1, 1, 1024]} : (tensor<1x1x3072xbf16>) -> tensor<1x1x1024xbf16>
    %72 = "tosa.slice" (%70) {start = [0, 0, 1024], size = [1, 1, 1024]} : (tensor<1x1x3072xbf16>) -> tensor<1x1x1024xbf16>
    %73 = "tosa.slice" (%70) {start = [0, 0, 2048], size = [1, 1, 1024]} : (tensor<1x1x3072xbf16>) -> tensor<1x1x1024xbf16>
    %74 = "tosa.reshape" (%72) {new_shape = [1, 16, 1, 64]} : (tensor<1x1x1024xbf16>) -> tensor<1x16x1x64xbf16>
    %75 = "tosa.reshape" (%73) {new_shape = [1, 16, 1, 64]} : (tensor<1x1x1024xbf16>) -> tensor<1x16x1x64xbf16>
    %76 = "tosa.reshape" (%arg49) {new_shape = [1, 1, 1, 1]} : (tensor<1xi32>) -> tensor<1x1x1x1xi32>
    %77 = "tosa.cast" (%76) : (tensor<1x1x1x1xi32>) -> tensor<1x1x1x1xi64>
    %78 = "tosa.equal" (%77, %41) : (tensor<1x1x1x1xi64>, tensor<1x1x128x1xi64>) -> tensor<1x1x128x1xi1>
    %79 = "tosa.tile" (%74) {multiples = [1, 1, 128, 1]} : (tensor<1x16x1x64xbf16>) -> tensor<1x16x128x64xbf16>
    %80 = "tosa.tile" (%75) {multiples = [1, 1, 128, 1]} : (tensor<1x16x1x64xbf16>) -> tensor<1x16x128x64xbf16>
    %81 = "tosa.select" (%78, %79, %arg51) : (tensor<1x1x128x1xi1>, tensor<1x16x128x64xbf16>, tensor<1x16x128x64xbf16>) -> tensor<1x16x128x64xbf16>
    %82 = "tosa.select" (%78, %80, %arg52) : (tensor<1x1x128x1xi1>, tensor<1x16x128x64xbf16>, tensor<1x16x128x64xbf16>) -> tensor<1x16x128x64xbf16>
    %tf_perm_0 = "tosa.const"() {value = dense<[0, 1, 3, 2]> : tensor<4xi32>} : () -> tensor<4xi32>
    %83 = "tosa.transpose" (%81, %tf_perm_0) : (tensor<1x16x128x64xbf16>, tensor<4xi32>) -> tensor<1x16x64x128xbf16>
    %84 = "tosa.reshape" (%71) {new_shape = [16, 1, 64]} : (tensor<1x1x1024xbf16>) -> tensor<16x1x64xbf16>
    %85 = "tosa.reshape" (%83) {new_shape = [16, 64, 128]} : (tensor<1x16x64x128xbf16>) -> tensor<16x64x128xbf16>
    %86 = "tosa.matmul" (%84, %85) : (tensor<16x1x64xbf16>, tensor<16x64x128xbf16>) -> tensor<16x1x128xf32>
    %87 = "tosa.reshape" (%86) {new_shape = [1, 16, 1, 128]} : (tensor<16x1x128xf32>) -> tensor<1x16x1x128xf32>
    %88 = "tosa.cast" (%87) : (tensor<1x16x1x128xf32>) -> tensor<1x16x1x128xbf16>
    %89 = "tosa.mul" (%88, %42) {shift = 0 : i32} : (tensor<1x16x1x128xbf16>, tensor<1x1x1x1xbf16>) -> tensor<1x16x1x128xbf16>
    %90 = "tosa.slice" (%arg0) {start = [0, 0, 127, 0], size = [1, 1, 1, 1024]} : (tensor<1x1x1024x1024xi1>) -> tensor<1x1x1x1024xi1>
    %91 = "tosa.slice" (%90) {start = [0, 0, 0, 0], size = [1, 1, 1, 128]} : (tensor<1x1x1x1024xi1>) -> tensor<1x1x1x128xi1>
    %92 = "tosa.select" (%91, %89, %43) : (tensor<1x1x1x128xi1>, tensor<1x16x1x128xbf16>, tensor<1x1x1x1xbf16>) -> tensor<1x16x1x128xbf16>
    %93 = "tosa.add" (%92, %arg50) : (tensor<1x16x1x128xbf16>, tensor<1x1x1x128xbf16>) -> tensor<1x16x1x128xbf16>
    %94 = "tosa.cast" (%93) : (tensor<1x16x1x128xbf16>) -> tensor<1x16x1x128xf32>
    %95 = "tosa.reduce_max" (%94) {axis = 3 : i64} : (tensor<1x16x1x128xf32>) -> tensor<1x16x1x1xf32>
    %96 = "tosa.sub" (%94, %95) : (tensor<1x16x1x128xf32>, tensor<1x16x1x1xf32>) -> tensor<1x16x1x128xf32>
    %97 = "tosa.exp" (%96) : (tensor<1x16x1x128xf32>) -> tensor<1x16x1x128xf32>
    %98 = "tosa.reduce_sum" (%97) {axis = 3 : i64} : (tensor<1x16x1x128xf32>) -> tensor<1x16x1x1xf32>
    %99 = "tosa.reciprocal" (%98) : (tensor<1x16x1x1xf32>) -> tensor<1x16x1x1xf32>
    %100 = "tosa.mul" (%97, %99) {shift = 0 : i32} : (tensor<1x16x1x128xf32>, tensor<1x16x1x1xf32>) -> tensor<1x16x1x128xf32>
    %101 = "tosa.cast" (%100) : (tensor<1x16x1x128xf32>) -> tensor<1x16x1x128xbf16>
    %102 = "tosa.reshape" (%101) {new_shape = [16, 1, 128]} : (tensor<1x16x1x128xbf16>) -> tensor<16x1x128xbf16>
    %103 = "tosa.reshape" (%82) {new_shape = [16, 128, 64]} : (tensor<1x16x128x64xbf16>) -> tensor<16x128x64xbf16>
    %104 = "tosa.matmul" (%102, %103) : (tensor<16x1x128xbf16>, tensor<16x128x64xbf16>) -> tensor<16x1x64xf32>
    %105 = "tosa.reshape" (%104) {new_shape = [1, 16, 1, 64]} : (tensor<16x1x64xf32>) -> tensor<1x16x1x64xf32>
    %106 = "tosa.cast" (%105) : (tensor<1x16x1x64xf32>) -> tensor<1x16x1x64xbf16>
    %107 = "tosa.reshape" (%106) {new_shape = [1, 1, 1024]} : (tensor<1x16x1x64xbf16>) -> tensor<1x1x1024xbf16>
    %108 = "tosa.reshape" (%7) {new_shape = [1, 1024, 1024]} : (tensor<1024x1024xbf16>) -> tensor<1x1024x1024xbf16>
    %109 = "tosa.matmul" (%107, %108) : (tensor<1x1x1024xbf16>, tensor<1x1024x1024xbf16>) -> tensor<1x1x1024xf32>
    %110 = "tosa.reshape" (%109) {new_shape = [1, 1024]} : (tensor<1x1x1024xf32>) -> tensor<1x1024xf32>
    %111 = "tosa.cast" (%110) : (tensor<1x1024xf32>) -> tensor<1x1024xbf16>
    %112 = "tosa.reshape" (%6) {new_shape = [1, 1024]} : (tensor<1024xbf16>) -> tensor<1x1024xbf16>
    %113 = "tosa.add" (%111, %112) : (tensor<1x1024xbf16>, tensor<1x1024xbf16>) -> tensor<1x1024xbf16>
    %114 = "tosa.reshape" (%113) {new_shape = [1, 1, 1024]} : (tensor<1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %115 = "tosa.add" (%114, %arg48) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %116 = "tosa.reduce_sum" (%115) {axis = 2 : i64} : (tensor<1x1x1024xbf16>) -> tensor<1x1x1xbf16>
    %117 = "tosa.mul" (%116, %39) {shift = 0 : i32} : (tensor<1x1x1xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %118 = "tosa.tile" (%117) {multiples = [1, 1, 1024]} : (tensor<1x1x1xbf16>) -> tensor<1x1x1024xbf16>
    %119 = "tosa.sub" (%115, %118) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %120 = "tosa.mul" (%119, %119) {shift = 0 : i32} : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %121 = "tosa.reduce_sum" (%120) {axis = 2 : i64} : (tensor<1x1x1024xbf16>) -> tensor<1x1x1xbf16>
    %122 = "tosa.mul" (%121, %39) {shift = 0 : i32} : (tensor<1x1x1xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %123 = "tosa.add" (%122, %40) : (tensor<1x1x1xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %124 = "tosa.rsqrt" (%123) : (tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %125 = "tosa.tile" (%124) {multiples = [1, 1, 1024]} : (tensor<1x1x1xbf16>) -> tensor<1x1x1024xbf16>
    %126 = "tosa.mul" (%119, %125) {shift = 0 : i32} : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %127 = "tosa.reshape" (%5) {new_shape = [1, 1, 1024]} : (tensor<1024xbf16>) -> tensor<1x1x1024xbf16>
    %128 = "tosa.mul" (%126, %127) {shift = 0 : i32} : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %129 = "tosa.reshape" (%4) {new_shape = [1, 1, 1024]} : (tensor<1024xbf16>) -> tensor<1x1x1024xbf16>
    %130 = "tosa.add" (%128, %129) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %131 = "tosa.reshape" (%3) {new_shape = [1, 1024, 4096]} : (tensor<1024x4096xbf16>) -> tensor<1x1024x4096xbf16>
    %132 = "tosa.matmul" (%130, %131) : (tensor<1x1x1024xbf16>, tensor<1x1024x4096xbf16>) -> tensor<1x1x4096xf32>
    %133 = "tosa.reshape" (%132) {new_shape = [1, 4096]} : (tensor<1x1x4096xf32>) -> tensor<1x4096xf32>
    %134 = "tosa.cast" (%133) : (tensor<1x4096xf32>) -> tensor<1x4096xbf16>
    %135 = "tosa.reshape" (%2) {new_shape = [1, 4096]} : (tensor<4096xbf16>) -> tensor<1x4096xbf16>
    %136 = "tosa.add" (%134, %135) : (tensor<1x4096xbf16>, tensor<1x4096xbf16>) -> tensor<1x4096xbf16>
    %137 = "tosa.reshape" (%136) {new_shape = [1, 1, 4096]} : (tensor<1x4096xbf16>) -> tensor<1x1x4096xbf16>
    %138 = "tosa.mul" (%137, %44) {shift = 0 : i32} : (tensor<1x1x4096xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x4096xbf16>
    %139 = "tosa.pow" (%137, %45) : (tensor<1x1x4096xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x4096xbf16>
    %140 = "tosa.mul" (%139, %46) {shift = 0 : i32} : (tensor<1x1x4096xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x4096xbf16>
    %141 = "tosa.add" (%137, %140) : (tensor<1x1x4096xbf16>, tensor<1x1x4096xbf16>) -> tensor<1x1x4096xbf16>
    %142 = "tosa.mul" (%141, %47) {shift = 0 : i32} : (tensor<1x1x4096xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x4096xbf16>
    %143 = "tosa.tanh" (%142) : (tensor<1x1x4096xbf16>) -> tensor<1x1x4096xbf16>
    %144 = "tosa.add" (%143, %48) : (tensor<1x1x4096xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x4096xbf16>
    %145 = "tosa.mul" (%138, %144) {shift = 0 : i32} : (tensor<1x1x4096xbf16>, tensor<1x1x4096xbf16>) -> tensor<1x1x4096xbf16>
    %146 = "tosa.reshape" (%1) {new_shape = [1, 4096, 1024]} : (tensor<4096x1024xbf16>) -> tensor<1x4096x1024xbf16>
    %147 = "tosa.matmul" (%145, %146) : (tensor<1x1x4096xbf16>, tensor<1x4096x1024xbf16>) -> tensor<1x1x1024xf32>
    %148 = "tosa.reshape" (%147) {new_shape = [1, 1024]} : (tensor<1x1x1024xf32>) -> tensor<1x1024xf32>
    %149 = "tosa.cast" (%148) : (tensor<1x1024xf32>) -> tensor<1x1024xbf16>
    %150 = "tosa.reshape" (%0) {new_shape = [1, 1024]} : (tensor<1024xbf16>) -> tensor<1x1024xbf16>
    %151 = "tosa.add" (%149, %150) : (tensor<1x1024xbf16>, tensor<1x1024xbf16>) -> tensor<1x1024xbf16>
    %152 = "tosa.reshape" (%151) {new_shape = [1, 1, 1024]} : (tensor<1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %153 = "tosa.add" (%115, %152) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    return %153, %81, %82 : tensor<1x1x1024xbf16>, tensor<1x16x128x64xbf16>, tensor<1x16x128x64xbf16>
  }
}
