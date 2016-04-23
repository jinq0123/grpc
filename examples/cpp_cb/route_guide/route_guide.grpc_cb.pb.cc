// Generated by the gRPC protobuf plugin.
// If you make any local change, they will be lost.
// source: route_guide.proto

#include "route_guide.grpc_cb.pb.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/once.h>

#include <grpc_cb/channel.h>
#include <grpc_cb/completion_queue.h>
#include <grpc_cb/impl/call.h>
#include <grpc_cb/impl/proto_utils.h>      // for Deserialize()
#include <grpc_cb/server_async_replier.h>  // for ServerAsyncReplier<>

// package routeguide
namespace routeguide {

namespace {

const ::google::protobuf::ServiceDescriptor*
service_descriptor_RouteGuide = nullptr;

void AssignDesc_route_5fguide_2eproto() {
  // Get the file's descriptor from the pool.
  const ::google::protobuf::FileDescriptor* file =
      ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
          "route_guide.proto");
  GOOGLE_CHECK(file != NULL);
  service_descriptor_RouteGuide = file->service(0);
}  // AssignDesc_route_5fguide_2eproto()

GOOGLE_PROTOBUF_DECLARE_ONCE(grpc_cb_AssignDescriptors_once_);
inline void AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(
      &grpc_cb_AssignDescriptors_once_,
      &AssignDesc_route_5fguide_2eproto);
}

}  // namespace

namespace RouteGuide {  // service RouteGuide

static const std::string method_names[] = {
    "/routeguide.RouteGuide/GetFeature",
    "/routeguide.RouteGuide/ListFeatures",
    "/routeguide.RouteGuide/RecordRoute",
    "/routeguide.RouteGuide/RouteChat",
};

const ::google::protobuf::ServiceDescriptor& GetServiceDescriptor() {
  AssignDescriptorsOnce();
  assert(service_descriptor_RouteGuide);
  return *service_descriptor_RouteGuide;
}

std::unique_ptr<Stub> NewStub(const ::grpc_cb::ChannelSptr& channel) {
  std::unique_ptr<Stub> stub(new Stub(channel));
  return stub;
}

Stub::Stub(const ::grpc_cb::ChannelSptr& channel)
    : ::grpc_cb::ServiceStub(channel) {}

::grpc_cb::Status Stub::BlockingGetFeature(
    const ::routeguide::Point& request,
    ::routeguide::Feature* response) {
  assert(response);
  ::grpc_cb::CompletionQueue cq;
  ::grpc_cb::CallUptr call(channel_->CreateCall(method_names[0], cq.cq()));
  void* tag = call.get();
  grpc_cb::Status status = call->StartBatch(request, tag);
  if (!status.ok()) return status;
  cq.Pluck(tag);
  return call->GetResponse(response);
}

void Stub::AsyncGetFeature(
    const ::routeguide::Point& request,
    const GetFeatureCallback& cb,
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

Service::Service() {}

Service::~Service() {}

const std::string& Service::GetMethodName(size_t method_index) const {
  assert(method_index < GetMethodCount());
  return method_names[method_index];
}

void Service::CallMethod(
    size_t method_index, grpc_byte_buffer& request_buffer,
    const ::grpc_cb::ServerAsyncMsgReplier& msg_replier) {
  assert(method_index < GetMethodCount());
  switch (method_index) {
    case 0:
      GetFeature(request_buffer,
          ::grpc_cb::ServerAsyncReplier<::routeguide::Feature>(msg_replier));
      return;
    case 1:
      // XXX
      //ListFeatures(request_buffer,
      //    ::grpc_cb::ServerAsyncReplier<::routeguide::Feature>(msg_replier));
      return;
    case 2:
      //RecordRoute(request_buffer,
      //    ::grpc_cb::ServerAsyncReplier<::routeguide::RouteSummary>(msg_replier));
      return;
    case 3:
      //RouteChat(
      //    request_buffer,
      //    ::grpc_cb::ServerAsyncReplier<::routeguide::RouteNote>(msg_replier));
      return;
  }  // switch
  assert(false);
  ::grpc_cb::ServerAsyncMsgReplier(msg_replier)
      .ReplyError(::grpc_cb::Status::InternalError("CallMethod() error"));
}

void Service::GetFeature(
    grpc_byte_buffer& request_buffer,
    const ::grpc_cb::ServerAsyncReplier<::routeguide::Feature>& replier) {
  using Request = ::routeguide::Point;
  Request request;
  ::grpc_cb::Status status =
      ::grpc_cb::SerializationTraits<Request>::Deserialize(
          &request_buffer, &request, 0 /* TODO: max_message_size*/);
  if (status.ok()) {
    GetFeature(request, replier);
    return;
  }
  ::grpc_cb::ServerAsyncReplier<::routeguide::Feature>(
      replier).ReplyError(status);
}
void Service::GetFeature(
    const ::routeguide::Point& request,
    ::grpc_cb::ServerAsyncReplier<::routeguide::Feature> replier_copy) {
  (void)request;
  replier_copy.ReplyError(::grpc_cb::Status::UNIMPLEMENTED);
}

::grpc_cb::Status Service::ListFeatures(
    const ::routeguide::Rectangle& request,
    ::grpc_cb::ServerWriter<::routeguide::Feature> writer) {
  (void)request;
  (void)writer;
  return ::grpc_cb::Status::UNIMPLEMENTED;
}

::grpc_cb::Status Service::RecordRoute(
    ::grpc_cb::ServerReader<::routeguide::Point> reader,
    ::grpc_cb::ServerAsyncReplier<::routeguide::RouteSummary> replier_copy) {
  (void)reader;
  (void)replier_copy;
  return ::grpc_cb::Status::UNIMPLEMENTED;
}

::grpc_cb::Status Service::RouteChat(
    ::grpc_cb::ServerReaderWriter<::routeguide::RouteNote,
                                  ::routeguide::RouteNote>
        stream) {
  (void)stream;
  return ::grpc_cb::Status::UNIMPLEMENTED;
}

}  // namespace RouteGuide

}  // namespace routeguide
