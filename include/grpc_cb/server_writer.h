// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_SERVER_WRITER_H
#define GRPC_CB_SERVER_SERVER_WRITER_H

#include <grpc_cb/impl/server/server_init_md_cqtag.h>  // for ServerInitMdCqTag

namespace grpc_cb {

template <class Response>
class ServerWriter {
 public:
  inline ServerWriter(const CallSptr& call_sptr);

 public:
  inline bool Write(const Response& response) const;
  inline void Close(const Status& status) const;
  // Todo: Close(data->status) on dtr()
  // Todo: Close() only once.

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  struct Data {
    CallSptr call_sptr;
    Status status;
  };
  std::shared_ptr<Data> data_sptr_;  // Easy to copy.
};  // class ServerWriter<>

template <class Response>
ServerWriter<Response>::ServerWriter(const CallSptr& call_sptr)
    : data_sptr_(new Data{call_sptr}) {
  assert(call_sptr);
  ServerInitMdCqTag* tag = new ServerInitMdCqTag(call_sptr);
  // Todo: Set init md
  if (tag->Start()) return;

  delete tag;
  data_sptr_->status.SetInternalError("Failed to init server stream.");
}

template <class Response>
bool ServerWriter<Response>::Write(const Response& response) const {
  Status& status = data_sptr_->status;
  if (!status.ok()) return false;
  SendMsgCqTag* tag = new SendMsgCqTag(data_sptr_->call_sptr);
  if (tag->Start(response)) return true;

  delete tag;
  status.SetInternalError("Failed to write server stream.");
  return false;
}

// Todo: no write if already closed.

template <class Response>
void ServerWriter<Response>::Close(const Status& status) const {
  // XXX
}

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_SERVER_WRITER_H
