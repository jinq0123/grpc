// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_SERVER_WRITER_H
#define GRPC_CB_SERVER_SERVER_WRITER_H

#include <grpc_cb/impl/server/server_init_md_cqtag.h>  // for ServerInitMdCqTag

namespace grpc_cb {

template <class Response>
class ServerWriter GRPC_FINAL {
 public:
  inline ServerWriter(const CallSptr& call_sptr);
  inline ~ServerWriter() { Close(Status::OK); }

 public:
  inline bool Write(const Response& response) const;
  // Close() is optional. Dtr() will auto Close().
  // Redundent Close() will be ignored.
  inline void Close(const Status& status) const;
  inline bool IsClosed() const { return data_sptr_->closed; }

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  struct Data {
    CallSptr call_sptr;
    bool closed = false;
  };
  std::shared_ptr<Data> data_sptr_;  // Easy to copy.
};  // class ServerWriter<>

template <class Response>
ServerWriter<Response>::ServerWriter(const CallSptr& call_sptr)
    : data_sptr_(new Data{call_sptr}) {
  assert(call_sptr);
  assert(!data_sptr_->closed);
  ServerInitMdCqTag* tag = new ServerInitMdCqTag(call_sptr);
  // Todo: Set init md
  if (tag->Start()) return;

  delete tag;
  data_sptr_->closed = true;  // error
}

template <class Response>
bool ServerWriter<Response>::Write(const Response& response) const {
  bool& closed = data_sptr_->closed;
  if (closed) return false;
  SendMsgCqTag* tag = new SendMsgCqTag(data_sptr_->call_sptr);
  if (tag->Start(response)) return true;

  delete tag;
  closed = true;  // error
  return false;
}

template <class Response>
void ServerWriter<Response>::Close(const Status& status) const {
  bool& closed = data_sptr_->closed;
  if (closed) return false;
  closed = true;

  // XXX
}

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_SERVER_WRITER_H
