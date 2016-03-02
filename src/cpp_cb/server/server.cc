// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/server.h>

#include <algorithm>  // for for_each()

#include <grpc/grpc.h>

#include <grpc_cb/completion_queue.h>  // for CompletionQueue
#include <grpc_cb/security/server_credentials.h>  // for InsecureServerCredentials
#include <grpc_cb/service.h>
#include <grpc/grpc_security.h>

#include "server_method_call.h"

namespace grpc_cb {

Server::Server()
    : cq_(new CompletionQueue),  // unique_ptr
      started_(false),
      shutdown_(false),
      server_(CreateServer()) {
  assert(cq_ && server_);
  grpc_server_register_completion_queue(server_.get(), &cq_->cq(), nullptr);
}

Server::~Server() {
  Shutdown();
}

void Server::RegisterService(Service& service) {
  for (size_t i = 0; i < service.GetMethodCount(); ++i) {
    const std::string& name = service.GetMethodName(i);
    void* registered_method = grpc_server_register_method(
      server_.get(), name.c_str(), nullptr);  // TODO: host
    registered_methods_.push_back(registered_method);  // maybe null
  }
}

int Server::AddListeningPort(
    const grpc::string& addr,
    const ServerCredentials& creds) {
  assert(!started_);
  assert(server_);
  grpc_server_credentials* c_creds = creds.creds();
  if (c_creds) {
    return grpc_server_add_secure_http2_port(server_.get(), addr.c_str(), c_creds);
  }
  else {
    return grpc_server_add_insecure_http2_port(server_.get(), addr.c_str());
  }
}

int Server::AddListeningPort(
    const grpc::string& addr) {
  return AddListeningPort(addr, InsecureServerCredentials());
}

void Server::ShutdownInternal(gpr_timespec deadline) {
  if (!started_) return;
  if (shutdown_) return;
  shutdown_ = true;

  assert(cq_);
  grpc_server_shutdown_and_notify(server_.get(), &cq_->cq(), this);
  cq_->Pluck(this);
  cq_->Shutdown();
}

void Server::Run() {
  assert(!started_);
  assert(!shutdown_);
  assert(server_);
  started_ = true;
  grpc_server_start(server_.get());
  RequestMethodsCalls();

  assert(cq_);
  CompletionQueue& cq = *cq_;
  while (true) {
    grpc_event ev = cq.Next();
    switch (ev.type) {
      case GRPC_OP_COMPLETE: {
        break;
      }  // case
      case GRPC_QUEUE_SHUTDOWN:
        return;
      case GRPC_QUEUE_TIMEOUT:
        assert(false);
        break;
      default:
        assert(false);
        break;
    }  // switch
  }
}

void Server::RequestMethodsCalls() {
  const auto& rms = registered_methods_;
  std::for_each(rms.begin(), rms.end(),
    [this](void* rm){ RequestMethodCall(rm); });
}

// registered_method is the return of grpc_server_register_method()
void Server::RequestMethodCall(void* registered_method) {
  if (!registered_method) return;
  MethodCallUptr mcp = CreateMethodCall(registered_method);
  assert(mcp);
  MethodCall& mc = *mcp;
  grpc_server_request_registered_call(
      server_.get(), registered_method,
      &mc.call_ptr(), &mc.deadline(),
      &mc.initial_metadata_array(), &mc.payload_ptr(),
      &mc.GetCompletionQueue().cq(), &cq_->cq(),
      mcp.get());
}

Server::MethodCallUptr
Server::CreateMethodCall(void* registered_method) const {
  assert(registered_method);
  return MethodCallUptr(new MethodCall);  // unique_ptr
}

Server::GrpcServerUptr Server::CreateServer() {
  grpc_channel_args channel_args{0, nullptr};
  grpc_server* svr = grpc_server_create(&channel_args, nullptr);
  return GrpcServerUptr(svr, grpc_server_destroy);
}

}  // namespace grpc_cb
