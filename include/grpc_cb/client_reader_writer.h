// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_WRITER_H
#define GRPC_CB_CLIENT_CLIENT_READER_WRITER_H

namespace grpc_cb {

template <class Request, class Response>
class ClientReaderWriter {
 public:
  void Write(const Request& request) const {
    // XXX
  }

  bool BlockingReadOne(const Response* response) const {
    assert(response);
    // XXX
    return false;
  }

  void WritesDone() const {}

  ::grpc_cb::Status BlockingFinish() const {
    return ::grpc_cb::Status::OK;
  }

  inline bool BlockingReadOne(Response* response) const;
  using ReadCallback = std::function<void(const Response&)>;
  inline void AsyncReadEach(const ReadCallback& readCallback) const;

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  struct Data {
    CompletionQueueSptr cq_sptr;
    CallSptr call_sptr;
    Status status;
    ReadCallback readCb;
  };
  std::shared_ptr<Data> data_sptr_;  // Easy to copy.
};  // class ClientReaderWriter<>

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_WRITER_H
