#include "conversion.h"
#include <kdebug.h>

QString Conversion::importAlignment( const QString& align )
{
    if ( align == "center" || align == "justify" )
        return align;
    if ( align == "start" )
        return "left";
    if ( align == "end" )
        return "right";
    kdWarning() << "Conversion::importAlignment unknown alignment " << align << endl;
    return "auto";
}

QString Conversion::exportAlignment( const QString& align )
{
    if ( align == "center" || align == "justify" )
        return align;
    if ( align == "left" || align == "auto" ) // ### How to do "auto" in OO?
        return "start";
    if ( align == "right" )
        return "end";
    kdWarning() << "Conversion::exportAlignment unknown alignment " << align << endl;
    return "auto";
}

