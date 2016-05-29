// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef CPP_CB_SERVER_SERVER_REPLIER_CQTAT_H
#define CPP_CB_SERVER_SERVER_REPLIER_CQTAT_H

#include <grpc_cb/impl/call_cqtag.h>  // for CallCqTag
#include <grpc_cb/impl/call_sptr.h>   // for CallSptr
#include <grpc_cb/support/config.h>   // for GRPC_FINAL

namespace grpc_cb {

class ServerReplierCqTag GRPC_FINAL : public CallCqTag {
 public:
  inline ServerReplierCqTag(const CallSptr& call_sptr, const ::google::protobuf::Message& msg);
  inline ServerReplierCqTag(const CallSptr& call_sptr, const Status& status);

  inline virtual ~ServerReplierCqTag() GRPC_OVERRIDE;

 public:
  inline size_t GetOpsNum() const { return cops_.size(); }
  inline const grpc_op* GetOps() const {
    return cops_.empty() ? nullptr : &cops_[0];
  }

 private:
  // Todo: Use CallOperations instead.
  void SendInitialMetadata();
  Status SendMessage(const ::google::protobuf::Message& msg);
  void ServerSendStatus(const Status& status);

 private:
  grpc_byte_buffer* send_buf_;
  std::string send_status_details_;

  // XXX Use COD member variables instead of such things as grpc_byte_buffer.
};

ReplyTag::ReplyTag(const CallSptr& call_sptr,
                   const ::google::protobuf::Message& msg)
    : CallCqTag(call_sptr) {
  SendInitialMetadata();
  Status status = SendMessage(msg);
  ServerSendStatus(status);
}

ReplyTag::ReplyTag(const CallSptr& call_sptr, const Status& status)
    : CallCqTag(call_sptr) {
  SendInitialMetadata();
  ServerSendStatus(status);
}

ReplyTag::~ReplyTag() {
  assert(call_sptr_);  // auto destroy by shared_ptr
  grpc_byte_buffer_destroy(send_buf_);
}

ReplyTag::ReplyTag(const CallSptr& call_sptr) : send_buf_(nullptr), call_sptr_(call_sptr) {
  assert(call_sptr);
}

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
