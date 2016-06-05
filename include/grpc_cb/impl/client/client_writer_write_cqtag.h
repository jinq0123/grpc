// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_WRITER_WRITE_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_WRITER_WRITE_CQTAG_H

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call.h>             // for StartBatch()
#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodRecvMsg
#include <grpc_cb/impl/call_operations.h>  // for CallOperations
#include <grpc_cb/support/config.h>        // for GRPC_FINAL

namespace grpc_cb {

class ClientWriterWriteCqTag GRPC_FINAL : public CallCqTag {
 public:
  inline explicit ClientWriterWriteCqTag(const CallSptr& call_sptr)
      : CallCqTag(call_sptr) {}
  inline Status Start(const ::google::protobuf::Message& message) GRPC_MUST_USE_RESULT;

 private:
  CodSendMsg cod_send_msg_;
};  // class ClientWriterWriteCqTag

Status ClientWriterWriteCqTag::Start(
    const ::google::protobuf::Message& message) {
  CallOperations ops;
  ops.SendMsg(cod_recv_msg_);
  ops.ClientRecvStatus(cod_client_recv_status_);
  return GetCallSptr()->StartBatch(ops, this);
}

};  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_WRITER_WRITE_CQTAG_H
