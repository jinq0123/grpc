// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVICE_H
#define GRPC_CB_SERVICE_H

#include <string>
#include <grpc_cb/support/protobuf_fwd.h>

namespace grpc_cb {

// Service base class.
class Service {
 public:
  const std::string& GetFullName() const;
  size_t GetMethodCount() const;

 public:
  virtual const std::string& GetMethodName(size_t method_index) const = 0;
  virtual const ::google::protobuf::Message& GetRequestPrototype(
      size_t method_index) const = 0;

  // TODO: need request_context. Need client address in Ctr?
  virtual void CallMethod(size_t method_index,
                          const ::google::protobuf::Message& request) = 0;

 private:
  virtual const ::google::protobuf::ServiceDescriptor& GetDescriptor()
      const = 0;
};

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVICE_H
