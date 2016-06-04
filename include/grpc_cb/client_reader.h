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
#include <grpc_cb/impl/completion_queue.h>       // for CompletionQueue::Pluck()
#include <grpc_cb/status.h>                      // for Status

namespace grpc_cb {

// Copyable.
template <class Response>
class ClientReader {
 public:
  inline ClientReader(const ChannelSptr& channel, const std::string& method,
               const ::google::protobuf::Message& request,
               const CompletionQueueSptr& cq_sptr);

 public:
  inline bool BlockingReadOne(Response* response) const;

  inline Status BlockingRecvStatus() const {
      // XXX BlockingRecvStatus()
    return Status::OK;
  }

  using ReadCallback = std::function<void(const Response&)>;
  inline void AsyncReadEach(const ReadCallback& readCallback) const;

 private:
  // Callback on each message.
  inline void OnReadEach(ClientReaderReadCqTag& tag) const;
  // Setup next async read.
  inline void AsyncReadNext(ClientReaderReadCqTag& tag) const;

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  struct Data {
    CompletionQueueSptr cq_sptr;
    CallSptr call_sptr;
    Status status;
    ReadCallback readCb;
  };
  std::shared_ptr<Data> data_sptr_;  // Easy to copy.
};  // class ClientReader<>

template <class Response>
ClientReader<Response>::ClientReader(
    const ChannelSptr& channel, const std::string& method,
    const ::google::protobuf::Message& request,
    const CompletionQueueSptr& cq_sptr)
    : data_sptr_(new Data{cq_sptr, channel->MakeSharedCall(method, *cq_sptr)}) {
  assert(cq_sptr);
  assert(channel);
  assert(data_sptr_->call_sptr);
  ClientReaderInitCqTag* tag = new ClientReaderInitCqTag(data_sptr_->call_sptr);
  Status& status = data_sptr_->status;
  status = tag->Start(request);
  if (!status.ok()) delete tag;
}

template <class Response>
bool ClientReader<Response>::BlockingReadOne(Response* response) const {
  assert(response);
  Status& status = data_sptr_->status;
  if (!status.ok()) return false;

  CallSptr& call_sptr = data_sptr_->call_sptr;
  ClientReaderReadCqTag tag(call_sptr);
  status = tag.Start();
  if (!status.ok()) return false;

  // tag.Start() has queued the tag. Wait for completion.
  data_sptr_->cq_sptr->Pluck(&tag);
  status = tag.GetResultMessage(*response);
  return status.ok();
}

// Used to bind to ClientReaderReadCqTag::Callback.
template <class Response>
void ReaderReadEach(const ClientReader<Response>& reader,
                ClientReaderReadCqTag& tag) {
  reader.OnReadEach(tag);
}

template <class Response>
void ClientReader<Response>::AsyncReadEach(const ReadCallback& readCallback) const {
  Status& status = data_sptr_->status;
  if (!status.ok()) return;
  data_sptr_->readCb = readCallback;

  CallSptr& call_sptr = data_sptr_->call_sptr;
  using std::placeholders::_1;
  // This ClientReader is copied in callback.
  ClientReaderReadCqTag::Callback cb = std::bind(ReaderReadEach<Response>, *this, _1);
  auto* tag = new ClientReaderReadCqTag(call_sptr, cb);
  status = tag->Start();
}

template <class Response>
void ClientReader<Response>::OnReadEach(ClientReaderReadCqTag& tag) const {
  Status& status = data_sptr_->status;
  assert(status.ok());
  Response resp;
  status = tag.GetResultMessage(resp);
  if (!status.ok()) return;

  ReadCallback& readCb = data_sptr_->readCb;
  if (!readCb) return;
  readCb(resp);
  AsyncReadNext(readCb);
}

template <class Response>
void ClientReader<Response>::AsyncReadNext(ClientReaderReadCqTag& tag) const {
  Status& status = data_sptr_->status;
  assert(status.ok());
  auto* tag = new ClientReaderReadCqTag(tag);
  status = tag->Start();
  // Old tag will be deleted after return.
}

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_H
