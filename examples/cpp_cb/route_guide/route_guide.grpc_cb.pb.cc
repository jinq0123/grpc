// Generated by the gRPC protobuf plugin.
// If you make any local change, they will be lost.
// source: route_guide.proto

#include "route_guide.grpc_cb.pb.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/once.h>

#include <grpc_cb/impl/call.h>                            // for Call
#include <grpc_cb/impl/call_operations.h>                 // CallOperations
#include <grpc_cb/impl/client/client_async_call_cqtag.h>  // for ClientAsyncCallCqTag<>
#include <grpc_cb/impl/client/client_call_cqtag.h>  // for ClientCallCqTag
#include <grpc_cb/impl/completion_queue.h>          // for CompletionQueue
#include <grpc_cb/impl/proto_utils.h>               // for DeserializeProto()

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
  ::grpc_cb::CallSptr call_sptr(channel_->MakeCall(method_names[0], cq.cq()));
  ::grpc_cb::ClientCallCqTag tag(call_sptr);
  ::grpc_cb::CallOperations ops;
  ::grpc_cb::Status status;  // Todo: = tag->Start(request)
  status = tag.InitCallOps(request, ops);
  if (!status.ok()) return status;
  status = call_sptr->StartBatch(ops, &tag);
  cq.Pluck(&tag);
  return call_sptr->GetResponse(response);  // XXX tag.GetResponse()
}

void Stub::AsyncGetFeature(
    const ::routeguide::Point& request,
    const GetFeatureCallback& cb,
    const ::grpc_cb::ErrorCallback& err_cb) {  // XXX rename to ecb
  assert(cb && err_cb && cq_);
  ::grpc_cb::CallSptr call_sptr(
      channel_->MakeCall(method_names[0], cq_->cq()));
  using CqTag = ::grpc_cb::ClientAsyncCallCqTag<::routeguide::Feature>;
  CqTag* tag = new CqTag(call_sptr, cb, err_cb);
  //::grpc_cb::CompletionQueueTag* tag =
  //    NewCompletionQueueTag(call_sptr, cb, err_cb);
  // Todo: tag->Start(request)
  ::grpc_cb::CallOperations ops;
  ::grpc_cb::Status status = tag->InitCallOps(request, ops);
  if (status.ok())
      status = call_sptr->StartBatch(ops, tag);
  if (!status.ok()) {
    // DEL DeleteCompletionQueueTag(tag);
    delete tag;
    err_cb(status);
  }
}

::grpc_cb::ClientReader<::routeguide::Feature>
Stub::ListFeatures(const ::routeguide::Rectangle& request) {
  return ::grpc_cb::ClientReader<::routeguide::Feature>(
      channel_, method_names[1], request, cq_->cq());
}

::grpc_cb::ClientWriter<::routeguide::Point>
Stub::RecordRoute(::routeguide::RouteSummary* response) {
  return ::grpc_cb::ClientWriter<::routeguide::Point>();
}

::grpc_cb::ClientReaderWriter<
  ::routeguide::RouteNote,
  ::routeguide::RouteNote>
Stub::RouteChat() {
  return ::grpc_cb::ClientReaderWriter<
    ::routeguide::RouteNote,
    ::routeguide::RouteNote>();
}

Service::Service() {}

Service::~Service() {}

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
      GetFeature(request_buffer,
          ::grpc_cb::ServerReplier<::routeguide::Feature>(call_sptr));
      return;
    case 1:
      // XXX
      //ListFeatures(request_buffer,
      //    ::grpc_cb::ServerReplier<::routeguide::Feature>(msg_replier));
      return;
    case 2:
      //RecordRoute(request_buffer,
      //    ::grpc_cb::ServerReplier<::routeguide::RouteSummary>(msg_replier));
      return;
    case 3:
      //RouteChat(
      //    request_buffer,
      //    ::grpc_cb::ServerReplier<::routeguide::RouteNote>(msg_replier));
      return;
  }  // switch
  assert(false);
}

void Service::GetFeature(
    grpc_byte_buffer& request_buffer,
    const ::grpc_cb::ServerReplier<::routeguide::Feature>& replier) {
  using Request = ::routeguide::Point;
  Request request;
  ::grpc_cb::Status status =
      ::grpc_cb::DeserializeProto(
          &request_buffer, &request, 0 /* TODO: max_message_size*/);
  if (status.ok()) {
    GetFeature(request, replier);
    return;
  }
  ::grpc_cb::ServerReplier<::routeguide::Feature>(
      replier).ReplyError(status);
}
void Service::GetFeature(
    const ::routeguide::Point& request,
    ::grpc_cb::ServerReplier<::routeguide::Feature> replier_copy) {
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
    ::grpc_cb::ServerReplier<::routeguide::RouteSummary> replier_copy) {
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
