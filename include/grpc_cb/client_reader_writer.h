// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_WRITER_H
#define GRPC_CB_CLIENT_CLIENT_READER_WRITER_H

namespace grpc_cb {

template <class Request, class Response>
class ClientReaderWriter {
 public:
  void BlockingWriteOne(const Request& request) {
    // XXX
  }

  bool BlockingReadOne(const Response* response)
  {
    assert(response);
    // XXX
    return false;
  }

  void WritesDone() {}

  ::grpc_cb::Status Finish() {
    return ::grpc_cb::Status::OK;
  }
};  // class ClientReaderWriter<>

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_WRITER_H
