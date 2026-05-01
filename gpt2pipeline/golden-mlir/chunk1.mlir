module {
  func.func @kernel_0_chip1(%arg0: tensor<1x1x1024xbf16> {chiplet.bytes = 2048 : i64, chiplet.comm_dir = "recv", chiplet.comm_kind = "axis", chiplet.sender_id = 0 : i64}) -> (tensor<1x1x4096xbf16> {chiplet.bytes = 8192 : i64, chiplet.comm_dir = "send", chiplet.comm_kind = "axis", chiplet.receiver_id = 2 : i64}) {
    %_tf_zero_i8 = "tosa.const"() {values = dense<0> : tensor<1xi8>} : () -> tensor<1xi8>
    %_tf_zero_bf16 = "tosa.const"() {values = dense<0.000000e+00> : tensor<1xbf16>} : () -> tensor<1xbf16>
    %0 = "tosa.const"() {values = dense<9.765620e-04> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %1 = "tosa.const"() {values = dense<1.001360e-05> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %2 = "tosa.const"() {values = dense_resource<torch_tensor_1024_torch.bfloat16_3> : tensor<1024xbf16>} : () -> tensor<1024xbf16>
    %3 = "tosa.const"() {values = dense_resource<torch_tensor_1024_torch.bfloat16_4> : tensor<1024xbf16>} : () -> tensor<1024xbf16>
    %4 = "tosa.const"() {values = dense_resource<torch_tensor_1024_4096_torch.bfloat16> : tensor<1024x4096xbf16>} : () -> tensor<1024x4096xbf16>
    %5 = "tosa.const"() {values = dense_resource<torch_tensor_4096_torch.bfloat16> : tensor<4096xbf16>} : () -> tensor<4096xbf16>
    %6 = "tosa.const"() {values = dense<5.000000e-01> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %7 = "tosa.const"() {values = dense<3.000000e+00> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %8 = "tosa.const"() {values = dense<4.467770e-02> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %9 = "tosa.const"() {values = dense<7.968750e-01> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %10 = "tosa.const"() {values = dense<1.000000e+00> : tensor<1x1x1xbf16>} : () -> tensor<1x1x1xbf16>
    %11 = "tosa.reduce_sum"(%arg0) {axis = 2 : i32} : (tensor<1x1x1024xbf16>) -> tensor<1x1x1xbf16>
    %12 = "tosa.mul" (%11, %0, %_tf_zero_i8) : (tensor<1x1x1xbf16>, tensor<1x1x1xbf16>, tensor<1xi8>) -> tensor<1x1x1xbf16>
    %_tf_tile_shape_0 = tosa.const_shape  {values = dense<[1, 1, 1024]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %13 = "tosa.tile" (%12, %_tf_tile_shape_0) : (tensor<1x1x1xbf16>, !tosa.shape<3>) -> tensor<1x1x1024xbf16>
    %14 = "tosa.sub"(%arg0, %13) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %15 = "tosa.mul" (%14, %14, %_tf_zero_i8) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>, tensor<1xi8>) -> tensor<1x1x1024xbf16>
    %16 = "tosa.reduce_sum"(%15) {axis = 2 : i32} : (tensor<1x1x1024xbf16>) -> tensor<1x1x1xbf16>
    %17 = "tosa.mul" (%16, %0, %_tf_zero_i8) : (tensor<1x1x1xbf16>, tensor<1x1x1xbf16>, tensor<1xi8>) -> tensor<1x1x1xbf16>
    %18 = "tosa.add"(%17, %1) : (tensor<1x1x1xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %19 = "tosa.rsqrt"(%18) : (tensor<1x1x1xbf16>) -> tensor<1x1x1xbf16>
    %_tf_tile_shape_1 = tosa.const_shape  {values = dense<[1, 1, 1024]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %20 = "tosa.tile" (%19, %_tf_tile_shape_1) : (tensor<1x1x1xbf16>, !tosa.shape<3>) -> tensor<1x1x1024xbf16>
    %21 = "tosa.mul" (%14, %20, %_tf_zero_i8) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>, tensor<1xi8>) -> tensor<1x1x1024xbf16>
    %_tf_reshape_shape_0 = tosa.const_shape  {values = dense<[1, 1, 1024]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %22 = "tosa.reshape" (%2, %_tf_reshape_shape_0) : (tensor<1024xbf16>, !tosa.shape<3>) -> tensor<1x1x1024xbf16>
    %23 = "tosa.mul" (%21, %22, %_tf_zero_i8) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>, tensor<1xi8>) -> tensor<1x1x1024xbf16>
    %_tf_reshape_shape_1 = tosa.const_shape  {values = dense<[1, 1, 1024]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %24 = "tosa.reshape" (%3, %_tf_reshape_shape_1) : (tensor<1024xbf16>, !tosa.shape<3>) -> tensor<1x1x1024xbf16>
    %25 = "tosa.add"(%23, %24) : (tensor<1x1x1024xbf16>, tensor<1x1x1024xbf16>) -> tensor<1x1x1024xbf16>
    %_tf_reshape_shape_2 = tosa.const_shape  {values = dense<[1, 1024, 4096]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %26 = "tosa.reshape" (%4, %_tf_reshape_shape_2) : (tensor<1024x4096xbf16>, !tosa.shape<3>) -> tensor<1x1024x4096xbf16>
    %27 = "tosa.matmul" (%25, %26, %_tf_zero_bf16, %_tf_zero_bf16) : (tensor<1x1x1024xbf16>, tensor<1x1024x4096xbf16>, tensor<1xbf16>, tensor<1xbf16>) -> tensor<1x1x4096xf32>
    %_tf_reshape_shape_3 = tosa.const_shape  {values = dense<[1, 4096]> : tensor<2xindex>} : () -> !tosa.shape<2>
    %28 = "tosa.reshape" (%27, %_tf_reshape_shape_3) : (tensor<1x1x4096xf32>, !tosa.shape<2>) -> tensor<1x4096xf32>
    %29 = "tosa.cast"(%28) : (tensor<1x4096xf32>) -> tensor<1x4096xbf16>
    %_tf_reshape_shape_4 = tosa.const_shape  {values = dense<[1, 4096]> : tensor<2xindex>} : () -> !tosa.shape<2>
    %30 = "tosa.reshape" (%5, %_tf_reshape_shape_4) : (tensor<4096xbf16>, !tosa.shape<2>) -> tensor<1x4096xbf16>
    %31 = "tosa.add"(%29, %30) : (tensor<1x4096xbf16>, tensor<1x4096xbf16>) -> tensor<1x4096xbf16>
    %_tf_reshape_shape_5 = tosa.const_shape  {values = dense<[1, 1, 4096]> : tensor<3xindex>} : () -> !tosa.shape<3>
    %32 = "tosa.reshape" (%31, %_tf_reshape_shape_5) : (tensor<1x4096xbf16>, !tosa.shape<3>) -> tensor<1x1x4096xbf16>
    %33 = "tosa.mul" (%32, %6, %_tf_zero_i8) : (tensor<1x1x4096xbf16>, tensor<1x1x1xbf16>, tensor<1xi8>) -> tensor<1x1x4096xbf16>
    %34 = "tosa.pow"(%32, %7) : (tensor<1x1x4096xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x4096xbf16>
    %35 = "tosa.mul" (%34, %8, %_tf_zero_i8) : (tensor<1x1x4096xbf16>, tensor<1x1x1xbf16>, tensor<1xi8>) -> tensor<1x1x4096xbf16>
    %36 = "tosa.add"(%32, %35) : (tensor<1x1x4096xbf16>, tensor<1x1x4096xbf16>) -> tensor<1x1x4096xbf16>
    %37 = "tosa.mul" (%36, %9, %_tf_zero_i8) : (tensor<1x1x4096xbf16>, tensor<1x1x1xbf16>, tensor<1xi8>) -> tensor<1x1x4096xbf16>
    %38 = "tosa.tanh"(%37) : (tensor<1x1x4096xbf16>) -> tensor<1x1x4096xbf16>
    %39 = "tosa.add"(%38, %10) : (tensor<1x1x4096xbf16>, tensor<1x1x1xbf16>) -> tensor<1x1x4096xbf16>
    %40 = "tosa.mul" (%33, %39, %_tf_zero_i8) : (tensor<1x1x4096xbf16>, tensor<1x1x4096xbf16>, tensor<1xi8>) -> tensor<1x1x4096xbf16>
    %41 = "tosa.const"() {values = dense<0.000000e+00> : tensor<1x1x4096xbf16>} : () -> tensor<1x1x4096xbf16>
    %42 = "tosa.add"(%40, %41) : (tensor<1x1x4096xbf16>, tensor<1x1x4096xbf16>) -> tensor<1x1x4096xbf16>
    return %42 : tensor<1x1x4096xbf16>
  }
}
