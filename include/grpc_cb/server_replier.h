// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_REPLIER_H
#define GRPC_CB_SERVER_REPLIER_H

#include <memory>

#include <grpc_cb/impl/call_sptr.h>                   // for CallSptr
#include <grpc_cb/impl/server/server_replier_cqtag.h>  // for ServerReplierCqTag

namespace grpc_cb {

class Status;

// ServerReplier is for unary rpc call and client stream rpc.
// ServerWriter is for server stream rpc.

// Copyable.
// Safe to delete before completion.
template <class ResponseType>
class ServerReplier {
 public:
  explicit ServerReplier(const CallSptr& call_sptr)
      : data_sptr_(new Data{call_sptr}) {
    assert(call_sptr);
    assert(!data_sptr_->replied);
  };
  virtual ~ServerReplier() {};

 public:
  // Todo: Add BlockingReply(response), AsyncReply(response), AsyncReply(response, cb)
  void Reply(const ResponseType& response) const {
    bool& replied = data_sptr_->replied;
    if (replied) return;
    replied = true;
    auto* tag = new ServerReplierCqTag(data_sptr_->call_sptr);  // delete in Run()
    Status status = tag->StartReply(response);
    if (!status.ok()) delete tag;
  }

  void ReplyError(const Status& status) const {
    bool& replied = data_sptr_->replied;
    if (replied) return;
    replied = true;
    auto* tag = new ServerReplierCqTag(data_sptr_->call_sptr);  // delete in Run()
    Status status2 = tag->StartReplyError(status);
    if (!status2.ok()) delete tag;
  }

private:
  struct Data {
    const CallSptr call_sptr;
    bool replied{false};
    explicit Data(const CallSptr& call_sp) : call_sptr(call_sp) {}
  };
  const std::shared_ptr<Data> data_sptr_;  // copyable
};  // class ServerReplier

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_REPLIER_H
