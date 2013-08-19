/*
 * This file is part of the KDE project.
 * Copyright © 2010, 2012 Michael Pyne <mpyne@kde.org>
 * Copyright © 2012 Ralf Jung <ralfjung-e@gmx.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library includes "MurmurHash" code from Austin Appleby, which is
 * placed in the public domain. See http://sites.google.com/site/murmurhash/
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

#include "kshareddatacache.h"
#include "kshareddatacache_p.h" // Various auxiliary support code

#include "qstandardpaths.h"

#include <krandom.h>

#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QSharedPointer>
#include <QtCore/QByteArray>
#include <QtCore/QFile>
#include <QtCore/QAtomicInt>
#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QDir>

#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>

/// The maximum number of probes to make while searching for a bucket in
/// the presence of collisions in the cache index table.
static const uint MAX_PROBE_COUNT = 6;

/**
 * A very simple class whose only purpose is to be thrown as an exception from
 * underlying code to indicate that the shared cache is apparently corrupt.
 * This must be caught by top-level library code and used to unlink the cache
 * in this circumstance.
 *
 * @internal
 */
class KSDCCorrupted
{
    public:
    KSDCCorrupted()
    {
        qCritical() << "Error detected in cache, re-generating";
    }
};

//-----------------------------------------------------------------------------
// MurmurHashAligned, by Austin Appleby
// (Released to the public domain, or licensed under the MIT license where
// software may not be released to the public domain. See
// http://sites.google.com/site/murmurhash/)

// Same algorithm as MurmurHash, but only does aligned reads - should be safer
// on certain platforms.
static unsigned int MurmurHashAligned(const void *key, int len, unsigned int seed)
{
    const unsigned int m = 0xc6a4a793;
    const int r = 16;

    const unsigned char * data = reinterpret_cast<const unsigned char *>(key);

    unsigned int h = seed ^ (len * m);

    int align = reinterpret_cast<quintptr>(data) & 3;

    if(align & (len >= 4))
    {
        // Pre-load the temp registers

        unsigned int t = 0, d = 0;

        switch(align)
        {
            case 1: t |= data[2] << 16;
            case 2: t |= data[1] << 8;
            case 3: t |= data[0];
        }

        t <<= (8 * align);

        data += 4-align;
        len -= 4-align;

        int sl = 8 * (4-align);
        int sr = 8 * align;

        // Mix

        while(len >= 4)
        {
            d = *reinterpret_cast<const unsigned int *>(data);
            t = (t >> sr) | (d << sl);
            h += t;
            h *= m;
            h ^= h >> r;
            t = d;

            data += 4;
            len -= 4;
        }

        // Handle leftover data in temp registers

        int pack = len < align ? len : align;

        d = 0;

        switch(pack)
        {
        case 3: d |= data[2] << 16;
        case 2: d |= data[1] << 8;
        case 1: d |= data[0];
        case 0: h += (t >> sr) | (d << sl);
                h *= m;
                h ^= h >> r;
        }

        data += pack;
        len -= pack;
    }
    else
    {
        while(len >= 4)
        {
            h += *reinterpret_cast<const unsigned int *>(data);
            h *= m;
            h ^= h >> r;

            data += 4;
            len -= 4;
        }
    }

    //----------
    // Handle tail bytes

    switch(len)
    {
    case 3: h += data[2] << 16;
    case 2: h += data[1] << 8;
    case 1: h += data[0];
            h *= m;
            h ^= h >> r;
    };

    h *= m;
    h ^= h >> 10;
    h *= m;
    h ^= h >> 17;

    return h;
}

/**
 * This is the hash function used for our data to hopefully make the
 * hashing used to place the QByteArrays as efficient as possible.
 */
static quint32 generateHash(const QByteArray &buffer)
{
    // The final constant is the "seed" for MurmurHash. Do *not* change it
    // without incrementing the cache version.
    return MurmurHashAligned(buffer.data(), buffer.size(), 0xF0F00F0F);
}

// Alignment concerns become a big deal when we're dealing with shared memory,
// since trying to access a structure sized at, say 8 bytes at an address that
// is not evenly divisible by 8 is a crash-inducing error on some
// architectures. The compiler would normally take care of this, but with
// shared memory the compiler will not necessarily know the alignment expected,
// so make sure we account for this ourselves. To do so we need a way to find
// out the expected alignment. Enter ALIGNOF...
#ifndef ALIGNOF
#if defined(Q_CC_GNU) || defined(Q_CC_SUN)
#define ALIGNOF(x) (__alignof__ (x)) // GCC provides what we want directly
#else

#include <stddef.h> // offsetof

template<class T>
struct __alignmentHack
{
    char firstEntry;
    T    obj;
    static const size_t size = offsetof(__alignmentHack, obj);
};
#define ALIGNOF(x) (__alignmentHack<x>::size)
#endif // Non gcc
#endif // ALIGNOF undefined

// Returns a pointer properly aligned to handle size alignment.
// size should be a power of 2. start is assumed to be the lowest
// permissible address, therefore the return value will be >= start.
template<class T>
T* alignTo(const void *start, uint size = ALIGNOF(T))
{
    quintptr mask = size - 1;

    // Cast to int-type to handle bit-twiddling
    quintptr basePointer = reinterpret_cast<quintptr>(start);

    // If (and only if) we are already aligned, adding mask into basePointer
    // will not increment any of the bits in ~mask and we get the right answer.
    basePointer = (basePointer + mask) & ~mask;

    return reinterpret_cast<T *>(basePointer);
}

/**
 * Returns a pointer to a const object of type T, assumed to be @p offset
 * *BYTES* greater than the base address. Note that in order to meet alignment
 * requirements for T, it is possible that the returned pointer points greater
 * than @p offset into @p base.
 */
template<class T>
const T *offsetAs(const void *const base, qint32 offset)
{
    const char *ptr = reinterpret_cast<const char*>(base);
    return alignTo<const T>(ptr + offset);
}

// Same as above, but for non-const objects
template<class T>
T *offsetAs(void *const base, qint32 offset)
{
    char *ptr = reinterpret_cast<char *>(base);
    return alignTo<T>(ptr + offset);
}

/**
 * @return the smallest integer greater than or equal to (@p a / @p b).
 * @param a Numerator, should be ≥ 0.
 * @param b Denominator, should be > 0.
 */
static unsigned intCeil(unsigned a, unsigned b)
{
    // The overflow check is unsigned and so is actually defined behavior.
    if (Q_UNLIKELY(b == 0 || ((a + b) < a))) {
        throw KSDCCorrupted();
    }

    return (a + b - 1) / b;
}

/**
 * @return number of set bits in @p value (see also "Hamming weight")
 */
static unsigned countSetBits(unsigned value)
{
    // K&R / Wegner's algorithm used. GCC supports __builtin_popcount but we
    // expect there to always be only 1 bit set so this should be perhaps a bit
    // faster 99.9% of the time.
    unsigned count = 0;
    for (count = 0; value != 0; count++) {
        value &= (value - 1); // Clears least-significant set bit.
    }
    return count;
}

