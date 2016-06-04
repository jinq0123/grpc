// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVICE_STUB_H
#define GRPC_CB_SERVICE_STUB_H

#include <cassert>
#include <unordered_map>

#include <grpc_cb/error_callback.h>  // for ErrorCallback
#include <grpc_cb/impl/call_sptr.h>  // for CallSptr
#include <grpc_cb/impl/channel_sptr.h>
#include <grpc_cb/impl/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb/impl/completion_queue_tag.h>   // for CompletionQueueTag
#include <grpc_cb/support/config.h>              // for GRPC_OVERRIDE

namespace grpc_cb {

// The base of generated service stubs.
class ServiceStub {
 public:
  explicit ServiceStub(const ChannelSptr& channel_sptr);
  virtual ~ServiceStub();

 public:
  inline Channel& GetChannel() const {
      assert(channel_sptr_);
      return *channel_sptr_;
  }
  inline ChannelSptr GetChannelSptr() const { return channel_sptr_; }
  inline const ErrorCallback& GetErrorCallback() const {
    return error_callback_;
  }
  inline void SetErrorCallback(const ErrorCallback& cb) {
    assert(cb);
    error_callback_ = cb;
  }
  inline CompletionQueue& GetCq() const {
    assert(cq_sptr_);
    return *cq_sptr_;
  }
  inline CompletionQueueSptr GetCqSptr() const { return cq_sptr_; }

 public:
  static inline ErrorCallback& GetDefaultErrorCallback() {
    return default_error_callback_;
  }
  static inline void SetDefaultErrorCallback(const ErrorCallback cb) {
    assert(cb);
    default_error_callback_ = cb;
  }
  // Default ignore error.
  static void IgnoreError(const Status&) {}

 public:
  template <class ResponseType>
  static void IgnoreResponse(const ResponseType&) {}

 public:
  void BlockingRun();
  // Request the shutdown of all runs.
  void Shutdown();

  // DEL
 //public:
 // template <class ResponseType>
 // CompletionQueueTag* NewCompletionQueueTag(
 //     const CallSptr& call, const std::function<void(const ResponseType&)>& cb,
 //     const ErrorCallback& ecb);
 // void DeleteCompletionQueueTag(CompletionQueueTag* tag) { delete tag; }

 private:
  ChannelSptr channel_sptr_;
  ErrorCallback error_callback_;
  CompletionQueueSptr cq_sptr_;

 private:
  static ErrorCallback default_error_callback_;
  // DEL
 //private:
 // template <class ResponseType>
 // class CompletionCb : public CompletionQueueTag {  // Rename to ClientAsyncCallCqTag
 //  public:
 //   typedef std::function<void(const ResponseType&)> ResponseCallback;
 //   CompletionCb(const CallSptr& call, const ResponseCallback& cb,
 //                const ErrorCallback& ecb)
 //       : call_(call), cb_(cb), ecb_(ecb){};
 //   virtual void DoComplete(bool success) GRPC_OVERRIDE;

 //  private:
 //   CallSptr call_;
 //   ResponseCallback cb_;
 //   ErrorCallback ecb_;
 // };
};

// Todo: Delete this function?
//template <class ResponseType>
//CompletionQueueTag* ServiceStub::NewCompletionQueueTag(
//    const CallSptr& call, const std::function<void(const ResponseType&)>& cb,
//    const ErrorCallback& ecb) {
//  assert(call && cb && ecb);
//  return new CompletionCb<ResponseType>(call, cb, ecb);
//  // DeleteCompletionQueueTag() will be called in ServiceStub::Run().
//}

// DEL
//template <class ResponseType>
//void ServiceStub::CompletionCb<ResponseType>::DoComplete(bool success) {
//  assert(cb_ && ecb_ && call_);
//  if (!success) {
//    ecb_(Status::InternalError("Failed to complete"));
//    return;
//  }
//  ResponseType response;
//  Status status = call_->GetResponse(&response);
//  if (status.ok()) {
//    cb_(response);
//    return;
//  }
//  ecb_(status);
//}

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVICE_STUB_H
