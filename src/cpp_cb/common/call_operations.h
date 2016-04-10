// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_INTERNAL_CPP_CB_COMMON_CALL_OPERATIONS_H
#define GRPC_INTERNAL_CPP_CB_COMMON_CALL_OPERATIONS_H

#include <vector>

#include <grpc/grpc.h>  // for grpc_op
#include <grpc/support/port_platform.h>  // for GRPC_MUST_USE_RESULT
#include <grpc_cb/support/config.h>  // for GRPC_FINAL
#include <grpc_cb/support/protobuf_fwd.h>  // for Message

namespace grpc_cb {

class Status;

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

 public:
  Status SendMessage(const ::google::protobuf::Message& message) GRPC_MUST_USE_RESULT;
  void SendInitialMetadata();
  void RecvInitialMetadata();
  void RecvMessage(grpc_byte_buffer** recv_buf);
  void ClientSendClose();
  void ClientRecvStatus();

 private:
  std::vector<grpc_op> cops_;

 private:
  // std::vector<grpc_metadata> initial_metadata_;
  grpc_byte_buffer* send_buf_;
  grpc_metadata_array recv_initial_metadata_arr_;
  grpc_metadata_array recv_trailing_metadata_arr_;
  grpc_status_code status_code_;
  char* status_details_;
  size_t status_details_capacity_;
};

}  // namespace grpb_cb

#endif  // GRPC_INTERNAL_CPP_CB_COMMON_CALL_OPERATIONS_H
