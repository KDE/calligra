#include "KDChartObjectFactory.h"
#include <KDChartParams.h>
#include <KDChartTable.h>
#include <qsargument.h>
#include <qdatetime.h>
#include <KDChartEnums.h>
#include <qfont.h>
#include <KDChartTextPiece.h>
#include <qcolor.h>
#include <KDChartWidget.h>
#include <KDChartPropertySet.h>
#include "factories/QtFactory.h"
#include "factories/QFontFactory.h"

// PENDING(blackie) Clean up code in this file, so it uses the correct getter methods like getBool.
KDChartObjectFactory::KDChartObjectFactory()
{
    registerClass( QString::fromLatin1( "KDChartWidget" ) );
    registerClass( QString::fromLatin1( "KDChartParams" ), new KDChartParams );
    registerClass( QString::fromLatin1( "KDChartTableData" ) );
    registerClass( QString::fromLatin1( "KDChartAxisParams" ), new KDChartAxisParams );
    registerClass( QString::fromLatin1( "KDChartEnums" ), 0, new KDChartEnums );
    registerClass( QString::fromLatin1( "KDChartTextPiece" ) );
    registerClass( QString::fromLatin1( "KDChartCustomBox" ), new KDChartCustomBox );
    registerClass( QString::fromLatin1( "KDChartPropertySet" ), new KDChartPropertySet );
    registerClass( QString::fromLatin1( "KDFrame" ), QString::null, new KDFrame );

    registerClass( QString::fromLatin1( "Qt" ), QString::null, new QtFactory() );
    registerClass( QString::fromLatin1( "QFont" ), QString::null, new QFontFactory );
}

QObject* KDChartObjectFactory::create( const QString& className, const QSArgumentList& args, QObject* /*context*/ )
{
    if ( className == QString::fromLatin1( "KDChartWidget" ) )
         return createKDChartWidget( args );

    if ( className == QString::fromLatin1("KDChartParams") )
        return new KDChartParams();

    else if ( className == QString::fromLatin1("KDChartTableData") )
        return createKDChartTableData( args );

    else if ( className == QString::fromLatin1("KDChartAxisParams") )
        return new KDChartAxisParams();


    else if ( className == QString::fromLatin1( "KDChartTextPiece" ) )
        return createKDChartTextPiece( args );

    else if ( className == QString::fromLatin1( "KDChartCustomBox" ) )
        return createKDChartCustomBox( args );

    else if ( className == QString::fromLatin1( "KDChartPropertySet" ) )
        return createKDChartPropertySet( args );

    else
        return 0;
}

QObject* KDChartObjectFactory::createKDChartWidget( const QSArgumentList& args )
{
    if ( !checkArgCount( "KDChartWidget", args.count(), 2, 2 ) ) return 0;
    if ( !checkArgsIsQtClass( args, 1, "KDChartParams", "KDChartWidget" ) ) return 0;
    if ( !checkArgsIsQtClass( args, 2, "KDChartTableDataBase", "KDChartWidget" ) ) return 0;
    KDChartParams* params = static_cast<KDChartParams*>( args[0].qobject() );
    KDChartTableDataBase* data = static_cast<KDChartTableDataBase*>( args[1].qobject() );

    return new KDChartWidget( params, data );

}

QObject* KDChartObjectFactory::createKDChartTableData( const QSArgumentList& args )
{
    if ( args.count() != 2 ) {
        throwError( QObject::tr( "wrong number of arguments to KDChartTableData" ) );
        return 0;
    }
    QSArgument arg1 = args[0];
    QSArgument arg2 = args[1];

    if ( arg1.type() != QSArgument::Variant || !isNumber(arg1.variant()) ) {
        throwError( QObject::tr( "wrong type for argument 1 of KDChartTableData" ) );
        return 0;
    }
    if ( arg2.type() != QSArgument::Variant || !isNumber(arg2.variant() ) ) {
         throwError( QObject::tr( "wrong type for argument 2 of KDChartTableData" ) );
        return 0;
    }
    return new KDChartTableData( arg1.variant().toUInt(), arg2.variant().toUInt() );
}


QObject* KDChartObjectFactory::createKDChartTextPiece( const QSArgumentList& args )
{
    if ( !checkArgCount( "KDChartTextPiece", args.count(), 2, 2 ) ) return 0;
    QSArgument arg1 = args[0];
    QSArgument arg2 = args[1];

    QString str;
    if ( !getString( args, 1, &str, "KDChartTextPiece" ) ) return 0;
    if ( !checkIsQtVariant( args, 2, QVariant::Font, QString::fromLatin1( "QFont" ), "KDChartTextPiece" ) ) return 0;
    QFont font = args[1].variant().toFont();

    return new KDChartTextPiece( str, font );
}

