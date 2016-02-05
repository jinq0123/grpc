// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "src/cpp_cb/common/call_operations.h"

#include <grpc_cb/support/status.h>

#include "src/cpp_cb/proto/proto_utils.h"

namespace grpc_cb {

CallOperations::CallOperations()
    : send_buf_(nullptr) {
}

CallOperations::~CallOperations() {
  grpc_byte_buffer_destroy(send_buf_);
}

Status CallOperations::SendMessage(
    const protobuf::Message& message) {
  Status status = SerializeProto(message, &send_buf_);
  if (!status.ok()) return status;
  if (send_buf_ == nullptr) return status;
  grpc_op op = {GRPC_OP_SEND_MESSAGE, 0, 0, 0};
  op.data.send_message = send_buf_;
  cops_.push_back(op);
  return status;
}

void CallOperations::SendInitialMetadata() {
  grpc_op op{GRPC_OP_SEND_INITIAL_METADATA, 0, 0};
  op.data.send_initial_metadata.count = 0;
  op.data.send_initial_metadata.metadata = nullptr;
  cops_.push_back(op);
}

void CallOperations::RecvInitialMetadata() {
}

void CallOperations::RecvMessage(grpc_byte_buffer** recv_buf) {
  grpc_op op = {GRPC_OP_RECV_MESSAGE, 0, 0};
  op.data.recv_message = recv_buf;
  cops_.push_back(op);
}

void CallOperations::ClientSendClose() {
}

void CallOperations::ClientRecvStatus(Status* status) {

  grpc_op op = {GRPC_OP_RECV_STATUS_ON_CLIENT, 0, 0};
  op.data.recv_status_on_client.trailing_metadata = nullptr;
  op.data.recv_status_on_client.status = nullptr;
  op.data.recv_status_on_client.status_details = nullptr;
  op.data.recv_status_on_client.status_details_capacity = nullptr;
  cops_.push_back(op);
}

}  // namespace grpc_cb
