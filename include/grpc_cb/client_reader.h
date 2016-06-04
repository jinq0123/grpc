// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_H
#define GRPC_CB_CLIENT_CLIENT_READER_H

#include <cassert>     // for assert()
#include <functional>  // for std::function

#include <grpc_cb/channel.h>         // for MakeSharedCall()
#include <grpc_cb/impl/call.h>       // for StartBatch()
#include <grpc_cb/impl/call_sptr.h>  // for CallSptr
#include <grpc_cb/impl/client/client_reader_init_cqtag.h>  // for ClientReaderInitCqTag
#include <grpc_cb/impl/client/client_reader_read_cqtag.h>  // for ClientReaderReadCqTag
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
      : data_sptr_(new Data{cq_sptr, channel->MakeSharedCall(method, *cq_sptr)}) {
    assert(cq_sptr);
    assert(channel);
    assert(data_sptr_->call_sptr);
    assert(data_sptr_->cq_sptr->cq() == data_sptr_->call_sptr_->call()->cq);
    ClientReaderInitCqTag* tag = new ClientReaderInitCqTag(data_sptr_->call_sptr_);
    CallOperations ops;
    Status& status = data_sptr->status;
    status = tag->InitCallOps(request, ops);
    if (status.ok()) {
      status = data_sptr_->call_sptr->StartBatch(ops, tag);  // tag keeps the buffer and other.
      return;
    }
    delete tag;
  }

 public:
  bool BlockingRead(Response* response) {
    assert(response);
    Status& status = data_sptr_->status;
    if (!status.ok())
        return false;
    CallSptr& call_sptr = data_sptr_->call_sptr;
    ClientReaderReadCqTag tag(call_sptr);
    CallOperations ops;
    tag.InitCallOps(ops);
    call_sptr->StartBatch(ops, &tag);
    data_sptr_->cq_sptr->Pluck(&tag);
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
  // Wrap all data in shared struct pointer to make copy quick.
  struct Data {
    CompletionQueueSptr cq_sptr;
    CallSptr call_sptr;
    Status status;
  };
  std::shared_ptr<Data> data_sptr_;  // Easy to copy.
};  // class ClientReader<>

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_H
