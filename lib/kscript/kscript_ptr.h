/* This class is property of Troll Tech. It's here temporarily
   until it appears in Qt's CVS
*/

#ifndef __KSCRIPT_PTR_H__
#define __KSCRIPT_PTR_H__

#include <qshared.h>

/**
 * Can be used to control the lifetime of an object
 * that has derived @ref QShared. As long a someone holds
 * a KSSharedPtr on some QShared object it won't become
 * deleted but is deleted once its reference count is 0.
 * This struct emulates C++ pointers perfectly. So just use
 * it like a simple C++ pointer.
 */
template< class T >
struct KSSharedPtr
{
public:
  KSSharedPtr() { ptr = 0; }
  KSSharedPtr( T* t ) { ptr = t; /* ptr->ref(); */ }
  KSSharedPtr( const KSSharedPtr& p ) { ptr = p.ptr; if ( ptr ) ptr->ref(); }
  ~KSSharedPtr() { if ( ptr && ptr->deref() ) delete ptr; }

  KSSharedPtr<T>& operator= ( const KSSharedPtr<T>& p ) {
    if ( ptr && ptr->deref() ) delete ptr;
    ptr = p.ptr; if ( ptr ) ptr->ref();
    return *this;
  }
  KSSharedPtr<T>& operator= ( T* p ) {
    if ( ptr && ptr->deref() ) delete ptr;
    ptr = p; /* ptr->ref(); */
    return *this;
  }
  bool operator== ( const KSSharedPtr<T>& p ) const { return ( ptr == p.ptr ); }
  bool operator!= ( const KSSharedPtr<T>& p ) const { return ( ptr != p.ptr ); }
  bool operator== ( const T& p ) const { return ( ptr == &p ); }
  bool operator!= ( const T& p ) const { return ( ptr != &p ); }
  bool operator!() const { return ( ptr == 0 ); }
  operator bool() const { return ( ptr != 0 ); }
  operator bool() { return (ptr != 0); }
  operator T*() { return ptr; }
  operator T*() const { return (T*)ptr; }

  const T& operator*() const { return *ptr; }
  T& operator*() { return *ptr; }
  const T* operator->() const { return ptr; }
  T* operator->() { return ptr; }

  uint count() const { return ptr->count; } // for debugging purposes
private:
  T* ptr;
};

#endif
