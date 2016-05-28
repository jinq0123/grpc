// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/server/server_replier_impl.h>

#include <google/protobuf/message.h>
#include <grpc/support/log.h>                   // for GPR_ASSERT()

#include <grpc_cb/impl/call.h>                  // for Call::call()

namespace grpc_cb {

void ServerReplierImpl::Reply(const ::google::protobuf::Message& msg) {
  ReplyTag* tag = new ReplyTag(call_sptr_, msg);  // Delete in Run().
  StartBatch(tag);
}

void ServerReplierImpl::ReplyError(const Status& status) {
  ReplyTag* tag = new ReplyTag(call_sptr_, status);  // Delete in Run().
  StartBatch(tag);
}

void ServerReplierImpl::StartBatch(ReplyTag* tag) {
  assert(tag);
  assert(call_sptr_);
  // XXX Status result = call_sptr_->StartBatch(tag);
  grpc_call_error result = grpc_call_start_batch(
    call_sptr_->call(), tag->GetOps(), tag->GetOpsNum(), tag, nullptr);
  GPR_ASSERT(GRPC_CALL_OK == result);
}

}  // namespace grpc_cb
