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

class Service;

// Used in class Server.
class ServerMethodCallTag GRPC_FINAL : public CompletionQueueTag {
 public:
  // registered_method is the return of grpc_server_register_method().
  ServerMethodCallTag(grpc_server* server, Service* service,
                      size_t method_index, void* registered_method,
                      grpc_completion_queue* cq);

  virtual ~ServerMethodCallTag() GRPC_OVERRIDE {
    grpc_metadata_array_destroy(&initial_metadata_array_);
  }

 public:
  virtual void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  grpc_server* server_;
  Service* service_;
  size_t method_index_;
  void* registered_method_;
  grpc_completion_queue* cq_;

  grpc_call* call_ptr_;
  gpr_timespec deadline_;
  grpc_metadata_array initial_metadata_array_;
  grpc_byte_buffer* payload_ptr_;
};

}  // namespace grpb_cb

#endif  // GRPC_CB_SERVER_SERVER_METHOD_CALL_TAG_H
