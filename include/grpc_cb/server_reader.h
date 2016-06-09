// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_SERVER_READER_H
#define GRPC_CB_SERVER_SERVER_READER_H

#include <cassert>  // for assert()

namespace grpc_cb {

template <class Request>
class ServerReader {
 public:
  inline ServerReader(const CallSptr& call_sptr) {};  // XXX

 public:
     // XXX no blocking
  bool BlockingReadOne(Request* request) const {
    assert(request);
    // XXX
    return false;
}

};  // class ServerReader<>

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_SERVER_READER_H
