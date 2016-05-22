// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_UNARY_CALL_TAG_H
#define GRPC_CB_CLIENT_UNARY_CALL_TAG_H

#include <grpc_cb/completion_queue_tag.h>

namespace grpc_cb {

// Completion queue tag for unary call. For both blocking and async calls.
// Rename to ClientUnaryCallCqTag.
// Async call should derive from it. Need cb, err_cb.
class ClientUnaryCallTag : public CompletionQueueTag {
 public:
  // XXX
};  // class ClientUnaryCallTag

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_UNARY_CALL_TAG_H
