// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_SERVER_ASYNC_MSG_REPILER_H
#define GRPC_CB_SERVER_SERVER_ASYNC_MSG_REPILER_H

#include <grpc_cb/support/config.h>  // for GRPC_FINAL
#include <grpc_cb/support/protobuf_fwd.h>  // for Message

struct grpc_call;

namespace grpc_cb {

class ReplyTag;
class Status;

// Reply ::google::protobuf::Message.
class ServerAsyncMsgReplier GRPC_FINAL {
 public:
  // TODO: Delete ServerAsyncMsgReplier and use grpc_call* directly.
  explicit ServerAsyncMsgReplier(grpc_call* call) : call_(call){};  // XXX

 public:
  void Reply(const ::google::protobuf::Message& msg);
  void ReplyError(const Status& status);

 private:
  void StartBatch(ReplyTag* tag);

 private:
  grpc_call* call_;
};

}  // namespace grpb_cb

#endif  // #define GRPC_CB_SERVER_SERVER_ASYNC_MSG_REPILER_H

