#ifndef __KSCRIPT_TYPECODE
#define __KSCRIPT_TYPECODE

#include <qshared.h>

#include "kscript_value.h"

class KSCorbaFunction;

class KSTypeCode : public QShared
{
  friend KSCorbaFunction;

public:
  typedef KSSharedPtr<KSTypeCode> Ptr;

  KSTypeCode();
  KSTypeCode( const QString& stringified );
  KSTypeCode( void* tc );
  ~KSTypeCode();

  /**
   * @return the typecode without increasing its reference count.
   */
  void* tc();

  /**
   * @return FALSE is the argument was no valid typecode.
   */
  bool fromString( const QString& stringified );
 
  /**
   * If the value is a string, then it is translated to a typecode.
   * If no typecode could be generated an exception is raised and
   * FALSE is returned.
   */
  static bool convertToTypeCode( KSContext&, KSValue* );
  /**
   * Assumes that the value defines some type and tries to get the
   * typecode out of it. Raises an exception and returns FALSE if
   * that was not possible.
   */
  static KSTypeCode::Ptr typeCode( KSContext&, KSValue* );

protected:
#ifdef WITH_CORBA
  CORBA::TypeCode* m_typecode;
#else
  void* m_typcode;
#endif
};

#endif
