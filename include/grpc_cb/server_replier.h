// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_ASYNC_REPLIER_H
#define GRPC_CB_SERVER_ASYNC_REPLIER_H

#include <grpc_cb/impl/call_sptr.h>                   // for CallSptr
#include <grpc_cb/impl/server/server_replier_cqtag.h>  // for ServerReplierCqTag

namespace grpc_cb {

class Status;

// ServerReplier is for unary rpc call.
// ServerWriter is for server stream rpc.

// Copyable.
// Safe to delete before completion.
template <class ResponseType>
class ServerReplier {
public:
  // Copy msg_replier.
  explicit ServerReplier(const CallSptr& call_sptr)
     : call_sptr_(call_sptr) {
   assert(call_sptr);
  };
  virtual ~ServerReplier() {};

 public:
  // Todo: Add BlockingReply(response), AsyncReply(response), AsyncReply(response, cb)
  void Reply(const ResponseType& response) {
    auto* tag = new ServerReplierCqTag(call_sptr_);  // delete in Run()
    tag->StartReply(response); 
  }
  void ReplyError(const Status& status) {
    auto* tag = new ServerReplierCqTag(call_sptr_);  // delete in Run()
    tag->StartReplyError(status);
  }

private:
  CallSptr call_sptr_;  // copyable
};  // class ServerReplier

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_ASYNC_REPLIER_H
