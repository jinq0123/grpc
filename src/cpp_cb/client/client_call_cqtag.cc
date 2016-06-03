// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/client/client_call_cqtag.h>

#include <grpc_cb/impl/call_operations.h>  // for CallOperations

namespace grpc_cb {

Status ClientCallCqTag::InitCallOps(const ::google::protobuf::Message& request,
                                    CallOperations& ops) {
  Status status = ops.SendMessage(request, cod_send_message_);
  if (!status.ok()) {
    return status;
  }
  // Todo: Fill send_init_md_array_ -> FillMetadataVector()
  ops.SendInitMd(cod_send_init_md_);
  ops.RecvInitMd(cod_recv_init_md_);
  ops.RecvMessage(cod_recv_message_);
  ops.ClientSendClose();
  ops.ClientRecvStatus(cod_client_recv_status_);
  return status;
}

// Todo: inline this

}  // namespace grpc_cb
