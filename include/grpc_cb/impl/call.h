// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_CALL_H
#define GRPC_CB_IMPL_CALL_H

#include <grpc_cb/support/config.h>  // for GRPC_FINAL
#include <grpc_cb/support/config_protobuf.h>  // for Message

struct grpc_byte_buffer;
struct grpc_call;

namespace grpc_cb {

class Status;
class CallOperations;

// Straightforward wrapping of the C call object.
// Differ from grpc++:
//  grpc_call is owned by Call instead of ClientContext/ServerContext.
class Call GRPC_FINAL {
 public:
  // Created by Channel::CreateCall().
  explicit Call(grpc_call* c_call);
  ~Call();

 public:
  Status StartBatch(const protobuf::Message& request, void* tag);
  Status GetResponse(protobuf::Message* response) const;

  inline int GetMaxMessageSize() const { return max_message_size_; }
  inline void SetMaxMessageSize(int size) { max_message_size_ = size; }
  static void SetDefaultMaxMessageSize(int size) { default_max_message_size_ = size; }

 public:
  inline grpc_call* call() const { return call_; }

 private:
  grpc_call* const call_;  // owned

 private:
  std::shared_ptr<CallOperations> ops_;
  grpc_byte_buffer* recv_buf_;
  int max_message_size_;

private:
  static int default_max_message_size_;  // for all call
};

}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_CALL_H
