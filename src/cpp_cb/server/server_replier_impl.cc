// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/server/server_replier_impl.h>

#include <google/protobuf/message.h>
#include <grpc/support/log.h>                   // for GPR_ASSERT()

#include <grpc_cb/impl/call.h>                  // for Call::call()

namespace grpc_cb {

ServerReplierImpl::~ServerReplierImpl() {}

void ServerReplierImpl::Reply(const ::google::protobuf::Message& msg) {
  assert(call_sptr_);
  ServerReplierCqTag* tag = new ServerReplierCqTag(call_sptr_);  // Delete in Run().
  CallOperations ops;
  tag->InitOps(msg, ops);
  Status status = call_sptr_->StartBatch(ops, tag);
  GPR_ASSERT(status.ok());
}

void ServerReplierImpl::ReplyError(const Status& status) {
  assert(call_sptr_);
  ServerReplierCqTag* tag = new ServerReplierCqTag(call_sptr_);  // Delete in Run().
  // XXX ops
  Status status = call_sptr_->StartBatch(ops, tag);
  GPR_ASSERT(status.ok());
}

}  // namespace grpc_cb