typedef qint32 pageID;

// =========================================================================
// Description of the cache:
//
// The shared memory cache is designed to be handled as two separate objects,
// all contained in the same global memory segment. First off, there is the
// basic header data, consisting of the global header followed by the
// accounting data necessary to hold items (described in more detail
// momentarily). Following the accounting data is the start of the "page table"
// (essentially just as you'd see it in an Operating Systems text).
//
// The page table contains shared memory split into fixed-size pages, with a
// configurable page size. In the event that the data is too large to fit into
// a single logical page, it will need to occupy consecutive pages of memory.
//
// The accounting data that was referenced earlier is split into two:
//
// 1. index table, containing a fixed-size list of possible cache entries.
// Each index entry is of type IndexTableEntry (below), and holds the various
// accounting data and a pointer to the first page.
//
// 2. page table, which is used to speed up the process of searching for
// free pages of memory. There is one entry for every page in the page table,
// and it contains the index of the one entry in the index table actually
// holding the page (or <0 if the page is free).
//
// The entire segment looks like so:
// ?════════?═════════════?════════════?═══════?═══════?═══════?═══════?═══?
// ? Header │ Index Table │ Page Table ? Pages │       │       │       │...?
// ?════════?═════════════?════════════?═══════?═══════?═══════?═══════?═══?
// =========================================================================

// All elements of this struct must be "plain old data" (POD) types since it
// will be in shared memory.  In addition, no pointers!  To point to something
// you must use relative offsets since the pointer start addresses will be
// different in each process.
struct IndexTableEntry
{
            uint   fileNameHash;
            uint   totalItemSize; // in bytes
    mutable uint   useCount;
            time_t addTime;
    mutable time_t lastUsedTime;
            pageID firstPage;
};

// Page table entry
struct PageTableEntry
{
    // int so we can use values <0 for unassigned pages.
    qint32 index;
};

// Each individual page contains the cached data. The first page starts off with
// the utf8-encoded key, a null '\0', and then the data follows immediately
// from the next byte, possibly crossing consecutive page boundaries to hold
// all of the data.
// There is, however, no specific struct for a page, it is simply a location in
// memory.

// This is effectively the layout of the shared memory segment. The variables
// contained within form the header, data contained afterwards is pointed to
// by using special accessor functions.
struct SharedMemory
{
    /**
     * Note to downstream packagers: This version flag is intended to be
     * machine-specific. The KDE-provided source code will not set the lower
     * two bits to allow for distribution-specific needs, with the exception
     * of version 1 which was already defined in KDE Platform 4.5.
     * e.g. the next version bump will be from 4 to 8, then 12, etc.
     */
    enum {
        PIXMAP_CACHE_VERSION = 12,
        MINIMUM_CACHE_SIZE = 4096
    };

    // Note to those who follow me. You should not, under any circumstances, ever
    // re-arrange the following two fields, even if you change the version number
    // for later revisions of this code.
    QAtomicInt ready; ///< DO NOT INITIALIZE
    quint8     version;

    // See kshareddatacache_p.h
    SharedLock shmLock;

    uint       cacheSize;
    uint       cacheAvail;
    QAtomicInt evictionPolicy;

    // pageSize and cacheSize determine the number of pages. The number of
    // pages determine the page table size and (indirectly) the index table
    // size.
    QAtomicInt pageSize;

    // This variable is added to reserve space for later cache timestamping
    // support. The idea is this variable will be updated when the cache is
    // written to, to allow clients to detect a changed cache quickly.
    QAtomicInt cacheTimestamp;

    /**
     * Converts the given average item size into an appropriate page size.
     */
    static unsigned equivalentPageSize(unsigned itemSize)
    {
        if (itemSize == 0) {
            return 4096; // Default average item size.
        }

        int log2OfSize = 0;
        while ((itemSize >>= 1) != 0) {
            log2OfSize++;
        }

        // Bound page size between 512 bytes and 256 KiB.
        // If this is adjusted, also alter validSizeMask in cachePageSize
        log2OfSize = qBound(9, log2OfSize, 18);

        return (1 << log2OfSize);
    }

    // Returns pageSize in unsigned format.
    unsigned cachePageSize() const
    {
        unsigned _pageSize = static_cast<unsigned>(pageSize.load());
        // bits 9-18 may be set.
        static const unsigned validSizeMask = 0x7FE00u;

        // Check for page sizes that are not a power-of-2, or are too low/high.
        if (Q_UNLIKELY(countSetBits(_pageSize) != 1 || (_pageSize & ~validSizeMask))) {
            throw KSDCCorrupted();
        }

        return _pageSize;
    }

    /**
     * This is effectively the class ctor.  But since we're in shared memory,
     * there's a few rules:
     *
     * 1. To allow for some form of locking in the initial-setup case, we
     * use an atomic int, which will be initialized to 0 by mmap().  Then to
     * take the lock we atomically increment the 0 to 1.  If we end up calling
     * the QAtomicInt constructor we can mess that up, so we can't use a
     * constructor for this class either.
     * 2. Any member variable you add takes up space in shared memory as well,
     * so make sure you need it.
     */
    bool performInitialSetup(uint _cacheSize, uint _pageSize)
    {
        if (_cacheSize < MINIMUM_CACHE_SIZE) {
            qCritical() << "Internal error: Attempted to create a cache sized < "
                        << MINIMUM_CACHE_SIZE;
            return false;
        }

        if (_pageSize == 0) {
            qCritical() << "Internal error: Attempted to create a cache with 0-sized pages.";
            return false;
        }

        shmLock.type = findBestSharedLock();
        if (shmLock.type == LOCKTYPE_INVALID) {
            qCritical() << "Unable to find an appropriate lock to guard the shared cache. "
                        << "This *should* be essentially impossible. :(";
            return false;
        }

        bool isProcessShared = false;
        QSharedPointer<KSDCLock> tempLock(createLockFromId(shmLock.type, shmLock));

        if (!tempLock->initialize(isProcessShared)) {
            qCritical() << "Unable to initialize the lock for the cache!";
            return false;
        }

        if (!isProcessShared) {
            qWarning() << "Cache initialized, but does not support being"
                          << "shared across processes.";
        }

        // These must be updated to make some of our auxiliary functions
        // work right since their values will be based on the cache size.
        cacheSize = _cacheSize;
        pageSize = _pageSize;
        version = PIXMAP_CACHE_VERSION;
        cacheTimestamp = static_cast<unsigned>(::time(0));

        clearInternalTables();

        // Unlock the mini-lock, and introduce a total memory barrier to make
        // sure all changes have propagated even without a mutex.
        ready.ref();

        return true;
    }

