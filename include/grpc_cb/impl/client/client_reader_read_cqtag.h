// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_READ_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_READER_READ_CQTAG_H

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call_cqtag.h>  // for CallCqTag
#include <grpc_cb/support/config.h>   // for GRPC_FINAL
#include <grpc_cb/impl/call_op_data.h>  // for CodRecvMsg

namespace grpc_cb {

    // TODO: rename to ClientRecvMsgCqTag, and ClientAsyncRecvMsgCqTag subclass.
class ClientReaderReadCqTag GRPC_FINAL : public CallCqTag {
 public:
  // Todo: Use MsgCb and ErrCb...
  using Callback = std::function<void (ClientReaderReadCqTag&)>;
  inline explicit ClientReaderReadCqTag(const CallSptr& call_sptr,
                                        const Callback& cb = Callback())
      : CallCqTag(call_sptr), cb_(cb) {}
  inline bool Start() GRPC_MUST_USE_RESULT;
  inline Status GetResultMsg(::google::protobuf::Message& message)
      GRPC_MUST_USE_RESULT {
    return cod_recv_msg_.GetResultMsg(
        message, GetCallSptr()->GetMaxMsgSize());
  }
  inline void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  CodRecvMsg cod_recv_msg_;
  // Callback will be triggered on completion in DoComplet().
  Callback cb_;
};  // class ClientReaderReadCqTag

bool ClientReaderReadCqTag::Start() {
  CallOperations ops;
  ops.RecvMsg(cod_recv_msg_);
  return GetCallSptr()->StartBatch(ops, this);
}

void ClientReaderReadCqTag::DoComplete(bool success) {
  assert(success);
  if (cb_) cb_(*this);
}

};  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_READ_CQTAG_H
