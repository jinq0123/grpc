// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_ASYNC_READ_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_READER_ASYNC_READ_CQTAG_H

#include <grpc_cb/support/config.h>   // for GRPC_FINAL
#include <grpc_cb/status_callback.h>  // for ErrorCallback
#include <grpc_cb/impl/client/client_reader_read_cqtag.h>  // for ClientReaderReadCqTag

namespace grpc_cb {

// For ClientReader and  ClientReaderWriter.
template <class Response>
class ClientReaderAsyncReadCqTag GRPC_FINAL : public ClientReaderReadCqTag {
 public:
  using MsgCallback = std::function<void (const Response&)>;
  inline explicit ClientReaderAsyncReadCqTag(const CallSptr& call_sptr,
                                        const MsgCallback& cb = MsgCallback(),
                                        const ErrorCallback& ecb = ErrorCallback())
      : ClientReaderReadCqTag(call_sptr), cb_(cb), ecb_(ecb) {}

  inline void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  // Callback will be triggered on completion in DoComplete().
  MsgCallback cb_;
  ErrorCallback ecb_;
};  // class ClientReaderAsyncReadCqTag

template <class Response>
void ClientReaderAsyncReadCqTag<Response>::DoComplete(bool success) {
  assert(success);

  Response resp;
  Status status = GetResultMsg(resp);
  if (status.ok()) {
    if (cb_) cb_(resp);
    return;
  }

  if (ecb_) ecb_(status);
}

};  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_ASYNC_READ_CQTAG_H
