/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#include <KDChartParams.h>
#include <KDChartAxisParams.h>
#include <KDChartEnums.h>
#include <KDFrame.h>
#include <KDXMLTools.h>
#ifndef KDCHART_MASTER_CVS
#include "KDChartParams.moc"
#endif

class KDChartData;

#include <qdom.h>

const uint KDChartParams::KDCHART_MAX_AXES = 12;
const uint KDChartParams::KDCHART_ALL_AXES = UINT_MAX-1;
const uint KDChartParams::KDCHART_NO_AXIS = UINT_MAX;
const uint KDChartParams::KDCHART_ALL_DATASETS = UINT_MAX - 1;
const uint KDChartParams::KDCHART_NO_DATASET = UINT_MAX;
const uint KDChartParams::KDCHART_UNKNOWN_CHART = UINT_MAX - 2;
const uint KDChartParams::KDCHART_ALL_CHARTS = UINT_MAX - 1;
const uint KDChartParams::KDCHART_NO_CHART = UINT_MAX;
const int KDChartParams::DATA_VALUE_AUTO_DIGITS = INT_MAX;
const int KDChartParams::SAGGITAL_ROTATION   = INT_MAX;
const int KDChartParams::TANGENTIAL_ROTATION = INT_MAX - 1;

QColor  KDChartParams::_internalPointer_DataValueAutoColor = QColor( 0,1,0 );
QColor* const KDChartParams::DATA_VALUE_AUTO_COLOR = &_internalPointer_DataValueAutoColor;

KDChartParams::KDChartParams()
{
    // GENERAL

    // Avoid Purify UMR
    _maxDatasetColor = 0;
    _maxDatasetSourceMode = 0;
    _setChartSourceModeWasUsed = false;

    // Set the default font params flag for data value texts
    // but do *not* call setPrintDataValues() there since
    // this will be called internally by setChartType() below.
    setPrintDataValuesWithDefaultFontParams( KDCHART_ALL_CHARTS, false );

    setGlobalLeading( 0,0,0,0 );


    // Default type is bar charts
    setChartType( Bar );

    // By default, there is no additional chart type
    setAdditionalChartType( NoType );

    // Default is to show all values.
    setNumValues( -1 );

    _defaultFont = QFont( "helvetica", 8, QFont::Normal, false );

    // The default frame settings: no border, no corners, no background
    _noFrame.clearAll();

    // The default frame settings: no inner gap, no outer gap and use the _noFrame
    _noFrameSettings = KDChartFrameSettings();

    // The default brightness of shadow colors (needs to be set
    // before the data colors to avoid an UMR).
    setShadowBrightnessFactor( 1.0 );

    // The default shadow fill style.
    setShadowPattern( Qt::SolidPattern );

    // Some default colors for the data.
    setDataColor( 0, red );
    setDataColor( 1, green );
    setDataColor( 2, blue );
    setDataColor( 3, cyan );
    setDataColor( 4, magenta );
    setDataColor( 5, yellow );
    setDataColor( 6, darkRed );
    setDataColor( 7, darkGreen );
    setDataColor( 8, darkBlue );
    setDataColor( 9, darkCyan );
    setDataColor( 10, darkMagenta );
    setDataColor( 11, darkYellow );

    // Default color for data display outlines.
    setOutlineDataColor( black );

    // Default line width for data display outlines.
    setOutlineDataLineWidth( 1 );

    // Default line style for data display outlines.
    setOutlineDataLineStyle( Qt::SolidLine );

    // END GENERAL



    // BAR CHART-SPECIFIC

    // Default bar subtype is normal
    setBarChartSubType( BarNormal );

    // Default is not to draw 3D bars
    setThreeDBars( false );

    // Default is to used shadowed colors for 3D bar effects
    setThreeDBarsShadowColors( true );

    // Default angle for 3D bars is 45 degrees.
    setThreeDBarAngle( 45 );


    // Default gap between datasets is 6 per mille of chart data area.
    setDatasetGap( 6 );
    setDatasetGapIsRelative( true );

    // Default gap between value blocks is 15 per mille of chart data area.
    setValueBlockGap( 15 );
    setValueBlockGapIsRelative( true );

    // END BAR CHART-SPECIFIC


    // LINE/AREA CHART-SPECIFIC
    // Normal lines by default
    setLineChartSubType( LineNormal );

    // No markers by default
    setLineMarker( false );

    // Lines have a width of 1 pixel by default
    setLineWidth( 1 );

    // Default line marker styles and size
    _maxDatasetLineMarkerStyle = 0; // avoid UMR
    setLineMarkerStyle( 0, LineMarkerCircle );
    setLineMarkerStyle( 1, LineMarkerSquare );
    setLineMarkerStyle( 2, LineMarkerDiamond );
    setLineMarkerSize( QSize( 6, 6 ) );

    // 3D line settings
    setThreeDLines( false );
    setThreeDLineDepth( 20 );
    setThreeDLineXRotation( 30 );
    setThreeDLineYRotation( 30 );

    // Normal areas by default
    setAreaChartSubType( AreaNormal );

    // Areas are filled below the value points by default.
    setAreaLocation( AreaBelow );

    // END LINE/AREA CHART-SPECIFIC

    // POLAR CHART-SPECIFIC
    // Normal polar charts by default
    setPolarChartSubType( PolarNormal );

    // Have markers by default
    setPolarMarker( true );

    // Polar charts show the zero point at the right side of the circle
    setPolarZeroDegreePos( 0 );

    // Lines have a width of 3/1000 of the chart's min size
    // (either width or height) by default
    setPolarLineWidth( -3 );

    // Default polar marker styles and size
    _maxDatasetPolarMarkerStyle = 0; // avoid UMR
    setPolarMarkerStyle( 0, PolarMarkerCircle );
    setPolarMarkerStyle( 1, PolarMarkerSquare );
    setPolarMarkerStyle( 2, PolarMarkerDiamond );
    setPolarMarkerSize( QSize( -40,-40 ) );

    // default circular axes delimiters
    setPolarRotateCircularLabels( false );
    setPolarDelimsAndLabelsAtPos( KDChartEnums::PosTopLeft,      false, false );
    setPolarDelimsAndLabelsAtPos( KDChartEnums::PosTopCenter,    true,  true  );
    setPolarDelimsAndLabelsAtPos( KDChartEnums::PosTopRight,     false, false );

    setPolarDelimsAndLabelsAtPos( KDChartEnums::PosCenterLeft,   false, false );
    setPolarDelimsAndLabelsAtPos( KDChartEnums::PosCenterRight,  false, false );

    setPolarDelimsAndLabelsAtPos( KDChartEnums::PosBottomLeft,   false, false );
    setPolarDelimsAndLabelsAtPos( KDChartEnums::PosBottomCenter, true,  true  );
    setPolarDelimsAndLabelsAtPos( KDChartEnums::PosBottomRight,  false, false );


    /* for test:
    setPolarDelimsAndLabelsAtPos( KDChartEnums::PosTopLeft,      true, true );
    setPolarDelimsAndLabelsAtPos( KDChartEnums::PosTopCenter,    true, true  );
    setPolarDelimsAndLabelsAtPos( KDChartEnums::PosTopRight,     true, true );
    setPolarDelimsAndLabelsAtPos( KDChartEnums::PosCenterRight,  true, true );
    setPolarDelimsAndLabelsAtPos( KDChartEnums::PosBottomRight,  true, true );
    setPolarDelimsAndLabelsAtPos( KDChartEnums::PosBottomCenter, true, true  );
    setPolarDelimsAndLabelsAtPos( KDChartEnums::PosBottomLeft,   true, true );
    setPolarDelimsAndLabelsAtPos( KDChartEnums::PosCenterLeft,   true, true );
    */


    // END POLAR CHART-SPECIFIC

    // PIE/RING CHART-SPECIFIC
    // Pie/ring charts do not explode by default.
    setExplode( false );
    // But if they do, the explode factor is 10%
    setExplodeFactor( 0.1 );
    // setExplodeValues() is not called for initialization, the default
    // empty list is the default.
    // Flat pies by default
    setThreeDPies( false );
    // Height of 3D effect
    setThreeDPieHeight( 20 );
    // Start of pie circle
    setPieStart( 0 );
    // Start of ring circle
    setRingStart( 0 );
    // Ring thickness is constant by default
    setRelativeRingThickness( false );
    // END PIE/RING CHART-SPECIFIC


    // HILO CHART-SPECIFIC
    // Simple HiLo charts by default
    setHiLoChartSubType( KDChartParams::HiLoSimple );
    setHiLoChartPrintLowValues( false );
    setHiLoChartPrintHighValues( false );
    setHiLoChartPrintOpenValues( false );
    setHiLoChartPrintCloseValues( false );

    // GANTT CHART-SPECIFIC
    // Simple Gantt charts by default
    setGanttChartSubType( KDChartParams::GanttNormal );

    // LEGEND
    // Distance between legend and data.
    setLegendSpacing( 20 );
    // Position of the legend
    setLegendPosition( LegendRight );
    // Where the legend labels come from
    setLegendSource( LegendAutomatic );
    // legend texts are drawn in black by default
    setLegendTextColor( Qt::black );
    // legend font size is calculated dynamically, but ignore the font size
    setLegendFont( QFont( "helvetica", 10, QFont::Normal, false ), false );
    // legend font size is calculated dynamically:
    //                    20 / 1000 of the average value of
    //                                 the printable area height and width
    setLegendFontRelSize( 20 );
    // the default legend title is "Legend"
    setLegendTitleText( tr( "Legend" ) );
    // legend title is drawn in black by default
    setLegendTitleTextColor( Qt::black );
    // legend title font size is calculated dynamically, but ignore
    // the font size
    setLegendTitleFont( QFont( "helvetica", 12, QFont::Normal, false ), false );
    // legend title font size is calculated dynamically:
    //                    25 / 1000 of the average value of
    //                                 the printable area height and width
    setLegendTitleFontRelSize( 25 );
    // END LEGEND


    // AXES

    setDefaultAxesTypes();
    // activate bottom (ordinate) and left (abcissa) axis
    activateDefaultAxes();

    // END AXES


    // HEADERS/FOOTERS

    // Set a default font for all sections not taking the build-in
    // defaults from c'tor KDChartParams::HdFtParams::HdFtParams()
    QFont defaultHdFtFont( "helvetica", 14, QFont::Normal, false );
    int relHd0Size = 16;
    int relHdSize  = 28;
    int relHd2Size = 22;

    int relFt0Size = 16;
    int relFtSize  = 22;
    int relFt2Size = 13;
    setHeaderFooterFont( KDChartParams::HdFtPosHeader0,
                         defaultHdFtFont, true, relHd0Size );
    setHeaderFooterFont( KDChartParams::HdFtPosHeader0A,
                         defaultHdFtFont, true, relHd0Size );
    setHeaderFooterFont( KDChartParams::HdFtPosHeader0B,
                         defaultHdFtFont, true, relHd0Size );
    setHeaderFooterFont( KDChartParams::HdFtPosHeader,
                         defaultHdFtFont, true, relHdSize );
    setHeaderFooterFont( KDChartParams::HdFtPosHeaderA,
                         defaultHdFtFont, true, relHdSize );
    setHeaderFooterFont( KDChartParams::HdFtPosHeaderB,
                         defaultHdFtFont, true, relHdSize );
    setHeaderFooterFont( KDChartParams::HdFtPosHeader2,
                         defaultHdFtFont, true, relHd2Size );
    setHeaderFooterFont( KDChartParams::HdFtPosHeader2A,
                         defaultHdFtFont, true, relHd2Size );
    setHeaderFooterFont( KDChartParams::HdFtPosHeader2B,
                         defaultHdFtFont, true, relHd2Size );

    setHeaderFooterFont( KDChartParams::HdFtPosFooter0,
                         defaultHdFtFont, true, relFt0Size );
    setHeaderFooterFont( KDChartParams::HdFtPosFooter0A,
                         defaultHdFtFont, true, relFt0Size );
    setHeaderFooterFont( KDChartParams::HdFtPosFooter0B,
                         defaultHdFtFont, true, relFt0Size );
    setHeaderFooterFont( KDChartParams::HdFtPosFooter,
                         defaultHdFtFont, true, relFtSize );
    setHeaderFooterFont( KDChartParams::HdFtPosFooterA,
                         defaultHdFtFont, true, relFtSize );
    setHeaderFooterFont( KDChartParams::HdFtPosFooterB,
                         defaultHdFtFont, true, relFtSize );
    setHeaderFooterFont( KDChartParams::HdFtPosFooter2,
                         defaultHdFtFont, true, relFt2Size );
    setHeaderFooterFont( KDChartParams::HdFtPosFooter2A,
                         defaultHdFtFont, true, relFt2Size );
    setHeaderFooterFont( KDChartParams::HdFtPosFooter2B,
                         defaultHdFtFont, true, relFt2Size );

    // END HEADERS/FOOTERS
}


KDChartParams::~KDChartParams()
{
    // Intentionally left blank for now.
}


KDChartParams::KDChartFrameSettings::~KDChartFrameSettings()
{
    // Intentionally left blank for now.
}


