// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_REPLIER_H
#define GRPC_CB_SERVER_REPLIER_H

#include <memory>

#include <grpc_cb/impl/call_sptr.h>       // for CallSptr
#include <grpc_cb/impl/server/server_replier_impl.h>  // for ServerReplierImpl

namespace grpc_cb {

class Status;

// ServerReplier is for unary rpc call and client stream rpc.
// ServerWriter is for server stream rpc.
// No ServerReader. Reading is done asynchronously and internally.

// Copyable.
// Safe to delete before completion.
// Only accept the 1st reply and ignore other replies.
template <class ResponseType>
class ServerReplier {
 public:
  explicit ServerReplier(const CallSptr& call_sptr)
      : impl_sptr_(new ServerReplierImpl(call_sptr)) {
    assert(call_sptr);
  };
  virtual ~ServerReplier() {};

 public:
  void Reply(const ResponseType& response) const {
    impl_sptr_->Reply(response);
  }

  void ReplyError(const Status& status) const {
    impl_sptr_->ReplyError(status);
  }

private:
  const std::shared_ptr<ServerReplierImpl> impl_sptr_;  // copyable
};  // class ServerReplier

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_REPLIER_H
