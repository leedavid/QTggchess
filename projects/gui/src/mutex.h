/*
  This file is part of Leela Chess Zero.
  Copyright (C) 2018 The LCZero Authors

  Leela Chess is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Leela Chess is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Leela Chess.  If not, see <http://www.gnu.org/licenses/>.

  Additional permission under GNU GPL version 3 section 7

  If you modify this Program, or any covered work, by linking or
  combining it with NVIDIA Corporation's libraries from the NVIDIA CUDA
  Toolkit and the NVIDIA CUDA Deep Neural Network library (or a
  modified version of those libraries), containing parts covered by the
  terms of the respective license agreement, the licensors of this
  Program grant you additional permission to convey the resulting work.
*/

#pragma once

#include <atomic>
#include <mutex>
#include <shared_mutex>
//#include "cppattributes.h"

#if defined(__clang__) && (!defined(SWIG))
#define ATTRIBUTE__(x) __attribute__((x))
#else
#define ATTRIBUTE__(x)  // no-op
#endif

#define CAPABILITY(x) ATTRIBUTE__(capability(x))
#define SCOPED_CAPABILITY ATTRIBUTE__(scoped_lockable)
#define GUARDED_BY(x) ATTRIBUTE__(guarded_by(x))
#define PT_GUARDED_BY(x) ATTRIBUTE__(pt_guarded_by(x))
#define ACQUIRED_BEFORE(...) ATTRIBUTE__(acquired_before(__VA_ARGS__))
#define ACQUIRED_AFTER(...) ATTRIBUTE__(acquired_after(__VA_ARGS__))
#define REQUIRES(...) ATTRIBUTE__(requires_capability(__VA_ARGS__))
#define REQUIRES_SHARED(...) \
  ATTRIBUTE__(requires_shared_capability(__VA_ARGS__))
#define ACQUIRE(...) ATTRIBUTE__(acquire_capability(__VA_ARGS__))
#define ACQUIRE_SHARED(...) ATTRIBUTE__(acquire_shared_capability(__VA_ARGS__))
#define RELEASE(...) ATTRIBUTE__(release_capability(__VA_ARGS__))
#define RELEASE_SHARED(...) ATTRIBUTE__(release_shared_capability(__VA_ARGS__))
#define TRY_ACQUIRE(...) ATTRIBUTE__(try_acquire_capability(__VA_ARGS__))
#define TRY_ACQUIRE_SHARED(...) \
  ATTRIBUTE__(try_acquire_shared_capability(__VA_ARGS__))
#define EXCLUDES(...) ATTRIBUTE__(locks_excluded(__VA_ARGS__))
#define ASSERT_CAPABILITY(x) ATTRIBUTE__(assert_capability(x))
#define ASSERT_SHARED_CAPABILITY(x) ATTRIBUTE__(assert_shared_capability(x))
#define RETURN_CAPABILITY(x) ATTRIBUTE__(lock_returned(x))
#define PACKED_STRUCT ATTRIBUTE__(packed)

#define NO_THREAD_SAFETY_ANALYSIS ATTRIBUTE__(no_thread_safety_analysis)

namespace Chess {

    // Implementation of reader-preferenced shared mutex. Based on fair shared
    // mutex.
    class CAPABILITY("mutex") RpSharedMutex {
    public:
        RpSharedMutex() : waiting_readers_(0) {}

        void lock() ACQUIRE() {
            while (true) {
                mutex_.lock();
                if (waiting_readers_ == 0) return;
                mutex_.unlock();
            }
        }
        void unlock() RELEASE() { mutex_.unlock(); }
        void lock_shared() ACQUIRE_SHARED() {
            ++waiting_readers_;
            mutex_.lock_shared();
        }
        void unlock_shared() RELEASE_SHARED() {
            --waiting_readers_;
            mutex_.unlock_shared();
        }

    private:
        std::shared_timed_mutex mutex_;
        std::atomic<int> waiting_readers_;
    };

    // std::mutex wrapper for clang thread safety annotation.
    class CAPABILITY("mutex") Mutex {
    public:
        // std::unique_lock<std::mutex> wrapper.
        class SCOPED_CAPABILITY Lock {
        public:
            Lock(Mutex& m) ACQUIRE(m) : lock_(m.get_raw()) {}
            ~Lock() RELEASE() {}
            std::unique_lock<std::mutex>& get_raw() { return lock_; }

        private:
            std::unique_lock<std::mutex> lock_;
        };

        void lock() ACQUIRE() { mutex_.lock(); }
        void unlock() RELEASE() { mutex_.unlock(); }
        std::mutex& get_raw() { return mutex_; }

    private:
        std::mutex mutex_;
    };

    // std::shared_mutex wrapper for clang thread safety annotation.
    class CAPABILITY("mutex") SharedMutex {
    public:
        // std::unique_lock<std::shared_mutex> wrapper.
        class SCOPED_CAPABILITY Lock {
        public:
            Lock(SharedMutex& m) ACQUIRE(m) : lock_(m.get_raw()) {}
            ~Lock() RELEASE() {}

        private:
            std::unique_lock<std::shared_timed_mutex> lock_;
        };

        // std::shared_lock<std::shared_mutex> wrapper.
        class SCOPED_CAPABILITY SharedLock {
        public:
            SharedLock(SharedMutex& m) ACQUIRE_SHARED(m) : lock_(m.get_raw()) {}
            ~SharedLock() RELEASE() {}

        private:
            std::shared_lock<std::shared_timed_mutex> lock_;
        };

        void lock() ACQUIRE() { mutex_.lock(); }
        void unlock() RELEASE() { mutex_.unlock(); }
        void lock_shared() ACQUIRE_SHARED() { mutex_.lock_shared(); }
        void unlock_shared() RELEASE_SHARED() { mutex_.unlock_shared(); }

        std::shared_timed_mutex& get_raw() { return mutex_; }

    private:
        std::shared_timed_mutex mutex_;
    };

}  // namespace lczero
