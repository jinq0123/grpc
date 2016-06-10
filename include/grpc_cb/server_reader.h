// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_SERVER_READER_H
#define GRPC_CB_SERVER_SERVER_READER_H

#include <cassert>  // for assert()

namespace grpc_cb {

template <class Request>
class ServerReader {
 public:
  inline ServerReader(const CallSptr& call_sptr);

 public:
     // XXX no blocking
  bool BlockingReadOne(Request* request) const {
    assert(request);
    // XXX
    return false;
  }

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  struct Data {
    CallSptr call_sptr;
    Status status;
  };
  std::shared_ptr<Data> data_sptr_;  // Easy to copy.
};  // class ServerReader<>

template <class Request>
ServerReader<Request>::ServerReader(const CallSptr& call_sptr)
    : data_sptr_(new Data{call_sptr}) {
  assert(call_sptr);
  ServerInitMdCqTag* tag = new ServerInitMdCqTag(call_sptr);
  // Todo: Set init md
  Status& status = data_sptr_->status;
  status = tag->Start();
  if (!status.ok()) delete tag;
}

// Todo: much like ServerWriter?

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_SERVER_READER_H
