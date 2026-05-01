module {
  func.func @kernel_0_chip2(%arg0: tensor<1x1x1024xbf16> {chiplet.bytes = 2048 : i64, chiplet.comm_dir = "recv", chiplet.comm_kind = "axis", chiplet.sender_id = 0 : i64}, %arg1: tensor<1x1x4096xbf16> {chiplet.bytes = 8192 : i64, chiplet.comm_dir = "recv", chiplet.comm_kind = "axis", chiplet.sender_id = 1 : i64}) -> tensor<1x1x1024xbf16> {
    %_tf_zero_i8 = "tosa.const"() {values = dense<0> : tensor<1xi8>} : () -> tensor<1xi8>
    %_tf_zero_bf16 = "tosa.const"() {values = dense<0.000000e+00> : tensor<1xbf16>} : () -> tensor<1xbf16>
    %0 = "tosa.const"() {values = dense_resource<torch_tensor_4096_1024_torch.bfloat16> : tensor<4096x1024xbf16>} : () -> tensor<4096x1024xbf16>
    %1 = "tosa.const"() {values = dense_resource<torch_tensor_1024_torch.bfloat16_5> : tensor<1024xbf16>} : () -> tensor<1024xbf16>
    %_tf_reshape_shape_0 = tosa.const_shape  {values = dense<[1, 4096, 1024]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %2 = "tosa.reshape" (%0, %_tf_reshape_shape_0) : (tensor<4096x1024xbf16>, !tosa.shape<3>) -> tensor<1x4096x1024xbf16>
    %3 = "tosa.matmul" (%arg1, %2, %_tf_zero_bf16, %_tf_zero_bf16) : (tensor<1x1x4096xbf16>, tensor<1x4096x1024xbf16>, tensor<1xbf16>, tensor<1xbf16>) -> tensor<1x1x1024xf32>
    %_tf_reshape_shape_1 = tosa.const_shape  {values = dense<[1, 1024]> : tensor<2xindex>} : () -> !tosa.shape<2>
    %4 = "tosa.reshape" (%3, %_tf_reshape_shape_1) : (tensor<1x1x1024xf32>, !tosa.shape<2>) -> tensor<1x1024xf32>
    %5 = "tosa.cast"(%4) : (tensor<1x1024xf32>) -> tensor<1x1024xbf16>
    %_tf_reshape_shape_2 = tosa.const_shape  {values = dense<[1, 1024]> : tensor<2xindex>} : () -> !tosa.shape<2>
    %6 = "tosa.reshape" (%1, %_tf_reshape_shape_2) : (tensor<1024xbf16>, !tosa.shape<2>) -> tensor<1x1024xbf16>
    %7 = "tosa.add"(%5, %6) : (tensor<1x1024xbf16>, tensor<1x1024xbf16>) -> tensor<1x1024xbf16>
    %_tf_reshape_shape_3 = tosa.const_shape  {values = dense<[1, 1, 1024]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %8 = "tosa.reshape" (%7, %_tf_reshape_shape_3) : (tensor<1x1024xbf16>, !tosa.shape<3>) -> tensor<1x1x1024xbf16>
    %9 = "tosa.add"(%arg0, %8) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    return %9 : tensor<1x1x1024xbf16>
  }
}