    void clearInternalTables()
    {
        // Assumes we're already locked somehow.
        cacheAvail = pageTableSize();

        // Setup page tables to point nowhere
        PageTableEntry *table = pageTable();
        for (uint i = 0; i < pageTableSize(); ++i) {
            table[i].index = -1;
        }

        // Setup index tables to be accurate.
        IndexTableEntry *indices = indexTable();
        for (uint i = 0; i < indexTableSize(); ++i) {
            indices[i].firstPage = -1;
            indices[i].useCount = 0;
            indices[i].fileNameHash = 0;
            indices[i].totalItemSize = 0;
            indices[i].addTime = 0;
            indices[i].lastUsedTime = 0;
        }
    }

    const IndexTableEntry *indexTable() const
    {
        // Index Table goes immediately after this struct, at the first byte
        // where alignment constraints are met (accounted for by offsetAs).
        return offsetAs<IndexTableEntry>(this, sizeof(*this));
    }

    const PageTableEntry *pageTable() const
    {
        const IndexTableEntry *base = indexTable();
        base += indexTableSize();

        // Let's call wherever we end up the start of the page table...
        return alignTo<PageTableEntry>(base);
    }

    const void *cachePages() const
    {
        const PageTableEntry *tableStart = pageTable();
        tableStart += pageTableSize();

        // Let's call wherever we end up the start of the data...
        return alignTo<void>(tableStart, cachePageSize());
    }

    const void *page(pageID at) const
    {
        if (static_cast<uint>(at) >= pageTableSize()) {
            return 0;
        }

        // We must manually calculate this one since pageSize varies.
        const char *pageStart = reinterpret_cast<const char *>(cachePages());
        pageStart += (at * cachePageSize());

        return reinterpret_cast<const void *>(pageStart);
    }

    // The following are non-const versions of some of the methods defined
    // above.  They use const_cast<> because I feel that is better than
    // duplicating the code.  I suppose template member functions (?)
    // may work, may investigate later.
    IndexTableEntry *indexTable()
    {
        const SharedMemory *that = const_cast<const SharedMemory*>(this);
        return const_cast<IndexTableEntry *>(that->indexTable());
    }

    PageTableEntry *pageTable()
    {
        const SharedMemory *that = const_cast<const SharedMemory*>(this);
        return const_cast<PageTableEntry *>(that->pageTable());
    }

    void *cachePages()
    {
        const SharedMemory *that = const_cast<const SharedMemory*>(this);
        return const_cast<void *>(that->cachePages());
    }

    void *page(pageID at)
    {
        const SharedMemory *that = const_cast<const SharedMemory*>(this);
        return const_cast<void *>(that->page(at));
    }

    uint pageTableSize() const
    {
        return cacheSize / cachePageSize();
    }

    uint indexTableSize() const
    {
        // Assume 2 pages on average are needed -> the number of entries
        // would be half of the number of pages.
        return pageTableSize() / 2;
    }

    /**
     * @return the index of the first page, for the set of contiguous
     * pages that can hold @p pagesNeeded PAGES.
     */
    pageID findEmptyPages(uint pagesNeeded) const
    {
        if (Q_UNLIKELY(pagesNeeded > pageTableSize())) {
            return pageTableSize();
        }

        // Loop through the page table, find the first empty page, and just
        // makes sure that there are enough free pages.
        const PageTableEntry *table = pageTable();
        uint contiguousPagesFound = 0;
        pageID base = 0;
        for (pageID i = 0; i < static_cast<int>(pageTableSize()); ++i) {
            if (table[i].index < 0) {
                if (contiguousPagesFound == 0) {
                    base = i;
                }
                contiguousPagesFound++;
            }
            else {
                contiguousPagesFound = 0;
            }

            if (contiguousPagesFound == pagesNeeded) {
                return base;
            }
        }

        return pageTableSize();
    }

    // left < right?
    static bool lruCompare(const IndexTableEntry &l, const IndexTableEntry &r)
    {
        // Ensure invalid entries migrate to the end
        if (l.firstPage < 0 && r.firstPage >= 0) {
            return false;
        }
        if (l.firstPage >= 0 && r.firstPage < 0) {
            return true;
        }

        // Most recently used will have the highest absolute time =>
        // least recently used (lowest) should go first => use left < right
        return l.lastUsedTime < r.lastUsedTime;
    }

    // left < right?
    static bool seldomUsedCompare(const IndexTableEntry &l, const IndexTableEntry &r)
    {
        // Ensure invalid entries migrate to the end
        if (l.firstPage < 0 && r.firstPage >= 0) {
            return false;
        }
        if (l.firstPage >= 0 && r.firstPage < 0) {
            return true;
        }

        // Put lowest use count at start by using left < right
        return l.useCount < r.useCount;
    }

    // left < right?
    static bool ageCompare(const IndexTableEntry &l, const IndexTableEntry &r)
    {
        // Ensure invalid entries migrate to the end
        if (l.firstPage < 0 && r.firstPage >= 0) {
            return false;
        }
        if (l.firstPage >= 0 && r.firstPage < 0) {
            return true;
        }

        // Oldest entries die first -- they have the lowest absolute add time,
        // so just like the others use left < right
        return l.addTime < r.addTime;
    }

    void defragment()
    {
        if (cacheAvail * cachePageSize() == cacheSize) {
            return; // That was easy
        }

        qDebug() << "Defragmenting the shared cache";

        // Just do a linear scan, and anytime there is free space, swap it
        // with the pages to its right. In order to meet the precondition
        // we need to skip any used pages first.

        pageID currentPage = 0;
        pageID idLimit = static_cast<pageID>(pageTableSize());
        PageTableEntry *pages = pageTable();

        if (Q_UNLIKELY(!pages || idLimit <= 0)) {
            throw KSDCCorrupted();
        }

        // Skip used pages
        while (currentPage < idLimit && pages[currentPage].index >= 0) {
            ++currentPage;
        }

        pageID freeSpot = currentPage;

        // Main loop, starting from a free page, skip to the used pages and
        // move them back.
        while (currentPage < idLimit) {
            // Find the next used page
            while (currentPage < idLimit && pages[currentPage].index < 0) {
                ++currentPage;
            }

            if (currentPage >= idLimit) {
                break;
            }

            // Found an entry, move it.
            qint32 affectedIndex = pages[currentPage].index;
            if (Q_UNLIKELY(affectedIndex < 0 ||
                        affectedIndex >= idLimit ||
                        indexTable()[affectedIndex].firstPage != currentPage))
            {
                throw KSDCCorrupted();
            }

            indexTable()[affectedIndex].firstPage = freeSpot;

            // Moving one page at a time guarantees we can use memcpy safely
            // (in other words, the source and destination will not overlap).
            while (currentPage < idLimit && pages[currentPage].index >= 0) {
                const void *const sourcePage = page(currentPage);
                void *const destinationPage = page(freeSpot);

                if(Q_UNLIKELY(!sourcePage || !destinationPage)) {
                    throw KSDCCorrupted();
                }

                ::memcpy(destinationPage, sourcePage, cachePageSize());
                pages[freeSpot].index = affectedIndex;
                pages[currentPage].index = -1;
                ++currentPage;
                ++freeSpot;

                // If we've just moved the very last page and it happened to
                // be at the very end of the cache then we're done.
                if (currentPage >= idLimit) {
                    break;
                }

                // We're moving consecutive used pages whether they belong to
                // our affected entry or not, so detect if we've started moving
                // the data for a different entry and adjust if necessary.
                if (affectedIndex != pages[currentPage].index) {
                    indexTable()[pages[currentPage].index].firstPage = freeSpot;
                }
                affectedIndex = pages[currentPage].index;
            }

            // At this point currentPage is on a page that is unused, and the
            // cycle repeats. However, currentPage is not the first unused
            // page, freeSpot is, so leave it alone.
        }
    }

