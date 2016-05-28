// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_ASYNC_REPLIER_H
#define GRPC_CB_SERVER_ASYNC_REPLIER_H

#include <grpc_cb/impl/call_sptr.h>                   // for CallSptr
#include <grpc_cb/impl/server/server_replier_impl.h>  // for ServerReplierImpl

namespace grpc_cb {

class Status;

// ServerReplier is for unary rpc call.
// ServerWriter is for server stream rpc.

// Copyable.
template <class ResponseType>
class ServerReplier {
public:
  // Copy msg_replier.
  explicit ServerReplier(const CallSptr& call_sptr)
     : msg_replier_(call_sptr) {
   assert(call_sptr);
  };
  virtual ~ServerReplier() {};

 public:
  // Todo: Add BlockingReply(response), AsyncReply(response), AsyncReply(response, cb)
  void Reply(const ResponseType& response) { msg_replier_.Reply(response); }
  void ReplyError(const Status& status) { msg_replier_.ReplyError(status); }

private:
  ServerReplierImpl msg_replier_;  // copyable
};  // class ServerReplier

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_ASYNC_REPLIER_H
