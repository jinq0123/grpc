// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVICE_STUB_H
#define GRPC_CB_SERVICE_STUB_H

#include <cassert>
#include <unordered_map>

#include <grpc_cb/channel_sptr.h>
#include <grpc_cb/error_callback.h>  // for ErrorCallback
#include <grpc_cb/impl/call_uptr.h>  // for CallUptr
#include <grpc_cb/support/config.h>  // for GRPC_OVERRIDE

namespace google {
namespace protobuf {
class Descriptor;
}  // namespace protobuf
}  // namespace google

namespace grpc_cb {

class CompletionQueue;

// The base of generated service stubs.
class ServiceStub {
 public:
  explicit ServiceStub(const ChannelSptr& channel);
  virtual ~ServiceStub();

 public:
  inline ChannelSptr GetChannelSptr() const { return channel_; }
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

 public:
  template <class ResponseCallback>
  void* AddCompletionCb(
      CallUptr&& call,
      const ::google::protobuf::Descriptor* resp_desc,
      const ResponseCallback& cb,
      const ErrorCallback& ecb);
  void EraseCompletionCb(void* tag) {
    cb_map_.erase(tag);
  }

 protected:
  ChannelSptr channel_;
  ErrorCallback error_callback_;
  std::unique_ptr<CompletionQueue> cq_;

 protected:
  static ErrorCallback default_error_callback_;

 private:
  class CompletionCbInterface {
   public:
    virtual void DoHandleResponse() = 0;
  };
  template <class ResponseCallback>
  class CompletionCb : public CompletionCbInterface {
   public:
    CompletionCb(CallUptr&& call,
                 const google::protobuf::Descriptor* response_descriptor,
                 const ResponseCallback& cb,
                 const ErrorCallback& ecb) :
        call_(std::forward<CallUptr>(call)),
        response_descriptor_(response_descriptor),
        cb_(cb),
        ecb_(ecb) {
    };
    virtual void DoHandleResponse() GRPC_OVERRIDE;
   private:
    CallUptr call_;
    const google::protobuf::Descriptor* response_descriptor_;
    ResponseCallback cb_;
    ErrorCallback ecb_;
  };

  typedef std::shared_ptr<CompletionCbInterface> CompletionCbSptr;
  typedef std::unordered_map<void*, CompletionCbSptr> CompletionCbMap;
  CompletionCbMap cb_map_;
};

template <class ResponseCallback>
void* ServiceStub::AddCompletionCb(
    CallUptr&& call,
    const ::google::protobuf::Descriptor* resp_desc,
    const ResponseCallback& cb,
    const ErrorCallback& ecb) {
  assert(call && resp_desc && cb && ecb);
  CompletionCbSptr tag(new CompletionCb<ResponseCallback>(
    std::forward<CallUptr>(call), resp_desc, cb, ecb));
  cb_map_[tag.get()] = tag;
  return tag.get();
}

template <class ResponseCallback>
void ServiceStub::CompletionCb<ResponseCallback>::DoHandleResponse() {
  // TODO
}

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVICE_STUB_H
