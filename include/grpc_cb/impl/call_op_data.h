#ifndef GRPC_CB_COMMON_CALL_OP_DATA_H
#define GRPC_CB_COMMON_CALL_OP_DATA_H

#include <grpc_cb/impl/metadata_vector.h>  // for MetadataVector
#include <grpc_cb/support/config.h>        // for GRPC_FINAL

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
};

// XXX Other Cod...

}  // namespace grpc_cb
#endif  // GRPC_CB_COMMON_CALL_OP_DATA_H