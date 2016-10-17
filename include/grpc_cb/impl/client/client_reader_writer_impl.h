// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_WRITER_IMPL_H
#define GRPC_CB_CLIENT_CLIENT_READER_WRITER_IMPL_H

#include <grpc_cb/impl/client/client_send_close_cqtag.h>  // for ClientSendCloseCqTag
// Todo: include?

namespace grpc_cb {

template <class Request, class Response>
class ClientReaderWriterImpl GRPC_FINAL {
 public:
  inline ClientReaderWriterImpl(const ChannelSptr& channel,
                            const std::string& method,
                            const CompletionQueueSptr& cq_sptr);
  inline ~ClientReaderWriterImpl();

 public:
  // Write is always asynchronous.
  inline bool Write(const Request& request) const;
  // WritesDone() is optional. Writes are auto done in dtr().
  inline void WritesDone();

  inline bool BlockingReadOne(Response* response) const;
  inline Status BlockingRecvStatus() const {
    const Data& d = *data_sptr_;
    return ClientReaderHelper::BlockingRecvStatus(d.call_sptr, d.cq_sptr);
  }

  using ReadCallback = std::function<void(const Response&)>;
  inline void AsyncReadEach(
      const ReadCallback& on_read,
      const StatusCallback& on_status = StatusCallback()) const;

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  using Data = ClientReaderData<Response>;
  using DataSptr = ClientReaderDataSptr<Response>;
  DataSptr data_sptr_;  // Same as reader. Easy to copy.
  bool writes_done_ = false;  // Is WritesDone() called?
};  // class ClientReaderWriterImpl<>

// Todo: BlockingGetInitMd();

template <class Request, class Response>
ClientReaderWriterImpl<Request, Response>::ClientReaderWriterImpl(
    const ChannelSptr& channel, const std::string& method,
    const CompletionQueueSptr& cq_sptr)
    // Todo: same as ClientReader?
    : data_sptr_(new Data{cq_sptr, channel->MakeSharedCall(method, *cq_sptr)}) {
  assert(cq_sptr);
  assert(channel);
  assert(data_sptr_->call_sptr);
  ClientInitMdCqTag* tag = new ClientInitMdCqTag(data_sptr_->call_sptr);
  if (tag->Start()) return;
  delete tag;
  data_sptr_->status.SetInternalError("Failed to init stream.");
}

template <class Request, class Response>
ClientReaderWriterImpl<Request, Response>::~ClientReaderWriterImpl() {
  WritesDone();
}

template <class Request, class Response>
bool ClientReaderWriterImpl<Request, Response>::Write(const Request& request) const {
  assert(data_sptr_);
  assert(data_sptr_->call_sptr);
  return ClientWriterHelper::Write(data_sptr_->call_sptr,
      request, data_sptr_->status);
}

template <class Request, class Response>
void ClientReaderWriterImpl<Request, Response>::WritesDone() {
  if (writes_done_) return;
  writes_done_ = true;
  Status& status = data_sptr_->status;
  if (!status.ok()) return;
  ClientSendCloseCqTag* tag = new ClientSendCloseCqTag(data_sptr_->call_sptr);
  if (tag->Start()) return;

  delete tag;
  status.SetInternalError("Failed to set stream writes done.");
}

// Todo: same as ClientReader?
template <class Request, class Response>
bool ClientReaderWriterImpl<Request, Response>::BlockingReadOne(Response* response) const {
  assert(response);
  Data& d = *data_sptr_;
  return ClientReaderHelper::BlockingReadOne(
      d.call_sptr, d.cq_sptr, *response, d.status);
}

template <class Request, class Response>
void ClientReaderWriterImpl<Request, Response>::AsyncReadEach(
    const ReadCallback& on_read,
    const StatusCallback& on_status) const {
    data_sptr_->on_msg = on_read;
    data_sptr_->on_status = on_status;
    ClientReaderHelper::AsyncReadNext(data_sptr_);
}

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_WRITER_IMPL_H
