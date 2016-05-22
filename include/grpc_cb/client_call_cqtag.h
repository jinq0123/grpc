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
  inline Status InitCallOps(CallOperations& ops,
                            const ::google::protobuf::Message& request);

 private:
  grpc_byte_buffer* send_buf_;
  MetadataVector send_init_metadata_;
};  // class ClientCallCqTag

Status ClientCallCqTag::InitCallOps(
    CallOperations& ops, const ::google::protobuf::Message& request) {
  Status status = ops.SendMessage(request, &send_buf_);
  if (!status.ok()) {
    return status;
  }
  // Todo: Fill send_init_md_array_ -> FillMetadataVector()
  ops.SendInitialMetadata(send_init_metadata_);
  ops.RecvInitialMetadata(XXX);  // XXX need grpc_metadata_array_destroy()
  ops.RecvMessage(&recv_buf_);
  ops.ClientSendClose();
  ops.ClientRecvStatus();
  return status;
}

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CALL_CQTAG_H
