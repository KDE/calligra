/*
 * This file is part of the KDE project.
 * Copyright © 2010 Michael Pyne <mpyne@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KSHAREDDATACACHE_P_H
#define KSHAREDDATACACHE_P_H

#include <config-caching.h> // HAVE_SYS_MMAN_H

#include <QtCore/QDebug>
#include <QtCore/QSharedPointer>
#include <qbasicatomic.h>

#include <unistd.h> // Check for sched_yield
#include <sched.h>  // sched_yield
#include <errno.h>
#include <fcntl.h>
#include <time.h>

// Mac OS X, for all its POSIX compliance, does not support timeouts on its
// mutexes, which is kind of a disaster for cross-process support. However
// synchronization primitives still work, they just might hang if the cache is
// corrupted, so keep going.
#if defined(_POSIX_TIMEOUTS) && ((_POSIX_TIMEOUTS == 0) || (_POSIX_TIMEOUTS >= 200112L))
#define KSDC_TIMEOUTS_SUPPORTED 1
#endif

#if defined(__GNUC__) && !defined(KSDC_TIMEOUTS_SUPPORTED)
#warning "No support for POSIX timeouts -- application hangs are possible if the cache is corrupt"
#endif

#if defined(_POSIX_THREAD_PROCESS_SHARED) && ((_POSIX_THREAD_PROCESS_SHARED == 0) || (_POSIX_THREAD_PROCESS_SHARED >= 200112L)) && !defined(__APPLE__)
#include <pthread.h>
#define KSDC_THREAD_PROCESS_SHARED_SUPPORTED 1
#endif

#if defined(_POSIX_SEMAPHORES) && ((_POSIX_SEMAPHORES == 0) || (_POSIX_SEMAPHORES >= 200112L))
#include <semaphore.h>
#define KSDC_SEMAPHORES_SUPPORTED 1
#endif

#if defined(__GNUC__) && !defined(KSDC_SEMAPHORES_SUPPORTED) && !defined(KSDC_THREAD_PROCESS_SHARED_SUPPORTED)
#warning "No system support claimed for process-shared synchronization, KSharedDataCache will be mostly useless."
#endif

#if defined(_POSIX_MAPPED_FILES) && ((_POSIX_MAPPED_FILES == 0) || (_POSIX_MAPPED_FILES >= 200112L))
#define KSDC_MAPPED_FILES_SUPPORTED 1
#endif

#if defined(_POSIX_SYNCHRONIZED_IO) && ((_POSIX_SYNCHRONIZED_IO == 0) || (_POSIX_SYNCHRONIZED_IO >= 200112L))
#define KSDC_SYNCHRONIZED_IO_SUPPORTED 1
#endif

// msync(2) requires both MAPPED_FILES and SYNCHRONIZED_IO POSIX options
#if defined(KSDC_MAPPED_FILES_SUPPORTED) && defined(KSDC_SYNCHRONIZED_IO_SUPPORTED)
#define KSDC_MSYNC_SUPPORTED
#endif

// posix_fallocate is used to ensure that the file used for the cache is
// actually fully committed to disk before attempting to use the file.
#if defined(_POSIX_ADVISORY_INFO) && ((_POSIX_ADVISORY_INFO == 0) || (_POSIX_ADVISORY_INFO >= 200112L))
#define KSDC_POSIX_FALLOCATE_SUPPORTED 1
#endif

// BSD/Mac OS X compat
#if HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
#define MAP_ANONYMOUS MAP_ANON
#endif

/**
 * This class defines an interface used by KSharedDataCache::Private to offload
 * proper locking and unlocking depending on what the platform supports at
 * runtime and compile-time.
 */
class KSDCLock {
public:
    virtual ~KSDCLock()
    {
    }

    // Return value indicates if the mutex was properly initialized (including
    // threads-only as a fallback).
    virtual bool initialize(bool &processSharingSupported)
    {
        processSharingSupported = false;
        return false;
    }