QObject* KDChartObjectFactory::createKDChartCustomBox( const QSArgumentList& args )
{
    int tmp;

    if ( args.count() == 0 )
        return new KDChartCustomBox();


    if ( args[0].type() == QSArgument::Variant && isNumber( args[0].variant() ) ) {
        if ( args.count() < 8 ) {
            throwError( QObject::tr( "Too few arguments to KDChartCustomBox" ) );
            return 0;
        }

        int rotation;
        if ( !getNumber( args, 1, &rotation, "KDChartCustomBox" ) ) return 0;

        if (!checkArgsIsQtClass( args, 2, "KDChartTextPiece", "KDChartCustomBox" )) return 0;
        KDChartTextPiece* content = static_cast<KDChartTextPiece*>( args[1].qobject() );

        int fontSize;
        if ( !getNumber( args, 3, &fontSize, "KDChartCustomBox" ) ) return 0;


        bool fontScaleGlobal;
        if ( !getBool( args, 4, &fontScaleGlobal, "KDChartCustomBox" ) ) return 0;

        int deltaX;
        if ( !getNumber( args, 5, &deltaX, "KDChartCustomBox" ) ) return 0;

        int deltaY;
        if ( !getNumber( args, 6, &deltaY, "KDChartCustomBox" ) ) return 0;

        int width;
        if ( !getNumber( args, 7, &width, "KDChartCustomBox" ) ) return 0;

        int height;
        if ( !getNumber( args, 8, &height, "KDChartCustomBox" ) ) return 0;

        QColor color = Qt::black;
        if ( args.count() >= 9 ) {
            if ( !checkIsQtVariant( args, 9, QVariant::Color, QString::fromLatin1( "QColor" ), "KDChartCustomBox" ) ) return 0;
            color = args[8].variant().toColor();
        }

        QBrush paper = Qt::NoBrush;
        if ( args.count() >= 10 ) {
            if ( !checkIsQtVariant( args, 10, QVariant::Brush, QString::fromLatin1( "QBrush" ), "KDChartCustomBox" ) ) return 0;
            paper = args[9].variant().toBrush();
        }

        uint area = KDChartEnums::AreaInnermost;
        if ( args.count() >= 11 ) {
            if ( !getNumber( args, 11, &area, "KDChartCustomBox" ) ) return 0;
        }

        KDChartEnums::PositionFlag position = KDChartEnums::PosTopLeft;
        if ( args.count() >= 12 ) {
            if ( !getNumber( args, 12, &tmp, "KDChartCustomBox" ) ) return 0;
            position = (KDChartEnums::PositionFlag) tmp;
        }

        uint align = Qt::AlignTop + Qt::AlignLeft;
        if ( args.count() >= 13 ) {
            if ( !getNumber( args, 13, &align, "KDChartCustomBox" ) ) return 0;
        }

        uint dataRow = 0;
        if ( args.count() >= 14 ) {
            if ( !getNumber( args, 14, &dataRow, "KDChartCustomBox" ) ) return 0;
        }

        uint dataCol = 0;
        if ( args.count() >= 15 ) {
            if ( !getNumber( args, 15, &dataCol, "KDChartCustomBox" ) ) return 0;
        }

        uint data3rd = 0;
        if ( args.count() >= 16 ) {
            if ( !getNumber( args, 16, &data3rd, "KDChartCustomBox" ) ) return 0;
        }

        uint deltaAlign = KDCHART_AlignAuto;
        if ( args.count() >= 17 ) {
            if ( !getNumber( args, 17, &deltaAlign, "KDChartCustomBox" ) ) return 0;
        }

        bool deltaScaleGlobal = true;
        if ( args.count() >= 18 ) {
            if ( !getBool( args, 18, &deltaScaleGlobal, "KDChartCustomBox" ) ) return 0;
        }

        return new KDChartCustomBox( rotation, *content, fontSize, fontScaleGlobal, deltaX, deltaY,
                                     width, height, color, paper, area, position, align, dataRow,
                                     dataCol, data3rd, deltaAlign, deltaScaleGlobal );
    }

    if (!checkArgsIsQtClass( args, 1, "KDChartTextPiece", "KDChartCustomBox" )) return 0;
    KDChartTextPiece* content = static_cast<KDChartTextPiece*>( args[0].qobject() );

    if ( args.count() < 2 ) {
        throwError( QObject::tr( "Too few arguments to KDChartCustomBox" ) );
        return 0;
    }

    int fontSize;
    if ( ! getNumber( args, 2, &fontSize, "KDChartCustomBox" ) ) return 0;

    bool fontScaleGlobal = true;
    if ( args.count() > 2 && !getBool( args, 3, &fontScaleGlobal, "KDChartCustomBox" ) ) return 0;

    if ( args.count() <= 3 )
        return new KDChartCustomBox( *content, fontSize, fontScaleGlobal );

    checkArgCount( "KDChartCustomBox", args.count(), 7, 17 );

    int deltaX;
    if ( !getNumber( args, 4, &deltaX, "KDChartCustomBox" ) ) return 0;

    int deltaY;
    if ( !getNumber( args, 5, &deltaY, "KDChartCustomBox" ) ) return 0;

    int width;
    if ( !getNumber( args, 6, &width, "KDChartCustomBox" ) ) return 0;

    int height;
    if ( !getNumber( args, 7, &height, "KDChartCustomBox" ) ) return 0;


    QColor color = Qt::black;
    if ( args.count() >= 8 ) {
        if ( !checkIsQtVariant( args, 8, QVariant::Color, QString::fromLatin1( "QColor" ), "KDChartCustomBox" ) ) return 0;
        color = args[7].variant().toColor();
    }

    QBrush paper = Qt::NoBrush;
    if ( args.count() >= 9 ) {
        if ( !checkIsQtVariant( args, 9, QVariant::Brush, QString::fromLatin1( "QBrush" ), "KDChartCustomBox" ) ) return 0;
        paper = args[8].variant().toBrush();
    }

    uint area = KDChartEnums::AreaInnermost;
    if ( args.count() >= 10 ) {
        if ( !getNumber( args, 10, &area, "KDChartCustomBox" ) ) return 0;
    }

    KDChartEnums::PositionFlag position = KDChartEnums::PosTopLeft;
    if ( args.count() >= 11 ) {
        if ( !getNumber( args, 11, &tmp, "KDChartCustomBox" ) ) return 0;
        position = (KDChartEnums::PositionFlag) tmp;
    }

    uint align = Qt::AlignTop + Qt::AlignLeft;
    if ( args.count() >= 12 ) {
        if ( !getNumber( args, 12, &align, "KDChartCustomBox" ) ) return 0;
    }

    uint dataRow = 0;
    if ( args.count() >= 13 ) {
        if ( !getNumber( args, 13, &dataRow, "KDChartCustomBox" ) ) return 0;
    }

    uint dataCol = 0;
    if ( args.count() >= 14 ) {
        if ( !getNumber( args, 14, &dataCol, "KDChartCustomBox" ) ) return 0;
    }

    uint data3rd = 0;
    if ( args.count() >= 15 ) {
        if ( !getNumber( args, 15, &data3rd, "KDChartCustomBox" ) ) return 0;
    }

    uint deltaAlign = KDCHART_AlignAuto;
    if ( args.count() >= 16 ) {
        if ( !getNumber( args, 16, &deltaAlign, "KDChartCustomBox" ) ) return 0;
    }

    bool deltaScaleGlobal = true;
    if ( args.count() >= 17 ) {
        if ( !getBool( args, 17, &deltaScaleGlobal, "KDChartCustomBox" ) ) return 0;
    }


    return new KDChartCustomBox( *content, fontSize, fontScaleGlobal, deltaX, deltaY, width, height, color,
                                 paper, area, position, align, dataRow, dataCol, data3rd, deltaAlign, deltaScaleGlobal );
}

