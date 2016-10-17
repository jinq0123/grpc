// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef CPP_CB_SERVER_SERVER_REPLIER_CQTAT_H
#define CPP_CB_SERVER_SERVER_REPLIER_CQTAT_H

#include <grpc_cb/impl/call.h>             // for Call
#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodSendInitMd
#include <grpc_cb/impl/call_operations.h>  // for CallOperations
#include <grpc_cb/impl/call_sptr.h>        // for CallSptr
#include <grpc_cb/support/config.h>        // for GRPC_FINAL
#include <grpc_cb/support/protobuf_fwd.h>  // for Message

namespace grpc_cb {

class ServerReplierCqTag GRPC_FINAL : public CallCqTag {
 public:
  ServerReplierCqTag(const CallSptr& call_sptr, bool send_init_md)
    : CallCqTag(call_sptr), send_init_md_(send_init_md) {}

  inline bool StartReply(const ::google::protobuf::Message& msg) GRPC_MUST_USE_RESULT;
  inline bool StartReplyError(const Status& status) GRPC_MUST_USE_RESULT;

 private:
  CodSendInitMd cod_send_init_md_;
  CodSendMsg cod_send_msg_;
  CodServerSendStatus cod_server_send_status_;
  bool send_init_md_ = false;  // need to send initial metadata
};

bool ServerReplierCqTag::StartReply(const ::google::protobuf::Message& msg) {
  CallOperations ops;
  if (send_init_md_) {  // Todo: use CodSendInitMd uptr?
    ops.SendInitMd(cod_send_init_md_);  // Todo: init metadata
  }
  Status status = ops.SendMsg(msg, cod_send_msg_);
  ops.ServerSendStatus(status, cod_server_send_status_);
  return GetCallSptr()->StartBatch(ops, this);
}

bool ServerReplierCqTag::StartReplyError(const Status& status) {
  CallOperations ops;
  if (send_init_md_) {
    ops.SendInitMd(cod_send_init_md_);
  }
  ops.ServerSendStatus(status, cod_server_send_status_);
  return GetCallSptr()->StartBatch(ops, this);
}

}  // namespace grpc_cb
#endif  // CPP_CB_SERVER_SERVER_REPLIER_CQTAT_H
