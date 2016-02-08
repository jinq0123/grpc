// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_CALL_UPTR_H
#define GRPC_CB_IMPL_CALL_UPTR_H

#include <memory>

namespace grpc_cb {

class Call;
typedef std::unique_ptr<Call> CallUptr;

}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_CALL_UPTR_H