    /**
     * Finds the index entry for a given key.
     * @param key UTF-8 encoded key to search for.
     * @return The index of the entry in the cache named by @p key. Returns
     *         <0 if no such entry is present.
     */
    qint32 findNamedEntry(const QByteArray &key) const
    {
        uint keyHash = generateHash(key);
        uint position = keyHash % indexTableSize();
        uint probeNumber = 1; // See insert() for description

        // Imagine 3 entries A, B, C in this logical probing chain. If B is
        // later removed then we can't find C either. So, we must keep
        // searching for probeNumber number of tries (or we find the item,
        // obviously).
        while (indexTable()[position].fileNameHash != keyHash &&
               probeNumber < MAX_PROBE_COUNT)
        {
            position = (keyHash + (probeNumber + probeNumber * probeNumber) / 2)
                       % indexTableSize();
            probeNumber++;
        }

        if (indexTable()[position].fileNameHash == keyHash) {
            pageID firstPage = indexTable()[position].firstPage;
            if (firstPage < 0 || static_cast<uint>(firstPage) >= pageTableSize()) {
                return -1;
            }

            const void *resultPage = page(firstPage);
            if (Q_UNLIKELY(!resultPage)) {
                throw KSDCCorrupted();
            }

            const char *utf8FileName = reinterpret_cast<const char *>(resultPage);
            if (qstrncmp(utf8FileName, key.constData(), cachePageSize()) == 0) {
                return position;
            }
        }

        return -1; // Not found, or a different one found.
    }

    // Function to use with QSharedPointer in removeUsedPages below...
    static void deleteTable(IndexTableEntry *table) {
        delete [] table;
    }

    /**
     * Removes the requested number of pages.
     *
     * @param numberNeeded the number of pages required to fulfill a current request.
     *        This number should be <0 and <= the number of pages in the cache.
     * @return The identifier of the beginning of a consecutive block of pages able
     *         to fill the request. Returns a value >= pageTableSize() if no such
     *         request can be filled.
     * @internal
     */
    uint removeUsedPages(uint numberNeeded)
    {
        if (numberNeeded == 0) {
            qCritical() << "Internal error: Asked to remove exactly 0 pages for some reason.";
            throw KSDCCorrupted();
        }

        if (numberNeeded > pageTableSize()) {
            qCritical() << "Internal error: Requested more space than exists in the cache.";
            qCritical() << numberNeeded << "requested, " << pageTableSize() << "is the total possible.";
            throw KSDCCorrupted();
        }

        // If the cache free space is large enough we will defragment first
        // instead since it's likely we're highly fragmented.
        // Otherwise, we will (eventually) simply remove entries per the
        // eviction order set for the cache until there is enough room
        // available to hold the number of pages we need.

        qDebug() << "Removing old entries to free up" << numberNeeded << "pages,"
                    << cacheAvail << "are already theoretically available.";

        if (cacheAvail > 3 * numberNeeded) {
            defragment();
            uint result = findEmptyPages(numberNeeded);

            if (result < pageTableSize()) {
                return result;
            }
            else {
                qCritical() << "Just defragmented a locked cache, but still there"
                            << "isn't enough room for the current request.";
            }
        }

        // At this point we know we'll have to free some space up, so sort our
        // list of entries by whatever the current criteria are and start
        // killing expired entries.
        QSharedPointer<IndexTableEntry> tablePtr(new IndexTableEntry[indexTableSize()], deleteTable);

        if (!tablePtr) {
            qCritical() << "Unable to allocate temporary memory for sorting the cache!";
            clearInternalTables();
            throw KSDCCorrupted();
        }

        // We use tablePtr to ensure the data is destroyed, but do the access
        // via a helper pointer to allow for array ops.
        IndexTableEntry *table = tablePtr.data();

        ::memcpy(table, indexTable(), sizeof(IndexTableEntry) * indexTableSize());

        // Our entry ID is simply its index into the
        // index table, which qSort will rearrange all willy-nilly, so first
        // we'll save the *real* entry ID into firstPage (which is useless in
        // our copy of the index table). On the other hand if the entry is not
        // used then we note that with -1.
        for (uint i = 0; i < indexTableSize(); ++i) {
            table[i].firstPage = table[i].useCount > 0 ? static_cast<pageID>(i)
                                                       : -1;
        }

        // Declare the comparison function that we'll use to pass to qSort,
        // based on our cache eviction policy.
        bool (*compareFunction)(const IndexTableEntry &, const IndexTableEntry &);
        switch((int) evictionPolicy.load()) {
        case (int) KSharedDataCache::EvictLeastOftenUsed:
        case (int) KSharedDataCache::NoEvictionPreference:
        default:
            compareFunction = seldomUsedCompare;
        break;

        case (int) KSharedDataCache::EvictLeastRecentlyUsed:
            compareFunction = lruCompare;
        break;

        case (int) KSharedDataCache::EvictOldest:
            compareFunction = ageCompare;
        break;
        }

        qSort(table, table + indexTableSize(), compareFunction);

        // Least recently used entries will be in the front.
        // Start killing until we have room.

        // Note on removeEntry: It expects an index into the index table,
        // but our sorted list is all jumbled. But we stored the real index
        // in the firstPage member.
        // Remove entries until we've removed at least the required number
        // of pages.
        uint i = 0;
        while (i < indexTableSize() && numberNeeded > cacheAvail) {
            int curIndex = table[i++].firstPage; // Really an index, not a page

            // Removed everything, still no luck (or curIndex is set but too high).
            if (curIndex < 0 || static_cast<uint>(curIndex) >= indexTableSize()) {
                qCritical() << "Trying to remove index" << curIndex
                    << "out-of-bounds for index table of size" << indexTableSize();
                throw KSDCCorrupted();
            }

            qDebug() << "Removing entry of" << indexTable()[curIndex].totalItemSize
                        << "size";
            removeEntry(curIndex);
        }

        // At this point let's see if we have freed up enough data by
        // defragmenting first and seeing if we can find that free space.
        defragment();

        pageID result = pageTableSize();
        while (i < indexTableSize() &&
              (static_cast<uint>(result = findEmptyPages(numberNeeded))) >= pageTableSize())
        {
            int curIndex = table[i++].firstPage;

            if (curIndex < 0) {
                // One last shot.
                defragment();
                return findEmptyPages(numberNeeded);
            }

            if (Q_UNLIKELY(static_cast<uint>(curIndex) >= indexTableSize())) {
                throw KSDCCorrupted();
            }

            removeEntry(curIndex);
        }

        // Whew.
        return result;
    }

