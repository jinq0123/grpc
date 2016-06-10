// Generated by the gRPC protobuf plugin.
// If you make any local change, they will be lost.
// source: helloworld.proto
#ifndef GRPC_CB_helloworld_2eproto__INCLUDED
#define GRPC_CB_helloworld_2eproto__INCLUDED

#include <grpc_cb/grpc_cb.h>  // Include all for user.
#include "helloworld.pb.h"

// package helloworld
namespace helloworld {

namespace Greeter {  // service Greeter

const ::google::protobuf::ServiceDescriptor& GetServiceDescriptor();

class Stub : public ::grpc_cb::ServiceStub {
 public:
  Stub(const ::grpc_cb::ChannelSptr& channel);

  inline ::grpc_cb::Status BlockingSayHello(
       const ::helloworld::HelloRequest& request) {
    ::helloworld::HelloReply response;
    return BlockingSayHello(request, &response);  // Ignore response.
  }
  ::grpc_cb::Status BlockingSayHello(
      const ::helloworld::HelloRequest& request,
      ::helloworld::HelloReply* response);

  using SayHelloCallback =
      std::function<void (const ::helloworld::HelloReply& response)>;
  inline void AsyncSayHello(
      const ::helloworld::HelloRequest& request) {
    return AsyncSayHello(request, &IgnoreResponse<::helloworld::HelloReply>);
  }
  inline void AsyncSayHello(
      const ::helloworld::HelloRequest& request,
      const SayHelloCallback& cb) {
    return AsyncSayHello(request, cb, GetErrorCallback());  // Use default error callback.
  }
  void AsyncSayHello(
      const ::helloworld::HelloRequest& request,
      const SayHelloCallback& cb,
      const ::grpc_cb::ErrorCallback& err_cb);

 private:
  // const ::grpc_cb::RpcMethod rpcmethod_SayHello_;
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
  void SayHello(
      grpc_byte_buffer& request_buffer,
      const ::grpc_cb::ServerReplier<::helloworld::HelloReply>& replier);
  virtual void SayHello(
      const ::helloworld::HelloRequest& request,
      ::grpc_cb::ServerReplier<::helloworld::HelloReply> replier_copy);

 private:
  virtual const ::google::protobuf::ServiceDescriptor& GetDescriptor()
      const GRPC_OVERRIDE {
    return GetServiceDescriptor();
  }
};

}  // namespace Greeter

}  // namespace helloworld

#endif  // GRPC_CB_helloworld_2eproto__INCLUDED