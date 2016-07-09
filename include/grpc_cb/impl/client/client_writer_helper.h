// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_CLIENT_CLIENT_WRITER__HELPER_H
#define GRPC_CB_IMPL_CLIENT_CLIENT_WRITER__HELPER_H

#include <cassert>     // for assert()

#include <grpc_cb/impl/call_sptr.h>                    // for CallSptr
#include <grpc_cb/impl/client/client_writer_finish_cqtag.h>  // for ClientWriterFinishCqTag
#include <grpc_cb/impl/completion_queue.h>  // for CompletionQueue::Pluck()
#include <grpc_cb/impl/send_msg_cqtag.h>    // for SendMsgCqTag
#include <grpc_cb/status.h>                 // for Status

namespace grpc_cb {
namespace ClientWriterHelper {

// Todo: BlockingGetInitMd();

// Todo: AsyncFinish

inline bool Write(
    const CallSptr& call_sptr,
    const ::google::protobuf::Message& request,
    Status& status) {
  assert(call_sptr);
  if (!status.ok()) return false;

  SendMsgCqTag* tag = new SendMsgCqTag(call_sptr);
  if (tag->Start(request)) return true;

  delete tag;
  status.SetInternalError("Failed to write client stream.");
  return false;
}  // Write()

// Todo: do not inline?
inline Status BlockingFinish(
    const CallSptr& call_sptr,
    const CompletionQueueSptr cq_sptr,
    ::google::protobuf::Message& response,
    Status& status) {
  assert(call_sptr);
  assert(cq_sptr);

  if (!status.ok()) return status;
  ClientWriterFinishCqTag tag(call_sptr);
  if (!tag.Start()) {
    status.SetInternalError("Failed to finish client stream.");
    return status;
  }

  cq_sptr->Pluck(&tag);

  // Todo: Get trailing metadata.
  if (tag.IsStatusOk())
    status = tag.GetResponse(response);
  else
    status = tag.GetStatus();

  return status;
}  // BlockingFinish()

}  // namespace ClientWriterHelper
}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_CLIENT_CLIENT_WRITER__HELPER_H