void KDChartParams::setPrintDataValues( bool active,
                                        uint chart,
                                        int divPow10,
                                        int digitsBehindComma,
                                        QFont* font,
                                        uint size,
                                        QColor* color,
                                        KDChartEnums::PositionFlag negativePosition,
                                        uint negativeAlign,
                                        int  negativeDeltaX,
                                        int  negativeDeltaY,
                                        int  negativeRotation,
                                        KDChartEnums::PositionFlag positivePosition,
                                        uint positiveAlign,
                                        int  positiveDeltaX,
                                        int  positiveDeltaY,
                                        int  positiveRotation,
                                        KDChartEnums::TextLayoutPolicy policy )
{
    uint count = (KDCHART_ALL_CHARTS == chart) ? 2 : 1;
    PrintDataValuesSettings * settings =    (( 1 < count ) || ( 0 == chart ))
                                            ? &_printDataValuesSettings
                                            : &_printDataValuesSettings2;
    for ( uint i = 0; i < count; ++i ) {
        settings->_printDataValues   = active;
        settings->_divPow10          = divPow10;
        settings->_digitsBehindComma = digitsBehindComma;

        const ChartType cType
            = (    ( 1  < count  &&  i )
                || ( 1 == count  &&  0 < chart  &&  chart < 1000 ) )
            ? additionalChartType()
            : chartType();

        bool finished( false );
        if ( UINT_MAX == size ) {
            finished = true;
            switch ( cType ) {
                case NoType:
                case Bar: {
                    if ( font )
                        settings->_dataValuesFont = *font;
                    else
                        settings->_dataValuesFont = QFont( "times", 1, QFont::Bold );
                    settings->_dataValuesUseFontRelSize = true;
                    settings->_dataValuesFontRelSize = 16;
                    settings->_dataValuesAutoColor            = false;  //  !!!
                    settings->_dataValuesColor = QColor( Qt::darkBlue );
                    // for values below zero:
                    settings->_dataValuesAnchorNegativePosition = KDChartEnums::PosBottomRight;
                    settings->_dataValuesAnchorNegativeAlign    = Qt::AlignBottom + Qt::AlignRight;
                    settings->_dataValuesAnchorNegativeDeltaX   =   0;
                    settings->_dataValuesAnchorNegativeDeltaY   =  55;
                    settings->_dataValuesNegativeRotation       = 300;
                    // for values greater/equal zero:
                    settings->_dataValuesAnchorPositivePosition = KDChartEnums::PosTopLeft;
                    settings->_dataValuesAnchorPositiveAlign    = Qt::AlignTop + Qt::AlignLeft;
                    settings->_dataValuesAnchorPositiveDeltaX   =    0;
                    settings->_dataValuesAnchorPositiveDeltaY   = - 65;
                    settings->_dataValuesPositiveRotation       =  300;

                    settings->_dataValuesLayoutPolicy = KDChartEnums::LayoutPolicyRotate;
                }
                break;
                case Line: {
                    if ( font )
                        settings->_dataValuesFont = *font;
                    else
                        settings->_dataValuesFont = QFont( "times", 1, QFont::Normal );
                    settings->_dataValuesUseFontRelSize = true;
                    settings->_dataValuesFontRelSize = 16;
                    settings->_dataValuesAutoColor            = false;  //  !!!
                    settings->_dataValuesColor = QColor( Qt::darkBlue );
                    // for values below zero:
                    settings->_dataValuesAnchorNegativePosition = KDChartEnums::PosBottomCenter;
                    settings->_dataValuesAnchorNegativeAlign    = Qt::AlignTop + Qt::AlignHCenter;
                    settings->_dataValuesAnchorNegativeDeltaX   =   0;
                    settings->_dataValuesAnchorNegativeDeltaY   =   0;
                    settings->_dataValuesNegativeRotation       =   0;
                    // for values greater/equal zero:
                    settings->_dataValuesAnchorPositivePosition = KDChartEnums::PosTopCenter;
                    settings->_dataValuesAnchorPositiveAlign    = Qt::AlignBottom + Qt::AlignHCenter;
                    settings->_dataValuesAnchorPositiveDeltaX   =    0;
                    settings->_dataValuesAnchorPositiveDeltaY   =    0;
                    settings->_dataValuesPositiveRotation       =    0;

                    settings->_dataValuesLayoutPolicy = KDChartEnums::LayoutPolicyRotate;
                }
                break;
                case Area: {
                    if ( font )
                        settings->_dataValuesFont = *font;
                    else
                        settings->_dataValuesFont = QFont( "times", 1, QFont::Bold );
                    settings->_dataValuesUseFontRelSize = true;
                    settings->_dataValuesFontRelSize = 16;
                    settings->_dataValuesAutoColor            = true;  //  !!!
                    settings->_dataValuesColor = QColor( Qt::black );
                    // for values below zero:
                    settings->_dataValuesAnchorNegativePosition = KDChartEnums::PosCenter;
                    settings->_dataValuesAnchorNegativeAlign    = Qt::AlignCenter;
                    settings->_dataValuesAnchorNegativeDeltaX   =   0;
                    settings->_dataValuesAnchorNegativeDeltaY   =   0;
                    settings->_dataValuesNegativeRotation       =   0;
                    // for values greater/equal zero:
                    settings->_dataValuesAnchorPositivePosition = KDChartEnums::PosCenter;
                    settings->_dataValuesAnchorPositiveAlign    = Qt::AlignCenter;
                    settings->_dataValuesAnchorPositiveDeltaX   =    0;
                    settings->_dataValuesAnchorPositiveDeltaY   =    0;
                    settings->_dataValuesPositiveRotation       =    0;

                    settings->_dataValuesLayoutPolicy = KDChartEnums::LayoutPolicyRotate;
                }
                break;
                case HiLo:
                    // settings will be defined here
                    // when also HiLo charts will be set up using setPrintDataValues()
                break;
                case Gantt:
                    // settings will be defined here
                    // when also Gantt charts will be set up using setPrintDataValues()
                break;
                case Pie: {
                    if ( font )
                        settings->_dataValuesFont = *font;
                    else
                        settings->_dataValuesFont = QFont( "times", 1, QFont::Bold );
                    settings->_dataValuesUseFontRelSize = true;
                    settings->_dataValuesFontRelSize = 26;
                    settings->_dataValuesAutoColor            = true;  //  !!!
                    settings->_dataValuesColor = QColor( Qt::black );
                    // for values below zero:
                    settings->_dataValuesAnchorNegativePosition = KDChartEnums::PosTopCenter;
                    settings->_dataValuesAnchorNegativeAlign    = Qt::AlignTop + Qt::AlignHCenter;
                    settings->_dataValuesAnchorNegativeDeltaX   =  0;
                    settings->_dataValuesAnchorNegativeDeltaY   = 50;
                    settings->_dataValuesNegativeRotation       = TANGENTIAL_ROTATION;
                    // for values greater/equal zero:
                    settings->_dataValuesAnchorPositivePosition = KDChartEnums::PosTopCenter;
                    settings->_dataValuesAnchorPositiveAlign    = Qt::AlignTop + Qt::AlignHCenter;
                    settings->_dataValuesAnchorPositiveDeltaX   =  0;
                    settings->_dataValuesAnchorPositiveDeltaY   = 50;
                    settings->_dataValuesPositiveRotation       = TANGENTIAL_ROTATION;

                    settings->_dataValuesLayoutPolicy = KDChartEnums::LayoutPolicyRotate;
                }
                break;
                case Ring: {
                    if ( font )
                        settings->_dataValuesFont = *font;
                    else
                        settings->_dataValuesFont = QFont( "times", 1, QFont::Bold );
                    settings->_dataValuesUseFontRelSize = true;
                    settings->_dataValuesFontRelSize = 26;
                    settings->_dataValuesAutoColor            = true;  //  !!!
                    settings->_dataValuesColor = QColor( Qt::black );
                    // for values below zero:
                    settings->_dataValuesAnchorNegativePosition = KDChartEnums::PosTopCenter;
                    settings->_dataValuesAnchorNegativeAlign    = Qt::AlignCenter;
                    settings->_dataValuesAnchorNegativeDeltaX   = 0;
                    settings->_dataValuesAnchorNegativeDeltaY   = 0;
                    settings->_dataValuesNegativeRotation       = TANGENTIAL_ROTATION;
                    // for values greater/equal zero:
                    settings->_dataValuesAnchorPositivePosition = KDChartEnums::PosCenter;
                    settings->_dataValuesAnchorPositiveAlign    = Qt::AlignCenter;
                    settings->_dataValuesAnchorPositiveDeltaX   = 0;
                    settings->_dataValuesAnchorPositiveDeltaY   = 0;
                    settings->_dataValuesPositiveRotation       = TANGENTIAL_ROTATION;

                    settings->_dataValuesLayoutPolicy = KDChartEnums::LayoutPolicyRotate;
                }
                break;

                case Polar: {
                    /*if ( font )
                        settings->_dataValuesFont = *font;
                    else
                        settings->_dataValuesFont = QFont( "times", 1, QFont::Bold );
                    settings->_dataValuesUseFontRelSize = true;
                    settings->_dataValuesFontRelSize = 16;
                    settings->_dataValuesAutoColor   = polarMarker();  //  !!!
                    settings->_dataValuesColor = QColor( Qt::black );

                    // for values below zero:
                    settings->_dataValuesAnchorNegativePosition = KDChartEnums::PosCenter;
                    settings->_dataValuesAnchorNegativeAlign    = Qt::AlignCenter;
                    settings->_dataValuesAnchorNegativeDeltaX   = 0;
                    settings->_dataValuesAnchorNegativeDeltaY   = 0;
                    settings->_dataValuesNegativeRotation       = 0;
                    // for values greater/equal zero:
                    settings->_dataValuesAnchorNegativePosition = KDChartEnums::PosCenter;
                    settings->_dataValuesAnchorNegativeAlign    = Qt::AlignCenter;
                    settings->_dataValuesAnchorNegativeDeltaX   = 0;
                    settings->_dataValuesAnchorNegativeDeltaY   = 0;
                    settings->_dataValuesNegativeRotation       = 0;

                    settings->_dataValuesLayoutPolicy = KDChartEnums::LayoutPolicyShrinkFontSize;
                    */
                    setDefaultAxesTypes();
                    finished = false;  // use build-in default params, see KDChartParams.h::setPrintDataValues()
                }
                break;

                default: {
                    qDebug( "IMPLEMENTATION ERROR: Unknown chartType in setPrintDataValues()" );
                    finished = false;  // use build-in default params, see KDChartParams.h::setPrintDataValues()
                }
            }
        }
        if ( !finished ) {
            settings->_useDefaultFontParams = false;

            if ( font )
                settings->_dataValuesFont = *font;
            else
                settings->_dataValuesFont = QFont( "times", 1, QFont::Bold );

            uint theSize( UINT_MAX == size ? 16 : size );
            settings->_dataValuesUseFontRelSize = ( 0 < theSize );
            settings->_dataValuesFontRelSize = theSize;
            if (    DATA_VALUE_AUTO_COLOR == color
                 && ( Polar != cType || polarMarker() ) ) {
                settings->_dataValuesAutoColor            = true;  //  !!!
                settings->_dataValuesColor = QColor( Qt::black );
            }
            else {
                settings->_dataValuesAutoColor = false;
                if ( 0 == color )
                    settings->_dataValuesColor
                                    = QColor( i ? Qt::darkBlue : Qt::black );
                else
                    settings->_dataValuesColor = *color;
            }
            // for values below zero:
            settings->_dataValuesAnchorNegativePosition = negativePosition;
            settings->_dataValuesAnchorNegativeAlign    = negativeAlign;
            settings->_dataValuesAnchorNegativeDeltaX   = negativeDeltaX;
            settings->_dataValuesAnchorNegativeDeltaY   = negativeDeltaY;
            settings->_dataValuesNegativeRotation       = negativeRotation;
            // for values greater/equal zero:
            settings->_dataValuesAnchorPositivePosition = positivePosition;
            settings->_dataValuesAnchorPositiveAlign    = positiveAlign;
            settings->_dataValuesAnchorPositiveDeltaX   = positiveDeltaX;
            settings->_dataValuesAnchorPositiveDeltaY   = positiveDeltaY;
            settings->_dataValuesPositiveRotation       = positiveRotation;

            settings->_dataValuesLayoutPolicy = policy;
        }
        if ( 0 < chart )
            settings = &_printDataValuesSettings2;
    }
    emit changed();
}


void KDChartParams::setChartSourceMode( SourceMode mode,
                                        uint dataset,
                                        uint dataset2,
                                        uint chart )
{
    if ( KDCHART_NO_DATASET != dataset
         && KDCHART_ALL_DATASETS != dataset
         && KDCHART_ALL_DATASETS != dataset2 ) {
        uint i;
        uint last = ( KDCHART_NO_DATASET == dataset2 ) ? dataset : dataset2;
        for ( i = dataset; i <= last; ++i )
            _dataSourceModeAndChart[ i ] = ModeAndChart( mode, chart );
        _maxDatasetSourceMode = QMAX( _maxDatasetSourceMode, --i );
        _setChartSourceModeWasUsed = true;
    } else if ( UnknownMode == mode && dataset == KDCHART_ALL_DATASETS ) {
        _dataSourceModeAndChart.clear();
        _setChartSourceModeWasUsed = false;
    }

    emit changed();
}

KDChartParams::SourceMode KDChartParams::chartSourceMode( uint dataset,
                                                          uint dataset2,
                                                          uint* pChart ) const
{
    uint chart = KDCHART_UNKNOWN_CHART;
    SourceMode mode = UnknownMode;

    if ( _setChartSourceModeWasUsed ) {
        if (    dataset <= _maxDatasetSourceMode
                && (    KDCHART_NO_DATASET == dataset2
                        || dataset2 <= _maxDatasetSourceMode ) ) {

            uint a, b;
            if ( KDCHART_ALL_DATASETS == dataset ) {
                a = 0;
                b = UINT_MAX;
            } else {
                a = dataset;
                b = KDCHART_NO_DATASET == dataset2 ? a : dataset2;
            }

            bool bStart = true;
            ModeAndChartMap::ConstIterator it;
            for ( it = _dataSourceModeAndChart.find( a );
                  ( it != _dataSourceModeAndChart.end() ) && ( it.key() <= b );
                  ++it ) {
                if ( bStart ) {
                    mode = it.data().mode();
                    chart = it.data().chart();
                    bStart = false;
                } else {
                    if ( mode != it.data().mode() )
                        mode = UnknownMode;
                    if ( chart != it.data().chart() )
                        chart = KDCHART_UNKNOWN_CHART;
                }
            }
        }
    } else {
        mode = DataEntry;
        chart = 0;
    }
    if ( pChart )
        * pChart = chart;
    return mode;
}

bool KDChartParams::findDataset( SourceMode mode,
                                 uint& dataset,
                                 uint& dataset2,
                                 uint chart ) const
{
    bool res = false;
    if ( _setChartSourceModeWasUsed ) {
        bool bStart = true;
        ModeAndChartMap::ConstIterator it;
        for ( it = _dataSourceModeAndChart.begin();
              it != _dataSourceModeAndChart.end(); ++it ) {
            if ( it.data().mode() == mode
                 && it.data().chart() == chart ) {
                if ( bStart ) {
                    dataset = it.key();
                    bStart = false;
                }
                dataset2 = it.key();
                res = true;
            } else if ( !bStart )
                return res;
        }
    } else {
        if ( DataEntry
             == mode ) {
            dataset = KDCHART_ALL_DATASETS;
            dataset2 = KDCHART_ALL_DATASETS;
            res = true;
        }
    }
    return res;
}


void KDChartParams::setDataColor( uint dataset, QColor color )
{
    _dataColors[ dataset ] = color;
    if ( !color.isValid() ) { // no fill color
        _dataColorsShadow1[ dataset ] = QColor();
        _dataColorsShadow2[ dataset ] = QColor();
    } else {
        int hue, saturation, value;
        color.hsv( &hue, &saturation, &value );
        QColor shadowColor1;
        double v = value;
        v = v * 2.0 / 3.0 * shadowBrightnessFactor();
        if ( 255.0 < v )
            v = 255.0;
        else if ( 1.0 > v )
            v = 0.0;
        shadowColor1.setHsv( hue, saturation, static_cast < int > ( v ) );
        _dataColorsShadow1[ dataset ] = shadowColor1;
        QColor shadowColor2;
        v = value;
        v = v / 3.0 * shadowBrightnessFactor();
        if ( 255.0 < v )
            v = 255.0;
        else if ( 1.0 > v )
            v = 0.0;
        shadowColor2.setHsv( hue, saturation, static_cast < int > ( v ) );
        _dataColorsShadow2[ dataset ] = shadowColor2;
    }

    _maxDatasetColor = QMAX( _maxDatasetColor, dataset );

    emit changed();
}


QColor KDChartParams::dataColor( uint dataset ) const
{
    uint index = dataset % _maxDatasetColor;
    if( _dataColors.find( index ) != _dataColors.end() )
        return _dataColors[ index ];
    else
        return QColor(); // documentation says undefined
}


void KDChartParams::recomputeShadowColors()
{
    // Simply reassign the available colors; this will trigger
    // recomputation of the shadow colors.
    for( QMap<uint,QColor>::Iterator it = _dataColors.begin();
         it != _dataColors.end(); ++it ) {
        setDataColor( it.key(), it.data() );
    }
}


QColor KDChartParams::dataShadow1Color( uint dataset ) const
{
    uint index = dataset % _maxDatasetColor;
    if ( _threeDShadowColors )
        if( _dataColorsShadow1.find( index ) != _dataColorsShadow1.end() )
            return _dataColorsShadow1[ index ];
        else
            return QColor(); // documentation says undefined
    else
        if( _dataColors.find( index ) != _dataColors.end() )
            return _dataColors[ index ];
        else
            return QColor(); // documentation says undefined
}


QColor KDChartParams::dataShadow2Color( uint dataset ) const
{
    uint index = dataset % _maxDatasetColor;
    if ( _threeDShadowColors )
        if( _dataColorsShadow2.find( index ) != _dataColorsShadow2.end() )
            return _dataColorsShadow2[ index ];
        else
            return QColor(); // documentation says undefined
    else
        if( _dataColors.find( index ) != _dataColors.end() )
            return _dataColors[ index ];
        else
            return QColor(); // documentation says undefined
}


