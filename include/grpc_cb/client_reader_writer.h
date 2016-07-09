// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_WRITER_H
#define GRPC_CB_CLIENT_CLIENT_READER_WRITER_H

#include <grpc_cb/impl/client/client_send_close_cqtag.h>  // for ClientSendCloseCqTag
// Todo: include?

namespace grpc_cb {

// Todo: Add base class?

template <class Request, class Response>
class ClientReaderWriter GRPC_FINAL {
 public:
  inline ClientReaderWriter(const ChannelSptr& channel,
                            const std::string& method,
                            const CompletionQueueSptr& cq_sptr);

 public:
  inline bool Write(const Request& request) const;
  // WritesDone() is optional. Writes are auto done when finish.
  inline void WritesDone() const;
  inline bool BlockingReadOne(Response* response) const;
  using ReadCallback = std::function<void(const Response&)>;
  inline void AsyncReadEach(
      const ReadCallback& on_read,
      const StatusCallback& on_status = StatusCallback()) const;
  inline Status BlockingFinish() const;
  // XXX AsyncFinish()

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  using Data = ClientReaderData<Response>;
  using DataSptr = ClientReaderDataSptr<Response>;
  DataSptr data_sptr_;  // Same as reader. Easy to copy.
};  // class ClientReaderWriter<>

// Todo: BlockingGetInitMd();

template <class Request, class Response>
ClientReaderWriter<Request, Response>::ClientReaderWriter(
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
bool ClientReaderWriter<Request, Response>::Write(const Request& request) const {
  assert(data_sptr_);
  assert(data_sptr_->call_sptr);
  return ClientWriterHelper::Write(data_sptr_->call_sptr,
      request, data_sptr_->status);
}

template <class Request, class Response>
void ClientReaderWriter<Request, Response>::WritesDone() const {
  Status& status = data_sptr_->status;
  if (!status.ok()) return;
  ClientSendCloseCqTag* tag = new ClientSendCloseCqTag(data_sptr_->call_sptr);
  if (tag->Start()) return;

  delete tag;
  status.SetInternalError("Failed to set stream writes done.");
}

// Todo: same as ClientReader?
template <class Request, class Response>
bool ClientReaderWriter<Request, Response>::BlockingReadOne(Response* response) const {
  assert(response);
  Data& d = *data_sptr_;
  return ClientReaderHelper::BlockingReadOne(
      d.call_sptr, d.cq_sptr, *response, d.status);
}

template <class Request, class Response>
void ClientReaderWriter<Request, Response>::AsyncReadEach(
    const ReadCallback& on_read,
    const StatusCallback& on_status) const {
    data_sptr_->on_msg = on_read;
    data_sptr_->on_status = on_status;
    ClientReaderHelper::AsyncReadNext(data_sptr_);
}

template <class Request, class Response>
Status ClientReaderWriter<Request, Response>::BlockingFinish() const {
  // XXX
  return Status::OK;
}

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_WRITER_H
