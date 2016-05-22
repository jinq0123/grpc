// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_H
#define GRPC_CB_CLIENT_CLIENT_READER_H

#include <cassert>     // for assert()
#include <functional>  // for std::function

#include <grpc_cb/support/status.h>
#include <grpc_cb/impl/call_op.h>
#include <grpc_cb/impl/call_sptr.h>  // for CallSptr

namespace grpc_cb {

// Copyable.
template <class Response>
class ClientReader {
 public:
  ClientReader(const ChannelSptr& channel,
               const ::google::protobuf::Message& request) {
    // XXX
  }

 public:
  bool BlockingRead(Response* response) {
    assert(response);
    // XXX
    return false;
  }

  ::grpc_cb::Status BlockingRecvStatus() {
    return ::grpc_cb::Status::OK;
  }

  void SetReadCallback(std::function<void (const Response&)> readCallback) {
    // XXX
  }

 private:
  CallSptr call_;
  CallOpSet<CallOpSendInitialMetadata, CallOpSendMessage,
      CallOpRecvInitialMetadata, CallOpClientSendClose> init_ops_;
  CallOpSet<CallOpRecvMessage<R>> read_ops_;
  CallOpSet<CallOpClientRecvStatus> finish_ops_;
};  // class ClientReader<>

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_H
