#ifndef FAKE_KSHAREDPTR_H
#define FAKE_KSHAREDPTR_H

#include <QSharedPointer>
#include <QSharedData>
#include <QExplicitlySharedDataPointer>
#include <QAtomicPointer>

typedef QSharedData KShared;

template< class T >
class KSharedPtr
{
public:
    inline KSharedPtr() : d(0) { }
    inline explicit KSharedPtr( T* p ) : d(p) { if(d) d->ref.ref(); }
    inline KSharedPtr( const KSharedPtr& o ) : d(o.d) { if(d) d->ref.ref(); }
    inline ~KSharedPtr() { if (d && !d->ref.deref()) delete d; }

    inline KSharedPtr<T>& operator= ( const KSharedPtr& o ) { attach(o.d); return *this; }
    inline bool operator== ( const KSharedPtr& o ) const { return ( d == o.d ); }
    inline bool operator!= ( const KSharedPtr& o ) const { return ( d != o.d ); }
    inline bool operator< ( const KSharedPtr& o ) const { return ( d < o.d ); }

    inline KSharedPtr<T>& operator= ( T* p ) { attach(p); return *this; }
    inline bool operator== ( const T* p ) const { return ( d == p ); }
    inline bool operator!= ( const T* p ) const { return ( d != p ); }

    inline operator bool() const { return ( d != 0 ); }

    inline T* data() { return d; }
    inline const T* data() const { return d; }
    inline const T* constData() const { return d; }

    inline const T& operator*() const { Q_ASSERT(d); return *d; }
    inline T& operator*() { Q_ASSERT(d); return *d; }
    inline const T* operator->() const { Q_ASSERT(d); return d; }
    inline T* operator->() { Q_ASSERT(d); return d; }

    void attach(T* p);
    void clear();

    inline int count() const { return d ? static_cast<int>(d->ref) : 0; } // for debugging purposes
    inline bool isNull() const { return (d == 0); }
    inline bool isUnique() const { return count() == 1; }

    template <class U> friend class KSharedPtr;

    template <class U>
    static KSharedPtr<T> staticCast( const KSharedPtr<U>& o ) {
        return KSharedPtr<T>( static_cast<T *>( o.d ) );
    }
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
