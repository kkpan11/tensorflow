/* Copyright 2025 The OpenXLA Authors.

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

#ifndef XLA_BACKENDS_AUTOTUNER_BACKENDS_GPU_GPU_CODEGEN_BACKEND_H_
#define XLA_BACKENDS_AUTOTUNER_BACKENDS_GPU_GPU_CODEGEN_BACKEND_H_

#include <memory>
#include <optional>
#include <string>
#include <utility>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "xla/backends/autotuner/codegen_backend.h"
#include "xla/hlo/ir/hlo_instruction.h"
#include "xla/hlo/ir/hlo_module.h"
#include "xla/service/compiler.h"
#include "xla/service/executable.h"
#include "xla/tsl/platform/statusor.h"
#include "xla/xla.pb.h"

namespace xla {
namespace  gpu {

// Abstract base class for GPU backends, implementing the Backend interface.
class GpuCodegenBackend : public CodegenBackend {
 public:
  // target_config, debug_options and compiler should outlive the backend.
  GpuCodegenBackend(absl::string_view name,
             const Compiler::TargetConfig* target_config,
             const DebugOptions* debug_options, Compiler* compiler)
      : name_(name),
        target_config_(*target_config),
        debug_options_(*debug_options),
        compiler_(compiler) {}

  absl::string_view name() const override { return name_; }

  const Compiler::TargetConfig& target_config() const { return target_config_; }
  const DebugOptions& debug_options() const { return debug_options_; }

  absl::StatusOr<std::unique_ptr<Executable>> Compile(
      const HloInstruction& hlo_instruction,
      const BackendConfig& config) override {
    TF_ASSIGN_OR_RETURN(auto hlo_module, WrapInModule(hlo_instruction, config));
    hlo_module->mutable_config().set_debug_options(debug_options_);

    Compiler::CompileOptions options;
    options.target_config = target_config_;

    TF_ASSIGN_OR_RETURN(auto optimized_module,
                        RunHloPasses(std::move(hlo_module), options));
    return compiler_->RunBackend(std::move(optimized_module),
                                 /*executor=*/nullptr, options);
  }

  absl::Status ApplyConfig(HloInstruction& instr,
                           const BackendConfig& config) override {
    return absl::UnimplementedError("Not implemented.");
  }

 private:
  // TODO(b/407494653): Provide a default implementation.
  virtual absl::StatusOr<std::unique_ptr<HloModule>> WrapInModule(
      const HloInstruction& hlo_instruction, const BackendConfig& config) = 0;

  // Optimize the HLO module.
  // TODO(b/407494653): Remove this when XLA pipelines is fixed and we autotune
  // only optimized and fused HLOs.
  virtual absl::StatusOr<std::unique_ptr<HloModule>> RunHloPasses(
      std::unique_ptr<HloModule> hlo_module,
      const Compiler::CompileOptions& options) = 0;

  std::string name_;
  const Compiler::TargetConfig& target_config_;
  const DebugOptions& debug_options_;
  // TODO(b/407494653): remove compiler when we don't need to run any HLO passes
  // and the codegen backend can directly produce an executable without a
  // compiler instance.
  Compiler* compiler_;
};

}  // namespace gpu
}  // namespace xla

#endif  // XLA_BACKENDS_AUTOTUNER_BACKENDS_GPU_GPU_CODEGEN_BACKEND_H_
