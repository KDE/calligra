
#include <kdebug.h>
#include <klocale.h>

#include "kformuladefs.h"
#include "esstixfontstyle.h"


KFORMULA_NAMESPACE_BEGIN

#include "esstixfontmapping.cc"

bool EsstixFontStyle::init( ContextStyle* context )
{
    if ( fontAvailable( "esstixeight" ) &&
         fontAvailable( "esstixeleven" ) &&
         fontAvailable( "esstixfifteen" ) &&
         fontAvailable( "esstixfive" ) &&
         fontAvailable( "esstixfour" ) &&
         fontAvailable( "esstixfourteen" ) &&
         fontAvailable( "esstixnine" ) &&
         fontAvailable( "esstixone" ) &&
         fontAvailable( "esstixseven" ) &&
         fontAvailable( "esstixseventeen" ) &&
         fontAvailable( "esstixsix" ) &&
         fontAvailable( "esstixsixteen" ) &&
         fontAvailable( "esstixten" ) &&
         fontAvailable( "esstixthirteen" ) &&
         fontAvailable( "esstixthree" ) &&
         fontAvailable( "esstixtwelve" ) &&
         fontAvailable( "esstixtwo" ) ) {

        SymbolTable* st = symbolTable();
        st->init( context );

        SymbolTable::NameTable tempNames;
        fillNameTable( tempNames );

        st->initFont( esstixeightMap, "esstixeight", tempNames, normalChar );
        st->initFont( esstixelevenMap, "esstixeleven", tempNames, boldItalicChar );
        st->initFont( esstixfifteenMap, "esstixfifteen", tempNames, normalChar );
        st->initFont( esstixfiveMap, "esstixfive", tempNames, normalChar );
        st->initFont( esstixfourMap, "esstixfour", tempNames, normalChar );
        st->initFont( esstixfourteenMap, "esstixfourteen", tempNames, normalChar );
        st->initFont( esstixnineMap, "esstixnine", tempNames, italicChar );
        st->initFont( esstixoneMap, "esstixone", tempNames, normalChar );
        st->initFont( esstixsevenMap, "esstixseven", tempNames, normalChar );
        st->initFont( esstixseventeenMap, "esstixseventeen", tempNames, normalChar );
        st->initFont( esstixsixMap, "esstixsix", tempNames, normalChar );
        st->initFont( esstixsixteenMap, "esstixsixteen", tempNames, normalChar );
        st->initFont( esstixtenMap, "esstixten", tempNames, normalChar );
        st->initFont( esstixthirteenMap, "esstixthirteen", tempNames, normalChar );
        st->initFont( esstixthreeMap, "esstixthree", tempNames, normalChar );
        st->initFont( esstixtwelveMap, "esstixtwelve", tempNames, boldChar );
        st->initFont( esstixtwoMap, "esstixtwo", tempNames, normalChar );

        return true;
    }
    else {
        return false;
    }
}


QString EsstixFontStyle::name()
{
    return i18n( "Esstix Font Style" );
}


const AlphaTable* EsstixFontStyle::alphaTable() const
{
    return &m_alphaTable;
}


EsstixAlphaTable::EsstixAlphaTable()
    : script_font( "esstixthirteen" ),
      fraktur_font( "esstixfifteen" ),
      double_struck_font( "esstixfourteen" )
{
}


AlphaTableEntry EsstixAlphaTable::entry( char pos,
                                         CharFamily family,
                                         CharStyle /*style*/ ) const
{
    AlphaTableEntry entry;

    // This is very font specific.
    switch( family ) {
        //case normal:
    case scriptFamily:
        if ( ( ( pos >= 'A' ) && ( pos <= 'Z' ) ) ||
             ( ( pos >= 'a' ) && ( pos <= 'z' ) ) ) {
            entry.pos = pos;
            entry.font = script_font;
        }
        break;
    case frakturFamily:
        if ( ( ( pos >= 'A' ) && ( pos <= 'Z' ) ) ||
             ( ( pos >= 'a' ) && ( pos <= 'z' ) ) ) {
            entry.pos = pos;
            entry.font = fraktur_font;
        }
        break;
    case doubleStruckFamily:
        if ( ( ( pos >= 'A' ) && ( pos <= 'Z' ) ) ||
             ( ( pos >= '0' ) && ( pos <= '9' ) ) ) {
            entry.pos = pos;
            entry.font = double_struck_font;
        }
        break;
    default:
        break;
    }

    return entry;
}

KFORMULA_NAMESPACE_END
