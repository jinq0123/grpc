// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_WRITER_H
#define GRPC_CB_CLIENT_CLIENT_READER_WRITER_H

namespace grpc_cb {

// Todo: Add base class?

template <class Request, class Response>
class ClientReaderWriter {
 public:
  inline ClientReaderWriter();
 public:
  inline void Write(const Request& request) const;
  inline void WritesDone() const;
  inline bool BlockingReadOne(Response* response) const;
  using ReadCallback = std::function<void(const Response&)>;
  inline void AsyncReadEach(const ReadCallback& readCallback) const;
  inline Status BlockingFinish() const;
  // XXX AsyncFinish()

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

template <class Request, class Response>
ClientReaderWriter<Request, Response>::ClientReaderWriter() {
    // XXX
}

template <class Request, class Response>
void ClientReaderWriter<Request, Response>::Write(const Request& request) const {
  // XXX
}

template <class Request, class Response>
void ClientReaderWriter<Request, Response>::WritesDone() const {
  // XXX
}

template <class Request, class Response>
bool ClientReaderWriter<Request, Response>::BlockingReadOne(Response* response) const {
  // XXX
  return false;
}

template <class Request, class Response>
void ClientReaderWriter<Request, Response>::AsyncReadEach(const ReadCallback& readCallback) const {
  // XXX
}

template <class Request, class Response>
Status ClientReaderWriter<Request, Response>::BlockingFinish() const {
  // XXX
  return Status::OK;
}

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_WRITER_H
