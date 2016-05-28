// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_INIT_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_READER_INIT_CQTAG_H

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/support/config.h>        // for GRPC_FINAL
#include <grpc_cb/completion_queue_tag.h>  // for CompletionQueueTag

namespace grpc_cb {

class CallOperations;
class Status;

class ClientReaderInitCqTag GRPC_FINAL : public CompletionQueueTag {
public:
  Status InitCallOps(CallOperations& ops) GRPC_MUST_USE_RESULT;
};  // class ClientReaderInitCqTag

};  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_INIT_CQTAG_H
