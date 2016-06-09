// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_SERVER_WRITER_INIT_CQTAG_H
#define GRPC_CB_SERVER_SERVER_WRITER_INIT_CQTAG_H

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call.h>             // for StartBatch()
#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodSendInitMd
#include <grpc_cb/impl/call_operations.h>  // for CallOperations
#include <grpc_cb/support/config.h>        // for GRPC_FINAL

namespace grpc_cb {

class ServerWriterInitCqTag GRPC_FINAL : public CallCqTag {
 public:
  inline explicit ServerWriterInitCqTag(const CallSptr& call_sptr) : CallCqTag(call_sptr) {}
  inline Status Start() GRPC_MUST_USE_RESULT;

 private:
  CodSendInitMd cod_send_init_md_;
  // Todo: no need? CodRecvInitMd cod_recv_init_md_;
};  // class ServerWriterInitCqTag

Status ServerWriterInitCqTag::Start() {
  CallOperations ops;
  // Todo: Fill send_init_md_array_ -> FillMetadataVector()
  ops.SendInitMd(cod_send_init_md_);
  return GetCallSptr()->StartBatch(ops, this);
}

};  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_SERVER_WRITER_INIT_CQTAG_H