QObject* KDChartObjectFactory::createKDChartPropertySet(const QSArgumentList& args )
{
    if ( args.count() == 0 )
        return new KDChartPropertySet();

    QString name;
    if ( !getString( args, 1, &name, "KDChartPropertySet"  ) ) return 0;

    if ( args.count() == 1 )
        return new KDChartPropertySet( name );

    int i;
    if ( !getNumber( args, 2, &i, "KDChartPropertySet" ) ) return 0;
    if ( args.count() == 2 )
        return new KDChartPropertySet( name, i );

    if ( !checkArgCount( "KDChartPropertySet", args.count(), 33, 33 ) ) return 0;

    int idLineWidth = i;

    int lineWidth;
    if ( !getNumber( args, 3,&lineWidth, "KDChartPropertySet" ) ) return 0;

    int idLineColor;
    if ( !getNumber( args, 4,&idLineColor, "KDChartPropertySet" ) ) return 0;


    if ( !checkIsQtVariant( args, 5, QVariant::Color, QString::fromLatin1( "QColor" ), "KDChartPropertySet" ) ) return 0;
    QColor lineColor = args[4].variant().toColor();

    int idLineStyle;
    if ( !getNumber( args, 6,&idLineStyle, "KDChartPropertySet" ) ) return 0;



    if ( !getNumber( args, 7,&i, "KDChartPropertySet" ) ) return 0;
    Qt::PenStyle lineStyle = (Qt::PenStyle) i;

    int idShowMarker;
    if ( !getNumber( args, 8,&idShowMarker, "KDChartPropertySet" ) ) return 0;

    bool showMarker;
    if ( !getBool( args, 9, &showMarker, "KDChartPropertySet" ) ) return 0;

    int idExtraLinesAlign;
    if ( !getNumber( args, 10,&idExtraLinesAlign, "KDChartPropertySet" ) ) return 0;

    uint extraLinesAlign;
    if ( !getNumber( args, 11,&extraLinesAlign, "KDChartPropertySet" ) ) return 0;

    int idExtraLinesInFront;
    if ( !getNumber( args, 12,&idExtraLinesInFront, "KDChartPropertySet" ) ) return 0;

    bool extraLinesInFront;
    if ( !getBool( args, 13, &extraLinesInFront, "KDChartPropertySet" ) ) return 0;

    int idExtraLinesLength;
    if ( !getNumber( args, 14,&idExtraLinesLength, "KDChartPropertySet" ) ) return 0;

    int extraLinesLength;
    if ( !getNumber( args, 15,&extraLinesLength, "KDChartPropertySet" ) ) return 0;

    int idExtraLinesWidth;
    if ( !getNumber( args, 16,&idExtraLinesWidth, "KDChartPropertySet" ) ) return 0;

    int extraLinesWidth;
    if ( !getNumber( args, 17,&extraLinesWidth, "KDChartPropertySet" ) ) return 0;

    int idExtraLinesColor;
    if ( !getNumber( args, 18,&idExtraLinesColor, "KDChartPropertySet" ) ) return 0;

    if ( !checkIsQtVariant( args, 19, QVariant::Color, QString::fromLatin1( "QColor" ), "KDChartPropertySet" ) ) return 0;
    QColor extraLinesColor = args[18].variant().toColor();

    int idExtraLinesStyle;
    if ( !getNumber( args, 20,&idExtraLinesStyle, "KDChartPropertySet" ) ) return 0;

    if ( !getNumber( args, 21,&i, "KDChartPropertySet" ) ) return 0;
    Qt::PenStyle extraLinesStyle = (Qt::PenStyle) i;

    int idExtraMarkersAlign;
    if ( !getNumber( args, 22,&idExtraMarkersAlign, "KDChartPropertySet" ) ) return 0;

    uint extraMarkersAlign;
    if ( !getNumber( args, 23,&extraMarkersAlign, "KDChartPropertySet" ) ) return 0;

    int idExtraMarkersSize;
    if ( !getNumber( args, 24,&idExtraMarkersSize, "KDChartPropertySet" ) ) return 0;

    if ( !checkIsQtVariant( args, 25, QVariant::Size, QString::fromLatin1( "QSize" ), "KDChartPropertySet" ) ) return 0;
    QSize extraMarkersSize = args[24].variant().toSize();

    int idExtraMarkersColor;
    if ( !getNumber( args, 26,&idExtraMarkersColor, "KDChartPropertySet" ) ) return 0;

    if ( !checkIsQtVariant( args, 27, QVariant::Color, QString::fromLatin1( "QColor" ), "KDChartPropertySet" ) ) return 0;
    QColor extraMarkersColor = args[26].variant().toColor();

    int idExtraMarkersStyle;
    if ( !getNumber( args, 28,&idExtraMarkersStyle, "KDChartPropertySet" ) ) return 0;

    int extraMarkersStyle;
    if ( !getNumber( args, 29,&extraMarkersStyle, "KDChartPropertySet" ) ) return 0;

    int idShowBar;
    if ( !getNumber( args, 30,&idShowBar, "KDChartPropertySet" ) ) return 0;

    bool showBar;
    if ( !getBool( args, 31, &showBar, "KDChartPropertySet" ) ) return 0;

    int idBarColor;
    if ( !getNumber( args, 32,&idBarColor, "KDChartPropertySet" ) ) return 0;

    if ( !checkIsQtVariant( args, 33, QVariant::Color, QString::fromLatin1( "QColor" ), "KDChartPropertySet" ) ) return 0;
    QColor barColor = args[32].variant().toColor();

    KDChartPropertySet* set = new KDChartPropertySet;
    set->setName( name );
    set->setLineWidth( idLineWidth, lineWidth );
    set->setLineColor( idLineColor, lineColor );
    set->setLineStyle( idLineStyle, lineStyle );
    set->setShowMarker( idShowMarker, showMarker );
    set->setExtraLinesAlign( idExtraLinesAlign, extraLinesAlign );
    set->setExtraLinesInFront( idExtraLinesInFront, extraLinesInFront );
    set->setExtraLinesLength( idExtraLinesLength, extraLinesLength );
    set->setExtraLinesWidth( idExtraLinesWidth, extraLinesWidth );
    set->setExtraLinesColor( idExtraLinesColor, extraLinesColor );
    set->setExtraLinesStyle( idExtraLinesStyle, extraLinesStyle );
    set->setExtraMarkersAlign( idExtraMarkersAlign, extraMarkersAlign );
    set->setExtraMarkersSize( idExtraMarkersSize, extraMarkersSize );
    set->setExtraMarkersColor( idExtraMarkersColor, extraMarkersColor );
    set->setExtraMarkersStyle( idExtraMarkersStyle, extraMarkersStyle );
    set->setShowBar( idShowMarker, showBar );
    set->setBarColor( idBarColor, barColor );
    return set;
}


