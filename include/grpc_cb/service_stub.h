// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVICE_STUB_H
#define GRPC_CB_SERVICE_STUB_H

#include <cassert>
#include <unordered_map>

#include <grpc_cb/channel_ptr.h>
#include <grpc_cb/error_callback.h>  // for ErrorCallback
#include <grpc_cb/impl/call_uptr.h>  // for CallUptr

namespace grpc_cb {

class CompletionQueue;

// The base of generated service stubs.
class ServiceStub {
 public:
  explicit ServiceStub(const ChannelPtr& channel);
  virtual ~ServiceStub();

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
  static void IgnoreError(const Status&) {
  }

 public:
  template <class ResponseType>
  static void IgnoreResponse(const ResponseType&) {
  }

 public:
  void Run();  // Blocking.
  // Request the shutdown of all runs.
  void Shutdown();

 protected:
  ChannelPtr channel_;
  ErrorCallback error_callback_;
  std::unique_ptr<CompletionQueue> cq_;
  typedef std::unordered_map<void*, CallUptr> CallMap;
  CallMap call_map_;

 protected:
  static ErrorCallback default_error_callback_;
};

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVICE_STUB_H