    virtual bool lock()
    {
        return false;
    }

    virtual void unlock()
    {
    }
};

/**
 * This is a very basic lock that should work on any system where GCC atomic
 * intrinsics are supported. It can waste CPU so better primitives should be
 * used if available on the system.
 */
class simpleSpinLock : public KSDCLock
{
public:
    simpleSpinLock(QBasicAtomicInt &spinlock)
        : m_spinlock(spinlock)
    {
    }

    virtual bool initialize(bool &processSharingSupported)
    {
        // Clear the spinlock
        m_spinlock.store(0);
        processSharingSupported = true;
        return true;
    }

    virtual bool lock()
    {
        // Spin a few times attempting to gain the lock, as upper-level code won't
        // attempt again without assuming the cache is corrupt.
        for (unsigned i = 50; i > 0; --i) {
            if (m_spinlock.testAndSetAcquire(0, 1)) {
                return true;
            }

            // Don't steal the processor and starve the thread we're waiting
            // on.
            loopSpinPause();
        }

        return false;
    }

    virtual void unlock()
    {
        m_spinlock.testAndSetRelease(1, 0);
    }

private:
#ifdef Q_CC_GNU
    __attribute__((always_inline, gnu_inline, artificial))
#endif
    static inline void loopSpinPause()
    {
        // TODO: Spinning might be better in multi-core systems... but that means
        // figuring how to find numbers of CPUs in a cross-platform way.
#ifdef _POSIX_PRIORITY_SCHEDULING
        sched_yield();
#else
        // Sleep for shortest possible time (nanosleep should round-up).
        struct timespec wait_time = { 0 /* sec */, 100 /* ns */ };
        ::nanosleep(&wait_time, static_cast<struct timespec*>(0));
#endif
    }

    QBasicAtomicInt &m_spinlock;
};

#ifdef KSDC_THREAD_PROCESS_SHARED_SUPPORTED
class pthreadLock : public KSDCLock
{
public:
    pthreadLock(pthread_mutex_t &mutex)
        : m_mutex(mutex)
    {
    }

    virtual bool initialize(bool &processSharingSupported)
    {
        // Setup process-sharing.
        pthread_mutexattr_t mutexAttr;
        processSharingSupported = false;

        // Initialize attributes, enable process-shared primitives, and setup
        // the mutex.
        if (::sysconf(_SC_THREAD_PROCESS_SHARED) >= 200112L && pthread_mutexattr_init(&mutexAttr) == 0) {
            if (pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED) == 0 &&
                pthread_mutex_init(&m_mutex, &mutexAttr) == 0)
            {
                processSharingSupported = true;
            }
            pthread_mutexattr_destroy(&mutexAttr);
        }

        // Attempt to setup for thread-only synchronization.
        if (!processSharingSupported && pthread_mutex_init(&m_mutex, NULL) != 0) {
            return false;
        }

        return true;
    }

    virtual bool lock()
    {
        return pthread_mutex_lock(&m_mutex) == 0;
    }

    virtual void unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }

protected:
    pthread_mutex_t &m_mutex;
};
#endif

#if defined(KSDC_THREAD_PROCESS_SHARED_SUPPORTED) && defined(KSDC_TIMEOUTS_SUPPORTED)
class pthreadTimedLock : public pthreadLock
{
public:
    pthreadTimedLock(pthread_mutex_t &mutex)
        : pthreadLock(mutex)
    {
    }

    virtual bool lock()
    {
        struct timespec timeout;

        // Long timeout, but if we fail to meet this timeout it's probably a cache
        // corruption (and if we take 8 seconds then it should be much much quicker
        // the next time anyways since we'd be paged back in from disk)
        timeout.tv_sec = 10 + ::time(NULL); // Absolute time, so 10 seconds from now
        timeout.tv_nsec = 0;

        return pthread_mutex_timedlock(&m_mutex, &timeout) == 0;
    }
};
#endif