    // Returns the total size required for a given cache size.
    static uint totalSize(uint cacheSize, uint effectivePageSize)
    {
        uint numberPages = intCeil(cacheSize, effectivePageSize);
        uint indexTableSize = numberPages / 2;

        // Knowing the number of pages, we can determine what addresses we'd be
        // using (properly aligned), and from there determine how much memory
        // we'd use.
        IndexTableEntry *indexTableStart =
                    offsetAs<IndexTableEntry>(static_cast<void*>(0), sizeof (SharedMemory));

        indexTableStart += indexTableSize;

        PageTableEntry *pageTableStart = reinterpret_cast<PageTableEntry *>(indexTableStart);
        pageTableStart = alignTo<PageTableEntry>(pageTableStart);
        pageTableStart += numberPages;

        // The weird part, we must manually adjust the pointer based on the page size.
        char *cacheStart = reinterpret_cast<char *>(pageTableStart);
        cacheStart += (numberPages * effectivePageSize);

        // ALIGNOF gives pointer alignment
        cacheStart = alignTo<char>(cacheStart, ALIGNOF(void*));

        // We've traversed the header, index, page table, and cache.
        // Wherever we're at now is the size of the enchilada.
        return static_cast<uint>(reinterpret_cast<quintptr>(cacheStart));
    }

    uint fileNameHash(const QByteArray &utf8FileName) const
    {
        return generateHash(utf8FileName) % indexTableSize();
    }

    void clear()
    {
        clearInternalTables();
    }

    void removeEntry(uint index);
};

// The per-instance private data, such as map size, whether
// attached or not, pointer to shared memory, etc.
class KSharedDataCache::Private
{
    public:
    Private(const QString &name,
            unsigned defaultCacheSize,
            unsigned expectedItemSize
           )
        : m_cacheName(name)
        , shm(0)
        , m_lock(0)
        , m_mapSize(0)
        , m_defaultCacheSize(defaultCacheSize)
        , m_expectedItemSize(expectedItemSize)
        , m_expectedType(LOCKTYPE_INVALID)
    {
        mapSharedMemory();
    }

    // Put the cache in a condition to be able to call mapSharedMemory() by
    // completely detaching from shared memory (such as to respond to an
    // unrecoverable error).
    // m_mapSize must already be set to the amount of memory mapped to shm.
    void detachFromSharedMemory()
    {
        // The lock holds a reference into shared memory, so this must be
        // cleared before shm is removed.
        m_lock.clear();

        if (shm && 0 != ::munmap(shm, m_mapSize)) {
            qCritical() << "Unable to unmap shared memory segment"
                << static_cast<void*>(shm) << ":" << ::strerror(errno);
        }

        shm = 0;
        m_mapSize = 0;
    }

