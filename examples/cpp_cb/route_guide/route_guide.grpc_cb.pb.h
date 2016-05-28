// Generated by the gRPC protobuf plugin.
// If you make any local change, they will be lost.
// source: route_guide.proto
#ifndef GRPC_CB_route_5fguide_2eproto__INCLUDED
#define GRPC_CB_route_5fguide_2eproto__INCLUDED

#include <grpc_cb/channel_sptr.h>
#include <grpc_cb/client/client_reader.h>
#include <grpc_cb/client/client_reader_writer.h>
#include <grpc_cb/client/client_writer.h>
#include <grpc_cb/error_callback.h>  // for ErrorCallback
#include <grpc_cb/server/server_reader.h>
#include <grpc_cb/server/server_reader_writer.h>
#include <grpc_cb/server/server_writer.h>
#include <grpc_cb/server_async_replier.h>  // for ServerAsyncReplier<>
#include <grpc_cb/service.h>
#include <grpc_cb/service_stub.h>
#include <grpc_cb/support/status.h>

#include "route_guide.pb.h"

// package routeguide
namespace routeguide {

namespace RouteGuide {  // service RouteGuide

const ::google::protobuf::ServiceDescriptor& GetServiceDescriptor();

class Stub : public ::grpc_cb::ServiceStub {
 public:
  explicit Stub(const ::grpc_cb::ChannelSptr& channel);

  inline ::grpc_cb::Status BlockingGetFeature(
       const ::routeguide::Point& request) {
    ::routeguide::Feature response;
    return BlockingGetFeature(request, &response);  // Ignore response.
  }
  ::grpc_cb::Status BlockingGetFeature(
      const ::routeguide::Point& request,
      ::routeguide::Feature* response);

  using GetFeatureCallback =
      std::function<void (const ::routeguide::Feature& response)>;
  inline void AsyncGetFeature(
      const ::routeguide::Point& request) {
    return AsyncGetFeature(request, &IgnoreResponse<::routeguide::Feature>);
  }
  inline void AsyncGetFeature(
      const ::routeguide::Point& request,
      const GetFeatureCallback& cb) {
    return AsyncGetFeature(request, cb, error_callback_);  // Use default error callback.
  }
  void AsyncGetFeature(
      const ::routeguide::Point& request,
      const GetFeatureCallback& cb,
      const ::grpc_cb::ErrorCallback& err_cb);

  ::grpc_cb::ClientReader<::routeguide::Feature>
  ListFeatures(const ::routeguide::Rectangle& request);

  ::grpc_cb::ClientWriter<::routeguide::Point>
  RecordRoute(::routeguide::RouteSummary* response) {
    return ::grpc_cb::ClientWriter<::routeguide::Point>();
  }

  ::grpc_cb::ClientReaderWriter<
    ::routeguide::RouteNote,
    ::routeguide::RouteNote>
  RouteChat() {
    return ::grpc_cb::ClientReaderWriter<
      ::routeguide::RouteNote,
      ::routeguide::RouteNote>();
  }

 private:
};

std::unique_ptr<Stub> NewStub(const ::grpc_cb::ChannelSptr& channel);

class Service : public ::grpc_cb::Service {
 public:
  Service();
  virtual ~Service();

  virtual const std::string& GetMethodName(size_t i) const GRPC_OVERRIDE;
  virtual void CallMethod(
      size_t method_index, grpc_byte_buffer& request_buffer,
      const ::grpc_cb::CallSptr& call_sptr) GRPC_OVERRIDE;

 private:
  void GetFeature(
      grpc_byte_buffer& request_buffer,
      const ::grpc_cb::ServerAsyncReplier<::routeguide::Feature>& replier);
  virtual void GetFeature(
      const ::routeguide::Point& request,
      ::grpc_cb::ServerAsyncReplier<::routeguide::Feature> replier_copy);

  virtual ::grpc_cb::Status ListFeatures(
      const ::routeguide::Rectangle& request,
      ::grpc_cb::ServerWriter<::routeguide::Feature> writer);

  virtual ::grpc_cb::Status RecordRoute(
      ::grpc_cb::ServerReader<::routeguide::Point> reader,
      ::grpc_cb::ServerAsyncReplier<::routeguide::RouteSummary> replier_copy);

  virtual ::grpc_cb::Status RouteChat(
      ::grpc_cb::ServerReaderWriter<::routeguide::RouteNote,
                                    ::routeguide::RouteNote>
          stream);

 private:
  virtual const ::google::protobuf::ServiceDescriptor& GetDescriptor()
      const GRPC_OVERRIDE {
    return GetServiceDescriptor();
  }
};

}  // namespace RouteGuide

}  // namespace routeguide

#endif  // GRPC_CB_route_5fguide_2eproto__INCLUDED
