// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CHANNEL_H
#define GRPC_CB_CHANNEL_H

#include <memory>  // for unique_ptr<>
#include <string>

#include <grpc_cb/channel_sptr.h>
#include <grpc_cb/impl/call_uptr.h>  // for CallUptr
#include <grpc_cb/impl/grpc_library.h>
#include <grpc_cb/support/config.h>  // for GRPC_OVERRIDE


struct grpc_channel;
struct grpc_completion_queue;

namespace grpc_cb {

/// Channel represents a connection to an endpoint.
class Channel : public GrpcLibrary,
                public std::enable_shared_from_this<Channel> {
 public:
  explicit Channel(const std::string& target);
  virtual ~Channel() GRPC_OVERRIDE;

 public:
  CallUptr CreateCall(
    const std::string& method,
    grpc_completion_queue& cp);

 private:
  Channel(const std::string& host, grpc_channel* c_channel);

 private: 
  const std::string host_;
  grpc_channel* const c_channel_;  // owned
};

}  // namespace grpc_cb

#endif  // GRPC_CB_CHANNEL_H
