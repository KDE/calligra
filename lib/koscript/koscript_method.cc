#include "koscript_method.h"
#include "koscript_struct.h"
#include "koscript_util.h"
#include "koscript.h"

#include <klocale.h>

/*******************************************************
 *
 * KSMethod
 *
 *******************************************************/

bool KSMethod::call( KSContext& context )
{
  ASSERT( context.value() && context.value()->type() == KSValue::ListType );

  if ( m_func->type() == KSValue::FunctionType )
  {
    context.value()->listValue().prepend( m_object );
    if ( !m_func->functionValue()->call( context ) )
      return false;
  }
  else if ( m_func->type() == KSValue::StructBuiltinMethodType )
  {
    KSStructBuiltinMethod m = m_func->structBuiltinMethodValue();
    KSStruct* o = m_object->structValue();
    if ( ! (o->*m)( context, name() ) )
      return false;
  }

  return true;
}
