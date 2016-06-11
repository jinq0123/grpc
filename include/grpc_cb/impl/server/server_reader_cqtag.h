// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_SERVER_SERVER_READER_CQTAG_H
#define GRPC_CB_IMPL_SERVER_SERVER_READER_CQTAG_H

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call.h>             // for StartBatch()
#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodSendInitMd
#include <grpc_cb/impl/call_operations.h>  // for CallOperations
#include <grpc_cb/support/config.h>        // for GRPC_FINAL

namespace grpc_cb {

template <class Request, class Response>
class ServerReaderCqTag GRPC_FINAL : public CallCqTag {
 public:
  using Replier = ServerReplier<Response>;
  using MsgCallback = std::function<void (const Request&, const Replier&)>;
  using EndCallback = std::function<void (const Replier&)>;
  struct Callbacks {
      MsgCallback on_msg;
      EndCallback on_end;
  };
  using CallbacksSptr = std::shared_ptr<Callbacks>;

 public:
  inline ServerReaderCqTag(const CallSptr& call_sptr,
      const CallbacksSptr& cbs_sptr);
  inline Status Start() GRPC_MUST_USE_RESULT;

 public:
  inline void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  CodRecvMsg cod_recv_msg_;
  CallbacksSptr cbs_sptr_;
};  // class ServerReaderCqTag

template <class Request, class Response>
ServerReaderCqTag<Request, Response>::ServerReaderCqTag(
    const CallSptr& call_sptr, const CallbacksSptr& cbs_sptr)
    : CallCqTag(call_sptr), cbs_sptr_(cbs_sptr) {
  assert(call_sptr);
  assert(cbs_sptr);
  assert(cbs_sptr->on_msg);
  assert(cbs_sptr->on_end);
}

template <class Request, class Response>
Status ServerReaderCqTag<Request, Response>::Start() {
  CallOperations ops;
  ops.RecvMsg(cod_recv_msg_);
  return GetCallSptr()->StartBatch(ops, this);
}

template <class Request, class Response>
void ServerReaderCqTag<Request, Response>::DoComplete(bool success) {
  assert(success);
  const CallSptr& call_sptr = GetCallSptr();
  Replier replier(call_sptr);
  if (!cod_recv_msg_.HasGotMsg()) {
    cbs_sptr->on_end(replier);
    return;
  }

  Response response;
  Status status = cod_recv_msg_.GetResultMsg(
      response, call_sptr->GetMaxMsgSize());
  if (!status.ok()) {
      replier.ReplyError(status);  // Todo: reply only once
      return;
  }

  cbs_sptr->on_msg(replier);

  auto* tag = new ServerReaderCqTag(call_sptr, cbs_sptr_);
  status = tag->Start();
  if (status.ok()) return;
  delete tag;
  replier.ReplyError(status);
}

};  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_SERVER_SERVER_READER_CQTAG_H
