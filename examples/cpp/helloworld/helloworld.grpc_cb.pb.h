// Generated by the gRPC protobuf plugin.
// If you make any local change, they will be lost.
// source: helloworld.proto
#ifndef GRPC_CB_helloworld_2eproto__INCLUDED
#define GRPC_CB_helloworld_2eproto__INCLUDED

#include <grpc_cb/channel_sptr.h>
#include <grpc_cb/error_callback.h>  // for ErrorCallback
#include <grpc_cb/service.h>
#include <grpc_cb/service_stub.h>
#include <grpc_cb/support/status.h>

#include "helloworld.pb.h"

namespace google {
namespace protobuf {
class ServiceDescriptor;
// class MethodDescriptor;
}  // namespace protobuf
}  // namespace google

namespace grpc_cb {
class RpcService;
}  // namespace grpc_cb

namespace helloworld {

namespace Greeter {

const ::google::protobuf::ServiceDescriptor& GetServiceDescriptor();

class Stub : public ::grpc_cb::ServiceStub {
 public:
  Stub(const ::grpc_cb::ChannelSptr& channel);

  inline ::grpc_cb::Status SayHello(const ::helloworld::HelloRequest& request) {
    ::helloworld::HelloReply response;
    return SayHello(request, &response);  // Ignore response.
  }
  ::grpc_cb::Status SayHello(const ::helloworld::HelloRequest& request, ::helloworld::HelloReply* response);

  typedef std::function<void (const ::helloworld::HelloReply& response)> SayHelloCallback;
  inline void AsyncSayHello(const ::helloworld::HelloRequest& request) {
    return AsyncSayHello(request, &IgnoreResponse<::helloworld::HelloReply>);
  }
  inline void AsyncSayHello(const ::helloworld::HelloRequest& request, const SayHelloCallback& cb) {
    return AsyncSayHello(request, cb, error_callback_);  // Use default error callback.
  }
  void AsyncSayHello(const ::helloworld::HelloRequest& request, const SayHelloCallback& cb, const ::grpc_cb::ErrorCallback& err_cb);

 private:
  // const ::grpc_cb::RpcMethod rpcmethod_SayHello_;
};

std::unique_ptr<Stub> NewStub(const ::grpc_cb::ChannelSptr& channel);

class Service : public ::grpc_cb::Service {
 public:
  Service();
  virtual ~Service();

  virtual const std::string& GetMethodName(size_t i) const GRPC_OVERRIDE;
  virtual const ::google::protobuf::Message& GetRequestPrototype(
      size_t method_index) const GRPC_OVERRIDE;

  virtual ::grpc_cb::Status SayHello(const ::helloworld::HelloRequest& request, ::helloworld::HelloReply* response);
 
private:
  virtual const ::google::protobuf::ServiceDescriptor& GetDescriptor()
      const GRPC_OVERRIDE {
    return GetServiceDescriptor();
  }
};

}  // namespace Greeter

}  // namespace helloworld

#endif  // GRPC_CB_helloworld_2eproto__INCLUDED
