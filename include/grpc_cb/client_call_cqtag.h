// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CALL_CQTAG_H
#define GRPC_CB_CLIENT_CALL_CQTAG_H

#include <grpc_cb/impl/call_operations.h>  // for CallOperations
#include <grpc_cb/completion_queue_tag.h>

namespace grpc_cb {

// Completion queue tag (CqTag) for client call. For both blocking and async calls.
// Todo: Async call should derive from it. Need cb, err_cb.
class ClientCallCqTag : public CompletionQueueTag {
public:
  ClientCallCqTag() {
    grpc_metadata_array_init(&recv_init_detadata_);
    grpc_metadata_array_init(&recv_trailing_metadata_arr_);
  }
  virtual ~ClientCallCqTag() {
    grpc_metadata_array_destroy(&recv_init_detadata_);
    grpc_metadata_array_destroy(&recv_trailing_metadata_arr_);
    grpc_byte_buffer_destroy(recv_buf_);
  }

public:
  inline Status InitCallOps(CallOperations& ops,
                           const ::google::protobuf::Message& request);

private:
  grpc_byte_buffer* send_buf_ = nullptr;
  MetadataVector send_init_metadata_;
  grpc_metadata_array recv_init_detadata_;
  grpc_byte_buffer* recv_buf_ = nullptr;

  grpc_metadata_array recv_trailing_metadata_arr_;
  grpc_status_code status_code_ = GRPC_STATUS_OK;
  char* status_details_ = nullptr;
  size_t status_details_capacity_ = 0;
};  // class ClientCallCqTag

Status ClientCallCqTag::InitCallOps(
    CallOperations& ops, const ::google::protobuf::Message& request) {
  Status status = ops.SendMessage(request, &send_buf_);
  if (!status.ok()) {
    return status;
  }
  // Todo: Fill send_init_md_array_ -> FillMetadataVector()
  ops.SendInitialMetadata(send_init_metadata_);
  ops.RecvInitialMetadata(&recv_init_detadata_);
  ops.RecvMessage(&recv_buf_);
  ops.ClientSendClose();
  ops.ClientRecvStatus(&recv_trailing_metadata_arr_,
      &status_code_, &status_details_, &status_details_capacity_);
  return status;
}

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CALL_CQTAG_H
