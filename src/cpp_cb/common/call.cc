// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/call.h>

#include <cassert>

#include <grpc/grpc.h>
#include <grpc_cb/support/status.h>

#include "src/cpp_cb/common/call_operations.h"  // for CallOperations
#include "src/cpp_cb/proto/proto_utils.h"  // for DeserializeProto()

namespace grpc_cb {

int Call::default_max_message_size_ = -1;

Call::Call(grpc_call* call) :
    call_(call, grpc_call_destroy),
    recv_buf_(nullptr),
    max_message_size_(default_max_message_size_) {
  assert(call);
}

Call::~Call() {
  grpc_byte_buffer_destroy(recv_buf_);
}

Status Call::StartBatch(const ::google::protobuf::Message& request, void* tag) {
  ops_.reset(new CallOperations);
  CallOperations& ops = *ops_;

  ops.SendInitialMetadata();
  Status status = ops.SendMessage(request);
  if (!status.ok()) {
    return status;
  }
  ops.RecvInitialMetadata();
  ops.RecvMessage(&recv_buf_);
  ops.ClientSendClose();
  ops.ClientRecvStatus();

  grpc_call_error result = grpc_call_start_batch(
    call(), ops.GetOps(), ops.GetOpsNum(), tag, nullptr);
  if (GRPC_CALL_OK == result) {
    return Status::OK;
  }
  return Status::InternalError("grpc_call_start_batch() failed");
}

Status Call::GetResponse(::google::protobuf::Message* response) const {
  assert(response);
  // TODO: check status first...
  return DeserializeProto(recv_buf_, response, max_message_size_);
}

}  // namespace grpc_cb
