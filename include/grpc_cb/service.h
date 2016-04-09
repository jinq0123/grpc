// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVICE_H
#define GRPC_CB_SERVICE_H

#include <string>

namespace google {
namespace protobuf {
class Message;
class MethodDescriptor;
class ServiceDescriptor;
}  // namespace protobuf
}  // namespace google

namespace grpc_cb {

// Service base class.
class Service {
 public:
  const std::string& GetFullName() const;
  size_t GetMethodCount() const;

 public:
  virtual const std::string& GetMethodName(size_t method_index) const = 0;
  virtual const google::protobuf::Message& GetRequestPrototype(
      size_t method_index) const = 0;

 private:
  virtual const ::google::protobuf::ServiceDescriptor& GetDescriptor()
      const = 0;
};

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVICE_H
