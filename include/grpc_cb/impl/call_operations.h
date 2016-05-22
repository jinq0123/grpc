// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_INTERNAL_CPP_CB_COMMON_CALL_OPERATIONS_H
#define GRPC_INTERNAL_CPP_CB_COMMON_CALL_OPERATIONS_H

#include <cassert>

#include <grpc/grpc.h>                     // for grpc_op
#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT
#include <grpc_cb/impl/proto_utils.h>      // for SerializeProto()
#include <grpc_cb/support/config.h>        // for GRPC_FINAL
#include <grpc_cb/support/protobuf_fwd.h>  // for Message
#include <grpc_cb/support/status.h>        // for Status

namespace grpc_cb {

class Status;

// Like grpc++ CallOpSet<>.
// Non-thread-safe.
class CallOperations GRPC_FINAL {
 public:
  inline size_t GetOpsNum() const {
    assert(nops_ <= MAX_OPS);
    return nops_;
  }
  inline const grpc_op* GetOps() const { return ops_; }

 public:
  inline void SendInitialMetadata(const grpc_metadata_array& init_metadata);
  inline Status SendMessage(const ::google::protobuf::Message& message,
                     grpc_byte_buffer** send_buf) GRPC_MUST_USE_RESULT;
  inline void RecvInitialMetadata(grpc_metadata_array* init_metadata = nullptr);
  inline void RecvMessage(grpc_byte_buffer** recv_buf);

  inline void ClientSendClose();
  inline void ClientRecvStatus(grpc_metadata_array* trailing_metadata,
                        grpc_status_code* status_code, char** status_details,
                        size_t* status_details_capacity);

  inline void ServerSendStatus(
      const grpc_metadata_array& trailing_metadata_array,
      const grpc_status_code& status_code, const char* status_details);

 private:
  static const size_t MAX_OPS = 8;

  size_t nops_ = 0;
  grpc_op ops_[MAX_OPS];
};  // class CallOperations

static inline void InitOp(grpc_op& op, grpc_op_type type, uint32_t flags = 0) {
    op.op = type;
    op.flags = flags;
    op.reserved = nullptr;
}

Status CallOperations::SendMessage(
    const ::google::protobuf::Message& message,
    grpc_byte_buffer** send_buf) {
  Status status = SerializeProto(message, send_buf);
  if (!status.ok()) return status;
  if (*send_buf == nullptr) return status;

  assert(nops_ < MAX_OPS);
  grpc_op& op = ops_[nops_++];
  InitOp(op, GRPC_OP_SEND_MESSAGE);
  op.data.send_message = *send_buf;
  return status;
}

// Todo: SendInitialMetadata()
void CallOperations::SendInitialMetadata(const grpc_metadata_array& init_metadata) {
  assert(nops_ < MAX_OPS);
  grpc_op& op = ops_[nops_++];
  InitOp(op, GRPC_OP_SEND_INITIAL_METADATA);
  op.data.send_initial_metadata.count = 0;
  op.data.send_initial_metadata.metadata = nullptr;
}

void CallOperations::RecvInitialMetadata(grpc_metadata_array* init_metadata) {
  assert(nops_ < MAX_OPS);
  grpc_op& op = ops_[nops_++];
  InitOp(op, GRPC_OP_RECV_INITIAL_METADATA);  // Todo
  op.data.recv_initial_metadata = init_metadata;
}

void CallOperations::RecvMessage(grpc_byte_buffer** recv_buf) {
  assert(nops_ < MAX_OPS);
  grpc_op& op = ops_[nops_++];
  InitOp(op, GRPC_OP_RECV_MESSAGE);
  op.data.recv_message = recv_buf;
}

void CallOperations::ClientSendClose() {
  assert(nops_ < MAX_OPS);
  grpc_op& op = ops_[nops_++];
  InitOp(op, GRPC_OP_SEND_CLOSE_FROM_CLIENT);
}

void CallOperations::ClientRecvStatus(grpc_metadata_array* trailing_metadata,
                                      grpc_status_code* status_code,
                                      char** status_details,
                                      size_t* status_details_capacity) {
  assert(nops_ < MAX_OPS);
  grpc_op& op = ops_[nops_++];
  InitOp(op, GRPC_OP_RECV_STATUS_ON_CLIENT);
  op.data.recv_status_on_client.trailing_metadata = trailing_metadata;
  op.data.recv_status_on_client.status = status_code;
  op.data.recv_status_on_client.status_details = status_details;
  op.data.recv_status_on_client.status_details_capacity =
      status_details_capacity;
}

void CallOperations::ServerSendStatus(
    const grpc_metadata_array& trailing_metadata_array,
                        const grpc_status_code& status_code,
                        const char* status_details) {
  assert(nops_ < MAX_OPS);
  grpc_op& op = ops_[nops_++];
  InitOp(op, GRPC_OP_SEND_STATUS_FROM_SERVER);
  op.data.send_status_from_server.trailing_metadata_count =
      trailing_metadata_array.count;
  op.data.send_status_from_server.trailing_metadata =
      trailing_metadata_array.metadata;
  op.data.send_status_from_server.status = status_code;
  op.data.send_status_from_server.status_details = status_details;
}

}  // namespace grpb_cb

#endif  // GRPC_INTERNAL_CPP_CB_COMMON_CALL_OPERATIONS_H
