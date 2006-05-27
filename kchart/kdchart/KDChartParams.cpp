/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDChart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KDChart licenses may use this file in
 ** accordance with the KDChart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.klaralvdalens-datakonsult.se/?page=products for
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@klaralvdalens-datakonsult.se if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#include <KDChartParams.h>
#include <KDChartAxisParams.h>
#include <KDChartEnums.h>
#include <KDFrame.h>
#include <KDChartCustomBox.h>
#include <KDChartTextPiece.h>
#include <KDXMLTools.h>
#ifndef KDCHART_MASTER_CVS
#include "KDChartParams.moc"
#endif

class KDChartData;

//#include <qdom.h>

/**
  \class KDChartParams KDChartParams.h
  \brief Bundles the display parameters of a chart.

  Bundles all parameters of a chart including the type except the
  actual data displayed. Serializing an object of this type plus the
  data displayed is enough to be able to recreate the chart later.
  */



//QColor  KDChartParams::_internalPointer_DataValueAutoColor = QColor( 0,1,0 );

KDChartAutoColor* KDChartAutoColor::mInstance = NULL;

KDChartAutoColor::KDChartAutoColor()
{
}

KDChartAutoColor::KDChartAutoColor( KDChartAutoColor const& )
{
}

KDChartAutoColor::~KDChartAutoColor()
{
}

const KDChartAutoColor* KDChartAutoColor::instance()
{
    if( mInstance == 0 )
        mInstance = new KDChartAutoColor();
    return mInstance;
}

void KDChartAutoColor::freeInstance()
{
    if( mInstance )
        delete mInstance;
    mInstance = 0;
}


/*
static QColor defaultColor;
QT_STATIC_CONST_IMPL QColor & KDChartParams_AutoColor = defaultColor;
*/

/**
  Constructor. Defines default values.
  */
KDChartParams::KDChartParams()
{
    tempPropSetA = new KDChartPropertySet();
    tempPropSetB = new KDChartPropertySet();

    // GENERAL

    // Avoid Purify UMR
    _maxDatasetColor = 1;
    _maxDatasetSourceMode = 0;
    _setChartSourceModeWasUsed = false;

    _customBoxDictMayContainHoles = false;

    // Set the default font params flag for data value texts
    // but do *not* call setPrintDataValues() there since
    // this will be called internally by setChartType() below.
    setPrintDataValuesWithDefaultFontParams( KDCHART_ALL_CHARTS, false );
    _printDataValuesSettings._dataValuesShowInfinite  = true;
    _printDataValuesSettings2._dataValuesShowInfinite = true;

    setAllowOverlappingDataValueTexts( false );

#if COMPAT_QT_VERSION >= 0x030100
    setOptimizeOutputForScreen( false );
#else
    setOptimizeOutputForScreen( true );
#endif

    setGlobalLeading( 0,0,0,0 );


    // Default type is bar charts
    setChartType( Bar );

    // By default, there is no additional chart type
    setAdditionalChartType( NoType );

    // Default is to show all values.
    setNumValues( static_cast<unsigned int>( -1 ) );

    _defaultFont = QFont( "helvetica", 8, QFont::Normal, false );

    // The default frame settings: no border, no corners, no background
    _noFrame.clearAll();

    // The default frame settings: no inner gap, no outer gap and use the _noFrame
    // *** no need to initialize _noFrameSettings: it is initialized by a default c'tor!

    // The default brightness of shadow colors (needs to be set
    // before the data colors to avoid an UMR).
    setShadowBrightnessFactor( 1.0 );

    // The default shadow fill style.
    setShadowPattern( Qt::SolidPattern );

    // Some default colors for the data.
    setDataDefaultColors();

    // Default color for data display outlines.
    setOutlineDataColor( black );

    // Default line width for data display outlines.
    setOutlineDataLineWidth( 1 );

    // Default line style for data display outlines.
    setOutlineDataLineStyle( Qt::SolidLine );

    // END GENERAL


    setDataArea(  QRect( QPoint(0,0), QSize(0,0)) );
    setLegendArea(QRect( QPoint(0,0), QSize(0,0)) );


    // BAR CHART-SPECIFIC

    // Default bar subtype is normal
    setBarChartSubType( BarNormal );

    // Default is not to draw 3D bars
    setThreeDBars( false );

    // Default is to used shadowed colors for 3D bar effects
    setThreeDBarsShadowColors( true );

    // Default angle for 3D bars is 45 degrees.
    setThreeDBarAngle( 45 );

    // Default depth of 3D bars is 1.0
    setThreeDBarDepth( 1.0 );

    // Default gap between datasets is 6 per mille of chart data area.
    setDatasetGap( 6 );
    setDatasetGapIsRelative( true );

    // Default gap between value blocks is 24 per mille of chart data area.
    setValueBlockGap( 24 );
    setValueBlockGapIsRelative( true );

    // Default is to have the bar width's calculated indiidually
    setBarWidth( KDCHART_AUTO_SIZE );
    // reset the special indicator members storing the number
    // of bars that were drawn last time / could not be drawn
    // due to too less horizontal space
    setBarsDisplayed( 0 );
    setBarsLeft( 0 );

    // By default, excess arrows are drawn in a split fashion
    setDrawSolidExcessArrows( false );

    // END BAR CHART-SPECIFIC


    // LINE/AREA CHART-SPECIFIC
    // Normal lines by default
    setLineChartSubType( LineNormal );

    // No markers by default
    setLineMarker( false );

    // Lines have a width of 1 pixel by default
    setLineWidth( 1 );

    // Lines are solid by default
    setLineStyle( Qt::SolidLine );

    // Lines have the same color as their
    // respective data points by default
    setLineColor();


    // Default line marker styles and size
    _maxDatasetLineMarkerStyle = 0; // avoid UMR
    setLineMarkerStyle( 0, LineMarkerCircle );
    setLineMarkerStyle( 1, LineMarkerSquare );
    setLineMarkerStyle( 2, LineMarkerDiamond );
    setLineMarkerSize( QSize( 6, 6 ) );

    // 3D line settings
    setThreeDLines( false );
    setThreeDLineDepth( 10 );
    /* temporary disabled:
       setThreeDLineXRotation( 30 );
       setThreeDLineYRotation( 30 );
       */
    setThreeDLineXRotation( 15 );
    setThreeDLineYRotation( 15 );

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


    // BOX+WHISKER CHART-SPECIFIC
    // Simple Box-and-Whisker charts by default
    setBWChartSubType( KDChartParams::BWSimple );
    setBWChartFences(1.5, 1.5,  3.0, 3.0);
    setBWChartOutValMarkerSize( -25 );
    setBWChartPrintStatistics( BWStatValALL, false );
    setBWChartBrush( Qt::white );

    // LEGEND
    // Distance between legend and data.
    setLegendSpacing( 20 );
    // Position of the legend
    setLegendPosition( LegendRight );
    // Orientation of the legend
    setLegendOrientation( Qt::Vertical );
    // Whether the legend shall show lines or just
    // show the markers (or squares, resp.)
    setLegendShowLines( false );
    // Where the legend labels come from
    setLegendSource( LegendAutomatic );
    // legend texts are drawn in black by default
    setLegendTextColor( Qt::black );
    // legend font size is calculated dynamically, but ignore the font size
    setLegendFont( QFont( "helvetica", 10, QFont::Normal, false ), false );
    // legend font size is calculated dynamically:
    //                    20 / 1000 of the average value of
    //                                 the printable area height and width
    setLegendFontRelSize( 16 );
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
    setLegendTitleFontRelSize( 20 );
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
    int relHd0Size = 15;
    int relHdSize  = 22;
    int relHd2Size = 19;

    int relFt0Size = 15;
    int relFtSize  = 19;
    int relFt2Size = 12;
    setHeaderFooterFont( KDChartParams::HdFtPosHeader0,
            defaultHdFtFont, true, relHd0Size );
    setHeaderFooterFont( KDChartParams::HdFtPosHeader0L,
            defaultHdFtFont, true, relHd0Size );
    setHeaderFooterFont( KDChartParams::HdFtPosHeader0R,
            defaultHdFtFont, true, relHd0Size );
    setHeaderFooterFont( KDChartParams::HdFtPosHeader,
            defaultHdFtFont, true, relHdSize );
    setHeaderFooterFont( KDChartParams::HdFtPosHeaderL,
            defaultHdFtFont, true, relHdSize );
    setHeaderFooterFont( KDChartParams::HdFtPosHeaderR,
            defaultHdFtFont, true, relHdSize );
    setHeaderFooterFont( KDChartParams::HdFtPosHeader2,
            defaultHdFtFont, true, relHd2Size );
    setHeaderFooterFont( KDChartParams::HdFtPosHeader2L,
            defaultHdFtFont, true, relHd2Size );
    setHeaderFooterFont( KDChartParams::HdFtPosHeader2R,
            defaultHdFtFont, true, relHd2Size );

    setHeaderFooterFont( KDChartParams::HdFtPosFooter0,
            defaultHdFtFont, true, relFt0Size );
    setHeaderFooterFont( KDChartParams::HdFtPosFooter0L,
            defaultHdFtFont, true, relFt0Size );
    setHeaderFooterFont( KDChartParams::HdFtPosFooter0R,
            defaultHdFtFont, true, relFt0Size );
    setHeaderFooterFont( KDChartParams::HdFtPosFooter,
            defaultHdFtFont, true, relFtSize );
    setHeaderFooterFont( KDChartParams::HdFtPosFooterL,
            defaultHdFtFont, true, relFtSize );
    setHeaderFooterFont( KDChartParams::HdFtPosFooterR,
            defaultHdFtFont, true, relFtSize );
    setHeaderFooterFont( KDChartParams::HdFtPosFooter2,
            defaultHdFtFont, true, relFt2Size );
    setHeaderFooterFont( KDChartParams::HdFtPosFooter2L,
            defaultHdFtFont, true, relFt2Size );
    setHeaderFooterFont( KDChartParams::HdFtPosFooter2R,
            defaultHdFtFont, true, relFt2Size );
    // END HEADERS/FOOTERS

    // PROPERTY SETS
    tempPropSetA->fullReset("normal data");
    setProperties(KDCHART_PROPSET_NORMAL_DATA, *tempPropSetA);
    // don't show the line, don't show the marker
    tempPropSetA->setName("transparent data");
    tempPropSetA->setLineStyle(  KDChartPropertySet::OwnID, Qt::NoPen );
    tempPropSetA->setShowMarker( KDChartPropertySet::OwnID, false );
    setProperties(KDCHART_PROPSET_TRANSPARENT_DATA, *tempPropSetA);
    // don't show line nor marker, but do show the horizontal line
    tempPropSetA->setName("horizontal line");
    tempPropSetA->setExtraLinesAlign(  KDChartPropertySet::OwnID, Qt::AlignLeft | Qt::AlignRight );
    setProperties(KDCHART_PROPSET_HORI_LINE, *tempPropSetA);
    // don't show line nor marker, but do show the vertical line
    tempPropSetA->setName("vertical line");
    tempPropSetA->setExtraLinesAlign(  KDChartPropertySet::OwnID, Qt::AlignTop | Qt::AlignBottom );
    setProperties(KDCHART_PROPSET_VERT_LINE, *tempPropSetA);
    // END PROPERTY SETS
}


/**
  Destructor. Only defined to have it virtual.
  */
KDChartParams::~KDChartParams()
{
    KDChartAutoColor::freeInstance();
    delete tempPropSetA;
    delete tempPropSetB;
}


// GENERAL
/** @name General parameters.

  These methods set general parameters that apply to several or all chart types.
  */
//@{


/**
  Stores a new property set: data are stored by value so you may
  use your \c rSet instance for other purpose later...

  \returns The property set's ID to be used for later retrieving
  the property information by calling the properties() function
  or for assigning these properties to a data cell - either in the
  KDChartData constructor or by calling KDChartData::setPropertySet().

  \note The property set's ID may also be retrieved by calling
  <b>set.id()</b> after calling registerProperties( set )

  \note The predefined (build-in) property ids (like KDCHART_PROPSET_NORMAL_DATA
  and KDCHART_PROPSET_TRANSPARENT_DATA) must not be registered
  but should be used without further initialization.

  \sa KDCHART_PROPSET_NORMAL_DATA, KDCHART_PROPSET_TRANSPARENT_DATA
  \sa KDCHART_PROPSET_HORI_LINE, KDCHART_PROPSET_VERT_LINE
  \sa KDChartData::setPropertySet
  \sa removeProperties
  */
int KDChartParams::registerProperties( KDChartPropertySet& rSet )
{
    _propertySetList.setAutoDelete( true );
    rSet.mOwnID = _propertySetList.count();
    _propertySetList.insert( rSet.mOwnID, rSet.clone() );
    return rSet.mOwnID;
}

/**
  Overwrites a property set with a new property set.

  \note This function might also be used to initialy store
  a property set using a specific ID number, but note
  that another property set having the same number
  would be replaced by this property set automatically. Therefor
  in general it is better to use the registerProperties function
  to initially obtain a unique ID number for your new property set.

  \returns TRUE if the property set had been stored before,
  or FALSE if the set was now stored initially.

  \sa KDCHART_PROPSET_TRANSPARENT_DATA
  \sa KDCHART_PROPSET_HORI_LINE, KDCHART_PROPSET_VERT_LINE
  \sa KDChartData::setPropertySet
  \sa removeProperties
  */
void KDChartParams::setProperties( int id, KDChartPropertySet& rSet )
{
    _propertySetList.setAutoDelete( true );
    rSet.mOwnID = id;
    _propertySetList.replace( rSet.mOwnID, rSet.clone() );
}

/**
  Removes a property set from the registry
  that was registered via registerProperties().

  \note It is not possible to remove the build-in default property set:
  function calls using KDCHART_PROPSET_NORMAL_DATA as ID
  will be ignored.

  \returns TRUE if the property set was found and removed, or FALSE
  if the set was not found or KDCHART_PROPSET_NORMAL_DATA
  was specified as ID value.

  \sa registerProperties, setProperties
  */
bool KDChartParams::removeProperties( int id )
{
    _propertySetList.setAutoDelete( true );
    // Removing the default property set is not allowed!
    if( KDCHART_PROPSET_NORMAL_DATA == id )
        return false;
    return _propertySetList.remove( id );
}


/**
  Retrieves the values specified for a property set that
  was stored by registerProperties().

  Use this function to retrieve the exact specification
  of a property set.

  Don't use this function to retrieve the properties that are
  valid for a given data cell but use KDChartParams::calculateProperties()
  instead.

  \note This function does <b>not</b> return a pointer
  to the property set itself but a copy of the data
  stored therein. To change a stored property set
  you may use the setProperties() function.

  \returns TRUE if the property set was found, FALSE
  if it no property set was registred with this ID.

  \sa registerProperties, KDChartData::setPropertySet
  */
bool KDChartParams::properties( int id, KDChartPropertySet& rSet ) const
{
    const KDChartPropertySet* R = _propertySetList.find( id );
    const bool bFound = (0 != R);
    if( bFound )
        rSet.deepCopy( R );
    return bFound;
}


/**
  Retrieves the internal property set object that
  created when you called registerProperties().

  Use this function to access a property set for modifying it directly.

  Don't use this function to retrieve the properties that are
  valid for a given data cell but use KDChartParams::calculateProperties()
  instead.

  \note This function <b>does</b> return a pointer
  to the property set itself, so you can directly modify its contents.
  <b>However</b> you have to make sure NOT to delete this pointer,
  since KDChartParams is owning it, so it would try to free
  the pointer again later, most likely resulting in a program crash.
  To delete a stored property set you may use the removeProperties() function.

  \returns A pointer to the internally stored property set if the property set was found,
  Zero if no property set was registred with this ID.

  \sa registerProperties, removeProperties, KDChartData::setPropertySet
  */
KDChartPropertySet* KDChartParams::properties( int id )
{
    return _propertySetList.find( id );
}


/**
  Retrieves the values that correspond to a property set by
  reading this set's properties and obtaining other property
  set's values if necessary (according to ID numbers that might
  be stored in the property set).

  Use this function to retrieve the properties that are
  valid for a given data cell.

  Don't use this function to retrieve the exact specification
  of a property set but use KDChartParams::properties() instead.

  \returns TRUE if the property set was found, FALSE
  if it no property set was registred with this ID.

  \sa registerProperties, KDChartData::setPropertySet
  */
bool KDChartParams::calculateProperties( int startId, KDChartPropertySet& rSet ) const
{
    KDChartPropertySet& startSet = *tempPropSetA;
    startSet.quickReset("");

    rSet.deepCopy( &startSet ); // reset all properties of rSet to the default !!

//qDebug("in KDChartParams::calculateProperties():");
//qDebug("   startId: %i",startId);
//qDebug("   startSet: %s",startSet.name().latin1());

    bool bOk = properties(startId, startSet);
    if( bOk ){
        int          lineWidth;
        QColor       lineColor;
        Qt::PenStyle lineStyle;
        bool         showMarker;
        uint         markerAlign;
        QSize        markerSize;
        QColor       markerColor;
        int          markerStyle;
        uint         extraLinesAlign;
        bool         extraLinesInFront;
        int          extraLinesLength;
        int          extraLinesWidth;
        QColor       extraLinesColor;
        Qt::PenStyle extraLinesStyle;
        uint         extraMarkersAlign;
        QSize        extraMarkersSize;
        QColor       extraMarkersColor;
        int          extraMarkersStyle;
        bool         showBar;
        QColor       barColor;
        QBrush       areaBrush;
        // c'tor sets all IDs to unknown by default
        KDChartPropertySet& propSet = *tempPropSetB;
        propSet.quickReset("");
        // PENDING(khz) replace the rustic depth counter i by a smart way
        // to quickly and safely recognize forbidden circular ID dependencies
        // *without* using this artificial maximal tree depth limitation.
        const int maxDepth = 1000;
        int i;
        int id;
        // retrieve lineWidth
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnLineWidth( id, lineWidth ) ){
                rSet.setLineWidth( KDChartPropertySet::OwnID, lineWidth );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );
        // retrieve lineColor
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnLineColor( id, lineColor ) ){
                rSet.setLineColor( KDChartPropertySet::OwnID, lineColor );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );
        // retrieve lineStyle
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnLineStyle( id, lineStyle ) ){
                rSet.setLineStyle( KDChartPropertySet::OwnID, lineStyle );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );

        // markers at cell value position:

        // retrieve showMarker
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnShowMarker( id, showMarker ) ){
                rSet.setShowMarker( KDChartPropertySet::OwnID, showMarker );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );
        // retrieve marker alignment
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnMarkerAlign( id, markerAlign ) ){
                rSet.setMarkerAlign( KDChartPropertySet::OwnID, markerAlign );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );
        // retrieve marker size
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnMarkerSize( id, markerSize ) ){
                rSet.setMarkerSize( KDChartPropertySet::OwnID, markerSize );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );
        // retrieve marker color
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnMarkerColor( id, markerColor ) ){
                rSet.setMarkerColor( KDChartPropertySet::OwnID, markerColor );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );
        // retrieve marker style
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnMarkerStyle( id, markerStyle ) ){
                rSet.setMarkerStyle( KDChartPropertySet::OwnID, markerStyle );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );

        // extra lines:

        // retrieve alignment of extra lines
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnExtraLinesAlign( id, extraLinesAlign ) ){
                rSet.setExtraLinesAlign( KDChartPropertySet::OwnID, extraLinesAlign );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );
        // retrieve whether the extra lines shall be printed in front of the normal lines
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnExtraLinesInFront( id, extraLinesInFront ) ){
                rSet.setExtraLinesInFront( KDChartPropertySet::OwnID, extraLinesInFront );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );
        // retrieve lineLength
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnExtraLinesLength( id, extraLinesLength ) ){
                rSet.setExtraLinesLength( KDChartPropertySet::OwnID, extraLinesLength );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );
        // retrieve lineWidth
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnExtraLinesWidth( id, extraLinesWidth ) ){
                rSet.setExtraLinesWidth( KDChartPropertySet::OwnID, extraLinesWidth );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );
        // retrieve lineColor
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnExtraLinesColor( id, extraLinesColor ) ){
                rSet.setExtraLinesColor( KDChartPropertySet::OwnID, extraLinesColor );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );
        // retrieve lineStyle
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnExtraLinesStyle( id, extraLinesStyle ) ){
                rSet.setExtraLinesStyle( KDChartPropertySet::OwnID, extraLinesStyle );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );

        // markers at the ends of the extra lines:

        // retrieve marker alignment
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnExtraMarkersAlign( id, extraMarkersAlign ) ){
                rSet.setExtraMarkersAlign( KDChartPropertySet::OwnID, extraMarkersAlign );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );
        // retrieve marker size
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnExtraMarkersSize( id, extraMarkersSize ) ){
                rSet.setExtraMarkersSize( KDChartPropertySet::OwnID, extraMarkersSize );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );
        // retrieve marker color
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnExtraMarkersColor( id, extraMarkersColor ) ){
                rSet.setExtraMarkersColor( KDChartPropertySet::OwnID, extraMarkersColor );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );
        // retrieve marker style
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnExtraMarkersStyle( id, extraMarkersStyle ) ){
                rSet.setExtraMarkersStyle( KDChartPropertySet::OwnID, extraMarkersStyle );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );

        // retrieve showBar
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnShowBar( id, showBar ) ){
                rSet.setShowBar( KDChartPropertySet::OwnID, showBar );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );
        // retrieve barColor
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnBarColor( id, barColor ) ){
                rSet.setBarColor( KDChartPropertySet::OwnID, barColor );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );

        // retrieve areaBrush
        propSet.deepCopy( &startSet ); i=0;
        do{
            if( propSet.hasOwnAreaBrush( id, areaBrush ) ){
                rSet.setAreaBrush( KDChartPropertySet::OwnID, areaBrush );
                break;
            }else if( KDChartPropertySet::UndefinedID == id || maxDepth < i )
                break;
            ++i;
        }while( properties(id, propSet) );

    }
    return bOk;
}


/**
  \fn int KDChartParams::roundVal(double)

  Returns the parameter \c d rounded to the nearest integer.
  */



