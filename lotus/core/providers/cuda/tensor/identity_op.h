#pragma once
#include "core/common/common.h"
#include "core/framework/op_kernel.h"
#include "core/providers/cuda/cuda_common.h"
#include "core/providers/cpu/tensor/concat.h"

namespace Lotus {
namespace Cuda {

template <bool is_dropout>
class IdentityOp final : public CudaKernel {
 public:
  IdentityOp(const OpKernelInfo& info) : CudaKernel(info) {
  }

  Status ComputeInternal(OpKernelContext* context) const override {
    const Tensor* X = context->Input<Tensor>(0);
    const TensorShape& shape = X->Shape();
    Tensor* Y = context->Output(0, shape);
    auto X_type = X->DataType();

    const void* source = X->DataRaw(X_type);
    void* target = Y->MutableDataRaw(X_type);
    //If source and target pointers are not equal, we need to copy the data.
    if (target != source) {
      CUDA_RETURN_IF_ERROR(cudaMemcpyAsync(target, source, X->Shape().Size() * X->DataType()->Size(), cudaMemcpyDeviceToDevice));
    }

    if (is_dropout) {
      context->Output(1, std::vector<int64_t>());
    }

    return Status::OK();
  }
};

}  // namespace Cuda
}  //namespace Lotus
