// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVICE_STUB_H
#define GRPC_CB_SERVICE_STUB_H

#include <cassert>

#include <grpc_cb/channel_ptr.h>
#include <grpc_cb/error_callback.h>  // for ErrorCallback

namespace grpc_cb {

// The base of generated service stubs.
class ServiceStub {
 public:
  inline explicit ServiceStub(const ChannelPtr& channel)
      : channel_(channel)  // copy shared_ptr
      , error_callback_(default_error_callback_) {
    assert(channel);
    assert(error_callback_);
  }
  inline virtual ~ServiceStub() {}

 public:
  inline ChannelPtr GetChannelPtr() const { return channel_; }
  inline const ErrorCallback& GetErrorCallback() const {
    return error_callback_;
  }
  inline void SetErrorCallback(const ErrorCallback& cb) {
    assert(cb);
    error_callback_ = cb;
  }

 public:
  static inline ErrorCallback& GetDefaultErrorCallback() {
    return default_error_callback_;
  }
  static inline void SetDefaultErrorCallback(const ErrorCallback cb) {
    assert(cb);
    default_error_callback_ = cb;
  }
  // Default ignore error.
  static void IgnoreError(const Status& error_status) {};

 protected:
  ChannelPtr channel_;
  ErrorCallback error_callback_;

 protected:
  static ErrorCallback default_error_callback_;
};

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVICE_STUB_H
