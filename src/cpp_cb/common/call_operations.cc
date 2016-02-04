// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "src/cpp_cb/common/call_operations.h"

#include <grpc_cb/support/status.h>

#include "src/cpp_cb/proto/proto_utils.h"

namespace grpc_cb {

CallOperations::CallOperations()
    : send_buf_(nullptr) {
}

CallOperations::~CallOperations() {
  grpc_byte_buffer_destroy(send_buf_);
}

Status CallOperations::SendMessage(
    const protobuf::Message& message) {
  return SerializeProto(message, &send_buf_);
}

}  // namespace grpc_cb
