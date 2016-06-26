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
#include <grpc_cb/impl/client/client_reader_async_read_cqtag.h>  // for ClientReaderAsyncReadCqTag
#include <grpc_cb/impl/client/client_reader_read_cqtag.h>  // for ClientReaderReadCqTag
#include <grpc_cb/impl/client/client_reader_async_recv_status_cqtag.h>  // for ClientReaderAsyncRecvStatusCqTag
#include <grpc_cb/impl/client/client_reader_recv_status_cqtag.h>  // for ClientReaderRecvStatusCqTag
#include <grpc_cb/impl/completion_queue.h>  // for CompletionQueue::Pluck()
#include <grpc_cb/status.h>                 // for Status
#include <grpc_cb/status_callback.h>  // for StatusCallback

namespace grpc_cb {

// Copyable.
template <class Response>
class ClientReader GRPC_FINAL {
 public:
  // Todo: Also need to template request?
  inline ClientReader(const ChannelSptr& channel, const std::string& method,
               const ::google::protobuf::Message& request,
               const CompletionQueueSptr& cq_sptr);

 public:
  inline bool BlockingReadOne(Response* response) const;
  inline Status BlockingRecvStatus() const;

  using MsgCallback = std::function<void(const Response&)>;
  inline void AsyncReadEach(
      const MsgCallback& on_msg,
      const StatusCallback& on_status = StatusCallback()) const;

 private:
  // Callback on each message.
  inline void OnReadEach(const Response& msg) const;
  // Callback on end of reading or by error.
  inline void OnEnd(const Status& status) const;

  // Setup next async read.
  inline void AsyncReadNext() const;
  inline void AsyncRecvStatus() const;
  inline void CallStatusCb(const Status& status) const;

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  struct Data {
    CompletionQueueSptr cq_sptr;
    CallSptr call_sptr;
    Status status;
    MsgCallback on_msg;
    StatusCallback on_status;
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
  if (tag->Start(request)) return;
  delete tag;
  data_sptr_->status.SetInternalError(
      "Failed to start client reader stream.");
}

template <class Response>
bool ClientReader<Response>::BlockingReadOne(Response* response) const {
  assert(response);
  Status& status = data_sptr_->status;
  if (!status.ok()) return false;

  ClientReaderReadCqTag tag(data_sptr_->call_sptr);
  if (!tag.Start()) {
    status.SetInternalError("End of server stream.");  // Todo: use EndOfStream instead of status.
    return false;
  }

  // tag.Start() has queued the tag. Wait for completion.
  data_sptr_->cq_sptr->Pluck(&tag);
  // Todo: check HasGotMsg()...
  status = tag.GetResultMsg(*response);
  return status.ok();
}

template <class Response>
Status ClientReader<Response>::BlockingRecvStatus() const {
    ClientReaderRecvStatusCqTag tag(data_sptr_->call_sptr);
    if (!tag.Start())
        return Status::InternalError("Failed to receive status.");
    data_sptr_->cq_sptr->Pluck(&tag);
    return tag.GetStatus();
}

template <class Response>
void ClientReader<Response>::AsyncReadEach(
    const MsgCallback& on_msg,
    const StatusCallback& on_status) const {
  Status& status = data_sptr_->status;
  if (!status.ok()) return;
  data_sptr_->on_msg = on_msg;
  data_sptr_->on_status = on_status;
  AsyncReadNext();
}

template <class Response>
void ClientReader<Response>::AsyncReadNext() const {
  Status& status = data_sptr_->status;
  assert(status.ok());

  CallSptr& call_sptr = data_sptr_->call_sptr;
  // This ClientReader is copied in callback.
  ClientReader<Response> this_copy = *this;
  auto* tag = new ClientReaderAsyncReadCqTag<Response>(
      call_sptr,
      [this_copy](const Response& msg) { this_copy.OnReadEach(msg); },
      [this_copy](const Status& status) { this_copy.OnEnd(status); });
  if (tag->Start()) return;

  delete tag;
  status.SetInternalError("Failed to async read server stream.");
  CallStatusCb(status);
}

template <class Response>
void ClientReader<Response>::AsyncRecvStatus() const {
  Status& status = data_sptr_->status;
  assert(status.ok());

  auto* tag = new ClientReaderAsyncRecvStatusCqTag(
      data_sptr_->call_sptr, data_sptr_->on_status);
  if (tag->Start()) return;

  delete tag;
  status.SetInternalError("Failed to receive status.");
  CallStatusCb(status);
}

template <class Response>
void ClientReader<Response>::OnReadEach(const Response& msg) const {
  Status& status = data_sptr_->status;
  assert(status.ok());

  MsgCallback& on_msg = data_sptr_->on_msg;
  if (on_msg) on_msg(msg);

  AsyncReadNext();
  // Old tag will be deleted after return in BlockingRun().
}

template <class Response>
void ClientReader<Response>::OnEnd(const Status& status) const {
  if (status.ok()) {
    AsyncRecvStatus();
    return;
  }

  CallStatusCb(status);
}

template <class Response>
void ClientReader<Response>::CallStatusCb(const Status& status) const {
  StatusCallback& on_status = data_sptr_->on_status;
  if (on_status) on_status(status);
}

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_H
