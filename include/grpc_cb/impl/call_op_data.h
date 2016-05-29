#ifndef GRPC_CB_IMPL_CALL_OP_DATA_H
#define GRPC_CB_IMPL_CALL_OP_DATA_H

#include <grpc/support/alloc.h>          // for gpr_free()
#include <grpc/support/port_platform.h>  // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/metadata_vector.h>  // for MetadataVector
#include <grpc_cb/impl/proto_utils.h>      // for SerializeProto()
#include <grpc_cb/status.h>                // for Status
#include <grpc_cb/support/config.h>        // for GRPC_FINAL
#include <grpc_cb/support/protobuf_fwd.h>  // for Message

namespace grpc_cb {

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
  ~CodSendMessage() {
    grpc_byte_buffer_destroy(send_buf_);
  }

  Status SerializeMessage(const ::google::protobuf::Message& message)
      GRPC_MUST_USE_RESULT {
    // send_buf_ is created here and destroyed in dtr().
    return SerializeProto(message, &send_buf_);
  }

  grpc_byte_buffer* GetSendBuf() { return send_buf_; }

 private:
  // send_buf_ is created in SerializeMessage() and destroyed in dtr().
  grpc_byte_buffer* send_buf_ = nullptr;  // owned
  // Todo: WriteOptions write_options_;
  //   or outside in CallOperations::SendMessage()?
};  // class CodSendMessage

// Cod to receive initial metadata.
class CodRecvInitMd GRPC_FINAL {
 public:
  CodRecvInitMd() {
    grpc_metadata_array_init(&recv_init_md_arr_);
  }
  ~CodRecvInitMd() {
    grpc_metadata_array_destroy(&recv_init_md_arr_);
  }

  grpc_metadata_array* GetRecvInitMdArrPtr() { return &recv_init_md_arr_; }
  // Todo: Get result metadata.

 private:
  // std::multimap<grpc::string_ref, grpc::string_ref>* recv_init_md_;
  grpc_metadata_array recv_init_md_arr_;
};  // class CodRecvInitMd

// Cod to receive message.
class CodRecvMessage GRPC_FINAL {
 public:
  ~CodRecvMessage() {
    grpc_byte_buffer_destroy(recv_buf_);
  }
  grpc_byte_buffer** GetRecvBufPtr() { return &recv_buf_; }

  Status GetResponse(::google::protobuf::Message& message, int max_message_size) {
    return DeserializeProto(recv_buf_, &message, max_message_size);
  }

 private:
  grpc_byte_buffer* recv_buf_ = nullptr;  // owned
};  // class CodRecvMessage

// No Cod for ClientSendClose

// Cod for client to receive status.
class CodClientRecvStatus {
 public:
  CodClientRecvStatus() {
    grpc_metadata_array_init(&recv_trail_md_arr_);
  }
  ~CodClientRecvStatus() {
    grpc_metadata_array_destroy(&recv_trail_md_arr_);
    gpr_free(status_details_);
  }

 public:
  grpc_metadata_array* GetTrailMdArrPtr() { return &recv_trail_md_arr_; }
  grpc_status_code* GetStatusCodePtr() { return &status_code_; }
  char** GetStatusDetailsBufPtr() { return &status_details_; }
  size_t* GetStatusDetailsCapacityPtr() { return &status_details_capacity_; }

 public:
  bool IsStatusOk() const { return status_code_ == GRPC_STATUS_OK; }
  Status GetStatus() const;

 private:
  // Todo: std::multimap<grpc::string_ref, grpc::string_ref>* recv_trailing_metadata_;
  // Todo: Status* recv_status_ = nullptr;

  // Metadata array to receive trailing metadata.
  grpc_metadata_array recv_trail_md_arr_;
  grpc_status_code status_code_ = GRPC_STATUS_OK;
  char* status_details_ = nullptr;
  size_t status_details_capacity_ = 0;
};

// XXX Other Cod...

}  // namespace grpc_cb
#endif  // GRPC_CB_IMPL_CALL_OP_DATA_H
