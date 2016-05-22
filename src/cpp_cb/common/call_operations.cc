// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/call_operations.h>

#include <grpc/support/alloc.h>         // for gpr_free()
#include <grpc_cb/impl/proto_utils.h>   // for SerializeProto()
#include <grpc_cb/support/status.h>

namespace grpc_cb {

Status CallOperations::SendMessage(
    const ::google::protobuf::Message& message,
    grpc_byte_buffer** send_buf) {
  Status status = SerializeProto(message, send_buf);
  if (!status.ok()) return status;
  if (*send_buf == nullptr) return status;
  grpc_op op = {GRPC_OP_SEND_MESSAGE, 0, 0, 0};
  op.data.send_message = *send_buf;
  cops_.push_back(op);
  return status;
}

// Todo: SendInitialMetadata()
void CallOperations::SendInitialMetadata(const grpc_metadata_array& init_metadata) {
  grpc_op op{GRPC_OP_SEND_INITIAL_METADATA, 0, 0};
  op.data.send_initial_metadata.count = 0;
  op.data.send_initial_metadata.metadata = nullptr;
  cops_.push_back(op);
}

void CallOperations::RecvInitialMetadata(grpc_metadata_array* init_metadata) {
  grpc_op op{GRPC_OP_RECV_INITIAL_METADATA, 0, 0};  // Todo
  op.data.recv_initial_metadata = init_metadata;
  cops_.push_back(op);
}

void CallOperations::RecvMessage(grpc_byte_buffer** recv_buf) {
  grpc_op op = {GRPC_OP_RECV_MESSAGE, 0, 0};
  op.data.recv_message = recv_buf;
  cops_.push_back(op);
}

void CallOperations::ClientSendClose() {
  grpc_op op{GRPC_OP_SEND_CLOSE_FROM_CLIENT, 0, 0};
  cops_.push_back(op);
}

void CallOperations::ClientRecvStatus(grpc_metadata_array* trailing_metadata,
                                      grpc_status_code* status_code,
                                      char** status_details,
                                      size_t* status_details_capacity) {
  grpc_op op = {GRPC_OP_RECV_STATUS_ON_CLIENT, 0, 0};
  op.data.recv_status_on_client.trailing_metadata = trailing_metadata;
  op.data.recv_status_on_client.status = status_code;
  op.data.recv_status_on_client.status_details = status_details;
  op.data.recv_status_on_client.status_details_capacity =
      status_details_capacity;
  cops_.push_back(op);
}

}  // namespace grpc_cb
