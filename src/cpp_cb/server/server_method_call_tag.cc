// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "server_method_call_tag.h"

#include <google/protobuf/message.h>
#include <grpc_cb/service.h>
#include <grpc_cb/impl/proto_utils.h>

namespace grpc_cb {

ServerMethodCallTag::ServerMethodCallTag(grpc_server* server, Service* service,
                                         size_t method_index,
                                         void* registered_method,
                                         grpc_completion_queue* cq)
    : server_(server),
      service_(service),
      method_index_(method_index),
      registered_method_(registered_method),
      cq_(cq),
      call_ptr_(nullptr),
      deadline_(gpr_inf_future(GPR_CLOCK_REALTIME)),
      payload_ptr_(nullptr) {
  assert(server);
  assert(registered_method);
  assert(cq);
  assert(method_index < service->GetMethodCount());

  memset(&initial_metadata_array_, 0, sizeof(initial_metadata_array_));

  grpc_server_request_registered_call(server, registered_method, &call_ptr_,
                                      &deadline_, &initial_metadata_array_,
                                      &payload_ptr_, cq, cq, this);
}

void ServerMethodCallTag::DoComplete(bool success)
{
  // TODO: check success

  // Deal payload.
  using Message = ::google::protobuf::Message;
  std::unique_ptr<Message> request;
  assert(service_);
  request.reset(service_->GetRequestPrototype(method_index_).New());
  Status status = SerializationTraits<Message>::Deserialize(
      payload_ptr_, request.get(), 0/* TODO: max_message_size*/);
  if (status.ok()) {
    service_->CallMethod(method_index_, *request);  // XXX replier
  }
  else {
    // XXX reply error
  }

  // Request the next method call.
  // Calls grpc_server_request_registered_call() in ctr().
  // Delete in Server::Run().
  new ServerMethodCallTag(server_, service_, method_index_, registered_method_,
                          cq_);
}

}  // namespace grpc_cb