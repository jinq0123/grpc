// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/server/server_writer_impl.h>

#include <grpc_cb/impl/send_msg_cqtag.h>    // for SendMsgCqTag

// Todo: Move from include/ to scr/ dir.
#include <grpc_cb/impl/server/server_init_md_cqtag.h>  // for ServerInitMdCqTag
#include <grpc_cb/impl/server/server_send_status_cqtag.h>  // for ServerSendStatusCqTag

namespace grpc_cb {

ServerWriterImpl::ServerWriterImpl(const CallSptr& call_sptr)
    : call_sptr_(call_sptr) {
  assert(call_sptr);
  ServerInitMdCqTag* tag = new ServerInitMdCqTag(call_sptr);
  // Todo: Set init md
  if (tag->Start()) return;

  delete tag;
  closed_ = true;  // error
}

ServerWriterImpl::~ServerWriterImpl() {
    Close(Status::OK);
}

bool ServerWriterImpl::Write(
    const ::google::protobuf::Message& response) {
  if (closed_) return false;
  SendMsgCqTag* tag = new SendMsgCqTag(call_sptr_);
  if (tag->Start(response)) return true;

  delete tag;
  closed_ = true;  // error
  return false;
}

void ServerWriterImpl::Close(const Status& status) {
  if (closed_) return;
  closed_ = true;

  using CqTag = ServerSendStatusCqTag;
  CqTag* tag = new CqTag(call_sptr_);
  if (tag->StartSend(status))
    return;
  delete tag;
}

}  // namespace grpc_cb
