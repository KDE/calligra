
#include <kdebug.h>
#include <klocale.h>

#include "kformuladefs.h"
#include "symbolfontstyle.h"


KFORMULA_NAMESPACE_BEGIN

#include "symbolfontmapping.cc"


SymbolFontHelper::SymbolFontHelper()
    : greek("abgdezhqiklmnxpvrstufjcywGDQLXPSUFYVW")
{
    for ( uint i = 0; symbolMap[ i ].unicode != 0; i++ ) {
        compatibility[ symbolMap[ i ].pos ] = symbolMap[ i ].unicode;
    }
}


bool SymbolFontStyle::init( ContextStyle* context )
{
    if ( fontAvailable( "symbol" ) ) {
        symbolTable()->init( context );

        SymbolTable::NameTable names;
        fillNameTable( names );
        symbolTable()->initFont( symbolMap, "symbol", names, normalChar );
        return true;
    }
    else {
        return false;
    }
}


QString SymbolFontStyle::name()
{
    return i18n( "Symbol Font Style" );
}

KFORMULA_NAMESPACE_END
