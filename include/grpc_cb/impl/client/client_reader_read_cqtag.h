// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_READ_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_READER_READ_CQTAG_H

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call_cqtag.h>  // for CallCqTag
#include <grpc_cb/support/config.h>   // for GRPC_FINAL
#include <grpc_cb/impl/call_op_data.h>  // for CodRecvMessage

namespace grpc_cb {

class CallOperations;
class Status;

class ClientReaderReadCqTag GRPC_FINAL : public CallCqTag {
 public:
  ClientReaderReadCqTag(const CallSptr& call_sptr) : CallCqTag(call_sptr) {}
  void InitCallOps(CallOperations& ops);
  Status GetMessage(::google::protobuf::Message& message) GRPC_MUST_USE_RESULT;

  // XXX DoComplete() GRPC_OVERRIDE
 private:
  CodRecvMessage cod_recv_message_;
};  // class ClientReaderReadCqTag

// XXX Implement ClientReaderReadCqTag

};  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_READ_CQTAG_H