    // This function does a lot of the important work, attempting to connect to shared
    // memory, a private anonymous mapping if that fails, and failing that, nothing (but
    // the cache remains "valid", we just don't actually do anything).
    void mapSharedMemory()
    {
        // 0-sized caches are fairly useless.
        unsigned cacheSize = qMax(m_defaultCacheSize, uint(SharedMemory::MINIMUM_CACHE_SIZE));
        unsigned pageSize = SharedMemory::equivalentPageSize(m_expectedItemSize);

        // Ensure that the cache is sized such that there is a minimum number of
        // pages available. (i.e. a cache consisting of only 1 page is fairly
        // useless and probably crash-prone).
        cacheSize = qMax(pageSize * 256, cacheSize);

        // The m_cacheName is used to find the file to store the cache in.
        const QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
        if (!QDir().mkpath(cacheDir)) {
            return;
        }
        QString cacheName = cacheDir + QLatin1String("/") + m_cacheName + QLatin1String(".kcache");
        QFile file(cacheName);

        // The basic idea is to open the file that we want to map into shared
        // memory, and then actually establish the mapping. Once we have mapped the
        // file into shared memory we can close the file handle, the mapping will
        // still be maintained (unless the file is resized to be shorter than
        // expected, which we don't handle yet :-( )

        // size accounts for the overhead over the desired cacheSize
        uint size = SharedMemory::totalSize(cacheSize, pageSize);
        void *mapAddress = MAP_FAILED;

        if (size < cacheSize) {
            qCritical() << "Asked for a cache size less than requested size somehow -- Logic Error :(";
            return;
        }

        // We establish the shared memory mapping here, only if we will have appropriate
        // mutex support (systemSupportsProcessSharing), then we:
        // Open the file and resize to some sane value if the file is too small.
        if (file.open(QIODevice::ReadWrite) &&
            (file.size() >= size ||
             (file.resize(size) && ensureFileAllocated(file.handle(), size))))
        {
            // Use mmap directly instead of QFile::map since the QFile (and its
            // shared mapping) will disappear unless we hang onto the QFile for no
            // reason (see the note below, we don't care about the file per se...)
            mapAddress = ::mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, file.handle(), 0);

            // So... it is possible that someone else has mapped this cache already
            // with a larger size. If that's the case we need to at least match
            // the size to be able to access every entry, so fixup the mapping.
            if (mapAddress != MAP_FAILED) {
                SharedMemory *mapped = reinterpret_cast<SharedMemory *>(mapAddress);

                // First make sure that the version of the cache on disk is
                // valid.  We also need to check that version != 0 to
                // disambiguate against an uninitialized cache.
                if (mapped->version != SharedMemory::PIXMAP_CACHE_VERSION &&
                    mapped->version > 0)
                {
                    qWarning() << "Deleting wrong version of cache" << cacheName;

                    // CAUTION: Potentially recursive since the recovery
                    // involves calling this function again.
                    m_mapSize = size;
                    shm = mapped;
                    recoverCorruptedCache();
                    return;
                }
                else if (mapped->cacheSize > cacheSize) {
                    // This order is very important. We must save the cache size
                    // before we remove the mapping, but unmap before overwriting
                    // the previous mapping size...
                    cacheSize = mapped->cacheSize;
                    unsigned actualPageSize = mapped->cachePageSize();
                    ::munmap(mapAddress, size);
                    size = SharedMemory::totalSize(cacheSize, actualPageSize);
                    mapAddress = ::mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, file.handle(), 0);
                }
            }
        }

        // We could be here without the mapping established if:
        // 1) Process-shared synchronization is not supported, either at compile or run time,
        // 2) Unable to open the required file.
        // 3) Unable to resize the file to be large enough.
        // 4) Establishing the mapping failed.
        // 5) The mapping succeeded, but the size was wrong and we were unable to map when
        //    we tried again.
        // 6) The incorrect version of the cache was detected.
        // 7) The file could be created, but posix_fallocate failed to commit it fully to disk.
        // In any of these cases, attempt to fallback to the
        // better-supported anonymous private page style of mmap. This memory won't
        // be shared, but our code will still work the same.
        // NOTE: We never use the on-disk representation independently of the
        // shared memory. If we don't get shared memory the disk info is ignored,
        // if we do get shared memory we never look at disk again.
        if (mapAddress == MAP_FAILED) {
            qWarning() << "Failed to establish shared memory mapping, will fallback"
                          << "to private memory -- memory usage will increase";

            mapAddress = ::mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        }

        // Well now we're really hosed. We can still work, but we can't even cache
        // data.
        if (mapAddress == MAP_FAILED) {
            qCritical() << "Unable to allocate shared memory segment for shared data cache"
                        << cacheName << "of size" << cacheSize;
            return;
        }

        m_mapSize = size;

        // We never actually construct shm, but we assign it the same address as the
        // shared memory we just mapped, so effectively shm is now a SharedMemory that
        // happens to be located at mapAddress.
        shm = reinterpret_cast<SharedMemory *>(mapAddress);

        // If we were first to create this memory map, all data will be 0.
        // Therefore if ready == 0 we're not initialized.  A fully initialized
        // header will have ready == 2.  Why?
        // Because 0 means "safe to initialize"
        //         1 means "in progress of initing"
        //         2 means "ready"
        uint usecSleepTime = 8; // Start by sleeping for 8 microseconds
        while (shm->ready.load() != 2) {
            if (Q_UNLIKELY(usecSleepTime >= (1 << 21))) {
                // Didn't acquire within ~8 seconds?  Assume an issue exists
                qCritical() << "Unable to acquire shared lock, is the cache corrupt?";

                file.remove(); // Unlink the cache in case it's corrupt.
                detachFromSharedMemory();
                return; // Fallback to QCache (later)
            }

            if (shm->ready.testAndSetAcquire(0, 1)) {
                if (!shm->performInitialSetup(cacheSize, pageSize)) {
                    qCritical() << "Unable to perform initial setup, this system probably "
                                   "does not really support process-shared pthreads or "
                                   "semaphores, even though it claims otherwise.";

                    file.remove();
                    detachFromSharedMemory();
                    return;
                }
            }
            else {
                usleep(usecSleepTime); // spin

                // Exponential fallback as in Ethernet and similar collision resolution methods
                usecSleepTime *= 2;
            }
        }

        m_expectedType = shm->shmLock.type;
        m_lock = QSharedPointer<KSDCLock>(createLockFromId(m_expectedType, shm->shmLock));
        bool isProcessSharingSupported = false;

        if (!m_lock->initialize(isProcessSharingSupported)) {
            qCritical() << "Unable to setup shared cache lock, although it worked when created.";
            detachFromSharedMemory();
        }
    }

    // Called whenever the cache is apparently corrupt (for instance, a timeout trying to
    // lock the cache). In this situation it is safer just to destroy it all and try again.
    void recoverCorruptedCache()
    {
        KSharedDataCache::deleteCache(m_cacheName);

        detachFromSharedMemory();

        // Do this even if we weren't previously cached -- it might work now.
        mapSharedMemory();
    }

    // This should be called for any memory access to shared memory. This
    // function will verify that the bytes [base, base+accessLength) are
    // actually mapped to d->shm. The cache itself may have incorrect cache
    // page sizes, incorrect cache size, etc. so this function should be called
    // despite the cache data indicating it should be safe.
    //
    // If the access is /not/ safe then a KSDCCorrupted exception will be
    // thrown, so be ready to catch that.
    void verifyProposedMemoryAccess(const void *base, unsigned accessLength) const
    {
        quintptr startOfAccess = reinterpret_cast<quintptr>(base);
        quintptr startOfShm = reinterpret_cast<quintptr>(shm);

        if (Q_UNLIKELY(startOfAccess < startOfShm)) {
            throw KSDCCorrupted();
        }

        quintptr endOfShm = startOfShm + m_mapSize;
        quintptr endOfAccess = startOfAccess + accessLength;

        // Check for unsigned integer wraparound, and then
        // bounds access
        if (Q_UNLIKELY((endOfShm < startOfShm) ||
                    (endOfAccess < startOfAccess) ||
                    (endOfAccess > endOfShm)))
        {
            throw KSDCCorrupted();
        }
    }

    bool lock() const
    {
        if (Q_LIKELY(shm && shm->shmLock.type == m_expectedType)) {
            return m_lock->lock();
        }

        // No shm or wrong type --> corrupt!
        throw KSDCCorrupted();
    }

    void unlock() const
    {
        m_lock->unlock();
    }

    class CacheLocker
    {
        mutable Private * d;

        bool cautiousLock()
        {
            int lockCount = 0;

            // Locking can fail due to a timeout. If it happens too often even though
            // we're taking corrective action assume there's some disastrous problem
            // and give up.
            while (!d->lock()) {
                d->recoverCorruptedCache();

                if (!d->shm) {
                    qWarning() << "Lost the connection to shared memory for cache"
                                  << d->m_cacheName;
                    return false;
                }

                if (lockCount++ > 4) {
                    qCritical() << "There is a very serious problem with the KDE data cache"
                                << d->m_cacheName << "giving up trying to access cache.";
                    d->detachFromSharedMemory();
                    return false;
                }
            }

            return true;
        }

        public:
        CacheLocker(const Private *_d) : d(const_cast<Private *>(_d))
        {
            if (Q_UNLIKELY(!d || !d->shm || !cautiousLock())) {
                return;
            }

            uint testSize = SharedMemory::totalSize(d->shm->cacheSize, d->shm->cachePageSize());

            // A while loop? Indeed, think what happens if this happens
            // twice -- hard to debug race conditions.
            while (testSize > d->m_mapSize) {
                qDebug() << "Someone enlarged the cache on us,"
                            << "attempting to match new configuration.";

                // Protect against two threads accessing this same KSDC
                // from trying to execute the following remapping at the
                // same time.
                QMutexLocker d_locker(&d->m_threadLock);
                if (testSize == d->m_mapSize) {
                    break; // Bail if the other thread already solved.
                }

                // Linux supports mremap, but it's not portable. So,
                // drop the map and (try to) re-establish.
                d->unlock();

#ifdef KSDC_MSYNC_SUPPORTED
                ::msync(d->shm, d->m_mapSize, MS_INVALIDATE | MS_ASYNC);
#endif
                ::munmap(d->shm, d->m_mapSize);
                d->m_mapSize = 0;
                d->shm = 0;

                QFile f(d->m_cacheName);
                if (!f.open(QFile::ReadWrite)) {
                    qCritical() << "Unable to re-open cache, unfortunately"
                                << "the connection had to be dropped for"
                                << "crash safety -- things will be much"
                                << "slower now.";
                    return;
                }

                void *newMap = ::mmap(0, testSize, PROT_READ | PROT_WRITE,
                                      MAP_SHARED, f.handle(), 0);
                if (newMap == MAP_FAILED) {
                    qCritical() << "Unopen to re-map the cache into memory"
                                << "things will be much slower now";
                    return;
                }

                d->shm = reinterpret_cast<SharedMemory *>(newMap);
                d->m_mapSize = testSize;

                if (!cautiousLock()) {
                    return;
                }

                testSize = SharedMemory::totalSize(d->shm->cacheSize, d->shm->cachePageSize());
            }
        }

        ~CacheLocker()
        {
            if (d && d->shm) {
                d->unlock();
            }
        }

        bool failed() const
        {
            return !d || d->shm == 0;
        }
    };

    QString m_cacheName;
    QMutex m_threadLock;
    SharedMemory *shm;
    QSharedPointer<KSDCLock> m_lock;
    uint m_mapSize;
    uint m_defaultCacheSize;
    uint m_expectedItemSize;
    SharedLockId m_expectedType;
};

