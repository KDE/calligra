#ifndef __kspread_util_h__
#define __kspread_util_h__

#include "kspread.h"

class KSpreadMap;

KSpread::Cell util_parseCell( const char *_str );
KSpread::Cell util_parseCell( const char *_str, KSpreadMap* _map );
KSpread::Range util_parseRange( const char *_str );
KSpread::Range util_parseRange2( const char *_str, KSpreadMap* _map );

#endif
