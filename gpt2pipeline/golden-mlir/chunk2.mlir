module {
  func.func @kernel_0_chip2(%arg0: tensor<1x1x1024xbf16> {chiplet.bytes = 2048 : i64, chiplet.comm_dir = "recv", chiplet.comm_kind = "axis", chiplet.sender_id = 0 : i64}, %arg1: tensor<1x1x4096xbf16> {chiplet.bytes = 8192 : i64, chiplet.comm_dir = "recv", chiplet.comm_kind = "axis", chiplet.sender_id = 1 : i64}) -> tensor<1x1x1024xbf16> {
    %0 = "tosa.const"() {value = dense_resource<torch_tensor_4096_1024_torch.bfloat16> : tensor<4096x1024xbf16>} : () -> tensor<4096x1024xbf16>
    %1 = "tosa.const"() {value = dense_resource<torch_tensor_1024_torch.bfloat16_5> : tensor<1024xbf16>} : () -> tensor<1024xbf16>
    %2 = "tosa.reshape"(%0) {new_shape = [1, 4096, 1024]} : (tensor<4096x1024xbf16>) -> tensor<1x4096x1024xbf16>
    %3 = "tosa.matmul"(%arg1, %2) : (tensor<1x1x4096xbf16>, tensor<1x4096x1024xbf16>) -> tensor<1x1x1024xf32>
    %4 = "tosa.reshape"(%3) {new_shape = [1, 1024]} : (tensor<1x1x1024xf32>) -> tensor<1x1024xf32>
    %5 = "tosa.cast"(%4) : (tensor<1x1024xf32>) -> tensor<1x1024xbf16>
    %6 = "tosa.reshape"(%1) {new_shape = [1, 1024]} : (tensor<1024xbf16>) -> tensor<1x1024xbf16>
    %7 = "tosa.add"(%5, %6) : (tensor<1x1024xbf16>, tensor<1x1024xbf16>) -> tensor<1x1024xbf16>
    %8 = "tosa.reshape"(%7) {new_shape = [1, 1, 1024]} : (tensor<1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %9 = "tosa.add"(%arg0, %8) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    return %9 : tensor<1x1x1024xbf16>
  }
}
