#include "koscript_property.h"
#include "koscript_struct.h"
#include "koscript_util.h"
#include "koscript.h"

#include <klocale.h>

/*******************************************************
 *
 * KSProperty
 *
 *******************************************************/

bool KSProperty::set( KSContext& context, const KSValue::Ptr& v )
{
    if ( m_struct )
	return m_struct->setMember( context, m_name, v );
    else if ( m_module )
	return m_module->setMember( context, m_name, v );
    else
	ASSERT( 0 );

    // Never reached
    return false;
}