#ifdef KSDC_SEMAPHORES_SUPPORTED
class semaphoreLock : public KSDCLock
{
public:
    semaphoreLock(sem_t &semaphore)
        : m_semaphore(semaphore)
    {
    }

    virtual bool initialize(bool &processSharingSupported)
    {
        processSharingSupported = false;
        if (::sysconf(_SC_SEMAPHORES) < 200112L) {
            return false;
        }

        // sem_init sets up process-sharing for us.
        if (sem_init(&m_semaphore, 1, 1) == 0) {
            processSharingSupported = true;
        }
        // If not successful try falling back to thread-shared.
        else if (sem_init(&m_semaphore, 0, 1) != 0) {
            return false;
        }

        return true;
    }

    virtual bool lock()
    {
        return sem_wait(&m_semaphore) == 0;
    }

    virtual void unlock()
    {
        sem_post(&m_semaphore);
    }

protected:
    sem_t &m_semaphore;
};
#endif

#if defined(KSDC_SEMAPHORES_SUPPORTED) && defined(KSDC_TIMEOUTS_SUPPORTED)
class semaphoreTimedLock : public semaphoreLock
{
public:
    semaphoreTimedLock(sem_t &semaphore)
        : semaphoreLock(semaphore)
    {
    }

    virtual bool lock()
    {
        struct timespec timeout;

        // Long timeout, but if we fail to meet this timeout it's probably a cache
        // corruption (and if we take 8 seconds then it should be much much quicker
        // the next time anyways since we'd be paged back in from disk)
        timeout.tv_sec = 10 + ::time(NULL); // Absolute time, so 10 seconds from now
        timeout.tv_nsec = 0;

        return sem_timedwait(&m_semaphore, &timeout) == 0;
    }
};
#endif

// This enum controls the type of the locking used for the cache to allow
// for as much portability as possible. This value will be stored in the
// cache and used by multiple processes, therefore you should consider this
// a versioned field, do not re-arrange.
enum SharedLockId {
    LOCKTYPE_INVALID   = 0,
    LOCKTYPE_MUTEX     = 1,  // pthread_mutex
    LOCKTYPE_SEMAPHORE = 2,  // sem_t
    LOCKTYPE_SPINLOCK  = 3   // atomic int in shared memory
};

// This type is a union of all possible lock types, with a SharedLockId used
// to choose which one is actually in use.
struct SharedLock
{
    union
    {
#if defined(KSDC_THREAD_PROCESS_SHARED_SUPPORTED)
        pthread_mutex_t mutex;
#endif
#if defined(KSDC_SEMAPHORES_SUPPORTED)
        sem_t semaphore;
#endif
        QBasicAtomicInt spinlock;

        // It would be highly unfortunate if a simple glibc upgrade or kernel
        // addition caused this structure to change size when an existing
        // lock was thought present, so reserve enough size to cover any
        // reasonable locking structure
        char unused[64];
    };

    SharedLockId type;
};

/**
 * This is a method to determine the best lock type to use for a
 * shared cache, based on local support. An identifier to the appropriate
 * SharedLockId is returned, which can be passed to createLockFromId().
 */
