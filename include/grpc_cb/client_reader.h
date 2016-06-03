// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_H
#define GRPC_CB_CLIENT_CLIENT_READER_H

#include <cassert>     // for assert()
#include <functional>  // for std::function

#include <grpc_cb/channel.h>                               // for MakeCall()
#include <grpc_cb/impl/call.h>                             // for StartBatch()
#include <grpc_cb/impl/call_sptr.h>                        // for CallSptr
#include <grpc_cb/impl/client/client_reader_init_cqtag.h>  // for ClientReaderInitCqTag
#include <grpc_cb/impl/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb/status.h>                      // for Status

namespace grpc_cb {

// Copyable.
template <class Response>
class ClientReader {
 public:
  ClientReader(const ChannelSptr& channel, const std::string& method,
               const ::google::protobuf::Message& request,
               const CompletionQueueSptr& cq_sptr)
      : cq_sptr_(cq_sptr), call_sptr_(channel->MakeCall(method, *cq_sptr)) {
    assert(cq_sptr);
    assert(channel);
    assert(call_sptr_);
    assert(cq_sptr->cq() == call_sptr_->call()->cq);
    ClientReaderInitCqTag* tag = new ClientReaderInitCqTag(call_sptr_);
    CallOperations ops;
    status_ = tag->InitCallOps(request, ops);
    if (status_.ok()) {
      status_ = call_sptr_->StartBatch(ops, tag);  // tag keeps the buffer and other.
      return;
    }
    delete tag;
  }

 public:
  bool BlockingRead(Response* response) {
    assert(response);
    if (!status_.ok())
        return false;
    ClientReaderReadCqTag tag(call_sptr_);
    CallOperations ops;
    tag.InitCallOps(ops);
    call_sptr_->StartBatch(ops, &tag);
    cq_sptr_->Pluck(&tag);
    tag.GetResponse(*response);  // XXX return result
    // XXX
    return true;
  }

  ::grpc_cb::Status BlockingRecvStatus() {
    return ::grpc_cb::Status::OK;
  }

  void SetReadCallback(std::function<void (const Response&)> readCallback) {
    // XXX
  }

 private:
  CompletionQueueSptr cq_sptr_;
  CallSptr call_sptr_;
  Status status_;
};  // class ClientReader<>

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_H