/**
  Specifies if and how a chart will print the data value texts near
  their respective entries.

  Data value texts will be printed immediately after drawing all of
  the chart data points (or bars, lines,...) but before drawing the
  legend and before drawing any custom boxes.  If more than one chart
  ist to be drawn (sharing the same data area) printing of the data
  text values will take place after drawing all of the last charts
  data points, this enables us to see the texts even if their
  respective data representations are covered by the second charts
  drawings. The same covering/colliding problem might occur with Area
  charts if one area is (partially) covering another area.  In such
  cases you might either want to specify an appropriate
  TextLayoutPolicy for getting a better looking result or specify an
  other text color for data value texts of the second chart since by
  default the first chart has black texts while the second chart
  shows its data value texts in dark blue color.

  \note Only used if chartType() is <b>neither HiLo nor
  BoxWhisker</b>. To specify printing of data values in a HiLo chart
  please use setHiLoChartPrintLowValues, setHiLoChartPrintHighValues,
  setHiLoChartPrintOpenValues, setHiLoChartPrintCloseValues. To
  specify printing of data values in a BoxWhisker chart please use
  setBWChartPrintStatistics.

  Calling <b>setPrintDataValues( false )</b> will <b>deactivate</b>
  printing of the values.

  Calling setPrintDataValuesWithDefaultFontParams( chart ) will
  <b>reset</b> the respective font size and colour and position
  parameters (but not the QFont itself) and <b>activate</b> printing
  of the values for the \c chart speficied (or for all charts by
  using \c KDCHART_ALL_CHARTS, resp.).

  \param active specifies whether the value texts are to be printed or not.
  \param chart The number of the chart: 0 for the first chart, 1 for
  the second chart in case there are two charts to be drawn sharing the
  same data area.  Use the special value KDCHART_ALL_CHARTS
  to specify that your settings are to be taken for both charts.
  \param divPow10 The power of 10 which the data value is to be divided by.
  A value of 2 means divide by 100, a value of  -3 means multiply by 1000,
  and 0 (by definition) would result in multiplying by 1.
  \param digitsBehindComma The number of digits to show behind the comma,
  to have this calculated automatically just use the default value
  KDCHART_DATA_VALUE_AUTO_DIGITS.
  \param font a Pointer to the font to be used, if zero the default data value
  texts font will be taken (this is a Times font since small Times digits are
  clearer than small Helvetica digits).

  Changing of one or more of <b>the following parameters</b>
  automatically de-activates all future font parameter adjustments
  that would otherwise take place after each call of setChartType (or
  setAdditionalChartType, resp.).  To re-enable this usefull feature
  you may call setPrintDataValuesWithDefaultFontParams at any time
  afterwards.

  \param size (in per mille of the chart width) the dynamic size of
  the font to be used. If this parameter is zero the size of the
  \c font is used instead - regardless of the size of the chart!
  You may use setPrintDataValuesFontRelSize to change this parameter setting
  without affecting the other ones.
  \param color the color to be used when printing the values.
  To have the color calculated automatically - useful when printing
  inside the bars (or pie slices, areas, ... resp.) - please use
  \c KDCHART_DATA_VALUE_AUTO_COLOR instead of a QColor*.
  You may use setPrintDataValuesColor to change this parameter setting
  without affecting the other ones.

  The following parameters apply to values less than zero only:

  \param negativePosition The anchor position which the text is to be
aligned to.
\param negativeAlign The way how the text is to be aligned to the anchor.
This must be a reasonable combination of Qt::AlignmentFlags.
\param negativeDeltaX The X distance between the <b>anchor
point</b> -- specified by \c negativePosition (or \c
        positivePosition, resp.) -- and the internal <b>alignment point</b>
of the text -- specified by \c negativeAlign (or \c positiveAlign,
        resp.). <b>Note: </b> For better compatibility to the dynamic font
size this parameter is interpreted as being a per-cent value of the
used font height.  If greater 0, the X position is increased, if
less than 0, it is reduced. Actual font size and thus also this
delta value are calculated dynamically before painting based on the
size of the chart and the specification made via parameter \c size.

\param negativeDeltaY The Y distance between the <b>anchor
point</b> -- specified by \c negativePosition (or \c
        positivePosition, resp.) -- and the internal <b>alignment point</b>
of the text -- specified by \c negativeAlign (or \c positiveAlign,
        resp.).  <b>Note: </b> For better compatibility to the dynamic font
size this parameter is interpreted as being a per-cent value of the
used font height.  If greater 0, the Y position is increased, if
less than 0, it is reduced. Actual font size and thus also this
delta value are calculated dynamically before painting based on the
size of the chart and the specification made via parameter \c size.
\param negativeRotation The amount of degrees (using a circle of
        360 degrees) taken to rotate the text. Positive values rotate
clockwise, negative values rotate counter-clockwise.  There are two
special values that you might find usefull for Pie charts or for
Ring charts: \c KDCHART_SAGGITAL_ROTATION and \c
KDCHART_TANGENTIAL_ROTATION both leading to individual
calculation of appropriate rotation for each data value.  Rotation
will be performed around the internal <b>alignment point</b> of the
text -- specified by \c negativeAlign (or \c positiveAlign, resp.).

The following parameters apply to values greater than zero or equal zero:

\param positivePosition The anchor position which the text is to be
aligned to.
\param positiveAlign The way how the text is to be aligned to the anchor.
This must be a reasonable combination of Qt::AlignmentFlags.
\param negativeDeltaX The X distance between the <b>anchor
point</b> -- specified by \c negativePosition (or \c
        positivePosition, resp.) -- and the internal <b>alignment point</b>
of the text -- specified by \c negativeAlign (or \c positiveAlign,
        resp.).  <b>Note: </b> For better compatibility to the dynamic font
size this parameter is interpreted as being a per-cent value of the
used font height.  If greater 0, the X position is increased, if
less than 0, it is reduced. Actual font size and thus also this
delta value are calculated dynamically before painting based on the
size of the chart and the specification made via parameter \c size.
\param positiveDeltaY The Y distance between the <b>anchor
point</b> -- specified by \c negativePosition (or \c
        positivePosition, resp.) -- and the internal <b>alignment point</b>
of the text -- specified by \c negativeAlign (or \c positiveAlign,
        resp.).  <b>Note: </b> For better compatibility to the dynamic font
size this parameter is interpreted as being a per-cent value of the
used font height.  If greater 0, the Y position is increased, if
less than 0, it is reduced. Actual font size and thus also this
delta value are calculated dynamically before painting based on the
size of the chart and the specification made via parameter \c size.
\param positiveRotation The amount of degrees (using a circle of
        360 degrees) taken to rotate the text. Positive values rotate
clockwise, negative values rotate counter-clockwise.  There are two
special values that you might find usefull for Pie charts or for
Ring charts: \c KDCHART_SAGGITAL_ROTATION and \c
KDCHART_TANGENTIAL_ROTATION both leading to individual
calculation of appropriate rotation for each data value.  Rotation
will be performed around the internal <b>alignment point</b> of the
text -- specified by \c negativeAlign (or \c positiveAlign, resp.).

\param layoutPolicy The way to handle too narrow space conflicts:
what to do if a data text covers a neighboring data text (or a
        neighboring data area, resp.).

\sa printDataValues
\sa setPrintDataValuesWithDefaultFontParams, printDataValuesWithDefaultFontParams
\sa setPrintDataValuesFontRelSize, setPrintDataValuesColor
\sa dataValuesDivPow10
\sa dataValuesDigitsBehindComma
\sa dataValuesFontUseRelSize
\sa dataValuesFontRelSize
\sa dataValuesFontColor
\sa dataValuesAnchorPosition
\sa dataValuesAnchorAlign
\sa dataValuesAnchorDeltaX
\sa dataValuesAnchorDeltaY
\sa dataValuesRotation
\sa dataValuesLayoutPolicy
*/
void KDChartParams::setPrintDataValues( bool active,
        uint chart,
        int divPow10,
        int digitsBehindComma,
        QFont* font,
        uint size,
        const QColor* color,
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
                              settings->_dataValuesBrush = QBrush( Qt::NoBrush );
                              // for values below zero:
                              settings->_dataValuesAnchorNegativePosition = KDChartEnums::PosBottomRight;
                              settings->_dataValuesAnchorNegativeAlign    = Qt::AlignBottom + Qt::AlignRight;
                              settings->_dataValuesAnchorNegativeDeltaX   =  20;
                              settings->_dataValuesAnchorNegativeDeltaY   =  55;
                              settings->_dataValuesNegativeRotation       = 300;
                              // for values greater/equal zero:
                              settings->_dataValuesAnchorPositivePosition = KDChartEnums::PosTopLeft;
                              settings->_dataValuesAnchorPositiveAlign    = Qt::AlignTop + Qt::AlignLeft;
                              settings->_dataValuesAnchorPositiveDeltaX   = - 20;
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
                               settings->_dataValuesBrush = QBrush( Qt::NoBrush );
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
                               settings->_dataValuesFontRelSize = 21;
                               settings->_dataValuesAutoColor              = true;  //  !!!
                               settings->_dataValuesColor = QColor( Qt::black );
                               settings->_dataValuesBrush = QBrush( Qt::white );

                               bool bShowOutside = areaChartSubType() == AreaNormal;
                               // for values below zero:
                               settings->_dataValuesAnchorNegativePosition = KDChartEnums::PosBottomCenter;
                               settings->_dataValuesAnchorNegativeAlign    = Qt::AlignHCenter
                                                            + (bShowOutside ? Qt::AlignTop : Qt::AlignBottom);
                               settings->_dataValuesAnchorNegativeDeltaX   =   0;
                               settings->_dataValuesAnchorNegativeDeltaY   = bShowOutside ? 20 : -35;
                               settings->_dataValuesNegativeRotation       =   0;
                               // for values greater/equal zero:
                               settings->_dataValuesAnchorPositivePosition = KDChartEnums::PosTopCenter;
                               settings->_dataValuesAnchorPositiveAlign    = Qt::AlignHCenter
                                                            + (bShowOutside ? Qt::AlignBottom : Qt::AlignTop);
                               settings->_dataValuesAnchorPositiveDeltaX   =   0;
                               settings->_dataValuesAnchorPositiveDeltaY   = bShowOutside ? -20 : 35;
                               settings->_dataValuesPositiveRotation       =   0;

                               settings->_dataValuesLayoutPolicy = KDChartEnums::LayoutPolicyRotate;
                           }
                           break;
                case HiLo:
                case BoxWhisker:
                           // settings are not defined here because HiLo and BW charts
                           // are *not* set up using setPrintDataValues()
                           // but by using their own methods
                           break;
                case Pie: {
                              if ( font )
                                  settings->_dataValuesFont = *font;
                              else
                                  settings->_dataValuesFont = QFont( "times", 1, QFont::Bold );
                              settings->_dataValuesUseFontRelSize = true;
                              settings->_dataValuesFontRelSize = 25;
                              settings->_dataValuesAutoColor            = true;  //  !!!
                              settings->_dataValuesColor = QColor( Qt::black );
                              settings->_dataValuesBrush = QBrush( Qt::NoBrush );
                              // for values below zero:
                              settings->_dataValuesAnchorNegativePosition = KDChartEnums::PosTopCenter;
                              settings->_dataValuesAnchorNegativeAlign    = Qt::AlignTop + Qt::AlignHCenter;
                              settings->_dataValuesAnchorNegativeDeltaX   =  0;
                              settings->_dataValuesAnchorNegativeDeltaY   = 50;
                              settings->_dataValuesNegativeRotation       = KDCHART_TANGENTIAL_ROTATION;
                              // for values greater/equal zero:
                              settings->_dataValuesAnchorPositivePosition = KDChartEnums::PosTopCenter;
                              settings->_dataValuesAnchorPositiveAlign    = Qt::AlignTop + Qt::AlignHCenter;
                              settings->_dataValuesAnchorPositiveDeltaX   =  0;
                              settings->_dataValuesAnchorPositiveDeltaY   = 50;
                              settings->_dataValuesPositiveRotation       = KDCHART_TANGENTIAL_ROTATION;

                              settings->_dataValuesLayoutPolicy = KDChartEnums::LayoutPolicyRotate;
                          }
                          break;
                case Ring: {
                               if ( font )
                                   settings->_dataValuesFont = *font;
                               else
                                   settings->_dataValuesFont = QFont( "times", 1, QFont::Bold );
                               settings->_dataValuesUseFontRelSize = true;
                               settings->_dataValuesFontRelSize = 25;
                               settings->_dataValuesAutoColor            = true;  //  !!!
                               settings->_dataValuesColor = QColor( Qt::black );
                               settings->_dataValuesBrush = QBrush( Qt::NoBrush );
                               // for values below zero:
                               settings->_dataValuesAnchorNegativePosition = KDChartEnums::PosCenter;
                               settings->_dataValuesAnchorNegativeAlign    = Qt::AlignCenter;
                               settings->_dataValuesAnchorNegativeDeltaX   = 0;
                               settings->_dataValuesAnchorNegativeDeltaY   = 10;
                               settings->_dataValuesNegativeRotation       = KDCHART_TANGENTIAL_ROTATION;
                               // for values greater/equal zero:
                               settings->_dataValuesAnchorPositivePosition = KDChartEnums::PosCenter;
                               settings->_dataValuesAnchorPositiveAlign    = Qt::AlignCenter;
                               settings->_dataValuesAnchorPositiveDeltaX   = 0;
                               settings->_dataValuesAnchorPositiveDeltaY   = 10;
                               settings->_dataValuesPositiveRotation       = KDCHART_TANGENTIAL_ROTATION;

                               settings->_dataValuesLayoutPolicy = KDChartEnums::LayoutPolicyRotate;
                           }
                           break;

                case Polar: {
                                settings->_dataValuesFontRelSize = 18;
                                if ( font )
                                    settings->_dataValuesFont = *font;
                                else
                                    settings->_dataValuesFont = QFont( "times", 1, QFont::Bold );
                                settings->_dataValuesUseFontRelSize = true;
                                settings->_dataValuesFontRelSize = 26;
                                settings->_dataValuesAutoColor   = polarMarker();  //  !!!
                                settings->_dataValuesColor = QColor( Qt::black );
                                settings->_dataValuesBrush = QBrush( Qt::NoBrush );

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

                                //settings->_dataValuesLayoutPolicy = KDChartEnums::LayoutPolicyShrinkFontSize;
                                //settings->_dataValuesFontRelSize = 26;
                                //setDefaultAxesTypes();
                                //finished = false;  // use build-in default params, see KDChartParams.h::setPrintDataValues()
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

            uint theSize( UINT_MAX == size ? 14 : size );
            settings->_dataValuesUseFontRelSize = ( 0 < theSize );
            settings->_dataValuesFontRelSize = theSize;
            if (    KDCHART_DATA_VALUE_AUTO_COLOR == color
                 && ( Polar != cType || polarMarker() ) ) {
                settings->_dataValuesAutoColor = true;  //  !!!
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
            settings->_dataValuesBrush = Qt::NoBrush;
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


/**
  \enum KDChartParams::SourceMode

  The chart source mode.
  Use \c setChartSourceMode to specify if and how any data
  should be taken into account for a given chart.

  \sa setChartSourceMode, chartSourceMode, setAdditionalChartType
  */


/**
  Specifies the chart source mode of one or more datasets.
  Use this to specify if and how any data should be taken
  into account for a given chart. You may call this function
  multiple times to specify how the different datasets are to
  be used. Number of datasets is only limited by the data that
  you actually provide to the chart widget. Usage of this method
  is mandatory when specifying combinations of more than one
  chart (e.g. via \c setAdditionalChartType() ).

  \note If specifying <b>more than one dataset</b> for the very
  same chart and the same source mode make sure they form
  a contiguous <b>series</b>.<br>
  It is not possible to use dataset 0..3 as DataEntry, dataset
  4 as AxisLabel and again dataset 5..6 as DataEntry for the
  very same chart!<br>
  Of course you could specify 0..3 as DataEntry for chart 0
  while 5..6 would contain the data entries for chart 1
  when specifying a widget showing two charts.

  <b>Also note:</b><br>
  Whenever you use \c setChartSourceMode() make sure not to
  forget any of the datasets which you want to define: not
  specifying a dataset would result in it being ignored.<br>
  So the rule is: either don't call setChartSourceMode() at all
  or call it for each dataset!

  To quickly clear all such settings just might want to call
  <b>setChartSourceMode( UnknownMode, KDCHART_ALL_DATASETS );</b>
  without specifying a dataset number and without specifying
  a chart number.<br>
  However this KDCHART_ALL_DATASETS may only be used to
  <b>clear</b> SourceMode settings - but not for defining the
  SourceMode for all datasets - the above mentioned combination
  with <b>UnknownMode</b> is the only way how to use
  <b>KDCHART_ALL_DATASETS</b> here!

  <b>Examples:</b>

  \verbatim
  setChartSourceMode( DataEntry, 2 );
  \endverbatim
  This would specify a simple chart obtaining its values from dataset 2.

  \verbatim
  setChartSourceMode( DataEntry, 0, 0, 0 );
  setChartSourceMode( DataEntry, 3, 3, 1 );
  \endverbatim
  This would specify a two chart widget - the 1st obtaining its values
  from dataset 0 and the 2nd taking the values from dataset 3.

  \verbatim
  setChartSourceMode( LegendText,       1,  1, 0 );
  setChartSourceMode( AxisLabel,        2,  2, 0 );
  setChartSourceMode( DataEntry,        3,  6, 0 );
  setChartSourceMode( ExtraLinesAnchor, 7,  7, 0 );
  setChartSourceMode( LegendText,       8,  8, 1 );
  setChartSourceMode( AxisLabel,        9,  9, 1 );
  setChartSourceMode( DataEntry,       10, 13, 1 );
  \endverbatim
  This would specify a two chart widget - the 1st obtaining its values
  from datasets 3..6 and the 2nd taking the values from datasets 10..13.
  Their respective legends and axes would take their values from
  datasets 1 and 2 (or 8 and 9, resp.).
  Additionally the 1st chart would use dataset 7 to determine the
  horizontal (or vertical, resp.) position of any extra lines
  that might be specified by KDChartPropertySet IDs assigned to
  the cells in dataset 7.

  \note If source mode is ExtraLinesAnchor all cells
  of the respective datset will be ignored when the normal
  cell representations (e.g. Bars or Lines) are drawn - these
  cells will <em>only</em> be used as anchor points for any
  extra lines that are specified with these cell's property sets.

  When specifying \c ExtraLinesAnchor for more than one dataset
  of the same chart make sure they form a contiguous row
  <b>together</b> with the \c DataEntry dataset(s): e.g
  you may use datasets 3..8 for DataEntry cells and datasets
  9..10 for ExtraLinesAnchor cells.
  All datasets with mode DataEntry or ExtraLinesAnchor are taken
  into account for calculation of the axes ranges and delimiters.

  <P>

  \param mode the way how to use information stored in this dataset.
  \param dataset the dataset number to be used (or ignored, resp.).
  Use \c KDCHART_ALL_DATASETS to indicate that information given
  applies to all of the datasets.
  \param dataset2 a second dataset number to be taken into account
  (or ignored, resp.). Use this one together with the \dataset
param to specify a range of datasets.
\param chart the chart taking into account this dataset.
Use this parameter only if more than one chart is to be displayed.
By using KDCHART_ALL_CHARTS here you may specify that the dataset
(or the range of datasets, resp.) is to be taken into account for
all of the charts displayed.

\sa SourceMode, chartSourceMode, findDataset
\sa maxDatasetSourceMode, setAdditionalChartType
*/
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


/**
  Retrieves usage information of a given dataset or a range of
  datasets.

  \note If you specify a range of datasets and not all of them
  share the same SourceMode the return value will be \c UnknownMode
  whereas in case of the dataset(s) not being used it will be \c DontUse.
  Accordingly if you provide a chart pointer and not all the
  datasets share the same chart the chart pointer will receive
  the value KDCHART_UNKNOWN_CHART.<br>

  <b>Also note:</b>
  If \c setChartSourceMode() was not used at all
  chartSourceMode() will return <b>DataEntry</b> no matter what
  dataset number is specified.

  <p>

  \param dataset the dataset number to be looked at.
  Use \c KDCHART_ALL_DATASETS to find out if all of the datasets share
  the very same SourceMode.
  \param dataset2 a second dataset number.
  Use this one together with the \dataset param to look at a range of
  datasets and find out if all of them share the very same SourceMode.
  \param chart If this parameter points to an int the method will
  provide you with the chart number that this dataset (or this range
  of datasets, resp.) is taken into account for.

  \sa SourceMode, setChartSourceMode, setAdditionalChartType
  \sa maxDatasetSourceMode, findDataset
  */
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
            for( it = _dataSourceModeAndChart.find( a );
                 ( it != _dataSourceModeAndChart.end() ) && ( it.key() <= b );
                 ++it ){
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


/**
  Retrieves the dataset number or a range of datasets
  being used with a given SourceMode for a given chart
  or KDCHART_NO_DATASET if no dataset was specified for
  that mode and that chart.

  \note If \c setChartSourceMode() was not used at all
  findDataset() will return true and set the dataset
  to KDCHART_ALL_DATASETS when called for SourceMode
  <b>DataEntry</b>. All other SourceModes will return false then.

  \return True if at least one dataset was found.

  \param mode the SourceMode to search for.
  \param dataset the dataset number found (or the first
  dataset number in case a series of datasets was found, resp.)
  \param dataset2 the dataset number found (or the last
  dataset number in case a series of datasets was found, resp.)
  <b>Hint:</b> You may give the same variable both for
  dataset and for dataset2, e.g. this makes sense in case you
  are sure that findDataset will find only one dataset anyway.
  \param chart the chart number that the dataset (or this range
  of datasets, resp.) must have been defined for. If this is missing
  findDataset assumes you are looking for datasets of the first chart.

  \sa findDatasets, SourceMode, setChartSourceMode, chartSourceMode
  \sa maxDatasetSourceMode, setAdditionalChartType
  */
bool KDChartParams::findDataset( SourceMode mode,
        uint& dataset,
        uint& dataset2,
        uint chart ) const
{
    bool res = false;
    dataset  = KDCHART_NO_DATASET;
    dataset2 = KDCHART_NO_DATASET;
    if ( _setChartSourceModeWasUsed ) {
        bool bStart = true;
        ModeAndChartMap::ConstIterator it;
        for ( it = _dataSourceModeAndChart.begin();
                it != _dataSourceModeAndChart.end(); ++it ) {
            if (    ( it.data().mode() == mode )
                    && (    ( KDCHART_ALL_CHARTS == chart )
                        || ( it.data().chart()  == chart ) ) ) {
                if ( bStart ) {
                    dataset = it.key();
                    bStart = false;
                }
                dataset2 = it.key();
                res = true;
            } else if ( !bStart )
                return res;
        }
    } else if ( DataEntry == mode ) {
        dataset = KDCHART_ALL_DATASETS;
        dataset2 = KDCHART_ALL_DATASETS;
        res = true;
    }
    return res;
}


/**
  Retrieves the dataset number or a range of datasets
  being used with one of two given SourceModes
  for a given chart or KDCHART_NO_DATASET
  if no dataset was specified for that mode and that chart.

  If \c setChartSourceMode() was not used at all
  findDataset() will return true and set the dataset
  to KDCHART_ALL_DATASETS when either \c modeA or \c modeB
  is <b>DataEntry</b>. All other SourceModes will return
  false then.

  \note If the found datasets do <em>not</em> form a
  <b>contiguous series</b> the return value will be false
  and the value of \c dataset will be KDCHART_NO_DATASET
  and a comment will be written to stderr
  (if KDChart was compiled in debug mode).

  \return True if at least one dataset was found.

  \param modeA one of the SourceModes to search for.
  \param modeB the other one of the two SourceModes to search for.
  \param dataset the dataset number found (or the first
  dataset number in case a series of datasets was found, resp.)
  \param dataset2 the dataset number found (or the last
  dataset number in case a series of datasets was found, resp.)
  <b>Hint:</b> You may give the same variable both for
  dataset and for dataset2, e.g. this makes sense in case you
  are sure that findDataset will find only one dataset anyway.
  \param chart the chart number that the dataset (or this range
  of datasets, resp.) must have been defined for. If this is missing
  findDataset assumes you are looking for datasets of the first chart.

  \sa findDataset, SourceMode, setChartSourceMode, chartSourceMode
  \sa maxDatasetSourceMode, setAdditionalChartType
  */
bool KDChartParams::findDatasets( SourceMode modeA,
                                  SourceMode modeB,
                                  uint& dataset,
                                  uint& dataset2,
                                  uint chart ) const
{
    bool res = false;
    dataset  = KDCHART_NO_DATASET;
    dataset2 = KDCHART_NO_DATASET;
    uint dsA1, dsA2, dsB1, dsB2;
    bool foundA = findDataset( modeA, dsA1, dsA2, chart );
    bool foundB = findDataset( modeB, dsB1, dsB2, chart );
    if( foundA || foundB ){
        if( dsA1 == KDCHART_ALL_DATASETS || dsB1 == KDCHART_ALL_DATASETS ){
            dataset  = KDCHART_ALL_DATASETS;
            dataset2 = KDCHART_ALL_DATASETS;
            res = true;
        }else{
            if( foundA && foundB ){
                if( QMIN(dsA2, dsB2) + 1 == QMAX(dsA1, dsB1) ){
                    dataset  = QMIN(dsA1, dsB1);
                    dataset2 = QMAX(dsA2, dsB2);
                    res = true;
                }else{
                    qDebug("ERROR in KDChartParams::findDatasets(): Datasets found are *not* a contiguous series.");
                }
            }else{
                dataset  = foundA ? dsA1 : dsB1;
                dataset2 = foundA ? dsA2 : dsB2;
                res = true;
            }
        }
    }
    return res;
}


/**
  Specifies a color for a dataset. Note that if you define a color for a
  dataset but not for a dataset with a lower number (and there is neither
  a default value), the color for that dataset with the lower number will
  be undefined. If you do not want any color, pass an invalid color
  (i.e. a default-constructed QColor object). This is only useful
  for chart types that have an outline (like bars or pies),
  because you would not see anything otherwise.<br>
  In charts that can only display one dataset (like pies), the specified
  colors are used for the different values instead.

  \param dataset the number of the dataset (or value for pies and similar
  charts) for which the color is specified
  \param color the color to use for this dataset/value
  \sa setDataDefaultColors, setDataRainbowColors, setDataSubduedColors
  \sa dataColor, maxDataColor
  */
void KDChartParams::setDataColor( uint dataset, QColor color )
{
    QColor shadow1;
    QColor shadow2;
    calculateShadowColors( color, shadow1, shadow2 );
    _dataColors[        dataset ] = color;
    _dataColorsShadow1[ dataset ] = shadow1;
    _dataColorsShadow2[ dataset ] = shadow2;
    _maxDatasetColor = QMAX( _maxDatasetColor, dataset );

    emit changed();
}

/**
  Specifies KD Chart's default colors for the datasets 0..11.

  This method may be usefull to re-set the colors after changing the
  colors of the datasets.

  \sa setDataColor, setDataRainbowColors, setDataSubduedColors
  \sa dataColor, maxDataColor
  */
void KDChartParams::setDataDefaultColors()
{
    setDataColor(  0, red );
    setDataColor(  1, green );
    setDataColor(  2, blue );
    setDataColor(  3, cyan );
    setDataColor(  4, magenta );
    setDataColor(  5, yellow );
    setDataColor(  6, darkRed );
    setDataColor(  7, darkGreen );
    setDataColor(  8, darkBlue );
    setDataColor(  9, darkCyan );
    setDataColor( 10, darkMagenta );
    setDataColor( 11, darkYellow );
}

/**
  Specifies a set of eight rainbow-like colors for the datasets 0..7
  and sets the lighter versions of this colors to datasets 8..15.

  This is a convenience method setting colors that look best on
  a dark gray or black background.

  \sa setDataColor, setDataDefaultColors, setDataSubduedColors
  \sa dataColor, maxDataColor
  */
void KDChartParams::setDataRainbowColors()
{
    setDataColor(  0, QColor(255,  0,196) );
    setDataColor(  1, QColor(255,  0, 96) );
    setDataColor(  2, QColor(255, 128,64) );
    setDataColor(  3, Qt::yellow );
    setDataColor(  4, Qt::green );
    setDataColor(  5, Qt::cyan );
    setDataColor(  6, QColor( 96, 96,255) );
    setDataColor(  7, QColor(160,  0,255) );
    for( int i=8; i<16; ++i )
        setDataColor( i, dataColor(i-8).light() );
}

/**
  Specifies a set of eighteen subdued colors for the datasets 0..17.

  This is a convenience method setting colors that is ideal for
  area charts since the dark data value texts can be easily
  read when printed onto such background colors.

  \param ordered if true all eighteen colors will be set according
  to their appearance in the color circle, if false they will be
  arranged in a logical order making it very easy ti distinguish
  two neighboring colors. Set this parameter to true if you want
  your neighboring colors look quite similiar, the default is false.

  \sa setDataColor, setDataDefaultColors, setDataRainbowColors
  \sa dataColor, maxDataColor
  */
void KDChartParams::setDataSubduedColors( bool ordered )
{
static const int NUM_SUBDUEDCOLORS = 18;
static const QColor SUBDUEDCOLORS[ NUM_SUBDUEDCOLORS ] = {
    QColor( 0xe0,0x7f,0x70 ),
    QColor( 0xe2,0xa5,0x6f ),
    QColor( 0xe0,0xc9,0x70 ),
    QColor( 0xd1,0xe0,0x70 ),
    QColor( 0xac,0xe0,0x70 ),
    QColor( 0x86,0xe0,0x70 ),
    QColor( 0x70,0xe0,0x7f ),
    QColor( 0x70,0xe0,0xa4 ),
    QColor( 0x70,0xe0,0xc9 ),
    QColor( 0x70,0xd1,0xe0 ),
    QColor( 0x70,0xac,0xe0 ),
    QColor( 0x70,0x86,0xe0 ),
    QColor( 0x7f,0x70,0xe0 ),
    QColor( 0xa4,0x70,0xe0 ),
    QColor( 0xc9,0x70,0xe0 ),
    QColor( 0xe0,0x70,0xd1 ),
    QColor( 0xe0,0x70,0xac ),
    QColor( 0xe0,0x70,0x86 ),
};
    if( ordered )
        for(int i=0; i<NUM_SUBDUEDCOLORS; ++i)
            setDataColor( i, SUBDUEDCOLORS[i] );
    else{
        setDataColor( 0, SUBDUEDCOLORS[ 0] );
        setDataColor( 1, SUBDUEDCOLORS[ 5] );
        setDataColor( 2, SUBDUEDCOLORS[10] );
        setDataColor( 3, SUBDUEDCOLORS[15] );
        setDataColor( 4, SUBDUEDCOLORS[ 2] );
        setDataColor( 5, SUBDUEDCOLORS[ 7] );
        setDataColor( 6, SUBDUEDCOLORS[12] );
        setDataColor( 7, SUBDUEDCOLORS[17] );
        setDataColor( 8, SUBDUEDCOLORS[ 4] );
        setDataColor( 9, SUBDUEDCOLORS[ 9] );
        setDataColor(10, SUBDUEDCOLORS[14] );
        setDataColor(11, SUBDUEDCOLORS[ 1] );
        setDataColor(12, SUBDUEDCOLORS[ 6] );
        setDataColor(13, SUBDUEDCOLORS[11] );
        setDataColor(14, SUBDUEDCOLORS[16] );
        setDataColor(15, SUBDUEDCOLORS[ 3] );
        setDataColor(16, SUBDUEDCOLORS[ 8] );
        setDataColor(17, SUBDUEDCOLORS[13] );
    }
}


void KDChartParams::calculateShadowColors( QColor color,
                                           QColor& shadow1,
                                           QColor& shadow2 ) const
{
    if ( !color.isValid() ) { // no fill color
        shadow1 = QColor();
        shadow2 = QColor();
    } else {
        int hue, saturation, value;
        color.hsv( &hue, &saturation, &value );
        double v = value;
        v = v * 2.0 / 3.0 * shadowBrightnessFactor();
        if ( 255.0 < v )
            v = 255.0;
        else if ( 1.0 > v )
            v = 0.0;
        shadow1.setHsv( hue, saturation, static_cast < int > ( v ) );
        v = value;
        v = v / 3.0 * shadowBrightnessFactor();
        if ( 255.0 < v )
            v = 255.0;
        else if ( 1.0 > v )
            v = 0.0;
        shadow2.setHsv( hue, saturation, static_cast < int > ( v ) );
    }
}


/**
  Returns the color for a dataset. If no color has been defined for this
  dataset and none for a higher dataset either, the number will be mapped
  to the range of defined colors. If no color has been defined for this
  dataset, but at least one for a higher dataset, the return value of
  this method is undefined.

  \param dataset the number of the dataset for which to return the color
  \return the color for this dataset
  \sa setDataColor, maxDataColor
  */
QColor KDChartParams::dataColor( uint dataset ) const
{
    uint index = dataset % (_maxDatasetColor+1);
    if( _dataColors.find( index ) != _dataColors.end() )
        return _dataColors[ index ];
    else
        return QColor(); // documentation says undefined
}


QString KDChartParams::dataRegionFrameAreaName( uint dataRow,
                             uint dataCol,
                             uint data3rd )
{
    return QString( "%1/%2/%3/%4" )
            .arg( KDChartEnums::AreaChartDataRegion, 5 )
            .arg( dataRow, 5 )
            .arg( dataCol, 5 )
            .arg( data3rd, 5 );
}


/**
  Recomputes the shadow colors by iterating over all configured
  data colors and reassigning the data colors with exactly the
  same values which in turn triggers computation of the shadow
  colors. Expensive if many data colors are set, but performance
  is OK for typical cases.
  */
void KDChartParams::recomputeShadowColors()
{
    // Simply reassign the available colors; this will trigger
    // recomputation of the shadow colors.
    for( QMap<uint,QColor>::Iterator it = _dataColors.begin();
            it != _dataColors.end(); ++it ) {
        setDataColor( it.key(), it.data() );
    }
}




/**
  Returns the first shadow color for a dataset. This is the color that
  is used to draw the top bars with 3D effects. It is somewhat
  darker than the original data color.  If no color has been defined for this
  dataset and none for a higher dataset either, the number will be mapped
  to the range of defined colors. If no color has been defined for this
  dataset, but at least one for a higher dataset, the return value of
  this method is undefined.

  \param dataset the number of the dataset for which to return the color
  \return the color for this dataset
  \sa setDataColor, maxDataColor, dataShadow2Color
  */
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


/**
  Returns the second shadow color for a dataset. This is the color that
  is used to draw the sides of bars with 3D effects. It is
  darker than the original data color.  If no color has been defined for this
  dataset and none for a higher dataset either, the number will be mapped
  to the range of defined colors. If no color has been defined for this
  dataset, but at least one for a higher dataset, the return value of
  this method is undefined.

  \param dataset the number of the dataset for which to return the color
  \return the color for this dataset
  \sa setDataColor, maxDataColor, dataShadow1Color
  */
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


/**
  Set the default axis types for all the axes the user might activate for this chart:
  \li Pie charts by default have no axes at all
  \li Bar/Line/Area charts may have up to 8 axes (up to 2 at each side of the chart)
  \li charts representing 3-dimensional data may have up to 12 axes

  \note This function also specifies the default way how to calculate
  the axis labels: abscissa starts with zero and counts by 1, ordinate
  is calculating the labels automatically based upon the values found in
  the associated dataset(s).
  */
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
                    KDCHART_AXIS_LABELS_AUTO_LIMIT,
                    1.0, 0 );
            setAxisLabelTextParams( KDChartAxisParams::AxisPosTop, false,
                    1.0,
                    KDCHART_AXIS_LABELS_AUTO_LIMIT,
                    1.0, 0 );
            setAxisLabelTextParams( KDChartAxisParams::AxisPosBottom2, false,
                    1.0,
                    KDCHART_AXIS_LABELS_AUTO_LIMIT,
                    1.0, 0 );
            setAxisLabelTextParams( KDChartAxisParams::AxisPosTop2, false,
                    1.0,
                    KDCHART_AXIS_LABELS_AUTO_LIMIT,
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
            1.0, KDCHART_AXIS_LABELS_AUTO_LIMIT, 1.0, 0 );
            setAxisLabelTextParams( KDChartAxisParams::AxisPosTop,     false,
            1.0, KDCHART_AXIS_LABELS_AUTO_LIMIT, 1.0, 0 );
            setAxisLabelTextParams( KDChartAxisParams::AxisPosLowerRightEdge,
            false,
            1.0, KDCHART_AXIS_LABELS_AUTO_LIMIT, 1.0, 0 );
            setAxisLabelTextParams( KDChartAxisParams::AxisPosLowerLeftEdge,
            false,
            1.0, KDCHART_AXIS_LABELS_AUTO_LIMIT, 1.0, 0 );
            setAxisLabelTextParams( KDChartAxisParams::AxisPosBottom2, false,
            1.0, KDCHART_AXIS_LABELS_AUTO_LIMIT, 1.0, 0 );
            setAxisLabelTextParams( KDChartAxisParams::AxisPosTop2,    false,
            1.0, KDCHART_AXIS_LABELS_AUTO_LIMIT, 1.0, 0 );
            setAxisLabelTextParams(KDChartAxisParams::AxisPosLowerRightEdge2,
            false,
            1.0, KDCHART_AXIS_LABELS_AUTO_LIMIT, 1.0, 0 );
            setAxisLabelTextParams( KDChartAxisParams::AxisPosLowerLeftEdge2,
            false,
            1.0, KDCHART_AXIS_LABELS_AUTO_LIMIT, 1.0, 0 );

            false,
            1.0, KDCHART_AXIS_LABELS_AUTO_LIMIT, 1.0, 0 );

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
            setAxisLabelsFont( KDChartAxisParams::AxisPosSaggital,
                               QFont( "helvetica", 1, QFont::Bold ),
                               -30,
                               Qt::darkBlue );
            setAxisLabelsVisible( KDChartAxisParams::AxisPosCircular, true );
            setAxisLabelsFont( KDChartAxisParams::AxisPosCircular,
                               QFont( "helvetica", 1, QFont::Bold ),
                               -22,
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


/**
  activate the default axis types for this chart:
  \li Pie charts by default have no axes at all
  \li Bar/Line/Area by default have 2 axes: the abscissa and the ordinate
  \li charts representing 3-dimensional data by default have 3 axes
  */
void KDChartParams::activateDefaultAxes()
{
    // deactivate all axes: specifying KDCHART_NO_DATASET will
    //                      also remove their visability flag
    for ( uint i = 0; i < KDCHART_MAX_AXES; ++i )
        setAxisDatasets( i, KDCHART_NO_DATASET );

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








/**
  \fn KDChartParams::setOptimizeOutputForScreen(bool)

  Specify whether drawing should be optimized for (low resolution)
  screen output or whether other routines should be used to obtain
  best quality when using a printer.

  \note The default of this flag is TRUE, so make sure to deactivate
  it before painting your KDChart onto a QPrinter device!

  \note This setting is NOT stored together with the other parameters,
  it is recommended to set the flag each time when the drawing targed
  (printer or screen, resp) is changed.

  \sa optimizeOutputForScreen
  */


/**
  \fn bool KDChartParams::optimizeOutputForScreen() const
  Returns whether drawing is optimized for (low resolution)
  screen output (this is the default) or whether other routines
  are used to obtain best quality when using a printer.

  \sa setOptimizeOutputForScreen
  */


/**
  Specifies the distance between the chart and the left border of the
  painter area.  If \c leading is a negative value it is interpreted
  as per-mille value of the painter area, the true offset will then
  be calculated dynamically at drawing time.

  \sa setGlobalLeading, setGlobalLeadingTop, setGlobalLeadingRight,
  setGlobalLeadingBottom
  \sa globalLeadingLeft, globalLeadingTop, globalLeadingRight,
  globalLeadingBottom
  */
void KDChartParams::setGlobalLeading( int left, int top, int right, int bottom )
{
    _globalLeadingLeft   = left;
    _globalLeadingTop    = top;
    _globalLeadingRight  = right;
    _globalLeadingBottom = bottom;
    emit changed();
}


/**
  \fn void KDChartParams::setGlobalLeadingLeft( int )

  Specifies the distance between the chart and the left border of the
  painter area.  If \c leading is a negative value it is interpreted
  as per-mille value of the painter area, the true offset will then
  be calculated dynamically at drawing time.

  \sa setGlobalLeading, setGlobalLeadingTop, setGlobalLeadingRight,
  setGlobalLeadingBottom
  \sa globalLeadingLeft, globalLeadingTop, globalLeadingRight,
  globalLeadingBottom
  */


/**
  \fn void KDChartParams::setGlobalLeadingTop( int )

  Specifies the distance between the chart and the upper border of
  the painter area. If \c leading is a negative value it is
  interpreted as per-mille value of the painter area, the true offset
  will then be calculated dynamically at drawing time.

  \sa setGlobalLeading, setGlobalLeadingLeft, setGlobalLeadingRight,
  setGlobalLeadingBottom
  \sa globalLeadingLeft, globalLeadingTop, globalLeadingRight,
  globalLeadingBottom
  */


/**
  \fn void KDChartParams::setGlobalLeadingRight( int )

  Specifies the distance between the chart and the right border
  of the painter area.  If \c leading is a negative value it is
  interpreted as per-mille value of the painter area, the true
  offset will then be calculated dynamically at drawing time.

  \sa setGlobalLeading, setGlobalLeadingLeft, setGlobalLeadingTop,
  setGlobalLeadingBottom
  \sa globalLeadingLeft, globalLeadingTop, globalLeadingRight,
  globalLeadingBottom
  */


/**
  \fn void KDChartParams::setGlobalLeadingBottom( int )

  Specifies the distance between the chart and the lower border of
  the painter area.  If \c leading is a negative value it is
  interpreted as per-mille value of the painter area, the true offset
  will then be calculated dynamically at drawing time.

  \sa setGlobalLeading, setGlobalLeadingLeft, setGlobalLeadingTop,
  setGlobalLeadingRight
  \sa globalLeadingLeft, globalLeadingTop, globalLeadingRight,
  globalLeadingBottom
  */


/**
  \fn int KDChartParams::globalLeadingLeft() const

  Return the distance between the chart and the left border of the
  painter area.  If \c leading is a negative value it is interpreted
  as per-mille value of the painter area, the true offset will then
  be calculated dynamically at drawing time.

  \sa setGlobalLeading, setGlobalLeadingLeft, setGlobalLeadingTop,
  setGlobalLeadingRight, setGlobalLeadingBottom
  \sa globalLeadingTop, globalLeadingRight, globalLeadingBottom
  */


/**
  \fn KDChartParams::globalLeadingTop() const

  Return the distance between the chart and the upper border of the
  painter area.  If \c leading is a negative value it is interpreted
  as per-mille value of the painter area, the true offset will then
  be calculated dynamically at drawing time.

  \sa setGlobalLeading, setGlobalLeadingLeft, setGlobalLeadingTop,
  setGlobalLeadingRight, setGlobalLeadingBottom
  \sa globalLeadingLeft, globalLeadingRight, globalLeadingBottom
  */


/**
  \fn KDChartParams::globalLeadingRight() const

  Return the distance between the chart and the right border of the
  painter area.  If \c leading is a negative value it is interpreted
  as per-mille value of the painter area, the true offset will then
  be calculated dynamically at drawing time.

  \sa setGlobalLeading, setGlobalLeadingLeft, setGlobalLeadingTop,
  setGlobalLeadingRight, setGlobalLeadingBottom
  \sa globalLeadingLeft, globalLeadingTop, globalLeadingBottom
  */


/**
  \fn int KDChartParams::globalLeadingBottom() const

  Return the distance between the chart and the lower border of the
  painter area.  If \c leading is a negative value it is interpreted
  as per-mille value of the painter area, the true offset will then
  be calculated dynamically at drawing time.

  \sa setGlobalLeading, setGlobalLeadingLeft, setGlobalLeadingTop,
  setGlobalLeadingRight, setGlobalLeadingBottom
  \sa globalLeadingLeft, globalLeadingTop, globalLeadingRight
  */


/**
  \fn void KDChartParams::setFrame( uint, const KDFrame&, int, int,
  int, int, bool, bool )

  \ifnot v200
  \deprecated Feature scheduled for future release, at present not implemented.
  \else
  Specify the frame settings to be used for one of the chart areas.

  The names and the meaning of the areas are explained with the enum
  AreaName.  The KDFrame class is explained in detail here: KDFrame
  <b>Note however:</b> The \c innerRect settings of this KDFrame will
  be ignored since position and size of this frame will be calculated
  dynamically based upon the AreaName!

  \param area The area to be surrounded by a frame.
  \param outerGap The distance between the frame and the surrounding
  parts of the chart.
  \param innerGap The distance between the frame and the inner area.
  \param addFrameWidthToLayout Specifies whether the inner area will
  shrink the area AND its frame will occupy the same space of the
  chart as the area would occupy if no frame were drawn. If false,
  the frame is drawn around the area without taking care not to
  override other content of the chart.

  \note An easier way to specify a frame setting is selecting a pre-defined
  setting using the setSimpleFrame methode.

  \sa setSimpleFrame, frameSettings
  \endif
  */

/**
  \fn void KDChartParams::setSimpleFrame( uint, int, int, int, int,
  bool, bool, KDFrame::SimpleFrame, int, int, QPen, QBrush, const
  QPixmap*, KDFrame::BackPixmapMode, int, KDFrame::CornerName )

  Select a pre-defined frame setting to be used for one of the chart areas.

  \param area The area to be surrounded by a frame.
  \param outerGap The distance between the frame and the surrounding
  parts of the chart.
  \param innerGap The distance between the frame and the inner area.
  \if v200
  \param addFrameWidthToLayout Specifies whether the inner area will
  shrink the area AND its frame will occupy the same space of the
  chart as the area would occupy if no frame were drawn. If false,
  the frame is drawn around the area without taking care not to
  override other content of the chart.

  The parameters specifying the pre-defined frame settings are
  defined here: KDFrame::setSimpleFrame.
  \sa setFrame, frameSettings
  \else
  \param addFrameWidthToLayout For internal use - must be \c true.
  \param addFrameHeightToLayout For internal use - must be \c true.
  \param simpleFrame For internal use - must be \c KDFrame::FrameFlat.
  \param lineWidth For internal use - must be \c 1.
  \param midLineWidth For internal use - must be \c 0.
  \param pen The pen to be used for drawing the four frame border lines.
  \param background The brush to be used for painting the frame background.
  \param backPixmap The picture to be displayed as background image
  of the frame.
  \param backPixmapMode The way how backPixmap is to be modified to
  fill the frame, these are possible:
  \li \c KDFrame::PixCentered Image will not be scaled nor stretched
  but shown in original size.
  \li \c KDFrame::PixScaled Image will be scaled until it's height or
  it's width match the frame's height (or width, resp.) - whichever
  comes first - but image proportions will be preserved.
  \li \c KDFrame::PixStretched Image will be stretched to have the
  same height and the same width as the frame - normally this results
  in changing the original image proportions.
  \param shadowWidth For internal use - do not set this parameter or
  set it to \c 0.
  \param sunPos For internal use - do not set this parameter or set
  it to \c KDFrame::CornerTopLeft.
  \endif
  */


/**
  \fn void KDChartParams::setDataRegionFrame( uint, uint, uint, int, int, int, int,
  bool, bool, KDFrame::SimpleFrame, int, int, QPen, QBrush, const
  QPixmap*, KDFrame::BackPixmapMode, int, KDFrame::CornerName )

  Select a pre-defined frame setting to be used for one of the data representations
  (e.g. to have the frame surrounding one of the bars in a Bar chart).

  \note KDChart currently supports specifying data frames without background only,
        it is not possible to draw a special background behind a data representation.

  \param dataRow The table row of the cell to be surrounded by a frame.
  \param dataCol The table column of the cell.
  \param data3rd The third table coordinate of the cell (parameter not used, its value will be ignored, set to 0, reserved for future use).
  \param innerGap The distance between the frame and the inner area.
  \if v200
  \param addFrameWidthToLayout Specifies whether the inner area will
  shrink the area AND its frame will occupy the same space of the
  chart as the area would occupy if no frame were drawn. If false,
  the frame is drawn around the area without taking care not to
  override other content of the chart.

  The parameters specifying the pre-defined frame settings are
  defined here: KDFrame::setSimpleFrame.
  \sa setFrame, frameSettings
  \else
  \param addFrameWidthToLayout For internal use - must be \c true.
  \param addFrameHeightToLayout For internal use - must be \c true.
  \param simpleFrame For internal use - must be \c KDFrame::FrameFlat.
  \param lineWidth For internal use - must be \c 1.
  \param midLineWidth For internal use - must be \c 0.
  \param pen The pen to be used for drawing the four frame border lines.
  \param shadowWidth For internal use - do not set this parameter or
  set it to \c 0.
  \param sunPos For internal use - do not set this parameter or set
  it to \c KDFrame::CornerTopLeft.
  \endif
  */


/**
  \fn bool KDChartParams::moveDataRegionFrame( uint, uint, uint, uint, uint, uint )

  Move a frame that was previously specified using setDataRegionFrame
  to another location: moves the frame from cell[ oldDataRow, oldDataCol ]
  to cell[ newDataRow, newDataCol ] without changing looking of the frame,
  or just removes the frame entirely.

  \param oldDataRow The table row of the frame to be removed.
  \param oldDataCol The table column of the frame to be removed.
  \param oldData3rd The third table coordinate of the old cell (parameter not used, its value will be ignored, set to 0, reserved for future use).
  \param newDataRow The table row of the cell to be framed instead, or KDCHART_NO_DATASET if the frame is to be removed
  without framing another cell then.
  \param newDataCol The table column of the cell to be framed instead.
  \param newData3rd The third table coordinate of the new cell (parameter not used, its value will be ignored, set to 0, reserved for future use).

  \note Using KDCHART_NO_DATASET for the newDataRow parameter will not
  result in returning FALSE because it is an allowed action: the frame is just removed then.

  \return TRUE if the frame could be moved or was removed; FALSE if either there was no frame around the old cell or the target cell does not exist.
  */
bool KDChartParams::moveDataRegionFrame( uint oldDataRow,
                             uint oldDataCol,
                             uint, // important: we ignore the data3rd parameter for now!
                             uint newDataRow,
                             uint newDataCol,
                             uint// important: we ignore the data3rd parameter for now!
                             )
{
    const QString oldKey( dataRegionFrameAreaName( oldDataRow, oldDataCol, 0 ) ); // oldData3rd ) );
    KDChartFrameSettings* it = _areaDict.find( oldKey );
    bool bFound = ( it != 0 );
    if( bFound ){
        if( KDCHART_NO_DATASET != newDataRow ){
            KDChartFrameSettings* frame = new KDChartFrameSettings;
            KDChartFrameSettings::deepCopy( *frame, *it );
            frame->setDataRow( newDataRow );
            frame->setDataCol( newDataCol );
            frame->setData3rd( 0 ); // newData3rd );
            _areaDict.setAutoDelete( TRUE );
            _areaDict.replace(
                dataRegionFrameAreaName( newDataRow, newDataCol, 0 ), //data3rd 5 ),
                frame );
        }
        _areaDict.remove( oldKey );
        emit changed();
    }
    return bFound;
}

/**
  \var KDChartParams::CustomBoxDict

  The type that stores the custom boxes in a chart.
  */

/**
  Return the frame settings of one of the chart areas.

  \sa nextFrameSettings, setSimpleFrame, setFrame
  */
const KDChartParams::KDChartFrameSettings* KDChartParams::frameSettings( uint area,
                                                                         bool& bFound,
                                                                         int* pIterIdx ) const
{
    if( pIterIdx )
        *pIterIdx = 0;
    const QString key( QString( "%1/-----/-----/-----" ).arg( area, 5 ) );
    KDChartFrameSettings* it = _areaDict.find( key );
    bFound = ( it != 0 );
    if( bFound )
        return it;
    else if( pIterIdx ){
        QString keyStart( key.left(6) );
        QDictIterator<KDChartFrameSettings> it2( _areaDict );
        for( ; it2.current(); ++it2 ){
            if( it2.currentKey().startsWith( keyStart ) ){
                bFound = true;
                return it2.current();
            }
            ++*pIterIdx;
        }
    }
    return &_noFrameSettings;
}


/**
  Return the next frame settings specified for the same area type
  as the frame that was retrieved by previous calls of \c frameSettings()
  or \c nextFrameSettings().

  \sa setDataRegionFrame, frameSettings
  */
const KDChartParams::KDChartFrameSettings* KDChartParams::nextFrameSettings( bool& bFound,
                                                                             int* pIterIdx ) const
{
    bFound = false;
    if( pIterIdx ){
        int i=0;
        QDictIterator<KDChartFrameSettings> it( _areaDict );
        for( ; it.current(); ++it ){
            if( *pIterIdx == i )
                break;
            ++i;
        }
        if( *pIterIdx == i ){
            QString keyStart( it.currentKey().left(6) );
            ++it;
            for( ; it.current(); ++it ){
                ++*pIterIdx;
                if( it.currentKey().startsWith( keyStart ) ){
                    bFound = true;
                    return it.current();
                }
            }
        }
    }
    return &_noFrameSettings;
}


/**
  Add a new custom box to the list of boxes.
  To remove this box from the list lateron just call removeCustomBox
  with the index that was returned by insertCustomBox.

  \sa removeCustomBox, removeAllCustomBoxes, customBox, maxCustomBoxIdx
  */
uint KDChartParams::insertCustomBox( const KDChartCustomBox & box )
{
    _customBoxDict.setAutoDelete( true );
    uint newIdx;
    if( _customBoxDictMayContainHoles ){
        _customBoxDictMayContainHoles = false;
        const uint maxIndex = maxCustomBoxIdx();
        newIdx = 1 + maxIndex;
        for( uint idx = 0; idx <= maxIndex; ++idx ) {
            if( ! _customBoxDict.find( idx ) ) {
                newIdx = idx;
                _customBoxDictMayContainHoles = true; // we found a hole, so there might be more of them
                break;
            }
        }
    }else{
        newIdx = _customBoxDict.count();
    }
    _customBoxDict.insert( newIdx, box.clone() );
    emit changed();
    return newIdx;
}


/**
  Remove a custom box from the list of boxes.
  Please fill in the index parameter with the value
  that was returned by insertCustomBox.

  \sa removeAllCustomBoxes, insertCustomBox, customBox, maxCustomBoxIdx
  */
bool KDChartParams::removeCustomBox( const uint & idx )
{
    const bool bFound = _customBoxDict.remove( idx );
    _customBoxDictMayContainHoles = true;
    emit changed();
    return bFound;
}


/**
  \fn void KDChartParams::removeAllCustomBoxes()
  Remove all custom boxes from the list of boxes.

  \sa removeCustomBox, insertCustomBox, customBox, maxCustomBoxIdx
  */


/**
  Retrieve a const pointer to a custom box from the list.
  You may fill in the index parameter with the value
  that was returned by insertCustomBox, or you may use a numerical value,
  e.g. when iterating from zero up to maxCustomBoxIdx().

  \return Zero if the custom box has been removed by calling removeCustomBox.

  \sa insertCustomBox customBoxRef, removeCustomBox, removeAllCustomBoxes, maxCustomBoxIdx
  */
const KDChartCustomBox* KDChartParams::customBox( uint box ) const
{
    return _customBoxDict.find( box );
}

/**
  Retrieve a non-const pointer to a  custom box from the list.
  You may fill in the index parameter with the value
  that was returned by insertCustomBox, or you may use a numerical value,
  e.g. when iterating from zero up to maxCustomBoxIdx().

  \note The reference returned by this method may be used to directly
  modify the properties of the respective box.

  \return Zero if the custom box has been removed by calling removeCustomBox.

  \sa insertCustomBox customBox, removeCustomBox, removeAllCustomBoxes, maxCustomBoxIdx
  */
KDChartCustomBox* KDChartParams::customBoxRef( uint box )
{
    return _customBoxDict.find( box );
}

/**
  Retrieve the biggest custom boxes ID used.

  \sa insertCustomBox, removeCustomBox, removeAllCustomBoxes, customBox
  */
uint KDChartParams::maxCustomBoxIdx() const
{
    uint cnt( _customBoxDict.count() );
    if( cnt ) {
        int maxIndex = cnt-1;
        QIntDictIterator<KDChartCustomBox> it( _customBoxDict );
        for( ; it.current(); ++it )
            if( it.currentKey() > maxIndex )
                maxIndex = it.currentKey();
        return maxIndex;
    }
    return 0;
}

/**
  \enum KDChartParams::ChartType

  The chart type. Covers only built-in chart types.

  \sa setChartType, chartType
  \sa setAdditionalChartType, additionalChartType,
  \sa setChartSourceMode
  */


/**
  Specifies the chart type. The default is bar charts (Bar).

  \note Allways call this <b>before</b> specifying other
  axis properties and before calling \c setAdditionalChartType().<br>
  \c setChartType() deactivates the right axis and also sets the
  axisLabelsTouchEdges back to their default for all bottom and
  top axes.

  \note Calling setChartType() results in changing the default data value
  texts font settings (i.e. the font size, colour, position, but not the
  QFont itself) <b>if</b> these parameters were not changed by
  setPrintDataValues().
  In order to re-activate this automatical adjustment of font settings
  even after you have changed them manually you may call
  setPrintDataValuesWithDefaultFontParams().
  \li When calling setPrintDataValuesWithDefaultFontParams()
  <b>before</b> calling setChartType() make sure you set the
  \c callSetPrintDataValues parameter to \c false.
  \li When calling setPrintDataValuesWithDefaultFontParams()
  <b>after</b> calling setChartType() you may set the \c
  callSetPrintDataValues parameter to \c true <b>or</b> you may call
  setPrintDataValues() yourself after calling
  setPrintDataValuesWithDefaultFontParams() to specify additional
  parameters.

  \param chartType the chart type to use
  \sa chartType, ChartType
  \sa setAdditionalChartType, additionalChartType,
  \sa setBarChartSubType, barChartSubType
  \sa setLineChartSubType, lineChartSubType
  \sa setPolarChartSubType, polarChartSubType
  \sa setChartSourceMode, chartSourceMode
  \sa setAxisLabelsTouchEdges
  */
void KDChartParams::setChartType( ChartType chartType )
{
    _chartType = chartType;

    // de-activate 2nd axis that might have been set automatically
    // by a previous call of \c setAdditionalChartType()
    setAxisVisible( KDChartAxisParams::AxisPosRight, false );

    // Make sure abscissa axes start their labeling at the very
    // first position and end at the last position when drawing
    // area charts.
    // Bar charts and line charts look better with their abscissa labels
    // in the respective middle positions below each bar (or point, resp.)
    bool bAbscissaAxisLabelsTouchEdges = ( Area == chartType );
    setAxisLabelsTouchEdges( KDChartAxisParams::AxisPosBottom,
            bAbscissaAxisLabelsTouchEdges );
    setAxisLabelsTouchEdges( KDChartAxisParams::AxisPosTop,
            bAbscissaAxisLabelsTouchEdges );
    setAxisLabelsTouchEdges( KDChartAxisParams::AxisPosBottom2,
            bAbscissaAxisLabelsTouchEdges );
    setAxisLabelsTouchEdges( KDChartAxisParams::AxisPosTop2,
            bAbscissaAxisLabelsTouchEdges );
    // activate default data value text settings for this chart type
    if (    printDataValues( 0 )
         && printDataValuesWithDefaultFontParams( 0 ) )
        setPrintDataValues( true, 0 );
    emit changed();
}


/**
  \fn KDChartParams::ChartType KDChartParams::chartType() const

  Returns the chart type configured in this params object.

  \return the chart type configured in this params object.
  \sa setChartType, ChartType
  \sa setAdditionalChartType, additionalChartType,
  \sa setBarChartSubType, barChartSubType
  \sa setLineChartSubType, lineChartSubType
  \sa setPolarChartSubType, polarChartSubType
  \sa setChartSourceMode, chartSourceMode
  */



/**
  Specifies the additional chart type.
  The default is no additional chart (NoType).
  The additional chart will have <b>no grid</b> so either make sure you
  specify the chart that should have a grid via \c setChartType() and
  the other one as additional chart or specify the grid manually if you
  need one for the additional chart too.

  The additional chart uses the same abscissa axis as the main chart,
  but cannot change the abscissa settings: the abscissa range and step width
  is determined according to the main chart's values.

  You may combine all different types of cartesian charts
  (== Bar, Line, HiLo, ..., but not Polar, Pie, ...)
  so e.g. the following combinations are possible:<br>

  \verbatim
  setChartType( KDChartParams::Bar );
  setAdditionalChartType( KDChartParams::Line );
  \endverbatim
  or
  \verbatim
  setChartType( KDChartParams::Line );
  setAdditionalChartType( KDChartParams::HiLo );
  \endverbatim
  or
  \verbatim
  setChartType( KDChartParams::Bar );
  setAdditionalChartType( KDChartParams::Bar );
  ...
  \endverbatim

  \note When defining multiple charts it is mandatory to
  call \c setChartSourceMode() for specifying the dataset(s)
  and their respective charts after calling \c setAdditionalChartType(),
  also it is recommended to call \c setAxisDatasets() for all
  axes to be used by the different datasets - otherwise the
  default setting will be used (all datasets except the last
  one match the left axis, the last dataset matches the right axis),
  so e.g. your code to have a bar chart for datasets 0..2 (using the
  left axis) and a line chart for datasets 3..4 (using the right axis)
  could look like this:

  \verbatim
  setChartType( KDChartParams::Bar );
  setAdditionalChartType( KDChartParams::Line );
  setChartSourceMode( KDChartParams::DataEntry,     0,2, 0 );
  setChartSourceMode( KDChartParams::DataEntry,     3,4, 1 );
  setAxisDatasets( KDChartAxisParams::AxisPosLeft,  0,2, 0 );
  setAxisDatasets( KDChartAxisParams::AxisPosRight, 3,4, 1 );
  \endverbatim

  \note In case you DON'T want to use the right axis but the left axis #2
  make sure to reset the dataset assignment that is automatically done for
  the right axis. Doing this will also clear the visibility flag of the
  right axis that is automatically set by setAdditionalChartType()
  implicitely assuming that the additional chart will use the right axis,
  so there is no need to explicitely call setAxisVisible( false ) for
  the right axis: specifying KDCHART_NO_DATASET is enough to disable it.
  e.g. in the example shown above you would replace the
  last line by these ones:

  \verbatim
  setAxisDatasets( KDChartAxisParams::AxisPosLeft2, 3,4, 1 );
  setAxisDatasets( KDChartAxisParams::AxisPosRight,
  KDCHART_NO_DATASET );
  \endverbatim

  Often your additional chart will look better when not overlapping with
  the first chart, specifying different y-positions for the charts
  is possible by \c KDChartAxisParams::setAxisUseAvailableSpace(),
  e.g. like this:

\verbatim
// specify which amount of the available space
// is to be used by the first y-axis:

KDChartAxisParams pa(
        _p->axisParams( KDChartAxisParams::AxisPosLeft ) );
pa.setAxisUseAvailableSpace( 0, -499 );
_p->setAxisParams( KDChartAxisParams::AxisPosLeft, pa );

// specify which amount of the available space
// is to be used by the second y-axis:

pa = _p->axisParams( KDChartAxisParams::AxisPosLeft2 );
pa.setAxisUseAvailableSpace( -750, -1000 );
_p->setAxisParams( KDChartAxisParams::AxisPosLeft2, pa );
\endverbatim

\param chartType the additional chart type to use

\sa additionalChartType, setChartSourceMode, setAxisDatasets
\sa <br>chartType, ChartType
\sa setBarChartSubType, barChartSubType
\sa setLineChartSubType, lineChartSubType
    */
void KDChartParams::setAdditionalChartType( ChartType chartType )
{
    _additionalChartType = chartType;
    if ( KDChartParams::NoType == chartType ) {
        setAxisDatasets( KDChartAxisParams::AxisPosRight,
                KDCHART_NO_DATASET );
    } else {
        setAxisDatasets( KDChartAxisParams::AxisPosRight,
                KDCHART_ALL_DATASETS,
                KDCHART_ALL_DATASETS,
                1 );
        if(   printDataValues( 1 )
             && printDataValuesWithDefaultFontParams( 1 ) )
            setPrintDataValues( true, 1 );
    }
    emit changed();
}


/**
  \fn void KDChartParams::ChartType KDChartParams::additionalChartType() const

  Returns the additional chart type configured in this params object.

  \return the additional chart type configured in this params object.
  \sa setAdditionalChartType, setChartSourceMode, chartSourceMode
  \sa <br>setChartType, ChartType
  \sa setBarChartSubType, barChartSubType
  \sa setLineChartSubType, lineChartSubType
  */



/**
  \fn bool KDChartParams::neverUsedSetChartSourceMode() const

  Retrieves whether setChartSourceMode() was called or not
  */




/**
  \fn uint KDChartParams::maxDatasetSourceMode() const

  Returns the number of the highest dataset for which a SourceMode has been
  defined.

  \return the number of the highest dataset for which a SourceMode has been
  defined.
  \sa setChartSourceMode, chartSourceMode, findDataset
  */

/**
  \fn void KDChartParams::setNumValues( uint )

  Specifies how many of the values should be shown. -1 means all
  available values.

  \param numValues the number of values to be shown
  \sa numValues
  */

/**
  \fn int KDChartParams::numValues() const

  Returns how many of the values should be shown. -1 means all
  available values.

  \return the number of values to be shown
  \sa setNumValues
  */


/**
  \fn void KDChartParams::setShadowBrightnessFactor( double factor )

  Specifies a factor to be used to adjust the
  built-in brightness of shadow colors in
  3-dimensional drawings like e.g. 3D Bar charts.

  \param factor a factor that is to be taken into account for internal
  calculation of shadow colors. By specifying values greater 1.0 you
  may lighten the shadows until the most intensive brightness is
  reached, while values smaller than 1.0 will darken the shadows until
  the respective colors are black.

  \sa shadowBrightnessFactor
  */


/**
  \fn double KDChartParams::shadowBrightnessFactor() const

  Returns a factor to be used to adjust the
  built-in brightness of shadow colors in
  3-dimensional drawings like e.g. 3D Bar charts.

  \return a factor that is to be taken into account for internal
  calculation of shadow colors.

  \sa setShadowBrightnessFactor
  */


/**
  \fn void KDChartParams::setShadowPattern( Qt::BrushStyle style )

  Specifies a filling style for filling the shadow areas in
  3-dimensional drawings like 3D bar charts. The default is to
  fill with a solid color, the color is determined with \a
  setThreeDShadowColors.

  \param style the fill style to use
  \sa shadowPattern(), setThreeDShadowColors(),
  threeDShadowColors()
  */


/**
  \fn Qt::BrushStyle KDChartParams::shadowPattern() const

  Returns the filling style used for filling the shadow areas in
  3-dimensional drawings like 3D bar charts. The default is to
  fill with a solid color, the fill color can be queried with \a
  threeDShadowColors().

  \return the fill style used
  \sa setShadowPattern(), setThreeDShadowColors(),
  threeDShadowColors()
  */

/**
  \fn void KDChartParams::setOutlineDataColor( QColor color )

  Specifies a color for the outlines of data displays. The default is
  black.

  \note Use setOutlineDataLineStyle( Qt::NoPen ) to hide the line.

  \param color the color to use for the outlines
  \sa outlineDataColor, setOutlineDataLineStyle, setOutlineDataLineWidth
  */


/**
  \fn QColor KDChartParams::outlineDataColor() const

  Returns the color for the outlines of data displays.

  \return the outline color
  \sa setOutlineDataColor
  */


/**
  \fn void KDChartParams::setOutlineDataLineWidth( uint width )

  Specifies the width of the outlines of data displays. The default is 1
  pixel.

  \note Use setOutlineDataLineStyle( Qt::NoPen ) to hide the line.

  \param width the line width to use for the outlines
  \sa outlineDataLineWidth, setOutlineDataLineStyle, setOutlineDataColor
  */


/**
  \fn uint KDChartParams::outlineDataLineWidth() const

  Returns the width of the outlines of data displays.

  \param style the line width that is used for outlines
  \sa setOutlineDataLineWidth
  */


/**
  \fn void KDChartParams::setOutlineDataLineStyle( PenStyle style )

  Specifies the line style of the outlines of data displays. The default
  is a solid line. Warning: On Windows 95/98, the style setting (other
  than NoPen and SolidLine) has no effect for lines with width greater
  than 1, due to a bug in the operating system.

  \param width the line style to use for the outlines
  \sa outlineDataLineStyle, setOutlineDataColor, setOutlineDataLineWidth
  */


/**
  \fn PenStyle KDChartParams::outlineDataLineStyle() const

  Returns the style of the outlines of data displays.

  \param style the line style that is used for outlines
  \sa setOutlineDataLineStyle
  */


/**
  \fn uint KDChartParams::maxDataColor() const

  Returns the number of the highest dataset for which a color has been
  defined.

  \return the number of the highest dataset for which a color has been
  defined.
  \sa setDataColor, dataColor
  */


/**
  \fn void KDChartParams::setThreeDShadowColors( bool shadow )

  Specifies whether the engine should draw the sides and tops of
  3D effects in shadowed versions of the data colors (the
  default) or in the data colors themselves. Only used with 3D
  effects in charts that support these.

  \param shadow true for shadowed colors, false for original colors
  \sa threeDShadowColors
  */


/**
  \fn bool KDChartParams::threeDShadowColors() const

  Returns whether the engine should draw the sides and tops of 3D effects in
  shadowed versions of the data colors or in the data colors
  themselves. Only used with 3D effects in charts that
  support these. The default is true.

  \return true if shadowed colors are used for 3D effects
  \sa setThreeDShadowColors
  */


/**
  \fn void KDChartParams::setAllowOverlappingDataValueTexts( bool allow )

  Specifies whether a data value text may be drawn even if it's
  region intersects with another data value text's region.

  By default this is FALSE to prevent ugly mutual overwriting of
  data value texts and to speed up drawing of cahrts containing
  thousands of data points.
  */


/**
  \fn bool KDChartParams::allowOverlappingDataValueTexts() const

  Returns whether a data value text may be drawn even if it's region
  intersects with another data value text's region.

  By default this is FALSE to prevent ugly mutual overwriting of data
  value texts and to speed up drawing of cahrts containing thousands
  of data points.
  */


/**
  Specifies how the data value text figures are modified before printing.

  This methode is provided for your convenience, to learn how to set the
  other text parameters please have a look at setPrintDataValues.

  \sa printDataValuesWithDefaultFontParams, setPrintDataValues
  \sa dataValuesColor
  \sa setPrintDataValuesFontRelSize setPrintDataValuesBackground
  */
void KDChartParams::setDataValuesCalc( int divPow10,
                                       int digitsBehindComma,
                                       uint chart )
{
    uint count = (KDCHART_ALL_CHARTS == chart) ? 2 : 1;
    PrintDataValuesSettings * settings =    (( 1 < count ) || ( 0 == chart ))
        ? &_printDataValuesSettings
        : &_printDataValuesSettings2;
    for ( uint i = 0; i < count; ++i ) {
        settings->_divPow10 = divPow10;
        settings->_digitsBehindComma = digitsBehindComma;
        if ( 0 < chart )
            settings = &_printDataValuesSettings2;
    }
    emit changed();
}

/**
  Specifies the font to be used for printing the data value texts.

  This methode is provided for your convenience, to learn how to set the
  other text parameters please have a look at setPrintDataValues.

  \sa printDataValuesWithDefaultFontParams, setPrintDataValues
  \sa dataValuesColor
  \sa setPrintDataValuesFontRelSize setPrintDataValuesBackground
  */
void KDChartParams::setDataValuesFont( QFont* font,
                                       uint size,
                                       uint chart )
{
    uint count = (KDCHART_ALL_CHARTS == chart) ? 2 : 1;
    PrintDataValuesSettings * settings =    (( 1 < count ) || ( 0 == chart ))
        ? &_printDataValuesSettings
        : &_printDataValuesSettings2;
    for ( uint i = 0; i < count; ++i ) {
        settings->_useDefaultFontParams = false;
        if( font )
            settings->_dataValuesFont = *font;
        if( UINT_MAX != size ){
            settings->_dataValuesUseFontRelSize = ( 0 < size );
            settings->_dataValuesFontRelSize = size;
        }
        if ( 0 < chart )
            settings = &_printDataValuesSettings2;
    }
    emit changed();
}

/**
  Specifies the position and rotation of the data value texts to be printed.

  This methode is provided for your convenience, to learn how to set the
  other text parameters please have a look at setPrintDataValues.

  \sa printDataValuesWithDefaultFontParams, setPrintDataValues
  \sa dataValuesColor
  \sa setPrintDataValuesFontRelSize setPrintDataValuesBackground
  */
void KDChartParams::setDataValuesPlacing( KDChartEnums::PositionFlag position,
                                          uint align,
                                          int  deltaX,
                                          int  deltaY,
                                          int  rotation,
                                          bool specifyingPositiveValues,
                                          uint chart )
{
    uint count = (KDCHART_ALL_CHARTS == chart) ? 2 : 1;
    PrintDataValuesSettings * settings =    (( 1 < count ) || ( 0 == chart ))
        ? &_printDataValuesSettings
        : &_printDataValuesSettings2;
    for ( uint i = 0; i < count; ++i ) {
        if( specifyingPositiveValues ){
            // for values greater/equal zero:
            settings->_dataValuesAnchorPositivePosition = position;
            settings->_dataValuesAnchorPositiveAlign    = align;
            settings->_dataValuesAnchorPositiveDeltaX   = deltaX;
            settings->_dataValuesAnchorPositiveDeltaY   = deltaY;
            settings->_dataValuesPositiveRotation       = rotation;
        }else{
            // for values below zero:
            settings->_dataValuesAnchorNegativePosition = position;
            settings->_dataValuesAnchorNegativeAlign    = align;
            settings->_dataValuesAnchorNegativeDeltaX   = deltaX;
            settings->_dataValuesAnchorNegativeDeltaY   = deltaY;
            settings->_dataValuesNegativeRotation       = rotation;
        }
        if ( 0 < chart )
            settings = &_printDataValuesSettings2;
    }
    emit changed();
}

/**
  Specifies the text color and background brush to be used for
  printing the data value texts.

  To have the color calculated automatically - useful when printing
  inside the bars (or pie slices, areas, ... resp.) - please use
  \c KDCHART_DATA_VALUE_AUTO_COLOR instead of a QColor*.

  Setting the background is normally not needed since reasonable
  settings are done by default: Area charts have a white background
  behind the data value texts while all other chart types use no
  special background (the value is Qt::NoBrush then).

  In case you want to use it to remove the extra background from
  your Area chart's data value texts you should consider changing
  the dataset colors as well: an idea might be trying the subdued
  colorset by calling \c setSubduedColors().

  \sa printDataValuesWithDefaultFontParams
  \sa setPrintDataValues
  \sa dataValuesColor
  \sa setPrintDataValuesFontRelSize
  */
void KDChartParams::setDataValuesColors( const QColor* color,
                                         const QBrush& background,
                                         uint chart )
{
    // first store the color
    if( color )
        setPrintDataValuesColor( chart, color );
    // now store the background
    uint count = (KDCHART_ALL_CHARTS == chart) ? 2 : 1;
    PrintDataValuesSettings * settings =    (( 1 < count ) || ( 0 == chart ))
        ? &_printDataValuesSettings
        : &_printDataValuesSettings2;
    for ( uint i = 0; i < count; ++i ) {
        settings->_dataValuesBrush = background;
        if ( 0 < chart )
            settings = &_printDataValuesSettings2;
    }
    emit changed();
}


/* sorry, but policy handling is not implemnted yet: */
void KDChartParams::setDataValuesPolicy(
    KDChartEnums::TextLayoutPolicy policy,
    uint chart )
{
    uint count = (KDCHART_ALL_CHARTS == chart) ? 2 : 1;
    PrintDataValuesSettings * settings =    (( 1 < count ) || ( 0 == chart ))
        ? &_printDataValuesSettings
        : &_printDataValuesSettings2;
    for ( uint i = 0; i < count; ++i ) {
        settings->_dataValuesLayoutPolicy = policy;
        if ( 0 < chart )
            settings = &_printDataValuesSettings2;
    }
    emit changed();
}


/**
  Specifies whether data value texts should be printed even if the
  value is KDCHART_POS_INFINITE (or KDCHART_NEG_INFINITE).

  Printing of an infinite symbol (lemniskate) is done by default,
  ou may use this function to disable it.

  \sa printDataValuesWithDefaultFontParams
  \sa setPrintDataValues
  \sa dataValuesColor
  \sa setPrintDataValuesFontRelSize
  */
void KDChartParams::setDataValuesShowInfinite( bool dataValuesShowInfinite,
                                               uint chart)
{
    uint count = (KDCHART_ALL_CHARTS == chart) ? 2 : 1;
    PrintDataValuesSettings * settings =    (( 1 < count ) || ( 0 == chart ))
        ? &_printDataValuesSettings
        : &_printDataValuesSettings2;
    for ( uint i = 0; i < count; ++i ) {
        settings->_dataValuesShowInfinite = dataValuesShowInfinite;
        if ( 0 < chart )
            settings = &_printDataValuesSettings2;
    }
    emit changed();
}


/* function only there for backward compatibility */
void KDChartParams::setPrintDataValuesColor( uint chart, const QColor* color )
{
    uint count = (KDCHART_ALL_CHARTS == chart) ? 2 : 1;
    PrintDataValuesSettings * settings =    (( 1 < count ) || ( 0 == chart ))
        ? &_printDataValuesSettings
        : &_printDataValuesSettings2;
    for ( uint i = 0; i < count; ++i ) {
        if ( KDCHART_DATA_VALUE_AUTO_COLOR == color ) {
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
        if ( 0 < chart )
            settings = &_printDataValuesSettings2;
    }
    emit changed();
}


/**
  Specifies the dynamic font size to be used for printing the data
  value texts.  To change settings for all charts specify \c
  KDCHART_ALL_CHARTS as \chart parameter.

  This methode is provided for your convenience, to learn how to set the
  other text parameters please have a look at setPrintDataValues.

  \sa printDataValuesWithDefaultFontParams, setPrintDataValues
  \sa setPrintdataValuesColor, dataValuesFontRelSize
  */
void KDChartParams::setPrintDataValuesFontRelSize( uint chart, uint size )
{
    uint count = (KDCHART_ALL_CHARTS == chart) ? 2 : 1;
    PrintDataValuesSettings * settings =    (( 1 < count ) || ( 0 == chart ))
        ? &_printDataValuesSettings
        : &_printDataValuesSettings2;
    uint theSize( UINT_MAX == size ? 16 : size );
    for ( uint i = 0; i < count; ++i ) {
        settings->_dataValuesUseFontRelSize = ( 0 < theSize );
        settings->_dataValuesFontRelSize = theSize;
        if ( 0 < chart )
            settings = &_printDataValuesSettings2;
    }
    emit changed();
}


/**
  Specifies that data value texts are to be printed with default font
  parameters.  Calling this methode results in resetting the
  respective font size and colour and position parameters but not the
  QFont itself.  By setting \c callSetPrintDataValues to true you
  select general enabling of text printing, when doing so it is
  <b>not</b> neccessary to call setPrintDataValues() after calling
  setPrintDataValuesWithDefaultFontParams().

  \note If you want to call setChartType() after calling
  setPrintDataValuesWithDefaultFontParams() you should set the \c
  callSetPrintDataValues parameter to false to prevent
  setPrintDataValues() from being called twice since it is called
  internally each time you call setChartType() <b>if</b> the default
  font params are to be set.

  \sa printDataValuesWithDefaultFontParams, setPrintDataValues
  */
void KDChartParams::setPrintDataValuesWithDefaultFontParams( uint chart,
        bool callSetPrintDataValues )
{
    uint count = (KDCHART_ALL_CHARTS == chart) ? 2 : 1;
    PrintDataValuesSettings * settings =    (( 1 < count ) || ( 0 == chart ))
        ? &_printDataValuesSettings
        : &_printDataValuesSettings2;
    for ( uint i = 0; i < count; ++i ) {
        settings->_printDataValues      = true;
        settings->_useDefaultFontParams = true;
        if ( 0 < chart )
            settings = &_printDataValuesSettings2;
    }
    if ( callSetPrintDataValues )
        setPrintDataValues( true, chart );
}


/**
  \fn bool KDChartParams::printDataValuesWithDefaultFontParams( uint chart ) const

  Retrieves whether data value texts are to be printed with
  non-default font parameters or not - <b>when</b> text printing is
  active.

  \note You might also want to call printDataValues to see if text
  printing is active since this default flag remains set in
  background even when printing has been de-activated.

  \sa setPrintDataValuesWithDefaultFontParams, printDataValues
  */


/**
  \fn bool KDChartParams::printDataValues( uint chart ) const

  Returns whether the data values will be printed near their
  respective entries.

  \param chart The number of the chart: 0 for the first chart, 1 for
  the second chart in case there are two charts to be drawn sharing the
  same data area.

  \return whether the data values will be printed near their
  respective entries.

  \sa setPrintDataValues
  */


/**
  \fn QFont KDChartParams::dataValuesFont( uint chart ) const

  Returns the font to be used for printing the data values

  \param chart The number of the chart: 0 for the first chart, 1 for
  the second chart in case there are two charts to be drawn sharing the
  same data area.

  \returns the font to be used for printing the data values

  \sa setPrintDataValues
  */


/**
  \fn bool KDChartParams::dataValuesUseFontRelSize( uint chart ) const

  Returns whether the font size to be used for printing the
  data values is calculated dynamically.

  \param chart The number of the chart: 0 for the first chart, 1 for
  the second chart in case there are two charts to be drawn sharing the
  same data area.

  \return whether the font size to be used for printing the
  data values is calculated dynamically.

  \sa setPrintDataValues, setPrintDataValuesFontRelSize
  */


/**
  \fn int KDChartParams::dataValuesFontRelSize( uint chart ) const

  Returns the relative size (in per mille of the chart width)
  of font size to be used for printing the
  data values.

  \param chart The number of the chart: 0 for the first chart, 1 for
  the second chart in case there are two charts to be drawn sharing the
  same data area.

  \return the relative size (in per mille of the chart width)
  of font size to be used for printing the
  data values.

  \sa setPrintDataValues, setPrintDataValuesFontRelSize
  */


/**
  \fn QColor KDChartParams::dataValuesColor( uint chart ) const

  Returns the colour of the font to be used for printing the
  data values.

  \param chart The number of the chart: 0 for the first chart, 1 for
  the second chart in case there are two charts to be drawn sharing the
  same data area.

  \return the colour of the font to be used for printing the
  data values.

  \sa setPrintDataValues, setPrintDataValuesColor
  */


/**
  \fn bool KDChartParams::dataValuesAutoColor( uint chart ) const

  Returns whether the font to be used for printing the data values
  texts shall have automatically calculated colors fitting to their
  respectivs data representations.

  \param chart The number of the chart: 0 for the first chart, 1 for
  the second chart in case there are two charts to be drawn sharing the
  same data area.

  \return whether the font to be used for printing the data values texts
  shall have automatically calculated colors fitting to their respectivs data representations.

  \sa setPrintDataValues
  */


/**
  Returns the anchor position which the text is to be aligned to.

  \param chart The number of the chart: 0 for the first chart, 1 for
  the second chart in case there are two charts to be drawn sharing the
  same data area.

  \param negative If true the return value is only valid for data
  values less than zero, if false it applies to data values greater
  or equal to zero.

  \returns the anchor position which the text is to be aligned to in case of
  the value being less than zero.

  \sa setPrintDataValues
  */
KDChartEnums::PositionFlag KDChartParams::dataValuesAnchorPosition( uint chart, bool negative ) const
{
    if ( negative )
        return chart ? _printDataValuesSettings2._dataValuesAnchorNegativePosition
            : _printDataValuesSettings._dataValuesAnchorNegativePosition;
    else
        return chart ? _printDataValuesSettings2._dataValuesAnchorPositivePosition
            : _printDataValuesSettings._dataValuesAnchorPositivePosition;
}


/**
  Returns the way how the text is to be aligned to the anchor.

  This must be a reasonable combination of Qt::AlignmentFlags.

  \param chart The number of the chart: 0 for the first chart, 1 for
  the second chart in case there are two charts to be drawn sharing the
  same data area.

  \param negative If true the return value is only valid for data
  values less than zero, if false it applies to data values greater
  or equal to zero.

  \returns the way how the text is to be aligned to the anchor in case of
  the value being less than zero.

  \sa setPrintDataValues
  */
uint KDChartParams::dataValuesAnchorAlign( uint chart, bool negative ) const
{
    if ( negative )
        return chart ? _printDataValuesSettings2._dataValuesAnchorNegativeAlign
            : _printDataValuesSettings._dataValuesAnchorNegativeAlign;
    else
        return chart ? _printDataValuesSettings2._dataValuesAnchorPositiveAlign
            : _printDataValuesSettings._dataValuesAnchorPositiveAlign;
}


/**
  Returns the X distance between the text and its anchor.

  \note For better compatibility to the dynamic font size,
  this parameter is <b>always</b> interpreted as being a per-mille
  value of the logical width of the drawing area. If greater 0, the X
  position is increased, if less than 0, it is reduced, this is
  calculated dynamically before painting.

  \param chart The number of the chart: 0 for the first chart, 1 for
  the second chart in case there are two charts to be drawn sharing the
  same data area.

  \param negative If true the return value is only valid for data
  values less than zero, if false it applies to data values greater
  or equal to zero.

  \returns the X distance between the text and its anchor.

  \sa setPrintDataValues
  */
int KDChartParams::dataValuesAnchorDeltaX( uint chart, bool negative ) const
{
    if ( negative )
        return chart ? _printDataValuesSettings2._dataValuesAnchorNegativeDeltaX
            : _printDataValuesSettings._dataValuesAnchorNegativeDeltaX;
    else
        return chart ? _printDataValuesSettings2._dataValuesAnchorPositiveDeltaX
            : _printDataValuesSettings._dataValuesAnchorPositiveDeltaX;
}


/**
  Returns the Y distance between the text and its anchor.

  \note For better compatibility to the dynamic font size this
  parameter is <b>always</b> interpreted as being a per-mille value
  of the logical width of the drawing area. If greater 0, the Y
  position is increased, if less than 0, it is reduced, this is
  calculated dynamically before painting.

  \param chart The number of the chart: 0 for the first chart, 1 for
  the second chart in case there are two charts to be drawn sharing the
  same data area.

  \param negative If true the return value is only valid for data
  values less than zero, if false it applies to data values greater
  or equal to zero.

  \returns the Y distance between the text and its anchor.

  \sa setPrintDataValues
  */
int KDChartParams::dataValuesAnchorDeltaY( uint chart, bool negative ) const
{
    if ( negative )
        return chart ? _printDataValuesSettings2._dataValuesAnchorNegativeDeltaY
            : _printDataValuesSettings._dataValuesAnchorNegativeDeltaY;
    else
        return chart ? _printDataValuesSettings2._dataValuesAnchorPositiveDeltaY
            : _printDataValuesSettings._dataValuesAnchorPositiveDeltaY;
}


/**
  Returns the amount of degrees (using a circle of 360 degrees) taken to
  rotate the text. Positive values rotate clockwise, negative values rotate counter-clockwise.

  \param chart The number of the chart: 0 for the first chart, 1 for
  the second chart in case there are two charts to be drawn sharing the
  same data area.

  \param negative If true the return value is only valid for data
  values less than zero, if false it applies to data values greater
  or equal to zero.

  \returns the amount of degrees (using a circle of 360 degrees) taken to
  rotate the text.

  \sa setPrintDataValues
  */
int KDChartParams::dataValuesRotation( uint chart, bool negative ) const
{
    if ( negative )
        return chart ? _printDataValuesSettings2._dataValuesNegativeRotation
            : _printDataValuesSettings._dataValuesNegativeRotation;
    else
        return chart ? _printDataValuesSettings2._dataValuesPositiveRotation
            : _printDataValuesSettings._dataValuesPositiveRotation;
}


/**
  \fn KDChartEnums::TextLayoutPolicy KDChartParams::dataValuesLayoutPolicy( uint chart ) const

  Returns the way to handle too narrow space conflicts: what to do if
  a data text covers a neighboring data text (or a neighboring data
  area, resp.).

  \note A layout policy different from LayoutJustOverwrite from does
  not mean that this policy is followed in any case. Rather than
  giving up when the selected policy does not result in a good layout
  the program will automatically try the next policy: if
  LayoutPolicyRotate did not succeed LayoutPolicyShiftVertically will
  be tried, if this did not succeed either
  LayoutPolicyShiftHorizontally will be tried.

  \param chart The number of the chart: 0 for the first chart, 1 for
  the second chart in case there are two charts to be drawn sharing the
  same data area.

  \returns the way to handle too narrow space conflicts.

  \sa setPrintDataValues
  */

/**
  \fn void KDChartParams::changed()

  This signal is emitted when any of the chart parameters has changed.
  */

/**
  \var const uint KDCHART_MAX_AXES

  Charts have up to 12 axes:
  up to two axes on both sides for X, Y
  and Z (if data is 3-dimensional). This constant is located here and
  in KDChartAxisParams, because one KDChartAxisParams object
  describes only one axis.
  */

/**
  \var static const int KDCHART_PROPSET_NORMAL_DATA;

  Default (built-in) property ID, used for data cells
  without any special properties.

  Use this IDs <b>without</b> previously defining a respective
  set of properties via KDChartParams::registerProperties(...).

  \note While (in theory) it <b>is</b> possible to assign special
  property values to this build-in property set this normally might not
  a very good idea since these values would apply to every(!) data cell
  that has no other property set assigned.  Such general changes
  would rather be done calling KDChartParam's main functions, e.g. you
  would invoke KDChartParams::setLineMarker(true) instead of changing
  the normal-data property set.

  \sa KDCHART_PROPSET_TRANSPARENT_DATA
  \sa KDCHART_PROPSET_HORI_LINE, KDCHART_PROPSET_VERT_LINE
  \sa KDChartData::setPropertySet
  \sa KDChartParams::registerProperties, KDChartParams::setProperties, KDChartParams::properties
  */

/**
  \var static const int KDCHART_PROPSET_TRANSPARENT_DATA;

  Predefined (build-in) property ID. used to specify a cell that should be displayed
  using a null pen: neither the data representation nor
  the connecting line to the next point will be shown,
  but the line will be drawn using line style Qt::NoPen.

  Use this IDs <b>without</b> previously defining a respective
  set of properties via KDChartParams::registerProperties(...).

  \note: Assigning this property to a data cell will result in the
  cell being invisible: there will be no marker for this point and
  there will be a 'gap' in this dataset's line from the point to the next one.
  In case you want to change this default behavior,
  use KDChartParams::setProperties(),
  e.g. the following piece of code still has an invisible line while now
  enabeling showing of the marker - but only if it was set active by
  KDChartParams::setLineMarker(true).

  \verbatim
  KDChartPropertySet transpProps;
  bool bDummy;
  if( properties(KDCHART_PROPSET_TRANSPARENT_DATA,
  transpProps) ){
// build-in property was found, let's modify it a bit:
transpProps.setShowMarker(
KDCHART_PROPSET_NORMAL_DATA, bDummy );
setProperties(KDCHART_PROPSET_TRANSPARENT_DATA,
transpProps);
}else{
// Ooops? The build-in property was NOT found!
// OK, never mind, let's specify it from scratch:
transpProps.setName("transparent data");
transpProps.setLineStyle(KDChartPropertySet::OwnID, Qt::NoPen);
transpProps.setShowMarker(
KDCHART_PROPSET_NORMAL_DATA, bDummy);
setProperties(KDCHART_PROPSET_TRANSPARENT_DATA,
transpProps);
}
\endverbatim

\note The example above uses setProperties (in the last line) to
define a property set. This is the right way for build-in property sets,
but it is not recomended for your additional user-defined property sets:
these should be registered with the registerProperties function
to initially obtain a unique ID number for your new property set,
see the second example given with \c KDCHART_PROPSET_VERT_LINE.

\sa KDCHART_PROPSET_NORMAL_DATA
\sa KDCHART_PROPSET_HORI_LINE, KDCHART_PROPSET_VERT_LINE
\sa KDChartData::setPropertySet
\sa KDChartParams::registerProperties, KDChartParams::setProperties, KDChartParams::properties
*/

/**
  \var static const int KDCHART_PROPSET_HORI_LINE;

  Predefined (build-in) property ID. may be used to specify a special cell
  which is not part of the normal data but to be used for positioning
  a separate horizontal line.
  Data display will be surpressed for this point: neither the data
  representation nor the connecting line to the next point will
  be shown, actually the point will remain invisible but a horizontal
  line will be drawn instead: using line style Qt::DotLine and the
  respective dataset's line color.

  Use this IDs <b>without</b> previously defining a respective
  set of properties via KDChartParams::registerProperties(...).

  In case you want to change this default behavior,
  use KDChartParams::setProperties(),
  e.g. the following piece of code still has an invisible line and
  also does not show the point's marker while now showing two
  separate markers at the horizontal line's start and end.

  \verbatim
  KDChartPropertySet horiProps;
  if( properties(KDCHART_PROPSET_HORI_LINE,
  horiProps) ){
// build-in property was found, let's modify it a bit:
horiProps.setExtraMarkersAlign(
KDChartPropertySet::OwnID,
Qt::AlignLeft | Qt::AlignRight );
setProperties(KDCHART_PROPSET_HORI_LINE,
horiProps);
}else{
// Ooops? The build-in property was NOT found!
// OK, never mind, let's specify it from scratch:
horiProps.setName("horizontal line");
horiProps.setLineStyle(KDChartPropertySet::OwnID, Qt::NoPen);
horiProps.setShowMarker( KDChartPropertySet::OwnID, false );
horiProps.setExtraMarkersAlign(  KDChartPropertySet::OwnID,
Qt::AlignLeft | Qt::AlignRight );
horiProps.setExtraMarkersAlign(
KDChartPropertySet::OwnID,
Qt::AlignLeft | Qt::AlignRight );
setProperties(KDCHART_PROPSET_HORI_LINE,
horiProps);
}
\endverbatim

\note The example above uses setProperties (in the last line) to
define a property set. This is the right way for build-in property sets,
but it is not recomended for your additional user-defined property sets:
these should be registered with the registerProperties function
to initially obtain a unique ID number for your new property set,
see the second example given with \c KDCHART_PROPSET_VERT_LINE.

\sa KDCHART_PROPSET_NORMAL_DATA
\sa KDCHART_PROPSET_VERT_LINE
\sa KDChartData::setPropertySet
\sa KDChartParams::registerProperties, KDChartParams::setProperties, KDChartParams::properties
*/

/**
  \var static const int KDCHART_PROPSET_VERT_LINE;

  Predefined (build-in) property ID. may be used to specify a special cell
  which is not part of the normal data but to be used for positioning
  a separate vertical line.
  Data display will be surpressed for this point: neither the data
  representation nor the connecting line to the next point will
  be shown, actually the point will remain invisible but a vertical
  line will be drawn instead: using line style Qt::DotLine and the
  respective dataset's line color.

  Use this IDs <b>without</b> previously defining a respective
  set of properties via KDChartParams::registerProperties(...).

  In case you want to change this default behavior,
  use KDChartParams::setProperties(),
  e.g. the following piece of code still has an invisible line and
  also does not show the point's marker while now showing a
  separate marker at the top end of the vertical line which
  will <b>not</b> run across the complete data area but start
  on the respective point's Y position and go to the top edge
  of the chart's area.

  \verbatim
  KDChartPropertySet vertProps;
  if( properties(KDCHART_PROPSET_VERT_LINE,
  vertProps) ){
// build-in property was found, let's modify it a bit:
vertProps.setExtraMarkersAlign(
KDChartPropertySet::OwnID, Qt::AlignTop );
setProperties(KDCHART_PROPSET_VERT_LINE,
vertProps);
}else{
// Ooops? The build-in property was NOT found!
// OK, never mind, let's specify it from scratch:
vertProps.setName("vertical line");
vertProps.setLineStyle(KDChartPropertySet::OwnID, Qt::NoPen);
vertProps.setShowMarker( KDChartPropertySet::OwnID, false );
vertProps.setExtraMarkersAlign(  KDChartPropertySet::OwnID,
Qt::AlignTop );
vertProps.setExtraMarkersAlign(
KDChartPropertySet::OwnID,
Qt::AlignTop );
setProperties(KDCHART_PROPSET_VERT_LINE,
vertProps);
}
\endverbatim

\note The example above uses setProperties (in the last line) to
define a property set. This is the right way for build-in property sets,
but it is not recomended for your additional user-defined property sets:
these should be registered with the registerProperties function
to initially obtain a unique ID number for your new property set,
see the following example:

The code shown below demonstrates how to specify a <b>new</b>
property set which then can be used for adding a red vertical
line with marker to some of your normal data points:

\verbatim
// 1. specify and register the new property set
KDChartPropertySet dataWithTopLineProps;
dataWithTopLineProps.setName(
"normal data with red vertical line on top");
dataWithTopLineProps.setExtraLinesAlign(
KDChartPropertySet::OwnID,
Qt::AlignTop );
dataWithTopLineProps.setExtraLinesWidth(
KDChartPropertySet::OwnID,
-2 );
dataWithTopLineProps.setExtraLinesColor(
KDChartPropertySet::OwnID,
    Qt::red );
dataWithTopLineProps.setExtraLinesStyle(
        KDChartPropertySet::OwnID,
        Qt::SolidLine );
dataWithTopLineProps.setExtraMarkersAlign(
        KDChartPropertySet::OwnID,
        Qt::AlignTop );
dataWithTopLineProps.setExtraMarkersSize(
        KDChartPropertySet::OwnID,
        QSize(-10, -10) );
dataWithTopLineProps.setExtraMarkersColor(
        KDChartPropertySet::OwnID,
        Qt::darkRed );
dataWithTopLineProps.setExtraMarkersStyle(
        KDChartPropertySet::OwnID,
        KDChartParams::LineMarkerCircle );
int idDataWithTopLineProps
= registerProperties( dataWithTopLineProps );
// 2. assign this property set
//    to some of your data cells
d = new KDChartTableData( 5, 10 );
// ...
d->cell( 0, 2 ).setPropertySet( idDataWithTopLineProps );
d->cell( 2, 5 ).setPropertySet( idDataWithTopLineProps );
\endverbatim

\sa KDCHART_PROPSET_NORMAL_DATA
\sa KDCHART_PROPSET_HORI_LINE
\sa KDChartData::setPropertySet
\sa KDChartParams::registerProperties, KDChartParams::setProperties, KDChartParams::properties
*/



//@}
// END GENERAL

// START BARCHART
/** @name Bar chart-specific methods.

  These methods query and set barchart-specific parameters.
  */
//@{

/**
  \enum KDChartParams::BarChartSubType

  The bar subtype. Only used when chartType() == Bar

  \sa setBarChartSubType, barChartSubType
  */


/**
  \fn void KDChartParams::setBarChartSubType( BarChartSubType barChartSubType )
  Specifies the bar chart subtype.

  This setting only has an effect in bar charts.

  \param barChartSubType the bar chart subtype
  \sa barChartSubType, BarChartSubType, setChartType, chartType
  */


/**
  \fn KDChartParams::BarChartSubType KDChartParams::barChartSubType() const

  Returns the bar chart subtype.

  This setting only has an effect in bar charts.

  \return the bar chart sub type
  \sa setBarChartSubType, BarChartSubType, setChartType, chartType
  */



/**
  \fn void KDChartParams::setThreeDBars( bool threeDBars )

  Specifies whether the engine should draw the bars in 3D.

  This setting only has an effect in bar charts.

  \param threeDBars true if bars should be drawn with a 3D effect
  \sa threeDBars, setThreeDBarAngle, threeDBarAngle
  */


/**
  \fn bool KDChartParams::threeDBars() const

  Returns whether the engine should draw any bars in 3D.

  This setting only has an effect in bar charts.

  \return true if bars should be draws with a 3D effect, false
  otherwise
  \sa setThreeDBars, threeDBarAngle, setThreeDBarAngle
  */

/**
  \fn void KDChartParams::setThreeDBarsShadowColors( bool shadow )
  \obsolete
  Specifies whether the engine should draw the sides and tops of 3D bars
  in shadowed versions of the data colors or in the data colors
  themselves. Only used if threeDBars() == true. The default is true.

  This setting only has an effect in bar charts.

  This method is obsolete; use setThreeDShadowColors instead

  \param shadow true for shadowed colors, false for original colors
  \sa setThreeDShadowColors
  */

/**
  \fn bool KDChartParams::threeDBarsShadowColors() const

  \obsolete
  Returns whether the engine should draw the sides and tops of 3D bars in
  shadowed versions of the data colors or in the data colors
  themselves. Only used if threeDBars() == true. The default is true.

  This setting only has an effect in bar charts.

  This method is obsolete; use threeDShadowColors instead

  \return true if bars use shadowed colors for 3D effects
  \sa setThreeDBarsShadowColors
  */


/**
  Specifies the angle used for 3D bars. Only used if threeDBars() == true.

  This setting only has an effect in bar charts.

  \param angle the angle in degrees. The default (and most useful
  value) is 45. Angle can be between 0 and 90, all other values
  are ignored. Values close to 0 or close to 90 may look
  strange, depending on the resolution of the output device.
  \sa setThreeDBars, threeDBars
  \sa threeDBarAngle, cosThreeDBarAngle
  \sa setThreeDBarDepth, threeDBarDepth
  */
void KDChartParams::setThreeDBarAngle( uint angle )
{
    if ( angle > 90 )  /* since angle is an uint, we do not need to
                          test for < 0 */
        return ;
    _threeDBarAngle = angle;

    // cache the cosine of this value
    _cosThreeDBarAngle = cos( static_cast < double > ( _threeDBarAngle ) * M_PI / 180.0 );
    emit changed();
}


/**
  \fn uint KDChartParams::threeDBarAngle() const

  Returns the angle in degrees used for 3D bars. Only used if threeDBars() == true.

  This setting only has an effect in bar charts.

  \return the angle in degrees used for 3D bars, always between 0 and 90.
  \sa setThreeDBars, threeDBars
  \sa setThreeDBarAngle, cosThreeDBarAngle
  \sa setThreeDBarDepth, threeDBarDepth
  */


/**
  \fn double KDChartParams::cosThreeDBarAngle() const

  Returns the cosine in rad of the angle used for 3D bars. Only used
  if threeDBars() == true.

  This setting only has an effect in bar charts.

  \return the cosine in rad of the angle used for 3D bars, always
  between 0 and 90.  \sa setThreeDBars, threeDBars \sa
  setThreeDBarAngle, threeDBarAngle \sa setThreeDBarDepth,
  threeDBarDepth
  */


/**
  \fn void KDChartParams::setThreeDBarDepth( double depth )

  Specifies the depth of the 3D Effect used for 3D bars.
  Only used if threeDBars() == true.

  This setting only has an effect in bar charts.

  \param depth the depth of the 3D Effect in relation to
  the bar width. The default (and most useful) value of
  1.0 may be used to specify bars with square basis.
  \sa threeDBarDepth
  \sa setThreeDBars,     threeDBars
  \sa setThreeDBarAngle, threeDBarAngle, cosThreeDBarAngle
  */


/**
  \fn double KDChartParams::threeDBarDepth() const

  Returns the depth of the 3D Effect used for 3D bars.
  Only used if threeDBars() == true.

  This setting only has an effect in bar charts.

  \return the depth of the 3D Effect in relation to the bar width.
  \sa setThreeDBarDepth
  \sa setThreeDBars,     threeDBars
  \sa setThreeDBarAngle, threeDBarAngle, cosThreeDBarAngle
  */


/**
  \fn void KDChartParams::setDatasetGap( int gap )

  Specifies the number of pixels between two dataset values.
  In addition you might want to use \c setOutlineDataLineStyle( Qt::NoPen ) to hide the line.

  Also the method \c setValueBlockGap might be usefull, please read the information given there ...

  This setting only has an effect in bar charts: if there is more than one dataset shown by your chart.

  \note Use negative values for overlaps, use \c
  setDatasetGapIsRelative to specify that the \c gap
  value is a per mille value of the chart data area width.

  The default is 6 per mille of the data area of the chart.

  \param gap the number of pixels between two dataset values.
  \sa setValueBlockGap
  \sa datasetGap
  \sa datasetGapIsRelative, setDatasetGapIsRelative
  \sa setOutlineDataLineStyle
  */


/**
  \fn int KDChartParams::datasetGap() const

  Returns the number of pixels between two dataset values.

  This setting only has an effect in bar charts.

  \note Negative values signify overlaps, use \c datasetGapIsRelative
  to find out if the \datasetGap value is a per mille value of the
  chart data area width.

  \return the number of pixels between two dataset values.
  \sa setDatasetGap
  \sa datasetGapIsRelative, setDatasetGapIsRelative
  */


/**
  \fn void KDChartParams::setDatasetGapIsRelative( bool gapIsRelative )

  Specifies if the value set by \c setDatasetGap is a
  per mille value of the chart data area width.

  This setting only has an effect in bar charts.

  \param gapIsRelative specifies if the value set by \c setDatasetGap
  is a per mille value of the chart data area width.
  \sa datasetGapIsRelative, datasetGap, setDatasetGap
  */


/**
  \fn bool KDChartParams::datasetGapIsRelative() const

  Returns if the value set by \c setDatasetGap
  is a per mille value of the chart data area width.

  This setting only has an effect in bar charts.

  \return if the value set by \c setDatasetGap
  is a per mille value of the chart data area width.
  \sa setDatasetGap, setDatasetGapIsRelative, datasetGap, setDatasetGap
  */


/**
  \fn void KDChartParams::setValueBlockGap( int gap )

  Specifies the number of pixels between each value block.
  If there is only one dataset shown this gap is appearing between each of the bars, otherwise it is appearing between each of the bar blocks.
  In addition you might want to use \c setOutlineDataLineStyle( Qt::NoPen ) to hide the line.

  Also the method \c setDatasetGap might be usefull, please read the information given there ...

  This setting only has an effect in bar charts.

  Use negative values for overlaps (which might look strange),
  use \c setValueBlockGapIsRelative to specify that the \c gap
  value is a per mille value of the chart data area width.

  The default is 15 per mille of the data area of the chart.

  \note Specifying a value block gap width AND a bar width
  may result in KD Chart not being able to display all bars
  if the data area is not wide enough. To make your
  application notice when this
  happens you should consider connecting the
  barsDisplayed() signal emitted by your KDChartWidget object
  to an appropriate slot function.  Additionally (or in case
  you are not using the KDChartWidget class, resp.)
  you may access the number of actually displayed/not displayed
  bars by the KDChartParams functions numBarsDisplayed
  and numBarsLeft, the return value of these functions
  are not not defined if they are called before drawing is done.

  \param gap the number of pixels between each value block.
  \sa setDatasetGap
  \sa setBarWidth, valueBlockGap
  \sa KDChartWidget::barsDisplayed, KDChart::barsDisplayed
  \sa numBarsDisplayed, numBarsLeft
  \sa valueBlockGapIsRelative, setValueBlockGapIsRelative
  \sa setOutlineDataLineStyle
  */


/**
  \fn int KDChartParams::valueBlockGap() const

  Returns the number of pixels between each value block.

  This setting only has an effect in bar charts.

  \note Negative values signify overlaps, use \c valueBlockGapIsRelative
  to find out if the \valueBlockGap value is a per mille value of the
  chart data area width.

  \return the number of pixels between each value block.
  \sa valueBlockGap
  \sa valueBlockGapIsRelative, setValueBlockGapIsRelative
  */


/**
  \fn void KDChartParams::setBarWidth( int width )

  Specifies the width of one bar: makes sure that all of
  the bars have exactly the same width.

  This setting only has an effect in bar charts.

  \note Use negative values for to specify that the \c width
  value is a per mille value of the chart data area width,
  use no value or the default KDCHART_AUTO_SIZE
  to (re)activate normal calculation mode making sure that all
  bars fit into the available space but might not look so nice
  if many bars are there.

  \note Specifying a bar width AND a value block gap width
  may result in KD Chart not being able to display all bars
  if the data area is not wide enough. To make your
  application notice when this
  happens you should consider connecting the
  barsDisplayed() signal emitted by your KDChartWidget object
  to an appropriate slot function.  Additionally (or in case
  you are not using the KDChartWidget class)
  you may access the number of actually displayed/not displayed
  bars by the KDChartParams functions numBarsDisplayed
  and numBarsLeft, the return value of these functions
  are not not defined if they are called before drawing is done.

  \param width the width of one bar.
  \sa KDChartWidget::barsDisplayed, KDChart::barsDisplayed
  \sa numBarsDisplayed, numBarsLeft
  \sa setValueBlockGap
  */

/**
  \fn int KDChartParams::barWidth() const

  Returns the width of one bar as set by setBarWidth
  or KDCHART_AUTO_SIZE if setBarWidth
  was never called.

  This setting only has an effect in bar charts.

  \note Negative values signify that the bar width is
  a per mille value of the chart data area width.

  \return the width of one bar as set by setBarWidth
  or KDCHART_AUTO_SIZE if setBarWidth
  was never called.
  \sa setBarWidth
  */

/**
  \fn int KDChartParams::numBarsDisplayed() const

  Returns the number of bars that were displayed last
  time your bar chart was drawn.

  This setting only has an effect in bar charts.

  This can be different from the number of bars you
  actually wanted to display: by specifying both
  the bar width and the value block gap width you
  may controll your chart very well but take the risk
  of not getting all bars drawn if there is not enough
  horizontal space to display them with the given width
  and gap width.

  To quickly determine if all bars were drawn just check
  if the numBarsLeft() function returns a zero value.

  \return the number of bars that were drawn.
  \sa KDChartWidget::barsDisplayed, KDChart::barsDisplayed
  \sa numBarsLeft
  \sa setBarWidth, setValueBlockGap
  */

/**
  \fn int KDChartParams::numBarsLeft() const

  Returns the number of bars that could NOT be painted
  last time your bar chart was drawn.

  This setting only has an effect in bar charts.

  If this value is greater than zero, the bar chart
  shows less bars than you
  actually wanted to display: by specifying both
  the bar width and the value block gap width you
  may controll your chart very well but take the risk
  of not getting all bars drawn if there is not enough
  horizontal space to display them with the given width
  and gap width.

  You may call the numBarsDisplayed() function to see
  how many bars were drawn.

  \return the number of bars that could not be drawn.
  \sa KDChartWidget::barsDisplayed, KDChart::barsDisplayed
  \sa numBarsDisplayed
  \sa setBarWidth, setValueBlockGap
  */

/**
  \fn void KDChartParams::setValueBlockGapIsRelative( bool gapIsRelative )

  Specifies if the value set by \c setValueBlockGap is a
  per mille value of the chart data area width.

  This setting only has an effect in bar charts.

  \param gapIsRelative specifies if the value set by \c setValueBlockGap
  is a per mille value of the chart data area width.
  \sa valueBlockGapIsRelative, valueBlockGap, setValueBlockGap
  */


/**
  \fn bool KDChartParams::valueBlockGapIsRelative() const

  Returns if the value set by \c setValueBlockGap
  is a per mille value of the chart data area width.

  This setting only has an effect in bar charts.

  \return if the value set by \c setValueBlockGap
  is a per mille value of the chart data area width.
  \sa setValueBlockGapIsRelative, setValueBlockGap, valueBlockGap
  \sa setValueBlockGap
  */


/**
   \fn void KDChartParams::setDrawSolidExcessArrows( bool solidArrows )

   Specifies whether arrows showing excess values in bar charts should
   be drawn solidly or split.

  This setting only has an effect in bar charts.

   If \a solidArrows is true, the bars with excess values (like
   infinity, or any other value that exceeds the y-axis labelling) will
   have an integrated arrow head. If \a solidArrows is false, they will
   still have an integrated arrow head at a lower position, but also
   two flat arrows on top of them to better indicate that there could
   be a large gap. It is recommended to set this value to false (this
   is also the default), as this makes the excess value situation much
   more obvious.

   This setting only has an effect in bar charts.

   \param solidArrows whether excess arrows shown be drawn solidly or
   split
   \sa drawSolidExcessArrows
*/


/**
   \fn bool KDChartParams::drawSolidExcessArrows() const

   Returns whether arrows showing excess values in bar charts should
   be drawn solidly or split.

   This setting only has an effect in bar charts.

   \return true if excess arrows are drawn solidly, false if they are
   split
   \sa setDrawSolidExcessArrows
*/


//@}
// END BARCHART

// START LINECHART
/** @name Line and area chart-specific methods.

  These methods query and set line and area chart-specific parameters.
  */
//@{

/**
  \enum KDChartParams::LineChartSubType

  The line subtype. Only used when chartType == Line

  \sa setLineChartSubType, lineChartSubType
  */


/**
  \fn void KDChartParams::setLineChartSubType( LineChartSubType lineChartSubType )

  Specifies the line chart subtype. Only used if chartType() ==
  Line. The default is LineNormal.

  \param lineChartSubType the line chart subtype
  \sa lineChartSubType, LineChartSubType, setChartType, chartType
  */


/**
  \fn KDChartParams::LineChartSubType KDChartParams::lineChartSubType() const

  Returns the line chart subtype. Only used if chartType() ==
  Line.

  \return the line chart sub type
  \sa setLineChartSubType, LineChartSubType, setChartType, chartType
  */



/**
  \fn void KDChartParams::setLineMarker( bool marker )

  Specifies whether there should be a marker at each data
  point. Only used if chartType() == Line and if threeDLines() ==
  false. The default is not to draw markers.

  \note Use the setLineMarkerStyle function to specify the shape
  of the markers, use the setLineWidth function to set the
  width of the lines connecting the markers (or to surpress
  drawing of such lines, resp.)

  \param marker true if markers should be drawn
  \sa setLineMarker, setLineMarkerSize
  \sa setLineMarkerStyle, setLineColor, setLineWidth, setLineStyle
  */


/**
  \fn bool KDChartParams::lineMarker() const

  Returns whether line markers should be drawn at each data
  point. Only used if chartType() == Line and if threeDLines() ==
  false.

  \return true if markers should be drawn.
  */


/**
  \enum KDChartParams::LineMarkerStyle

  The available line marker styles.
  */


/**
  Specifies the line marker to be used for a dataset. Only used if
  chartType() == Line and lineMarker() == true. If you specify a
  marker for a dataset, but not for a dataset with a lower
  value, then the marker for the dataset with the lower value
  will be undefined unless it was previously defined. The default
  is a circle for the first dataset, a square for the second, a
  diamond for the third and undefined for all subsequent
  datasets.

  \note Extra marker styles LineMarker1Pixel and LineMarker4Pixels
  are intended for drawing charts representing large numbers
  of data cells, when using these styles it might be a good
  idea to set the line width to zero. This would surpress
  drawing of the lines resulting in drawing the markers only.

  \param dataset the dataset for which to set the line marker
  \param style the style to set for the specified dataset
  \sa LineMarkerStyle, lineMarkerStyle, setLineMarkerSize
  \sa setLineColor, setLineWidth, setLineStyle
  */
void KDChartParams::setLineMarkerStyle( uint dataset, LineMarkerStyle style )
{
    _lineMarkerStyles[ dataset ] = style;
    _maxDatasetLineMarkerStyle = QMAX( dataset,
            _maxDatasetLineMarkerStyle );
    emit changed();
}

/**
  Returns the marker to be used for a dataset. Only used if
  chartType() == Line and lineMarker() == true.

  \param dataset the dataset for which to return the line marker
  \return the line marker for the specified data set
  \sa LineMarkerStyle, setLineMarkerStyle
  */
KDChartParams::LineMarkerStyle KDChartParams::lineMarkerStyle( uint dataset ) const
{
    if( _lineMarkerStyles.find( dataset ) != _lineMarkerStyles.end() )
        return _lineMarkerStyles[ dataset ];
    else
        return LineMarkerCircle; // default
}




/**
  \var KDChartParams::LineMarkerStyleMap;

  The type of the storage structure for line marker styles. You
  should normally not need to use this.
  */


/**
  Sets a whole map of line marker styles. You can use this for
  assigning many line marker styles at the same time, but
  typically it is better to set them individually with \a
  setLineMarkerStyle().

  \param map the map of styles
  \sa lineMarkerStyles, setLineMarker, setLineMarkerStyle, setLineMarkerSize
  \sa setLineColor, setLineWidth, setLineStyle
  */
void KDChartParams::setLineMarkerStyles( LineMarkerStyleMap map ) {
    _lineMarkerStyles = map;
    // update _maxDatasetLineMarkerStyle
    uint maxDataset = 0;
    for( LineMarkerStyleMap::ConstIterator it = _lineMarkerStyles.begin();
            it != _lineMarkerStyles.end(); ++it )
        maxDataset = QMAX( maxDataset, it.key() );
    _maxDatasetLineMarkerStyle = maxDataset;
}


/**
  \fn KDChartParams::LineMarkerStyleMap KDChartParams::lineMarkerStyles() const

  Returns the whole map of line marker styles. You will typically
  not need this. You can query individual line marker styles by
  calling \a lineMarkerStyle().

  \return the map of styles
  \sa lineMarkerStyle, setLineMarkerStyles
  */


/**
  \fn uint KDChartParams::maxDatasetLineMarkerStyle() const


  Returns the highest dataset for which a line marker style has been
  defined. Not all datasets with a lower number necessarily have
  a defined line marker.

  \return the highest dataset with a defined line marker
  \sa LineMarkerStyle, setLineMarkerStyle, lineMarkerStyle
  */


/**
  \fn void KDChartParams::setLineMarkerSize( QSize size )

  Specifies the sizes of line markers. Only used if chartType() == Line
  and lineMarker() == true. The default is 6x6.

  Negative values of \c size.width() and/or \c size.height() will
  be  interpreted as per-mille value of the chart's drawing area,
  the true marker width (or height, resp.) will then be calculated
  dynamically depending on the actual chart size.

  \param size the size of the line marker in pixels
  \sa lineMarkerSize, setLineMarker, setLineMarkerStyle
  \sa setLineColor, setLineWidth, setLineStyle
  */


/**
  \fn QSize KDChartParams::lineMarkerSize() const

  Returns the sizes of line markers. Only used if chartType() == Line and
  lineMarker() == true.

  \return the size of the line marker in pixels
  \sa setLineMarkerSize
  */


/**
  \fn void KDChartParams::setLineWidth( int width )

  Specifies the width for lines in line charts. Default is 1.
  Only used if threeDLines() == false.

  If \c width is negative it is interpreted as per-mille
  value of the chart's drawing area, the true line width
  will then be calculated dynamically depending on the
  actual chart size.

  \note Set the line width to zero to surpress drawing of the lines
  but do not forget to call setLineMarker( true ) and specify an
  appropriate marker style to have the data points drawn.

  \param width the new width
  \sa lineWidth, setLineColor, setLineStyle
  \sa setLineMarker, setLineMarkerSize, setLineMarkerStyle
  */


/**
  \fn int KDChartParams::lineWidth() const

  Returns the line width of the lines in line charts.

  \return the line width of lines in line charts
  \sa lineWidth
  */


/**
  \fn void KDChartParams::setLineColor( QColor color = QColor() )

  Specifies the color of the lines in line charts. By default
  line chart lines are drawn in the color specified by the
  respective setDataColor function, to reset your chart to
  this default just call setLineColor() without specifying a
  parameter.

  \note Setting the line color to a special value is usefull to
  visualize gaps in a normally very narrow series of data values.

  e.g. You might want to specify setLineMarkerStyle( i, LineMarker4Pixels )
  or even setLineMarkerStyle( i, LineMarker1Pixel ) and have your
  marker dots normally form a contiguous line without the need of
  any connecting lines except at the places where missing data
  values would interrupt the 'line'. Using setLineColor( Qt::darkGray)
  could make your chart more readable in such cases.

  \param color of the lines in line charts
  \sa lineColor, setLineWidth, setLineStyle
  \sa setLineMarker, setLineMarkerSize, setLineMarkerStyle
  */


/**
  \fn QColor KDChartParams::lineColor() const

  Returns the color of the lines in line charts.

  \return the color of the lines in line charts
  \sa setLineColor
  */


/**
  Specifies a line style to be used for line charts.

  The dataset parameter is optional, by default saying
  KDCHART_GLOBAL_LINE_STYLE, you may use it to
  set a  dataset specific line style for one the lines.

  Global and dataset specific line styles may be used
  simultaneously: If no dataset specific style is
  defined the global style is used which by default
  is Qt::SolidLine.

  Warning: On Windows 95/98, the style setting (other
  than NoPen and SolidLine) has no effect for lines with width greater
  than 1, due to a bug in the operating system.

  \param style the line style of the line
  \param dataset the number of the dataset controlling the respective line
  \sa lineStyle, setLineWidth, setLineColor
  \sa setLineMarker, setLineMarkerSize, setLineMarkerStyle
  */
void KDChartParams::setLineStyle( Qt::PenStyle style, uint dataset )
{
    if( KDCHART_GLOBAL_LINE_STYLE == dataset )
        _lineStyle = style;
    else
        _datasetLineStyles[ dataset ] = style;
    emit changed();
}

/**
  Returns the line style of one of the lines in a line chart
  or the global line style if no style was specified for the
  dataset or if the optional dataset parameter has its default
  value KDCHART_GLOBAL_LINE_STYLE.

  Warning: On Windows 95/98, the style setting (other than NoPen and
  SolidLine) has no effect for lines with width greater than 1, due
  to a bug in the operating system.

  \param dataset the dataset for which to return the line style
  \return the line style for the specified data set
  \sa setLineStyle, setLineMarkerStyle
  */
Qt::PenStyle KDChartParams::lineStyle( uint dataset ) const
{
    if( KDCHART_GLOBAL_LINE_STYLE == dataset )
        // global line style
        return _lineStyle;
    else if( _datasetLineStyles.find( dataset ) == _datasetLineStyles.end() )
        return lineStyle();
    else
        return _datasetLineStyles[ dataset ];
}


/**
  \fn void KDChartParams::setThreeDLines( bool threeD )

  Specifies whether lines should be drawn three-dimensionally or
  not. The default is to draw two-dimensionally. Only used if
  chartType() == Line.

  \param threeD if true, draw three-dimensionally, otherwise draw
  two-dimensionally.
  \sa threeDLines(), setThreeDLineDepth(), threeDLineDepth(),
//   setThreeDLineXRotation(), threeDLineXRotation(),
//   setThreeDLineYRotation(), threeDLineYRotation()
*/


/**
  \fn bool KDChartParams::threeDLines() const

  Returns whether lines are drawn three-dimensionally or not. The
  default is to draw two-dimensionally. Only used if chartType() ==
  Line.

  \return true if lines are drawn three-dimensionally, false
  otherwise.
  \sa setThreeDLines(), setThreeDLineDepth(), threeDLineDepth(),
//   setThreeDLineXRotation(), threeDLineXRotation(),
//   setThreeDLineYRotation(), threeDLineYRotation()
*/


/**
  \fn void KDChartParams::setThreeDLineDepth( int depth )

  Specifies the depth of 3D lines (the "width" in Z
  direction). Only used if chartType() == Line and threeDLines() ==
  true. The default is 20 pixels.

  \param depth the depth in pixels
  \sa setThreeDLines(), threeDLines(), threeDLineDepth()
  */


/**
  \fn int KDChartParams::threeDLineDepth() const

  Returns the depth of 3D lines (the "width" in Z direction). Only
  used if chartType() == Line and threeDLines() == true. The
  default is 20 pixels.

  \return the depth in pixels
  \sa setThreeDLines(), threeDLines(), setThreeDLineDepth()
  */


/**
  \fn void KDChartParams::setThreeDLineXRotation( int degrees )

  \deprecated Feature scheduled for future release, at present not implemented.
  */
/*
   \fn void KDChartParams::setThreeDLineXRotation( int degrees )

   Specifies the rotation around the X axis in degrees. The value
   may be between 0 and 90. Only used if chartType() == Line and
   threeDLines() == true. The default is 30 degrees. If 0 degrees is
   specified for both the X and the Y rotation, the lines will look
   like 2D lines.

   \param rotation the rotation in degrees. Must be between 0 and
   90.
   \sa setThreeDLines(), threeDLines(), threeDLineXRotation()
   */



/**
  \fn int KDChartParams::threeDLineXRotation() const

  \deprecated Feature scheduled for future release, at present not implemented.
  */
/*
   \fn int KDChartParams::threeDLineXRotation() const

   Returns the rotation around the X axis in degrees. The value may
   be between 0 and 90. Only used if chartType() == Line and
   threeDLines() == true. The default is 30 degrees.

   \return the rotation in degrees. Is always between 0 and 90.
   */


/**
  \fn void KDChartParams::setThreeDLineYRotation( int degrees )

  \deprecated Feature scheduled for future release, at present not implemented.
  */
/*
   \fn void KDChartParams::setThreeDLineYRotation( int degrees )

   Specifies the rotation around the Y axis in degrees. The value
   may be between 0 and 90. Only used if chartType() == Line and
   threeDLines() == true. The default is 30 degrees. If 0 degrees is
   specified for both the X and the Y rotation, the lines will look
   like 2D lines.

   \param rotation the rotation in degrees. Must be between 0 and
   90.
   \sa setThreeDLines(), threeDLines(), threeDLineYRotation()
   */

/**
  \fn int KDChartParams::threeDLineYRotation() const

  \deprecated Feature scheduled for future release, at present not implemented.
  */
/*
   \fn int KDChartParams::threeDLineYRotation() const

   Returns the rotation around the X axis in degrees. The value may
   be between 0 and 90. Only used if chartType() == Line and
   threeDLines() == true. The default is 30 degrees.

   \return the rotation in degrees. Is always between 0 and 90.
   */


/**
  \enum KDChartParams::AreaChartSubType

  The area subtype. Only used when chartType == Area

  \sa setAreaChartSubType, areaChartSubType
  */


/**
  \fn void KDChartParams::setAreaChartSubType( AreaChartSubType areaChartSubType )

  Specifies the area chart subtype. Only used if chartType() ==
  Area. The default is AreaNormal.

  \param areaChartSubType the area chart subtype
  \sa areaChartSubType, AreaChartSubType, setChartType, chartType
  */


/**
  \fn KDChartParams::AreaChartSubType KDChartParams::areaChartSubType() const


  Returns the area chart subtype. Only used if chartType() ==
  Area.

  \return the area chart sub type
  \sa setAreaChartSubType, AreaChartSubType, setChartType, chartType
  */


/**
  \enum KDChartParams::AreaLocation

  Specifies whether the area above the value points or below the
  value points should be filled. The default is to fill below the
  value points.

  \sa setAreaLocation(), areaLocation()
  */


/**
  \fn void KDChartParams::setAreaLocation( AreaLocation location )

  Specifies whether the area above or below the value points
  should be filled. The default is to fill below the value
  points.

  This setting is <em>not</em> used with percent areas
  (i.e. areaChartSubType() == AreaPercent), because these always
  cover the whole chart.

  \param location whether to fill above or below the value points
  \sa AreaLocation, areaLocation()
  */


/**
  \fn KDChartParams::AreaLocation KDChartParams::areaLocation() const

  Returns whether the area above or below the value points is
  filled.

  \return whether the area above or below the value points is
  filled.
  \sa AreaLocation, setAreaLocation()
  */


//@}
// END LINECHART

// START POLARCHART
/** @name Polar chart-specific methods.

  These methods query and set polar chart-specific parameters.
  */
//@{


/**
  \enum KDChartParams::PolarChartSubType

  The polar subtype. Only used when chartType == Polar

  \sa setPolarChartSubType, polarChartSubType
  */


/**
  \fn void KDChartParams::setPolarChartSubType( PolarChartSubType polarChartSubType )

  Specifies the polar chart subtype. Only used if chartType() ==
  Polar. The default is PolarNormal.

  \param polarChartSubType the polar chart subtype
  \sa polarChartSubType, PolarChartSubType, setChartType, chartType
  */


/**
  \fn KDChartParams::PolarChartSubType KDChartParams::polarChartSubType() const

  Returns the polar chart subtype. Only used if chartType() ==
  Polar.

  \return the polar chart sub type
  \sa setPolarChartSubType, PolarChartSubType, setChartType, chartType
  */


/**
  \fn void KDChartParams::setPolarMarker( bool marker )

  Specifies whether there should be a marker at each data
  point. Only used if chartType() == Polar. The default is not to
  draw markers.

  \param marker true if markers should be drawn
  */


/**
  \fn bool KDChartParams::polarMarker() const

  Returns whether polar markers should be drawn at each data
  point. Only used if chartType() == Polar.

  \return true if markers should be drawn.
  */


/**
  \enum KDChartParams::PolarMarkerStyle

  The available polar marker styles.
  */


/**
  Specifies the polar marker to be used for a dataset. Only used if
  chartType() == Polar and polarMarker() == true. If you specify a
  marker for a dataset, but not for a dataset with a lower
  value, then the marker for the dataset with the lower value
  will be undefined unless it was previously defined. The default
  is a circle for the first dataset, a square for the second, a
  diamond for the third and undefined for all subsequent
  datasets.

  \param dataset the dataset for which to set the polar marker
  \param style the style to set for the specified dataset
  \sa PolarMarkerStyle, polarMarkerStyle
  */
void KDChartParams::setPolarMarkerStyle( uint dataset, PolarMarkerStyle style )
{
    _polarMarkerStyles[ dataset ] = style;
    _maxDatasetPolarMarkerStyle = QMAX( dataset,
            _maxDatasetPolarMarkerStyle );
    emit changed();
}

/**
  Returns the marker to be used for a dataset. Only used if
  chartType() == Polar and polarMarker() == true.

  \param dataset the dataset for which to return the polar marker
  \return the polar marker for the specified data set
  \sa PolarMarkerStyle, setPolarMarkerStyle
  */
KDChartParams::PolarMarkerStyle KDChartParams::polarMarkerStyle( uint dataset ) const
{
    if( _polarMarkerStyles.find( dataset ) != _polarMarkerStyles.end() )
        return _polarMarkerStyles[ dataset ];
    else
        return PolarMarkerCircle; // default
}


/**
  \var KDChartParams::PolarMarkerStyleMap

  The type of the storage structure for polar marker styles. You
  should normally not need to use this.
  */



/**
  Sets a whole map of polar marker styles. You can use this for
  assigning many polar marker styles at the same time, but
  typically it is better to set them individually with \a
  setPolarMarkerStyle().

  \param map the map of styles
  \sa polarMarkerStyles(), setPolarMarkerStyle()
  */
void KDChartParams::setPolarMarkerStyles( PolarMarkerStyleMap map ) {
    _polarMarkerStyles = map;
    // update _maxDatasetPolarMarkerStyle
    uint maxDataset = 0;
    for( PolarMarkerStyleMap::ConstIterator it = _polarMarkerStyles.begin();
            it != _polarMarkerStyles.end(); ++it )
        maxDataset = QMAX( maxDataset, it.key() );
    _maxDatasetPolarMarkerStyle = maxDataset;
}


/**
  \fn KDChartParams::PolarMarkerStyleMap KDChartParams::polarMarkerStyles() const

  Returns the whole map of polar marker styles. You will usually
  not need this. You can query individual polar marker styles by
  calling \a polarMarkerStyle().

  \return the map of styles
  \sa polarMarkerStyle(), setPolarMarkerStyles()
  */


/**
  \fn uint KDChartParams::maxDatasetPolarMarkerStyle() const

  Returns the highest dataset for which a polar marker style has been
  defined. Not all datasets with a lower number necessarily have
  a defined polar marker.

  \return the highest dataset with a defined polar marker
  \sa PolarMarkerStyle, setPolarMarkerStyle, polarMarkerStyle
  */


/**
  \fn void KDChartParams::setPolarMarkerSize( QSize )

  Specifies the sizes of polar markers. Only used if chartType() == Polar
  and polarMarker() == true. The default is -40 x -40.

  \param size the size of the polar marker in pixels, if negative this is a
  per mille value of the chart min. size (the chart width or height, depending
  what is smaller), if positive the value is taken as absolute number of pixels.

  \sa polarMarkerSize
  */


/**
  \fn QSize KDChartParams::polarMarkerSize() const

  Returns the sizes of polar markers. Only used if chartType() ==
  Polar and polarMarker() == true.

  \return the size of the polar marker in pixels
  \sa setPolarMarkerSize
  */


/**
  \fn void KDChartParams::setPolarLineWidth( int width )

  Specifies the width for lines in polar charts. Default is -3.

  \param width the new width
  \sa polarLineWidth
  */


/**
  \fn int KDChartParams::polarLineWidth() const

  Returns the line width of the lines in polar charts.

  \return the line width of lines in polar charts
  \sa setPolarLineWidth
  */


/**
  \fn void KDChartParams::setPolarZeroDegreePos( int degrees )

  Specifies the zero degree position in polar charts.

  Use this function to have the zero degree point on a position
  different from the right side of the circle. Valid parameters
  are -359..359.

  \sa polarZeroDegreePos
  */


/**
  \fn int KDChartParams::polarZeroDegreePos() const

  Returns the zero degree position in polar charts.

  \sa setPolarZeroDegreePos
  */


/**
  \fn void KDChartParams::setPolarRotateCircularLabels( bool rotateCircularLabels )

  Specifies whether to rotate circular labels in polar charts.

  \sa polarRotateCircularLabels, setPolarDelimsAndLabelsAtPos
  */


/**
  \fn bool KDChartParams::polarRotateCircularLabels() const

  Returns whether circular labels will be rotated in polar charts.

  \sa setPolarRotateCircularLabels, polarDelimAtPos, polarLabelsAtPos
  */


/**
  Specifies whether to show circular delimiters and labels at a given
  position in polar charts.

  \note Value \c KDChartEnums::PosCenter will be ignored.

  \sa setPolarRotateCircularLabels, polarDelimAtPos, polarLabelsAtPos
  */
void KDChartParams::setPolarDelimsAndLabelsAtPos( KDChartEnums::PositionFlag pos,
        bool showDelimiters,
        bool showLabels )
{
    if( KDCHART_MAX_POLAR_DELIMS_AND_LABELS_POS >= pos ) {
        _polarDelimsAndLabels[ pos ].showDelimiters = showDelimiters;
        _polarDelimsAndLabels[ pos ].showLabels     = showLabels;
    }
}

/**
  Returns whether to show circular delimiters at a given
  position in polar charts.

  \sa polarRotateCircularLabels, setPolarDelimAndLabelsPos, polarLabelsAtPos
  */
bool KDChartParams::polarDelimAtPos( KDChartEnums::PositionFlag pos ) const
{
    if( KDCHART_MAX_POLAR_DELIMS_AND_LABELS_POS >= pos )
        return _polarDelimsAndLabels[ pos ].showDelimiters;
    else
        return false;
}

/**
  Returns whether to show circular labels at a given
  position in polar charts.

  \sa polarRotateCircularLabels, setPolarDelimAndLabelsPos, polarDelimAtPos
  */
bool KDChartParams::polarLabelsAtPos( KDChartEnums::PositionFlag pos ) const
{
    if( KDCHART_MAX_POLAR_DELIMS_AND_LABELS_POS >= pos )
        return _polarDelimsAndLabels[ pos ].showLabels;
    else
        return false;
}



//@}
// END LINECHART

// START RING/PIECHART
/** @name Ring and pie chart-specific methods.

  These methods query and set ring and pie chart-specific parameters.
  */
//@{

/**
  \fn void KDChartParams::setExplode( bool explode )

  Specifies whether the pie chart or ring chart should be
  exploding (all pies or ring segments are slightly displaced
  from the center) or not. The default is not to explode. Only
  used if chartType() == Pie or chartType() == Ring.

  Note that calling this method by passing true turns on
  exploding for all segments. You can turn exploding on and off
  for individual segments by calling setExplodeValues() and also
  change the explode factors by calling setExplodeFactor() or
  setExplodeFactors().

  Note that in ring charts, only the outermost ring is
  exploded. Also note that exploding rings are only useful if
  none of the values in the outermost dataset covers more than
  1/4 of the ring, otherwise the exploded ring will look funny.

  \param explode true if the pie or ring should be exploding,
  false otherwise
  \sa exploding(), setExplodeFactor(), explodeFactor(),
  setExplodeFactors(), explodeFactors(), setExplodeValues(),
  explodeValues()
  */


/**
  \fn bool KDChartParams::explode() const

  Returns whether the pie or ring chart should be exploding or not

  \return true if the pie should be exploding
  \sa setExplode, setExplodeFactor(), explodeFactor(),
  setExplodeFactors(), explodeFactors(), setExplodeValues(),
  explodeValues()
  */


/**
  \fn void KDChartParams::setExplodeValues( QValueList<int> explodeList )

  Specifies which values to explode. Explosion of values only
  happens if setExplode( true ) has been called. Normally, all
  values are exploded (all values on the outer ring in the case of a
  ring chart). With this function, you have a finer control about
  the explosion. You can specify the values that should be exploded
  by their position. Passing an empty list here turns on explosion
  for all pies or ring segments (but only if explosion is turned on
  in general).

  To repeat: Setting anything here has no effect if setExplode( true )
  is not called. You can, however, set any values here even if explosion is
  not turned on; they will be saved in case explosion will be turned on in
  the future and then be valid automatically.

  The explode factor, i.e., the amount with which a segment is moved
  from the center can either be set for all segments with
  \a setExplodeFactor() or for individual segments with
  \a setExplodeFactors(). The default is 10%.

  \param explodeList the list of positions in the displayed dataset that
  should be drawn in an exploded position. Pass an empty list here to
  explode all values.
  \sa setExplode(), explode(), setExplodeFactor(), explodeFactor(),
  explodeValues(), setExplodeFactors(), explodeFactors()
  */


/**
  \fn QValueList<int> KDChartParams::explodeValues() const

  Returns which values are exploded in a pie or ring chart. For a detailed
  explanation of this feature, please see \a setExplodeValues().

  \return the list of values that are exploded
  \sa setExplode(), explode(), setExplodeFactor(), explodeFactor(),
  setExplodeValues()
  */


/**
  \fn void KDChartParams::setExplodeFactors( QMap<int,double> factors )

  Specifies the explode factors for each segment in percent,
  i.e. how much an exploded pie or ring segment is displaced from
  the center. The factor is given as a double value between 0 and 1;
  0.1 means 10%. Only used if chartType() == Pie or chartType() ==
  Ring and explode() == true.

  Segments that are not contained in the map specified here will
  have the default explode factor of 10%, if exploding is turned on
  for them at all. This also means that passing an empty list to
  this method does not turn off exploding in general; use \a
  setExplode( false ) for that.

Note: This method has no immediate effect if setExplode( true )
has not been called. It is, however, possible to preset explode
factors and then turn on exploding later.

Note: Besides giving a segment an explode factor and turning on
exploding in general, you also need to specify to explode a
certain segment by calling \a setExplodeValues(). This gives maximum
flexibility as it allows you to preset explode factors and then
explode or not not explode a segment at leisure - at the expense
of one more method call.

\param factors the list of explode factors
\sa setExplode(), explode(), setExplodeValues(), explodeValues(),
setExplodeFactor(), explodeFactor(), explodeFactors()
*/


/**
  \fn QMap<int,double> KDChartParams::explodeFactors() const {

  Returns the list of explode factors. Each explode factor in the
  list corresponds to the segment at the same position.

  \return the list of explode factors
  \sa setExplode(), explode(), setExplodeValues(), explodeValues(),
  setExplodeFactor(), explodeFactor(), setExplodeFactors()
  */


/**
  \fn void KDChartParams::setExplodeFactor( double factor )

  Specifies the explode factor in percent, i.e. how much an
  exploded pie or ring segment is displaced from the center. The
  factor is given as a double value between 0 and 1; 0.1 means
  10% and is the default. Only used if
  chartType() == Pie or chartType() == Ring and explode() == true.

  Note that this method sets the explode factor for all segments
  that are exploded but for which no more specific explode factor
  has been set with \a setExplodeFactors(). Thus, to make the value
  specified here the default value, you can pass an empty list to
  \a setExplodeFactors() (which also is the default).

  \param factor the explode factor
  \sa setExplode(), explode(), explodeFactor(),
  setExplodeValues(), explodeValues(), setExplodeFactors(),
  explodeFactors()
  */


/**
  \fn double KDChartParams::explodeFactor() const

  Returns the default explode factor, i.e., the explode factor
  used for those segments which should be exploded but for which
  no more specific explode factor has been set. See \a
  setExplodeFactor() for an explanation of the value.

  \return the explode factor
  \sa setExplodeFactor(), setExplode(), explode()
  */


/**
  \fn void KDChartParams::setThreeDPies( bool threeDPies )

  Specifies whether the engine should draw the pies in 3D. Only
  used if chartType() == Pie.

  \param threeDPies true if pies should be drawn with a 3D effect
  \sa threeDPies(), setThreeDPieHeight(), threeDPieHeight()
  */


/**
  \fn bool KDChartParams::threeDPies() const

  Returns whether the engine should draw any pies in 3D. Only
  used if chartType() == Pie.

  \return true if pies should be drawn with a 3D effect, false
  otherwise
  \sa setThreeDPies(), threeDPieHeight(), setThreeDPieHeight()
  */


/**
  \fn void KDChartParams::setThreeDPieHeight( int pixels )

  Specifies the height of the 3D effect for pies in pixels. Only used if
  chartType() == Pie and threeDPies() == true. Negative values are
  interpreted relative to the height of the pie where -100 means
  that the 3D effect has the same height as the pie itself; values
  smaller than -25 are hardly useful. Positive values are
  interpreted as absolute sizes in pixels. The default is an
  absolute value of 20 pixels.

  \param height the height of the 3D effect in pixels
  \sa threeDHeight(), setThreeDPies(), threeDPies()
  */


/**
  \fn int KDChartParams::threeDPieHeight() const

  Returns the height of the 3D effect for pies in pixels. See \a
  setThreeDPieHeight for the interpretation of the value.

  \return the height of the 3D effect in pixels
  \sa setThreeDPieHeight(), setThreeDPies(), threeDPies()
  */


/**
  \fn void KDChartParams::setPieStart( int degrees )

  Specifies the starting point of the pie circle. The default is the 3 o'
  clock position which equals to 0. Positive values are angles in degrees
  counterclockwise, negative values are angles in degrees
  clockwise. All values will be normalized to [0;360[.

  \param degrees the starting point in degrees
  \sa pieStart()
  */


/**
  \fn int KDChartParams::pieStart() const

  Returns the starting point of the pie circle. See \a setPieStart() for
  the interpretation of these values. The return value is
  always in the interval [0;360[

  \return the starting point of the pie circle in degrees
  \sa setPieStart()
  */


/**
  \fn void KDChartParams::setRingStart( int degrees )

  Specifies the starting point of the ring circle. The default is the 3 o'
  clock position which equals to 0. Positive values are angles in degrees
  counterclockwise, negative values are angles in degrees
  clockwise. All values will be normalized to [0;360[.

  \param degrees the starting point in degrees
  \sa pieStart()
  */


/**
  \fn int KDChartParams::ringStart() const

  Returns the starting point of the ring circle. See \a
  setRingStart() for the interpretation of these values. The
  return value is always in the interval [0;360[

  \return the starting point of the ring circle in degrees
  \sa setRingStart()
  */


/**
  \fn void KDChartParams::setRelativeRingThickness( bool relativeThickness )

  Specifies whether the ring thickness should be relative to the sum of the
  values in the dataset that the ring represents. The default is to have all
  the rings with the same thickness which itself depends on the size of the
  chart and the number of rings.

  \param relativeThickness if true, ring thickness is relative, if false, it
  is constant.
  \sa relativeThickness()
  */


/**
  \fn bool KDChartParams::relativeRingThickness() const

  Returns whether the ring thickness is relative to the sum of values in the
  dataset that the ring represents, or whether the ring thickness should be
  constant.

  \return true if thickness is relative, false if it is constant
  \sa setRelativeRingThickness()
  */



//@}
// END RING/PIECHART

// START HILO CHART
/** @name Hi-lo chart-specific methods.

  These methods query and set hi-lo chart-specific parameters.
  */
//@{

/**
  \enum KDChartParams::HiLoChartSubType

  The HiLo subtype. Only used when chartType == HiLo

  \sa setHiLoChartSubType, hiLoChartSubType
  */

/**
  \fn void KDChartParams::setHiLoChartSubType( HiLoChartSubType hiLoChartSubType )

  Specifies the HiLo chart subtype. Only used if chartType() ==
  HiLo

  \param hiLoChartSubType the HiLo chart subtype
  \sa hiLoChartSubType, HiLoChartSubType, setChartType, chartType
  */


/**
  \fn KDChartParams::HiLoChartSubType KDChartParams::hiLoChartSubType() const

  Returns the HiLo chart subtype. Only used if chartType() ==
  HiLo.

  \return the HiLo chart sub type
  \sa setHiLoChartSubType, HiLoChartSubType, setChartType, chartType
  */


/**
  Specifies if and how a HiLo chart will print the Low
  values below their respective entries. Only used if chartType() == HiLo

  \note <b>setHiLoChartPrintLowValues( false )</b> will
  deactivate printing of Low values.

  \param active specifies whether the values are to be printed or not.
  \param font a Pointer to the font to be used.
  \param size (in per mille of the chart width) the dynamic size of
  the font to be used. If this parameter is zero the size of the
  \c font is used instead - regardless of the size of the chart!
  \param color the color to be used when printing the values.
  \sa setChartType, chartType
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setHiLoChartPrintHighValues
  \sa hiLoChartPrintLowValues, hiLoChartPrintLowValues
  \sa hiLoChartLowValuesFontColor, hiLoChartLowValuesFontColor
  \sa hiLoChartLowValuesFontUseRelSize, hiLoChartLowValuesFontUseRelSize
  \sa hiLoChartLowValuesFontRelSize, hiLoChartLowValuesFontRelSize
  \sa hiLoChartPrintHighValues, hiLoChartPrintHighValues
  \sa hiLoChartHighValuesFontColor, hiLoChartHighValuesFontColor
  \sa hiLoChartHighValuesFontUseRelSize, hiLoChartHighValuesFontUseRelSize
  \sa hiLoChartHighValuesFontRelSize, hiLoChartHighValuesFontRelSize
  */
void KDChartParams::setHiLoChartPrintLowValues( bool active, QFont* font,
        int size, QColor* color )
{
    _hiLoChartPrintLowValues = active;
    if ( font )
        _hiLoChartLowValuesFont = *font;
    else
        _hiLoChartLowValuesFont = _defaultFont;
    _hiLoChartLowValuesUseFontRelSize = ( 0 < size );
    _hiLoChartLowValuesFontRelSize = size;
    if ( 0 == color )
        _hiLoChartLowValuesColor = QColor( 0, 0, 0 );
    else
        _hiLoChartLowValuesColor = *color;
    emit changed();
}


/**
  \fn bool KDChartParams::hiLoChartPrintLowValues() const

  Returns whether the low values
  will be printed under their respective entries.

  \return whether the low values
  will be printed under their respective entries.

  \sa setHiLoChartPrintLowValues, setHiLoChartPrintHighValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  \fn QFont KDChartParams::hiLoChartLowValuesFont() const

  Returns the font to be used for printing the
  low values

  \returns the font to be used for printing the
  low values

  \sa setHiLoChartPrintLowValues, setHiLoChartPrintHighValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  \fn bool KDChartParams::hiLoChartLowValuesUseFontRelSize() const

  Returns whether the font size to be used for printing the
  low values is calculated dynamically.

  \return whether the font size to be used for printing the
  low values is calculated dynamically.

  \sa setHiLoChartPrintLowValues, setHiLoChartPrintHighValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  \fn int KDChartParams::hiLoChartLowValuesFontRelSize() const

  Returns the relative size (in per mille of the chart width)
  of font size to be used for printing the
  low values.

  \return the relative size (in per mille of the chart width)
  of font size to be used for printing the
  low values.

  \sa setHiLoChartPrintLowValues, setHiLoChartPrintHighValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  \fn QColor KDChartParams::hiLoChartLowValuesColor() const

  Returns the colour of the font size to be used for printing the
  low values.

  \return the colour of the font size to be used for printing the
  low values.

  \sa setHiLoChartPrintLowValues, setHiLoChartPrintHighValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  Specifies if and how a HiLo chart will print the High
  values below their respective entries.
  Only used if chartType() == HiLo

  \note <b>setHiLoChartPrintHighValues( false )</b> will
  deactivate printing of High values.

  \param active specifies whether the values are to be printed or not.
  \param font specifies a Pointer to the font to be used.
  \param size (in per mille of the chart width) specifies the
  dynamic size of the font to be used. If this parameter is zero
  the size of the
  \c font is used instead - regardless of the size of the chart!
  \param specifies the color the color to be used when printing
  the values.
  \sa setHiLoChartPrintLowValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  \sa hiLoChartPrintHighValues, hiLoChartPrintHighValues
  \sa hiLoChartHighValuesFontColor, hiLoChartHighValuesFontColor
  \sa hiLoChartHighValuesFontUseRelSize, hiLoChartHighValuesFontUseRelSize
  \sa hiLoChartHighValuesFontRelSize, hiLoChartHighValuesFontRelSize
  \sa hiLoChartPrintLowValues, hiLoChartPrintLowValues
  \sa hiLoChartLowValuesFontColor, hiLoChartLowValuesFontColor
  \sa hiLoChartLowValuesFontUseRelSize, hiLoChartLowValuesFontUseRelSize
  \sa hiLoChartLowValuesFontRelSize, hiLoChartLowValuesFontRelSize
  */
void KDChartParams::setHiLoChartPrintHighValues( bool active, QFont* font,
        int size, QColor* color )
{
    _hiLoChartPrintHighValues = active;
    if ( font )
        _hiLoChartHighValuesFont = *font;
    else
        _hiLoChartHighValuesFont = _defaultFont;
    _hiLoChartHighValuesUseFontRelSize = ( 0 < size );
    _hiLoChartHighValuesFontRelSize = size;
    if ( 0 == color )
        _hiLoChartHighValuesColor = QColor( 0, 0, 0 );
    else
        _hiLoChartHighValuesColor = *color;
    emit changed();
}


/**
  \fn bool KDChartParams::hiLoChartPrintHighValues() const

  Returns whether the high values
  will be printed under their respective entries.

  \return whether the high values
  will be printed under their respective entries.

  \sa setHiLoChartPrintHighValues, setHiLoChartPrintLowValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  \fn QFont KDChartParams::hiLoChartHighValuesFont() const

  Returns the font to be used for printing the
  high values.

  \returns the font to be used for printing the
  high values.

  \sa setHiLoChartPrintHighValues, setHiLoChartPrintLowValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  \fn bool KDChartParams::hiLoChartHighValuesUseFontRelSize() const

  Returns whether the font size to be used for printing the
  high values is calculated dynamically.

  \return whether the font size to be used for printing the
  high values is calculated dynamically.

  \sa setHiLoChartPrintHighValues, setHiLoChartPrintLowValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  \fn int KDChartParams::hiLoChartHighValuesFontRelSize() const

  Returns the relative size (in per mille of the chart width)
  of font size to be used for printing the
  high values.

  \return the relative size (in per mille of the chart width)
  of font size to be used for printing the
  high values.

  \sa setHiLoChartPrintHighValues, setHiLoChartPrintHighValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  \fn QColor KDChartParams::hiLoChartHighValuesColor() const

  Returns the colour of the font size to be used for printing the
  high values.

  \return the colour of the font size to be used for printing the
  high values.

  \sa setHiLoChartPrintHighValues, setHiLoChartPrintLowValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  Specifies if and how a HiLo chart will print the Open values
  below their respective entries.
  Only used if chartType() == HiLo and if the HiLo chart sub type
  shows open values.

  \note <b>setHiLoChartPrintOpenValues( false )</b> will
  deactivate printing of Open values.

  \param active specifies whether the values are to be printed or not.
  \param font a Pointer to the font to be used.
  \param size (in per mille of the chart width) the dynamic size of
  the font to be used. If this parameter is zero the size of the
  \c font is used instead - regardless of the size of the chart!
  \param color the color to be used when printing the values.
  \sa setHiLoChartPrintCloseValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  \sa hiLoChartPrintOpenValues, hiLoChartPrintOpenValues
  \sa hiLoChartOpenValuesFontColor, hiLoChartOpenValuesFontColor
  \sa hiLoChartOpenValuesFontUseRelSize, hiLoChartOpenValuesFontUseRelSize
  \sa hiLoChartOpenValuesFontRelSize, hiLoChartOpenValuesFontRelSize
  \sa hiLoChartPrintCloseValues, hiLoChartPrintCloseValues
  \sa hiLoChartCloseValuesFontColor, hiLoChartCloseValuesFontColor
  \sa hiLoChartCloseValuesFontUseRelSize, hiLoChartCloseValuesFontUseRelSize
  \sa hiLoChartCloseValuesFontRelSize, hiLoChartCloseValuesFontRelSize
  */
void KDChartParams::setHiLoChartPrintOpenValues( bool active, QFont* font,
        uint size, QColor* color )
{
    _hiLoChartPrintOpenValues = active;
    if ( font )
        _hiLoChartOpenValuesFont = *font;
    else
        _hiLoChartOpenValuesFont = _defaultFont;
    _hiLoChartOpenValuesUseFontRelSize = ( 0 < size );
    _hiLoChartOpenValuesFontRelSize = size;
    if ( 0 == color )
        _hiLoChartOpenValuesColor = QColor( 0, 0, 0 );
    else
        _hiLoChartOpenValuesColor = *color;
    emit changed();
}


/**
  \fn bool KDChartParams::hiLoChartPrintOpenValues() const

  Returns whether the open values will be printed under their
  respective entries.

  \return whether the open values will be printed under their
  respective entries.

  \sa setHiLoChartPrintOpenValues, setHiLoChartPrintCloseValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  \fn QFont KDChartParams::hiLoChartOpenValuesFont() const

  Returns the font to be used for printing the
  open values.

  \returns the font to be used for printing the
  open values.

  \sa setHiLoChartPrintOpenValues, setHiLoChartPrintCloseValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  \fn bool KDChartParams::hiLoChartOpenValuesUseFontRelSize() const

  Returns whether the font size to be used for printing the
  open values is calculated dynamically.

  \return whether the font size to be used for printing the
  open values is calculated dynamically.

  \sa setHiLoChartPrintOpenValues, setHiLoChartPrintCloseValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  \fn int KDChartParams::hiLoChartOpenValuesFontRelSize() const

  Returns the relative size (in per mille of the chart width)
  of font size to be used for printing the open values.

  \return the relative size (in per mille of the chart width) of
  font size to be used for printing the open values.

  \sa setHiLoChartPrintOpenValues, setHiLoChartPrintCloseValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  \fn QColor KDChartParams::hiLoChartOpenValuesColor() const

  Returns the colour of the font size to be used for printing the
  open values.

  \return the colour of the font size to be used for printing the
  open values.

  \sa setHiLoChartPrintOpenValues, setHiLoChartPrintCloseValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  Specifies if and how a HiLo chart will print the Close
  values next to their respective entries.
  Only used if chartType() == HiLo and the HiLo chart sub type
  contains Close values.

  \note <b>setHiLoChartPrintCloseValues( false )</b> will
  deactivate printing of Close values.

  \param active specifies whether the values are to be printed or not.
  \param font specifies a Pointer to the font to be used.
  \param size (in per mille of the chart width) specifies the dynamic size of
  the font to be used. If this parameter is zero the size of the
  \c font is used instead - regardless of the size of the chart!
  \param specifies the color the color to be used when printing the values.
  \sa setHiLoChartPrintOpenValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  \sa hiLoChartPrintCloseValues, hiLoChartPrintCloseValues
  \sa hiLoChartCloseValuesFontColor, hiLoChartCloseValuesFontColor
  \sa hiLoChartCloseValuesFontUseRelSize, hiLoChartCloseValuesFontUseRelSize
  \sa hiLoChartCloseValuesFontRelSize, hiLoChartCloseValuesFontRelSize
  \sa hiLoChartPrintOpenValues, hiLoChartPrintOpenValues
  \sa hiLoChartOpenValuesFontColor, hiLoChartOpenValuesFontColor
  \sa hiLoChartOpenValuesFontUseRelSize, hiLoChartOpenValuesFontUseRelSize
  \sa hiLoChartOpenValuesFontRelSize, hiLoChartOpenValuesFontRelSize
  */
void KDChartParams::setHiLoChartPrintCloseValues( bool active, QFont* font,
        int size, QColor* color )
{
    _hiLoChartPrintCloseValues = active;
    if ( font )
        _hiLoChartCloseValuesFont = *font;
    else
        _hiLoChartCloseValuesFont = _defaultFont;
    _hiLoChartCloseValuesUseFontRelSize = ( 0 < size );
    _hiLoChartCloseValuesFontRelSize = size;
    if ( 0 == color )
        _hiLoChartCloseValuesColor = QColor( 0, 0, 0 );
    else
        _hiLoChartCloseValuesColor = *color;
    emit changed();
}

/**
  \fn bool KDChartParams::hiLoChartPrintCloseValues() const

  Returns whether the close values will be printed under their
  respective entries.

  \return whether the close values will be printed under their
  respective entries.

  \sa setHiLoChartPrintCloseValues, setHiLoChartPrintOpenValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  \fn QFont KDChartParams::hiLoChartCloseValuesFont() const

  Returns the font to be used for printing the close values.

  \returns the font to be used for printing the close values.

  \sa setHiLoChartPrintCloseValues, setHiLoChartPrintOpenValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  \fn bool KDChartParams::hiLoChartCloseValuesUseFontRelSize() const

  Returns whether the font size to be used for printing the close
  values is calculated dynamically.

  \return whether the font size to be used for printing the close
  values is calculated dynamically.

  \sa setHiLoChartPrintCloseValues, setHiLoChartPrintOpenValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  \fn int KDChartParams::hiLoChartCloseValuesFontRelSize() const
  Returns the relative size (in per mille of the chart width) of
  font size to be used for printing the close values.

  \return the relative size (in per mille of the chart width) of
  font size to be used for printing the close values.

  \sa setHiLoChartPrintCloseValues, setHiLoChartPrintOpenValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  \fn QColor KDChartParams::hiLoChartCloseValuesColor() const

  Returns the colour of the font size to be used for printing the
  close values.

  \return the colour of the font size to be used for printing the
  close values.

  \sa setHiLoChartPrintCloseValues, setHiLoChartPrintOpenValues
  \sa setHiLoChartSubType, hiLoChartSubType
  \sa setChartType, chartType
  */


/**
  \fn void KDChartParams::setHiLoChartPrintFirstValues( bool, QFont*, uint, QColor* )

  \obsolete

  This method is obsolete; use \a setHiLoChartPrintOpenValues().
  */


/**
  \fn bool KDChartParams::hiLoChartPrintFirstValues() const

  \obsolete

  This method is obsolete; use \a hiLoChartPrintOpenValues()
  instead.
  */


/**
  \fn QFont KDChartParams::hiLoChartFirstValuesFont() const

  \obsolete

  This method is obsolete; use \a hiLoChartOpenValuesFont()
  instead.
  */


/**
  \fn bool KDChartParams::hiLoChartFirstValuesUseFontRelSize() const

  \obsolete

  This method is obsolete; use \a
  hiLoChartOpenValuesUseFontRelSize() instead.
  */


/**
  \fn int KDChartParams::hiLoChartFirstValuesFontRelSize() const

  \obsolete

  This method is obsolete; use \a
  hiLoChartOpenValuesFontRelSize() instead.
  */


/**
  \fn QColor KDChartParams::hiLoChartFirstValuesColor() const

  \obsolete

  This method is obsolete; use \a hiLoChartOpenValuesColor()
  instead.
  */


/**
  \fn void KDChartParams::setHiLoChartPrintLastValues( bool, QFont*, int, QColor* )

  \obsolete

  This method is obsolete; use \a setHiLoChartPrintCloseValues()
  instead.
  */


/**
  \fn bool KDChartParams::hiLoChartPrintLastValues() const

  \obsolete

  This method is obsolete; use \a hiLoChartPrintCloseValues()
  instead.
  */


/**
  \fn QFont KDChartParams::hiLoChartLastValuesFont() const

  \obsolete

  This method is obsolete; use \a hiLoChartCloseValuesFont()
  instead.
  */


/**
  \fn bool KDChartParams::hiLoChartLastValuesUseFontRelSize() const

  \obsolete

  This method is obsolete; use \a hiLoChartCloseValuesUseFontRelSize()
  instead.
  */


/**
  \fn int KDChartParams::hiLoChartLastValuesFontRelSize() const

  \obsolete

  This method is obsolete; use \a hiLoChartCloseValuesFontRelSize()
  instead.
  */


/**
  \fn QColor KDChartParams::hiLoChartLastValuesColor() const

  \obsolete

  This method is obsolete; use \a hiLoChartCloseValuesColor()
  instead.
  */



//@}
// END HILO CHART

// START BOX/WHISKER CHART
/** @name Box-and-whisker chart-specific methods.

  These methods query and set box-and-whisker chart-specific parameters.
  */
//@{

/**
  \enum KDChartParams::BWChartSubType

  The BoxWhisker subtype. Only used when chartType == BoxWhisker

  \sa setBWChartSubType, bWChartSubType
  */


/**
  \enum KDChartParams::BWStatVal

  The different statistical values that can be printed into or
  asked from a Box and Whisker chart. Only used when chartType == BoxWhisker

  \sa setBWChartSubType, bWChartSubType
  */               // use the following syntax to avoid warnings:


/**
  \fn void KDChartParams::setBWChartSubType( BWChartSubType bWChartSubType )

  Specifies the BoxWhisker chart subtype. Only used if chartType() ==
  BoxWhisker

  \param bWChartSubType the BoxWhisker chart subtype
  \sa bWChartSubType, BWChartSubType, setChartType, chartType
  */


/**
  \fn KDChartParams::BWChartSubType KDChartParams::bWChartSubType() const

  Returns the BoxWhisker chart subtype. Only used if chartType() ==
  BoxWhisker.

  \return the BoxWhisker chart sub type
  \sa setBWChartSubType, BWChartSubType, setChartType, chartType
  */



/**
  Specifies the factors to be used to calculate the position of
  the inner (or outer, resp.) fences.

  Default values for both inner fences are 1.5 times the
  interquartile range from the corresponding quartile,
  for both outer fences default is 3.0 times the IQR.

  \sa bWChartFences
  \sa setBWChartOutValMarkerSize, setBWChartPrintStatistics
  \sa setBWChartSubType, bWChartSubType
  \sa setChartType, chartType
  */
void KDChartParams::setBWChartFences( double upperInner, double lowerInner,
        double upperOuter, double lowerOuter )
{
    _BWChartFenceUpperInner = upperInner;
    _BWChartFenceLowerInner = lowerInner;
    _BWChartFenceUpperOuter = upperOuter;
    _BWChartFenceLowerOuter = lowerOuter;
}
/**
  Retrievs the factors to be used to calculate the position of
  the inner (or outer, resp.) fences.

  \sa setBWChartFences
  \sa setBWChartOutValMarkerSize, setBWChartPrintStatistics
  \sa setBWChartSubType, bWChartSubType
  \sa setChartType, chartType
  */
void KDChartParams::bWChartFences( double& upperInner, double& lowerInner,
        double& upperOuter, double& lowerOuter ) const
{
    upperInner = _BWChartFenceUpperInner;
    lowerInner = _BWChartFenceLowerInner;
    upperOuter = _BWChartFenceUpperOuter;
    lowerOuter = _BWChartFenceLowerOuter;
}


/**
  \fn void KDChartParams::setBWChartBrush( const QBrush& bWChartBrush )

  Specifies the brush to be used to fill the BoxWhisker charts.

  \param bWChartBrush the brush to be used
  \sa bWChartBrush, setChartType, chartType
  */


/**
  \fn QBrush KDChartParams::bWChartBrush() const

  Returns the brush to be used to fill the BoxWhisker charts.

  \return the brush to be used
  \sa setBWChartBrush, setChartType, chartType
  */


/**
  \fn void KDChartParams::setBWChartOutValMarkerSize( int size )

  Specifies how big outlier markers (representing the values outside
  the inner fences) will be drawn.

  When specifying a value different from 0 the chart will show the outlier
  markers in the respective size, use zero to disable showing the outliers,
  use negative values to have the marker size calculated dynamically
  (in percent of the current boxes' width).  Thus marker sizes will be
  recalculated after modification of either the number of boxes
  or the size of the entire chart.

  \note Shapes of the outlier markers depend on their distance to the
  respective fence: circles are used to represent near outside values
  while asteriscs indicate far outside values (the ones outside the
  outer fence), you may use setBWChartFences() to specify the factor
  to be used to calculate the value of the inner and outer fence.

  \sa bWChartOutValMarkerSize
  \sa setBWChartFences, setBWChartPrintStatistics
  \sa setBWChartSubType, bWChartSubType
  \sa setChartType, chartType
  */


/**
  \fn int KDChartParams::bWChartOutValMarkerSize() const

  Returns how big outlier markers (representing the values outside
  the inner fences) will be drawn.

  \return how big outside value markers will be drawn, zero means
  they will be invisible, negative values indicate that the size
  is calculated dynamically based upon the current boxes' width.

  \sa setBWChartOutValMarkerSize
  \sa setBWChartFences, setBWChartPrintStatistics
  \sa setBWChartSubType, bWChartSubType
  \sa setChartType, chartType
  */


/**
  Specifies if and how a BoxWhisker chart will print the
  respective statistical values near to the drawing.
  Only used if chartType() == BoxWhisker

  \note <b>setBWChartPrintStatistics( BWStatVal, false )</b> will
  deactivate printing of the respective statistical values.

  \param statValue specifies the statistical value to be set, use
  BWStatValALL to define equal settings for all
  of the statistical values.
  \param active specifies whether the values are to be printed or not.
  \param font a Pointer to the font to be used.
  \param size (relative to the boxes' width) the dynamic size of
  the font to be used. If this parameter is zero the size of the
  \c font is used instead - regardless of the size of the chart
  and regardless of the number of boxes!
  \param color the color to be used when printing the values.
  \param brush the brush to be used to fill the text's area before printing
  \sa setChartType
  \sa setBWChartSubType
  \sa bWChartPrintStatistics
  \sa bWChartStatisticsColor,
  \sa bWChartStatisticsBrush,
  \sa bWChartStatisticsFontUseRelSize
  \sa bWChartStatisticsFontRelSize
  */
void KDChartParams::setBWChartPrintStatistics( BWStatVal statValue,
        bool active,
        QFont* font,
        int size,
        QColor* color,
        QBrush* brush )
{
    BWStatVal statValA = (BWStatValALL == statValue) ? BWStatValSTART : statValue;
    BWStatVal statValZ = (BWStatValALL == statValue) ? BWStatValEND   : statValue;
    for( int i = statValA; i <= statValZ; ++i ){
        _BWChartStatistics[ i ].active = active;
        if ( font )
            _BWChartStatistics[ i ].font = *font;
        else
            _BWChartStatistics[ i ].font = _defaultFont;
        _BWChartStatistics[ i ].useRelSize = ( 0 < size );
        _BWChartStatistics[ i ].relSize = size;
        if ( 0 == color )
            _BWChartStatistics[ i ].color = QColor( 0, 0, 0 );
        else
            _BWChartStatistics[ i ].color = *color;
        if ( 0 == brush )
            _BWChartStatistics[ i ].brush = QBrush( Qt::white );
        else
            _BWChartStatistics[ i ].brush = *brush;
    }
    emit changed();
}


/**
  \fn bool KDChartParams::bWChartPrintStatistics( BWStatVal statValue ) const

  Returns whether the respective statistical values
  will be printed near to the drawing.

  \return whether the respective statistical values
  will be printed near to the drawing.

  \sa setBWChartPrintStatistics
  \sa setBWChartSubType, bWChartSubType
  \sa setChartType, chartType
  */


/**
  \fn QFont KDChartParams::bWChartStatisticsFont( BWStatVal statValue ) const

  Returns the font to be used for printing the respective statistical values

  \returns the font to be used for printing the respective statistical values

  \sa setBWChartPrintStatistics
  \sa setBWChartSubType, bWChartSubType
  \sa setChartType, chartType
  */


/**
  \fn bool KDChartParams::bWChartStatisticsUseRelSize( BWStatVal statValue ) const

  Returns whether the font size to be used for printing the respective statistical values
  is calculated dynamically.

  \returns whether the font size to be used for printing the respective statistical values
  is calculated dynamically.

  \sa setBWChartPrintStatistics
  \sa setBWChartSubType, bWChartSubType
  \sa setChartType, chartType
  */


/**
  \fn int KDChartParams::bWChartStatisticsFontRelSize( BWStatVal statValue ) const

  Returns the relative size (in per mille of the chart width)
  of the font to be used for printing the respective statistical values.

  \returns the relative size (in per mille of the chart width)
  of the font to be used for printing the respective statistical values.

  \sa setBWChartPrintStatistics
  \sa setBWChartSubType, bWChartSubType
  \sa setChartType, chartType
  */


/**
  \fn QColor KDChartParams::bWChartStatisticsColor( BWStatVal statValue ) const

  Returns the colour
  of the font to be used for printing the respective statistical values.

  \returns the colour
  of the font to be used for printing the respective statistical values.

  \sa setBWChartPrintStatistics
  \sa setBWChartSubType, bWChartSubType
  \sa setChartType, chartType
  */


/**
  \fn QBrush KDChartParams::bWChartStatisticsBrush( BWStatVal statValue ) const

  Returns the brush used
  to fill the text's area before printing the respective statistical values.

  \returns the brush used
  to fill the text's area before printing the respective statistical values.

  \sa setBWChartPrintStatistics
  \sa setBWChartSubType, bWChartSubType
  \sa setChartType, chartType
  */


//@}
// END BOX/WHISKER CHART

// START LEGENDS
/** @name Legend methods.

  These methods query and set legend parameters.
  */
//@{

/**
  \enum KDChartParams::LegendPosition

  The possible positions for a legend. NoLegend turns legend display
  off.

  \sa setLegendPosition, legendPosition
  */


/**
  \fn void KDChartParams::setLegendPosition( LegendPosition position )

  Specifies where the legend should be shown. NoLegend turns legend
  display off. The default is LegendRight which positions the legend
  right to the data display.

  \param the position for the legend
  \sa LegendPosition, legendPosition, setLegendOrientation, setLegendShowLines
  */


/**
  \fn KDChartParams::LegendPosition KDChartParams::legendPosition() const

  Returns where the legend will be shown.

  \return where the legend will be shown
  \sa LegendPosition, setLegendPosition, setLegendShowLines
  */


/**
  \fn void KDChartParams::setLegendOrientation( Qt::Orientation orientation )

  Specifies how the legend should be printed. Qt::Vertical (the default)
  prints the legend entries below each other, Qt::Horizontal prints them
  aside each other.

  \note Horizontal orientation is only possible if the chart is NOT making
        room in horizontal direction, this means you may specify horizontal
        orientation if the legend position in one of the following values
        only: LegendTop, LegendBottom, LegendTopLeftTop, LegendTopRightTop,
        LegendBottomLeftBottom, LegendBottomRightBottom

  \param the orientation for the legend
  \sa legendOrientation, setLegendPosition, setLegendShowLines
  */


/**
  \fn Qt::Orientation KDChartParams::legendOrientation() const

  Returns how the legend will be printed.

  \return how the legend will be printed.
  \sa setLegendOrientation, setLegendPosition, setLegendShowLines
  */

/**
  \fn void KDChartParams::setLegendShowLines( bool legendShowLines )

  Specifies whether the legend shall show lines or just
  show the markers (or squares, resp.).

  \note By default no lines are drawn but just the markers are shown,
        for Line charts you might want to set this flag to true.

  \param flag whether lines are drawn or only the markers
  \sa legendShowLines, setLegendOrientation, setLegendPosition
  */


/**
  \fn bool KDChartParams::legendShowLines() const

  Returns whether the legend shows lines or just
  the markers (or squares, resp.).

  \return  whether the legend shows lines or just the markers (or squares, resp.).
  \sa setLegendShowLines, setLegendOrientation, setLegendPosition
  */



/**
  \enum KDChartParams::LegendSource

  The possible ways for getting the legend text. Available are:
  <ul>
  <li>Manual - Uses texts set with setLegendText(); if no text is set for
  a dataset, the legend text will be empty.
  <li>FirstColumn - Uses values from the first column, no matter what
  this contains.
  <li>Automatic - Tries first to use values from the first column; if
  this does not contain any string values, tries to use values set
  manually with setLegendText(). If there are no values set manually
  either, resolves to standard texts like Series 1, Series 2, .... This
  is the default.
  </ul>
  \sa setLegendSource, legendSource, setLegendText, legendText
  */


/**
  \fn void KDChartParams::setLegendSource( LegendSource source )

  Specifies where the legend text is coming from. See LegendSource for an
  explanation of the possible values.

  \param source the source of the legend texts
  \sa legendSource, LegendSource, setLegendText, legendText
  */


/**
  \fn LegendSource KDChartParams::legendSource() const

  Returns where the legend text is coming from. See LegendSource for an
  explanation of the possible return values.

  \return where the legend text is coming from
  \sa LegendSource, setLegendSource, setLegendText, legendText
  */



/**
  \fn void KDChartParams::setLegendText( uint dataset, const QString& text )

  Manually specifies a legend text for a certain dataset. This is only
  used if legendSource() == Manual or if legendSource() == Automatic and
  it is not possible to retrieve values from the first column.

  \param dataset the dataset for which to set the legend text
  \param the legend text to be set for a given dataset
  \sa LegendSource, setLegendSource, legendSource, legendText
  */


/**
  \fn QString KDChartParams::legendText( uint dataset ) const

  Returns the manually specified legend text for a given dataset.

  \return the manually specified legend text for the specified dataset
  \sa setLegendText, LegendSource, setLegendSource, legendSource
  */


/**
  \fn void KDChartParams::setLegendTextColor( const QColor& color )

  Specifies the color to be used for the legend texts. The
  default is black.

  \param color the color to be used for the legend texts
  \sa legendTextColor(), setLegendTitleTextColor()
  */


/**
  \fn QColor KDChartParams::legendTextColor() const

  Returns the color in which the legend texts are drawn. The
  default is black.

  \return the color in which the legend texts are drawn
  \sa setLegendTextColor(), legendTitleTextColor()
  */


/**
  \fn void KDChartParams::setLegendFont( const QFont& font, bool useFontSize )

  Specifies the font in which the legend texts are drawn. The default is a
  10pt helvetica font.

  \note The font size will be ignored if \c useFontSize is false,
  in this case the font size will be calculated dynamically using
  the value stored by you calling setLegendFontRelSize().

  \param font the font to draw the legend texts in
  \sa setLegendFontUseRelSize, legendFont
  */


/**
  \fn QFont KDChartParams::legendFont() const

  Returns the font in which the legend texts are drawn.

  \return the font in which the legend texts are drawn
  \sa setLegendFont, setLegendFontUseRelSize
  */


/**
  \fn void KDChartParams::setLegendFontUseRelSize( bool legendFontUseRelSize )

  Specifies whether the legend shall be drawn
  using relative font size.

  \param legendFontUseRelSize whether legend shall be drawn
  using relative font size.
  If true the absolute value of the value set by \c
  setLegendFontRelSize is per thousand
  of of the average value of the printable area height and width.
  This will make the legend look nice even if scaled to very
  different size.

  \sa setLegendFontRelSize, setAxisLabelsFont
  */


/**
  \fn bool KDChartParams::legendFontUseRelSize() const

  Returns whether the legend font size is used.

  \return whether the fix legend font size is used.
  \sa setLegendFontRelSize, setLegendFont
  */


/**
  \fn void KDChartParams::setLegendFontRelSize( int legendFontRelSize )

  Specifies the legend relative font size.

  \param legendFontRelSize the relative legend size.
  If this value unequals zero the absolute value is per thousand
  of of the average value of the printable area height and width.
  This will make the legend look nice even if scaled to very
  different size.

  \sa setLegendFontUseRelSize, setLegendFont
  */


/**
  \fn int KDChartParams::legendFontRelSize() const

  Returns the relative legend font size.

  \return the relative legend font size.
  \sa setLegendFontRelSize, setLegendFontUseRelSize
  */


/**
  \fn void KDChartParams::setLegendTitleText( const QString& text )

  Specifies a text to be shown as the title of the legend. The
  default is "Legend" (possibly localized). To turn off the
  legend title completely, pass an empty string.

  \param text the text to use as the legend title
  \sa legendTitleText(), setLegendTitleFont(),
  setLegendTitleFontUseRelSize()
  */




/**
  \fn QString KDChartParams::legendTitleText() const

  Returns the text that is shown as the title of the legend.

  \return the text that is used as the legend title
  \sa setLegendTitleText(), legendTitleFont(),
  legendTitleFontUseRelSize()
  */


/**
  \fn void KDChartParams::setLegendTitleTextColor( const QColor& color )

  Specifies the color to be used for the legend title. The
  default is black. Note that this color is only the default when
  rich text is used; tags in the rich text might change the color.

  \param color the color to be used for the legend title
  \sa legendTitleTextColor(), setLegendTextColor()
  */


/**
  \fn QColor KDChartParams::legendTitleTextColor() const

  Returns the color in which the legend title is drawn. The
  default is black.

  \return the color in which the legend title is drawn
  \sa setLegendTitleTextColor(), legendTextColor()
  */


/**
  \fn void KDChartParams::setLegendTitleFont( const QFont& font, bool useFontSize )

  Specifies the font in which the legend title (if any) is drawn. The
  default is a 12pt helvetica font.

  \note The font size will be ignored if \c useFontSize is false,
  in this case the font size will be calculated dynamically using
  the value stored by you calling setLegendFontRelSize().

  \param font the font to draw the legend texts in
  \sa setLegendTitleFontUseRelSize, legendTitleFont
  */


/**
  \fn QFont KDChartParams::legendTitleFont() const

  Returns the font in which the legend title (if any) is drawn.

  \return the font in which the legend texts are drawn
  \sa setLegendTitleFont, setLegendTitleFontUseRelSize
  */


/**
  \fn void KDChartParams::setLegendTitleFontUseRelSize( bool legendTitleFontUseRelSize )


  Specifies whether the legend title (if any) shall be drawn using
  relative font size.

  \param legendTitleFontUseRelSize whether the legend title shall
  be drawn using relative font size.
  If true the absolute value of the value set by \c
  setLegendTitleFontRelSize is per thousandth of the average
  value of the printable area height and width.
  This will make the legend title look nice even if scaled to a very
  different size.

  \sa setLegendTitleFontRelSize, setAxisLabelsFont
  */


/**
  \fn bool KDChartParams::legendTitleFontUseRelSize() const

  Returns whether the legend title font size is used.

  \return whether the fixed legend title font size is used.
  \sa setLegendTitleFontRelSize, setLegendTitleFont
  */


/**
  \fn void KDChartParams::setLegendTitleFontRelSize( int legendTitleFontRelSize )


  Specifies the legend title relative font size.

  \param legendTitleFontRelSize the relative legend size.
  If this value unequals zero the absolute value is per thousand
  of of the average value of the printable area height and width.
  This will make the legend title look nice even if scaled to a very
  different size.

  \sa setLegendTitleFontUseRelSize, setLegendTitleFont
  */


/**
  \fn int KDChartParams::legendTitleFontRelSize() const

  Returns the relative legend title font size.

  \return the relative legend title font size.
  \sa setLegendTitleFontRelSize, setLegendTitleFontUseRelSize
  */


/**
  \fn void KDChartParams::setLegendSpacing( uint space )

  Specifies how much space in pixels there should be between the legend
  and the data display. The default is 20 pixels.

  \param space the space in pixels between the legend and the data
  display
  \sa legendSpacing
  */


/**
  \fn uint KDChartParams::legendSpacing() const

  Returns how much space in pixels there is between the legend and the
  data display.

  \return the space in pixels between the legend and the data display
  \sa setLegendSpacing
  */

//@}
// END LEGENDS

// START AXES
/** @name Axis methods.

  These methods query and set axis parameters.
  */
//@{

/**
  \fn void KDChartParams::setAxisType( uint n, const KDChartAxisParams::AxisType axisType )

  Specifies the axis type.

  \param n the number of the axis settings to be modified
  \param axisType the type of the axis.
  \sa axisVisible
  */


/**
  \fn void KDChartParams::setAxisVisible( uint n, const bool axisVisible )

  Specifies if the axis will be drawn. The default is false.

  \param n the number of the axis settings to be modified
  \param axisVisible set true to make visible the respective axis.
  \sa axisVisible
  */


/**
  \fn bool KDChartParams::axisVisible( uint n ) const

  Returns if the axis will be drawn.

  \return if the axis is visible or not.
  \sa setAxisVisible
  */


/**
  Specifies whether a grid should be drawn at the chart data area.

  The grid will show a solid line for each delimiter.
  (or a line with a pattern defined by \c KDChartAxisParams::setAxisGridStyle(), resp.)

  \note If true and axisShowSubDelimiters is also true the grid
  will show a thin dotted line for each sub-delimiter.
  (or a line with a pattern defined by \c KDChartAxisParams::setAxisGridSubStyle(), resp.)

  \param n the number of the axis for which to specify whether a grid should be drawn.
  \param axisShowGrid if true a grid will be drawn on the chart data area.
  \sa axisShowGrid, setAxisGridStyle, setAxisGridSubStyle
  */
void KDChartParams::setAxisShowGrid( uint n, bool axisShowGrid )
{
    if ( n < KDCHART_MAX_AXES ) {
        _axisSettings[ n ].params.setAxisShowGrid( axisShowGrid );
        emit changed();
    }
}

/**
  Returns if any grid will be drawn at the data area.

  \return if any grid will be drawn at the data area.
  \sa KDChartAxisParams::setAxisShowGrid
  */
bool KDChartParams::showGrid() const
{
    for ( uint i = 0; i < KDCHART_MAX_AXES; ++i ) {
        if ( _axisSettings[ i ].params.axisVisible()
                && _axisSettings[ i ].params.axisShowGrid() )
            return true;
    }
    return false;
}

/**
  Specifies which data are to be represented by a given axis.<br>
  If you specify a valid dataset-number for parameter dataset
  you may use the same number or a greater number for
  parameter dataset2 to specify a range of contiguous series
  of datasets.<br>
  If you specify <b>KDCHART_ALL_DATASETS</b> for dataset
  the value of dataset2 does not matter.<br>
  By setting the \c chart parameter you may specify which chart is
  represented by the axis, you may use this when displaying
  more than one chart in the same widget.

  Calling setAxisDataset() with a dataset number will automatically set
  the visability flag of the respective axis (or axes, resp.) while calling
  it with KDCHART_NO_DATASET will clear the visibility flag so there is no
  need to explicitely call \c setAxisVisible() after calling setAxisDatasets().

  \note If you specify \c KDCHART_ALL_DATASETS the axis will
  represent all the datasets with SourceMode DataEntry
  (see \c setChartSourceMode for explanation) that have been
  specified for this chart.<br>

  \param n the number of the axis for which to specify the dataset(s)
  \param dataset the dataset represented by axis \c n

  \sa axisDatasets, chartAxes, setChartSourceMode, setAdditionalChartType
  */
void KDChartParams::setAxisDatasets( uint n, uint dataset,
        uint dataset2,
        uint chart )
{
    uint a1 = ( KDCHART_ALL_AXES == n )
        ? 0
        : QMIN( n, KDCHART_MAX_AXES-1 );
    uint a2 = ( KDCHART_ALL_AXES == n )
        ? KDCHART_MAX_AXES-1
        : QMIN( n, KDCHART_MAX_AXES-1 );
    for( uint i = a1;  i <= a2;  ++i ) {
        _axisSettings[ i ].params.setAxisVisible( KDCHART_NO_DATASET != dataset );
        _axisSettings[ i ].dataset = dataset;
        _axisSettings[ i ].dataset2 =
            ( KDCHART_ALL_DATASETS == dataset
              || KDCHART_NO_DATASET == dataset
              || KDCHART_ALL_DATASETS == dataset2
              || KDCHART_NO_DATASET == dataset2 )
            ? dataset
            : dataset2;
        _axisSettings[ i ].chart = chart;
    }
    emit changed();
}

/**
  Returns which data are to be represented by a given axis.
  ( see explanation given with \c setAxisDatasets() )

  \param n the axis for which to return the dataset number(s)
  \param dataset the starting number of the series of datasets represented by axis \c n or KDCHART_ALL_DATASETS
  \param dataset2 the end number of the series of datasets
  \param chart the chart represented by axis \c n
  \return true if a valid axis number was specified by parameter n
  \sa setAxisDataset, chartAxes
  */
bool KDChartParams::axisDatasets( uint n, uint& dataset,
        uint& dataset2, uint& chart ) const
{
    bool bOk = ( n < KDCHART_MAX_AXES );
    if ( bOk ) {
        dataset  = _axisSettings[ n ].dataset;
        dataset2 = _axisSettings[ n ].dataset2;
        chart    = _axisSettings[ n ].chart;
    }
    return bOk;
}


/**
  Returns which ORDINATE axis(axes) is(are) representing a given chart.
  This will look for the following axes: AxisPosLeft, AxisPosRight,
  AxisPosLeft2, AxisPosRight2.

  It is allowed to specify the same variable as axis1, axis2, axis3,
  axis4 - when doing this you will get the last axis number that was
  found for this chart.  In case you are sure there is only one axis
  per chart you may act that way.

  \param chart the chart for which to return the axis number(s)
  \param cnt the count of axis numbers returned
  \param axes on return, a vector with the numbers of the axes
  \return true if at least one axis is representing the chart \c chart
  \sa setAxisDataset, axisDataset
  */
bool KDChartParams::chartAxes( uint chart, uint& cnt, AxesArray& axes ) const
{
    cnt = 0;
    axes.resize( KDCHART_CNT_ORDINATES );
    for ( int i2 = 0; i2 < KDCHART_CNT_ORDINATES; ++i2 ) {
        axes[ i2 ] = KDCHART_NO_AXIS;
    }
    for ( uint i = 0; i < KDCHART_MAX_AXES; ++i ) {
        if (    chart == _axisSettings[ i ].chart
                && (    KDChartAxisParams::AxisPosLeft   == i
                    || KDChartAxisParams::AxisPosRight  == i
                    || KDChartAxisParams::AxisPosLeft2  == i
                    || KDChartAxisParams::AxisPosRight2 == i ) ) {
            for( int j = 0;  j < KDCHART_CNT_ORDINATES;  ++j ) {
                if( KDCHART_NO_AXIS == axes[ j ] || axes[ j ] == i ) {
                    if( KDCHART_NO_AXIS == axes[ j ] ) {
                        ++cnt;
                        axes[ j ] = i;
                    }
                    break;
                }
            }
        }
    }
    return (0 < cnt);
}




/**
  \fn QRect KDChartParams::axisArea( const uint n ) const

  Returns the true axis area rectangle as it was was calculate
  by KD Chart.

  \param n the number of the axis

  \note This special function may be called *after* calling
  KDChart::setupGeometry().  Normally you don't need to call
  it at all, its only purpose is to provide you with a way to
  retrieve the true position and size of an axis area.

  \sa dataArea
  */

/**
  \fn QRect KDChartParams::legendArea() const

  Returns the true legend area rectangle as it was was calculate
  by KD Chart.

  \note This special function may be called *after* calling
  KDChart::setupGeometry().  Normally you don't need to call
  it at all, its only purpose is to provide you with a way to
  retrieve the true position and size of an legend area.

  \sa dataArea
  */

/**
  \fn QRect KDChartParams::dataArea() const

  Returns the true data area rectangle as it was was calculate
  by KD Chart.

  \note This special function may be called *after* calling
  KDChart::setupGeometry().  Normally you don't need to call
  it at all, its only purpose is to provide you with a way to
  retrieve the true position and size of the data area.

  \sa axisArea
  */


/**
  \fn void KDChartParams::setAxisArea( const uint n, const QRect& areaRect )

  Specifies the true axis area rectangle.

  \note Do <b>not call</b> this function unless you are knowing
  exactly what you are doing. \c setAxisTrueAreaRect is normally
  reserved for internal usage by methods calculating the area
  size based upon \c axisAreaMin and \c axisAreaMax. Thus the
  signal \c changed() is not sended here.

  \sa axisAreaMax, axisAreaMin, setAxisAreaMode, setAxisAreaMin
  \sa setAxisArea
  */


/**
  \fn void KDChartParams::setAxisLabelsTouchEdges( uint n, bool axisLabelsTouchEdges )

  Specifies whether the axis labels start and end at the
  edges of the charts instead being positioned in the
  middle of the first data point (or the last one, resp.)

  \note If you decide to call setAxisLabelsTouchEdges() to
  override the default settings of abscissa axes
  (AxisPosBottom, AxisPosTop, AxisPosBottom2, AxisPosTop2)
  make sure to call it again each time you have called
  setChartType() since your settings will be overwritten
  when changing the chart type.

  \param axisLabelsTouchEdges if the axis labels start and end at
  the edges of the charts instead being positioned in the
  middle of the first data point (or the last one, resp.)

  \sa axisLabelsTouchEdges
  */


/**
  \fn void KDChartParams::setAxisLabelsVisible( uint n, bool axisLabelsVisible )

  Specifies whether the axis labels of an axis are to be shown.

  \param axisLabelsVisible if true the labels of this axis will be drawn.

  \sa setAxisLabelsFont
  */


/**
  Specifies the axis labels font for one axis.

  \note The font size will be ignored if \c axisLabelsFontSize is not zero,
  in this case the font size will be calculated dynamically using
  the value axisLabelsFontSize.

  \param axisLabelsFont the font to be used for the axis' labels.
  \param axisLabelsFontSize the (fixed or relative) axis font size.
  If this value is less than zero the absolute value is per thousand
  of the average value of the printable area height and width
  to be used. This will make the axis look the same even if scaled
  to very different size.
  \param axisLabelsColor the axis labels colour.

  \sa setAxisLabelsVisible
  \sa setAxisLabelsFontUseRelSize, setAxisLabelsFontRelSize
  \sa KDChartAxisParams::setAxisLabelsFontMinSize
  */
void KDChartParams::setAxisLabelsFont( uint n,
                                       QFont axisLabelsFont,
                                       int axisLabelsFontSize,
                                       QColor axisLabelsColor )
{
    if ( n < KDCHART_MAX_AXES ) {
        bool extraSize = (0 != axisLabelsFontSize);
        QFont theFont( axisLabelsFont );
        bool useFontFixedSize = true;
        if ( extraSize ){
            if( 0 > axisLabelsFontSize ){
                useFontFixedSize = false;
                _axisSettings[ n ].params.setAxisLabelsFontRelSize( -axisLabelsFontSize );
            }else{
                theFont.setPointSize( axisLabelsFontSize );
            }
        }
        _axisSettings[ n ].params.setAxisLabelsFont(  theFont, useFontFixedSize );
        _axisSettings[ n ].params.setAxisLabelsColor( axisLabelsColor );
    }
}


/**
  \fn void KDChartParams::setAxisLabelTextParams( uint n, bool axisSteadyValueCalc,
  KDChartData axisValueStart, KDChartData axisValueEnd,
  double axisValueDelta, int axisDigitsBehindComma,
  int axisMaxEmptyInnerSpan,
  KDChartAxisParams::LabelsFromDataRow takeLabelsFromDataRow,
  int labelTextsDataRow, QStringList* axisLabelStringList,
  QStringList* axisShortLabelsStringList )

  Specifies the label text parameters for one axis.

  \sa KDChartAxisParams::setAxisLabelTextParams
  */


/**
  Specifies a \c QStringList which the axis label texts for one
  axis are to be taken from, the optional short-labels parameter
  (if not zero) may be used to specify an alternative list of
  strings that are to be displayed in case the axis area size
  is too small for showing the full-length names.

  \note Normally axis labeling starts with the list's first string
  and end with its last string, but by specifying a start and an
  end value as additional parameters you can make KDChart repeat
  the strings between these two values only, as shown here:

  \verbatim
  QStringList abscissaNames;
  abscissaNames << "Sunday" << "Monday" << "Tuesday" << "Wednesday"
  << "Thursday" << "Friday" << "Saturday";
  QStringList abscissaShortNames;
  abscissaShortNames << "Sun" << "Mon" << "Tue" << "Wed"
  << "Thu" << "Fri" << "Sat";

  setAxisLabelStringParams( KDChartAxisParams::AxisPosBottom,
  &names, &shortNames,
  "Monday", "Friday")
  \endverbatim


  \param n the ID of the axis
  \param QStringList* axisLabelStringList points to the list of labels to be displayed
  \param  QStringList* axisShortLabelStringList points to
  an alternative list of short names to be displayed if the long labels take too much place
  \param QString valueStart ( default null ) the label to begin with
  \param QString valueEnd ( default null ) the label to end with

  \sa KDChartAxisParams::setAxisLabelStringLists
  */
void KDChartParams::setAxisLabelStringParams( uint n,
        QStringList*   axisLabelStringList,
        QStringList*   axisShortLabelStringList,
        const QString& valueStart,
        const QString& valueEnd )
{
    if ( n < KDCHART_MAX_AXES ) {
        _axisSettings[ n ].params.setAxisLabelStringLists( axisLabelStringList,
                axisShortLabelStringList,
                valueStart,
                valueEnd );
        emit changed();
    }
}


/**
  Specifies the parameters for one axis.

  \param axisParams the axis parameters to be used.
  \sa axisParams, AxisParams
  */
void KDChartParams::setAxisParams( uint n,
        const KDChartAxisParams& axisParams )
{
    if ( n < KDCHART_MAX_AXES ) {
        _axisSettings[ n ].params = axisParams;
        emit changed();
    }
}


/**
  \fn const KDChartAxisParams& KDChartParams::axisParams( uint n ) const

  Returns the parameters that are currently defined for axis n.

  \return all the parameters defined for axis n. If called with invalid
  value for \c n it returns the default axis parameters.
  \sa setAxisParams, AxisParams
  */


//@}
// END AXES

// START HEADERFOOTER

/** @name Header and footer methods.

  These methods query and set header and footer parameters.

  The names of the Header and Footer sections.

  You may use up to 9 header and/or up to 9 footer sections.

  \li Up to three <b>leading headers</b> ( \c HdFtPosHeader0 , \c
  HdFtPosHeader0L , \c HdFtPosHeader0R ) may be drawn at the very upper
  edge of the printable area. <br>
  You might want to use those sections to show some marginal information
  like department name, print date, page number... <br>
  <b>Note:</b> Those headers <b>share the same area</b> so make sure to
  specify propper horizontal alignment for each section when using more than
  one of them. By default \c HdFtPosHeader0 has centered alignement,
  \c HdFtPosHeader0L is aligned to the left and \c HdFtPosHeader0R to the
  right side. All of them are vertically aligned to the bottom, you may
  change this at your own risk: the resulting output might look strange. <br>
  <b>also note:</b> Alignment to bottom here means their baselines are
  horizontally adjusted to each other when more than one of them is used - it
  does <b>not</b> mean the <b>lowest pixel positions</b> of their output will
  be at the very same y coordinate since those might vary depending from the
  font size and or the font size you have specified for each of the sections.

  \li Up to three <b>main headers</b> ( \c HdFtPosHeader , \c
  HdFtPosHeaderL , \c HdFtPosHeaderR ) may be drawn under the
  leading header(s). <br>
  You could use this headers to show the main information such as project name,
  chart title or period of time (e.g. census year). <br>
  Like their counterparts they share the same part of the printable area so the
  restrictions regarding alignment mentioned above apply also to these three
  sections.

  \li Up to three <b>additional headers</b> ( \c HdFtPosHeader2 , \c
  HdFtPosHeader2L , \c HdFtPosHeader2R ) may be drawn under the main
  header(s). <br>
  This headers could show additional information such as project phase, chart
  sub-title or sub-period of time (e.g. census quarter-year). <br>
  Like their counterparts they share the same part of the printable area so the
  restrictions regarding alignment mentioned above apply also to these three
  sections.

  <p>

  \li Up to three <b>main footers</b> ( \c HdFtPosFooter , \c
  HdFtPosFooterL , \c HdFtPosFooterR ) may be drawn under the chart data
  area (or the legend that might be shown below that area, resp.). <br>
  You might want to use these footers instead of (or additional to) the
  main header(s) for showing the main information... <br>
  Like their header-counterparts they share the same part of the printable area
  so the restrictions regarding alignment mentioned above apply also to these
  three sections.

  \li Up to three <b>additional footers</b> ( \c HdFtPosFooter2 , \c
  HdFtPosFooter2L , \c HdFtPosHeader2R ) may be drawn under the main
  footer(s). <br>
  This footers could show additional information instead of (or additional to)
  the additional header(s). <br>
  Like their counterparts they share the same part of the printable area so the
  restrictions regarding alignment mentioned above apply also to these three
  sections.

  \li Up to three <b>trailing footers</b> ( \c HdFtPosFooter0 , \c
  HdFtPosFooter0L , \c HdFtPosFooter0R ) may be drawn at the very lower
  edge of the printable area: <b>under</b> the other footer sections <br>
  You might want to use those sections to show some marginal information
  instead of (or additional to) the leading header(s). <br>
  Like their counterparts they share the same part of the printable area so the
  restrictions regarding alignment mentioned above apply also to these three
  sections.

\note The names \c HdFtPosHeader or \c HdFtPosFooter are the basic names also returned by \c basicAxisPos.

<p>
<b>Usage:</b> Please see example code at \c setHeaderFooterText function explanation.

\sa setHeaderFooterText,           headerFooterText
\sa setHeaderFooterFont,           headerFooterFont
\sa setHeaderFooterFontUseRelSize, headerFooterFontUseRelSize
\sa setHeaderFooterFontRelSize,    headerFooterFontRelSize
*/
//@{


/**
  \enum KDChartParams::HdFtPos

  This enum lists the positions that headers and footers can take.
  */


/**
  \enum KDChartParams::HdFtPosSTART,

  start of all sections
  */

/**
  \enum KDChartParams::HdFtPosHeadersSTART

  start of header sections
  */

/**
  \enum KDChartParams::HdFtPosHeader0

  leading header (at the very top of the printable area)
  */

/**
  \enum KDChartParams::HdFtPosHeader0L

  another leading header, by default left-aligned
  */

/**
  \enum KDChartParams::HdFtPosHeader0R

  yet another leading header, by default right-aligned
  */

/**
  \enum KDChartParams::HdFtPosHeader

  main header (below the leading header)
  */

/**
  \enum KDChartParams::HdFtPosHeaderL

  another main header, by default left-aligned
  */

/**
  \enum KDChartParams::HdFtPosHeaderR

  yet another main header, by default right-aligned
  */

/**
  \enum KDChartParams::HdFtPosHeader2

  additional header (below the main header)
  */

/**
  \enum KDChartParams::HdFtPosHeader2L

  another additional header, by default left-aligned
  */

/**
  \enum KDChartParams::HdFtPosHeader2R

  yet another additional header, by default right-aligned
  */

/**
  \enum KDChartParams::HdFtPosHeadersEND

  end of header sections
  */

/**
  \enum KDChartParams::HdFtPosFootersSTART

  start of footer sections
  */


/**
  \enum KDChartParams::HdFtPosFooter

  main footer (below the data area or the bottom legend (if any), resp.)
  */

/**
  \enum KDChartParams::HdFtPosFooterL

  another main footer, by default left-aligned
  */

/**
  \enum KDChartParams::HdFtPosFooterR

  yet another main footer, by default right-aligned
  */


/**
  \enum KDChartParams::HdFtPosFooter2

  additional footer (below the main footer)
  */

/**
  \enum KDChartParams::HdFtPosFooter2L

  another additional footer, by default left-aligned
  */

/**
  \enum KDChartParams::HdFtPosFooter2R

  yet another additional footer, by default right-aligned
  */


/**
  \enum KDChartParams::HdFtPosFooter0

  trailing footer (below the additional footer)
  */

/**
  \enum KDChartParams::HdFtPosFooter0L

  another trailing footer, by default left-aligned
  */

/**
  \enum KDChartParams::HdFtPosFooter0R

  yet another trailing footer, by default right-aligned
  */

/**
  \enum KDChartParams::HdFtPosFootersEND

  end of footer sections
  */

/**
  \enum KDChartParams::HdFtPosEND

  end of all sections
  */


/**
  Specifies the text to be displayed in the header (or footer, resp.)
  section specified by \c pos.
  The default is not to display any header text nor footer text.

  \note To learn about the different header (or footer, resp.)
  sections please see information given at \c HdFtPos enum explanation.

  \param pos the section in which the text is to be shown
  \param text the text to be shown
  \sa HdFtPos, headerFooterText()
  \sa setHeaderFooterFont,           headerFooterFont
  \sa setHeaderFooterFontUseRelSize, headerFooterFontUseRelSize
  \sa setHeaderFooterFontRelSize,    headerFooterFontRelSize
  \sa setHeader1Text(), header1Text()
  \sa setHeader2Text(), header2Text()
  \sa setFooterText(),  footerText()
  */
void KDChartParams::setHeaderFooterText( uint pos, const QString& text )
{
    if ( HdFtPosEND >= pos ) {
        _hdFtParams[ pos ]._text = text;
        emit changed();
    }
}


/**
  Returns the text that is displayed in the header (or footer, resp.)
  section specified by \c pos.

  \note To learn about the different header (or footer, resp.)
  sections please see information given at \c HdFtPos enum explanation.

  \param pos the section for which to return the text
  \return the displayed text
  \sa HdFtPos, setHeaderFooterText()
  \sa setHeaderFooterFont,           headerFooterFont
  \sa setHeaderFooterFontUseRelSize, headerFooterFontUseRelSize
  \sa setHeaderFooterFontRelSize,    headerFooterFontRelSize
  \sa setHeader1Text(), header1Text()
  \sa setHeader2Text(), header2Text()
  \sa setFooterText(),  footerText()
  */
QString KDChartParams::headerFooterText( uint pos ) const
{
    if ( HdFtPosEND >= pos )
        return _hdFtParams[ pos ]._text;
    else
        return QString::null;
}



/**
  \note This methode for internal use.
  Return the position and size of one header/footer area.
  <b>Will return valid data only if called after KDChartPainter::setupGeometry()!</b>
  */
const QRect& KDChartParams::headerFooterRect( uint pos ) const
{
    if ( HdFtPosEND >= pos )
        return _hdFtParams[ pos ].rect();
    else
        return _noRect;
}


/**
  Specifies the colour to be used for the header text (or footer text, resp.)
  of the header/footer section specified by \c pos.
  The default colour is <b>QColor( Qt::black )</b>.

  \note To learn about the different header (or footer, resp.)
  sections please see information given at \c HdFtPos enum explanation.

  \param pos the section for which the colour is to be changed
  \param color the text colour to be used
  \sa HdFtPos, headerFooterText()
  \sa setHeaderFooterFont,           headerFooterFont
  \sa setHeaderFooterFontUseRelSize, headerFooterFontUseRelSize
  \sa setHeaderFooterFontRelSize,    headerFooterFontRelSize
  \sa setHeader1Text(), header1Text()
  \sa setHeader2Text(), header2Text()
  \sa setFooterText(),  footerText()
  */
void KDChartParams::setHeaderFooterColor( uint pos, const QColor color )
{
    if ( HdFtPosEND >= pos ) {
        _hdFtParams[ pos ]._color = color;
        emit changed();
    }
}


/**
  Returns the colour used for the header text (or footer text, resp.)
  of the header/footer section specified by \c pos.

  \note To learn about the different header (or footer, resp.)
  sections please see information given at \c HdFtPos enum explanation.

  \param pos the section for which to return the text colour
  \return the text colour
  \sa HdFtPos, setHeaderFooterText()
  \sa setHeaderFooterFont,           headerFooterFont
  \sa setHeaderFooterFontUseRelSize, headerFooterFontUseRelSize
  \sa setHeaderFooterFontRelSize,    headerFooterFontRelSize
  \sa setHeader1Text(), header1Text()
  \sa setHeader2Text(), header2Text()
  \sa setFooterText(),  footerText()
  */
QColor KDChartParams::headerFooterColor( uint pos ) const
{
    if ( HdFtPosEND >= pos )
        return _hdFtParams[ pos ]._color;
    else
        return QColor( Qt::black );
}


/**
  Specifies the font to be used in the header (or footer,
  resp.) section specified by \c pos..
  The default is a 14pt Helvetica font.

  \note To learn about the different header (or footer, resp.)
  sections please see information given at \c HdFtPos enum explanation.

  \param pos the section in which the font is to be used
  \param font the font to use
  \sa HdFtPos, setHeaderFooterText(), headerFooterText()
  \sa headerFooterFont()
  \sa setHeaderFooterFontUseRelSize, headerFooterFontUseRelSize
  \sa setHeaderFooterFontRelSize,    headerFooterFontRelSize
  \sa setHeader1Font(), header1Font()
  \sa setHeader2Font(), header2Font()
  \sa setFooterFont(),  footerFont()
  \sa setHeader1Text(), header1Text()
  \sa setHeader2Text(), header2Text()
  \sa setFooterText(),  footerText()
  */
void KDChartParams::setHeaderFooterFont( uint pos, const QFont& font,
        bool fontUseRelSize,
        int fontRelSize )
{
    if ( HdFtPosEND >= pos ) {
        _hdFtParams[ pos ]._font = font;
        _hdFtParams[ pos ]._fontUseRelSize = fontUseRelSize;
        _hdFtParams[ pos ]._fontRelSize = fontRelSize;
        emit changed();
    }
}


/**
  Returns the font used in the header (or footer, resp.)
  section specified by \c pos.

  \note To learn about the different header (or footer, resp.)
  sections please see information given at \c HdFtPos enum explanation.

  \param pos the section for which to return the font
  \return the used font
  \sa HdFtPos, setHeaderFooterText(), headerFooterText()
  \sa setHeaderFooterFont()
  \sa setHeaderFooterFontUseRelSize, headerFooterFontUseRelSize
  \sa setHeaderFooterFontRelSize,    headerFooterFontRelSize
  \sa setHeader1Font(), header1Font()
  \sa setHeader2Font(), header2Font()
  \sa setFooterFont(),  footerFont()
  \sa setHeader1Text(), header1Text()
  \sa setHeader2Text(), header2Text()
  \sa setFooterText(),  footerText()
  */
QFont KDChartParams::headerFooterFont( uint pos ) const
{
    if ( HdFtPosEND >= pos )
        return _hdFtParams[ pos ]._font;
    else
        return QApplication::font();
}


/**
  Returns is the font size used in the header (or footer, resp.)
  section specified by \c pos will be calculated dynamically.

  \note To learn about the different header (or footer, resp.)
  sections please see information given at \c HdFtPos enum explanation.

  \param pos the section for which to return the font calculation flag
  \return the font calculation flag
  \sa HdFtPos, setHeaderFooterText(), headerFooterText()
  \sa setHeaderFooterFont()
  \sa setHeaderFooterFontUseRelSize, headerFooterFontUseRelSize
  \sa setHeaderFooterFontRelSize,    headerFooterFontRelSize
  \sa setHeader1Font(), header1Font()
  \sa setHeader2Font(), header2Font()
  \sa setFooterFont(),  footerFont()
  \sa setHeader1Text(), header1Text()
  \sa setHeader2Text(), header2Text()
  \sa setFooterText(),  footerText()
  */
bool KDChartParams::headerFooterFontUseRelSize( uint pos ) const
{
    if ( HdFtPosEND >= pos )
        return _hdFtParams[ pos ]._fontUseRelSize;
    else
        return false;
}


/**
  Returns the factor used for dynamical calculation of
  the font size of the text in the header (or footer, resp.)
  section specified by \c pos.

  \note To learn about the different header (or footer, resp.)
  sections please see information given at \c HdFtPos enum explanation.

  \param pos the section for which to return the font size factor
  \return the font size factor
  \sa HdFtPos, setHeaderFooterText(), headerFooterText()
  \sa setHeaderFooterFont()
  \sa setHeaderFooterFontUseRelSize, headerFooterFontUseRelSize
  \sa setHeaderFooterFontRelSize,    headerFooterFontRelSize
  \sa setHeader1Font(), header1Font()
  \sa setHeader2Font(), header2Font()
  \sa setFooterFont(),  footerFont()
  \sa setHeader1Text(), header1Text()
  \sa setHeader2Text(), header2Text()
  \sa setFooterText(),  footerText()
  */
int KDChartParams::headerFooterFontRelSize( uint pos ) const
{
    if ( HdFtPosEND >= pos )
        return _hdFtParams[ pos ]._fontRelSize;
    else
        return 10;
}


/**
  \fn void KDChartParams::setHeader1Text( const QString& text )

  Specifies the text to be displayed as the first header line. The
  default is not to display any header text.

  \note This function provided for convenience only. <br>
  To gain the <b>full</b> benefits of the header/footer management
  please consider learning about the different header (or footer,
  resp.) sections, see information given at \c HdFtPos enum
  explanation. <br>
  The 'better' way to specify content and settings of headers or
  footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
  , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
  resp.) while specifying the appropriate \c HdFtPos. <br>
  The canonical way to retrieve settings and content of headers or
  footers is to call \c headerFooterText (or \c headerFooterFont ,
  \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

  \param text the text to display
  \sa HdFtPos, setHeaderFooterText(), headerFooterText()
  \sa header2Text(), setHeader1Text(), header1Text(), setFooterText(),
  footerText()
  */


/**
  \fn QString KDChartParams::header1Text() const

  Returns the text that is displayed as the first header line.

  \note This function provided for convenience only. <br>
  To gain the <b>full</b> benefits of the header/footer management
  please consider learning about the different header (or footer,
  resp.) sections, see information given at \c HdFtPos enum
  explanation. <br>
  The 'better' way to specify content and settings of headers or
  footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
  , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
  resp.) while specifying the appropriate \c HdFtPos. <br>
  The canonical way to retrieve settings and content of headers or
  footers is to call \c headerFooterText (or \c headerFooterFont ,
  \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

  \return the displayed text
  \sa HdFtPos, setHeaderFooterText(), headerFooterText()
  \sa setHeader2Text(), setHeader1Text(), header1Text()
  \sa setFooterText(), footerText()
  */


/**
  \fn void KDChartParams::setHeader1Font( const QFont& font )

  Specifies the font to be used for displaying the first header line.
  The default is a 14pt Helvetica font.

  \note This function provided for convenience only. <br>
  To gain the <b>full</b> benefits of the header/footer management
  please consider learning about the different header (or footer,
  resp.) sections, see information given at \c HdFtPos enum
  explanation. <br>
  The 'better' way to specify content and settings of headers or
  footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
  , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
  resp.) while specifying the appropriate \c HdFtPos. <br>
  The canonical way to retrieve settings and content of headers or
  footers is to call \c headerFooterText (or \c headerFooterFont ,
  \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

  \param font the font to use
  \sa HdFtPos, setHeaderFooterFont(), headerFooterFont()
  \sa setHeaderFooterFontUseRelSize(), headerFooterFontUseRelSize()
  \sa setHeaderFooterFontRelSize(), headerFooterFontRelSize()
  \sa header2Font(), setHeader2Text(), header2Text()
  \sa setHeader1Font(), header1Font()
  \sa setFooterFont(), footerFont()
  */


/**
  \fn QFont KDChartParams::header1Font() const

  Returns the font used for displaying the second header line.

  \note This function provided for convenience only. <br>
  To gain the <b>full</b> benefits of the header/footer management
  please consider learning about the different header (or footer,
  resp.) sections, see information given at \c HdFtPos enum
  explanation. <br>
  The 'better' way to specify content and settings of headers or
  footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
  , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
  resp.) while specifying the appropriate \c HdFtPos. <br>
  The canonical way to retrieve settings and content of headers or
  footers is to call \c headerFooterText (or \c headerFooterFont ,
  \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

  \return the used font
  \sa HdFtPos, setHeaderFooterFont(), headerFooterFont()
  \sa setHeaderFooterFontUseRelSize(), headerFooterFontUseRelSize()
  \sa setHeaderFooterFontRelSize(), headerFooterFontRelSize()
  \sa setHeader2Font(), setHeader2Text(), header2Text(),
  setHeader1Font(), header1Font(), setFooterFont(), footerFont()
  */


/**
  \fn void KDChartParams::setHeader2Text( const QString& text )

  Specifies the text to be displayed as the second header line. The
  default is not to display any header text.

  \note This function provided for convenience only. <br>
  To gain the <b>full</b> benefits of the header/footer management
  please consider learning about the different header (or footer,
  resp.) sections, see information given at \c HdFtPos enum
  explanation. <br>
  The 'better' way to specify content and settings of headers or
  footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
  , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
  resp.) while specifying the appropriate \c HdFtPos. <br>
  The canonical way to retrieve settings and content of headers or
  footers is to call \c headerFooterText (or \c headerFooterFont ,
  \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

  \param text the text to display
  \sa HdFtPos, setHeaderFooterText(), headerFooterText()
  \sa header2Text(), setHeader1Text(), header1Text(), setFooterText(),
  footerText()
  */


/**
  \fn QString KDChartParams::header2Text() const

  Returns the text that is displayed as the second header line.

  \note This function provided for convenience only. <br>
  To gain the <b>full</b> benefits of the header/footer management
  please consider learning about the different header (or footer,
  resp.) sections, see information given at \c HdFtPos enum
  explanation. <br>
  The 'better' way to specify content and settings of headers or
  footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
  , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
  resp.) while specifying the appropriate \c HdFtPos. <br>
  The canonical way to retrieve settings and content of headers or
  footers is to call \c headerFooterText (or \c headerFooterFont ,
  \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

  \return the displayed text
  \sa HdFtPos, setHeaderFooterText(), headerFooterText()
  \sa setHeader2Text(), setHeader1Text(), header1Text(), setFooterText(),
  footerText()
  */


/**
  \fn void KDChartParams::setHeader2Font( const QFont& font )

  Specifies the font to be used for displaying the second header line. The
  default is a 12pt Helvetica font.

  \note This function provided for convenience only. <br>
  To gain the <b>full</b> benefits of the header/footer management
  please consider learning about the different header (or footer,
  resp.) sections, see information given at \c HdFtPos enum
  explanation. <br>
  The 'better' way to specify content and settings of headers or
  footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
  , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
  resp.) while specifying the appropriate \c HdFtPos. <br>
  The canonical way to retrieve settings and content of headers or
  footers is to call \c headerFooterText (or \c headerFooterFont ,
  \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

  \param font the font to use
  \sa HdFtPos, setHeaderFooterFont(), headerFooterFont()
  \sa setHeaderFooterFontUseRelSize(), headerFooterFontUseRelSize()
  \sa setHeaderFooterFontRelSize(), headerFooterFontRelSize()
  \sa header2Font(), setHeader2Text(), header2Text(), setHeader1Font(), header1Font(),
  setFooterFont(), footerFont()
  */



/**
  \fn QFont KDChartParams::header2Font() const

  Returns the font used for displaying the second header line.

  \note This function provided for convenience only. <br>
  To gain the <b>full</b> benefits of the header/footer management
  please consider learning about the different header (or footer,
  resp.) sections, see information given at \c HdFtPos enum
  explanation. <br>
  The 'better' way to specify content and settings of headers or
  footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
  , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
  resp.) while specifying the appropriate \c HdFtPos. <br>
  The canonical way to retrieve settings and content of headers or
  footers is to call \c headerFooterText (or \c headerFooterFont ,
  \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

  \return the used font
  \sa HdFtPos, setHeaderFooterFont(), headerFooterFont()
  \sa setHeaderFooterFontUseRelSize(), headerFooterFontUseRelSize()
  \sa setHeaderFooterFontRelSize(), headerFooterFontRelSize()
  \sa setHeader2Font(), setHeader2Text(), header2Text(),
  setHeader1Font(), header1Font(), setFooterFont(), footerFont()
  */


/**
  \fn void KDChartParams::setFooterText( const QString& text )

  Specifies the text to be displayed as the footer line. The
  default is not to display any footer text.

  \note This function provided for convenience only. <br>
  To gain the <b>full</b> benefits of the header/footer management
  please consider learning about the different header (or footer,
  resp.) sections, see information given at \c HdFtPos enum
  explanation. <br>
  The 'better' way to specify content and settings of headers or
  footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
  , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
  resp.) while specifying the appropriate \c HdFtPos. <br>
  The canonical way to retrieve settings and content of headers or
  footers is to call \c headerFooterText (or \c headerFooterFont ,
  \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

  \param text the text to display
  \sa HdFtPos, setHeaderFooterText(), headerFooterText()
  \sa footerText(), setHeader1Text(), header1Text(), setHeader2Text(),
  header2Text()
  */


/**
  \fn QString KDChartParams::footerText() const

  Returns the text that is displayed as the footer line.

  \note This function provided for convenience only. <br>
  To gain the <b>full</b> benefits of the header/footer management
  please consider learning about the different header (or footer,
  resp.) sections, see information given at \c HdFtPos enum
  explanation. <br>
  The 'better' way to specify content and settings of headers or
  footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
  , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
  resp.) while specifying the appropriate \c HdFtPos. <br>
  The canonical way to retrieve settings and content of headers or
  footers is to call \c headerFooterText (or \c headerFooterFont ,
  \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

  \return the displayed text
  \sa HdFtPos, setHeaderFooterText(), headerFooterText()
  \sa setFooterText(), setHeader1Text(), header1Text(), setHeader2Text(),
  header2Text()
  */


/**
  \fn void KDChartParams::setFooterFont( const QFont& font )

  Specifies the font to be used for displaying the footer line. The
  default is a 12pt Helvetica font.

  \note This function provided for convenience only. <br>
  To gain the <b>full</b> benefits of the header/footer management
  please consider learning about the different header (or footer,
  resp.) sections, see information given at \c HdFtPos enum
  explanation. <br>
  The 'better' way to specify content and settings of headers or
  footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
  , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
  resp.) while specifying the appropriate \c HdFtPos. <br>
  The canonical way to retrieve settings and content of headers or
  footers is to call \c headerFooterText (or \c headerFooterFont ,
  \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

  \param font the font to use
  \sa HdFtPos, setHeaderFooterFont(), headerFooterFont()
  \sa setHeaderFooterFontUseRelSize(), headerFooterFontUseRelSize()
  \sa setHeaderFooterFontRelSize(), headerFooterFontRelSize()
  \sa footerFont(), setFooterText(), footerText(), setHeader1Font(), header1Font(),
  setHeader2Font(), header2Font()
  */


/**
  \fn QFont KDChartParams::footerFont() const

  Returns the font used for displaying the footer line.

  \note This function provided for convenience only. <br>
  To gain the <b>full</b> benefits of the header/footer management
  please consider learning about the different header (or footer,
  resp.) sections, see information given at \c HdFtPos enum
  explanation. <br>
  The 'better' way to specify content and settings of headers or
  footers is to call \c setHeaderFooterText ( or \c setHeaderFooterFont
  , \c setHeaderFooterFontUseRelSize , \c setHeaderFooterFontRelSize ,
  resp.) while specifying the appropriate \c HdFtPos. <br>
  The canonical way to retrieve settings and content of headers or
  footers is to call \c headerFooterText (or \c headerFooterFont ,
  \c headerFooterFontUseRelSize , \c headerFooterFontRelSize, resp.).

  \return the used font
  \sa HdFtPos, setHeaderFooterFont(), headerFooterFont()
  \sa setHeaderFooterFontUseRelSize(), headerFooterFontUseRelSize()
  \sa setHeaderFooterFontRelSize(), headerFooterFontRelSize()
  \sa setFooterFont(), setFooterText(), footerText(),
  setHeader1Font(), header1Font(), setHeader2Font(), header2Font()
  */


//@}
// END HEADERFOOTER



// PRIVATE VARIABLES
/**
  \var int KDChartParams::_globalLeadingLeft;
  Stores the distance between the chart and the left border of the painter area.
  */

/**
  \var int KDChartParams::_globalLeadingTop;

  Stores the distance between the chart and the upper border of the painter area.
  */


/**
  \var int KDChartParams::_globalLeadingRight;

  Stores the distance between the chart and the left border of the painter area.
  */


/**
  \var int KDChartParams::_globalLeadingBottom;

  Stores the distance between the chart and the right border of the painter area.
  */

/**
  \var ChartType KDChartParams::_chartType;

  Stores the chart type.
  */

/**
  \var ChartType KDChartParams::_additionalChartType;

  Stores the additional chart type.
  */

/**
  \var int KDChartParams::_numValues;

  Stores how many of the values should be shown. -1 means all
  available values.
  */

/**
  \var QFont KDChartParams::_defaultFont;

  Stores our default font used for many different purposes.
  */

/**
  \var KDChartParams::AreaMap

  The type used for storing the frame settings for all of the chart areas.
  */

/**
  \var AreaMap KDChartParams::_areaMap;
  Stores the frame settings for all of the chart areas.
  */

/**
  \var CustomBoxMap KDChartParams::_customBoxDict;

  Stores the settings for all of the custom boxes.
  */


/**
  \var QRect KDChartParams::_noRect;

  Stores an empty rectangle.
  */

/**
  \var KDFrame KDChartParams::_noFrame;

  Stores the default frame settings: no border, no corners, no background.
  */

/**
  \var KDChartFrameSettings KDChartParams::_noFrameSettings;

  Stores the default area settings: no frame, no inner gap, no outer gap.
  */


/**
  \class KDChartParams::ModeAndChart

  Stores the SourceMode and the chart(s) for one dataset.
  */


/**
  \var ModeAndChartMap KDChartParams::_dataSourceModeAndChart;

  Stores the SourceMode and the chart(s) for the datasets.
  */

/**
  \var typedef KDChartParams::ModeAndChartMap

  The type used for storing the source mode and charts of the datasets
  */


/**
  \var bool KDChartParams::_setChartSourceModeWasUsed;

  Stores whether setChartSourceMode() was called or not
  */

/**
  \var QMap < uint, QColor > KDChartParams::_dataColors;

  Stores the colors for the datasets.
  */

/**
  \var uint KDChartParams::_maxDatasetSourceMode;

  Stores the highest dataset number for which a SourceMode has been
  defined. Note that if you define a value for a dataset but not for a
  dataset with a lower number (and there is neither a default value), the
  source mode for that dataset with the lower number will be undefined.
  */


/**
  \var KDChartPropertySetList KDChartParams::_propertySetList;

  Stores all property-sets that were defined.
  Each of these sets may be assigned to an individual cell.
  */


/**
  \var double KDChartParams::_shadowBrightnessFactor;

  Stores a factor to be used to adjust the
  built-in brightness of shadow colors in
  3-dimensional drawings like e.g. 3D bar charts.
  */


/**
  \var Qt::BrushStyle KDChartParams::_shadowPattern;

  Stores a fill style to be used for filling shadow area in
  3-dimensional drawings like e.g. 3D bar charts.
  */

/**
  \var bool KDChartParams::_threeDShadowColors;

  Stores  whether shadowed colors are used for 3D effects. Only used
  for 3D effects in charts that support these.
  */


/**
  \var uint KDChartParams::_maxDatasetColor;

  Stores the highest dataset number for which a color has been
  defined. Note that if you define a value for a dataset but not for a
  dataset with a lower number (and there is neither a default value), the
  color for that dataset with the lower number will be undefined.
  */

/**
  \var QMap < uint, QColor > KDChartParams::_dataColorsShadow1;

  Stores the shadow colors that are somewhat darker than the colors in
  _dataColors.
  */

/**
  \var QMap < uint, QColor > KDChartParams::_dataColorsShadow2;

  Stores the shadow colors that are even more darker than the colors in
  _dataColorsShadow1;
  */

/**
  \var QColor KDChartParams::_outlineDataColor;

  Stores the color to be used for outlines of data displays.
  */

/**
  \var uint KDChartParams::_outlineDataLineWidth;

  Stores the line width to be used for outlines of data displays.
  */

/**
  \var PenStyle KDChartParams::_outlineDataLineStyle;

  Stores the line style to be used for outlines of data displays.
  */



/**
  \struct PrintValueDataSettings KDChartParams.h

  Stores if and how a chart will print the data values near their
  respective entries.
  */


/**
  \var PrintDataValuesSettings KDChartParams::_printDataValuesSettings;

  Stores if and how the FIRST chart will print the data values
  near their respective entries.
  */

/**
  \var PrintDataValuesSettings KDChartParams::_printDataValuesSettings2;

  Stores if and how the SECOND chart will print the data values
  near their respective entries, in case two charts are to be
  drawn sharing the same data area.
  */

/**
  \var bool KDChartParams::_allowOverlappingDataValueTexts;

  Stores whether a data value may be drawn near it's respective entry
  even if it's region intersects with another data value text's region.

  By default this is FALSE to prevent ugly mutual overwriting of data
  value texts and to speed up drawing of cahrts containing thousands
  of data points.
  */


/**
  \var BarChartSubType KDChartParams::_barChartSubType;

  Stores the bar chart subtype. Only used when chartType() ==
  Bar
  */

/**
  \var bool KDChartParams::_threeDBars;

  Stores whether the engine should draw the bars in 3D. Only
  used if chartType() == Bar.
  */

/**
  \var int KDChartParams::_threeDBarAngle;

  Stores the angle used for 3D display. Only used if
  threeDBars == true.
  */

/**
  \var double KDChartParams::_threeDBarDepth;

  Stores the depth of the 3D Effect used for 3D bars
  in relation to the bar width.
  Only used if chartType() == Bar and threeDBars() == true.
  */


/**
  \var double KDChartParams::_cosThreeDBarAngle;

  The cosine in rad of the angle for 3D display. Only used if
  threeDBars == true.
  */

/**
  \var int KDChartParams::_datasetGap;

  Stores the number of pixels between two dataset values.
  */

/**
  \var bool KDChartParams::_datasetGapIsRelative;

  Stores if the value set by \c setDatasetGap
  is a per mille value of the chart data area width.
  */

/**
  \var int KDChartParams::_valueBlockGap;

  Stores the number of pixels between each value block.
  */

/**
  \var bool KDChartParams::_valueBlockGapIsRelative;

  Stores if the value set by \c setValueBlockGap
  is a per mille value of the chart data area width.
  */

/// LINES/AREAS-specific
/**
  \var LineChartSubType KDChartParams::_lineChartSubType;

  Stores the line chart subtype. Only used when chartType() ==
  Line
  */

/**
  \var bool KDChartParams::_threeDLines;

  Stores whether 3D lines should be drawn.
  */


/**
  \var int KDChartParams::_threeDLineDepth;

  Stores the depth of 3D lines.
  */


// documentation of this member variable temporary disabled:
// Feature is currently not supported, will be implemented
// by future versions of KDChart
/*
   \var int KDChartParams::_threeDLineXRotation;

   Stores the X rotation of 3D lines.
   */


// documentation of this member variable temporary disabled:
// Feature is currently not supported, will be implemented
// by future versions of KDChart
/*
   \var int KDChartParams::_threeDLineYRotation;

   Stores the Y rotation of 3D lines.
   */


/**
  \var bool KDChartParams::_lineMarker;

  Stores whether line markers should be drawn. Only used when
  chartType() == Line.
  */


/**
  \var LineMarkerStyleMap KDChartParams::_lineMarkerStyles;

  Stores the styles of the line makers
  */

/**
  \var uint KDChartParams::_maxDatasetLineMarkerStyle;

  Stores the highest dataset number for which a line marker has been
  defined. Note that if you define a value for a dataset but not for a
  dataset with a lower number (and there is neither a default value), the
  line marker for that dataset with the lower number will be undefined.
  */

/**
  \var QSize KDChartParams::_lineMarkerSize;

  Stores the size of the line markers.
  */


/**
  \var QColor KDChartParams::_lineColor;

  Stores the color of lines in line charts.
  */

/**
  \var int KDChartParams::_lineWidth;

  Stores the line width of lines in line charts.
  */

/**
  \var PenStyle KDChartParams::_lineStyle;

  Stores the line style of lines in line charts.
  */

/**
  \var AreaChartSubType KDChartParams::_areaChartSubType;

  Stores  the area chart subtype. Only used when chartType() ==
  Area
  */


/**
  \var AreaLocation KDChartParams::_areaLocation;

  Stores  whether the area above or below the value points should
  be filled.
  */


/// POLAR-specific
/**
  \var PolarChartSubType KDChartParams::_polarChartSubType;

  Stores  the polar chart subtype. Only used when chartType() ==
  Polar
  */

/**
  \var bool KDChartParams::_polarMarker;

  Stores  whether polar markers should be drawn. Only used when
  chartType() == Polar.
  */

/**
  \var PolarMarkerStyleMap KDChartParams::_polarMarkerStyles;

  Stores the styles of the markers in a polar char.
  */

/**
  \var uint KDChartParams::_maxDatasetPolarMarkerStyle;

  Stores the highest dataset number for which a polar marker has been
  defined. Note that if you define a value for a dataset but not for a
  dataset with a lower number (and there is neither a default value), the
  polar marker for that dataset with the lower number will be undefined.
  */

/**
  \var QSize KDChartParams::_polarMarkerSize;

  Stores the size of the polar markers.
  */


/**
  \var int KDChartParams::_polarLineWidth;

  Stores the line width of lines in polar charts.
  */


/**
  \var int KDChartParams::_polarZeroDegreePos;

  Stores the zero degree position in polar charts.
  */


/**
  \var bool KDChartParams::_polarRotateCircularLabels;

  Stores whether to rotate circular labels in polar charts.
  */

/**
  \struct KDChartParams::_polarDelimsAndLabelStruct KDChartParams.h

  Stores whether to show circular delimiters and/or labels
  at a given position in polar charts.
  */

/**
  \var _polarDelimsAndLabelStruct KDChartParams::_polarDelimsAndLabels[ 1 + KDCHART_MAX_POLAR_DELIMS_AND_LABELS_POS ];

  Stores where to show circular delimiters and labels in polar charts.
  */



// PIES/RINGS
/**
  \var bool KDChartParams::_explode;

  Stores whether pies or ring segments are drawn exploding.
  */

/**
  \var double KDChartParams::_explodeFactor;

  Stores the default explode factor of pie or ring segments.
  */

/**
  \var QMap<int,double> KDChartParams::_explodeFactors;

  Stores the list of segment-specific explode factors of pie or
  ring segments.
  */

/**
  \var QValueList<int> KDChartParams::_explodeList;

  Stores the list of pies or ring segments to explode.
  */

/**
  \var bool KDChartParams::_threeDPies;

  Stores whether pies will have a 3D effect.
  */

/**
  \var int KDChartParams::_threeDPieHeight;

  Stores the height of the pie 3D effect.
  */

/**
  \var int KDChartParams::_pieStart;

  Stores the starting point of the pie circle.
  */

/**
  \var int KDChartParams::_ringStart;

  Stores the starting point of the ring circle.
  */

/**
  \var bool KDChartParams::_relativeRingThickness;

  Stores whether the ring thickness is relativ or constant.
  */


// HI-LO CHARTS

/**
  \var HiLoChartSubType KDChartParams::_hiLoChartSubType;

  Stores the HiLo subtype.
  */

/**
  \var bool KDChartParams::_hiLoChartPrintLowValues;

  Stores whether the low values
  will be printed under their respective entries.
  */


/**
  \var QFont KDChartParams::_hiLoChartLowValuesFont;

  Stores the font to be used for printing the low values.
  */


/**
  \var bool KDChartParams::_hiLoChartLowValuesUseFontRelSize;

  Stores whether the font size to be used for printing the low
  values is calculated dynamically.
  */


/**
  \var int KDChartParams::_hiLoChartLowValuesFontRelSize;

  Stores the relative size (in per mille of the chart width) of
  font size to be used for printing the low values.
  */


/**
  \var QColor KDChartParams::_hiLoChartLowValuesColor;

  Stores the colour of the font size to be used for printing the
  low values.
  */

/**
  \var bool KDChartParams::_hiLoChartPrintHighValues;

  Stores whether the high values will be printed under their
  respective entries.
  */


/**
  \var QFont KDChartParams::_hiLoChartHighValuesFont;

  Stores the font to be used for printing the high values.
  */


/**
  \var bool KDChartParams::_hiLoChartHighValuesUseFontRelSize;

  Stores whether the font size to be used for printing the high
  values is calculated dynamically.
  */


/**
  \var int KDChartParams::_hiLoChartHighValuesFontRelSize;

  Stores the relative size (in per mille of the chart width) of
  font size to be used for printing the high values.
  */


/**
  \var QColor KDChartParams::_hiLoChartHighValuesColor;

  Stores the colour of the font size to be used for printing the
  high values.
  */

/**
  \var bool KDChartParams::_hiLoChartPrintOpenValues;

  Stores whether the open values
  will be printed under their respective entries.
  */


/**
  \var QFont KDChartParams::_hiLoChartOpenValuesFont;

  Stores the font to be used for printing the open values.
  */


/**
  \var bool KDChartParams::_hiLoChartOpenValuesUseFontRelSize;

  Stores whether the font size to be used for printing the open
  values is calculated dynamically.
  */


/**
  \var int KDChartParams::_hiLoChartOpenValuesFontRelSize;

  Stores the relative size (in per mille of the chart width) of
  font size to be used for printing the open values.
  */


/**
  \var QColor KDChartParams::_hiLoChartOpenValuesColor;

  Stores the colour of the font size to be used for printing the
  open values.
  */

/**
  \var bool KDChartParams::_hiLoChartPrintCloseValues;

  Stores whether the close values will be printed under their
  respective entries.
  */


/**
  \var QFont KDChartParams::_hiLoChartCloseValuesFont;

  Stores the font to be used for printing the close values.
  */


/**
  \var bool KDChartParams::_hiLoChartCloseValuesUseFontRelSize;

  Stores whether the font size to be used for printing the close
  values is calculated dynamically.
  */


/**
  \var int KDChartParams::_hiLoChartCloseValuesFontRelSize;

  Stores the relative size (in per mille of the chart width) of
  font size to be used for printing the close values.
  */


/**
  \var QColor KDChartParams::_hiLoChartCloseValuesColor;

  Stores the colour of the font size to be used for printing the
  close values.
  */


/**
  \var BWChartSubType KDChartParams::_BWChartSubType;

  Stores the Box And Whisker subtype.
  */

/**
  \struct BWChartStatistics KDChartParams.h

  Stores the statistical data for a box-and-whisker chart
  */

/**
  \var BWChartStatistics KDChartParams::_BWChartStatistics[ BWStatValOffEndValue ];

  Stores whether and how the statistical data
  will be printed near the drawing.
  */

/**
  \var double KDChartParams::_BWChartFenceUpperInner;
  \var double KDChartParams::_BWChartFenceLowerInner;
  \var double KDChartParams::_BWChartFenceUpperOuter;
  \var double KDChartParams::_BWChartFenceLowerOuter;

  Stores the factors to be used to calculate the width of
  the inner and outer fences.
  */

/**
  \var int KDChartParams::_BWChartOutValMarkerSize;

  Stores the size (or the negative size factor, resp.) of
  the markers representing the outliers.
  */


/**
  \var QBrush KDChartParams::_BWChartBrush;

  Stores the brush to be used.
  */

/**
  \var LegendPosition KDChartParams::_legendPosition;

  Stores the legend position.
  */

/**
  \var LegendSource KDChartParams::_legendSource;

  Stores the legend source.
  */

/**
  \var QMap < int, QString > KDChartParams::_legendText;

  Stores the manually set legend texts.
  */


/**
  \var QColor KDChartParams::_legendTextColor;

  Stores the color for drawing the legend texts.
  */

/**
  \var QFont KDChartParams::_legendFont;

  Stores the font for drawing the legends.
  */

/**
  \var bool KDChartParams::_legendFontUseRelSize;

  Stores  whether the size of the legend font is to be calculated
  on a relative basis.

  \sa setLegendFontUseRelSize, setLegendFontRelSize
  \sa setLegendFont
  */

/**
  \var int KDChartParams::_legendFontRelSize;

  Stores  the per mille basis for calculating the relative
  legend font size.

  \sa setLegendFontRelSize, setLegendFontUseRelSize
  \sa setLegendFont
  */


/**
  \var QString KDChartParams::_legendTitleText;

  Stores the legend title text.
  */


/**
  \var QColor KDChartParams::_legendTitleTextColor;

  Stores the color for drawing the legend title.
  */


/**
  \var QFont KDChartParams::_legendTitleFont;

  Stores the font for drawing the legend title.
  */

/**
  \var bool KDChartParams::_legendTitleFontUseRelSize;

  Stores  whether the size of the legend title font is to be
  calculated on a relative basis.

  \sa setLegendTitleFontUseRelSize, setLegendTitleFontRelSize
  \sa setLegendTitleFont
  */

/**
  \var int KDChartParams::_legendTitleFontRelSize;

  Stores  the per mille basis for calculating the relative
  legend title font size.

  \sa setLegendTitleFontRelSize, setLegendTitleFontUseRelSize
  \sa setLegendTitleFont
  */


/**
  \var uint KDChartParams::_legendSpacing;

  Stores the number of pixels between the legend and the data display
  */


/**
  \struct KDChartParams::AxisSettings KDChartParams.h

  Stores  all the settings of all the axis \em plus one more
  parameter set containing the build-in defauls axis settings.
  */

/**
  \var KDChartParams::_axisSettings

  Stores the settings for each of the possible thirteen axes
  */



/**
  \struct HdFtParams KDChartParams.h

  Stores  all the settings of all the header sections
  and all the footer sections.
  */



/**
  Retrieves the ID of the first KDChartCustomBox that is anchored at axis \c n.

  By default this box is used for the default axis title text: it is created
  (or modified, resp.) by calling one of the axis title convenience functions
  setAxisTitleColor or setAxisTitleFont or setAxisTitleFontUseRelSize or setAxisTitleFontRelSize.

  \note If you have specified another custom box manually that is anchored to this axis
  before (or instead of, resp.) calling one of the convenience functions mentioned above
  your custom box will become the default axis title text box: you then can access/modify
  its properties using these convenience functions.

  \return TRUE if a KDChartCustomBox was found that is anchored to axis \c n.
  \param n the axis number, this could be KDChartAxisParams::AxisPosLeft or KDChartAxisParams::AxisPosBottom...
  \param boxID will obtain the respective KDChartCustomBox's ID if return value is TRUE, otherwise the parameter's value remains unchanged.
  \sa setAxisTitle, setAxisTitleColor, setAxisTitleFont, setAxisTitleFontUseRelSize, setAxisTitleFontRelSize
  */
bool KDChartParams::findFirstAxisCustomBoxID( uint n, uint& boxID ) const
{
    QIntDictIterator<KDChartCustomBox> it( _customBoxDict );
    for( ; it.current(); ++it ){
        if( (*it).anchorArea() == KDChartEnums::AreaAxisBASE + n ){
            boxID = it.currentKey();
            return true;
        }
    }
    return false;
}

void KDChartParams::insertDefaultAxisTitleBox( uint n,
                                               bool setTitle,          const QString& axisTitle,
                                               bool setColor,          const QColor&  axisTitleColor,
                                               bool setFont,           const QFont&   axisTitleFont,
                                               bool setFontUseRelSize, bool           axisTitleFontUseRelSize,
                                               bool setFontRelSize,    int            axisTitleFontRelSize )
{
    bool bVert = false;
    bool bHorz = false;
    bool b3rd  = false;
    switch( KDChartAxisParams::basicAxisPos( n ) ){
        case KDChartAxisParams::AxisPosLeft:
        case KDChartAxisParams::AxisPosRight:
        case KDChartAxisParams::AxisPosLeft2:
        case KDChartAxisParams::AxisPosRight2:
            bVert = true;
            break;
        case KDChartAxisParams::AxisPosTop:
        case KDChartAxisParams::AxisPosBottom:
        case KDChartAxisParams::AxisPosTop2:
        case KDChartAxisParams::AxisPosBottom2:
            bHorz = true;
            break;
        default:
            b3rd = true;
            break;
    }
    const QFont defaultFont( "helvetica", 6, QFont::Normal, false );

    // SGI IRIX: Compiling error.
    // QString titleString( setTitle ? axisTitle : "<qt><center> </center></qt>" );
    QString titleString;
    if( setTitle )
       titleString = axisTitle;
    else
       titleString = "<qt><center> </center></qt>";


    const QString stripTitleString( titleString.simplifyWhiteSpace().upper() );
    if( setTitle ){
        if( !stripTitleString.startsWith("<QT>" ) )
            titleString.prepend("<qt><center>");
        if( !stripTitleString.endsWith("</QT>" ) )
            titleString.append("</center></qt>");
    }

    KDChartTextPiece textPiece( titleString, setFont  ? axisTitleFont : defaultFont );
    int fixedFontSize = textPiece.font().pointSize();
    if( -1 == fixedFontSize )
        fixedFontSize = textPiece.font().pixelSize();
    if( -1 == fixedFontSize )
        fixedFontSize = 15;
    int relFontSize = setFontRelSize ? -axisTitleFontRelSize : -18;

    KDChartCustomBox customBox( bVert ? -90 : 0,
                                textPiece,
                                setFontUseRelSize
                                ? ( axisTitleFontUseRelSize ? relFontSize : fixedFontSize )
                                : relFontSize,
                                true,
                                0,     bVert ?  -40 : (bHorz ?   0 : 0),
                                -2000, bVert ? -200 : (bHorz ? -45 : 0),
                                setColor ? axisTitleColor : Qt::darkBlue,
                                Qt::NoBrush,
                                KDChartEnums::AreaAxisBASE + n,
                                bVert ? KDChartEnums::PosCenterLeft        : KDChartEnums::PosBottomCenter,
                                bVert ? (Qt::AlignTop + Qt::AlignHCenter)  : (Qt::AlignBottom + Qt::AlignHCenter),
                                0,0,0,
                                bVert ? (Qt::AlignBottom + Qt::AlignRight) : (Qt::AlignTop + Qt::AlignHCenter),
                                false );
    insertCustomBox( customBox );
}

/**
  Specifies the default axis title text.

  \note This is a convenience function accessing the first KDChartCustomBox that is anchored at axis \c n.

  \param axisTitle the axis title text
  \sa setAxisTitleColor,setAxisTitleFont,setAxisTitleFontUseRelSize, setAxisTitleFontRelSize
  \sa axisTitle, axisTitleColor, axisTitleFont, axisTitleFontUseRelSize, axisTitleFontRelSize
  */
void KDChartParams::setAxisTitle( uint n, const QString& axisTitle )
{
    bool bDone = false;
    uint boxID;
    if( findFirstAxisCustomBoxID( n, boxID ) ){
        KDChartCustomBox* box = (KDChartCustomBox*)customBox( boxID );
        if( box ){
	  QString title = axisTitle;
	  const QString stripTitleString( title.simplifyWhiteSpace().upper() );
	  if( !stripTitleString.startsWith("<QT>" ) )
	    title.prepend("<qt><center>");
	  if( !stripTitleString.endsWith("</QT>" ) )
	    title.append("</center></qt>");
	  
	  KDChartTextPiece textPiece( 0, title,
				      box->content().font() );
	  
	  box->setContent( textPiece );            
          //qDebug ("old Axis Title updated");
	  bDone = true;
        }
    }
    if( !bDone ){
        insertDefaultAxisTitleBox( n,
                                   true,  axisTitle,
                                   false, QColor(),
                                   false, QFont(),
                                   false, false,
                                   false, 0 );
        //qDebug("new Axis Title Box inserted");
    }
    emit changed();
}
/**
  Returns the default axis title text

  \return the axis title text.
  \sa setAxisTitle
  */
QString KDChartParams::axisTitle( uint n ) const
{
    uint boxID;
    if( findFirstAxisCustomBoxID( n, boxID ) ){
        const KDChartCustomBox* box = customBox( boxID );
        if( box )
            return box->content().text();
    }
    return QString();
}

/**
  Specifies the color of the default axis title text.

  \param axisTitleColor the color of the axis title text
  \sa setAxisTitle, setAxisTitleFont,setAxisTitleFontUseRelSize, setAxisTitleFontRelSize
  \sa axisTitle, axisTitleColor, axisTitleFont, axisTitleFontUseRelSize, axisTitleFontRelSize
  */
void  KDChartParams::setAxisTitleColor( uint n, QColor axisTitleColor )
{
    bool bDone = false;
    uint boxID;
    if( findFirstAxisCustomBoxID( n, boxID ) ){
        KDChartCustomBox* box = (KDChartCustomBox*)customBox( boxID );
        if( box ){
            box->setColor( axisTitleColor );
            bDone = true;
        }
    }
    if( !bDone )
        insertDefaultAxisTitleBox( n,
                                   false, QString(),
                                   true,  axisTitleColor,
                                   false, QFont(),
                                   false, false,
                                   false, 0 );
    emit changed();
}
/**
  Returns the color of the default axis title text

  \return the color of the axis title text.
  \sa setAxisTitleColor
  */
QColor KDChartParams::axisTitleColor( uint n ) const
{
    uint boxID;
    if( findFirstAxisCustomBoxID( n, boxID ) ){
        const KDChartCustomBox* box = customBox( boxID );
        if( box )
            return box->color();
    }
    return Qt::darkBlue;
}

/**
  Specifies the font and the size of the default axis title text.

  \param axisTitleFont the font of the axis title text
  \param useFontSize flag indicating whether the font's size is to be used
  \param axisTitleFontRelSize the size to be used if \c useFontSize is false, this is interpreted as per mil value of the printable area size
  \sa setAxisTitle, setAxisTitleColor,setAxisTitleFontUseRelSize, setAxisTitleFontRelSize
  \sa axisTitle, axisTitleColor, axisTitleFont, axisTitleFontUseRelSize, axisTitleFontRelSize
  */
void  KDChartParams::setAxisTitleFont( uint n,
                                       QFont axisTitleFont )
{
    bool bDone = false;
    uint boxID;
    if( findFirstAxisCustomBoxID( n, boxID ) ){
        KDChartCustomBox* box = (KDChartCustomBox*)customBox( boxID );
        if( box ){
            KDChartTextPiece textPiece( 0, box->content().text(),
                                        axisTitleFont );
            box->setContent( textPiece );
            bDone = true;
        }
    }
    if( !bDone )
        insertDefaultAxisTitleBox( n,
                                   false, QString(),
                                   false, QColor(),
                                   true,  axisTitleFont,
                                   false, false,
                                   false, 0 );
    emit changed();
}

/**
  Specifies the font and the size of the default axis title text.
 \param axisTitleFont the font of the axis title text - by default the font will be relative 
 \param useFixedFontSize flag indicating whether the font's size is to be used as fixed or calculated  as per mil value. 
  \param axisTitleFontRelSize the size to be used if \c useFixedFontSize is false, this is interpreted as per mil value of the printable area size
  \sa setAxisTitle, setAxisTitleColor, setAxisTitleFont, setAxisTitleFontUseRelSize, setAxisTitleFontRelSize
  \sa axisTitle, axisTitleColor, axisTitleFont, axisTitleFontUseRelSize, axisTitleFontRelSize
  */

void KDChartParams::setAxisTitleFont( uint n,
				      QFont axisTitleFont,
                                      bool useFixedFontSize )
{

  bool bDone = false;
  uint boxID;
  if( findFirstAxisCustomBoxID( n, boxID ) ){
    KDChartCustomBox* box = (KDChartCustomBox*)customBox( boxID );
    if( box ){
      KDChartTextPiece textPiece( 0, box->content().text(),
				  axisTitleFont );
      int fixedFontSize = textPiece.font().pointSize();
      setAxisTitleFontRelSize( n, fixedFontSize );
      box->setContent( textPiece );
      bDone = true;
    }
  }
  if( !bDone )
    insertDefaultAxisTitleBox( n,
			       false, QString(),
			       false, QColor(),
			       true,  axisTitleFont,
			       false, false,
			       false, 0 );

  emit changed();
    
  if ( useFixedFontSize ) 
    setAxisTitleFontUseRelSize( n, false);
    
}


/**
  Returns the font of the default axis title text.

  \return the axis title's font.
  \sa setAxisTitleFont
  */
QFont KDChartParams::axisTitleFont( uint n ) const
{
    uint boxID;
    if( findFirstAxisCustomBoxID( n, boxID ) ){
        const KDChartCustomBox* box = customBox( boxID );
        if( box )
            return box->content().font();
    }
    return QFont( "helvetica", 6, QFont::Normal, false );
}

/**
  Specifies whether the fixed size of the the default axis title text's font.

  \param axisTitleFontUseRelSize flag indicating whether the font's size is to be used or the size specified via setAxisTitleFontRelSize.
  \sa setAxisTitle, setAxisTitleColor,setAxisTitleFont, setAxisTitleFontRelSize
  \sa axisTitle, axisTitleColor, axisTitleFont, axisTitleFontUseRelSize, axisTitleFontRelSize
  */
void  KDChartParams::setAxisTitleFontUseRelSize( uint n,
                                                 bool axisTitleFontUseRelSize )
{
    bool bDone = false;
    uint boxID;
    if( findFirstAxisCustomBoxID( n, boxID ) ){
        KDChartCustomBox* box = (KDChartCustomBox*)customBox( boxID );
        if( box ){
          if ( !axisTitleFontUseRelSize ) {
            if ( box->fontSize() < 0 )
	      box->setFontSize( -(box->fontSize()), true );
	  } else {
	    if( 0 <= box->fontSize() ) {
	      box->setFontSize( -(box->fontSize()), true );
	    } else
              box->setFontSize( box->fontSize(), true);
	  }
            bDone = true;
        }
    }
    if( !bDone )
        insertDefaultAxisTitleBox( n,
                                   false, QString(),
                                   false, QColor(),
                                   false, QFont(),
                                   true,  axisTitleFontUseRelSize,
                                   false, 0 );
    emit changed();
}
/**
  Returns whether the fix font size is used for the default axis title text.

  \return whether the fix axis title font size is used.
  \sa setAxisTitleFontUseRelSize
  */
bool KDChartParams::axisTitleFontUseRelSize( uint n ) const
{
    uint boxID;
    if( findFirstAxisCustomBoxID( n, boxID ) ){
        const KDChartCustomBox* box = customBox( boxID );
        if( box )
            return ( 0 > box->fontSize() );
    }
    return true;
}

/**
  Specifies whether a relative size to be used for the default axis title text instead of the fixed size of the font.

  Calling this function implicitely sets this axis's \c axisTitleFontUseRelSize flag to TRUE.

  \param axisTitleFontRelSize the size to be used, this is interpreted as per mil value of the printable area size.
  \sa setAxisTitle, setAxisTitleColor,setAxisTitleFont, setAxisTitleFontUseRelSize
  \sa axisTitle, axisTitleColor, axisTitleFont, axisTitleFontUseRelSize, axisTitleFontRelSize
  */
void  KDChartParams::setAxisTitleFontRelSize( uint n,
                                              int axisTitleFontRelSize )
{
    bool bDone = false;
    uint boxID;
    if( findFirstAxisCustomBoxID( n, boxID ) ){
        KDChartCustomBox* box = (KDChartCustomBox*)customBox( boxID );
        if( box ){
            box->setFontSize( -axisTitleFontRelSize, true );
            bDone = true;
        }
    }
    if( !bDone )
        insertDefaultAxisTitleBox( n,
                                   false, QString(),
                                   false, QColor(),
                                   false, QFont(),
                                   true,  true,
                                   true,  axisTitleFontRelSize );
    emit changed();
}
/**
  Returns the relative size of the default axis title text, should
  be ignored if \c axisTitleFontUseRelSize returns false.

  \return the axis title's relative font size.
  \sa setAxisTitleFontRelSize
  */
int KDChartParams::axisTitleFontRelSize( uint n ) const
{
    uint boxID;
    if( findFirstAxisCustomBoxID( n, boxID ) ){
        const KDChartCustomBox* box = customBox( boxID );
        if( box ){
            int i = box->fontSize();
            return (0 > i) ? -i : i;
        }
    }
    return 18;
}
