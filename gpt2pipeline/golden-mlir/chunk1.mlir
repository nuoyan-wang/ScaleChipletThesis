module {
  func.func @kernel_0_chip1(%arg0: tensor<1x1x1024xbf16> {chiplet.bytes = 2048 : i64, chiplet.comm_dir = "recv", chiplet.comm_kind = "axis", chiplet.sender_id = 0 : i64}) -> (tensor<1x1x4096xbf16> {chiplet.bytes = 8192 : i64, chiplet.comm_dir = "send", chiplet.comm_kind = "axis", chiplet.receiver_id = 2 : i64}) {
    %0 = "tosa.const"() {value = dense<9.765620e-04> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %1 = "tosa.const"() {value = dense<1.001360e-05> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %2 = "tosa.const"() {value = dense_resource<torch_tensor_1024_torch.bfloat16_3> : tensor<1024xbf16>} : () -> tensor<1024xbf16>
    %3 = "tosa.const"() {value = dense_resource<torch_tensor_1024_torch.bfloat16_4> : tensor<1024xbf16>} : () -> tensor<1024xbf16>
    %4 = "tosa.const"() {value = dense_resource<torch_tensor_1024_4096_torch.bfloat16> : tensor<1024x4096xbf16>} : () -> tensor<1024x4096xbf16>
    %5 = "tosa.const"() {value = dense_resource<torch_tensor_4096_torch.bfloat16> : tensor<4096xbf16>} : () -> tensor<4096xbf16>
    %6 = "tosa.const"() {value = dense<5.000000e-01> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %7 = "tosa.const"() {value = dense<3.000000e+00> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %8 = "tosa.const"() {value = dense<4.467770e-02> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %9 = "tosa.const"() {value = dense<7.968750e-01> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %10 = "tosa.const"() {value = dense<1.000000e+00> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %11 = "tosa.reduce_sum"(%arg0) {axis = 2 : i64} : (tensor<1x1x1024xbf16>) -> tensor<1x1x1xbf16>
    %12 = "tosa.mul"(%11, %0) {shift = 0 : i32} : (tensor<1x1x1xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %13 = "tosa.tile"(%12) {multiples = [1, 1, 1024]} : (tensor<1x1x1xbf16>) -> tensor<1x1x1024xbf16>
    %14 = "tosa.sub"(%arg0, %13) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %15 = "tosa.mul"(%14, %14) {shift = 0 : i32} : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %16 = "tosa.reduce_sum"(%15) {axis = 2 : i64} : (tensor<1x1x1024xbf16>) -> tensor<1x1x1xbf16>
    %17 = "tosa.mul"(%16, %0) {shift = 0 : i32} : (tensor<1x1x1xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %18 = "tosa.add"(%17, %1) : (tensor<1x1x1xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %19 = "tosa.rsqrt"(%18) : (tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %20 = "tosa.tile"(%19) {multiples = [1, 1, 1024]} : (tensor<1x1x1xbf16>) -> tensor<1x1x1024xbf16>
    %21 = "tosa.mul"(%14, %20) {shift = 0 : i32} : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %22 = "tosa.reshape"(%2) {new_shape = [1, 1, 1024]} : (tensor<1024xbf16>) -> tensor<1x1x1024xbf16>
    %23 = "tosa.mul"(%21, %22) {shift = 0 : i32} : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %24 = "tosa.reshape"(%3) {new_shape = [1, 1, 1024]} : (tensor<1024xbf16>) -> tensor<1x1x1024xbf16>
    %25 = "tosa.add"(%23, %24) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %26 = "tosa.reshape"(%4) {new_shape = [1, 1024, 4096]} : (tensor<1024x4096xbf16>) -> tensor<1x1024x4096xbf16>
    %27 = "tosa.matmul"(%25, %26) : (tensor<1x1x1024xbf16>, tensor<1x1024x4096xbf16>) -> tensor<1x1x4096xf32>
    %28 = "tosa.reshape"(%27) {new_shape = [1, 4096]} : (tensor<1x1x4096xf32>) -> tensor<1x4096xf32>
    %29 = "tosa.cast"(%28) : (tensor<1x4096xf32>) -> tensor<1x4096xbf16>
    %30 = "tosa.reshape"(%5) {new_shape = [1, 4096]} : (tensor<4096xbf16>) -> tensor<1x4096xbf16>
    %31 = "tosa.add"(%29, %30) : (tensor<1x4096xbf16>, tensor<1x4096xbf16>) -> tensor<1x4096xbf16>
    %32 = "tosa.reshape"(%31) {new_shape = [1, 1, 4096]} : (tensor<1x4096xbf16>) -> tensor<1x1x4096xbf16>
    %33 = "tosa.mul"(%32, %6) {shift = 0 : i32} : (tensor<1x1x4096xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x4096xbf16>
    %34 = "tosa.pow"(%32, %7) : (tensor<1x1x4096xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x4096xbf16>
    %35 = "tosa.mul"(%34, %8) {shift = 0 : i32} : (tensor<1x1x4096xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x4096xbf16>
    %36 = "tosa.add"(%32, %35) : (tensor<1x1x4096xbf16>, tensor<1x1x4096xbf16>) -> tensor<1x1x4096xbf16>
    %37 = "tosa.mul"(%36, %9) {shift = 0 : i32} : (tensor<1x1x4096xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x4096xbf16>
    %38 = "tosa.tanh"(%37) : (tensor<1x1x4096xbf16>) -> tensor<1x1x4096xbf16>
    %39 = "tosa.add"(%38, %10) : (tensor<1x1x4096xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x4096xbf16>
    %40 = "tosa.mul"(%33, %39) {shift = 0 : i32} : (tensor<1x1x4096xbf16>, tensor<1x1x4096xbf16>) -> tensor<1x1x4096xbf16>
    %41 = "tosa.const"() {value = dense<0.000000e+00> : tensor<1x1x4096xbf16>} : () -> tensor<1x1x4096xbf16>
    %42 = "tosa.add"(%40, %41) : (tensor<1x1x4096xbf16>, tensor<1x1x4096xbf16>) -> tensor<1x1x4096xbf16>
    return %42 : tensor<1x1x4096xbf16>
  }
}
