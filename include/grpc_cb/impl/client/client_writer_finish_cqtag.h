// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_WRITER_FINISH_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_WRITER_FINISH_CQTAG_H

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call.h>             // for StartBatch()
#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodRecvMessage
#include <grpc_cb/impl/call_operations.h>  // for CallOperations
#include <grpc_cb/support/config.h>        // for GRPC_FINAL

namespace grpc_cb {

class ClientWriterFinishCqTag GRPC_FINAL : public CallCqTag {
 public:
  inline explicit ClientWriterFinishCqTag(const CallSptr& call_sptr)
      : CallCqTag(call_sptr) {}
  inline Status Start() GRPC_MUST_USE_RESULT;
  inline bool IsStatusOk() const {
    return cod_client_recv_status_.IsStatusOk();
  }
  inline Status GetStatus() const {
    return cod_client_recv_status_.GetStatus();
  }
  inline Status GetResponse(::google::protobuf::Message& response) GRPC_MUST_USE_RESULT;

  inline void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  CodRecvInitMd cod_recv_init_md_;
  CodRecvMessage cod_recv_message_;
  CodClientRecvStatus cod_client_recv_status_;
};  // class ClientWriterFinishCqTag

Status ClientWriterFinishCqTag::Start() {
  CallOperations ops;
  ops.ClientSendClose();
  ops.RecvInitMd(cod_recv_init_md_);
  ops.RecvMessage(cod_recv_message_);
  ops.ClientRecvStatus(cod_client_recv_status_);
  return GetCallSptr()->StartBatch(ops, this);
}

Status ClientWriterFinishCqTag::GetResponse(
    ::google::protobuf::Message& response) {
  return cod_recv_message_.GetResultMessage(
        response, GetCallSptr()->GetMaxMessageSize());
}

void ClientWriterFinishCqTag::DoComplete(bool success) {
  // Todo: Add async client writer example.
}

};  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_WRITER_FINISH_CQTAG_H
