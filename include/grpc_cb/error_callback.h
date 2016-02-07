// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_ERROR_CALLBACK_H
#define GRPC_CB_ERROR_CALLBACK_H

#include <functional>  // for std::function()

namespace grpc_cb {

class Status;
typedef std::function<void (const Status& error_status)> ErrorCallback;

}  // namespace grpc_cb

#endif  // GRPC_CB_ERROR_CALLBACK_H
