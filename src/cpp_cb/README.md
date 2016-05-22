
#Overview

This directory contains source code for C++ callback implementation of gRPC.

#Status

Not ready

#Todo
* WriteOptions
* Add interfaces for protobuf bytes message.
* Delete call_op.h.
* DoComplete(bool success) use success parameter.

#Other
 
To generate grpc_cb files:
```
D:\Jinq\tools\protoc.exe ^
  -I D:\Jinq\Git\grpc_jinq0123\examples\protos ^
  --grpc_out=D:\Jinq\Git\grpc_jinq0123\examples\cpp\helloworld ^
  --plugin=protoc-gen-grpc=D:\Jinq\Git\grpc_jinq0123\vsprojects\Debug\grpc_cpp_cb_plugin.exe ^
  D:\Jinq\Git\grpc_jinq0123\examples\protos\helloworld.proto
```