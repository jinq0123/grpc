// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "src/cpp_cb/common/call_operations.h"

#include <grpc/support/alloc.h>  // for gpr_free()
#include <grpc_cb/support/status.h>

#include "src/cpp_cb/proto/proto_utils.h"

namespace grpc_cb {

CallOperations::CallOperations()
    : send_buf_(nullptr) {
  grpc_metadata_array_init(&recv_initial_metadata_arr_);
  grpc_metadata_array_init(&recv_trailing_metadata_arr_);
}

CallOperations::~CallOperations() {
  grpc_byte_buffer_destroy(send_buf_);
  grpc_metadata_array_destroy(&recv_initial_metadata_arr_);
  grpc_byte_buffer_destroy(recv_buf_);
  grpc_metadata_array_destroy(&recv_trailing_metadata_arr_);
  gpr_free(status_details_);
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
  grpc_op op{GRPC_OP_RECV_INITIAL_METADATA, 0, 0};
  op.data.recv_initial_metadata = &recv_initial_metadata_arr_;
  cops_.push_back(op);
}

void CallOperations::RecvMessage() {
  grpc_op op = {GRPC_OP_RECV_MESSAGE, 0, 0};
  op.data.recv_message = &recv_buf_;
  cops_.push_back(op);
}

void CallOperations::ClientSendClose() {
  grpc_op op{GRPC_OP_SEND_CLOSE_FROM_CLIENT, 0, 0};
  cops_.push_back(op);
}

void CallOperations::ClientRecvStatus() {

  grpc_op op = {GRPC_OP_RECV_STATUS_ON_CLIENT, 0, 0};
  op.data.recv_status_on_client.trailing_metadata =
    &recv_trailing_metadata_arr_;
  op.data.recv_status_on_client.status = &status_code_;
  op.data.recv_status_on_client.status_details = &status_details_;
  op.data.recv_status_on_client.status_details_capacity =
        &status_details_capacity_;
  cops_.push_back(op);
}

}  // namespace grpc_cb
