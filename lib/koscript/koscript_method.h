#ifndef __KSCRIPT_METHOD_H__
#define __KSCRIPT_METHOD_H__

#include <qmap.h>
#include <qlist.h>
#include <qstring.h>

#include <ksharedptr.h>

#include "koscript_context.h"
#include "koscript_func.h"
#include "koscript_struct.h"

#include <string.h>

/**
 * A method holds a reference to some instance and some function. The function
 * may be of the type @ref KSFunction, KSBuiltinFunction or KSStructBuiltinFunction.
 * The instance may be a @ref KSObject or @ref KSStruct or derived types.
 */
class KSMethod : public KShared
{
public:
  KSMethod( KSModule* m, const KSValue::Ptr& obj, const KSValue::Ptr& func ) : KShared(), m_object( obj ), m_func( func ), m_module( m ) { }
  /**
   * Use this method if the function is KSBuiltinFunction or KSStructBuiltinFunction.
   * In this case we pass the name of the method. No copy of this name is made and the
   * destructor does not free the string. So you should only pass constants here.
   */
  KSMethod( KSModule* m, const KSValue::Ptr& obj, const KSValue::Ptr& func, const QString& name )
      : KShared(), m_object( obj ), m_func( func ), m_module( m ), m_methodName( name ) { }
  virtual ~KSMethod() { }

  bool call( KSContext& context );

  KSValue* object() { return m_object; }
  KSValue* function() { return m_func; }

  KSModule* module() { return m_module; }

  QString name() { return ( m_methodName.isEmpty() ? m_func->functionValue()->name() : m_methodName ); }

private:
  KSValue::Ptr m_object;
  KSValue::Ptr m_func;
  KSModule* m_module;

  QString m_methodName;
};

#endif
