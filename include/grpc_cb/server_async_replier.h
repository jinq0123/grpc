// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_ASYNC_REPLIER_H
#define GRPC_CB_SERVER_ASYNC_REPLIER_H

#include <grpc_cb/server_async_msg_replier.h>  // for ServerAsyncMsgReplier

namespace grpc_cb {

// Copyable.
template <class ResponseType>
class ServerAsyncReplier {
public:
 // Copy msg_replier.
 explicit ServerAsyncReplier(const ServerAsyncMsgReplier& msg_replier)
     : msg_replier_(msg_replier){};
 ~ServerAsyncReplier(){};

public:
  void Reply(const ResponseType& response) { msg_replier_.Reply(response); }
  void ReplyError(const Status& status) { msg_replier_.ReplyError(status); }

private:
  ServerAsyncMsgReplier msg_replier_;  // copy
};  // class ServerAsyncReplier

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_ASYNC_REPLIER_H
