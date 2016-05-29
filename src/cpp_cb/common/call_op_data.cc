// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/call_op_data.h>

// Tood: Move proto_utils.h to src dir.
#include <grpc_cb/impl/proto_utils.h>  // for SerializeProto()
#include <grpc_cb/status.h>            // for Status

namespace grpc_cb {

Status CodSendMessage::SerializeMessage(
    const ::google::protobuf::Message& message) {
  // send_buf_ is created here and destroyed in dtr().
  return SerializeProto(message, &send_buf_);

}

}  // namespace grpc_cb