// Must be called while the lock is already held!
void SharedMemory::removeEntry(uint index)
{
    if (index >= indexTableSize() || cacheAvail > pageTableSize()) {
        throw KSDCCorrupted();
    }

    PageTableEntry *pageTableEntries = pageTable();
    IndexTableEntry *entriesIndex = indexTable();

    // Update page table first
    pageID firstPage = entriesIndex[index].firstPage;
    if (firstPage < 0 || static_cast<quint32>(firstPage) >= pageTableSize()) {
        qDebug() << "Trying to remove an entry which is already invalid. This "
                    << "cache is likely corrupt.";
        throw KSDCCorrupted();
    }

    if (index != static_cast<uint>(pageTableEntries[firstPage].index)) {
        qCritical() << "Removing entry" << index << "but the matching data"
                    << "doesn't link back -- cache is corrupt, clearing.";
        throw KSDCCorrupted();
    }

    uint entriesToRemove = intCeil(entriesIndex[index].totalItemSize, cachePageSize());
    uint savedCacheSize = cacheAvail;
    for (uint i = firstPage; i < pageTableSize() &&
        (uint) pageTableEntries[i].index == index; ++i)
    {
        pageTableEntries[i].index = -1;
        cacheAvail++;
    }

    if ((cacheAvail - savedCacheSize) != entriesToRemove) {
        qCritical() << "We somehow did not remove" << entriesToRemove
                    << "when removing entry" << index << ", instead we removed"
                    << (cacheAvail - savedCacheSize);
        throw KSDCCorrupted();
    }

    // For debugging
#ifdef NDEBUG
    void *const startOfData = page(firstPage);
    if (startOfData) {
        QByteArray str((const char *) startOfData);
        str.prepend(" REMOVED: ");
        str.prepend(QByteArray::number(index));
        str.prepend("ENTRY ");

        ::memcpy(startOfData, str.constData(), str.size() + 1);
    }
#endif

    // Update the index
    entriesIndex[index].fileNameHash = 0;
    entriesIndex[index].totalItemSize = 0;
    entriesIndex[index].useCount = 0;
    entriesIndex[index].lastUsedTime = 0;
    entriesIndex[index].addTime = 0;
    entriesIndex[index].firstPage = -1;
}

KSharedDataCache::KSharedDataCache(const QString &cacheName,
                                   unsigned defaultCacheSize,
                                   unsigned expectedItemSize)
  : d(0)
{
    try {
        d = new Private(cacheName, defaultCacheSize, expectedItemSize);
    }
    catch(KSDCCorrupted) {
        KSharedDataCache::deleteCache(cacheName);

        // Try only once more
        try {
            d = new Private(cacheName, defaultCacheSize, expectedItemSize);
        }
        catch(KSDCCorrupted) {
            qCritical()
                << "Even a brand-new cache starts off corrupted, something is"
                << "seriously wrong. :-(";
            d = 0; // Just in case
        }
    }
}

KSharedDataCache::~KSharedDataCache()
{
    // Note that there is no other actions required to separate from the
    // shared memory segment, simply unmapping is enough. This makes things
    // *much* easier so I'd recommend maintaining this ideal.
    if (!d) {
        return;
    }

    if (d->shm) {
#ifdef KSDC_MSYNC_SUPPORTED
        ::msync(d->shm, d->m_mapSize, MS_INVALIDATE | MS_ASYNC);
#endif
        ::munmap(d->shm, d->m_mapSize);
    }

    // Do not delete d->shm, it was never constructed, it's just an alias.
    d->shm = 0;

    delete d;
}

