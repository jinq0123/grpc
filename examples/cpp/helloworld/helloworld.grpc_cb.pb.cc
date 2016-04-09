// Generated by the gRPC protobuf plugin.
// If you make any local change, they will be lost.
// source: helloworld.proto

#include "helloworld.grpc_cb.pb.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/once.h>

#include <grpc_cb/channel.h>
#include <grpc_cb/completion_queue.h>
#include <grpc_cb/impl/call.h>

namespace helloworld {

namespace {

const ::google::protobuf::ServiceDescriptor* service_descriptor_Greeter = nullptr;

void AssignDesc_helloworld_2eproto() {
  // Get the file's descriptor from the pool.
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "helloworld.proto");
  GOOGLE_CHECK(file != NULL);
  service_descriptor_Greeter = file->service(0);
}  // AssignDesc_helloworld_2eproto()

GOOGLE_PROTOBUF_DECLARE_ONCE(grpc_cb_AssignDescriptors_once_);
inline void AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(
    &grpc_cb_AssignDescriptors_once_,
    &AssignDesc_helloworld_2eproto);
}

}  // namespace

namespace Greeter {

static const std::string method_names[] = {
  "/helloworld.Greeter/SayHello",
};

const ::google::protobuf::ServiceDescriptor& GetServiceDescriptor() {
  AssignDescriptorsOnce();
  assert(service_descriptor_Greeter);
  return *service_descriptor_Greeter;
}

std::unique_ptr< Stub> NewStub(const ::grpc_cb::ChannelSptr& channel) {
  std::unique_ptr< Stub> stub(new Stub(channel));
  return stub;
}

Stub::Stub(const ::grpc_cb::ChannelSptr& channel)
  : ::grpc_cb::ServiceStub(channel)
    // , rpcmethod_SayHello_(method_names[0], ::grpc_cb::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc_cb::Status Stub::SayHello(
    const ::helloworld::HelloRequest& request,
    ::helloworld::HelloReply* response) {
  assert(response);
  ::grpc_cb::CompletionQueue cq;
  ::grpc_cb::CallUptr call(channel_->CreateCall(method_names[0], cq.cq()));
  void* tag = call.get();
  grpc_cb::Status status = call->StartBatch(request, tag);
  if (!status.ok()) return status;
  cq.Pluck(tag);
  return call->GetResponse(response);
}

void Stub::AsyncSayHello(
    const ::helloworld::HelloRequest& request,
    const SayHelloCallback& cb,
    const ::grpc_cb::ErrorCallback& err_cb) {
  assert(cb && err_cb && cq_);
  ::grpc_cb::CallUptr call_uptr(
      channel_->CreateCall(method_names[0], cq_->cq()));
  ::grpc_cb::Call* call = call_uptr.get();
  ::grpc_cb::CompletionQueueTag* tag =
      NewCompletionQueueTag(std::move(call_uptr), cb, err_cb);
  grpc_cb::Status status = call->StartBatch(request, tag);
  if (!status.ok()) {
    DeleteCompletionQueueTag(tag);
    err_cb(status);
  }
}

// AsyncService::AsyncService() : ::grpc_cb::AsynchronousService(method_names, 1) {}

Service::Service() {
}

Service::~Service() {
}

const std::string& Service::GetMethodName(size_t method_index) const {
  assert(method_index < GetMethodCount());
  return method_names[method_index];
}

const ::google::protobuf::Message& Service::GetRequestPrototype(
    size_t method_index) const {
  assert(method_index < GetMethodCount());
  switch (method_index) {
  case 0: return ::helloworld::HelloRequest::default_instance();
  }
  assert(false);
  return *reinterpret_cast<::google::protobuf::Message*>(nullptr);
}
::grpc_cb::Status Service::SayHello(const ::helloworld::HelloRequest& request, ::helloworld::HelloReply* response) {
  (void) request;
  (void) response;
  return ::grpc_cb::Status::UNIMPLEMENTED;
}

}  // namespace Greeter

}  // namespace helloworld

