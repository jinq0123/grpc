// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_SERVER_READER_WRITER_H
#define GRPC_CB_SERVER_SERVER_READER_WRITER_H

#include <cassert>  // for assert()

namespace grpc_cb {

template <class Request, class Response>
class ServerReaderWriter {
 public:
  inline ServerReaderWriter(const CallSptr& call_sptr) {}  // XXX

 public:
     // XXX no blocking
  inline bool BlockingReadOne(Request* request) const {
    assert(request);
    // XXX
    return false;
  }

  inline void Write(const Response& response) const {
    // XXX
  }

  inline void Close(const Status& status) const {}  // XXX
};  // class ServerReaderWriter<>

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_SERVER_READER_WRITER_H
