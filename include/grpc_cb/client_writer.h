// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_WRITER_H
#define GRPC_CB_CLIENT_WRITER_H

#include <cassert>     // for assert()

#include <grpc_cb/channel.h>                           // for MakeSharedCall()
#include <grpc_cb/impl/call_sptr.h>                    // for CallSptr
#include <grpc_cb/impl/client/client_init_md_cqtag.h>  // for ClientInitMdCqTag
#include <grpc_cb/impl/client/client_writer_helper.h>  // for ClientWriterHelper
#include <grpc_cb/status.h>                            // for Status

namespace grpc_cb {

// Use template class instead of template member function
//    to ensure client input the correct request type.
// Todo: Use non_template class as the implement.
template <class Request>
class ClientWriter GRPC_FINAL {
 public:
  inline ClientWriter(const ChannelSptr& channel, const std::string& method,
                      const CompletionQueueSptr& cq_sptr);

  // Todo: BlockingGetInitMd();
  bool Write(const Request& request) const {
    Data& d = *data_sptr_;
    return ClientWriterHelper::Write(d.call_sptr, request, d.status);
  }

  Status BlockingFinish(
      ::google::protobuf::Message* response) const {
    assert(response);
    Data& d = *data_sptr_;
    return ClientWriterHelper::BlockingFinish(
        d.call_sptr, d.cq_sptr, *response, d.status);
  }
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

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_WRITER_H