void KDChartParams::setDefaultAxesTypes()
{
    // reset types of all axes
    uint i = 0;
    for ( i = 0; i < KDCHART_MAX_AXES; ++i )
        setAxisType( i, KDChartAxisParams::AxisTypeUnknown );

    // Note that abscissa axes should start labeling at the very
    // first position and end at the last position when drawing
    // area charts.
    // Bar charts and line charts look better with their abscissa labels
    // in the respective middle positions below each bar (or point, resp.)
    for ( i = KDChartAxisParams::AxisPosSTART;
          i <= KDChartAxisParams::AxisPosEND; ++i )
        switch ( i ) {
            // abscissa axes:
        case KDChartAxisParams::AxisPosBottom:
        case KDChartAxisParams::AxisPosTop:
        case KDChartAxisParams::AxisPosBottom2:
        case KDChartAxisParams::AxisPosTop2:
            setAxisLabelsTouchEdges( i, Area == chartType() );
            break;
            // ordinate axes:
        case KDChartAxisParams::AxisPosLeft:
        case KDChartAxisParams::AxisPosRight:
        case KDChartAxisParams::AxisPosLeft2:
        case KDChartAxisParams::AxisPosRight2:
            setAxisLabelsTouchEdges( i, true ); //Polar != chartType() );
            break;
            // additional axes for charts representing 3-dimensional data:
        case KDChartAxisParams::AxisPosLowerRightEdge:
        case KDChartAxisParams::AxisPosLowerLeftEdge:
        case KDChartAxisParams::AxisPosLowerRightEdge2:
        case KDChartAxisParams::AxisPosLowerLeftEdge2:
            setAxisLabelsTouchEdges( i, false );
            break;
        default: {
            qDebug( "IMPLEMENTATION ERROR: axis type missing in KDChartParams::setDefaultAxesTypes()" );
            Q_ASSERT( !this );
        }
        }

    // set default axis types according to chart type
    switch ( chartType() ) {
    case NoType:
        break;
    case Bar:
    case Line:
    case Area:
        // default axes
        setAxisType( KDChartAxisParams::AxisPosBottom,
                     KDChartAxisParams::AxisTypeEAST );
        setAxisShowGrid( KDChartAxisParams::AxisPosBottom,
                         true );
        setAxisType( KDChartAxisParams::AxisPosLeft,
                     KDChartAxisParams::AxisTypeNORTH );
        setAxisShowGrid( KDChartAxisParams::AxisPosLeft,
                         true );
        // 'other side' axes
        setAxisType( KDChartAxisParams::AxisPosTop,
                     KDChartAxisParams::AxisTypeEAST );
        setAxisType( KDChartAxisParams::AxisPosRight,
                     KDChartAxisParams::AxisTypeNORTH );
        // additional, 2nd axes
        setAxisType( KDChartAxisParams::AxisPosBottom2,
                     KDChartAxisParams::AxisTypeEAST );
        setAxisType( KDChartAxisParams::AxisPosLeft2,
                     KDChartAxisParams::AxisTypeNORTH );
        // additional, 2nd axes for 'other' sides
        setAxisType( KDChartAxisParams::AxisPosTop2,
                     KDChartAxisParams::AxisTypeEAST );
        setAxisType( KDChartAxisParams::AxisPosRight2,
                     KDChartAxisParams::AxisTypeNORTH );

        // Specify default numbering information
        // for all 'non-ordinate' axes (this are the X axes):
        //
        // axisSteadyValueCalc flag is set to false
        // Start value 1
        // End value   following the number of entries
        //             in the associated dataset(s)
        // Delta value 1.0
        // and dont show any Digits behind the comma.
        setAxisLabelTextParams( KDChartAxisParams::AxisPosBottom, false,
                                1.0,
                                KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT,
                                1.0, 0 );
        setAxisLabelTextParams( KDChartAxisParams::AxisPosTop, false,
                                1.0,
                                KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT,
                                1.0, 0 );
        setAxisLabelTextParams( KDChartAxisParams::AxisPosBottom2, false,
                                1.0,
                                KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT,
                                1.0, 0 );
        setAxisLabelTextParams( KDChartAxisParams::AxisPosTop2, false,
                                1.0,
                                KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT,
                                1.0, 0 );

        // no need to specify numbering information for
        // the ordinate-axes since the default auto-calc
        // feature is fine for here.

        break;

        // Code for charts representing 3-dimensional data.

        /*

        //   ( not active since there are no such charts yet )

        case KDChartParams::BarMatrix:
        // default axes
        setAxisType( KDChartAxisParams::AxisPosBottom,
        KDChartAxisParams::AxisTypeEAST );
        setAxisType( KDChartAxisParams::AxisPosLowerRightEdge,
        KDChartAxisParams::AxisTypeNORTH );
        setAxisType( KDChartAxisParams::AxisPosLeft,
        KDChartAxisParams::AxisTypeUP );
        // 'other side' axes
        setAxisType( KDChartAxisParams::AxisPosTop,
        KDChartAxisParams::AxisTypeEAST );
        setAxisType( KDChartAxisParams::AxisPosLowerLeftEdge,
        KDChartAxisParams::AxisTypeNORTH );
        setAxisType( KDChartAxisParams::AxisPosRight,
        KDChartAxisParams::AxisTypeUP );
        // additional, 2nd axes
        setAxisType( KDChartAxisParams::AxisPosBottom2,
        KDChartAxisParams::AxisTypeEAST );
        setAxisType( KDChartAxisParams::AxisPosLowerRightEdge2,
        KDChartAxisParams::AxisTypeNORTH );
        setAxisType( KDChartAxisParams::AxisPosLeft2,
        KDChartAxisParams::AxisTypeUP );
        // additional, 2nd axes for 'other' sides
        setAxisType( KDChartAxisParams::AxisPosTop2,
        KDChartAxisParams::AxisTypeEAST );
        setAxisType( KDChartAxisParams::AxisPosLowerLeftEdge2,
        KDChartAxisParams::AxisTypeNORTH );
        setAxisType( KDChartAxisParams::AxisPosRight2,
        KDChartAxisParams::AxisTypeUP );

        // Specify default numbering information
        // for all 'non-ordinate' axes (this are the X and the Y axes):
        // Start vaule 1
        // End value   following the number of entries
        //             in the associated dataset(s)
        // Delta value 1.0
        // and don't show any Digits behind the comma.
        setAxisLabelTextParams( KDChartAxisParams::AxisPosBottom,  false,
        1.0, AXIS_LABELS_AUTO_LIMIT, 1.0, 0 );
        setAxisLabelTextParams( KDChartAxisParams::AxisPosTop,     false,
        1.0, AXIS_LABELS_AUTO_LIMIT, 1.0, 0 );
        setAxisLabelTextParams( KDChartAxisParams::AxisPosLowerRightEdge,
        false,
        1.0, AXIS_LABELS_AUTO_LIMIT, 1.0, 0 );
        setAxisLabelTextParams( KDChartAxisParams::AxisPosLowerLeftEdge,
        false,
        1.0, AXIS_LABELS_AUTO_LIMIT, 1.0, 0 );
        setAxisLabelTextParams( KDChartAxisParams::AxisPosBottom2, false,
        1.0, AXIS_LABELS_AUTO_LIMIT, 1.0, 0 );
        setAxisLabelTextParams( KDChartAxisParams::AxisPosTop2,    false,
        1.0, AXIS_LABELS_AUTO_LIMIT, 1.0, 0 );
        setAxisLabelTextParams(KDChartAxisParams::AxisPosLowerRightEdge2,
        false,
        1.0, AXIS_LABELS_AUTO_LIMIT, 1.0, 0 );
        setAxisLabelTextParams( KDChartAxisParams::AxisPosLowerLeftEdge2,
        false,
        1.0, AXIS_LABELS_AUTO_LIMIT, 1.0, 0 );

         false,
        1.0, AXIS_LABELS_AUTO_LIMIT, 1.0, 0 );

       // no need to specify numbering information for
        // the ordinate-axes since the default auto-calc
        // feature is fine for here.

        break;
        */

    case KDChartParams::Pie:
    case KDChartParams::Ring:
        // by default there are no axes defined for pie and ring charts
        break;
    case KDChartParams::Polar:
        setAxisType( KDChartAxisParams::AxisPosSaggital,
                     KDChartAxisParams::AxisTypeEAST );
        setAxisType( KDChartAxisParams::AxisPosCircular,
                     KDChartAxisParams::AxisTypeNORTH );
        setAxisLabelsVisible( KDChartAxisParams::AxisPosSaggital, true );
        setAxisLabelsFont(    KDChartAxisParams::AxisPosSaggital,
                              QFont( "helvetica", 1, QFont::Bold ),
                              24,
                              Qt::darkBlue );
        setAxisLabelsVisible( KDChartAxisParams::AxisPosCircular, true );
        setAxisLabelsFont(    KDChartAxisParams::AxisPosCircular,
                              QFont( "helvetica", 1, QFont::Bold ),
                              18,
                              Qt::darkBlue );
        setPolarRotateCircularLabels( false );
        break;
    default: {
        qDebug( "IMPLEMENTATION ERROR: type missing in KDChartParams::setDefaultAxesTypes()" );
        Q_ASSERT( !this );
    }
    }

    emit changed();
}


void KDChartParams::activateDefaultAxes()
{
    // deactivate all axes
    for ( uint i = 0; i < KDCHART_MAX_AXES; ++i ) {
        setAxisVisible( i, false );
        setAxisDatasets( i, KDCHART_NO_DATASET );
    }
    switch ( chartType() ) {
    case NoType:
        break;
    case Bar:
    case Line:
    case Area:
        setAxisVisible( KDChartAxisParams::AxisPosBottom, true );
        setAxisDatasets( KDChartAxisParams::AxisPosBottom, KDCHART_ALL_DATASETS );
        setAxisVisible( KDChartAxisParams::AxisPosLeft, true );
        setAxisDatasets( KDChartAxisParams::AxisPosLeft, KDCHART_ALL_DATASETS );
        /* test:
           setAxisVisible( KDChartAxisParams::AxisPosTop,    true );
           setAxisDatasets(KDChartAxisParams::AxisPosTop,    KDCHART_ALL_DATASETS );
           setAxisVisible( KDChartAxisParams::AxisPosRight,  true );
           setAxisDatasets(KDChartAxisParams::AxisPosRight,  KDCHART_ALL_DATASETS );

           setAxisVisible( KDChartAxisParams::AxisPosBottom2, true );
           setAxisDatasets(KDChartAxisParams::AxisPosBottom2, KDCHART_ALL_DATASETS );
           setAxisVisible( KDChartAxisParams::AxisPosLeft2,   true );
           setAxisDatasets(KDChartAxisParams::AxisPosLeft2,   KDCHART_ALL_DATASETS );

           setAxisVisible( KDChartAxisParams::AxisPosTop2,    true );
           setAxisDatasets(KDChartAxisParams::AxisPosTop2,    KDCHART_ALL_DATASETS );
           setAxisVisible( KDChartAxisParams::AxisPosRight2,  true );
           setAxisDatasets(KDChartAxisParams::AxisPosRight2,  KDCHART_ALL_DATASETS );
        */
        break;
        // Code for charts representing 3-dimensional data.

        /*

        //   ( not active since there are no such charts yet )

        case KDChartParams::BarMatrix:
        setAxisVisible( KDChartAxisParams::AxisPosBottom,         true );
        setAxisDatasets(KDChartAxisParams::AxisPosBottom, KDCHART_ALL_DATASETS );
        setAxisVisible( KDChartAxisParams::AxisPosLeft,           true );
        setAxisDatasets(KDChartAxisParams::AxisPosLeft,   KDCHART_ALL_DATASETS );
        setAxisVisible( KDChartAxisParams::AxisPosLowerRightEdge, true );
        setAxisDatasets(KDChartAxisParams::AxisPosLowerRightEdge,
        KDCHART_ALL_DATASETS);
        break;
        */
    case KDChartParams::Pie:
    case KDChartParams::Ring:
    case KDChartParams::Polar:
        // by default there are no axes defined for pie, ring, and polar charts
        break;
    default: {
        qDebug( "IMPLEMENTATION ERROR: type missing in KDChartParams::activateDefaultAxes()" );
        Q_ASSERT( !this );
    }
    }
}



QTextStream& operator<<( QTextStream& s, const KDChartParams& p )
{
    QDomDocument document = p.saveXML();
    s << document.toString();

    return s;
}


