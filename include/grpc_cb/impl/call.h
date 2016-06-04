// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_CALL_H
#define GRPC_CB_IMPL_CALL_H

#include <grpc/support/port_platform.h>  // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call_sptr.h>
#include <grpc_cb/support/config.h>         // for GRPC_FINAL
#include <grpc_cb/support/protobuf_fwd.h>   // for Message

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
  explicit Call(grpc_call* c_call);  // own
  ~Call();

 public:
  Status StartBatch(const CallOperations& ops, void* tag) GRPC_MUST_USE_RESULT;

  inline int GetMaxMessageSize() const { return max_message_size_; }
  inline void SetMaxMessageSize(int size) { max_message_size_ = size; }
  static void SetDefaultMaxMessageSize(int size) { default_max_message_size_ = size; }

 public:
     // Todo: return reference? None-null?
  inline grpc_call* c_call() const { return c_call_uptr_.get(); }

 private:
  std::unique_ptr<grpc_call, void (*)(grpc_call*)> c_call_uptr_;  // owned

 private:
  int max_message_size_;  // XXX necessary? Move to Channel?

 private:
  static int default_max_message_size_;  // for all call
};

}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_CALL_H
