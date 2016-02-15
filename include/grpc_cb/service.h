// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVICE_H
#define GRPC_CB_SERVICE_H

#include <string>

namespace grpc_cb {

// Service base class.
class Service {
 public:
  virtual const std::string& GetMethodName(size_t i) const = 0;
  virtual size_t GetMethodCount() const = 0;
};

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVICE_H
