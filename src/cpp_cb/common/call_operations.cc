// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "src/cpp_cb/common/call_operations.h"

namespace grpc_cb {

CallOperations::CallOperations()
    : send_buf_(nullptr) {
}

CallOperations::~CallOperations() {
  grpc_byte_buffer_destroy(send_buf_);
}

}  // namespace grpc_cb
