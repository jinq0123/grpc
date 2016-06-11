// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_SERVER_SERVER_READER_INIT_CQTAG_H
#define GRPC_CB_IMPL_SERVER_SERVER_READER_INIT_CQTAG_H

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call.h>             // for StartBatch()
#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodSendInitMd
#include <grpc_cb/impl/call_operations.h>  // for CallOperations
#include <grpc_cb/support/config.h>        // for GRPC_FINAL

namespace grpc_cb {

template <class Request, class Response>
class ServerReaderInitCqTag GRPC_FINAL : public CallCqTag {
 public:
  using Replier = ServerReplier<Response>;
  using MsgCallback = std::function<void (const Request&, const Replier&)>;
  using EndCallback = std::function<void (const Replier&)>;

 public:
  inline ServerReaderInitCqTag(const CallSptr& call_sptr,
      const MsgCallback& on_msg, const EndCallback& on_end);
  inline Status Start() GRPC_MUST_USE_RESULT;

 public:
  inline void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  CodSendInitMd cod_send_init_md_;
  // Already got init_md and request, so no CodRecvInitMd.
  MsgCallback on_msg_;
  EndCallback on_end_;
};  // class ServerReaderInitCqTag

template <class Request, class Response>
ServerReaderInitCqTag<Request, Response>::ServerReaderInitCqTag(
    const CallSptr& call_sptr,
    const MsgCallback& on_msg,
    const EndCallback& on_end)
    : CallCqTag(call_sptr), on_msg_(on_msg), on_end_(on_end) {
  assert(call_sptr);
  assert(on_msg);
  assert(on_end);
}

template <class Request, class Response>
Status ServerReaderInitCqTag<Request, Response>::Start() {
  CallOperations ops;
  // Todo: Fill send_init_md_array_ -> FillMetadataVector()
  ops.SendInitMd(cod_send_init_md_);
  return GetCallSptr()->StartBatch(ops, this);
}

template <class Request, class Response>
void ServerReaderInitCqTag<Request, Response>::DoComplete(bool success) {
  assert(success);
  const CallSptr& call_sptr = GetCallSptr();
  auto* tag = new ServerReaderCqTag(call_sptr, on_msg_, on_end_);
  ::grpc_cb::Status status = tag->Start();
  if (status.ok()) return;
  delete tag;
  Replier(call_sptr).ReplyError(status);
}

};  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_SERVER_SERVER_READER_INIT_CQTAG_H
