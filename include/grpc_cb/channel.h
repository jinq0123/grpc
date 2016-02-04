// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CHANNEL_H
#define GRPC_CB_CHANNEL_H

#include <string>

#include <grpc_cb/channel_ptr.h>
#include <grpc_cb/impl/grpc_library.h>
#include <grpc_cb/support/config.h>  // for GRPC_OVERRIDE

struct grpc_channel;

namespace grpc_cb {

class Call;

/// Channel represents a connection to an endpoint.
class Channel : public GrpcLibrary,
                public std::enable_shared_from_this<Channel> {
 public:
  explicit Channel(const std::string& target);
  virtual ~Channel() GRPC_OVERRIDE;

 public:
  Call CreateCall(const std::string& method);

 private:
  Channel(const std::string& host, grpc_channel* c_channel);

 private: 
  const std::string host_;
  grpc_channel* const c_channel_;  // owned
};

}  // namespace grpc_cb

#endif  // GRPC_CB_CHANNEL_H
