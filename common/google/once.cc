// Copyright 2008 Google Inc. All Rights Reserved.

#include "common/google/dynamic_annotations.h"
#include "common/google/logging-inl.h"
#include "common/google/mutex.h"
#include "common/google/once.h"
#include "common/google/spinlock_wait.h"
#include "thirdparty/glog/logging.h"

// All modifications to a GoogleOnceType occur inside GoogleOnceInternalInit.
// The fast path reads the variable with an acquire-load..
// This is safe provided we always perform a memory barrier
// immediately before setting the value to GOOGLE_ONCE_INTERNAL_DONE.

void GoogleOnceInternalInit(Atomic32 *control, void (*func)(),
                            void (*func_with_arg)(void*), void* arg) {
  if (DEBUG_MODE) {
    int32 old_control = base::subtle::Acquire_Load(control);
    if (old_control != GOOGLE_ONCE_INTERNAL_INIT &&
        old_control != GOOGLE_ONCE_INTERNAL_RUNNING &&
        old_control != GOOGLE_ONCE_INTERNAL_WAITER &&
        old_control != GOOGLE_ONCE_INTERNAL_DONE) {
      LOG(FATAL) << "Either GoogleOnceType is used in non-static storage "
                    "(where GoogleOnceDynamic might be appropriate), "
                    "or there's a memory corruption.";
    }
  }
  static const base::subtle::SpinLockWaitTransition trans[] = {
    { GOOGLE_ONCE_INTERNAL_INIT, GOOGLE_ONCE_INTERNAL_RUNNING, true },
    { GOOGLE_ONCE_INTERNAL_RUNNING, GOOGLE_ONCE_INTERNAL_WAITER, false },
    { GOOGLE_ONCE_INTERNAL_DONE, GOOGLE_ONCE_INTERNAL_DONE, true }
  };
  // Short circuit the simplest case to avoid procedure call overhead.
  if (base::subtle::Acquire_CompareAndSwap(control, GOOGLE_ONCE_INTERNAL_INIT,
          GOOGLE_ONCE_INTERNAL_RUNNING) == GOOGLE_ONCE_INTERNAL_INIT ||
      base::subtle::SpinLockWait(control, ARRAYSIZE(trans), trans) ==
      GOOGLE_ONCE_INTERNAL_INIT) {
    if (func != 0) {
      (*func)();
    } else {
      (*func_with_arg)(arg);
    }
    ANNOTATE_HAPPENS_BEFORE(control);
    int32 old_control = base::subtle::NoBarrier_Load(control);
    base::subtle::Release_Store(control, GOOGLE_ONCE_INTERNAL_DONE);
    if (old_control == GOOGLE_ONCE_INTERNAL_WAITER) {
      base::subtle::SpinLockWake(control, true);
    }
  } // else *control is already GOOGLE_ONCE_INTERNAL_DONE
}