QDomDocument KDChartParams::saveXML( bool withPI ) const
{
    // Create an inital DOM document
    QString docstart = "<ChartParams/>";

    QDomDocument doc( "ChartParams" );
    doc.setContent( docstart );
    if( withPI )
        doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement docRoot = doc.documentElement();
    docRoot.setAttribute( "xmlns", "http://www.klaralvdalens-datakonsult.se/kdchart" );
    docRoot.setAttribute( "xmlns:xsi", "http://www.w3.org/2000/10/XMLSchema-instance" );
    docRoot.setAttribute( "xsi:schemaLocation", "http://www.klaralvdalens-datakonsult.se/kdchart" );

    // the ChartType element
    QDomElement chartTypeElement = doc.createElement( "ChartType" );
    docRoot.appendChild( chartTypeElement );
    chartTypeElement.setAttribute( "primary",
                                   KDChartParams::chartTypeToString( _chartType ) );
    chartTypeElement.setAttribute( "secondary",
                                   KDChartParams::chartTypeToString( _additionalChartType ) );


    // the NumValues element
    KDXML::createIntNode( doc, docRoot, "NumValues", _numValues );

    // the ModeAndChartMap element
    QDomElement modeAndChartMapElement =
        doc.createElement( "ModeAndChartMap" );
    docRoot.appendChild( modeAndChartMapElement );
    for( QMap<uint,KDChartParams::ModeAndChart>::ConstIterator it = _dataSourceModeAndChart.begin();
         it != _dataSourceModeAndChart.end(); ++it ) {
        // Dataset element
        QDomElement datasetElement = doc.createElement( "Dataset" );
        QDomText datasetContent =
            doc.createTextNode( QString::number( it.key() ) );
        datasetElement.appendChild( datasetContent );
        modeAndChartMapElement.appendChild( datasetElement );
        // ModeAndChart element
        QDomElement modeAndChartElement = doc.createElement( "ModeAndChart" );
        modeAndChartElement.setAttribute( "Mode", it.data().mode() );
        modeAndChartElement.setAttribute( "Chart", it.data().chart() );
        modeAndChartMapElement.appendChild( modeAndChartElement );
    }

    KDXML::createBoolNode( doc, docRoot, "ChartSourceModeWasUsed",
                           _setChartSourceModeWasUsed );

    // the MaxDatasetSourceMode element
    KDXML::createIntNode( doc, docRoot, "MaxDatasetSourceMode",
                          _maxDatasetSourceMode );

    // the ColorSettings element
    QDomElement colorSettingsElement =
        doc.createElement( "ColorSettings" );
    docRoot.appendChild( colorSettingsElement );

    {
        // the DataColors element
        createColorMapNode( doc, colorSettingsElement,
                            "DataColors", _dataColors );

        // the MaxDatasetColor element
        KDXML::createIntNode( doc, colorSettingsElement, "MaxDatasetColor",
                              _maxDatasetColor );

        // the ShadowBrightnessFactor element
        KDXML::createDoubleNode( doc, colorSettingsElement,
                                 "ShadowBrightnessFactor",
                                 _shadowBrightnessFactor );

        // the ShadowPattern element
        KDXML::createStringNode( doc, colorSettingsElement,
                                 "ShadowPattern",
                                 KDXML::brushStyleToString(_shadowPattern ) );

        // the ThreeDShadowColors element
        KDXML::createBoolNode( doc, colorSettingsElement,
                               "ThreeDShadowColors",
                               _threeDShadowColors );

        // the DataColorsShadow1 element
        createColorMapNode( doc, colorSettingsElement,
                            "DataColorsShadow1",
                            _dataColorsShadow1 );

        // the DataColorsShadow2 element
        createColorMapNode( doc, colorSettingsElement,
                            "DataColorsShadow2",
                            _dataColorsShadow2 );

        // the OutlineDataColor element
        KDXML::createColorNode( doc, colorSettingsElement,
                                "OutlineDataColor",
                                _outlineDataColor );

        // the OutlineDataLineWidth element
        KDXML::createIntNode( doc, colorSettingsElement,
                              "OutlineDataLineWidth",
                              _outlineDataLineWidth );

        // the OutlineDataLineStyle element
        QDomElement outlineDataLineStyleElement =
            doc.createElement( "OutlineDataLineStyle" );
        colorSettingsElement.appendChild( outlineDataLineStyleElement );
        outlineDataLineStyleElement.setAttribute( "Style",
                                                  KDXML::penStyleToString( _outlineDataLineStyle ) );
    }


    // the BarSettings elemenet
    QDomElement barSettingsElement =
        doc.createElement( "BarSettings" );
    docRoot.appendChild( barSettingsElement );

    {
        // the SubType element
        KDXML::createStringNode( doc, barSettingsElement,
                                 "SubType", KDChartParams::barChartSubTypeToString( _barChartSubType ) );

        KDXML::createBoolNode( doc, barSettingsElement,
                               "ThreeDBars", _threeDBars );

        KDXML::createDoubleNode( doc, barSettingsElement,
                                 "ThreeDBarDepth", _threeDBarDepth );

        KDXML::createIntNode( doc, barSettingsElement,
                              "DatasetGap", _datasetGap );

        KDXML::createBoolNode( doc, barSettingsElement,
                               "DatasetGapIsRelative", _datasetGapIsRelative );

        KDXML::createIntNode( doc, barSettingsElement,
                              "ValueBlockGap", _valueBlockGap );

        KDXML::createBoolNode( doc, barSettingsElement,
                               "ValueBlockGapIsRelative",
                               _valueBlockGapIsRelative );
    }


    // the LineSettings element
    QDomElement lineSettingsElement =
        doc.createElement( "LineSettings" );
    docRoot.appendChild( lineSettingsElement );

    {
        // the SubType element
        KDXML::createStringNode( doc, lineSettingsElement,
                                 "SubType", KDChartParams::lineChartSubTypeToString( _lineChartSubType ) );

        // the Marker element
        KDXML::createBoolNode( doc, lineSettingsElement,
                               "Marker", _lineMarker );

        // the MarkerStyles elements
        for( QMap<uint,KDChartParams::LineMarkerStyle>::ConstIterator it = _lineMarkerStyles.begin();
             it != _lineMarkerStyles.end(); ++it ) {
            QDomElement markerStyleElement = doc.createElement( "MarkerStyle" );
            lineSettingsElement.appendChild( markerStyleElement );
            markerStyleElement.setAttribute( "Dataset", it.key() );
            markerStyleElement.setAttribute( "Style", KDChartParams::lineMarkerStyleToString( it.data() ) );
        }

        // the MarkerSize element
        KDXML::createSizeNode( doc, lineSettingsElement,
                               "MarkerSize", _lineMarkerSize );

        // the LineWidth element
        KDXML::createIntNode( doc, lineSettingsElement,
                              "LineWidth", _lineWidth );

	// the ThreeD element
	KDXML::createBoolNode( doc, lineSettingsElement,
			       "ThreeD", _threeDLines );

	// the ThreeDDepth element
	KDXML::createIntNode( doc, lineSettingsElement,
			      "ThreeDDepth", _threeDLineDepth );

	// the ThreeDXRotation element
	KDXML::createIntNode( doc, lineSettingsElement,
			      "ThreeDXRotation", _threeDLineXRotation );

	// the ThreeDYRotation element
	KDXML::createIntNode( doc, lineSettingsElement,
			      "ThreeDYRotation", _threeDLineYRotation );
    }


    // the AreaSettings element
    QDomElement areaSettingsElement =
        doc.createElement( "AreaSettings" );
    docRoot.appendChild( areaSettingsElement );

    {
        // the SubType element
        KDXML::createStringNode( doc, areaSettingsElement, "SubType",
                                 KDChartParams::areaChartSubTypeToString( _areaChartSubType ) );

        // the Location elemenet
        KDXML::createStringNode( doc, areaSettingsElement, "Location",
                                 KDChartParams::areaLocationToString( _areaLocation ) );
    }


    // the PieRingSettings element
    QDomElement pieRingSettingsElement =
        doc.createElement( "PieRingSettings" );
    docRoot.appendChild( pieRingSettingsElement );

    {
        // the Explode element
        KDXML::createBoolNode( doc, pieRingSettingsElement, "Explode",
                               _explode );

        // the DefaultExplodeFactor element
        KDXML::createDoubleNode( doc, pieRingSettingsElement, "DefaultExplodeFactor",
                                 _explodeFactor );

        // the ExplodeFactors element
        createDoubleMapNode( doc, pieRingSettingsElement, "ExplodeFactors",
                             _explodeFactors );

        // the ExplodeSegment element
        for( QValueList<int>::ConstIterator it = _explodeList.begin();
             it != _explodeList.end(); ++it )
            KDXML::createIntNode( doc, pieRingSettingsElement,
                                  "ExplodeSegment", *it );

        // the ThreeDPies element
        KDXML::createBoolNode( doc, pieRingSettingsElement, "ThreeDPies",
                               _threeDPies );

        // the ThreeDPieHeight element
        KDXML::createIntNode( doc, pieRingSettingsElement, "ThreeDPieHeight",
                              _threeDPieHeight );

        // the PieStart element
        KDXML::createIntNode( doc, pieRingSettingsElement, "PieStart",
                              _pieStart );

        // the RingStart element
        KDXML::createIntNode( doc, pieRingSettingsElement, "RingStart",
                              _ringStart );

        // the RelativeRingThickness element
        KDXML::createBoolNode( doc, pieRingSettingsElement,
                               "RelativeRingThickness", _relativeRingThickness );
    }

    // the HiLoSettings element
    QDomElement hiLoSettingsElement =
        doc.createElement( "HiLoSettings" );
    docRoot.appendChild( hiLoSettingsElement );
    {
        // the SubType element
        KDXML::createStringNode( doc, hiLoSettingsElement, "SubType",
                                 KDChartParams::hiLoChartSubTypeToString( _hiLoChartSubType ) );

        // the PrintLowValues element
        KDXML::createBoolNode( doc, hiLoSettingsElement, "PrintLowValues",
                               _hiLoChartPrintLowValues );

        // the LowValuesFont element
        createChartFontNode( doc, hiLoSettingsElement, "LowValuesFont",
                             _hiLoChartLowValuesFont,
                             _hiLoChartLowValuesUseFontRelSize,
                             _hiLoChartLowValuesFontRelSize );

        // the LowValuesColor element
        KDXML::createColorNode( doc, hiLoSettingsElement, "LowValuesColor",
                                _hiLoChartLowValuesColor );

        // the PrintHighValues element
        KDXML::createBoolNode( doc, hiLoSettingsElement, "PrintHighValues",
                               _hiLoChartPrintHighValues );

        // the HighValuesFont element
        createChartFontNode( doc, hiLoSettingsElement, "HighValuesFont",
                             _hiLoChartHighValuesFont,
                             _hiLoChartHighValuesUseFontRelSize,
                             _hiLoChartHighValuesFontRelSize );

        // the HighValuesColor element
        KDXML::createColorNode( doc, hiLoSettingsElement, "HighValuesColor",
                                _hiLoChartHighValuesColor );

        // the PrintOpenValues element
        KDXML::createBoolNode( doc, hiLoSettingsElement, "PrintOpenValues",
                               _hiLoChartPrintOpenValues );

        // the OpenValuesFont element
        createChartFontNode( doc, hiLoSettingsElement, "OpenValuesFont",
                             _hiLoChartOpenValuesFont,
                             _hiLoChartOpenValuesUseFontRelSize,
                             _hiLoChartOpenValuesFontRelSize );

        // the OpenValuesColor element
        KDXML::createColorNode( doc, hiLoSettingsElement, "OpenValuesColor",
                                _hiLoChartOpenValuesColor );

        // the PrintCloseValues element
        KDXML::createBoolNode( doc, hiLoSettingsElement, "PrintCloseValues",
                               _hiLoChartPrintCloseValues );

        // the CloseValuesFont element
        createChartFontNode( doc, hiLoSettingsElement, "CloseValuesFont",
                             _hiLoChartCloseValuesFont,
                             _hiLoChartCloseValuesUseFontRelSize,
                             _hiLoChartCloseValuesFontRelSize );

        // the CloseValuesColor element
        KDXML::createColorNode( doc, hiLoSettingsElement, "CloseValuesColor",
                                _hiLoChartCloseValuesColor );
    }


    // the GanttSettings element
    QDomElement ganttSettingsElement =
        doc.createElement( "GanttSettings" );
    docRoot.appendChild( ganttSettingsElement );
    {
        // the SubType element
        KDXML::createStringNode( doc, ganttSettingsElement, "SubType",
                                 KDChartParams::ganttChartSubTypeToString( _ganttChartSubType ) );

        // the TemporalResolution element
        KDXML::createStringNode( doc, ganttSettingsElement, "TemporalResolution",
                                 KDChartParams::ganttTemporalResolutionToString( _ganttChartTemporalResolution ) );

        // the PrintStartValues element
        KDXML::createBoolNode( doc, ganttSettingsElement, "PrintStartValues",
                               _ganttChartPrintStartValues );

        // the StartValuesPos element
        QDomElement startValuesPosElement =
            doc.createElement( "StartValuesPos" );
        ganttSettingsElement.appendChild( startValuesPosElement );
        KDXML::createStringNode( doc, startValuesPosElement, "PositionFlag",
                                 KDChartEnums::positionFlagToString( _ganttChartStartValuesPos ) );

        // the StartValuesAlign element
        KDXML::createIntNode( doc, ganttSettingsElement,
                              "StartValuesAlign",
                              static_cast < int > ( _ganttChartStartValuesAlign ) );

        // the StartValuesDateTimeFormat element
        KDXML::createStringNode( doc, ganttSettingsElement,
                                 "StartValuesDateTimeFormat",
                                 _ganttChartStartValuesDateTimeFormat );

        // the StartValuesFont element
        createChartFontNode( doc, ganttSettingsElement,
                             "StartValuesFont",
                             _ganttChartStartValuesFont,
                             _ganttChartStartValuesUseFontRelSize,
                             _ganttChartStartValuesFontRelSize );

        // the StartValuesColor element
        KDXML::createColorNode( doc, ganttSettingsElement, "StartValuesColor",
                                _ganttChartStartValuesColor );

        // the PrintEndValues element
        KDXML::createBoolNode( doc, ganttSettingsElement, "PrintEndValues",
                               _ganttChartPrintEndValues );

        // the EndValuesPos element
        QDomElement endValuesPosElement =
            doc.createElement( "EndValuesPos" );
        ganttSettingsElement.appendChild( endValuesPosElement );
        KDXML::createStringNode( doc, endValuesPosElement, "PositionFlag",
                                 KDChartEnums::positionFlagToString( _ganttChartEndValuesPos ) );

        // the EndValuesAlign element
        KDXML::createIntNode( doc, ganttSettingsElement,
                              "EndValuesAlign",
                              static_cast < int > ( _ganttChartEndValuesAlign ) );

        // the EndValuesDateTimeFormat element
        KDXML::createStringNode( doc, ganttSettingsElement,
                                 "EndValuesDateTimeFormat",
                                 _ganttChartEndValuesDateTimeFormat );

        // the EndValuesFont element
        createChartFontNode( doc, ganttSettingsElement,
                             "EndValuesFont",
                             _ganttChartEndValuesFont,
                             _ganttChartEndValuesUseFontRelSize,
                             _ganttChartEndValuesFontRelSize );

        // the EndValuesColor element
        KDXML::createColorNode( doc, ganttSettingsElement, "EndValuesColor",
                                _ganttChartEndValuesColor );

        // the PrintDurations element
        KDXML::createBoolNode( doc, ganttSettingsElement, "PrintDurations",
                               _ganttChartPrintDurations );

        // the DurationsPos element
        QDomElement durationsPosElement =
            doc.createElement( "DurationsPos" );
        ganttSettingsElement.appendChild( durationsPosElement );
        KDXML::createStringNode( doc, durationsPosElement, "PositionFlag",
                                 KDChartEnums::positionFlagToString( _ganttChartDurationsPos ) );

        // the DurationsAlign element
        KDXML::createIntNode( doc, ganttSettingsElement,
                              "DurationsAlign",
                              static_cast < int > ( _ganttChartDurationsAlign ) );

        // the DurationsDateTimeFormat element
        KDXML::createStringNode( doc, ganttSettingsElement,
                                 "DurationsDateTimeFormat",
                                 _ganttChartDurationsDateTimeFormat );

        // the DurationsFont element
        createChartFontNode( doc, ganttSettingsElement,
                             "DurationsFont",
                             _ganttChartDurationsFont,
                             _ganttChartDurationsUseFontRelSize,
                             _ganttChartDurationsFontRelSize );

        // the DurationsColor element
        KDXML::createColorNode( doc, ganttSettingsElement, "DurationsColor",
                                _ganttChartDurationsColor );
    }


    // the PolarSettings element
    QDomElement polarSettingsElement =
        doc.createElement( "PolarSettings" );
    docRoot.appendChild( polarSettingsElement );

    {
        // the SubType element
        KDXML::createStringNode( doc, polarSettingsElement,
                                 "SubType", KDChartParams::polarChartSubTypeToString( _polarChartSubType ) );

        // the Marker element
        KDXML::createBoolNode( doc, polarSettingsElement,
                               "Marker", _polarMarker );

        // the MarkerStyles elements
        for( QMap<uint,KDChartParams::PolarMarkerStyle>::ConstIterator it = _polarMarkerStyles.begin();
             it != _polarMarkerStyles.end(); ++it ) {
            QDomElement markerStyleElement = doc.createElement( "MarkerStyle" );
            polarSettingsElement.appendChild( markerStyleElement );
            markerStyleElement.setAttribute( "Dataset", it.key() );
            markerStyleElement.setAttribute( "Style", KDChartParams::polarMarkerStyleToString( it.data() ) );
        }

        // the MarkerSize element
        KDXML::createSizeNode( doc, polarSettingsElement,
                               "MarkerSize", _polarMarkerSize );

        // the PolarLineWidth element
        KDXML::createIntNode( doc, polarSettingsElement,
                              "PolarLineWidth", _polarLineWidth );
    }



    // the LegendSettings element
    QDomElement legendSettingsElement =
        doc.createElement( "LegendSettings" );
    docRoot.appendChild( legendSettingsElement );
    {
        // the Position element
        KDXML::createStringNode( doc, legendSettingsElement, "Position",
                                 KDChartParams::legendPositionToString( _legendPosition ) );

        // the Source element
        KDXML::createStringNode( doc, legendSettingsElement, "Source",
                                 KDChartParams::legendSourceToString( _legendSource ) );

        // the Text elements
        for( QMap<int,QString>::ConstIterator it = _legendText.begin();
             it != _legendText.end(); ++it ) {
            QDomElement legendTextElement = doc.createElement( "LegendText" );
            legendSettingsElement.appendChild( legendTextElement );
            legendTextElement.setAttribute( "Dataset", it.key() );
            legendTextElement.setAttribute( "Text", it.data() );
        }

        // the TextColor element
        KDXML::createColorNode( doc, legendSettingsElement, "TextColor",
                                _legendTextColor );

        // the TextFont element
        createChartFontNode( doc, legendSettingsElement, "TextFont",
                             _legendFont,
                             _legendFontUseRelSize,
                             _legendFontRelSize );

        // the TitleText element
        KDXML::createStringNode( doc, legendSettingsElement, "TitleText",
                                 _legendTitleText );

        // the TitleColor element
        KDXML::createColorNode( doc, legendSettingsElement, "TitleColor",
                                _legendTitleTextColor );

        // the TitleFont element
        createChartFontNode( doc, legendSettingsElement, "TitleFont",
                             _legendTitleFont,
                             _legendTitleFontUseRelSize,
                             _legendTitleFontRelSize );

        // the Spacing element
        KDXML::createIntNode( doc, legendSettingsElement, "Spacing",
                              _legendSpacing );
    }

    // the AxisSettings elements
    for( int axis = 0; axis < 13; axis++ ) {
        QDomElement axisSettingsElement =
            doc.createElement( "AxisSettings" );
        docRoot.appendChild( axisSettingsElement );
        axisSettingsElement.setAttribute( "Dataset",
                                          _axisSettings[axis].dataset );
        axisSettingsElement.setAttribute( "Dataset2",
                                          _axisSettings[axis].dataset2 );
        axisSettingsElement.setAttribute( "Chart",
                                          _axisSettings[axis].chart );
        {
            // the Type element
            KDXML::createStringNode( doc, axisSettingsElement, "Type",
                                     KDChartAxisParams::axisTypeToString( _axisSettings[axis].params._axisType ) );

            // the Visible element
            KDXML::createBoolNode( doc, axisSettingsElement, "Visible",
                                   _axisSettings[axis].params._axisVisible );

            // the LabelsTouchEdges element
            KDXML::createBoolNode( doc, axisSettingsElement, "LabelsTouchEdges",
                                   _axisSettings[axis].params._axisLabelsTouchEdges );

            // the AreaMode element
            KDXML::createStringNode( doc, axisSettingsElement, "AreaMode",
                                     KDChartAxisParams::axisAreaModeToString( _axisSettings[axis].params._axisAreaMode ) );

            // the AreaMin element
            KDXML::createIntNode( doc, axisSettingsElement, "AreaMin",
                                  _axisSettings[axis].params._axisAreaMin );

            // the AreaMax element
            KDXML::createIntNode( doc, axisSettingsElement, "AreaMax",
                                  _axisSettings[axis].params._axisAreaMax );

            // the CalcMode element
            KDXML::createStringNode( doc, axisSettingsElement, "CalcMode",
                                     KDChartAxisParams::axisCalcModeToString( _axisSettings[axis].params._axisCalcMode ) );

            // the TrueAreaSize element
            KDXML::createIntNode( doc, axisSettingsElement, "TrueAreaSize",
                                  _axisSettings[axis].params._axisTrueAreaSize );

            // the TrueAreaRect element
            KDXML::createRectNode( doc, axisSettingsElement, "TrueAreaRect",
                                   _axisSettings[axis].params._axisTrueAreaRect );

            // the ShowSubDelimiters element
            KDXML::createBoolNode( doc, axisSettingsElement, "ShowSubDelimiters",
                                   _axisSettings[axis].params._axisShowSubDelimiters );

            // the LineVisible element
            KDXML::createBoolNode( doc, axisSettingsElement, "LineVisible",
                                   _axisSettings[axis].params._axisLineVisible );

            // the LineWidth element
            KDXML::createIntNode( doc, axisSettingsElement, "LineWidth",
                                  _axisSettings[axis].params._axisLineWidth );

            // the TrueLineWidth element
            KDXML::createIntNode( doc, axisSettingsElement, "TrueLineWidth",
                                  _axisSettings[axis].params._axisTrueLineWidth );

            // the LineColor element
            KDXML::createColorNode( doc, axisSettingsElement, "LineColor",
                                    _axisSettings[axis].params._axisLineColor );

            // the ShowGrid element
            KDXML::createBoolNode( doc, axisSettingsElement, "ShowGrid",
                                   _axisSettings[axis].params._axisShowGrid );

            // the GridColor element
            KDXML::createColorNode( doc, axisSettingsElement, "GridColor",
                                    _axisSettings[axis].params._axisGridColor );

            // the GridLineWidth element
            KDXML::createIntNode( doc, axisSettingsElement, "GridLineWidth",
                                  _axisSettings[axis].params._axisGridLineWidth );

            // the GridStyle element
            KDXML::createStringNode( doc, axisSettingsElement, "GridStyle",
                                     KDXML::penStyleToString( _axisSettings[axis].params._axisGridStyle ) );

            // the GridSubStyle element
            KDXML::createStringNode( doc, axisSettingsElement, "GridSubStyle",
                                     KDXML::penStyleToString( _axisSettings[axis].params._axisGridSubStyle ) );

            // the ZeroLineColor element
            KDXML::createColorNode( doc, axisSettingsElement, "ZeroLineColor",
                                    _axisSettings[axis].params._axisZeroLineColor );

            // the LabelsVisible element
            KDXML::createBoolNode( doc, axisSettingsElement, "LabelsVisible",
                                   _axisSettings[axis].params._axisLabelsVisible );

            // the LabelsFont element
            createChartFontNode( doc, axisSettingsElement, "LabelsFont",
                                 _axisSettings[axis].params._axisLabelsFont,
                                 _axisSettings[axis].params._axisLabelsFontUseRelSize,
                                 _axisSettings[axis].params._axisLabelsFontRelSize );

            // the LabelsColor element
            KDXML::createColorNode( doc, axisSettingsElement, "LabelsColor",
                                    _axisSettings[axis].params._axisLabelsColor );

            // the SteadyValueCalc element
            KDXML::createBoolNode( doc, axisSettingsElement, "SteadyValueCalc",
                                   _axisSettings[axis].params._axisSteadyValueCalc );

            // the ValueStart element
            if( ! ( KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT == _axisSettings[axis].params._axisValueStart ))
              createChartValueNode( doc, axisSettingsElement, "ValueStart",
                                    _axisSettings[axis].params._axisValueStart );

            // the ValueEnd element
            if( ! ( KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT == _axisSettings[axis].params._axisValueEnd ))
              createChartValueNode( doc, axisSettingsElement, "ValueEnd",
                                    _axisSettings[axis].params._axisValueEnd );

            // the ValueDelta element
            if( ! ( KDChartAxisParams::AXIS_LABELS_AUTO_DELTA == _axisSettings[axis].params._axisValueDelta ))
              KDXML::createDoubleNode( doc, axisSettingsElement, "ValueDelta",
                                       _axisSettings[axis].params._axisValueDelta );

            // the TrueLow element
            KDXML::createDoubleNode( doc, axisSettingsElement, "TrueLow",
                                     _axisSettings[axis].params._trueLow );

            // the TrueHigh element
            KDXML::createDoubleNode( doc, axisSettingsElement, "TrueHigh",
                                     _axisSettings[axis].params._trueHigh );

            // the TrueDelta element
            KDXML::createDoubleNode( doc, axisSettingsElement, "TrueDelta",
                                     _axisSettings[axis].params._trueDelta );

            // the ZeroLineStart element
            QDomElement zeroLineStartElement = doc.createElement( "ZeroLineStart" );
            axisSettingsElement.appendChild( zeroLineStartElement );
            zeroLineStartElement.setAttribute( "X", _axisSettings[axis].params._axisZeroLineStartX );
            zeroLineStartElement.setAttribute( "Y", _axisSettings[axis].params._axisZeroLineStartY );

            // the DigitsBehindComma element
            KDXML::createIntNode( doc, axisSettingsElement, "DigitsBehindComma",
                                  _axisSettings[axis].params._axisDigitsBehindComma );

            // the MaxEmptyInnerSpan element
            KDXML::createIntNode( doc, axisSettingsElement, "MaxEmptyInnerSpan",
                                  _axisSettings[axis].params._axisMaxEmptyInnerSpan );

            // the LabelsFromDataRow element
            KDXML::createStringNode( doc, axisSettingsElement, "LabelsFromDataRow",
                                     KDChartAxisParams::labelsFromDataRowToString( _axisSettings[axis].params._takeLabelsFromDataRow ) );

            // the TextsDataRow element
            KDXML::createIntNode( doc, axisSettingsElement, "TextsDataRow",
                                  _axisSettings[axis].params._labelTextsDataRow );

            // the LabelString elements
            KDXML::createStringListNodes( doc, axisSettingsElement, "LabelString",
                                          _axisSettings[axis].params._axisLabelStringList );

            // the ShortLabelString elements
            KDXML::createStringListNodes( doc, axisSettingsElement, "ShortLabelString",
                                          _axisSettings[axis].params._axisShortLabelsStringList );

            // the LabelText elements
            KDXML::createStringListNodes( doc, axisSettingsElement, "LabelText",
                                          &_axisSettings[axis].params._axisLabelTexts );

            // the LabelTextsDirty element
            KDXML::createBoolNode( doc, axisSettingsElement, "LabelTextsDirty",
                                   _axisSettings[axis].params._axisLabelTextsDirty );
        }
    }

    // the HeaderFooterSettings elements
    for( int hf = 0; hf < 18; hf++ ) {
        QDomElement hfSettingsElement =
            doc.createElement( "HeaderFooterSettings" );
        docRoot.appendChild( hfSettingsElement );
        {
            KDXML::createStringNode( doc, hfSettingsElement, "Text",
                                     _hdFtParams[hf]._text );
            createChartFontNode( doc, hfSettingsElement, "Font",
                                 _hdFtParams[hf]._font,
                                 _hdFtParams[hf]._fontUseRelSize,
                                 _hdFtParams[hf]._fontRelSize );
            KDXML::createColorNode( doc, hfSettingsElement, "Color",
                                    _hdFtParams[hf]._color );
        }
    }


    // the GlobalLeading element
    QDomElement globalLeadingElement =
        doc.createElement( "GlobalLeading" );
    docRoot.appendChild( legendSettingsElement );
    {
	KDXML::createIntNode( doc, globalLeadingElement, "Left",
                              _globalLeadingLeft );
	KDXML::createIntNode( doc, globalLeadingElement, "Top",
                              _globalLeadingTop );
	KDXML::createIntNode( doc, globalLeadingElement, "Right",
                              _globalLeadingRight );
	KDXML::createIntNode( doc, globalLeadingElement, "Bottom",
                              _globalLeadingBottom );
    }

    // the DataValuesSettings1 element
    QDomElement dataValuesSettings1Element =
	doc.createElement( "DataValuesSettings1" );
    docRoot.appendChild( dataValuesSettings1Element );
    {
	KDXML::createBoolNode( doc, dataValuesSettings1Element, "PrintDataValues",
                               _printDataValuesSettings._printDataValues );
	KDXML::createIntNode( doc, dataValuesSettings1Element, "DivPow10",
                              _printDataValuesSettings._divPow10 );
	KDXML::createIntNode( doc, dataValuesSettings1Element, "DigitsBehindComma",
                              _printDataValuesSettings._digitsBehindComma );
	createChartFontNode( doc, dataValuesSettings1Element, "Font",
			     _printDataValuesSettings._dataValuesFont,
			     _printDataValuesSettings._dataValuesUseFontRelSize,
			     _printDataValuesSettings._dataValuesFontRelSize );
	KDXML::createColorNode( doc, dataValuesSettings1Element, "Color",
                                _printDataValuesSettings._dataValuesColor );
        KDXML::createBoolNode( doc, dataValuesSettings1Element, "AutoColor",
                               _printDataValuesSettings._dataValuesAutoColor );
	KDXML::createStringNode( doc, dataValuesSettings1Element,
                                 "AnchorNegativePosition",
                                 KDChartEnums::positionFlagToString( _printDataValuesSettings._dataValuesAnchorNegativePosition ) );
	KDXML::createIntNode( doc, dataValuesSettings1Element,
                              "AnchorNegativeAlign",
                              _printDataValuesSettings._dataValuesAnchorNegativeAlign );
        KDXML::createIntNode( doc, dataValuesSettings1Element,
                              "AnchorNegativeDeltaX",
                              _printDataValuesSettings._dataValuesAnchorNegativeDeltaX );
        KDXML::createIntNode( doc, dataValuesSettings1Element,
                              "AnchorNegativeDeltaY",
                              _printDataValuesSettings._dataValuesAnchorNegativeDeltaY );
	KDXML::createIntNode( doc, dataValuesSettings1Element,
                              "NegativeRotation",
                              _printDataValuesSettings._dataValuesNegativeRotation );
	KDXML::createStringNode( doc, dataValuesSettings1Element,
                                 "AnchorPositivePosition",
                                 KDChartEnums::positionFlagToString( _printDataValuesSettings._dataValuesAnchorPositivePosition ) );
	KDXML::createIntNode( doc, dataValuesSettings1Element,
                              "AnchorPositiveAlign",
                              _printDataValuesSettings._dataValuesAnchorPositiveAlign );
        KDXML::createIntNode( doc, dataValuesSettings1Element,
                              "AnchorPositiveDeltaX",
                              _printDataValuesSettings._dataValuesAnchorPositiveDeltaX );
        KDXML::createIntNode( doc, dataValuesSettings1Element,
                              "AnchorPositiveDeltaY",
                              _printDataValuesSettings._dataValuesAnchorPositiveDeltaY );
	KDXML::createIntNode( doc, dataValuesSettings1Element,
                              "PositiveRotation",
                              _printDataValuesSettings._dataValuesPositiveRotation );

	KDXML::createStringNode( doc, dataValuesSettings1Element,
                                 "LayoutPolicy",
                                 KDChartEnums::layoutPolicyToString( _printDataValuesSettings._dataValuesLayoutPolicy ) );
    }

    // the DataValuesSettings2 element
    QDomElement dataValuesSettings2Element =
	doc.createElement( "DataValuesSettings2" );
    docRoot.appendChild( dataValuesSettings2Element );
    {
	KDXML::createBoolNode( doc, dataValuesSettings2Element, "PrintDataValues",
                               _printDataValuesSettings2._printDataValues );
	KDXML::createIntNode( doc, dataValuesSettings2Element, "DivPow10",
                              _printDataValuesSettings2._divPow10 );
	KDXML::createIntNode( doc, dataValuesSettings2Element, "DigitsBehindComma",
                              _printDataValuesSettings2._digitsBehindComma );
	createChartFontNode( doc, dataValuesSettings2Element, "Font",
			     _printDataValuesSettings2._dataValuesFont,
			     _printDataValuesSettings2._dataValuesUseFontRelSize,
			     _printDataValuesSettings2._dataValuesFontRelSize );
	KDXML::createColorNode( doc, dataValuesSettings2Element, "Color",
                                _printDataValuesSettings2._dataValuesColor );
        KDXML::createBoolNode( doc, dataValuesSettings2Element, "AutoColor",
                               _printDataValuesSettings2._dataValuesAutoColor );
	KDXML::createStringNode( doc, dataValuesSettings2Element,
                                 "AnchorNegativePosition",
                                 KDChartEnums::positionFlagToString( _printDataValuesSettings2._dataValuesAnchorNegativePosition ) );
	KDXML::createIntNode( doc, dataValuesSettings2Element,
                              "AnchorNegativeAlign",
                              _printDataValuesSettings2._dataValuesAnchorNegativeAlign );
        KDXML::createIntNode( doc, dataValuesSettings2Element,
                              "AnchorNegativeDeltaX",
                              _printDataValuesSettings2._dataValuesAnchorNegativeDeltaX );
        KDXML::createIntNode( doc, dataValuesSettings2Element,
                              "AnchorNegativeDeltaY",
                              _printDataValuesSettings2._dataValuesAnchorNegativeDeltaY );
	KDXML::createIntNode( doc, dataValuesSettings2Element,
                              "NegativeRotation",
                              _printDataValuesSettings2._dataValuesNegativeRotation );
	KDXML::createStringNode( doc, dataValuesSettings2Element,
                                 "AnchorPositivePosition",
                                 KDChartEnums::positionFlagToString( _printDataValuesSettings2._dataValuesAnchorPositivePosition ) );
	KDXML::createIntNode( doc, dataValuesSettings2Element,
                              "AnchorPositiveAlign",
                              _printDataValuesSettings2._dataValuesAnchorPositiveAlign );
        KDXML::createIntNode( doc, dataValuesSettings2Element,
                              "AnchorPositiveDeltaX",
                              _printDataValuesSettings2._dataValuesAnchorPositiveDeltaX );
        KDXML::createIntNode( doc, dataValuesSettings2Element,
                              "AnchorPositiveDeltaY",
                              _printDataValuesSettings2._dataValuesAnchorPositiveDeltaY );
	KDXML::createIntNode( doc, dataValuesSettings2Element,
                              "PositiveRotation",
                              _printDataValuesSettings2._dataValuesPositiveRotation );

	KDXML::createStringNode( doc, dataValuesSettings2Element,
                                 "LayoutPolicy",
                                 KDChartEnums::layoutPolicyToString( _printDataValuesSettings2._dataValuesLayoutPolicy ) );
    }

    // the AreaMap element
    QDomElement areaMapElement =
	doc.createElement( "AreaMap" );
    docRoot.appendChild( areaMapElement );
    {
        for( QMap<uint,KDChartFrameSettings>::ConstIterator it = _areaMap.begin();
             it != _areaMap.end(); ++it ) {
	    KDChartFrameSettings::createFrameSettingsNode( doc, areaMapElement,
                                                           "FrameSettings",
                                                           &(it.data()),
                                                           it.key() );
        }
    }

    // the CustomBoxMap element
    QDomElement customBoxMapElement =
	doc.createElement( "CustomBoxMap" );
    docRoot.appendChild( customBoxMapElement );
    {
	for( QMap<uint,KDChartCustomBox>::ConstIterator it = _customBoxMap.begin();
	     it != _customBoxMap.end(); ++it ) {
	    KDXML::createIntNode( doc, customBoxMapElement, "Number", it.key() );
	    KDChartCustomBox::createCustomBoxNode( doc, customBoxMapElement,
                                                   "CustomBox", &(it.data()) );
	}
    }


    return doc;
}


