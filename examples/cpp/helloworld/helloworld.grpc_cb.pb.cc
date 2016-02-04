// Generated by the gRPC protobuf plugin.
// If you make any local change, they will be lost.
// source: helloworld.proto

#include "helloworld.pb.h"
#include "helloworld.grpc_cb.pb.h"

#include <grpc_cb/channel.h>
#include <grpc_cb/completion_queue.h>
#include <grpc_cb/impl/call.h>

namespace helloworld {

// static const char* Greeter_method_names[] = {
//  "/helloworld.Greeter/SayHello",
//};

std::unique_ptr< Greeter::Stub> Greeter::NewStub(const ::grpc_cb::ChannelPtr& channel) {
  std::unique_ptr< Greeter::Stub> stub(new Greeter::Stub(channel));
  return stub;
}

Greeter::Stub::Stub(const ::grpc_cb::ChannelPtr& channel)
  : ::grpc_cb::ServiceStub(channel)
    // , rpcmethod_SayHello_(Greeter_method_names[0], ::grpc_cb::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc_cb::Status Greeter::Stub::SayHello(
    const ::helloworld::HelloRequest& request,
    ::helloworld::HelloReply* response) {
  grpc_cb::CompletionQueue cq;
  grpc_cb::Call call(channel_->CreateCall("SayHello"));
  grpc_cb::Status status = call.StartBatch(request);
  if (!status.ok()) return status;
  cq.Pluck(1234);
  return status;
}

// Greeter::AsyncService::AsyncService() : ::grpc_cb::AsynchronousService(Greeter_method_names, 1) {}

Greeter::Service::Service() {
}

Greeter::Service::~Service() {
}

::grpc_cb::Status Greeter::Service::SayHello(const ::helloworld::HelloRequest& request, ::helloworld::HelloReply* response) {
  (void) request;
  (void) response;
  return ::grpc_cb::Status::UNIMPLEMENTED;
}


}  // namespace helloworld

