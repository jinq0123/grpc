// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/client/client_call_cqtag.h>

#include <grpc_cb/impl/call_operations.h>  // for CallOperations

namespace grpc_cb {

// XXX MOve ops to the last.
Status ClientCallCqTag::InitCallOps(const ::google::protobuf::Message& request,
                                    CallOperations& ops) {
  Status status = ops.SendMessage(request, cod_send_message_);
  if (!status.ok()) {
    return status;
  }
  // Todo: Fill send_init_md_array_ -> FillMetadataVector()
  ops.SendInitMd(cod_send_init_md_);
  ops.RecvInitMd(&recv_init_detadata_);
  ops.RecvMessage(&recv_buf_);
  ops.ClientSendClose();
  ops.ClientRecvStatus(&recv_trailing_metadata_arr_,
      &status_code_, &status_details_, &status_details_capacity_);
  return status;
}

}  // namespace grpc_cb
