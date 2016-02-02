// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_CALL_H
#define GRPC_CB_IMPL_CALL_H

struct grpc_call;

namespace grpc_cb {

// Straightforward wrapping of the C call object.
// Differ from grpc++:
//  grpc_call is owned by Call instead of ClientContext/ServerContext.
class Call {
 public:
  // Created by Channel::CreateCall().
  explicit Call(grpc_call* c_call);
  virtual ~Call();

 public: 
  inline grpc_call* call() const { return call_; }

 private:
  grpc_call* const call_;  // owned
};

}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_CALL_H
