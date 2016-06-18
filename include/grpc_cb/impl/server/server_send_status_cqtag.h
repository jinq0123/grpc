// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef CPP_CB_SERVER_SERVER_SEND_STATUS_CQTAT_H
#define CPP_CB_SERVER_SERVER_SEND_STATUS_CQTAT_H

#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodServerSendStatus
#include <grpc_cb/impl/call_operations.h>  // for CallOperations
#include <grpc_cb/impl/call_sptr.h>        // for CallSptr
#include <grpc_cb/support/config.h>        // for GRPC_FINAL

namespace grpc_cb {

class ServerSendStatusCqTag GRPC_FINAL : public CallCqTag {
 public:
  explicit ServerSendStatusCqTag(const CallSptr& call_sptr)
      : CallCqTag(call_sptr) {}

  inline bool StartSend(const Status& status) GRPC_MUST_USE_RESULT;

 private:
  CodServerSendStatus cod_server_send_status_;
};

bool ServerSendStatusCqTag::StartSend(const Status& status) {
  CallOperations ops;
  ops.ServerSendStatus(status, cod_server_send_status_);
  return GetCallSptr()->StartBatch(ops, this);
}

}  // namespace grpc_cb
#endif  // CPP_CB_SERVER_SERVER_SEND_STATUS_CQTAT_H
