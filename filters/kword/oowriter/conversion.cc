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
    if ( align == "left" || align == "auto" ) // auto handled by text-auto-align
        return "start";
    if ( align == "right" )
        return "end";
    kdWarning() << "Conversion::exportAlignment unknown alignment " << align << endl;
    return "auto";
}

QPair<int,QString> Conversion::importWrapping( const QString& oowrap )
{
    if ( oowrap == "none" )
        // 'no wrap' means 'avoid horizontal space'
        return qMakePair( 2, QString::null );
    if ( oowrap == "left" || oowrap == "right" )
        // Left and right, no problem
        return qMakePair( 1, oowrap );
    if ( oowrap == "run-through" )
        return qMakePair( 0, QString::null );
    if ( oowrap == "biggest" ) // OASIS extension
        return qMakePair( 1, QString::fromLatin1( "biggest" ) );

    ////if ( oowrap == "parallel" || oowrap == "dynamic" )
    // dynamic is called "optimal" in the OO GUI. It's different from biggest because it can lead to parallel.

    // Those are not supported in KWord, let's use biggest instead
    return qMakePair( 1, QString::fromLatin1( "biggest" ) );
}

QString Conversion::exportWrapping( const QPair<int,QString>& runAroundAttribs )
{
    switch( runAroundAttribs.first ) {
    case 0:
        return "run-through";
    case 1: // left, right, or biggest -> ok
        return runAroundAttribs.second;
    case 2:
        return "none";
    default:
        return "ERROR"; // ERROR
    }
}

int Conversion::importOverflowBehavior( const QString& oasisOverflowBehavior )
{
    if ( oasisOverflowBehavior == "auto-extend-frame" )
        return 0; // AutoExtendFrame
    if ( oasisOverflowBehavior == "auto-create-new-frame" )
        return 1; // AutoCreateNewFrame
    if ( oasisOverflowBehavior == "ignore" )
        return 2; // Ignore extra text
    kdWarning(30518) << "Invalid overflow behavior " << oasisOverflowBehavior << endl;
    return 0;
}

QString Conversion::exportOverflowBehavior( const QString& kwordAutoCreateNewFrame )
{
    switch ( kwordAutoCreateNewFrame.toInt() ) {
    case 1:
        return "auto-create-new-frame";
    case 2:
        return "ignore";
    default:
    case 0:
        return "auto-extend-frame";
    }
}

