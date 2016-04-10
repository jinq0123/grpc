// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/channel.h>

#include <cassert>

#include <grpc/grpc.h>
#include <grpc_cb/impl/call.h>

namespace grpc_cb {

Channel::Channel(const std::string& target)
    : c_channel_(grpc_insecure_channel_create(
        target.c_str(), nullptr, nullptr)) {
  if (!c_channel_) {
    throw("Can not create channel.");
  }
}

Channel::Channel(const std::string& host, grpc_channel* channel)
    : host_(host), c_channel_(channel) {
  assert(channel);
}

Channel::~Channel() {
  assert(c_channel_);
  grpc_channel_destroy(c_channel_);
}

CallUptr Channel::CreateCall(
    const std::string& method,
    grpc_completion_queue& cp) {
  grpc_call* c_call = grpc_channel_create_call(
    c_channel_, nullptr, GRPC_PROPAGATE_DEFAULTS, &cp, method.c_str(), nullptr,
    gpr_inf_future(GPR_CLOCK_REALTIME), nullptr);
  return CallUptr(new Call(c_call));
}

}  // namespace grpc_cb