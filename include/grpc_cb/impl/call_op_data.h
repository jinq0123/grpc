#ifndef GRPC_CB_IMPL_CALL_OP_DATA_H
#define GRPC_CB_IMPL_CALL_OP_DATA_H

#include <grpc/support/port_platform.h>  // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/metadata_vector.h>  // for MetadataVector
#include <grpc_cb/support/config.h>        // for GRPC_FINAL
#include <grpc_cb/support/protobuf_fwd.h>  // for Message

namespace grpc_cb {

class Status;

// Call operation data (Cod) classes.
// Call operation data need to be kept in CallCqTag instead of CallOperations,
//   because CallOperations object is transient.

// Cod to send initial metadata.
class CodSendInitMd GRPC_FINAL {
 public:
  // Todo: Set metadata.
  MetadataVector& GetMdVec() { return init_md_vec_; }

 private:
  // MetadataVector is only references of keys and values.
  // The key-value strings must live until completion.
  // Keeping metadata key-values outside is because they are usually const.
  // Variable metadata can be kept in CallCqTag.
  MetadataVector init_md_vec_;
};  // class CodSendInitMd

// Cod to send message.
class CodSendMessage GRPC_FINAL {
 public:
  CodSendMessage() : send_buf_(nullptr) {}
  ~CodSendMessage() {
    grpc_byte_buffer_destroy(send_buf_);
  }

  Status SerializeMessage(const ::google::protobuf::Message& message)
      GRPC_MUST_USE_RESULT;
  grpc_byte_buffer* GetSendBuf() { return send_buf_; }

 private:
  grpc_byte_buffer* send_buf_;  // owned
  // Todo: WriteOptions write_options_;
  //   or outside in CallOperations::SendMessage()?
};

// XXX Other Cod...

}  // namespace grpc_cb
#endif  // GRPC_CB_IMPL_CALL_OP_DATA_H