static SharedLockId findBestSharedLock()
{
    // We would prefer a process-shared capability that also supports
    // timeouts. Failing that, process-shared is preferred over timeout
    // support. Failing that we'll go thread-local
    bool timeoutsSupported = false;
    bool pthreadsProcessShared = false;
    bool semaphoresProcessShared = false;

#ifdef KSDC_TIMEOUTS_SUPPORTED
    timeoutsSupported = ::sysconf(_SC_TIMEOUTS) >= 200112L;
#endif

    // Now that we've queried timeouts, try actually creating real locks and
    // seeing if there's issues with that.
#ifdef KSDC_THREAD_PROCESS_SHARED_SUPPORTED
    {
        pthread_mutex_t tempMutex;
        QSharedPointer<KSDCLock> tempLock(0);
        if (timeoutsSupported) {
#ifdef KSDC_TIMEOUTS_SUPPORTED
            tempLock = QSharedPointer<KSDCLock>(new pthreadTimedLock(tempMutex));
#endif
        }
        else {
            tempLock = QSharedPointer<KSDCLock>(new pthreadLock(tempMutex));
        }

        tempLock->initialize(pthreadsProcessShared);
    }
#endif

    // Our first choice is pthread_mutex_t for compatibility.
    if(timeoutsSupported && pthreadsProcessShared) {
        return LOCKTYPE_MUTEX;
    }

#ifdef KSDC_SEMAPHORES_SUPPORTED
    {
        sem_t tempSemaphore;
        QSharedPointer<KSDCLock> tempLock(0);
        if (timeoutsSupported) {
            tempLock = QSharedPointer<KSDCLock>(new semaphoreTimedLock(tempSemaphore));
        }
        else {
            tempLock = QSharedPointer<KSDCLock>(new semaphoreLock(tempSemaphore));
        }

        tempLock->initialize(semaphoresProcessShared);
    }
#endif

    if(timeoutsSupported && semaphoresProcessShared) {
        return LOCKTYPE_SEMAPHORE;
    }
    else if(pthreadsProcessShared) {
        return LOCKTYPE_MUTEX;
    }
    else if(semaphoresProcessShared) {
        return LOCKTYPE_SEMAPHORE;
    }

    // Fallback to a dumb-simple but possibly-CPU-wasteful solution.
    return LOCKTYPE_SPINLOCK;
}

static KSDCLock *createLockFromId(SharedLockId id, SharedLock &lock)
{
    switch(id) {
#ifdef KSDC_THREAD_PROCESS_SHARED_SUPPORTED
    case LOCKTYPE_MUTEX:
#ifdef KSDC_TIMEOUTS_SUPPORTED
        if (::sysconf(_SC_TIMEOUTS) >= 200112L) {
            return new pthreadTimedLock(lock.mutex);
        }
#endif
        return new pthreadLock(lock.mutex);

    break;
#endif

#ifdef KSDC_SEMAPHORES_SUPPORTED
    case LOCKTYPE_SEMAPHORE:
#ifdef KSDC_TIMEOUTS_SUPPORTED
        if (::sysconf(_SC_SEMAPHORES) >= 200112L) {
            return new semaphoreTimedLock(lock.semaphore);
        }
#endif
        return new semaphoreLock(lock.semaphore);

    break;
#endif

    case LOCKTYPE_SPINLOCK:
        return new simpleSpinLock(lock.spinlock);
    break;

    default:
        qCritical() << "Creating shell of a lock!";
        return new KSDCLock;
    }
}

static bool ensureFileAllocated(int fd, size_t fileSize)
{
#ifdef KSDC_POSIX_FALLOCATE_SUPPORTED
    int result;
    while ((result = ::posix_fallocate(fd, 0, fileSize)) == EINTR) {
        ;
    }

    if (result < 0) {
        qCritical() << "The operating system is unable to promise"
                           << fileSize
                           << "bytes for mapped cache, "
                              "abandoning the cache for crash-safety.";
        return false;
    }

    return true;
#else

#ifdef __GNUC__
#warning "This system does not seem to support posix_fallocate, which is needed to ensure KSharedDataCache's underlying files are fully committed to disk to avoid crashes with low disk space."
#endif
    qWarning() << "This system misses support for posix_fallocate()"
                            " -- ensure this partition has room for at least"
                         << fileSize << "bytes.";

    // TODO: It's possible to emulate the functionality, but doing so
    // overwrites the data in the file so we don't do this. If you were to add
    // this emulation, you must ensure it only happens on initial creation of a
    // new file and not just mapping an existing cache.

    return true;
#endif
}

#endif /* KSHAREDDATACACHE_P_H */
