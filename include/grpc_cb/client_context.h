/*
 *
 * Copyright 2015-2016, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/// A ClientContext allows the person implementing a service client to:
///
/// - Add custom metadata key-value pairs that will propagated to the server
/// side.
/// - Control call settings such as compression and authentication.
/// - Initial and trailing metadata coming from the server.
/// - Get performance metrics (ie, census).
///
/// Context settings are only relevant to the call they are invoked with, that
/// is to say, they aren't sticky. Some of these settings, such as the
/// compression options, can be made persistant at channel construction time
/// (see \a grpc::CreateCustomChannel).
///
/// \warning ClientContext instances should \em not be reused across rpcs.

#ifndef GRPC_CB_CLIENT_CONTEXT_H
#define GRPC_CB_CLIENT_CONTEXT_H

namespace grpc_cb {

class ClientContext {
 public:
  ClientContext();
  ~ClientContext();

  /// Set the deadline for the client call.
  ///
  /// \warning This method should only be called before invoking the rpc.
  ///
  /// \param deadline the deadline for the client call. Units are determined by
  /// the type used.
  template <typename T>
  void set_deadline(const T& deadline) {
    TimePoint<T> deadline_tp(deadline);
    deadline_ = deadline_tp.raw_time();
  }

#ifndef GRPC_CXX0X_NO_CHRONO
  /// Return the deadline for the client call.
  std::chrono::system_clock::time_point deadline() {
    return Timespec2Timepoint(deadline_);
  }
#endif  // !GRPC_CXX0X_NO_CHRONO

  /// Return a \a gpr_timespec representation of the client call's deadline.
  gpr_timespec raw_deadline() { return deadline_; }

  /// Return the compression algorithm to be used by the client call.
  grpc_compression_algorithm compression_algorithm() const {
    return compression_algorithm_;
  }

  /// Set \a algorithm to be the compression algorithm used for the client call.
  ///
  /// \param algorith The compression algorithm used for the client call.
  void set_compression_algorithm(grpc_compression_algorithm algorithm);

  /// Return the peer uri in a string.
  ///
  /// \warning This value is never authenticated or subject to any security
  /// related code. It must not be used for any authentication related
  /// functionality. Instead, use auth_context.
  ///
  /// \return The call's peer URI.
  std::string peer() const;

  /// Send a best-effort out-of-band cancel. The call could be in any stage.
  /// e.g. if it is already finished, it may still return success.
  ///
  /// There is no guarantee the call will be cancelled.
  void TryCancel();

 private:
  // Disallow copy and assign.
  ClientContext(const ClientContext&);
  ClientContext& operator=(const ClientContext&);

  ChannelPtr channel_;
  grpc::mutex mu_;
  gpr_timespec deadline_;
  grpc_compression_algorithm compression_algorithm_;
};

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CONTEXT_H
