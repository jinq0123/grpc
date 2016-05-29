// Generated by the gRPC protobuf plugin.
// If you make any local change, they will be lost.
// source: helloworld.proto

#include "helloworld.grpc_cb.pb.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/once.h>

#include <grpc_cb/impl/call.h>                            // for Call
#include <grpc_cb/impl/call_operations.h>                 // for CallOperations
#include <grpc_cb/impl/client/client_async_call_cqtag.h>  // for ClientAsyncCallCqTag
#include <grpc_cb/impl/client/client_call_cqtag.h>        // for ClientCallCqTag
#include <grpc_cb/impl/completion_queue.h>                // for CompletionQueue
#include <grpc_cb/impl/proto_utils.h>  // for DeserializeProto()

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

::grpc_cb::Status Stub::BlockingSayHello(
    const ::helloworld::HelloRequest& request,
    ::helloworld::HelloReply* response) {
  assert(response);
  ::grpc_cb::CompletionQueue cq;
  ::grpc_cb::CallSptr call_sptr(channel_->MakeCall(method_names[0], cq.cq()));
  ::grpc_cb::ClientCallCqTag tag(call_sptr);
  ::grpc_cb::CallOperations ops;
  ::grpc_cb::Status status;
  // Todo: status = tag->Start(request)
  status = tag.InitCallOps(request, ops);
  if (!status.ok()) return status;
  status = call_sptr->StartBatch(ops, &tag);
  if (!status.ok()) return status;
  cq.Pluck(&tag);  // Todo: Make sure tag was not queued if StartBatch() failed.
  return tag.GetResponse(*response);
}

void Stub::AsyncSayHello(
    const ::helloworld::HelloRequest& request,
    const SayHelloCallback& cb,
    const ::grpc_cb::ErrorCallback& err_cb) {
  assert(cb && err_cb && cq_);
  ::grpc_cb::CallSptr call_sptr(
      channel_->MakeCall(method_names[0], cq_->cq()));
  using CqTag = ::grpc_cb::ClientAsyncCallCqTag<::helloworld::HelloReply>;
  CqTag* tag = new CqTag(call_sptr, cb, err_cb);
  // DEL ::grpc_cb::CompletionQueueTag* tag =  // XXX Rename to ClientAsyncUnaryCallTag : public ClientUnaryCallTag
  // DEL    NewCompletionQueueTag(call_sptr, cb, err_cb);

  // Todo: tag->Start(request);
  ::grpc_cb::CallOperations ops;
  ::grpc_cb::Status status = tag->InitCallOps(request, ops);
  if (status.ok())
      status = call_sptr->StartBatch(ops, tag);
  if (!status.ok()) {
    delete tag;  // DEL DeleteCompletionQueueTag(tag);  // XXX just delete?
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

void Service::CallMethod(
    size_t method_index, grpc_byte_buffer& request_buffer,
    const ::grpc_cb::CallSptr& call_sptr) {
  assert(method_index < GetMethodCount());
  switch (method_index) {
    case 0:
      SayHello(request_buffer,
          ::grpc_cb::ServerReplier<::helloworld::HelloReply>(call_sptr));
      return;
  }  // switch
  assert(false);
}

void Service::SayHello(
    grpc_byte_buffer& request_buffer,
    const ::grpc_cb::ServerReplier<::helloworld::HelloReply>& replier) {
  using Request = ::helloworld::HelloRequest;
  Request request;
  ::grpc_cb::Status status =
      ::grpc_cb::DeserializeProto(
          &request_buffer, &request, 0 /* TODO: max_message_size*/);
  if (status.ok()) {
    SayHello(request, replier);
    return;
  }
  ::grpc_cb::ServerReplier<::helloworld::HelloReply>(
      replier).ReplyError(status);
}
void Service::SayHello(
    const ::helloworld::HelloRequest& request,
    ::grpc_cb::ServerReplier<::helloworld::HelloReply> replier_copy) {
  (void) request;
  replier_copy.ReplyError(::grpc_cb::Status::UNIMPLEMENTED);
}

}  // namespace Greeter

}  // namespace helloworld

