// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_SERVER_READER_WRITER_H
#define GRPC_CB_SERVER_SERVER_READER_WRITER_H

#include <cassert>  // for assert()

namespace grpc_cb {

template <class Request, class Response>
class ServerReaderWriter {
 public:
  ServerReaderWriter(const CallSptr& call_sptr) {}  // XXX

 public:
     // XXX no blocking
  bool BlockingReadOne(Request* request) const {
    assert(request);
    // XXX
    return false;
  }

  void Write(const Response& response) const {
    // XXX
  }
};  // class ServerReaderWriter<>

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_SERVER_READER_WRITER_H
