// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef CPP_CB_SERVER_SERVER_REPLIER_CQTAT_H
#define CPP_CB_SERVER_SERVER_REPLIER_CQTAT_H

#include <grpc_cb/impl/call.h>             // for Call
#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodSendInitMd
#include <grpc_cb/impl/call_operations.h>  // for CallOperations
#include <grpc_cb/impl/call_sptr.h>        // for CallSptr
#include <grpc_cb/support/config.h>        // for GRPC_FINAL
#include <grpc_cb/support/protobuf_fwd.h>  // for Message

namespace grpc_cb {

class ServerReplierCqTag GRPC_FINAL : public CallCqTag {
 public:
  explicit ServerReplierCqTag(const CallSptr& call_sptr) : CallCqTag(call_sptr) {}
  virtual ~ServerReplierCqTag() {}

  inline void StartReply(const ::google::protobuf::Message& msg);
  inline void StartReplyError(const Status& status);

 private:
  CodSendInitMd cod_send_init_md_;
  CodSendMessage cod_send_message_;
  CodServerSendStatus cod_server_send_status_;
};

void ServerReplierCqTag::StartReply(const ::google::protobuf::Message& msg) {
  CallOperations ops;
  ops.SendInitMd(cod_send_init_md_);  // Todo: init metadata
  Status status = ops.SendMessage(msg, cod_send_message_);
  ops.ServerSendStatus(status, cod_server_send_status_);
  GetCallSptr()->StartBatch(ops, this);
}

void ServerReplierCqTag::StartReplyError(const Status& status) {
  CallOperations ops;
  ops.SendInitMd(cod_send_init_md_);
  ops.ServerSendStatus(status, cod_server_send_status_);
  GetCallSptr()->StartBatch(ops, this);
}

//  SendInitialMetadata();
//  Status status = SendMessage(msg);
//  ServerSendStatus(status);
//}

//ReplyTag::ReplyTag(const CallSptr& call_sptr, const Status& status)
//    : CallCqTag(call_sptr) {
//  SendInitialMetadata();
//  ServerSendStatus(status);
//}

//void ReplyTag::SendInitialMetadata() {
//  grpc_op op{GRPC_OP_SEND_INITIAL_METADATA, 0, 0};
//  op.data.send_initial_metadata.count = 0;
//  op.data.send_initial_metadata.metadata = nullptr;
//  cops_.push_back(op);
//}
//
//Status ReplyTag::SendMessage(const ::google::protobuf::Message& msg) {
//  Status status = SerializeProto(msg, &send_buf_);
//  if (!status.ok()) return status;
//  if (send_buf_ == nullptr) return status;
//  grpc_op op = {GRPC_OP_SEND_MESSAGE, 0, 0, 0};
//  op.data.send_message = send_buf_;
//  cops_.push_back(op);
//  return status;
//}
//
//void ReplyTag::ServerSendStatus(const Status& status) {
//  grpc_op op{GRPC_OP_SEND_STATUS_FROM_SERVER, 0, 0};
//  op.data.send_status_from_server.trailing_metadata_count = 0;
//  op.data.send_status_from_server.trailing_metadata = nullptr;
//  op.data.send_status_from_server.status = status.error_code();
//  send_status_details_ = status.error_message();
//  op.data.send_status_from_server.status_details =
//      send_status_details_.empty() ? nullptr : send_status_details_.c_str();
//  cops_.push_back(op);
//}

}  // namespace grpc_cb
#endif  // CPP_CB_SERVER_SERVER_REPLIER_CQTAT_H
