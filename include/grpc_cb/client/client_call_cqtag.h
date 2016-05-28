// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CALL_CQTAG_H
#define GRPC_CB_CLIENT_CALL_CQTAG_H

#include <grpc/support/port_platform.h>  // for GRPC_MUST_USE_RESULT

#include <grpc_cb/completion_queue_tag.h>  // for CompletionQueueTag
#include <grpc_cb/impl/metadata_vector.h>  // for MetadataVector
#include <grpc_cb/support/protobuf_fwd.h>  // for Message

namespace grpc_cb {

class CallOperations;
class Status;

// Completion queue tag (CqTag) for client call for both blocking and async calls.
// ClientAsyncCallCqTag derives from it.
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

    // XXX DoComplete()

 public:
  Status InitCallOps(const ::google::protobuf::Message& request,
                     CallOperations& ops) GRPC_MUST_USE_RESULT;

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

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CALL_CQTAG_H
