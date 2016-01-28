// Generated by the gRPC protobuf plugin.
// If you make any local change, they will be lost.
// source: helloworld.proto
#ifndef GRPC_CB_helloworld_2eproto__INCLUDED
#define GRPC_CB_helloworld_2eproto__INCLUDED

#include "helloworld.pb.h"

// #include <grpc++/support/async_stream.h>
// #include <grpc++/impl/rpc_method.h>

namespace grpc_cb {
class Channel;
class RpcService;
}  // namespace grpc_cb

namespace helloworld {

namespace Greeter {
class StubInterface {
 public:
  virtual ~StubInterface() {}
  virtual ::grpc::Status SayHello(::grpc::ClientContext* context, const ::helloworld::HelloRequest& request, ::helloworld::HelloReply* response) = 0;
  std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::helloworld::HelloReply>> AsyncSayHello(::grpc::ClientContext* context, const ::helloworld::HelloRequest& request, ::grpc::CompletionQueue* cq) {
    return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::helloworld::HelloReply>>(AsyncSayHelloRaw(context, request, cq));
  }
 private:
  virtual ::grpc::ClientAsyncResponseReaderInterface< ::helloworld::HelloReply>* AsyncSayHelloRaw(::grpc::ClientContext* context, const ::helloworld::HelloRequest& request, ::grpc::CompletionQueue* cq) = 0;
};
class Stub GRPC_FINAL : public StubInterface {
 public:
  Stub(const std::shared_ptr< ::grpc::Channel>& channel);
  ::grpc::Status SayHello(::grpc::ClientContext* context, const ::helloworld::HelloRequest& request, ::helloworld::HelloReply* response) GRPC_OVERRIDE;
  std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::helloworld::HelloReply>> AsyncSayHello(::grpc::ClientContext* context, const ::helloworld::HelloRequest& request, ::grpc::CompletionQueue* cq) {
    return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::helloworld::HelloReply>>(AsyncSayHelloRaw(context, request, cq));
  }

 private:
  std::shared_ptr< ::grpc::Channel> channel_;
  ::grpc::ClientAsyncResponseReader< ::helloworld::HelloReply>* AsyncSayHelloRaw(::grpc::ClientContext* context, const ::helloworld::HelloRequest& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
  const ::grpc::RpcMethod rpcmethod_SayHello_;
};

std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::Channel>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

class Service : public ::grpc::SynchronousService {
 public:
  Service();
  virtual ~Service();
  virtual ::grpc::Status SayHello(::grpc::ServerContext* context, const ::helloworld::HelloRequest* request, ::helloworld::HelloReply* response);
  ::grpc::RpcService* service() GRPC_OVERRIDE GRPC_FINAL;
 private:
  std::unique_ptr< ::grpc::RpcService> service_;
};
class AsyncService GRPC_FINAL : public ::grpc::AsynchronousService {
 public:
  explicit AsyncService();
  ~AsyncService() {};
  void RequestSayHello(::grpc::ServerContext* context, ::helloworld::HelloRequest* request, ::grpc::ServerAsyncResponseWriter< ::helloworld::HelloReply>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag);
};
}  // namespace Greeter

}  // namespace helloworld

#endif  // GRPC_CB_helloworld_2eproto__INCLUDED
