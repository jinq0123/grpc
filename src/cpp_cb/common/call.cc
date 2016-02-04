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

Status Call::StartBatch() {
  CallOperations ops;

  Status status = ops.SendMessage(request);
  if (!status.ok()) {
    return status;
  }
  ops.SendInitialMetadata(context->send_initial_metadata_);
  ops.RecvInitialMetadata(context);
  ops.RecvMessage(result);
  ops.ClientSendClose();
  ops.ClientRecvStatus(context, &status);

  // ops->FillOps(cops, &nops);
  grpc_call_error result = grpc_call_start_batch(
    call_, cops, nops, (void*)1234, nullptr);
  if (GRPC_CALL_OK == result) {
    return Status::OK;
  }
  return Status::InternalError("grpc_call_start_batch() failed");
}

}  // namespace grpc_cb
