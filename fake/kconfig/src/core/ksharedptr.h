/*
 * This file is part of the KDE libraries.
 *
 * Copyright 2005 Frerich Raabe <raabe@kde.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef KSHAREDPTR_H
#define KSHAREDPTR_H

#include <QtCore/QSharedData>

/**
 * @deprecated
 * Use QSharedData instead.
 */
typedef QSharedData KShared;

/**
 * \class KSharedPtr ksharedptr.h <KSharedPtr>
 *
 * Can be used to control the lifetime of an object that has derived
 * QSharedData. As long a someone holds
 * a KSharedPtr on some QSharedData object it won't become deleted but
 * is deleted once its reference count is 0.
 * This struct emulates C++ pointers virtually perfectly.
 * So just use it like a simple C++ pointer.
 *
 * The difference with QSharedPointer is that QSharedPointer does the refcounting
 * in the pointer, while KSharedPtr does the refcounting in the object.
 * This allows to convert to a raw pointer temporarily and back to a KSharedPtr
 * without deleting the object, if another reference exists. But it imposes a
 * requirement on the object, which must inherit QSharedData.
 *
 * The difference with using QSharedDataPointer is that QSharedDataPointer is
 * a building block for implementing a value class with implicit sharing (like QString),
 * whereas KSharedPtr provides refcounting to code that uses pointers.
 *
 * @author Waldo Bastian <bastian@kde.org>
 */
template< class T >
class KSharedPtr
{
public:
    /**
     * Creates a null pointer.
     */
    inline KSharedPtr()
        : d(0) { }

    /**
     * Creates a new pointer.
     * @param p the pointer
     */
    inline explicit KSharedPtr( T* p )
        : d(p) { if(d) d->ref.ref(); }

    /**
     * Copies a pointer.
     * @param o the pointer to copy
     */
    inline KSharedPtr( const KSharedPtr& o )
        : d(o.d) { if(d) d->ref.ref(); }

    /**
     * Unreferences the object that this pointer points to. If it was
     * the last reference, the object will be deleted.
     */
    inline ~KSharedPtr() { if (d && !d->ref.deref()) delete d; }

    inline KSharedPtr<T>& operator= ( const KSharedPtr& o ) { attach(o.d); return *this; }
    inline bool operator== ( const KSharedPtr& o ) const { return ( d == o.d ); }
    inline bool operator!= ( const KSharedPtr& o ) const { return ( d != o.d ); }
    inline bool operator< ( const KSharedPtr& o ) const { return ( d < o.d ); }

    inline KSharedPtr<T>& operator= ( T* p ) { attach(p); return *this; }
    inline bool operator== ( const T* p ) const { return ( d == p ); }
    inline bool operator!= ( const T* p ) const { return ( d != p ); }

    /**
     * Test if the shared pointer is NOT null.
     * @return true if the shared pointer is NOT null, false otherwise.
     * @see isNull
     */
    inline operator bool() const { return ( d != 0 ); }

    /**
     * @return the pointer
     */
    inline T* data() { return d; }

    /**
     * @return the pointer
     */
    inline const T* data() const { return d; }

    /**
     * @return a const pointer to the shared object.
     */
    inline const T* constData() const { return d; }

    inline const T& operator*() const { Q_ASSERT(d); return *d; }
    inline T& operator*() { Q_ASSERT(d); return *d; }
    inline const T* operator->() const { Q_ASSERT(d); return d; }
    inline T* operator->() { Q_ASSERT(d); return d; }

    /**
     * Attach the given pointer to the current KSharedPtr.
     * If the previous shared pointer is not owned by any KSharedPtr,
     * it is deleted.
     */
    void attach(T* p);

    /**
     * Clear the pointer, i.e. make it a null pointer.
     */
    void clear();

    /**
     * Returns the number of references.
     * @return the number of references
     */
    inline int count() const { return d ?
        d->ref.load()
        : 0; } // for debugging purposes

    /**
     * Test if the shared pointer is null.
     * @return true if the pointer is null, false otherwise.
     * @see opertor (bool)
     */
    inline bool isNull() const { return (d == 0); }

    /**
     * @return Whether this is the only shared pointer pointing to
     * to the pointee, or whether it's shared among multiple
     * shared pointers.
     */
    inline bool isUnique() const { return count() == 1; }

    template <class U> friend class KSharedPtr;

    /**
     * Convert KSharedPtr<U> to KSharedPtr<T>, using a static_cast.
     * This will compile whenever T* and U* are compatible, i.e.
     * T is a subclass of U or vice-versa.
     * Example syntax:
     * <code>
     *   KSharedPtr<T> tPtr;
     *   KSharedPtr<U> uPtr = KSharedPtr<U>::staticCast( tPtr );
     * </code>
     */
    template <class U>
    static KSharedPtr<T> staticCast( const KSharedPtr<U>& o ) {
        return KSharedPtr<T>( static_cast<T *>( o.d ) );
    }
    /**
     * Convert KSharedPtr<U> to KSharedPtr<T>, using a dynamic_cast.
     * This will compile whenever T* and U* are compatible, i.e.
     * T is a subclass of U or vice-versa.
     * Example syntax:
     * <code>
     *   KSharedPtr<T> tPtr;
     *   KSharedPtr<U> uPtr = KSharedPtr<U>::dynamicCast( tPtr );
     * </code>
     * Since a dynamic_cast is used, if U derives from T, and tPtr isn't an instance of U, uPtr will be 0.
     */
    template <class U>
    static KSharedPtr<T> dynamicCast( const KSharedPtr<U>& o ) {
        return KSharedPtr<T>( dynamic_cast<T *>( o.d ) );
    }

protected:
    T* d;
};

template <class T>
Q_INLINE_TEMPLATE bool operator== (const T* p, const KSharedPtr<T>& o)
{
    return ( o == p );
}

template <class T>
Q_INLINE_TEMPLATE bool operator!= (const T* p, const KSharedPtr<T>& o)
{
    return ( o != p );
}

template <class T>
Q_INLINE_TEMPLATE void KSharedPtr<T>::attach(T* p)
{
    if (d != p) {
        if (p) p->ref.ref();
        if (d && !d->ref.deref())
            delete d;
        d = p;
    }
}

template <class T>
Q_INLINE_TEMPLATE void KSharedPtr<T>::clear()
{
    attach(static_cast<T*>(0));
}

#endif

