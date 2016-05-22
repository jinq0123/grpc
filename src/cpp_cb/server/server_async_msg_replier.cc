// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/server_async_msg_replier.h>

#include <google/protobuf/message.h>
#include <grpc/grpc.h>                     // for grpc_op
#include <grpc/support/log.h>              // for GPR_ASSERT()
#include <grpc_cb/completion_queue_tag.h>  // for CompletionQueueTag
#include <grpc_cb/impl/call.h>             // for Call::call()
#include <grpc_cb/impl/proto_utils.h>      // for SerializeProto()
#include <grpc_cb/service.h>

namespace grpc_cb {

class ReplyTag GRPC_FINAL : public CompletionQueueTag {
 public:
  inline ReplyTag(const CallSptr& call_sptr, const ::google::protobuf::Message& msg);
  inline ReplyTag(const CallSptr& call_sptr, const Status& status);

  inline virtual ~ReplyTag() GRPC_OVERRIDE;

 public:
  inline size_t GetOpsNum() const { return cops_.size(); }
  inline const grpc_op* GetOps() const {
    return cops_.empty() ? nullptr : &cops_[0];
  }

 public:
  virtual void DoComplete(bool success) GRPC_OVERRIDE {};

 private:
  inline explicit ReplyTag(const CallSptr& call_sptr);

  void SendInitialMetadata();
  Status SendMessage(const ::google::protobuf::Message& msg);
  void ServerSendStatus(const Status& status);

 private:
  std::vector<grpc_op> cops_;

 private:
  grpc_byte_buffer* send_buf_;
  std::string send_status_details_;
  CallSptr call_sptr_;
};

ReplyTag::ReplyTag(const CallSptr& call_sptr,
                   const ::google::protobuf::Message& msg)
    : ReplyTag(call_sptr) {
  SendInitialMetadata();
  Status status = SendMessage(msg);
  ServerSendStatus(status);
}

ReplyTag::ReplyTag(const CallSptr& call_sptr, const Status& status) : ReplyTag(call_sptr) {
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
  ReplyTag* tag = new ReplyTag(call_sptr_, msg);  // Delete in Run().
  StartBatch(tag);
}

void ServerAsyncMsgReplier::ReplyError(const Status& status) {
  ReplyTag* tag = new ReplyTag(call_sptr_, status);  // Delete in Run().
  StartBatch(tag);
}

void ServerAsyncMsgReplier::StartBatch(ReplyTag* tag) {
  assert(tag);
  assert(call_sptr_);
  // XXX Status result = call_sptr_->StartBatch(tag);
  grpc_call_error result = grpc_call_start_batch(
    call_sptr_->call(), tag->GetOps(), tag->GetOpsNum(), tag, nullptr);
  GPR_ASSERT(GRPC_CALL_OK == result);
}

}  // namespace grpc_cb
