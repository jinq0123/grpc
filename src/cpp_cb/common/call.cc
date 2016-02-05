// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/call.h>

#include <cassert>

#include <grpc/grpc.h>
#include <grpc_cb/support/status.h>

#include "src/cpp_cb/common/call_operations.h"  // for CallOperations

namespace grpc_cb {

Call::Call(grpc_call* call)
    : call_(call) {
  assert(call);
}

Call::~Call() {
  assert(call_);
  grpc_call_destroy(call_);
}

Status Call::StartBatch(const protobuf::Message& request) {
  static CallOperations ops;

  ops.SendInitialMetadata();
  Status status = ops.SendMessage(request);
  if (!status.ok()) {
    return status;
  }
  ops.RecvInitialMetadata();
  ops.RecvMessage();
  ops.ClientSendClose();
  ops.ClientRecvStatus();

  grpc_call_error result = grpc_call_start_batch(
    call_, ops.GetOps(), ops.GetOpsNum(), (void*)1234, nullptr);
  if (GRPC_CALL_OK == result) {
    return Status::OK;
  }
  return Status::InternalError("grpc_call_start_batch() failed");
}

}  // namespace grpc_cb
