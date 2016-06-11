// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_INIT_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_READER_INIT_CQTAG_H

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call.h>             // for StartBatch()
#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodSendInitMd
#include <grpc_cb/impl/call_operations.h>  // for CallOperations
#include <grpc_cb/support/config.h>        // for GRPC_FINAL

namespace grpc_cb {

class ClientReaderInitCqTag GRPC_FINAL : public CallCqTag {
 public:
  inline ClientReaderInitCqTag(const CallSptr& call_sptr) : CallCqTag(call_sptr) {}
  inline bool Start(const ::google::protobuf::Message& request) GRPC_MUST_USE_RESULT;

 private:
  CodSendMsg cod_send_msg_;
  CodSendInitMd cod_send_init_md_;
  CodRecvInitMd cod_recv_init_md_;
};  // class ClientReaderInitCqTag

bool ClientReaderInitCqTag::Start(const ::google::protobuf::Message& request) {
  CallOperations ops;
  Status status = ops.SendMsg(request, cod_send_msg_);
  if (!status.ok()) return false;

  // Todo: Fill send_init_md_array_ -> FillMetadataVector()
  ops.SendInitMd(cod_send_init_md_);
  ops.RecvInitMd(cod_recv_init_md_);
  ops.ClientSendClose();
  return GetCallSptr()->StartBatch(ops, this);
}

};  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_INIT_CQTAG_H
