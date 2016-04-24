// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_H
#define GRPC_CB_CLIENT_CLIENT_READER_H

#include <cassert>     // for assert()
#include <functional>  // for std::function

#include <grpc_cb/support/status.h>

namespace grpc_cb {

template <class Response>
class ClientReader {
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
};  // class ClientReader<>

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_H
