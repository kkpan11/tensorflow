/* Copyright 2024 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "tensorflow/compiler/mlir/tf2xla/api/v2/testing/compile_mlir.h"
#include "tensorflow/compiler/tf2xla/xla_compiler.h"
#include "xla/tsl/platform/status_matchers.h"
#include "xla/tsl/platform/statusor.h"
#include "tensorflow/core/platform/test.h"
#include "tensorflow/core/protobuf/config.pb.h"
#include "tensorflow/core/protobuf/tpu/compile_metadata.pb.h"

namespace tensorflow {
namespace tf2xla {
namespace v2 {

// These test are in a separate file because they requires separate build
// environments (--config=cuda).
using tensorflow::tf2xla::v2::testing::CompileMlirModule;
using tsl::testing::StatusIs;

// MLIR which is legalize only with the right device type.
// The mlir is generated by running
// tensorflow/compiler/mlir/tf-opt -tf-xla-call-module-serialization
//
// module attributes {tf.versions = {bad_consumers = [], min_consumer = 0 : i32,
// producer = 268 : i32}} {
//   func.func private @_jit_sin(%arg0: tensor<f32>) -> tensor<f32> {
//       %0 = stablehlo.sine %arg0 : tensor<f32>
//       return %0 : tensor<f32>
//     }
//   func.func @main(%arg0: tensor<f32>) -> tensor<*xf32> {
//       %0 = "tf.XlaCallModule"(%arg0) {Sout = [#tf_type.shape<*>], device =
//            "", dim_args_spec = [], _entry_function = @_jit_sin,
//            _stablehlo_version = "1.0.0", module = "", platforms = ["CUDA"],
//            version = 6 : i64} : (tensor<f32>) -> tensor<*xf32>
//       func.return %0 : tensor<*xf32>
//    }
// }
//
static constexpr char kGpuMlirModuleStr[] = R"(
module attributes {tf.versions = {bad_consumers = [], min_consumer = 0 : i32, producer = 268 : i32}} {
  func.func @main(%arg0: tensor<f32>) -> tensor<*xf32> {
   %0 = "tf.XlaCallModule"(%arg0) {Sout = [#tf_type.shape<*>], device = "", dim_args_spec = [], module = "ML\EFR\03StableHLOv1.0.0\00\01\17\05\01\05\01\03\05\03\07\07\t\0B\03K5\07\01\1B\07\0B\13\0B3\0B\0B\0B\0B\0F\0B\13\0B\03\1B\0F\1B\0B\0B\0B\0B\0B\0F\13\0B\0B\0B\0B\03\07\0F\17\07\02\A7\1F\05\0D\03\03\03\07\05\0F\03\0B\0B\1B\0D'\0F)\031\113\05\11\05\13\05\15\05\17\1D\15\17\05\19\17\19\EF\01\05\1B\03\03\1D\0D\05\1F!#%\1D\1D\1D\1F\1D!\1D##\03\03\03+\0D\03-/\1D%\1D'\1D)\1D+)\01\05\11\03\01\03\01\t\04A\05\01\11\01\05\07\03\01\05\03\11\01\t\05\03\05\0B\03\01\01\05\06\13\03\01\03\01\07\04\01\03\03\06\03\01\05\01\00\9A\04-\0F\0B\03!\1B\1D\05\1B\83/\1F\15\1D\15\11\13\15\11\11\0F\0B\11builtin\00vhlo\00module\00func_v1\00sine_v1\00return_v1\00sym_name\00jit_sin\00arg_attrs\00function_type\00res_attrs\00sym_visibility\00jit(sin)/jit(main)/sin\00third_party/py/jax/experimental/jax2tf/tests/back_compat_test.py\00jax.arg_info\00x\00mhlo.sharding\00{replicated}\00jax.result_info\00\00main\00public\00", platforms = ["CUDA"], version = 6 : i64} : (tensor<f32>) -> tensor<*xf32>
    func.return %0 : tensor<*xf32>
  }
})";

TEST(LegalizeTFTest, RightDeviceTypeShallPass) {
  TF_ASSERT_OK_AND_ASSIGN(
      XlaCompiler::CompilationResult result,
      CompileMlirModule(
          kGpuMlirModuleStr,
          ConfigProto::Experimental::MLIR_BRIDGE_ROLLOUT_UNSPECIFIED,
          "XLA_GPU_JIT"));
}

TEST(LegalizeTFTest, WrongDeviceTypeShallFail) {
  absl::StatusOr<XlaCompiler::CompilationResult> result = CompileMlirModule(
      kGpuMlirModuleStr,
      ConfigProto::Experimental::MLIR_BRIDGE_ROLLOUT_UNSPECIFIED,
      "XLA_TPU_JIT");
  EXPECT_THAT(result, StatusIs(absl::StatusCode::kNotFound));
}

}  // namespace v2
}  // namespace tf2xla
}  // namespace tensorflow
