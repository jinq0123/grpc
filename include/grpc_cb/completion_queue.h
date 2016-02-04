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

/// A completion queue implements a concurrent producer-consumer queue.

#ifndef GRPC_CB_COMPLETION_QUEUE_H
#define GRPC_CB_COMPLETION_QUEUE_H

#include <grpc_cb/impl/grpc_library.h>
#include <grpc_cb/support/config.h>  // for GRPC_OVERRIDE

struct grpc_completion_queue;

namespace grpc_cb {

/// A thin wrapper around \a grpc_completion_queue (see / \a
/// src/core/surface/completion_queue.h).
class CompletionQueue : public GrpcLibrary {
 public:
  /// Default constructor. Implicitly creates a \a grpc_completion_queue
  /// instance.
  CompletionQueue();

  /// Wrap \a take, taking ownership of the instance.
  ///
  /// \param take The completion queue instance to wrap. Ownership is taken.
  explicit CompletionQueue(grpc_completion_queue* take);

  /// Destructor. Destroys the owned wrapped completion queue / instance.
  virtual ~CompletionQueue() GRPC_OVERRIDE;

 public:
  /// Tri-state return for AsyncNext: SHUTDOWN, GOT_EVENT, TIMEOUT.
  enum NextStatus {
    SHUTDOWN,   ///< The completion queue has been shutdown.
    GOT_EVENT,  ///< Got a new event; \a tag will be filled in with its
                ///< associated value; \a ok indicating its success.
    TIMEOUT     ///< deadline was reached.
  };

 public:
  typedef uintptr_t Tag;
  static_assert(sizeof(Tag) == sizeof(void*), "Tag size is wrong.");

 public:
  /// Read from the queue, blocking up to \a deadline (or the queue's shutdown).
  /// Both \a tag and \a ok are updated upon success (if an event is available
  /// within the \a deadline).  A \a tag points to an arbitrary location usually
  /// employed to uniquely identify an event.
  ///
  /// \param tag[out] Upon sucess, updated to point to the event's tag.
  /// \param ok[out] Upon sucess, true if read a regular event, false otherwise.
  /// \param deadline[in] How long to block in wait for an event.
  ///
  /// \return The type of event read.
  template <typename T>
  NextStatus AsyncNext(bool* ok, const T& deadline) {
    TimePoint<T> deadline_tp(deadline);
    return AsyncNextInternal(ok, deadline_tp.raw_time());
  }

  /// Read from the queue, blocking until an event is available or the queue is
  /// shutting down.
  ///
  /// \param tag[out] Updated to point to the read event's tag.
  /// \param ok[out] true if read a regular event, false otherwise.
  ///
  /// \return true if read a regular event, false if the queue is shutting down.
  bool Next(bool* ok) {
    return (AsyncNextInternal(ok, gpr_inf_future(GPR_CLOCK_REALTIME)) !=
            SHUTDOWN);
  }

  /// Request the shutdown of the queue.
  ///
  /// \warning This method must be called at some point. Once invoked, \a Next
  /// will start to return false and \a AsyncNext will return \a
  /// NextStatus::SHUTDOWN. Only once either one of these methods does that
  /// (that is, once the queue has been \em drained) can an instance of this
  /// class be destroyed.
  void Shutdown();

  /// Returns a \em raw pointer to the underlying \a grpc_completion_queue
  /// instance.
  ///
  /// \warning Remember that the returned instance is owned. No transfer of
  /// owership is performed.
  grpc_completion_queue* cq() { return cq_; }

 public:
  NextStatus AsyncNextInternal(bool* ok, gpr_timespec deadline);

  /// Wraps \a grpc_completion_queue_pluck.
  /// \warning Must not be mixed with calls to \a Next.
  bool Pluck(const Tag& tag);

  /// Performs a single polling pluck on \a tag.
  void TryPluck(const Tag& tag);

 private:
  grpc_completion_queue* const cq_;  // owned
};

}  // namespace grpc_cb

#endif  // GRPC_CB_COMPLETION_QUEUE_H
