# This package contains the C API libraries for asynchronous execution and buffer interop.
# For clients using async APIs, please use tensorflow/lite/async/c instead of this package.
# NOTE: Targets in this package are experimental.

load("//tensorflow:tensorflow.default.bzl", "get_compatible_with_portable")
load(
    "//tensorflow/lite:build_def.bzl",
    "tflite_cc_library_with_c_headers_test",
    "tflite_copts",
    "tflite_copts_warnings",
)

package(
    # copybara:uncomment default_applicable_licenses = ["//tensorflow:license"],
    licenses = ["notice"],
)

exports_files(
    srcs = [
        "types.h",
    ],
    visibility = [
        "//tensorflow/lite:__subpackages__",
    ],
)

tflite_cc_library_with_c_headers_test(
    name = "types",
    hdrs = ["types.h"],
    compatible_with = get_compatible_with_portable(),
    copts = tflite_copts() + tflite_copts_warnings(),
    visibility = [
        # TODO(b/271281434): Update targets that depend on this to instead use
        # tensorflow/lite/async/c.
        "//visibility:public",
    ],
)

tflite_cc_library_with_c_headers_test(
    name = "async_kernel",
    srcs = ["async_kernel.cc"],
    hdrs = ["async_kernel.h"],
    compatible_with = get_compatible_with_portable(),
    copts = tflite_copts() + tflite_copts_warnings(),
    visibility = [
        # TODO(b/271281434): Update targets that depend on this to instead use
        # tensorflow/lite/async/c.
        "//visibility:public",
    ],
    deps = [
        ":types",
        "//tensorflow/lite/core/async:async_kernel_internal",
        "//tensorflow/lite/core/async/interop/c:attribute_map",
        "//tensorflow/lite/core/async/interop/c:types",
        "//tensorflow/lite/core/c:c_api_types",
        "//tensorflow/lite/core/c:common",
    ],
)

tflite_cc_library_with_c_headers_test(
    name = "task",
    srcs = ["task.cc"],
    hdrs = ["task.h"],
    compatible_with = get_compatible_with_portable(),
    copts = tflite_copts() + tflite_copts_warnings(),
    visibility = [
        # TODO(b/271281434): Update targets that depend on this to instead use
        # tensorflow/lite/async/c.
        "//visibility:public",
    ],
    deps = [
        ":types",
        "//tensorflow/lite/core/async:task_internal",
        "//tensorflow/lite/core/async/interop/c:types",
        "//tensorflow/lite/core/c:c_api_types",
        "//tensorflow/lite/core/c:common",
    ],
)

cc_test(
    name = "task_test",
    srcs = ["task_test.cc"],
    deps = [
        ":task",
        ":types",
        "//tensorflow/lite/core/async:task_internal",
        "//tensorflow/lite/core/async/interop/c:types",
        "//tensorflow/lite/core/c:common",
        "@com_google_googletest//:gtest_main",
    ],
)

tflite_cc_library_with_c_headers_test(
    name = "async_signature_runner",
    srcs = ["async_signature_runner.cc"],
    hdrs = ["async_signature_runner.h"],
    compatible_with = get_compatible_with_portable(),
    copts = tflite_copts() + tflite_copts_warnings(),
    visibility = [
        # TODO(b/271281434): Update targets that depend on this to instead use
        # tensorflow/lite/async/c.
        "//visibility:public",
    ],
    deps = [
        ":internal",
        ":types",
        "//tensorflow/lite/c:c_api_internal",
        "//tensorflow/lite/core/async:async_signature_runner",
        "//tensorflow/lite/core/async/interop/c:attribute_map",
        "//tensorflow/lite/core/async/interop/c:types",
        "//tensorflow/lite/core/c:c_api",
        "//tensorflow/lite/core/c:c_api_types",
        "//tensorflow/lite/core/c:common",
    ],
)

cc_test(
    name = "async_signature_runner_test",
    srcs = ["async_signature_runner_test.cc"],
    copts = tflite_copts() + tflite_copts_warnings(),
    data = [
        "//tensorflow/lite:testdata/no_signatures.bin",
    ],
    deps = [
        ":async_signature_runner",
        ":internal",
        ":task",
        ":types",
        "//tensorflow/lite:interpreter_test_util",
        "//tensorflow/lite/c:c_api_experimental",
        "//tensorflow/lite/c:c_api_internal",
        "//tensorflow/lite/core:cc_api_stable",
        "//tensorflow/lite/core/async:async_kernel_internal",
        "//tensorflow/lite/core/async:backend_async_kernel_interface",
        "//tensorflow/lite/core/async/interop/c:attribute_map",
        "//tensorflow/lite/core/async/interop/c:types",
        "//tensorflow/lite/core/async/testing:mock_async_kernel",
        "//tensorflow/lite/core/async/testing:test_backend",
        "//tensorflow/lite/core/c:c_api_types",
        "//tensorflow/lite/core/c:c_api_without_op_resolver",
        "//tensorflow/lite/core/c:common",
        "//tensorflow/lite/core/kernels:builtin_ops",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "internal",
    hdrs = ["internal.h"],
    compatible_with = get_compatible_with_portable(),
    copts = tflite_copts() + tflite_copts_warnings(),
    visibility = ["//visibility:private"],
    deps = ["//tensorflow/lite/core/async:async_signature_runner"],
)