bool KDChartObjectFactory::isNumber( const QVariant& v )
{
    return ( v.type() == QVariant::Int || v.type() == QVariant::UInt || v.type() == QVariant::Double );
}

    // PENDING(blackie) rework order of parameters so it matches the other methods
bool KDChartObjectFactory::checkArgCount( const QString& className, int count, int min, int max )
{
    if ( count < min ) {
        throwError( QObject::tr( "Too few arguments when creating %1 object." ).arg( className ) );
        return false;
    }
    if ( count > max ) {
        throwError( QObject::tr( "Too many arguments when creating %1 object." ).arg( className ) );
        return false;
    }
    return true;
}


bool KDChartObjectFactory::checkArgsIsQtClass( const QSArgumentList& args, int index, const char* expected, const char* constructing )
{
    const QSArgument& arg = args[index-1];
    if ( arg.type() != QSArgument::QObjectPtr || !arg.qobject()->inherits( expected ) ) {
        throwError( QObject::tr( "Invalid type for argument no %1 to %2, must be a %3" ).arg(index).arg(constructing).arg(expected) );
        return false;
    }
    return true;
}


bool KDChartObjectFactory::getString( const QSArgumentList& args, int index, QString* str, const char* constructing  )
{
    const QSArgument& arg = args[index-1];
    if ( arg.type() != QSArgument::Variant || arg.variant().type() != QVariant::String ) {
        throwError( QObject::tr( "Invalid type for argument %1 to %2, must be a string" ).arg(index).arg(constructing) );
        return false;
    }
    else {
        *str = arg.variant().toString();
        return true;
    }
}

