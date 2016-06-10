// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_SERVER_READER_H
#define GRPC_CB_SERVER_SERVER_READER_H

#include <cassert>  // for assert()

namespace grpc_cb {

template <class Request, class Response>
class ServerReader {
 public:
  inline ServerReader(const CallSptr& call_sptr);

 public:
  // BlockingReadOne() is intended to use in thread other than server run thread.
  inline bool BlockingReadOne(Request* request) const;
  // Todo: ReadCallback, ReadErrorCb, DoneCallback
  inline bool AsyncReadEach() const {}  // Todo: async read each callback
  inline bool AsyncReadOne() const {}  // XXX

  inline void ReplyError(const Status& error_status) const {}  // XXX
  inline void Reply(const Response& response) const {}  // XXX

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  struct Data {
    CallSptr call_sptr;
    Status status;
  };
  std::shared_ptr<Data> data_sptr_;  // Easy to copy.
};  // class ServerReader<>

template <class Request, class Response>
ServerReader<Request, Response>::ServerReader(const CallSptr& call_sptr)
    : data_sptr_(new Data{call_sptr}) {
  assert(call_sptr);
  ServerInitMdCqTag* tag = new ServerInitMdCqTag(call_sptr);
  // Todo: Set init md
  Status& status = data_sptr_->status;
  status = tag->Start();
  if (!status.ok()) delete tag;
}

// Todo: much like ServerWriter?

template <class Request, class Response>
bool ServerReader<Request, Response>::BlockingReadOne(Request* request) const {
    assert(request);
    // XXX
    return false;
}

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_SERVER_READER_H
