// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/client/client_reader_init_cqtag.h>

#include <grpc_cb/impl/call_operations.h>  // for CallOperations

namespace grpc_cb {

Status InitCallOps(CallOperations& ops) { return Status::OK; }

}  // namespace grpc_cb
