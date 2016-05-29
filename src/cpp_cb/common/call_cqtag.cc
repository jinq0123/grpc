// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CPP_CB_COMMON_CALL_CQTAG_H
#define GRPC_CB_CPP_CB_COMMON_CALL_CQTAG_H

#include <grpc_cb/impl/call_cqtag.h>

#include <grpc_cb/impl/call.h>  // for Call

namespace grpc_cb {

CallCqTag::~CallCqTag() {
	assert(call_sptr_);
}

}  // namespace grpc_cb

#endif  // GRPC_CB_CPP_CB_COMMON_CALL_CQTAG_H
