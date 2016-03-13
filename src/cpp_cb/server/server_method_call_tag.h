// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_SERVER_METHOD_CALL_TAG_H
#define GRPC_CB_SERVER_SERVER_METHOD_CALL_TAG_H

#include <cassert>

#include <grpc/grpc.h>          // for grpc_metadata_array
#include <grpc/support/time.h>  // for gpr_timespec

#include <grpc_cb/completion_queue_tag.h>  // for CompletionQueueTag
#include <grpc_cb/support/config.h>        // for GRPC_FINAL

struct grpc_call;

namespace grpc_cb {

// Used in class Server.
class ServerMethodCallTag GRPC_FINAL : public CompletionQueueTag {
 public:
  ServerMethodCallTag()
      : call_ptr_(nullptr),
        deadline_(gpr_inf_future(GPR_CLOCK_REALTIME)),
        payload_ptr_(nullptr) {
    memset(&initial_metadata_array_, 0, sizeof(initial_metadata_array_));
  }

  virtual ~ServerMethodCallTag() GRPC_OVERRIDE {
    grpc_metadata_array_destroy(&initial_metadata_array_);
  }

 public:
  grpc_call*& call_ptr() { return call_ptr_; }
  gpr_timespec& deadline() { return deadline_; }
  grpc_metadata_array& initial_metadata_array() {
    return initial_metadata_array_;
  }
  grpc_byte_buffer*& payload_ptr() { return payload_ptr_; }

 public:
  virtual void DoComplete(bool success) GRPC_OVERRIDE{
      // XXX
  };

 private:
  grpc_call* call_ptr_;
  gpr_timespec deadline_;
  grpc_metadata_array initial_metadata_array_;
  grpc_byte_buffer* payload_ptr_;
};

}  // namespace grpb_cb

#endif  // GRPC_CB_SERVER_SERVER_METHOD_CALL_TAG_H
