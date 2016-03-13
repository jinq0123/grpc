// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_H
#define GRPC_CB_SERVER_H

#include <unordered_map>
#include <vector>

#include <grpc/support/time.h>  // for gpr_inf_future()

#include <grpc_cb/completion_queue_ptr.h>  // for CompletionQueuePtr
#include <grpc_cb/impl/grpc_library.h>  // for GrpcLibrary
#include <grpc_cb/support/config.h>  // for GRPC_FINAL
#include <grpc_cb/support/time.h>  // for TimePoint

struct grpc_server;

namespace grpc_cb {

class InsecureServerCredentials;
class ServerCredentials;
class ServerMethodCallTag;
class Service;

/// Models a gRPC server.
class Server GRPC_FINAL : public GrpcLibrary {
 public:
  Server();
  ~Server();

  /// Shutdown the server, blocking until all rpc processing finishes.
  /// Forcefully terminate pending calls after \a deadline expires.
  ///
  /// \param deadline How long to wait until pending rpcs are forcefully
  /// terminated.
  template <class T>
  void Shutdown(const T& deadline) {
    ShutdownInternal(TimePoint<T>(deadline).raw_time());
  }

  /// Shutdown the server, waiting for all rpc processing to finish.
  void Shutdown() { ShutdownInternal(gpr_inf_future(GPR_CLOCK_MONOTONIC)); }

  /// Block waiting for all work to complete.
  ///
  /// \warning The server must be either shutting down or some other thread must
  /// call \a Shutdown for this function to ever return.
  void Run();

  /// Register a service. This call does not take ownership of the service.
  /// The service must exist for the lifetime of the Server instance.
  // bool RegisterService(/*const grpc::string* host, RpcService* service*/);
  void RegisterService(Service& service);

  /// Tries to bind \a server to the given \a addr.
  ///
  /// It can be invoked multiple times.
  ///
  /// \param addr The address to try to bind to the server (eg, localhost:1234,
  /// 192.168.1.1:31416, [::1]:27182, etc.).
  /// \params creds The credentials associated with the server.
  ///
  /// \return bound port number on sucess, 0 on failure.
  ///
  /// \warning It's an error to call this method on an already started server.
  int AddListeningPort(const grpc::string& addr,
                       const ServerCredentials& creds);
  int AddListeningPort(const grpc::string& addr);  // with InsecureServerCredentials

 private:
  void ShutdownInternal(gpr_timespec deadline);
  void RequestMethodsCalls();
  void RequestMethodCall(void* registered_method);

 private:
  typedef std::unique_ptr<grpc_server, void (*)(grpc_server*)> GrpcServerUptr;
  static GrpcServerUptr CreateServer();

 private:
  // Completion queue.
  const CompletionQueueUptr cq_;

  // Sever status
  bool started_;
  bool shutdown_;

  // Pointer to the c grpc server. Owned.
  const std::unique_ptr<grpc_server, void (*)(grpc_server*)> server_;
  std::vector<void*> registered_methods_;
};

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_H
