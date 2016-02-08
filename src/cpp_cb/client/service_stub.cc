// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/service_stub.h>

#include <grpc_cb/completion_queue.h>

namespace grpc_cb {

ErrorCallback ServiceStub::default_error_callback_(&ServiceStub::IgnoreError);

ServiceStub::ServiceStub(const ChannelPtr& channel) :
    channel_(channel),  // copy shared_ptr
    error_callback_(default_error_callback_),
    cq_(new CompletionQueue) {
  assert(channel);
  assert(error_callback_);
}

ServiceStub::~ServiceStub() {
}

// Blocking run stub.
void ServiceStub::Run() {
  assert(cq_);
  CompletionQueue& cq = *cq_;
  bool ok = false;
  while (true) {
    grpc_event ev = cq.Next();
    ;  // TODO
  }
}

void ServiceStub::Shutdown() {
  assert(cq_);
  cq_->Shutdown();
}

}  // namespace grpc_cb

