// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_SERVER_SERVER_READER_WRITER_INIT_CQTAG_H
#define GRPC_CB_IMPL_SERVER_SERVER_READER_WRITER_INIT_CQTAG_H

#include <memory>  // for shared_ptr<>

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call.h>                        // for StartBatch()
#include <grpc_cb/impl/call_cqtag.h>                  // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>                // for CodSendInitMd
#include <grpc_cb/impl/call_operations.h>             // for CallOperations
#include <grpc_cb/impl/server/server_reader_writer_cqtag.h>  // for ServerReaderWriterCqTag
#include <grpc_cb/support/config.h>                   // for GRPC_FINAL

namespace grpc_cb {

template <class Request, class Response>
class ServerReaderWriterInitCqTag GRPC_FINAL : public CallCqTag {
 public:
  using RwCqTag = ServerReaderWriterCqTag<Request, Response>;
  using Replier = ServerReplier<Response>;
  using MsgCallback = typename RwCqTag::MsgCallback;
  using EndCallback = typename RwCqTag::EndCallback;
  using Callbacks = typename RwCqTag::Callbacks;
  using CallbacksSptr = typename RwCqTag::CallbacksSptr;

 public:
  inline ServerReaderWriterInitCqTag(const CallSptr& call_sptr,
      const MsgCallback& on_msg, const EndCallback& on_end);
  inline bool Start() GRPC_MUST_USE_RESULT;

 public:
  inline void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  CodSendInitMd cod_send_init_md_;
  // Already got init_md and request, so no CodRecvInitMd.
  CallbacksSptr cbs_sptr_;
};  // class ServerReaderWriterInitCqTag

template <class Request, class Response>
ServerReaderWriterInitCqTag<Request, Response>::ServerReaderWriterInitCqTag(
    const CallSptr& call_sptr,
    const MsgCallback& on_msg,
    const EndCallback& on_end)
    : CallCqTag(call_sptr), cbs_sptr_(new Callbacks{on_msg, on_end}) {
  assert(call_sptr);
  assert(on_msg);
  assert(on_end);
}

template <class Request, class Response>
bool ServerReaderWriterInitCqTag<Request, Response>::Start() {
  CallOperations ops;
  // Todo: Fill send_init_md_array_ -> FillMetadataVector()
  ops.SendInitMd(cod_send_init_md_);
  return GetCallSptr()->StartBatch(ops, this);
}

template <class Request, class Response>
void ServerReaderWriterInitCqTag<Request, Response>::DoComplete(bool success) {
  assert(success);
  const CallSptr& call_sptr = GetCallSptr();
  auto* tag = new RwCqTag(call_sptr, cbs_sptr_);
  if (tag->Start()) return;

  delete tag;
  Replier(call_sptr).ReplyError(
      Status::InternalError("Failed to init client stream."));
}

};  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_SERVER_SERVER_READER_WRITER_INIT_CQTAG_H