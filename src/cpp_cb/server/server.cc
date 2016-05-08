// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/server.h>

#include <algorithm>  // for for_each()

#include <grpc/grpc.h>
#include <grpc/grpc_security.h>  // for grpc_server_add_secure_http2_port()
#include <grpc/support/log.h>    // for GPR_ASSERT()

#include <grpc_cb/completion_queue.h>             // for CompletionQueue
#include <grpc_cb/security/server_credentials.h>  // for InsecureServerCredentials
#include <grpc_cb/service.h>

#include "server_method_call_tag.h"

namespace grpc_cb {

Server::Server()
    : cq_(new CompletionQueue),  // unique_ptr
      started_(false),
      shutdown_(false),
      server_(CreateServer()) {
  assert(cq_ && server_);
  grpc_server_register_completion_queue(server_.get(), &cq_->cq(), nullptr);
}

Server::~Server() { Shutdown(); }

void Server::RegisterService(Service& service) {
  RegisteredService& rs = service_map_[service.GetFullName()];
  rs.service = &service;
  RegisteredMethodVec& registered_methods = rs.registered_methods;
  registered_methods.clear();

  for (size_t i = 0; i < service.GetMethodCount(); ++i) {
    const std::string& name = service.GetMethodName(i);
    void* registered_method = grpc_server_register_method(
        server_.get(), name.c_str(), nullptr);         // TODO: host
    registered_methods.push_back(registered_method);  // maybe null
  }
}

int Server::AddListeningPort(const grpc::string& addr,
                             const ServerCredentials& creds) {
  assert(!started_);
  assert(server_);
  grpc_server_credentials* c_creds = creds.creds();
  if (c_creds) {
    return grpc_server_add_secure_http2_port(server_.get(), addr.c_str(),
                                             c_creds);
  } else {
    return grpc_server_add_insecure_http2_port(server_.get(), addr.c_str());
  }
}

int Server::AddListeningPort(const grpc::string& addr) {
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

void Server::BlockingRun() {
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
        GPR_ASSERT(ev.success);
        auto* tag = static_cast<CompletionQueueTag*>(ev.tag);
        assert(tag);
        tag->DoComplete(0 != ev.success);
        delete tag;  // created in RequestMethodCall()
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

void Server::RequestMethodsCalls() const {
  for (auto itr = service_map_.begin(); itr != service_map_.end(); ++itr) {
    const RegisteredService& rs = (*itr).second;
    RequestServiceMethodsCalls(rs);
  }
}

void Server::RequestServiceMethodsCalls(const RegisteredService& rs) const {
  assert(rs.service);
  const RegisteredMethodVec& rms = rs.registered_methods;
  for (size_t i = 0; i < rms.size(); ++i) {
    if (!rms[i]) continue;
    // Delete in Run(). Calls grpc_server_request_registered_call() in ctr().
    new ServerMethodCallTag(server_.get(), rs.service, i, rms[i], &cq_->cq());
  }
}

Server::GrpcServerUptr Server::CreateServer() {
  grpc_channel_args channel_args{0, nullptr};
  grpc_server* svr = grpc_server_create(&channel_args, nullptr);
  return GrpcServerUptr(svr, grpc_server_destroy);
}

}  // namespace grpc_cb