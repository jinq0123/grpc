#ifndef GRPC_CB_SUPPORT_REPLIER_READER_WRITER_FWD_H
#define GRPC_CB_SUPPORT_REPLIER_READER_WRITER_FWD_H

// Forward declarations of server and client replier, reader and writer.
namespace grpc_cb {

template <class ResponseType> class ServerReplier;
template <class Request> class ServerReader;
template <class Response> class ServerWriter;
template <class Request, class Response> class ServerReaderWriter;

template <class Response> class ClientReader;
template <class Request> class ClientWriter;
template <class Request, class Response> class ClientReaderWriter;

}  // namespace grpc_cb

#endif  // GRPC_CB_SUPPORT_REPLIER_READER_WRITER_FWD_H
