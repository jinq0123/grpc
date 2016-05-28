// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_WRITER_H
#define GRPC_CB_CLIENT_CLIENT_WRITER_H

namespace grpc_cb {

template <class Request>
class ClientWriter {
 public:
  bool Write(const Request& request) {
    // XXX
    return false;
  }
  ::grpc_cb::Status Finish() {
    // XXX
    return ::grpc_cb::Status::OK;
  }
};  // class ClientWriter<>

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_WRITER_H
