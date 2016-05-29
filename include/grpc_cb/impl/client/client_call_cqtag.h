// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CALL_CQTAG_H
#define GRPC_CB_CLIENT_CALL_CQTAG_H

#include <grpc/support/port_platform.h>  // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call.h>             // for GetMaxMessageSize()
#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodSendInitMd
#include <grpc_cb/impl/metadata_vector.h>  // for MetadataVector
#include <grpc_cb/support/protobuf_fwd.h>  // for Message

namespace grpc_cb {

class CallOperations;
class Status;

// Completion queue tag (CqTag) for client call for both blocking and async calls.
// ClientAsyncCallCqTag derives from it.
class ClientCallCqTag : public CallCqTag {
 public:
  explicit ClientCallCqTag(const CallSptr& call_sptr) : CallCqTag(call_sptr) {
    assert(call_sptr);
  }
  virtual ~ClientCallCqTag() {}

 public:
  Status InitCallOps(const ::google::protobuf::Message& request,
                     CallOperations& ops) GRPC_MUST_USE_RESULT;

public:
  Status GetResponse(::google::protobuf::Message& message) {
    // Todo: Get trailing metadata.
    if (!cod_client_recv_status_.IsStatusOk())
      return cod_client_recv_status_.GetStatus();
    return cod_recv_message_.GetResponse(message,
                                         GetCallSptr()->GetMaxMessageSize());
  }

 private:
  CodSendInitMd cod_send_init_md_;  // Todo: set init metadata
  CodSendMessage cod_send_message_;
  CodRecvInitMd cod_recv_init_md_;
  CodRecvMessage cod_recv_message_;
  CodClientRecvStatus cod_client_recv_status_;
};  // class ClientCallCqTag

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CALL_CQTAG_H
