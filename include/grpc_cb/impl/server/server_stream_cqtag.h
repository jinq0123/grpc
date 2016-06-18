// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_SERVER_SERVER_STREAM_CQTAG_H
#define GRPC_CB_IMPL_SERVER_SERVER_STREAM_CQTAG_H

#include <functional>
#include <memory>  // for shared_ptr<>

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call.h>             // for StartBatch()
#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodSendInitMd
#include <grpc_cb/impl/call_operations.h>  // for CallOperations
#include <grpc_cb/support/config.h>        // for GRPC_FINAL

namespace grpc_cb {

template <class Request, class Response>
class ServerStreamCqTag GRPC_FINAL : public CallCqTag {
 public:
  // Like ServerReaderCqTag but use Writer instead of Replier.
  using Writer = ServerWriter<Response>;
  using MsgCallback = std::function<void (const Request&, const Writer&)>;
  using EndCallback = std::function<void (const Writer&)>;
  struct Callbacks {
      MsgCallback on_msg;
      EndCallback on_end;
  };
  using CallbacksSptr = std::shared_ptr<Callbacks>;

 public:
  inline ServerStreamCqTag(const CallSptr& call_sptr,
      const CallbacksSptr& cbs_sptr);
  inline bool Start() GRPC_MUST_USE_RESULT;

 public:
  inline void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  CodRecvMsg cod_recv_msg_;
  CallbacksSptr cbs_sptr_;
};  // class ServerStreamCqTag

template <class Request, class Response>
ServerStreamCqTag<Request, Response>::ServerStreamCqTag(
    const CallSptr& call_sptr, const CallbacksSptr& cbs_sptr)
    : CallCqTag(call_sptr), cbs_sptr_(cbs_sptr) {
  assert(call_sptr);
  assert(cbs_sptr);
  assert(cbs_sptr->on_msg);
  assert(cbs_sptr->on_end);
}

template <class Request, class Response>
bool ServerStreamCqTag<Request, Response>::Start() {
  CallOperations ops;
  ops.RecvMsg(cod_recv_msg_);
  return GetCallSptr()->StartBatch(ops, this);
}

template <class Request, class Response>
void ServerStreamCqTag<Request, Response>::DoComplete(bool success) {
  assert(success);
  const CallSptr& call_sptr = GetCallSptr();
  Replier replier(call_sptr);
  if (!cod_recv_msg_.HasGotMsg()) {
    cbs_sptr_->on_end(replier);
    return;
  }

  Request request;
  Status status = cod_recv_msg_.GetResultMsg(
      request, call_sptr->GetMaxMsgSize());
  if (!status.ok()) {
      replier.ReplyError(status);
      return;
  }

  // Todo: Need the same writer copy...
  cbs_sptr_->on_msg(request, replier);

  auto* tag = new ServerStreamCqTag(call_sptr, cbs_sptr_);
  if (tag->Start()) return;

  delete tag;
  replier.ReplyError(Status::InternalError("Failed to read client stream."));
}

};  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_SERVER_SERVER_STREAM_CQTAG_H
