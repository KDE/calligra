#include "koIcons.h"

#include <klibglobal.h>

KLibGlobal* KoIcons::s_global = 0;

KLibGlobal* KoIcons::global()
{
    if ( s_global == 0 )
	s_global = new KLibGlobal( "koffice" );
    
    return s_global;
}
