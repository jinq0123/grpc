// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_WRITER_H
#define GRPC_CB_CLIENT_CLIENT_WRITER_H

#include <cassert>     // for assert()
#include <functional>  // for std::function

#include <grpc_cb/channel.h>                           // for MakeSharedCall()
#include <grpc_cb/impl/call.h>                         // for StartBatch()
#include <grpc_cb/impl/call_sptr.h>                    // for CallSptr
#include <grpc_cb/impl/client/client_init_md_cqtag.h>  // for ClientInitMdCqTag
#include <grpc_cb/impl/client/client_writer_finish_cqtag.h>  // for ClientWriterFinishCqTag
#include <grpc_cb/impl/completion_queue.h>  // for CompletionQueue::Pluck()
#include <grpc_cb/impl/send_msg_cqtag.h>    // for SendMsgCqTag
#include <grpc_cb/status.h>                 // for Status

namespace grpc_cb {

// Use template class instead of template member function
//    to ensure client input the correct request type.
// Todo: Use non_template class as the implement.
template <class Request>
class ClientWriter {
 public:
  inline ClientWriter(const ChannelSptr& channel, const std::string& method,
                      const CompletionQueueSptr& cq_sptr);

  // Todo: BlockingGetInitMd();
  bool Write(const Request& request) const;
  inline Status BlockingFinish(
      ::google::protobuf::Message* response) const;
  // Todo: AsyncFinish

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  struct Data {
    CompletionQueueSptr cq_sptr;
    CallSptr call_sptr;
    Status status;
  };
  std::shared_ptr<Data> data_sptr_;  // Easy to copy.
};  // class ClientWriter<>

template <class Request>
ClientWriter<Request>::ClientWriter(const ChannelSptr& channel,
                                    const std::string& method,
                                    const CompletionQueueSptr& cq_sptr)
    // Todo: same as ClientReader?
    : data_sptr_(new Data{cq_sptr, channel->MakeSharedCall(method, *cq_sptr)}) {
  assert(cq_sptr);
  assert(channel);
  assert(data_sptr_->call_sptr);
  ClientInitMdCqTag* tag = new ClientInitMdCqTag(data_sptr_->call_sptr);
  if (tag->Start()) return;
  delete tag;
  data_sptr_->status.SetInternalError("Failed to init client stream.");
}

template <class Request>
bool ClientWriter<Request>::Write(const Request& request) const {
  assert(data_sptr_);
  assert(data_sptr_->call_sptr);
  Status& status = data_sptr_->status;
  if (!status.ok()) return false;

  SendMsgCqTag* tag = new SendMsgCqTag(data_sptr_->call_sptr);
  if (tag->Start(request)) return true;

  delete tag;
  status.SetInternalError("Failed to write client stream.");
  return false;
}

template <class Request>
Status ClientWriter<Request>::BlockingFinish(
    ::google::protobuf::Message* response) const {
  assert(data_sptr_);
  assert(data_sptr_->call_sptr);
  Status& status = data_sptr_->status;
  if (!status.ok()) return status;
  ClientWriterFinishCqTag tag(data_sptr_->call_sptr);
  if (!tag.Start()) {
    status.SetInternalError("Failed to finish client stream.");
    return status;
  }

  data_sptr_->cq_sptr->Pluck(&tag);

  // Todo: Get trailing metadata.
  if (tag.IsStatusOk())
    status = tag.GetResponse(*response);
  else
    status = tag.GetStatus();

  return status;
}

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_WRITER_H