QTextStream& operator>>( QTextStream& s, KDChartParams& p )
{
    QDomDocument doc( "ChartParams" );
    // would be nicer if QDomDocument could read from a QTextStream...
    QString docString = s.read();
    doc.setContent( docString );

    p.loadXML( doc );

    return s;
}


bool KDChartParams::loadXML( const QDomDocument& doc )
{
    int curAxisSettings = 0;
    int curHFSettings = 0;

    QDomElement docRoot = doc.documentElement(); // ChartParams element
    QDomNode node = docRoot.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "ChartType" ) {
                if( element.hasAttribute( "primary" ) )
                    _chartType = KDChartParams::stringToChartType( element.attribute( "primary" ) );
                if( element.hasAttribute( "secondary" ) )
                    _additionalChartType = KDChartParams::stringToChartType( element.attribute( "secondary" ) );
            } else if( tagName == "NumValues" ) {
                int numValues;
                if( KDXML::readIntNode( element, numValues ) )
                    _numValues = numValues;
            } else if( tagName == "ModeAndChartMap" ) {
                int dataset = -1;
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "Dataset" ) {
                            KDXML::readIntNode( element, dataset );
                        } else if( tagName == "ModeAndChart" ) {
                            Q_ASSERT( dataset != -1 ); // there must have been a dataset tag before
                            if( element.hasAttribute( "Mode" ) &&
                                element.hasAttribute( "Chart" ) ) {
                                KDChartParams::SourceMode sourceMode = KDChartParams::stringToChartSourceMode( element.attribute( "Mode" ) );
                                bool ok = false;
                                uint chart = element.attribute( "Chart" ).toUInt( &ok );
                                if( ok )
                                    _dataSourceModeAndChart[dataset] = KDChartParams::ModeAndChart( sourceMode, chart );
                            }
                        } else {
                            qDebug( "Unknown subelement of ModeAndChartMap found: %s", tagName.latin1() );
                            return false;
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "ChartSourceModeWasUsed" ) {
                bool chartSourceModeWasUsed;
                if( KDXML::readBoolNode( element, chartSourceModeWasUsed ) )
                    _setChartSourceModeWasUsed = chartSourceModeWasUsed;
            } else if( tagName == "MaxDatasetSourceMode" ) {
                int maxDatasetSourceMode;
                if( KDXML::readIntNode( element, maxDatasetSourceMode ) )
                    _maxDatasetSourceMode = maxDatasetSourceMode;
            } else if( tagName == "ColorSettings" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "DataColors" ) {
                            QMap<uint,QColor>* map = &_dataColors;
                            readColorMapNode( element, map );
                        } else if( tagName == "MaxDatasetColor" ) {
                            int maxDatasetColor;
                            if( KDXML::readIntNode( element, maxDatasetColor ) )
                                _maxDatasetColor = maxDatasetColor;
                        } else if( tagName == "ShadowBrightnessFactor" ) {
                            double shadowBrightnessFactor;
                            if( KDXML::readDoubleNode( element, shadowBrightnessFactor ) )
                                _shadowBrightnessFactor = shadowBrightnessFactor;
                        } else if( tagName == "ShadowPattern" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _shadowPattern = KDXML::stringToBrushStyle( value );
                        } else if( tagName == "ThreeDShadowColors" ) {
                            bool threeDShadowColors;
                            if( KDXML::readBoolNode( element, threeDShadowColors ) )
                                _threeDShadowColors = threeDShadowColors;
                        } else if( tagName == "DataColorsShadow1" ) {
                            QMap<uint,QColor>* map = &_dataColorsShadow1;
                            readColorMapNode( element, map );
                        } else if( tagName == "DataColorsShadow2" ) {
                            QMap<uint,QColor>* map = &_dataColorsShadow2;
                            readColorMapNode( element, map );
                        } else if( tagName == "OutlineDataColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                _outlineDataColor = color;
                        } else if( tagName == "OutlineDataLineWidth" ) {
                            int outlineDataLineWidth;
                            if( KDXML::readIntNode( element, outlineDataLineWidth ) )
                                _outlineDataLineWidth = outlineDataLineWidth;
                        } else if( tagName == "OutlineDataLineStyle" ) {
                            if( element.hasAttribute( "Style" ) )
                                _outlineDataLineStyle = KDXML::stringToPenStyle( element.attribute( "Style" ) );
                        } else {
                            qDebug( "!!!Unknown subelement of ColorSettings found: %s", tagName.latin1() );
                            return false;
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "BarSettings" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "SubType" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _barChartSubType = KDChartParams::stringToBarChartSubType( value );
                        } else if( tagName == "ThreeDBars" ) {
                            bool threeDBars;
                            if( KDXML::readBoolNode( element, threeDBars ) )
                                _threeDBars = threeDBars;
                        } else if( tagName == "ThreeDBarDepth" ) {
                            double threeDBarDepth;
                            if( KDXML::readDoubleNode( element, threeDBarDepth ) )
                                _threeDBarDepth = threeDBarDepth;
                        } else if( tagName == "DatasetGap" ) {
                            int datasetGap;
                            if( KDXML::readIntNode( element, datasetGap ) )
                                _datasetGap = datasetGap;
                        } else if( tagName == "DatasetGapIsRelative" ) {
                            bool datasetGapIsRelative;
                            if( KDXML::readBoolNode( element, datasetGapIsRelative ) )
                                _datasetGapIsRelative = datasetGapIsRelative;
                        } else if( tagName == "ValueBlockGap" ) {
                            int valueBlockGap;
                            if( KDXML::readIntNode( element, valueBlockGap ) )
                                _valueBlockGap = valueBlockGap;
                        } else if( tagName == "ValueBlockGapIsRelative" ) {
                            bool valueBlockGapIsRelative;
                            if( KDXML::readBoolNode( element, valueBlockGapIsRelative ) )
                                _valueBlockGapIsRelative = valueBlockGapIsRelative;
                        } else {
                            qDebug( "Unknown subelement of BarSettings found: %s", tagName.latin1() );
                            return false;
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "LineSettings" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "SubType" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _lineChartSubType = KDChartParams::stringToLineChartSubType( value );
                        } else if( tagName == "Marker" ) {
                            bool marker;
                            if( KDXML::readBoolNode( element, marker ) )
                                _lineMarker = marker;
                        } else if( tagName == "MarkerStyle" ) {
                            bool ok = true;
                            uint dataset;
                            KDChartParams::LineMarkerStyle style;
                            if( element.hasAttribute( "Dataset" ) &&
                                element.hasAttribute( "Style" ) ) {
                                dataset = element.attribute( "Dataset" ).toUInt( &ok );
                                if( ok )
                                    style = KDChartParams::stringToLineMarkerStyle( element.attribute( "Style" ) );
                            } else
                                ok = false;
                            if( ok )
                                _lineMarkerStyles[dataset] = style;
                        } else if( tagName == "MarkerSize" ) {
                            int width, height;
                            bool ok = true;
                            if( element.hasAttribute( "Width" ) &&
                                element.hasAttribute( "Height" ) ) {
                                width = element.attribute( "Width" ).toInt( &ok );
                                if( ok )
                                    height = element.attribute( "Height" ).toInt( &ok );
                            } else
                                ok = false;

                            if( ok ) {
                                _lineMarkerSize.setWidth( width );
                                _lineMarkerSize.setHeight( height );
                            }
                        } else if( tagName == "LineWidth" ) {
                            int lineWidth;
                            if( KDXML::readIntNode( element, lineWidth ) )
                                _lineWidth = lineWidth;
                        } else if( tagName == "ThreeD" ) {
                            bool threeD;
                            if( KDXML::readBoolNode( element, threeD ) )
                                _threeDLines = threeD;
                        } else if( tagName == "ThreeDDepth" ) {
                            int depth;
                            if( KDXML::readIntNode( element, depth ) )
                                _threeDLineDepth = depth;
                        } else if( tagName == "ThreeDXRotation" ) {
                            int rotation;
                            if( KDXML::readIntNode( element, rotation ) )
                                _threeDLineXRotation = rotation;
                        } else if( tagName == "ThreeDYRotation" ) {
                            int rotation;
                            if( KDXML::readIntNode( element, rotation ) )
                                _threeDLineYRotation = rotation;
                        } else {
                            qDebug( "Unknown subelement of LineSettings found: %s", tagName.latin1() );
                            return false;
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "AreaSettings" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "SubType" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _areaChartSubType = KDChartParams::stringToAreaChartSubType( value );
                        } else if( tagName == "Location" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                _areaLocation = KDChartParams::stringToAreaLocation( string );
                        } else {
                            qDebug( "Unknown subelement of AreaSettings found: %s", tagName.latin1() );
                            return false;
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "PieRingSettings" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "Explode" ) {
                            bool explode;
                            if( KDXML::readBoolNode( element, explode ) )
                                _explode = explode;
                        } else if( tagName == "DefaultExplodeFactor" ) {
                            double defaultExplodeFactor;
                            if( KDXML::readDoubleNode( element, defaultExplodeFactor ) )
                                _explodeFactor = defaultExplodeFactor;
                        } else if( tagName == "ExplodeFactors" ) {
                            QMap<int,double>* map = &_explodeFactors;
                            readDoubleMapNode( element, map );
                        } else if( tagName == "ExplodeSegment" ) {
                            int explodeSegment;
                            if( KDXML::readIntNode( element, explodeSegment ) )
                                _explodeList << explodeSegment;
                        } else if( tagName == "ThreeDPies" ) {
                            bool threeDPies;
                            if( KDXML::readBoolNode( element, threeDPies ) )
                                _threeDPies = threeDPies;
                        } else if( tagName == "ThreeDPieHeight" ) {
                            int threeDPieHeight;
                            if( KDXML::readIntNode( element, threeDPieHeight ) )
                                _threeDPieHeight = threeDPieHeight;
                        } else if( tagName == "PieStart" ) {
                            int pieStart;
                            if( KDXML::readIntNode( element, pieStart ) )
                                _pieStart = pieStart;
                        } else if( tagName == "RingStart" ) {
                            int ringStart;
                            if( KDXML::readIntNode( element, ringStart ) )
                                _ringStart = ringStart;
                        } else if( tagName == "RelativeRingThickness" ) {
                            bool relativeRingThickness;
                            if( KDXML::readBoolNode( element, relativeRingThickness ) )
                                _relativeRingThickness = relativeRingThickness;
                        } else {
                            qDebug( "Unknown subelement of PieRingSettings found: %s", tagName.latin1() );
                            return false;
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "HiLoSettings" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "SubType" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _hiLoChartSubType = KDChartParams::stringToHiLoChartSubType( value );
                        } else if( tagName == "PrintLowValues" ) {
                            bool printLowValues;
                            if( KDXML::readBoolNode( element, printLowValues ) )
                                _hiLoChartPrintLowValues = printLowValues;
                        } else if( tagName == "LowValuesFont" ) {
                            readChartFontNode( element,
                                               _hiLoChartLowValuesFont,
                                               _hiLoChartLowValuesUseFontRelSize,
                                               _hiLoChartLowValuesFontRelSize );
                        } else if( tagName == "LowValuesColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                _hiLoChartLowValuesColor = color;
                        } else if( tagName == "PrintHighValues" ) {
                            bool printHighValues;
                            if( KDXML::readBoolNode( element, printHighValues ) )
                                _hiLoChartPrintHighValues = printHighValues;
                        } else if( tagName == "HighValuesFont" ) {
                            readChartFontNode( element,
                                               _hiLoChartHighValuesFont,
                                               _hiLoChartHighValuesUseFontRelSize,
                                               _hiLoChartHighValuesFontRelSize );
                        } else if( tagName == "HighValuesColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                _hiLoChartHighValuesColor = color;
                        } else if( tagName == "PrintOpenValues" ) {
                            bool printOpenValues;
                            if( KDXML::readBoolNode( element, printOpenValues ) )
                                _hiLoChartPrintOpenValues = printOpenValues;
                        } else if( tagName == "OpenValuesFont" ) {
                            readChartFontNode( element,
                                               _hiLoChartOpenValuesFont,
                                               _hiLoChartOpenValuesUseFontRelSize,
                                               _hiLoChartOpenValuesFontRelSize );
                        } else if( tagName == "OpenValuesColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                _hiLoChartOpenValuesColor = color;
                        } else if( tagName == "PrintCloseValues" ) {
                            bool printCloseValues;
                            if( KDXML::readBoolNode( element, printCloseValues ) )
                                _hiLoChartPrintCloseValues = printCloseValues;
                        } else if( tagName == "CloseValuesFont" ) {
                            readChartFontNode( element,
                                               _hiLoChartCloseValuesFont,
                                               _hiLoChartCloseValuesUseFontRelSize,
                                               _hiLoChartCloseValuesFontRelSize );
                        } else if( tagName == "CloseValuesColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                _hiLoChartCloseValuesColor = color;
                        } else {
                            qDebug( "Unknown subelement of HiLoSettings found: %s", tagName.latin1() );
                            return false;
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "GanttSettings" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "SubType" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _ganttChartSubType = KDChartParams::stringToGanttChartSubType( value );
                        } else if( tagName == "TemporalResolution" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                _ganttChartTemporalResolution = KDChartParams::stringToGanttChartTemporalResolution( string );
                        } else if( tagName == "PrintStartValues" ) {
                            bool printStartValues;
                            if( KDXML::readBoolNode( element, printStartValues ) )
                                _ganttChartPrintStartValues = printStartValues;
                        } else if( tagName == "StartValuesPos" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                _ganttChartStartValuesPos = KDChartEnums::stringToPositionFlag( string );
                        } else if( tagName == "StartValuesAlign" ) {
                            int startValuesAlign;
                            if( KDXML::readIntNode( element, startValuesAlign ) )
                                _ganttChartStartValuesAlign = static_cast < uint > ( startValuesAlign );
                        } else if( tagName == "StartValuesDateTimeFormat" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                _ganttChartStartValuesDateTimeFormat = string;
                        } else if( tagName == "StartValuesFont" ) {
                            readChartFontNode( element,
                                               _ganttChartStartValuesFont,
                                               _ganttChartStartValuesUseFontRelSize,
                                               _ganttChartStartValuesFontRelSize );
                        } else if( tagName == "StartValuesColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                _ganttChartStartValuesColor = color;
                        } else if( tagName == "PrintEndValues" ) {
                            bool printEndValues;
                            if( KDXML::readBoolNode( element, printEndValues ) )
                                _ganttChartPrintEndValues = printEndValues;
                        } else if( tagName == "EndValuesPos" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                _ganttChartEndValuesPos = KDChartEnums::stringToPositionFlag( string );
                        } else if( tagName == "EndValuesAlign" ) {
                            int endValuesAlign;
                            if( KDXML::readIntNode( element, endValuesAlign ) )
                                _ganttChartEndValuesAlign = static_cast < uint > ( endValuesAlign );
                        } else if( tagName == "EndValuesDateTimeFormat" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                _ganttChartEndValuesDateTimeFormat = string;
                        } else if( tagName == "EndValuesFont" ) {
                            readChartFontNode( element,
                                               _ganttChartEndValuesFont,
                                               _ganttChartEndValuesUseFontRelSize,
                                               _ganttChartEndValuesFontRelSize );
                        } else if( tagName == "EndValuesColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                _ganttChartEndValuesColor = color;
                        } else if( tagName == "PrintDurations" ) {
                            bool printDurations;
                            if( KDXML::readBoolNode( element, printDurations ) )
                                _ganttChartPrintDurations = printDurations;
                        } else if( tagName == "DurationsPos" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                _ganttChartDurationsPos = KDChartEnums::stringToPositionFlag( string );
                        } else if( tagName == "DurationsAlign" ) {
                            int durationsAlign;
                            if( KDXML::readIntNode( element, durationsAlign ) )
                                _ganttChartDurationsAlign = static_cast < uint > ( durationsAlign );
                        } else if( tagName == "DurationsDateTimeFormat" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                _ganttChartDurationsDateTimeFormat = string;
                        } else if( tagName == "DurationsFont" ) {
                            readChartFontNode( element,
                                               _ganttChartDurationsFont,
                                               _ganttChartDurationsUseFontRelSize,
                                               _ganttChartDurationsFontRelSize );
                        } else if( tagName == "DurationsColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                _ganttChartDurationsColor = color;
                        } else {
                            qDebug( "Unknown subelement of GanttSettings found: %s", tagName.latin1() );
                            return false;
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "PolarSettings" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "SubType" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _polarChartSubType = KDChartParams::stringToPolarChartSubType( value );
                        } else if( tagName == "Marker" ) {
                            bool marker;
                            if( KDXML::readBoolNode( element, marker ) )
                                _polarMarker = marker;
                        } else if( tagName == "MarkerStyle" ) {
                            bool ok = true;
                            uint dataset;
                            KDChartParams::PolarMarkerStyle style;
                            if( element.hasAttribute( "Dataset" ) &&
                                element.hasAttribute( "Style" ) ) {
                                dataset = element.attribute( "Dataset" ).toUInt( &ok );
                                if( ok )
                                    style = KDChartParams::stringToPolarMarkerStyle( element.attribute( "Style" ) );
                            } else
                                ok = false;
                            if( ok )
                                _polarMarkerStyles[dataset] = style;
                        } else if( tagName == "MarkerSize" ) {
                            int width, height;
                            bool ok = true;
                            if( element.hasAttribute( "Width" ) &&
                                element.hasAttribute( "Height" ) ) {
                                width = element.attribute( "Width" ).toInt( &ok );
                                if( ok )
                                    height = element.attribute( "Height" ).toInt( &ok );
                            } else
                                ok = false;

                            if( ok ) {
                                _polarMarkerSize.setWidth( width );
                                _polarMarkerSize.setHeight( height );
                            }
                        } else if( tagName == "PolarLineWidth" ) {
                            int polarLineWidth;
                            if( KDXML::readIntNode( element, polarLineWidth ) )
                                _polarLineWidth = polarLineWidth;
                        } else {
                            qDebug( "Unknown subelement of PolarSettings found: %s", tagName.latin1() );
                            return false;
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "LegendSettings" ) {
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "Position" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                _legendPosition = KDChartParams::stringToLegendPosition( string );
                        } else if( tagName == "Source" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                _legendSource = KDChartParams::stringToLegendSource( string );
                        } else if( tagName == "LegendText" ) {
                            bool ok = true;
                            uint dataset;
                            QString text;
                            if( element.hasAttribute( "Dataset" ) &&
                                element.hasAttribute( "Text" ) ) {
                                dataset = element.attribute( "Dataset" ).toUInt( &ok );
                                if( ok )
                                    text = element.attribute( "Text" );
                            } else
                                ok = false;
                            if( ok )
                                _legendText[dataset] = text;
                        } else if( tagName == "TextColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                _legendTextColor = color;
                        } else if( tagName == "TextFont" ) {
                            readChartFontNode( element,
                                               _legendFont,
                                               _legendFontUseRelSize,
                                               _legendFontRelSize );
                        } else if( tagName == "TitleText" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                _legendTitleText = string;
                        } else if( tagName == "TitleColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                _legendTitleTextColor = color;
                        } else if( tagName == "TitleFont" ) {
                            readChartFontNode( element,
                                               _legendTitleFont,
                                               _legendTitleFontUseRelSize,
                                               _legendTitleFontRelSize );
                        } else if( tagName == "Spacing" ) {
                            int spacing;
                            if( KDXML::readIntNode( element, spacing ) )
                                _legendSpacing = spacing;
                        } else {
                            qDebug( "Unknown subelement of LegendSettings found: %s", tagName.latin1() );
                            return false;
                        }
                    }
                    node = node.nextSibling();
                }
            } else if( tagName == "AxisSettings" ) {
                KDChartAxisParams* axisSettings =
                    &( _axisSettings[ curAxisSettings ].params );
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "Type" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                axisSettings->_axisType = KDChartAxisParams::stringToAxisType( string );
                        } else if( tagName == "Visible" ) {
                            bool visible;
                            if( KDXML::readBoolNode( element, visible ) )
                                axisSettings->_axisVisible = visible;
                        } else if( tagName == "LabelsTouchEdges" ) {
                            bool labelsTouchEdges;
                            if( KDXML::readBoolNode( element, labelsTouchEdges ) )
                                axisSettings->_axisLabelsTouchEdges = labelsTouchEdges;
                        } else if( tagName == "AreaMode" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                axisSettings->_axisAreaMode = KDChartAxisParams::stringToAxisAreaMode( string );
                        } else if( tagName == "AreaMin" ) {
                            int areaMin;
                            if( KDXML::readIntNode( element, areaMin ) )
                                axisSettings->_axisAreaMin = areaMin;
                        } else if( tagName == "AreaMax" ) {
                            int areaMax;
                            if( KDXML::readIntNode( element, areaMax ) )
                                axisSettings->_axisAreaMax = areaMax;
                        } else if( tagName == "CalcMode" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                axisSettings->_axisCalcMode = KDChartAxisParams::stringToAxisCalcMode( string );
                        } else if( tagName == "TrueAreaSize" ) {
                            int trueAreaSize;
                            if( KDXML::readIntNode( element, trueAreaSize ) )
                                axisSettings->_axisTrueAreaSize = trueAreaSize;
                        } else if( tagName == "TrueAreaRect" ) {
                            QRect trueAreaRect;
                            if( KDXML::readRectNode( element, trueAreaRect ) )
                                axisSettings->_axisTrueAreaRect = trueAreaRect;
                        } else if( tagName == "ShowSubDelimiters" ) {
                            bool showSubDelimiters;
                            if( KDXML::readBoolNode( element, showSubDelimiters ) )
                                axisSettings->_axisShowSubDelimiters = showSubDelimiters;
                        } else if( tagName == "LineVisible" ) {
                            bool lineVisible;
                            if( KDXML::readBoolNode( element, lineVisible ) )
                                axisSettings->_axisLineVisible = lineVisible;
                        } else if( tagName == "LineWidth" ) {
                            int lineWidth;
                            if( KDXML::readIntNode( element, lineWidth ) )
                                axisSettings->_axisLineWidth = lineWidth;
                        } else if( tagName == "TrueLineWidth" ) {
                            int trueLineWidth;
                            if( KDXML::readIntNode( element, trueLineWidth ) )
                                axisSettings->_axisTrueLineWidth = trueLineWidth;
                        } else if( tagName == "LineColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                axisSettings->_axisLineColor = color;
                        } else if( tagName == "ShowGrid" ) {
                            bool showGrid;
                            if( KDXML::readBoolNode( element, showGrid ) )
                                axisSettings->_axisShowGrid = showGrid;
                        } else if( tagName == "GridColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                axisSettings->_axisGridColor = color;
                        } else if( tagName == "GridLineWidth" ) {
                            int gridLineWidth;
                            if( KDXML::readIntNode( element, gridLineWidth ) )
                                axisSettings->_axisGridLineWidth = gridLineWidth;
                        } else if( tagName == "GridStyle" ) {
                            if( element.hasAttribute( "Style" ) )
                                axisSettings->_axisGridStyle = KDXML::stringToPenStyle( element.attribute( "Style" ) );
                        } else if( tagName == "GridSubStyle" ) {
                            if( element.hasAttribute( "Style" ) )
                                axisSettings->_axisGridSubStyle = KDXML::stringToPenStyle( element.attribute( "Style" ) );
                        } else if( tagName == "ZeroLineColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                axisSettings->_axisZeroLineColor = color;
                        } else if( tagName == "LabelsVisible" ) {
                            bool labelsVisible;
                            if( KDXML::readBoolNode( element, labelsVisible ) )
                                axisSettings->_axisLabelsVisible = labelsVisible;
                        } else if( tagName == "LabelsFont" ) {
                            readChartFontNode( element,
                                               axisSettings->_axisLabelsFont,
                                               axisSettings->_axisLabelsFontUseRelSize,
                                               axisSettings->_axisLabelsFontRelSize );
                        } else if( tagName == "LabelsColor" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                axisSettings->_axisLabelsColor = color;
                        } else if( tagName == "SteadyValueCalc" ) {
                            bool steadyValueCalc;
                            if( KDXML::readBoolNode( element, steadyValueCalc ) )
                                axisSettings->_axisSteadyValueCalc = steadyValueCalc;
                        } else if( tagName == "ValueStart" ) {
                            KDChartData value;
                            if( readChartValueNode( element, value ) )
                                axisSettings->_axisValueStart = value;
                        } else if( tagName == "ValueEnd" ) {
                            KDChartData value;
                            if( readChartValueNode( element, value ) )
                                axisSettings->_axisValueEnd = value;
                        } else if( tagName == "ValueDelta" ) {
                            double valueDelta;
                            if( KDXML::readDoubleNode( element, valueDelta ) )
                                axisSettings->_axisValueDelta = valueDelta;
                        } else if( tagName == "TrueLow" ) {
                            double trueLow;
                            if( KDXML::readDoubleNode( element, trueLow ) )
                                axisSettings->_trueLow  = trueLow;
                        } else if( tagName == "TrueHigh" ) {
                            double trueHigh;
                            if( KDXML::readDoubleNode( element, trueHigh ) )
                                axisSettings->_trueHigh  = trueHigh;
                        } else if( tagName == "TrueDelta" ) {
                            double trueDelta;
                            if( KDXML::readDoubleNode( element, trueDelta ) )
                                axisSettings->_trueDelta  = trueDelta;
                        } else if( tagName == "ZeroLineStart" ) {
                            double x, y;
                            bool ok = true;
                            if( element.hasAttribute( "X" ) &&
                                element.hasAttribute( "Y" ) ) {
                                x = element.attribute( "X" ).toDouble( &ok );
                                if( ok )
                                    y = element.attribute( "Y" ).toDouble( &ok );
                            } else
                                ok = false;

                            if( ok ) {
                                axisSettings->_axisZeroLineStartX = x;
                                axisSettings->_axisZeroLineStartY = y;
                            }
                        } else if( tagName == "DigitsBehindComma" ) {
                            int digitsBehindComma;
                            if( KDXML::readIntNode( element, digitsBehindComma ) )
                                axisSettings->_axisDigitsBehindComma = digitsBehindComma;
                        } else if( tagName == "MaxEmptyInnerSpan" ) {
                            int maxEmptyInnerSpan;
                            if( KDXML::readIntNode( element, maxEmptyInnerSpan ) )
                                axisSettings->_axisMaxEmptyInnerSpan = maxEmptyInnerSpan;
                        } else if( tagName == "LabelsFromDataRow" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                axisSettings->_labelTextsDataRow = KDChartAxisParams::stringToLabelsFromDataRow( string );
                        } else if( tagName == "TextsDataRow" ) {
                            int textsDataRow;
                            if( KDXML::readIntNode( element, textsDataRow ) )
                                axisSettings->_labelTextsDataRow = textsDataRow;
                        } else if( tagName == "LabelString" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) ) {
                                if( !axisSettings->_axisLabelStringList )
                                    axisSettings->_axisLabelStringList = new QStringList();
                                *(axisSettings->_axisLabelStringList) << string;
                            }
                        } else if( tagName == "ShortLabelString" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) ) {
                                if( !axisSettings->_axisShortLabelsStringList )
                                    axisSettings->_axisShortLabelsStringList = new QStringList();
                                *(axisSettings->_axisShortLabelsStringList) << string;
                            }
                        } else if( tagName == "LabelText" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                axisSettings->_axisLabelTexts = string;
                        } else if( tagName == "LabelTextsDirty" ) {
                            bool labelTextsDirty;
                            if( KDXML::readBoolNode( element, labelTextsDirty ) )
                                axisSettings->_axisLabelTextsDirty = labelTextsDirty;
                        } else {
                            qDebug( "Unknown subelement of AxisSettings found: %s", tagName.latin1() );
                            return false;
                        }
                    }
                    node = node.nextSibling();
                }
                curAxisSettings++; // one axis settings entry finished
            } else if( tagName == "HeaderFooterSettings" ) {
                KDChartParams::HdFtParams* hfSettings =
                    &( _hdFtParams[ curHFSettings ] );
                QDomNode node = element.firstChild();
                while( !node.isNull() ) {
                    QDomElement element = node.toElement();
                    if( !element.isNull() ) { // was really an element
                        QString tagName = element.tagName();
                        if( tagName == "Text" ) {
                            QString string;
                            if( KDXML::readStringNode( element, string ) )
                                hfSettings->_text = string;
                        } else if( tagName == "Font" ) {
                            readChartFontNode( element,
                                               hfSettings->_font,
                                               hfSettings->_fontUseRelSize,
                                               hfSettings->_fontRelSize );
                        } else if( tagName == "Color" ) {
                            QColor color;
                            if( KDXML::readColorNode( element, color ) )
                                hfSettings->_color = color;
                        } else {
                            qDebug( "Unknown subelement of HeaderFooterSettings found: %s", tagName.latin1() );
                        }
                    }
                    node = node.nextSibling();
                }
                curHFSettings++; // one header/footer setting finished
            } else if( tagName == "GlobalLeading" ) {
		QDomNode node = element.firstChild();
		while( !node.isNull() ) {
		    QDomElement element = node.toElement();
		    if( !element.isNull() ) { // was really an elemente
			QString tagName = element.tagName();
			if( tagName == "Left" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_globalLeadingLeft = value;
			}
			else if( tagName == "Top" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_globalLeadingTop = value;
			}
			else if( tagName == "Right" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_globalLeadingRight = value;
			}
			else if( tagName == "Bottom" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_globalLeadingBottom = value;
			} else {
			    qDebug( "Unknown subelement of GlobalLeading found: %s", tagName.latin1() );
			    return false;
			}
		    }
		    node = node.nextSibling();
		}
	    } else if( tagName == "DataValuesSettings1" ) {
		QDomNode node = element.firstChild();
		while( !node.isNull() ) {
		    QDomElement element = node.toElement();
		    if( !element.isNull() ) { // was really an element
			QString tagName = element.tagName();
			if( tagName == "PrintDataValues" ) {
			    bool value;
			    if( KDXML::readBoolNode( element, value ) )
				_printDataValuesSettings._printDataValues = value;
			} else if( tagName == "DivPow10" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings._divPow10 = value;
			} else if( tagName == "DigitsBehindComma" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings._digitsBehindComma = value;
			} else if( tagName == "Font" ) {
			    readChartFontNode( element,
					       _printDataValuesSettings._dataValuesFont,
					       _printDataValuesSettings._dataValuesUseFontRelSize,
					       _printDataValuesSettings._dataValuesFontRelSize );
			} else if( tagName == "Color" ) {
			    KDXML::readColorNode( element, _printDataValuesSettings._dataValuesColor );
                        } else if( tagName == "AutoColor" ) {
                            KDXML::readBoolNode( element,
                                                 _printDataValuesSettings._dataValuesAutoColor );
			} else if( tagName == "AnchorNegativePosition" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _printDataValuesSettings._dataValuesAnchorNegativePosition = KDChartEnums::stringToPositionFlag( value );
			} else if( tagName == "AnchorNegativeAlign" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings._dataValuesAnchorNegativeAlign = value;
			} else if( tagName == "AnchorNegativeDeltaX" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings._dataValuesAnchorNegativeDeltaX = value;
			} else if( tagName == "AnchorNegativeDeltaY" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings._dataValuesAnchorNegativeDeltaY = value;
			} else if( tagName == "NegativeRotation" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings._dataValuesNegativeRotation = value;
			} else if( tagName == "AnchorPositivePosition" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _printDataValuesSettings._dataValuesAnchorPositivePosition = KDChartEnums::stringToPositionFlag( value );
			} else if( tagName == "AnchorPositiveAlign" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings._dataValuesAnchorPositiveAlign = value;
			} else if( tagName == "AnchorPositiveDeltaX" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings._dataValuesAnchorPositiveDeltaX = value;
			} else if( tagName == "AnchorPositiveDeltaY" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings._dataValuesAnchorPositiveDeltaY = value;
			} else if( tagName == "PositiveRotation" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings._dataValuesPositiveRotation = value;
			} else if( tagName == "LayoutPolicy" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _printDataValuesSettings._dataValuesLayoutPolicy = KDChartEnums::stringToLayoutPolicy( value );
			} else {
			    qDebug( "Unknown subelement of DataValuesSettings1 found: %s", tagName.latin1() );
			    return false;
			}
		    }
		    node = node.nextSibling();
		}
	    } else if( tagName == "DataValuesSettings2" ) {
		QDomNode node = element.firstChild();
		while( !node.isNull() ) {
		    QDomElement element = node.toElement();
		    if( !element.isNull() ) { // was really an element
			QString tagName = element.tagName();
			if( tagName == "PrintDataValues" ) {
			    bool value;
			    if( KDXML::readBoolNode( element, value ) )
				_printDataValuesSettings2._printDataValues = value;
			} else if( tagName == "DivPow10" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings2._divPow10 = value;
			} else if( tagName == "DigitsBehindComma" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings2._digitsBehindComma = value;
			} else if( tagName == "Font" ) {
			    readChartFontNode( element,
					       _printDataValuesSettings2._dataValuesFont,
					       _printDataValuesSettings2._dataValuesUseFontRelSize,
					       _printDataValuesSettings2._dataValuesFontRelSize );
			} else if( tagName == "Color" ) {
			    KDXML::readColorNode( element, _printDataValuesSettings2._dataValuesColor );
                        } else if( tagName == "AutoColor" ) {
                            KDXML::readBoolNode( element,
                                                 _printDataValuesSettings2._dataValuesAutoColor );
			} else if( tagName == "AnchorNegativePosition" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _printDataValuesSettings2._dataValuesAnchorNegativePosition = KDChartEnums::stringToPositionFlag( value );
			} else if( tagName == "AnchorNegativeAlign" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings2._dataValuesAnchorNegativeAlign = value;
			} else if( tagName == "AnchorNegativeDeltaX" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings2._dataValuesAnchorNegativeDeltaX = value;
			} else if( tagName == "AnchorNegativeDeltaY" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings2._dataValuesAnchorNegativeDeltaY = value;
			} else if( tagName == "NegativeRotation" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings2._dataValuesNegativeRotation = value;
			} else if( tagName == "AnchorPositivePosition" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _printDataValuesSettings2._dataValuesAnchorPositivePosition = KDChartEnums::stringToPositionFlag( value );
			} else if( tagName == "AnchorPositiveAlign" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings2._dataValuesAnchorPositiveAlign = value;
			} else if( tagName == "AnchorPositiveDeltaX" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings2._dataValuesAnchorPositiveDeltaX = value;
			} else if( tagName == "AnchorPositiveDeltaY" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings2._dataValuesAnchorPositiveDeltaY = value;
			} else if( tagName == "PositiveRotation" ) {
			    int value;
			    if( KDXML::readIntNode( element, value ) )
				_printDataValuesSettings2._dataValuesPositiveRotation = value;
			} else if( tagName == "LayoutPolicy" ) {
                            QString value;
                            if( KDXML::readStringNode( element, value ) )
                                _printDataValuesSettings2._dataValuesLayoutPolicy = KDChartEnums::stringToLayoutPolicy( value );
			} else {
			    qDebug( "Unknown subelement of DataValuesSettings2 found: %s", tagName.latin1() );
			    return false;
			}
		    }
		    node = node.nextSibling();
		}
	    } else if( tagName == "AreaMap" ) {
		QDomNode node = element.firstChild();
		while( !node.isNull() ) {
		    QDomElement element = node.toElement();
		    if( !element.isNull() ) { // was really an element
			QString tagName = element.tagName();
			if( tagName == "FrameSettings" ) {
	                    KDChartFrameSettings frameSettings;
                            uint areaId = KDChartEnums::AreaUNKNOWN;
			    if( KDChartFrameSettings::readFrameSettingsNode(
                                element, frameSettings, areaId ) ) {
                                _areaMap.insert( areaId, frameSettings );
                            }
			}
			else
			    qDebug( "Unknown tag in area map" );
                            // do _not_ return false here (to enable future extentions)
		    }
		    node = node.nextSibling();
                }
	    } else if( tagName == "CustomBoxMap" ) {
		QDomNode node = element.firstChild();
		int curNumber = -1;
		while( !node.isNull() ) {
		    QDomElement element = node.toElement();
		    if( !element.isNull() ) { // was really an element
			QString tagName = element.tagName();
			if( tagName == "Number" ) {
			    KDXML::readIntNode( element, curNumber );
			} else if( tagName == "FrameSettings" ) {
			    Q_ASSERT( curNumber != -1 ); // there was a Dataset tag before
			    KDChartCustomBox customBox;
			    KDChartCustomBox::readCustomBoxNode( element,
                                                                 customBox );
			    _customBoxMap.insert( curNumber, customBox );
			} else {
			    qDebug( "Unknown tag in area map" );
			    return false;
			}
		    }
		    node = node.nextSibling();
		}
	    } else {
		qDebug( "Unknown second-level element found: %s", tagName.latin1() );
		// NOTE: We do *not* 'return false' here but continue normal operation
		//       since additional elements might have been added in future versions
	    }
	}
	node = node.nextSibling();
    }
    return true;
}


