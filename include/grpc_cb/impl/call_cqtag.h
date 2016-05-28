// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_CALL_CQTAG_H
#define GRPC_CB_IMPL_CALL_CQTAG_H

#include <cassert>

#include <grpc_cb/impl/call_sptr.h>             // for CallSptr
#include <grpc_cb/impl/completion_queue_tag.h>  // for CompletionQueueTag

namespace grpc_cb {

// Call completion queue tag.
// Used as base class of other call CqTag which keeps a CallSptr.
class CallCqTag : public CompletionQueueTag {
 public:
  CallCqTag(const CallSptr& call_sptr) : call_sptr_(call_sptr) {
    assert(call_sptr);
  }
  virtual ~CallCqTag() { assert(call_sptr_); }

 private:
  // call_sptr is not used. Just keep it until completed.
  CallSptr call_sptr_;
};

}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_CALL_CQTAG_H
