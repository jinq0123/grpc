// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_ASYNC_REPLIER_H
#define GRPC_CB_SERVER_ASYNC_REPLIER_H

#include <grpc_cb/impl/call_sptr.h>            // for CallSptr
#include <grpc_cb/server_async_msg_replier.h>  // for ServerAsyncMsgReplier

namespace grpc_cb {

// Todo: Rename to ServerReplier, always async.

// Copyable.
template <class ResponseType>
class ServerAsyncReplier {
public:
 // Copy msg_replier.
 explicit ServerAsyncReplier(const CallSptr& call_sptr)
     : msg_replier_(call_sptr){
   assert(call_sptr);
 };
 ~ServerAsyncReplier(){};

public:
    // XXX BlockingReply(), AsyncReply(response), AsyncReply(response, cb)
  void Reply(const ResponseType& response) { msg_replier_.Reply(response); }
  // XXX Reply(ResponseType&) calls Reply(Message&) to hide impl.
  void ReplyError(const Status& status) { msg_replier_.ReplyError(status); }

private:
    // XXX Add Reply(const Message&)
    // XXX Use shared_ptr<ServerReplierImpl>
  ServerAsyncMsgReplier msg_replier_;  // copyable
};  // class ServerAsyncReplier

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_ASYNC_REPLIER_H
