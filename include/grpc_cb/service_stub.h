// Licensed under the Apache License, Version 2.0 (the "License");
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVICE_STUB_H
#define GRPC_CB_SERVICE_STUB_H

#include <cassert>

#include <grpc_cb/channel_ptr.h>
#include <grpc_cb/support/config.h>  // for GRPC_OVERRIDE

namespace grpc_cb {

// The base of generated service stubs.
class ServiceStub {
 public:
  inline explicit ServiceStub(const ChannelPtr& channel)
    : channel_(channel) {  // copy
    assert(channel);
  }
  inline virtual ~ServiceStub() GRPC_OVERRIDE {}

 public:
  inline ChannelPtr GetChannelPtr() const { return channel_; }

 protected:
  ChannelPtr channel_;
};

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVICE_STUB_H
