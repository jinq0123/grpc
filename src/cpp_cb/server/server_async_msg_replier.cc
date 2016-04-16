// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/server_async_msg_replier.h>

#include <google/protobuf/message.h>
#include <grpc/grpc.h>                     // for grpc_op
#include <grpc/support/log.h>              // for GPR_ASSERT()
#include <grpc_cb/completion_queue_tag.h>  // for CompletionQueueTag
#include <grpc_cb/service.h>

#include "src/cpp_cb/proto/proto_utils.h"  // for SerializeProto()

namespace grpc_cb {

class ReplyTag GRPC_FINAL : public CompletionQueueTag {
 public:
  explicit ReplyTag(const ::google::protobuf::Message& msg);
  explicit ReplyTag(const Status& status);

  virtual ~ReplyTag() GRPC_OVERRIDE;

 public:
  inline size_t GetOpsNum() const { return cops_.size(); }
  inline const grpc_op* GetOps() const {
    return cops_.empty() ? nullptr : &cops_[0];
  }

 public:
  virtual void DoComplete(bool success) GRPC_OVERRIDE {};

 private:
  ReplyTag();

  void SendInitialMetadata();
  Status SendMessage(const ::google::protobuf::Message& msg);
  void ServerSendStatus(const Status& status);

 private:
  std::vector<grpc_op> cops_;

 private:
  grpc_byte_buffer* send_buf_;
  std::string send_status_details_;
};

ReplyTag::ReplyTag(const ::google::protobuf::Message& msg) : ReplyTag() {
  SendInitialMetadata();
  Status status = SendMessage(msg);
  ServerSendStatus(status);
}

ReplyTag::ReplyTag(const Status& status) : ReplyTag() {
  SendInitialMetadata();
  ServerSendStatus(status);
}

ReplyTag::~ReplyTag() {
  grpc_byte_buffer_destroy(send_buf_);
}

ReplyTag::ReplyTag() : send_buf_(nullptr) {
}

void ReplyTag::SendInitialMetadata() {
  grpc_op op{GRPC_OP_SEND_INITIAL_METADATA, 0, 0};
  op.data.send_initial_metadata.count = 0;
  op.data.send_initial_metadata.metadata = nullptr;
  cops_.push_back(op);
}

Status ReplyTag::SendMessage(const ::google::protobuf::Message& msg) {
  Status status = SerializeProto(msg, &send_buf_);
  if (!status.ok()) return status;
  if (send_buf_ == nullptr) return status;
  grpc_op op = {GRPC_OP_SEND_MESSAGE, 0, 0, 0};
  op.data.send_message = send_buf_;
  cops_.push_back(op);
  return status;
}

void ReplyTag::ServerSendStatus(const Status& status) {
  grpc_op op{GRPC_OP_SEND_STATUS_FROM_SERVER, 0, 0};
  op.data.send_status_from_server.trailing_metadata_count = 0;
  op.data.send_status_from_server.trailing_metadata = nullptr;
  op.data.send_status_from_server.status = status.error_code();
  send_status_details_ = status.error_message();
  op.data.send_status_from_server.status_details =
      send_status_details_.empty() ? nullptr : send_status_details_.c_str();
  cops_.push_back(op);
}

void ServerAsyncMsgReplier::Reply(const ::google::protobuf::Message& msg) {
  ReplyTag* tag = new ReplyTag(msg);  // Delete in Run().
  StartBatch(tag);
}

void ServerAsyncMsgReplier::ReplyError(const Status& status) {
  ReplyTag* tag = new ReplyTag(status);  // Delete in Run().
  StartBatch(tag);
}

void ServerAsyncMsgReplier::StartBatch(ReplyTag* tag) {
  assert(tag);
  grpc_call_error result = grpc_call_start_batch(
    call_, tag->GetOps(), tag->GetOpsNum(), tag, nullptr);
  GPR_ASSERT(GRPC_CALL_OK == result);
}

}  // namespace grpc_cb
