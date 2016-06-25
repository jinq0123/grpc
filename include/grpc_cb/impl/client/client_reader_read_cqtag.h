// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_READ_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_READER_READ_CQTAG_H

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call_cqtag.h>  // for CallCqTag
#include <grpc_cb/support/config.h>   // for GRPC_FINAL
#include <grpc_cb/impl/call_op_data.h>  // for CodRecvMsg
#include <grpc_cb/status_callback.h>  // for ErrorCallback

namespace grpc_cb {

// TODO: rename to ClientRecvMsgCqTag, and ClientAsyncRecvMsgCqTag subclass.
template <class Response>
class ClientReaderReadCqTag GRPC_FINAL : public CallCqTag {
 public:
  using MsgCallback = std::function<void (const Response&)>;
  inline explicit ClientReaderReadCqTag(const CallSptr& call_sptr,
                                        const MsgCallback& cb = MsgCallback(),
                                        const ErrorCallback& ecb = ErrorCallback())
      : CallCqTag(call_sptr), cb_(cb), ecb_(ecb) {}
  inline bool Start() GRPC_MUST_USE_RESULT;
  inline Status GetResultMsg(Response& message) GRPC_MUST_USE_RESULT {
    return cod_recv_msg_.GetResultMsg(
        message, GetCallSptr()->GetMaxMsgSize());
  }
  inline void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  CodRecvMsg cod_recv_msg_;
  // Callback will be triggered on completion in DoComplete().
  MsgCallback cb_;
  ErrorCallback ecb_;
};  // class ClientReaderReadCqTag

template <class Response>
bool ClientReaderReadCqTag<Response>::Start() {
  CallOperations ops;
  ops.RecvMsg(cod_recv_msg_);
  return GetCallSptr()->StartBatch(ops, this);
}

template <class Response>
void ClientReaderReadCqTag<Response>::DoComplete(bool success) {
  assert(success);

  Response resp;
  Status status = GetResultMsg(resp);
  if (status.ok()) {
    if (cb_) cb_(resp);
    return;
  }

  if (ecb_) ecb_(status);
}

};  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_READ_CQTAG_H