void KDChartParams::createColorMapNode( QDomDocument& doc, QDomNode& parent,
                                        const QString& elementName,
                                        const QMap< uint, QColor >& map )
{
    QDomElement mapElement =
        doc.createElement( elementName );
    parent.appendChild( mapElement );
    for( QMap<uint,QColor>::ConstIterator it = map.begin();
         it != map.end(); ++it ) {
        // Dataset element
        QDomElement datasetElement = doc.createElement( "Dataset" );
        mapElement.appendChild( datasetElement );
        QDomText datasetContent =
            doc.createTextNode( QString::number( it.key() ) );
        datasetElement.appendChild( datasetContent );
        // Color element
        KDXML::createColorNode( doc, mapElement, "Color", it.data() );
    }
}


void KDChartParams::createDoubleMapNode( QDomDocument& doc, QDomNode& parent,
                                         const QString& elementName,
                                         const QMap< int, double >& map )
{
    QDomElement mapElement =
        doc.createElement( elementName );
    parent.appendChild( mapElement );
    for( QMap<int,double>::ConstIterator it = map.begin();
         it != map.end(); ++it ) {
        // Dataset element
        QDomElement valueElement = doc.createElement( "Value" );
        mapElement.appendChild( valueElement );
        QDomText valueContent =
            doc.createTextNode( QString::number( it.key() ) );
        valueElement.appendChild( valueContent );
        // Color element
        KDXML::createDoubleNode( doc, mapElement, "Factor", it.data() );
    }
}



