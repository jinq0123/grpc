// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/server.h>

#include <grpc/grpc.h>

#include <grpc_cb/completion_queue.h>  // for CompletionQueue

namespace grpc_cb {

Server::Server()
    : cq_(new CompletionQueue),  // unique_ptr
      started_(false),
      shutdown_(false),
      server_(CreateServer()) {
}

Server::~Server() {
}

bool Server::RegisterService() {
  return true;
}

int Server::AddListeningPort(const grpc::string& addr) {
  assert(!started_);
  return 1;
}

void Server::ShutdownInternal(gpr_timespec deadline) {
}

void Server::Run() {
}

Server::GrpcServerUptr Server::CreateServer() {
  grpc_channel_args channel_args{0, nullptr};
  grpc_server* svr = grpc_server_create(&channel_args, nullptr);
  return GrpcServerUptr(svr, grpc_server_destroy);
}

}  // namespace grpc_cb
