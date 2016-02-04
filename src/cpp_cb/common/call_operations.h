// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_INTERNAL_CPP_CB_COMMON_CALL_OPERATIONS_H
#define GRPC_INTERNAL_CPP_CB_COMMON_CALL_OPERATIONS_H

#include <vector>

#include <grpc_cb/support/config.h>  // for GRPC_FINAL
#include <grpc/grpc.h>  // for grpc_op

namespace grpc_cb {

// Non-thread-safe.
class CallOperations GRPC_FINAL {
 public:
  CallOperations();
  ~CallOperations();

 public:
  inline size_t GetOpsNum() const { return cops_.size(); }
  inline const grpc_op* GetOps() const {
    return cops_.empty() ? nullptr : &cops_[0];
  }

 private:
  std::vector<grpc_op> cops_;
};

}  // namespace grpb_cb

#endif  // GRPC_INTERNAL_CPP_CB_COMMON_CALL_OPERATIONS_H
