
#Overview

This directory contains source code for C++ callback implementation of gRPC.

#Status

Not ready

#Todo
* WriteOptions
* Add interfaces for protobuf bytes message.
* DoComplete(bool success) use success parameter.
* Handle ServerReplier, ServerReader ServerWriter likely.

#Design
CompletionQueueTag is the element of CompletionQueue.
On completion, CompletionQueueTag is poped, and DoComplete() is called,
and then got deleted.
CompletionQueueTag keeps the input and output of one rpc call.
CallOperations keeps references to members of CompletionQueueTag, and is 
used in Call::StartBatch().
Async rpc callback is got called in DoComplete().

#Other
 
To generate grpc_cb files:
```
D:\Jinq\tools\protoc.exe ^
  -I D:\Jinq\Git\grpc_jinq0123\examples\protos ^
  --grpc_out=D:\Jinq\Git\grpc_jinq0123\examples\cpp\helloworld ^
  --plugin=protoc-gen-grpc=D:\Jinq\Git\grpc_jinq0123\vsprojects\Debug\grpc_cpp_cb_plugin.exe ^
  D:\Jinq\Git\grpc_jinq0123\examples\protos\helloworld.proto
```