// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_ASYNC_CALL_CQTAG_H
#define GRPC_CB_CLIENT_ASYNC_CALL_CQTAG_H

#include <grpc_cb/client_call_cqtag.h>

namespace grpc_cb {

// Completion queue tag (CqTag) for client async call.
// Derived from ClientCallCqTag, adding cb, err_cb.
template <class ResponseType>
class ClientAsyncCallCqTag : public ClientCallCqTag {
public:
  using Callback = std::function<void(const ResponseType&)>;
public:
  ClientAsyncCallCqTag(const Callback& cb, const ErrorCallback& ecb)
     : cb_(cb), ecb_(ecb) {}
  virtual ~ClientAsyncCallCqTag() {}

  // XXX DoComplete()

 private:
  Callback cb_;
  ErrorCallback ecb_;
};  // class ClientAsyncCallCqTag

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_ASYNC_CALL_CQTAG_H
