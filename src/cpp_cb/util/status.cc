// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/support/status.h>

namespace grpc_cb {

const Status& Status::OK = Status();
const Status& Status::CANCELLED = Status(GRPC_STATUS_CANCELLED, "");
const Status& Status::UNIMPLEMENTED = Status(GRPC_STATUS_UNIMPLEMENTED, "");

Status Status::InternalError(const std::string& details) {
  return Status(GRPC_STATUS_INTERNAL, details);
}

}  // namespace grpc_cb