bool KSharedDataCache::insert(const QString &key, const QByteArray &data)
{
    try {
        Private::CacheLocker lock(d);
        if (lock.failed()) {
            return false;
        }

        QByteArray encodedKey = key.toUtf8();
        uint keyHash = generateHash(encodedKey);
        uint position = keyHash % d->shm->indexTableSize();

        // See if we're overwriting an existing entry.
        IndexTableEntry *indices = d->shm->indexTable();

        // In order to avoid the issue of a very long-lived cache having items
        // with a use count of 1 near-permanently, we attempt to artifically
        // reduce the use count of long-lived items when there is high load on
        // the cache. We do this randomly, with a weighting that makes the event
        // impossible if load < 0.5, and guaranteed if load >= 0.96.
        const static double startCullPoint = 0.5l;
        const static double mustCullPoint = 0.96l;

        // cacheAvail is in pages, cacheSize is in bytes.
        double loadFactor = 1.0 - (1.0l * d->shm->cacheAvail * d->shm->cachePageSize()
                                  / d->shm->cacheSize);
        bool cullCollisions = false;

        if (Q_UNLIKELY(loadFactor >= mustCullPoint)) {
            cullCollisions = true;
        }
        else if (loadFactor > startCullPoint) {
            const int tripWireValue = RAND_MAX * (loadFactor - startCullPoint) / (mustCullPoint - startCullPoint);
            if (KRandom::random() >= tripWireValue) {
                cullCollisions = true;
            }
        }

        // In case of collisions in the index table (i.e. identical positions), use
        // quadratic chaining to attempt to find an empty slot. The equation we use
        // is:
        // position = (hash + (i + i*i) / 2) % size, where i is the probe number.
        uint probeNumber = 1;
        while (indices[position].useCount > 0 && probeNumber < MAX_PROBE_COUNT) {
            // If we actually stumbled upon an old version of the key we are
            // overwriting, then use that position, do not skip over it.

            if (Q_UNLIKELY(indices[position].fileNameHash == keyHash)) {
                break;
            }

            // If we are "culling" old entries, see if this one is old and if so
            // reduce its use count. If it reduces to zero then eliminate it and
            // use its old spot.

            if (cullCollisions && (::time(0) - indices[position].lastUsedTime) > 60) {
                indices[position].useCount >>= 1;
                if (indices[position].useCount == 0) {
                    qDebug() << "Overwriting existing old cached entry due to collision.";
                    d->shm->removeEntry(position); // Remove it first
                    break;
                }
            }

            position = (keyHash + (probeNumber + probeNumber * probeNumber) / 2)
                       % d->shm->indexTableSize();
            probeNumber++;
        }

        if (indices[position].useCount > 0 && indices[position].firstPage >= 0) {
            //qDebug() << "Overwriting existing cached entry due to collision.";
            d->shm->removeEntry(position); // Remove it first
        }

        // Data will be stored as fileNamefoo\0PNGimagedata.....
        // So total size required is the length of the encoded file name + 1
        // for the trailing null, and then the length of the image data.
        uint fileNameLength = 1 + encodedKey.length();
        uint requiredSize = fileNameLength + data.size();
        uint pagesNeeded = intCeil(requiredSize, d->shm->cachePageSize());
        uint firstPage = (uint) -1;

        if (pagesNeeded >= d->shm->pageTableSize()) {
            qWarning() << key << "is too large to be cached.";
            return false;
        }

        // If the cache has no room, or the fragmentation is too great to find
        // the required number of consecutive free pages, take action.
        if (pagesNeeded > d->shm->cacheAvail ||
           (firstPage = d->shm->findEmptyPages(pagesNeeded)) >= d->shm->pageTableSize())
        {
            // If we have enough free space just defragment
            uint freePagesDesired = 3 * qMax(1u, pagesNeeded / 2);

            if (d->shm->cacheAvail > freePagesDesired) {
                // TODO: How the hell long does this actually take on real
                // caches?
                d->shm->defragment();
                firstPage = d->shm->findEmptyPages(pagesNeeded);
            }
            else {
                // If we already have free pages we don't want to remove a ton
                // extra. However we can't rely on the return value of
                // removeUsedPages giving us a good location since we're not
                // passing in the actual number of pages that we need.
                d->shm->removeUsedPages(qMin(2 * freePagesDesired, d->shm->pageTableSize())
                                        - d->shm->cacheAvail);
                firstPage = d->shm->findEmptyPages(pagesNeeded);
            }

            if (firstPage >= d->shm->pageTableSize() ||
               d->shm->cacheAvail < pagesNeeded)
            {
                qCritical() << "Unable to free up memory for" << key;
                return false;
            }
        }

        // Update page table
        PageTableEntry *table = d->shm->pageTable();
        for (uint i = 0; i < pagesNeeded; ++i) {
            table[firstPage + i].index = position;
        }

        // Update index
        indices[position].fileNameHash = keyHash;
        indices[position].totalItemSize = requiredSize;
        indices[position].useCount = 1;
        indices[position].addTime = ::time(0);
        indices[position].lastUsedTime = indices[position].addTime;
        indices[position].firstPage = firstPage;

        // Update cache
        d->shm->cacheAvail -= pagesNeeded;

        // Actually move the data in place
        void *dataPage = d->shm->page(firstPage);
        if (Q_UNLIKELY(!dataPage)) {
            throw KSDCCorrupted();
        }

        // Verify it will all fit
        d->verifyProposedMemoryAccess(dataPage, requiredSize);

        // Cast for byte-sized pointer arithmetic
        uchar *startOfPageData = reinterpret_cast<uchar *>(dataPage);
        ::memcpy(startOfPageData, encodedKey.constData(), fileNameLength);
        ::memcpy(startOfPageData + fileNameLength, data.constData(), data.size());

        return true;
    }
    catch(KSDCCorrupted) {
        d->recoverCorruptedCache();
        return false;
    }
}

bool KSharedDataCache::find(const QString &key, QByteArray *destination) const
{
    try {
        Private::CacheLocker lock(d);
        if (lock.failed()) {
            return false;
        }

        // Search in the index for our data, hashed by key;
        QByteArray encodedKey = key.toUtf8();
        qint32 entry = d->shm->findNamedEntry(encodedKey);

        if (entry >= 0) {
            const IndexTableEntry *header = &d->shm->indexTable()[entry];
            const void *resultPage = d->shm->page(header->firstPage);
            if (Q_UNLIKELY(!resultPage)) {
                throw KSDCCorrupted();
            }

            d->verifyProposedMemoryAccess(resultPage, header->totalItemSize);

            header->useCount++;
            header->lastUsedTime = ::time(0);

            // Our item is the key followed immediately by the data, so skip
            // past the key.
            const char *cacheData = reinterpret_cast<const char *>(resultPage);
            cacheData += encodedKey.size();
            cacheData++; // Skip trailing null -- now we're pointing to start of data

            if (destination) {
                *destination = QByteArray(cacheData, header->totalItemSize - encodedKey.size() - 1);
            }

            return true;
        }
    }
    catch(KSDCCorrupted) {
        d->recoverCorruptedCache();
    }

    return false;
}

void KSharedDataCache::clear()
{
    try {
        Private::CacheLocker lock(d);

        if(!lock.failed()) {
            d->shm->clear();
        }
    }
    catch(KSDCCorrupted) {
        d->recoverCorruptedCache();
    }
}

bool KSharedDataCache::contains(const QString &key) const
{
    try {
        Private::CacheLocker lock(d);
        if (lock.failed()) {
            return false;
        }

        return d->shm->findNamedEntry(key.toUtf8()) >= 0;
    }
    catch(KSDCCorrupted) {
        d->recoverCorruptedCache();
        return false;
    }
}

void KSharedDataCache::deleteCache(const QString &cacheName)
{
    QString cachePath = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + QLatin1String("/") + cacheName + QLatin1String(".kcache");

    // Note that it is important to simply unlink the file, and not truncate it
    // smaller first to avoid SIGBUS errors and similar with shared memory
    // attached to the underlying inode.
    qDebug() << "Removing cache at" << cachePath;
    QFile::remove(cachePath);
}

unsigned KSharedDataCache::totalSize() const
{
    try {
        Private::CacheLocker lock(d);
        if (lock.failed()) {
            return 0u;
        }

        return d->shm->cacheSize;
    }
    catch(KSDCCorrupted) {
        d->recoverCorruptedCache();
        return 0u;
    }
}

unsigned KSharedDataCache::freeSize() const
{
    try {
        Private::CacheLocker lock(d);
        if (lock.failed()) {
            return 0u;
        }

        return d->shm->cacheAvail * d->shm->cachePageSize();
    }
    catch(KSDCCorrupted) {
        d->recoverCorruptedCache();
        return 0u;
    }
}

KSharedDataCache::EvictionPolicy KSharedDataCache::evictionPolicy() const
{
    if (d && d->shm) {
        return static_cast<EvictionPolicy>(d->shm->evictionPolicy.fetchAndAddAcquire(0));
    }

    return NoEvictionPreference;
}

void KSharedDataCache::setEvictionPolicy(EvictionPolicy newPolicy)
{
    if (d && d->shm) {
        d->shm->evictionPolicy.fetchAndStoreRelease(static_cast<int>(newPolicy));
    }
}

unsigned KSharedDataCache::timestamp() const
{
    if (d && d->shm) {
        return static_cast<unsigned>(d->shm->cacheTimestamp.fetchAndAddAcquire(0));
    }

    return 0;
}

void KSharedDataCache::setTimestamp(unsigned newTimestamp)
{
    if (d && d->shm) {
        d->shm->cacheTimestamp.fetchAndStoreRelease(static_cast<int>(newTimestamp));
    }
}
