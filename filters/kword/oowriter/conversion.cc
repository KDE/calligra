#include "conversion.h"
#include <kdebug.h>
#include <klocale.h>

QString Conversion::importAlignment( const QString& align )
{
    if ( align == "center" || align == "justify" )
        return align;
    if ( align == "start" )
        return "left";
    if ( align == "end" )
        return "right";
    kdWarning(30518) << "Conversion::importAlignment unknown alignment " << align << endl;
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
    kdWarning(30518) << "Conversion::exportAlignment unknown alignment " << align << endl;
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

int Conversion::importCounterType( const QString& numFormat )
{
    if ( numFormat == "1" )
        return 1;
    if ( numFormat == "a" )
        return 2;
    if ( numFormat == "A" )
        return 3;
    if ( numFormat == "i" )
        return 4;
    if ( numFormat == "I" )
        return 5;
    return 0;
}

QString Conversion::headerTypeToFramesetName( const QString& localName, bool hasEvenOdd )
{
    if ( localName == "header" )
        return hasEvenOdd ? i18n("Odd Pages Header") : i18n( "Header" );
    if ( localName == "header-left" )
        return i18n("Even Pages Header");
    if ( localName == "footer" )
        return hasEvenOdd ? i18n("Odd Pages Footer") : i18n( "Footer" );
    if ( localName == "footer-left" )
        return i18n("Even Pages Footer");
    kdWarning(30518) << "Unknown tag in headerTypeToFramesetName: " << localName << endl;
    // ######
    //return i18n("First Page Header");
    //return i18n("First Page Footer");
    return QString::null;
}

int Conversion::headerTypeToFrameInfo( const QString& localName, bool /*hasEvenOdd*/ )
{
    if ( localName == "header" )
        return 3; // odd headers
    if ( localName == "header-left" )
        return 2; // even headers
    if ( localName == "footer" )
        return 6; // odd footers
    if ( localName == "footer-left" )
        return 5; // even footers

    // ### return 1; // first header
    // ### return 4; // first footer
    return 0;
}
