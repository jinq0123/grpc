// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMP_SERVER_SERVER_REPILER_IMPL_H
#define GRPC_CB_IMP_SERVER_SERVER_REPILER_IMPL_H

#include <cassert>

#include <grpc_cb/impl/call_sptr.h>        // for CallSptr
#include <grpc_cb/support/config.h>        // for GRPC_FINAL
#include <grpc_cb/support/protobuf_fwd.h>  // for Message

namespace grpc_cb {

class ReplyTag;
class Status;

// Reply ::google::protobuf::Message.
class ServerReplierImpl GRPC_FINAL {
 public:
  explicit ServerReplierImpl(const CallSptr& call_sptr)
      : call_sptr_(call_sptr){
    assert(call_sptr);
  };

 public:
  void Reply(const ::google::protobuf::Message& msg);
  void ReplyError(const Status& status);

 private:
  void StartBatch(ReplyTag* tag);

 private:
  CallSptr call_sptr_;
};

}  // namespace grpb_cb

#endif  // #define GRPC_CB_IMP_SERVER_SERVER_REPILER_IMPL_H