void KDChartParams::createChartValueNode( QDomDocument& doc, QDomNode& parent,
                                          const QString& elementName,
                                          const KDChartData& data )
{
    QDomElement element = doc.createElement( elementName );
    parent.appendChild( element );
    if( data.isDouble() )
        element.setAttribute( "DoubleValue",
                              QString::number( data.doubleValue() ) );
    else if( data.isString() )
        element.setAttribute( "StringValue", data.stringValue() );
    else
        element.setAttribute( "NoValue", "true" );
}


void KDChartParams::createChartFontNode( QDomDocument& doc, QDomNode& parent,
                                         const QString& elementName,
                                         const QFont& font, bool useRelFont,
                                         int relFont )
{
    QDomElement chartFontElement = doc.createElement( elementName );
    parent.appendChild( chartFontElement );
    KDXML::createFontNode( doc, chartFontElement, "Font", font );
    KDXML::createBoolNode( doc, chartFontElement, "UseRelFontSize",
                           useRelFont );
    KDXML::createIntNode( doc, chartFontElement, "RelFontSize", relFont );
}


void KDChartParams::KDChartFrameSettings::createFrameSettingsNode( QDomDocument& document,
                                                                   QDomNode& parent,
                                                                   const QString& elementName,
                                                                   const KDChartParams::KDChartFrameSettings* settings,
                                                                   uint areaId )
{
    QDomElement frameSettingsElement = document.createElement( elementName );
    parent.appendChild( frameSettingsElement );
    KDFrame::createFrameNode( document, frameSettingsElement, "Frame",
			      settings->_frame );
    KDXML::createIntNode( document, frameSettingsElement, "AreaId",
                          areaId );
    KDXML::createIntNode( document, frameSettingsElement, "OuterGapX",
                          settings->_outerGapX );
    KDXML::createIntNode( document, frameSettingsElement, "OuterGapY",
                          settings->_outerGapY );
    KDXML::createIntNode( document, frameSettingsElement, "InnerGapX",
                          settings->_innerGapX );
    KDXML::createIntNode( document, frameSettingsElement, "InnerGapY",
                          settings->_innerGapY );
    KDXML::createBoolNode( document, frameSettingsElement,
                           "AddFrameWidthToLayout",
                           settings->_addFrameWidthToLayout );
    KDXML::createBoolNode( document, frameSettingsElement,
                           "AddFrameHeightToLayout",
                           settings->_addFrameHeightToLayout );
}





