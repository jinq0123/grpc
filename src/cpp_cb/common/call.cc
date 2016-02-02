// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/call.h>

#include <cassert>

#include <grpc/grpc.h>

namespace grpc_cb {

Call::Call(grpc_call* call)
    : call_(call) {
  assert(call);
}

Call::~Call() {
  assert(call_);
  grpc_call_destroy(call_);
}

}  // namespace grpc_cb
