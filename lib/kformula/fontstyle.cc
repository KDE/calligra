
#include "fontstyle.h"


KFORMULA_NAMESPACE_BEGIN

bool FontStyle::fontAvailable( QString fontName )
{
    QFont f( fontName );
    QStringList fields = QStringList::split( '-', f.rawName() );
    if ( ( fields.size() > 1 ) &&
         ( ( fields[1].upper() == fontName.upper() ) ||
           ( fields[0].upper() == fontName.upper() ) ) ) {
        return true;
    }
    else {
        kdWarning( DEBUGID ) << "Font '" << fontName << "' not found but '" << f.rawName() << "'." << endl;
        return false;
    }
}


#include "unicodenames.cc"

void FontStyle::fillNameTable( SymbolTable::NameTable& names )
{
    for ( int i=0; nameTable[i].unicode != 0; ++i ) {
        names[QChar( nameTable[i].unicode )] = nameTable[i].name;
    }
}

KFORMULA_NAMESPACE_END