bool KDChartParams::readColorMapNode( const QDomElement& element,
                                      QMap<uint,QColor>* value )
{
    QDomNode node = element.firstChild();
    int curDataset = -1;
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "Dataset" ) {
                KDXML::readIntNode( element, curDataset );
            } else if( tagName == "Color" ) {
                Q_ASSERT( curDataset != -1 ); // there was a Dataset tag before
                QColor color;
                KDXML::readColorNode( element, color );
                value->insert( curDataset, color );
            } else {
                qDebug( "Unknown tag in color map" );
                return false;
            }
        }
        node = node.nextSibling();
    }

    return true;
}


bool KDChartParams::readDoubleMapNode( const QDomElement& element,
                                       QMap<int,double>* value )
{
    QDomNode node = element.firstChild();
    int curValue = -1;
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "Value" ) {
                KDXML::readIntNode( element, curValue );
            } else if( tagName == "Factor" ) {
                Q_ASSERT( curValue != -1 ); // there was a Value tag before
                double doubleValue;
                KDXML::readDoubleNode( element, doubleValue );
                value->insert( curValue, doubleValue );
            } else {
                qDebug( "Unknown tag in double map" );
                return false;
            }
        }
        node = node.nextSibling();
    }

    return true;
}



bool KDChartParams::readChartFontNode( const QDomElement& element,
                                       QFont& font,
                                       bool& useRelFont,
                                       int& relFontSize )
{
    bool ok = true;
    QFont tempFont;
    bool tempRelFont;
    int tempRelFontSize;
    QDomNode node = element.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "Font" ) {
                ok = ok & KDXML::readFontNode( element, tempFont );
            } else if( tagName == "UseRelFontSize" ) {
                ok = ok & KDXML::readBoolNode( element, tempRelFont );
            } else if( tagName == "RelFontSize" ) {
                ok = ok & KDXML::readIntNode( element, tempRelFontSize );
            } else {
                qDebug( "Unknown tag in color map" );
            }
        }
        node = node.nextSibling();
    }

    if( ok ) {
        font = tempFont;
        useRelFont = tempRelFont;
        relFontSize = tempRelFontSize;
    }

    return ok;
}



// PENDING(kalle) Support DateTime values, even when writing.
bool KDChartParams::readChartValueNode( const QDomElement& element,
                                        KDChartData& value )
{
    bool ok = true;
    if( element.hasAttribute( "NoValue" ) )
        value = KDChartData();
    else if( element.hasAttribute( "DoubleValue" ) ) {
        double d = element.attribute( "DoubleValue" ).toDouble( &ok );
        if( ok )
            value = KDChartData( d );
    } else if( element.hasAttribute( "StringValue" ) )
        value = KDChartData( element.attribute( "StringValue" ) );
    else // should not happen
        ok = false;

    return ok;
}


bool KDChartParams::KDChartFrameSettings::readFrameSettingsNode( const QDomElement& element,
                                                                 KDChartFrameSettings& settings,
                                                                 uint& areaId )
{
    bool ok = true;
    KDFrame tempFrame;
    int tempId = KDChartEnums::AreaUNKNOWN;
    int tempOuterGapX, tempOuterGapY, tempInnerGapX, tempInnerGapY;
    bool tempAddFrameWidthToLayout, tempAddFrameHeightToLayout;
    QDomNode node = element.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "Frame" ) {
                ok = ok & KDFrame::readFrameNode( element, tempFrame );
            } else if( tagName == "AreaId" ) {
                ok = ok & KDXML::readIntNode( element, tempId );
            } else if( tagName == "OuterGapX" ) {
                ok = ok & KDXML::readIntNode( element, tempOuterGapX );
            } else if( tagName == "OuterGapY" ) {
                ok = ok & KDXML::readIntNode( element, tempOuterGapY );
            } else if( tagName == "InnerGapX" ) {
                ok = ok & KDXML::readIntNode( element, tempInnerGapX );
            } else if( tagName == "InnerGapY" ) {
                ok = ok & KDXML::readIntNode( element, tempInnerGapY );
            } else if( tagName == "AddFrameWidthToLayout" ) {
                ok = ok & KDXML::readBoolNode( element, tempAddFrameWidthToLayout );
            } else if( tagName == "AddFrameHeightToLayout" ) {
                ok = ok & KDXML::readBoolNode( element, tempAddFrameHeightToLayout );
            } else {
                qDebug( "Unknown tag in frame settings" );
            }
        }
        node = node.nextSibling();
    }

    if( ok ) {
	settings._frame = tempFrame;
	settings._outerGapX = tempOuterGapX;
	settings._outerGapY = tempOuterGapY;
	settings._innerGapX = tempInnerGapX;
	settings._innerGapY = tempInnerGapY;
	settings._addFrameWidthToLayout = tempAddFrameWidthToLayout;
	settings._addFrameHeightToLayout = tempAddFrameHeightToLayout;
        areaId = tempId;
    }

    return ok;
}
