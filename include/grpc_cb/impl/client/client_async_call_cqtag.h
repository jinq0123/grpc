// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_ASYNC_CALL_CQTAG_H
#define GRPC_CB_CLIENT_ASYNC_CALL_CQTAG_H

#include <grpc_cb/error_callback.h>                 // for ErrorCallback
#include <grpc_cb/impl/client/client_call_cqtag.h>  // for ClientCallCqTag

namespace grpc_cb {

// Completion queue tag (CqTag) for client async call.
// Derived from ClientCallCqTag, adding cb, err_cb.
template <class ResponseType>
class ClientAsyncCallCqTag : public ClientCallCqTag {
public:
  using ResponseCallback = std::function<void (const ResponseType&)>;
public:
  ClientAsyncCallCqTag(const ResponseCallback& cb, const ErrorCallback& ecb)
     : cb_(cb), ecb_(ecb) {}
  virtual ~ClientAsyncCallCqTag() {}

  // XXX DoComplete()

 private:
  ResponseCallback cb_;
  ErrorCallback ecb_;
};  // class ClientAsyncCallCqTag

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_ASYNC_CALL_CQTAG_H
