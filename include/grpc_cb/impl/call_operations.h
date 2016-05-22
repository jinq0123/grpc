// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_INTERNAL_CPP_CB_COMMON_CALL_OPERATIONS_H
#define GRPC_INTERNAL_CPP_CB_COMMON_CALL_OPERATIONS_H

#include <map>  // for multimap
#include <vector>

#include <grpc/grpc.h>                     // for grpc_op
#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT
#include <grpc_cb/support/config.h>        // for GRPC_FINAL
#include <grpc_cb/support/protobuf_fwd.h>  // for Message
#include <grpc_cb/support/status.h>        // for Status

namespace grpc_cb {

class Status;

// Like grpc++ CallOpSet<>.
// Non-thread-safe.
class CallOperations GRPC_FINAL {
 public:
  inline size_t GetOpsNum() const { return cops_.size(); }
  inline const grpc_op* GetOps() const {
    return cops_.empty() ? nullptr : &cops_[0];
  }

 public:
  void SendInitialMetadata(const grpc_metadata_array& init_metadata);
  Status SendMessage(const ::google::protobuf::Message& message,
                     grpc_byte_buffer** send_buf) GRPC_MUST_USE_RESULT;
  void RecvInitialMetadata(grpc_metadata_array* init_metadata = nullptr);
  void RecvMessage(grpc_byte_buffer** recv_buf);

  void ClientSendClose();
  void ClientRecvStatus(grpc_metadata_array* trailing_metadata,
                        grpc_status_code* status_code, char** status_details,
                        size_t* status_details_capacity);

  void ServerSendStatus(const grpc_metadata_array& trailing_metadata,
                        const Status& status);

 private:
  std::vector<grpc_op> cops_;

 private:
     // Todo: Delete these member, use outside variables instead. (In CompleteTag?)
  // std::vector<grpc_metadata> initial_metadata_;
  //grpc_byte_buffer* send_buf_;
  //grpc_metadata_array recv_initial_metadata_arr_;
  //grpc_metadata_array recv_trailing_metadata_arr_;
  //grpc_status_code status_code_;
  //char* status_details_;
  //size_t status_details_capacity_;
};

}  // namespace grpb_cb

#endif  // GRPC_INTERNAL_CPP_CB_COMMON_CALL_OPERATIONS_H
