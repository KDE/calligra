#include "koIcons.h"

#include <kinstance.h>

KInstance* KoIcons::s_global = 0;

KInstance* KoIcons::global()
{
    if ( s_global == 0 )
	s_global = new KInstance( "koffice" );
    
    return s_global;
}
