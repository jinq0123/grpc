// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_CLIENT_CLIENT_WRITER__HELPER_H
#define GRPC_CB_IMPL_CLIENT_CLIENT_WRITER__HELPER_H

#include <cassert>     // for assert()

#include <grpc_cb/impl/call_sptr.h>                    // for CallSptr
#include <grpc_cb/impl/client/client_send_msg_cqtag.h>    // for ClientSendMsgCqTag
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

  ClientSendMsgCqTag* tag = new ClientSendMsgCqTag(call_sptr);
  if (tag->Start(request)) return true;

  delete tag;
  status.SetInternalError("Failed to write client stream.");
  return false;
}  // Write()

}  // namespace ClientWriterHelper
}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_CLIENT_CLIENT_WRITER__HELPER_H