bool KDChartObjectFactory::getNumber( const QSArgumentList& args, int index, double* number, const char* constructing )
{
    const QSArgument& arg = args[index-1];
    if ( arg.type() != QSArgument::Variant || !isNumber(arg.variant()) ) {
        throwError( QObject::tr( "Invalid type for argument %1 to %2, must be a number" ).arg(index).arg( constructing ) );
        return false;
    }
    else {
        *number = arg.variant().toDouble();
        return true;
    }
}

bool KDChartObjectFactory::getNumber( const QSArgumentList& args, int index, int* number, const char* constructing )
{
    double tmp;
    bool ok = getNumber( args, index, &tmp, constructing );
    *number = static_cast<int>( tmp );
    return ok;
}

bool KDChartObjectFactory::getNumber( const QSArgumentList& args, int index, uint* number, const char* constructing )
{
    double tmp;
    bool ok = getNumber( args, index, &tmp, constructing );
    *number = static_cast<uint>( tmp );
    return ok;
}



bool KDChartObjectFactory::getBool( const QSArgumentList& args, int index, bool* b, const char* constructing )
{
    const QSArgument& arg = args[index-1];
    if ( arg.type() != QSArgument::Variant || arg.variant().type() != QVariant::Bool ) {
        throwError( QObject::tr( "Invalid type for argument %1 to %2, must be a boolean" ).arg(index).arg( constructing ) );
        return false;
    }
    else {
        *b = arg.variant().toBool();
        return true;
    }
}

bool KDChartObjectFactory::checkIsQtVariant( const QSArgumentList& args, int index, QVariant::Type expected, const QString& variantName, const char* constructing )
{
    const QSArgument& arg = args[index-1];
    if ( arg.type() != QSArgument::Variant || arg.variant().type() != expected ) {
        throwError( QObject::tr( "Invalid type for argument %1 to %2, must be a %3").arg(index).arg(constructing).arg(variantName) );
        return false;
    }
    else
        return true;
}


