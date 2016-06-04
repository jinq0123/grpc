// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/call.h>

#include <cassert>

#include <grpc/grpc.h>
#include <grpc_cb/impl/call_operations.h>  // for CallOperations
#include <grpc_cb/impl/proto_utils.h>      // for DeserializeProto()
#include <grpc_cb/status.h>                // for Status

namespace grpc_cb {

int Call::default_max_message_size_ = -1;

// Owns c_call.
Call::Call(grpc_call* c_call) :
    c_call_uptr_(c_call, grpc_call_destroy),
    max_message_size_(default_max_message_size_) {
  assert(c_call);
}

Call::~Call() {
}

Status Call::StartBatch(const CallOperations& ops, void* tag) {
  grpc_call_error result = grpc_call_start_batch(
    c_call(), ops.GetOps(), ops.GetOpsNum(), tag, nullptr);
  if (GRPC_CALL_OK == result) {
    return Status::OK;
  }
  return Status::InternalError("grpc_call_start_batch() failed");
}

// XXX: Move out. Call is only wrap of grpc_call.
//Status Call::GetResponse(::google::protobuf::Message* response) const {
//  assert(response);
//  // TODO: check status first...
//  return DeserializeProto(recv_buf_, response, max_message_size_);
//}

// Todo: Inline these.

}  // namespace grpc_cb
