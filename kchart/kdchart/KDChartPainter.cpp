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
#if defined ( SUN7 ) || defined (_SGIAPI) || defined ( Q_WS_WIN)
  #include <math.h>
#else
  #include <cmath>
  #include <stdlib.h>
#endif

#include <KDDrawText.h>
#include <KDChartPainter.h>
#include <KDChartEnums.h>
#include <KDChartParams.h>
#include <KDChartCustomBox.h>
#include <KDChartTableBase.h>
#include <KDChartDataRegion.h>
#include <KDChartUnknownTypeException.h>
#include <KDChartNotEnoughSpaceException.h>
#include <KDChartBarPainter.h>
#include <KDChartAreaPainter.h>
#include <KDChartLinesPainter.h>
#include <KDChartPiePainter.h>
#include <KDChartPolarPainter.h>
#include <KDChartRingPainter.h>
#include <KDChartHiLoPainter.h>
#include <KDChartBWPainter.h>
#include <KDChartTextPiece.h>

#include <KDChart.h>  // for static method KDChart::painterToDrawRect()

#include <qpainter.h>
#include <qpaintdevice.h>
#include <q3paintdevicemetrics.h>
//Added by qt3to4:
#include <Q3PointArray>

#define DEGTORAD(d) (d)*M_PI/180


/**
  \class KDChartPainter KDChartPainter.h

  \brief An abstract base class that defines an interface for classes
  that implement chart drawing.

  Applications don't use this class directly (except for
  registering/unregistering, see below) new chart implementations,
  but instead use the method KDChart::paint() which takes care of the
  correct creation and deletion of the painter implementation
  used. Or they use KDChartWidget which handles everything
  automatically.

  This class cannot be instantiated directly. Even the concrete
  subclasses are not instantiated directly, but are instantiated via
  KDChartPainter::create() which creates a subclass according to the
  parameters passed.

  Application developers can provide their own chart implementations
  by subclassing from KDChartPainter, instantiating their subclass
  and registering their implementation with
  KDChartPainter::registerPainter(). These registrations can be
  removed with KDChartPainter::unregisterPainter().
  */

/**
  Constructor. Will only be called by subclass constructors since
  this class can never be instantiated directly.

  \param params the parameters of the chart to be drawn
  */
KDChartPainter::KDChartPainter( KDChartParams* params ) :
_outermostRect( QRect(QPoint(0,0), QSize(0,0))),
_legendTitle( 0 ),
_params( params ),
_legendNewLinesStartAtLeft( true ),
_legendTitleHeight( 0 ),
_legendTitleWidth( 0 ),
_legendTitleMetricsHeight( 0 )
{
    // This constructor intentionally left blank so far; we cannot setup the
    // geometry yet since we do not know the size of the painter.
}

/**
  Destructor. Cleans up any data structures that might have been allocated in
  the meantime.
  */
KDChartPainter::~KDChartPainter()
{
    delete _legendTitle;
}

bool KDChartPainter::calculateAllAxesLabelTextsAndCalcValues(
        QPainter*,
        KDChartTableDataBase*,
        double,
        double,
        double& )
{
    // This function intentionally returning false; it is implemented
    // by the KDChartAxesPainter class only.
    return false;
}

/**
  Creates an object of a concrete subclass of KDChartPainter that
  KDChart::paint() (and consequently, the application) can use to
  have charts painted. The subclass is determined on the base of the
  params parameter which among other things indicates the type of the
  chart.

  \param params the parameter set which is used to determine the
  painter implementation to be used
  \return a pointer to an object of a subclass of KDChartPainter that
  can be used to draw charts as defined by the \a params
  parameter. Returns 0 if there is no registered
  KDChartPainter subclass for the type specified in \a params. This
  can only happen with user-defined chart types.
  */
KDChartPainter* KDChartPainter::create( KDChartParams* params, bool make2nd )
{
    KDChartParams::ChartType cType = make2nd
        ? params->additionalChartType()
        : params->chartType();
    switch ( cType )
    {
        case KDChartParams::Bar:
            return new KDChartBarPainter( params );
        case KDChartParams::Line:
            return new KDChartLinesPainter( params );
        case KDChartParams::Area:
            return new KDChartAreaPainter( params );
        case KDChartParams::Pie:
            return new KDChartPiePainter( params );
        case KDChartParams::Ring:
            return new KDChartRingPainter( params );
        case KDChartParams::HiLo:
            return new KDChartHiLoPainter( params );
        case KDChartParams::BoxWhisker:
            return new KDChartBWPainter( params );
        case KDChartParams::Polar:
            return new KDChartPolarPainter( params );
        case KDChartParams::NoType:
        default:
            return 0;
    }
}


/**
  Registers a user-defined painter implementation which is identified
  by a string. If there is already a painter implementation
  registered under that name, the old registration will be deleted.

  KDChartPainter does not assume ownership of the registered painter,
  but you should unregister a painter before deleting an
  implementation object to avoid that that object is called after its
  deletion.

  \param painterName the name under which the painter implementation
  should be registered. This will be matched against the user-defined
  chart type name in the KDChartParams structure.
  \param painter an implementation object of a user-defined chart
  implementation
  */
void KDChartPainter::registerPainter( const QString& /*painterName*/,
        KDChartPainter* /*painter*/ )
{
    // PENDING(kalle) Implement this
    qDebug( "Sorry, not implemented:  KDChartPainter::registerPainter()" );
}


/**
  Unregisters a user-defined painter implementation. Does not delete
  the implementation object. If no implementation has been registered
  under this name, an exception is thrown if KDChart is compiled with
  exceptions, otherwise nothing happens.

  \param the name under which the painter implementation is
  registered
  */
void KDChartPainter::unregisterPainter( const QString& /*painterName*/ )
{
    // PENDING(kalle) Implement this
    qDebug( "Sorry, not implemented:  KDChartPainter::unregisterPainter()" );
}


/**
  Paints the chart for which this chart painter is configured on a
  QPainter. This is the method that bundles all the painting
  functions that paint specific parts of the chart like axes or
  legends. Subclasses can override this method, but should rarely
  need to do so.

  \param painter the QPainter onto which the chart should be drawn
  \param data the data which will be displayed as a chart
  \param regions a pointer to a region list that will be filled with
  regions representing the data segments if not null
  */
void KDChartPainter::paint( QPainter* painter,
                            KDChartTableDataBase* data,
                            bool paintFirst,
                            bool paintLast,
                            KDChartDataRegionList* regions,
                            const QRect* rect,
                            bool mustCalculateGeometry )
{

  
  if( paintFirst && regions ) 
        regions->clear();
  
    // Protect against non-existing data
    if( data->usedCols() == 0 && data->usedRows() == 0 )
        return ;

    QRect drawRect;
    //Pending Michel: at this point we have to setupGeometry
    if( mustCalculateGeometry || _outermostRect.isNull() ){
      if( rect ) 
            drawRect = *rect;
        else if( !KDChart::painterToDrawRect( painter, drawRect ) ){
            qDebug("ERROR: KDChartPainter::paint() could not calculate the drawing area.");
            return;
        }
      setupGeometry( painter, data, drawRect );
    }
    else
        drawRect = _outermostRect;

    //qDebug("A2: _legendRect:\n%i,%i\n%i,%i", _legendRect.left(),_legendRect.top(),_legendRect.right(),_legendRect.bottom() );


    // Note: In addition to the below paintArea calls there might be several
    //       other paintArea calls regarding to the BASE areas (AreaAxisBASE,
    //       AreaHdFtBASE, AreaCustomBoxesBASE).
    //       These additional calls result in smaller areas being drawn inside
    //       on the larger ones specifies here.
    if ( paintFirst ) {
        paintArea( painter, KDChartEnums::AreaOutermost );
        paintArea( painter, KDChartEnums::AreaInnermost );

        paintArea( painter, KDChartEnums::AreaDataAxesLegendHeadersFooters );

        paintArea( painter, KDChartEnums::AreaHeaders );
        paintArea( painter, KDChartEnums::AreaFooters );
        // header areas are drawn in the following order:
        //   1st center: main header, left main header, right main header
        //   2nd above:  header #0,   left header #0,   right header #0
        //   3rd below:  header #2,   left header #2,   right header #2
        paintArea( painter, KDChartEnums::AreaHdFtBASE + KDChartParams::HdFtPosHeader  );
        paintArea( painter, KDChartEnums::AreaHdFtBASE + KDChartParams::HdFtPosHeaderL );
        paintArea( painter, KDChartEnums::AreaHdFtBASE + KDChartParams::HdFtPosHeaderR );
        paintArea( painter, KDChartEnums::AreaHdFtBASE + KDChartParams::HdFtPosHeader0  );
        paintArea( painter, KDChartEnums::AreaHdFtBASE + KDChartParams::HdFtPosHeader0L );
        paintArea( painter, KDChartEnums::AreaHdFtBASE + KDChartParams::HdFtPosHeader0R );
        paintArea( painter, KDChartEnums::AreaHdFtBASE + KDChartParams::HdFtPosHeader2  );
        paintArea( painter, KDChartEnums::AreaHdFtBASE + KDChartParams::HdFtPosHeader2L );
        paintArea( painter, KDChartEnums::AreaHdFtBASE + KDChartParams::HdFtPosHeader2R );
        // footer areas are drawn in the same order as the header areas:
        paintArea( painter, KDChartEnums::AreaHdFtBASE + KDChartParams::HdFtPosFooter  );
        paintArea( painter, KDChartEnums::AreaHdFtBASE + KDChartParams::HdFtPosFooterL );
        paintArea( painter, KDChartEnums::AreaHdFtBASE + KDChartParams::HdFtPosFooterR );
        paintArea( painter, KDChartEnums::AreaHdFtBASE + KDChartParams::HdFtPosFooter0  );
        paintArea( painter, KDChartEnums::AreaHdFtBASE + KDChartParams::HdFtPosFooter0L );
        paintArea( painter, KDChartEnums::AreaHdFtBASE + KDChartParams::HdFtPosFooter0R );
        paintArea( painter, KDChartEnums::AreaHdFtBASE + KDChartParams::HdFtPosFooter2  );
        paintArea( painter, KDChartEnums::AreaHdFtBASE + KDChartParams::HdFtPosFooter2L );
        paintArea( painter, KDChartEnums::AreaHdFtBASE + KDChartParams::HdFtPosFooter2R );

        paintHeaderFooter( painter, data );

        paintArea( painter, KDChartEnums::AreaDataAxesLegend );
        paintArea( painter, KDChartEnums::AreaDataAxes );
        paintArea( painter, KDChartEnums::AreaAxes );
        for( int axis = KDChartAxisParams::AxisPosSTART;
             KDChartAxisParams::AxisPosEND >= axis; ++axis )
            paintArea( painter, KDChartEnums::AreaAxisBASE + axis );
        paintArea( painter, KDChartEnums::AreaData );
        paintAxes( painter, data );
    }

    painter->save();
    paintData( painter, data, !paintFirst, regions );
    painter->restore();

    if ( paintLast ) {
        // paint the frame lines of all little data region areas
        // on top of all data representations
        paintDataRegionAreas( painter, regions );
        if( KDChartParams::Bar          != params()->chartType() ||
            KDChartParams::BarMultiRows != params()->barChartSubType() )
            paintDataValues( painter, data, regions );
        if (params()->legendPosition()!=KDChartParams::NoLegend)
            paintArea(   painter, KDChartEnums::AreaLegend );
        paintLegend( painter, data );
        paintCustomBoxes( painter, regions );
    }
}


/**
  Paints an area frame.
  */
void KDChartPainter::paintArea( QPainter* painter,
        uint area,
        KDChartDataRegionList* regions,
        uint dataRow,
        uint dataCol,
        uint data3rd )
{
    if( KDChartEnums::AreaCustomBoxesBASE != (KDChartEnums::AreaBASEMask & area) ){
        bool bFound;
        const KDChartParams::KDChartFrameSettings* settings =
            params()->frameSettings( area, bFound );
        if( bFound ) {
            bool allCustomBoxes;
            QRect rect( calculateAreaRect( allCustomBoxes,
                                           area,
                                           dataRow, dataCol, data3rd, regions ) );

            if( !allCustomBoxes )
                paintAreaWithGap( painter, rect, *settings );
        }
    }
}


void KDChartPainter::paintDataRegionAreas( QPainter* painter,
                                           KDChartDataRegionList* regions )
{
    if( regions ){
        int iterIdx;
        bool bFound;
        const KDChartParams::KDChartFrameSettings* settings =
            params()->frameSettings( KDChartEnums::AreaChartDataRegion, bFound, &iterIdx );
        while( bFound ) {
            bool bDummy;
            QRect rect( calculateAreaRect( bDummy,
                                           KDChartEnums::AreaChartDataRegion,
                                           settings->dataRow(),
                                           settings->dataCol(),
                                           settings->data3rd(),
                                           regions ) );
            // NOTE: we can *not* draw any background behind the
            //       data representations.
            // reason: for being able to do that we would have to
            //         know the respective regions _before_ the
            //         data representations are drawn; since that
            //         is impossible, we just draw the borders only
            //         ( == the corners and the edges ) and ignore the background
            //
            // (actually: Since the respective interface function does not allow
            //            specifying a background there is nothing to be ignored anyway.)
            settings->frame().paint( painter,
                                     KDFrame::PaintBorder,
                                     trueFrameRect( rect, settings ) );
            settings = params()->nextFrameSettings( bFound, &iterIdx );
        }
    }
}


QRect KDChartPainter::trueFrameRect( const QRect& orgRect,
                                     const KDChartParams::KDChartFrameSettings* settings ) const
{
    QRect rect( orgRect );
    if( settings ){
        rect.moveBy( -settings->innerGapX(), -settings->innerGapY() );
        rect.setWidth(  rect.width()  + 2*settings->innerGapX() );
        rect.setHeight( rect.height() + 2*settings->innerGapY() );
    }
    return rect;
}


/**
  Paints an area frame.
  This methode is called internally by KDChartPainter::paintArea.
  NOTE: areas around KDChartCustomBoxes are _not_ drawn here but
        in KDChartCustomBox::paint() which is called by paintCustomBoxes().
  */
void KDChartPainter::paintAreaWithGap( QPainter* painter,
                                       QRect rect,
                                       const KDChartParams::KDChartFrameSettings& settings )
{
    if( painter && rect.isValid() )
        settings.frame().paint( painter,
                                KDFrame::PaintAll,
                                trueFrameRect( rect, &settings ) );
}


/**
  Paints the data value texts near the data representations.
  */
void KDChartPainter::paintDataValues( QPainter* painter,
        KDChartTableDataBase* data,
        KDChartDataRegionList* regions )
{
    KDChartDataRegion* region;
    if (    painter
            && data
            && regions
            && regions->count()
            && params()
            && (    params()->printDataValues( 0 )
                || params()->printDataValues( 1 ) ) ) {

        // out of loop status saving
        painter->save();
     
        QFont font0( params()->dataValuesFont( 0 ) );
         
        if( params()->dataValuesUseFontRelSize(  0 ) ) {
            float size = qMin(_areaWidthP1000, _areaHeightP1000) * abs(params()->dataValuesFontRelSize( 0 ));        
            if ( 9.0 > size )
                size = 9.0;
            font0.setPixelSize( static_cast < int > ( size ) );
        }
        painter->setFont( font0 );
        QFontMetrics fm0( painter->fontMetrics() );
        double fm0HeightP100( fm0.height() / 100.0 );
        QFont font1( params()->dataValuesFont( 1 ) );

        if( params()->dataValuesUseFontRelSize(  1 ) ) {
            float size = qMin(_areaWidthP1000, _areaHeightP1000) * abs(params()->dataValuesFontRelSize( 1 ));
            if ( 9.0 > size )
                size = 9.0;
            font1.setPixelSize( static_cast < int > ( size ) );
        } else 
	  font1.setPixelSize( font0.pixelSize());
        painter->setFont( font1 );
        QFontMetrics fm1( painter->fontMetrics() );
        double fm1HeightP100( fm1.height() / 100.0 );

        bool lastDigitIrrelevant0 = true;
        bool lastDigitIrrelevant1 = true;
        // get and format the texts
        for ( region=regions->first();
                region != 0;
                region = regions->next() ) {
            QVariant vValY;
            if( data->cellCoord( region->row, region->col, vValY, 1 ) ){
                if( QVariant::String == vValY.type() ){
                    const QString sVal( vValY.toString() );
                    if( !sVal.isEmpty() )
                        region->text = sVal;
                }else if( QVariant::Double == vValY.type() ){
                    double value( vValY.toDouble() );
                    region->negative = 0.0 > value;
                    double divi( pow( 10.0, params()->dataValuesDivPow10( region->chart ) ) );
                    if ( 1.0 != divi )
                        value /= divi;
                    int digits( params()->dataValuesDigitsBehindComma( region->chart ) );
                    bool autoDigits( KDCHART_DATA_VALUE_AUTO_DIGITS == digits );
                    if( autoDigits ) {
                        if( 10 < digits )
                            digits = 10;
                    } else
                        (   region->chart
                            ? lastDigitIrrelevant1
                            : lastDigitIrrelevant0 ) = false;
                    if( value == KDCHART_NEG_INFINITE )
                        region->text = "-LEMNISKATE";
                    else if( value == KDCHART_POS_INFINITE )
                        region->text = "+LEMNISKATE";
                    else {
                        region->text.setNum( value, 'f', digits );
                        if ( autoDigits && region->text.contains( '.' ) ) {
                            int len = region->text.length();
                            while (    3 < len
                                    && '0' == region->text[ len-1 ]
                                    && '.' != region->text[ len-2 ] ) {
                                --len;
                                region->text.truncate( len );
                            }
                            if( '0' != region->text[ len-1 ] )
                                (   region->chart
                                    ? lastDigitIrrelevant1
                                    : lastDigitIrrelevant0 ) = false;
                        }
                    }
                }
            }
        }

        if ( lastDigitIrrelevant0 || lastDigitIrrelevant1 )
            for ( region=regions->first();
                    region != 0;
                    region = regions->next() )
                if (   (     ( lastDigitIrrelevant0 && !region->chart )
                            || ( lastDigitIrrelevant1 &&  region->chart ) )
                        && region->text.contains( '.' )
                        && ( 2 < region->text.length() ) )
                    region->text.truncate ( region->text.length() - 2 );


        // draw the Data Value Texts and calculate the text regions
        painter->setPen( Qt::black );

        bool allowOverlapping = params()->allowOverlappingDataValueTexts();
        bool drawThisOne;
        QRegion lastRegionDone;

        QFontMetrics actFM( painter->fontMetrics() );

        QFont* oldFont = 0;
        int oldRotation = 0;
        uint oldChart = UINT_MAX;
        uint oldDatacolorNo = UINT_MAX;
        for ( region=regions->first();
                region != 0;
                region = regions->next() ) {

            // in loop status saving
            painter->save();

            if ( region->text.length() ) {

                QVariant vValY;
                bool zero =
                    data->cellCoord( region->row, region->col, vValY, 1 ) &&
                    QVariant::Double == vValY.type() &&
                    ( 0.0 == vValY.toDouble() || 0 == vValY.toDouble() );
                uint align( params()->dataValuesAnchorAlign( region->chart,
                            region->negative ) );
                KDChartParams::ChartType cType = region->chart
                    ? params()->additionalChartType()
                    : params()->chartType();


                // these use the bounding rect of region-region:
                bool bIsAreaChart = KDChartParams::Area == cType;
                bool rectangular = (    KDChartParams::Bar        == cType
                                     || KDChartParams::Line       == cType
                                     || bIsAreaChart
                                     || KDChartParams::HiLo       == cType
                                     || KDChartParams::BoxWhisker == cType );

                // these use the nine anchor points stored in region->points
                bool circular    = (    KDChartParams::Pie        == cType
                                     || KDChartParams::Ring       == cType
                                     || KDChartParams::Polar      == cType );


                KDChartEnums::PositionFlag anchorPos(
                    params()->dataValuesAnchorPosition( region->chart, region->negative ) );

                QPoint anchor(
                        rectangular
                        ? KDChartEnums::positionFlagToPoint( region->rect(), anchorPos )
                        : KDChartEnums::positionFlagToPoint( region->points, anchorPos ) );

                double & fmHeightP100 = region->chart ? fm1HeightP100 : fm0HeightP100;

                int angle = region->startAngle;
                switch ( anchorPos ) {
                    case KDChartEnums::PosTopLeft:
                    case KDChartEnums::PosCenterLeft:
                    case KDChartEnums::PosBottomLeft:
                        angle += region->angleLen;
                        break;
                    case KDChartEnums::PosTopCenter:
                    case KDChartEnums::PosCenter:
                    case KDChartEnums::PosBottomCenter:
                        angle += region->angleLen / 2;
                        break;
                        /*
                           case KDChartEnums::PosTopRight:
                           case KDChartEnums::PosCenterRight:
                           case KDChartEnums::PosBottomRight:
                           angle += 0;
                           break;
                           */
                    default:
                        break;
                }
                 double anchorDX( params()->dataValuesAnchorDeltaX( region->chart, region->negative )
                        * fmHeightP100 );
                 double anchorDY( params()->dataValuesAnchorDeltaY( region->chart, region->negative )
                        * fmHeightP100 );
                 if ( circular ) {
                     if ( 0.0 != anchorDY ) {
                        double normAngle = angle / 16;
                        double normAngleRad = DEGTORAD( normAngle );
                        double sinAngle = sin( normAngleRad );
                        QPoint& pM = region->points[ KDChartEnums::PosCenter ];
                        double dX( pM.x() - anchor.x() );
                        double dY( pM.y() - anchor.y() );
                        double radialLen( sinAngle ? dY / sinAngle : dY );
                        double radialFactor( ( radialLen == 0.0 ) ? 0.0 : ( ( radialLen - anchorDY ) / radialLen ) );
                        anchor.setX( static_cast < int > ( pM.x() - dX * radialFactor ) );
                        anchor.setY( static_cast < int > ( pM.y() - dY * radialFactor ) );
                     }
                } else {
                    anchor.setX( anchor.x() + static_cast < int > ( anchorDX ) );
                    anchor.setY( anchor.y() + static_cast < int > ( anchorDY ) );
                }


                if(anchor.x() < -250){
                    anchor.setX(-250);
                    //qDebug("!! bad negative x position in KDChartPainter::paintDataValues() !!");
                }
                if(anchor.y() < -2500){
                    anchor.setY(-2500);
                    //qDebug("!! bad negative y position in KDChartPainter::paintDataValues() !!");
                }

                int rotation( params()->dataValuesRotation( region->chart,
                                                            region->negative ) );
                bool incRotationBy90 = false;
                if( region->text == "-LEMNISKATE" ||
                        region->text == "+LEMNISKATE" ){
                    if( params()->dataValuesShowInfinite( region->chart ) ){
                        //bool bIsLineChart = KDChartParams::Line == cType;
                        if( region->text == "-LEMNISKATE" )
                            align = Qt::AlignRight + Qt::AlignVCenter;
                        else
                            align = Qt::AlignLeft  + Qt::AlignVCenter;
                        if( !rotation )
                            rotation = 90;
                        else
                            incRotationBy90 = true;
                        region->text = " 8 ";
                    }else{
                        region->text = "";
                    }
                }

                if ( rotation ) {
		  anchor = painter->worldMatrix().map( anchor );

                    //   Temporary solution for fixing the data labels size
                    // bug when in QPrinter::HighResolution mode:
                    //   There seem to be no backdraws by acting like this,
                    // but further investigation is required to detect the
                    // real error in the previous code/
                    if (    KDCHART_SAGGITAL_ROTATION   == rotation
                         || KDCHART_TANGENTIAL_ROTATION == rotation ) {
                        rotation = (   KDCHART_TANGENTIAL_ROTATION == rotation
                                     ? -1440
                                     : 0 )
                                 + angle;
                        rotation /= 16;
                        if( incRotationBy90 )
                            rotation += 90;
                        if ( 360 <= rotation )
                            rotation -= 360;
                        else if ( 0 > rotation )
                            rotation += 360;
                        rotation = 360 - rotation;
                    }else if( incRotationBy90 )
                        rotation = (rotation + 90) % 360;


		     if( rotation != oldRotation ) {
		      painter->rotate( rotation - oldRotation );
                      // Comment this out - zooming and scrolling
		      // oldRotation = rotation;
			 }

                    QFont* actFont = region->chart ? &font1 : &font0;
                    if( oldFont != actFont ) {
                        painter->setFont( *actFont );
                        actFM = QFontMetrics( painter->fontMetrics() );
			// Comment this out - zooming and scrolling
                        //oldFont = actFont;
                    }

                    KDDrawTextRegionAndTrueRect infosKDD =
                        KDDrawText::measureRotatedText( painter,
                                                        rotation,
                                                        anchor,
                                                        region->text,
                                                        0,
                                                        align,
                                                        &actFM,
                                                        true,
                                                        true,
                                                        5 );
                    //anchor = painter->worldMatrix().map( anchor );

                    if( allowOverlapping ) {
                        drawThisOne = true;
                    }else {
                        QRegion sectReg( infosKDD.region.intersect( lastRegionDone ) );
                        drawThisOne = sectReg.isEmpty();
                    }
                    if( drawThisOne ) {
                        lastRegionDone     = lastRegionDone.unite( infosKDD.region );
                        region->pTextRegion = new QRegion( infosKDD.region );

                        if( params()->dataValuesAutoColor( region->chart ) ) {
                            if( bIsAreaChart ){
                                QColor color( params()->dataColor( region->row ) );
                                /*
                                if(    ( (0.0 > anchorDY) &&  region->negative )
                                    || ( (0.0 < anchorDY) && !region->negative ) )
                                    painter->setPen(
                                        QColor( static_cast < int > ( 255- color.red() ),
                                                static_cast < int > ( 255- color.green() ),
                                                static_cast < int > ( 255- color.blue() ) ) );
                                else
                                */
                                    painter->setPen( color.dark() );
                            }else{
                                if( zero ) {
                                    if( oldDatacolorNo != UINT_MAX ) {
                                        painter->setPen( Qt::black );
                                        oldDatacolorNo = UINT_MAX;
                                    }
                                }
                                else {
                                    uint datacolorNo = (    KDChartParams::Pie   == cType
                                                        || KDChartParams::Ring  == cType )
                                        ? region->col
                                        : region->row;
                                    if(  oldDatacolorNo != datacolorNo ) {
                                        oldDatacolorNo = datacolorNo;
                                        QColor color( params()->dataColor( datacolorNo ) );
                                        painter->setPen( QColor(
                                                        static_cast < int > (255-color.red()  ),
                                                        static_cast < int > (255-color.green()),
                                                        static_cast < int > (255-color.blue() )));
                                    }
                                }
                            }
                        }
                        else if( oldChart != region->chart ) {
                            oldChart = region->chart;
                            painter->setPen( params()->dataValuesColor( region->chart ) );
                        }

                        if( params()->optimizeOutputForScreen() ){
                            painter->rotate( -oldRotation );
                            oldRotation = 0;
                            if ( anchor.y() < 0 )
			      anchor.setY( -anchor.y() );

                            KDDrawText::drawRotatedText( painter,
                                                         rotation,
                                                         anchor,
                                                         region->text,
                                                         region->chart ? &font1 : &font0,
                                                         align,
                                                         false,   // bool showAnchor
                                                         0,       // const QFontMetrics* fontMet
                                                         false,   // bool noFirstrotate
                                                         false,   // bool noBackrotate
                                                         0,       // KDDrawTextRegionAndTrueRect* infos
                                                         true );  // bool optimizeOutputForScreen
                        }else{
                           painter->setPen( params()->dataValuesColor( region->chart ) );
                           painter->drawText( infosKDD.x , infosKDD.y ,
                                               infosKDD.width, infosKDD.height,
                                               Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextSingleLine,
                                               region->text );

                        }


                    } // if not intersect

                } else {

                    // no rotation:
                    painter->rotate( -oldRotation );
                    oldRotation = 0;
                    QFontMetrics & fm = region->chart ? fm1 : fm0;
                    int boundingRectWidth = fm.boundingRect( region->text ).width();
                    int leftBearing = fm.leftBearing( region->text[ 0 ] );
                    const QChar c =  region->text.at( region->text.length() - 1 );
                    int rightBearing = fm.rightBearing( c );
                    int w =  boundingRectWidth + leftBearing + rightBearing + 1;
                    int h = fm.height(); // ascent + descent + 1
                    int dx = 0;
                    int dy = 0;
                    switch( align & ( Qt::AlignLeft | Qt::AlignRight | Qt::AlignHCenter ) ) {
                        case Qt::AlignRight:
                            dx = -w+1;
                            break;
                        case Qt::AlignHCenter:
                            // Center on the middle of the bounding rect, not
                            // the painted area, because numbers appear centered then
                            dx = -( ( boundingRectWidth / 2 ) + leftBearing );
                            break;
                    }
                    switch( align & ( Qt::AlignTop | Qt::AlignBottom | Qt::AlignVCenter ) ) {
                        case Qt::AlignBottom:
                            dy = -h+1;
                            break;
                        case Qt::AlignVCenter:
                            dy = -h / 2;
                            break;
                    }

                    QRegion thisRegion(
                            QRect( anchor.x() + dx, anchor.y() + dy, w, h ) );
                    if( allowOverlapping )
                        drawThisOne = true;
                    else {
                        QRegion sectReg( thisRegion.intersect( lastRegionDone ) );
                        drawThisOne = sectReg.isEmpty();
                    }
                    if( drawThisOne ) {
                        lastRegionDone      = lastRegionDone.unite( thisRegion );
                        region->pTextRegion = new QRegion( thisRegion );
#ifdef DEBUG_TEXT_PAINTING
                        // for testing:
                        QRect rect( region->pTextRegion->boundingRect() );
                        painter->drawRect( rect );
                        painter->setPen( Qt::red );
                        rect.setLeft( rect.left() + leftBearing );
                        rect.setTop( rect.top() + ( fm.height()-fm.boundingRect( region->text ).height() ) /2 );
                        rect.setWidth( fm.boundingRect( region->text ).width() );
                        rect.setHeight( fm.boundingRect( region->text ).height() );
                        painter->drawRect( rect );
                        painter->setPen( Qt::black );
#endif
                        /*

NOTE: The following will be REMOVED again once
the layout policy feature is implemented !!!

*/
                        QRect textRect( region->pTextRegion->boundingRect() );
                        if( bIsAreaChart ){
                            QBrush brush( params()->dataValuesBackground( region->chart ) );
                            painter->setBrush( brush );
                            painter->setPen(   Qt::NoPen );
                            QRect rect( textRect );
                            rect.moveBy( -2, 0 );
                            rect.setWidth( rect.width() + 4 );
                            painter->drawRect( rect );
                        }
                        painter->setFont( region->chart ? font1 : font0 );
                        if( params()->dataValuesAutoColor( region->chart ) ) {
                            if( bIsAreaChart ){
                                QColor color( params()->dataColor( region->row ) );
                                /*
                                if(    ( (0.0 > anchorDY) &&  region->negative )
                                    || ( (0.0 < anchorDY) && !region->negative ) )
                                    painter->setPen(
                                        QColor( static_cast < int > ( 255- color.red() ),
                                                static_cast < int > ( 255- color.green() ),
                                                static_cast < int > ( 255- color.blue() ) ) );
                                else
                                */
                                    painter->setPen( color.dark() );
                            }else{
                                if( zero )
                                    painter->setPen( Qt::black );
                                else {
                                    QColor color( params()->dataColor(
                                                (    KDChartParams::Pie   == params()->chartType()
                                                    || KDChartParams::Ring  == params()->chartType() )
                                                ? region->col
                                                : region->row ) );
                                    painter->setPen( QColor( static_cast < int > ( 255- color.red() ),
                                                static_cast < int > ( 255- color.green() ),
                                                static_cast < int > ( 255- color.blue() ) ) );
                                }
                            }
                        }else{
                            painter->setPen( params()->dataValuesColor( region->chart ) );
                        }
                        painter->drawText( textRect.left(),    textRect.top(),
                                           textRect.width()+1, textRect.height()+1,
                                           Qt::AlignLeft | Qt::AlignTop, region->text );
                    }


                }
            }
            //
	      painter->restore();

        }
	painter->restore();
    }

}


/**
  Paints all custom boxes.
  */
void KDChartPainter::paintCustomBoxes( QPainter* painter,
                                       KDChartDataRegionList* regions )
{
    // paint all of the custom boxes AND their surrounding frames+background (if any)
    bool bGlobalFound;
    const KDChartParams::KDChartFrameSettings* globalFrameSettings
        = params()->frameSettings( KDChartEnums::AreasCustomBoxes, bGlobalFound );

    uint idx;
    for( idx = 0; idx <= params()->maxCustomBoxIdx(); ++idx ) {
        const KDChartCustomBox * box = params()->customBox( idx );
        if( box ) {
            // paint border and background
            paintArea( painter,
                       KDChartEnums::AreaCustomBoxesBASE + idx,
                       regions,
                       box->dataRow(),
                       box->dataCol(),
                       box->data3rd() );
            // retrieve frame information
            bool bIndividualFound;
            const KDChartParams::KDChartFrameSettings * individualFrameSettings
                = params()->frameSettings( KDChartEnums::AreaCustomBoxesBASE + idx,
                                           bIndividualFound );
            const KDChartParams::KDChartFrameSettings * settings
                = bIndividualFound ? individualFrameSettings
                                   : bGlobalFound ? globalFrameSettings : 0;
            // paint content
            const QPoint anchor( calculateAnchor( *box, regions ) );
            box->paint( painter,
                        anchor,
                        _areaWidthP1000,
                        _areaHeightP1000,
                        settings ? settings->framePtr() : 0,
                        trueFrameRect( box->trueRect( anchor, _areaWidthP1000, _areaHeightP1000 ),
                                       settings ) );
        }
    }
}


/**
  Calculated the top left corner of a custom box.
  */
QPoint KDChartPainter::calculateAnchor( const KDChartCustomBox & box,
        KDChartDataRegionList* regions ) const
{
    QPoint pt(0,0);

    // Recursion handling:
    //
    //    *  calculateAnchor() normally calls calculateAreaRect()
    //
    //    *  calculateAreaRect() will in turn calls calculateAnchor() in case of
    //       box.anchorArea() being based on KDChartEnums::AreaCustomBoxesBASE
    //
    //    This is Ok as long as the recursive call of calculateAnchor() is NOT
    //    intend examination the same box as a previous call.
    //
    // Rule:
    //
    //    A box may be aligned to another box (and the 2nd box may again be
    //    aligned to a 3rd box and so on) but NO CIRCULAR alignment is allowed.
    //
    if( !box.anchorBeingCalculated() ) {

        box.setInternalFlagAnchorBeingCalculated( true );

        bool allCustomBoxes;
        QRect rect( calculateAreaRect( allCustomBoxes,
                                       box.anchorArea(),
                                       box.dataRow(),
                                       box.dataCol(),
                                       box.data3rd(),
                                       regions ) );
        if( allCustomBoxes ) {
            //
            //  Dear user of this library.
            //
            //  You faced the above error during program runtime?
            //
            //  The reason for this is that you may NOT use  AreasCustomBoxes
            //  as a value for the KDChartCustomBox anchor area.
            //
            //  This is due to the fact that an anchor area allways must specify one AREA
            //  or some contiguous areas that form an area when combined.
            //  The flag  AreasCustomBoxes  however specifies a list of custom boxes
            //  that normally do not form a contiguos ares, so they cannot be used as anchor area.
            //
            //  In order to specify a SINGLE custom box please use AreaCustomBoxBASE+boxId.
            //
        }
        pt = KDChartEnums::positionFlagToPoint( rect, box.anchorPosition() );

        box.setInternalFlagAnchorBeingCalculated( false );
    }

    return pt;
}


/**
  Calculated the rectangle covered by an area.
  NOTE: KDChartCustomBox areas are _not_ calculated here.
  */
QRect KDChartPainter::calculateAreaRect( bool & allCustomBoxes,
                                         uint area,
                                         uint dataRow,
                                         uint dataCol,
                                         uint /*data3rd*/,
                                         KDChartDataRegionList* regions ) const
{
    QRect rect(0,0, 0,0);
    allCustomBoxes = false;
    uint pos;
    switch( area ) {
        case KDChartEnums::AreaData:
            rect = _dataRect;
            break;
        case KDChartEnums::AreaAxes:
            break;
        case KDChartEnums::AreaLegend:
            rect = _legendRect;
            break;
        case KDChartEnums::AreaDataAxes:
            rect = _axesRect;
            break;
        case KDChartEnums::AreaDataAxesLegend:
            rect = _axesRect;
            if( _legendRect.isValid() ) {
                if( rect.isValid() )
                    rect = rect.unite( _legendRect );
                else
                    rect = _legendRect;
            }
            break;
        case KDChartEnums::AreaHeaders: {
                                            bool bStart = true;
                                            for( pos = KDChartParams::HdFtPosHeadersSTART;
                                                    KDChartParams::HdFtPosHeadersEND >= pos;
                                                    ++pos ) {
                                                const QRect& r = params()->headerFooterRect( pos );
                                                if( r.isValid() ) {
                                                    if( bStart )
                                                        rect = r;
                                                    else
                                                        rect = rect.unite( r );
                                                    bStart = false;
                                                }
                                            }
                                        }
                                        break;
        case KDChartEnums::AreaFooters: {
                                            bool bStart = true;
                                            for( pos = KDChartParams::HdFtPosFootersSTART;
                                                    KDChartParams::HdFtPosFootersEND >= pos;
                                                    ++pos ) {
                                                const QRect& r = params()->headerFooterRect( pos );
                                                if( r.isValid() ) {
                                                    if( bStart )
                                                        rect = r;
                                                    else
                                                        rect = rect.unite( r );
                                                    bStart = false;
                                                }
                                            }
                                        }
                                        break;
        case KDChartEnums::AreaDataAxesLegendHeadersFooters: {
                                                                 rect = _axesRect;
                                                                 bool bStart = !rect.isValid();
                                                                 if( _legendRect.isValid() ) {
                                                                     if( bStart )
                                                                         rect = _legendRect;
                                                                     else
                                                                         rect = rect.unite( _legendRect );
                                                                     bStart = false;
                                                                 }
                                                                 for( pos = KDChartParams::HdFtPosSTART;
                                                                         KDChartParams::HdFtPosEND >= pos;
                                                                         ++pos ) {
                                                                     const QRect& r = params()->headerFooterRect( pos );
                                                                     if( r.isValid() ) {
                                                                         if( bStart )
                                                                             rect = r;
                                                                         else
                                                                             rect = rect.unite( r );
                                                                         bStart = false;
                                                                     }
                                                                 }
                                                             }
                                                             break;
        case KDChartEnums::AreaOutermost:
                                                             rect = _outermostRect;
                                                             break;
        case KDChartEnums::AreaInnermost:
                                                             rect = _innermostRect;
                                                             break;
        case KDChartEnums::AreasCustomBoxes:
                                                             allCustomBoxes = true;
                                                             break;
        case KDChartEnums::AreaChartDataRegion:
                                                             if( regions ) {
                                                                 KDChartDataRegion* current;
                                                                 for ( current = regions->first();
                                                                         current != 0;
                                                                         current =  regions->next() ) {
                                                                     if (    current->row == dataRow
                                                                             && current->col == dataCol
                                                                             //
                                                                             // the line below prepared for true 3-dimensional data charts
                                                                             //
                                                                             /* && current->region.thirdDimension == data3rd */ ) {
                                                                         rect = current->rect();
                                                                         break;
                                                                     }
                                                                 }
                                                             }
                                                             break;
        case KDChartEnums::AreaUNKNOWN:
                                                             break;

        default: {
                     uint maskBASE = KDChartEnums::AreaBASEMask & area;
                     pos = area - maskBASE;
                     if ( KDChartEnums::AreaAxisBASE == maskBASE ) {
                         rect = params()->axisParams( pos ).axisTrueAreaRect();
                     } else if ( KDChartEnums::AreaHdFtBASE == maskBASE ) {
                         rect = params()->headerFooterRect( pos );
                     } else if ( KDChartEnums::AreaCustomBoxesBASE == maskBASE ) {
                         const KDChartCustomBox * box = params()->customBox( pos );
                         if( box ) {
                             rect = box->trueRect( calculateAnchor( *box, regions ),
                                     _areaWidthP1000,
                                     _areaHeightP1000 );
                         }
                     }
                 }
    }
    return rect;
}


QPoint KDChartPainter::pointOnCircle( const QRect& rect, double angle )
{
    // There are two ways of computing this: The simple, but slow one
    // is to use QPointArray.makeArc() and take the first point. The
    // more advanced, but faster one is to do the trigonometric
    // computionations ourselves. Since the comments in
    // QPointArray::makeArc() very often say that the code there is
    // "poor", we'd better do it outselves...

    double normAngle = angle / 16.0;
    double normAngleRad = DEGTORAD( normAngle );
    double cosAngle = cos( normAngleRad );
    double sinAngle = -sin( normAngleRad );
    double posX = floor( cosAngle * ( double ) rect.width() / 2.0 + 0.5 );
    double posY = floor( sinAngle * ( double ) rect.height() / 2.0 + 0.5 );
    return QPoint( static_cast<int>(posX) + rect.center().x(),
                   static_cast<int>(posY) + rect.center().y() );

}

void KDChartPainter::makeArc( Q3PointArray& points,
                              const QRect& rect,
                              double startAngle, double angles )
{
    double endAngle = startAngle + angles;
    int rCX = rect.center().x();
    int rCY = rect.center().y();
    double rWid2 = ( double ) rect.width() / 2.0;
    double rHig2 = ( double ) rect.height() / 2.0;
    int numSteps = static_cast<int>(angles);
    if( floor( angles ) < angles )
        ++numSteps;
    points.resize( numSteps );
    double angle = startAngle;
    if( angle < 0.0 )
        angle += 5760.0;
    else if( angle >= 5760.0 )
        angle -= 5760.0;
    for(int i = 0; i < numSteps; ++i){
        double normAngle = angle / 16.0;
        double normAngleRad = DEGTORAD( normAngle );
        double cosAngle = cos( normAngleRad );
        double sinAngle = -sin( normAngleRad );
        double posX = floor( cosAngle * rWid2 + 0.5 );
        double posY = floor( sinAngle * rHig2 + 0.5 );
        points[i] = QPoint( ( int ) posX + rCX,
                            ( int ) posY + rCY );
        if( i+1 >= numSteps-1 )
            angle = endAngle; // the very last step width may be smaller than 1.0
        else
            angle += 1.0;
        if( angle >= 5760.0 )
            angle -= 5760.0;
    }
}

/**
  Paints the axes for the chart. The implementation in KDChartPainter
  does nothing; subclasses for chart types that have axes will
  provide the appropriate drawing code here. This method serves as a
  fallback for chart types that do not have axes (like pies).

  \param painter the QPainter onto which the chart should be drawn
  \param data the data that will be displayed as a chart
  */
void KDChartPainter::paintAxes( QPainter* /*painter*/, KDChartTableDataBase* /*data*/ )
{
    // This method intentionally left blank.
}


int KDChartPainter::legendTitleVertGap() const
{
    return _legendTitleHeight
           + static_cast < int > ( _legendTitleMetricsHeight * 0.20 );
}


QFont KDChartPainter::trueLegendFont() const
{
    QFont trueFont = params()->legendFont();
    if ( params()->legendFontUseRelSize() ) {
        const double averageValueP1000 = qMin(_areaWidthP1000, _areaHeightP1000);//( _areaWidthP1000 + _areaHeightP1000 ) / 2.0;
        trueFont.setPixelSize(
            static_cast < int > ( params()->legendFontRelSize() * averageValueP1000 ) );
    }
    return trueFont;
}


/**
  Calculates the size of the rectangle for horizontal legend orientation.

  \param painter the QPainter onto which the chart should be drawn
  */
void KDChartPainter::calculateHorizontalLegendSize( QPainter* painter,
                                                    QSize& size,
                                                    bool& legendNewLinesStartAtLeft ) const
{
 
  legendNewLinesStartAtLeft = false;
  QRect legendRect( _legendRect );
  /*
   * Pending Michel reset the left side before calculating 
   *the new legend position calculation
   *otherwise we occasionally reach the edge and get a wrong 
   *result
   */

  legendRect.setLeft( _innermostRect.left() );
  
    const int em2 = 2 * _legendEMSpace;
    const int em4 = 4 * _legendEMSpace;
    const int emDiv2 = static_cast < int > ( _legendEMSpace / 2.0 );

    const int xposHori0 = legendRect.left() + _legendEMSpace;
    
    int xpos = xposHori0;

    int ypos = legendRect.top() + emDiv2;

    // first paint the title, if any
    if( _legendTitle )
        xpos += _legendTitleWidth + em4;

    int maxX = _legendTitleWidth + _legendEMSpace;

    // save the x position: here start the item texts if in horizontal mode
    int xposHori1 = xpos;

    // add the space of the box plus the space between the box and the text
    int x2 = xpos + em2;

    // loop over all the datasets, each one has one row in the legend
    // if its data are to be used in at least one of the charts drawn
    // *but* only if there is a legend text for it!
    const int rightEdge = _innermostRect.right()-_legendEMSpace;
    bool bFirstLFWithTitle = _legendTitle;
    painter->setFont( trueLegendFont() );
    QFontMetrics txtMetrics( painter->fontMetrics() );
    int dataset;
    for ( dataset = 0; dataset < _numLegendTexts; ++dataset ) {
        /*
           if( KDChartParams::DataEntry == params()->chartSourceMode( dataset ) ) {
           */
        if( !_legendTexts[ dataset ].isEmpty() ){
            int txtWidth = txtMetrics.width( _legendTexts[ dataset ] ) + 1;
            if( x2 + txtWidth > rightEdge ){
                if( xposHori1 + em2 + txtWidth > rightEdge){
                    xposHori1 = xposHori0;
                    legendNewLinesStartAtLeft = true;
                }
                xpos = xposHori1;
                x2   = xpos + em2;
                ypos += bFirstLFWithTitle ? legendTitleVertGap() : _legendSpacing;
                bFirstLFWithTitle = false;
            }
            maxX = qMax(maxX, x2+txtWidth+_legendEMSpace);
            
            xpos += txtWidth + em4;
            x2   += txtWidth + em4;
        }
    }
    if( bFirstLFWithTitle ) 
        ypos += _legendTitleHeight;
    else 
        ypos += txtMetrics.height();
    
    size.setWidth(  maxX - legendRect.left() );
    size.setHeight( ypos + emDiv2 - _legendRect.top()  );
}


bool KDChartPainter::mustDrawVerticalLegend() const
{
    return
        params()->legendOrientation() == Qt::Vertical ||
        params()->legendPosition() == KDChartParams::LegendLeft ||
        params()->legendPosition() == KDChartParams::LegendRight ||
        params()->legendPosition() == KDChartParams::LegendTopLeft ||
        params()->legendPosition() == KDChartParams::LegendTopLeftLeft ||
        params()->legendPosition() == KDChartParams::LegendTopRight ||
        params()->legendPosition() == KDChartParams::LegendTopRightRight ||
        params()->legendPosition() == KDChartParams::LegendBottomLeft ||
        params()->legendPosition() == KDChartParams::LegendBottomLeftLeft ||
        params()->legendPosition() == KDChartParams::LegendBottomRight ||
        params()->legendPosition() == KDChartParams::LegendBottomRightRight;
}


/**
  Paints the legend for the chart. The implementation in KDChartPainter
  draws a standard legend that should be suitable for most chart
  types. Subclasses can provide their own implementations.

  \param painter the QPainter onto which the chart should be drawn
  \param data the data that will be displayed as a chart
  */
void KDChartPainter::paintLegend( QPainter* painter,
        KDChartTableDataBase* /*data*/ )
{
    if ( params()->legendPosition() == KDChartParams::NoLegend ) 
        return ; // do not draw legend

    const bool bVertical = mustDrawVerticalLegend();
    painter->save();

    
    bool bFrameFound;
    params()->frameSettings( KDChartEnums::AreaLegend, bFrameFound );

    // start out with a rectangle around the legend
    //painter->setPen( QPen( Qt::black, 1 ) );
    //painter->setBrush( QBrush::NoBrush );
    //Pending Michel: let us paint the frame at the end of the drawmarker 
    //and draw text process, in case we need to resize it then    
    /*
    if( !bFrameFound ) {
      painter->drawRect( _legendRect );
    }
    */
    //qDebug("B:  _legendRect:\n          %i,%i\n          %i,%i", _legendRect.left(),_legendRect.top(),_legendRect.right(),_legendRect.bottom() );
    //qDebug("B: legendArea():\n          %i,%i\n          %i,%i\n", _params->legendArea().left(),_params->legendArea().top(),_params->legendArea().right(),_params->legendArea().bottom() );

    const int em2 = 2 * _legendEMSpace;
    const int em4 = 4 * _legendEMSpace;
    const int emDiv2 = static_cast < int > ( _legendEMSpace / 2.0 );

    const int xposHori0 = _legendRect.left() + _legendEMSpace;

    int xpos = xposHori0;

    int ypos = _legendRect.top() + emDiv2;
    
 
	   

    // first paint the title, if any
    if( _legendTitle ) {
        painter->setFont( params()->legendTitleFont() );
        _legendTitle->draw( painter,
                            xpos,
                            ypos,
                            QRegion( xpos,
                                     ypos ,
                                     _legendTitleWidth,
                                     _legendTitleHeight ),
                            params()->legendTitleTextColor() );
        if( bVertical ) 
            ypos += legendTitleVertGap();
	
        else 
            xpos += _legendTitleWidth + em4;
	
    }

    // save the x position: here start the item texts if in horizontal mode
    const int xposHori1 = _legendNewLinesStartAtLeft ? xposHori0 : xpos;

    // add the space of the box plus the space between the box and the text
    int x2 = xpos + em2;

    // loop over all the datasets, each one has one row in the legend
    // if its data are to be used in at least one of the charts drawn
    // *but* only if there is a legend text for it!
    const int rightEdge = _legendRect.right();
    bool bFirstLF = true;
    painter->setFont( trueLegendFont() );
    QFontMetrics txtMetrics( painter->fontMetrics() );
    int dataset;
    for ( dataset = 0; dataset < _numLegendTexts; ++dataset ) {
        /*
           if( KDChartParams::DataEntry == params()->chartSourceMode( dataset ) ) {
           */
        if( !_legendTexts[ dataset ].isEmpty() ){
            int txtWidth = txtMetrics.width( _legendTexts[ dataset ] ) + 1;
  
            // calculate the width and height for the marker, relative to the font height
            // we need the legend text to be aligned to the marker
            // substract a gap. 
	    int legHeight = static_cast <int>((txtMetrics.height() - (int)(txtMetrics.height() * 0.1))*0.85);
	    
	    //int legHeight = static_cast <int> (_legendRect.height()*0.8);
	    
            if( !bVertical && x2 + txtWidth >= rightEdge ){
	      _legendRect.setHeight( _legendRect.height() + _legendSpacing );
                xpos = xposHori1;
                x2   = xpos + em2;
                ypos += bFirstLF ? legendTitleVertGap() : _legendSpacing;
                bFirstLF = false;
            }
            painter->setBrush( QBrush( params()->dataColor( dataset ),
                               Qt::SolidPattern ) );

            if( params()->legendShowLines() ){
                painter->setPen( QPen( params()->dataColor( dataset ), 2,
                                 params()->lineStyle( dataset ) ) );
                painter->drawLine(
                    xpos - emDiv2,
                    ypos + emDiv2 + 1,
                    xpos + static_cast < int > ( _legendEMSpace * 1.5 ),
                    ypos + emDiv2 + 1);
		 }

            /*
            // draw marker if we have a marker, OR we have no marker and no line
            if ( params()->lineMarker() ||
                 params()->lineStyle( dataset ) == Qt::NoPen )*/           
	    drawMarker( painter,
			params(),
			_areaWidthP1000, _areaHeightP1000,
			_dataRect.x(), _dataRect.y(),
			params()->lineMarker()
			? params()->lineMarkerStyle( dataset )
			: KDChartParams::LineMarkerSquare,
			params()->dataColor(dataset),
			QPoint(xpos + emDiv2,
			       bVertical? ypos + emDiv2: !bFirstLF ?ypos + _legendSpacing:_legendRect.center().y() - (legHeight / 2 ))/*ypos + emDiv2*/ ,
			0, 0, 0, NULL,  // these params are deadweight here. TODO
		        &legHeight /*&_legendEMSpace*/, &legHeight /*&_legendEMSpace*/,
			bVertical ? Qt::AlignCenter : (Qt::AlignTop | Qt::AlignHCenter) );  
	    /*	     
	    painter->drawText(_legendRect.topLeft(), "topLeft" );
            painter->drawText(_legendRect.topLeft().x(), _legendRect.center().y(), "center" );
           painter->drawText(_legendRect.bottomLeft(), "bottomLeft" );
	    */
            /* old:
            painter->setPen( Qt::black );
            painter->drawRect( xpos,
                               ypos + ( _legendHeight - _legendEMSpace ) / 2,
                               _legendEMSpace,
                               _legendEMSpace );
            */	       
            painter->setPen( params()->legendTextColor() );
            painter->drawText( x2,
                               bVertical ?  ypos : !bFirstLF ? ypos + _legendSpacing : _legendRect.center().y() - (legHeight / 2 ),
                               txtWidth,
                               legHeight,
                               Qt::AlignLeft | Qt::AlignVCenter,
                               _legendTexts[ dataset ] );

            if( bVertical )
                ypos += _legendSpacing;
            else {
                xpos += txtWidth + em4;
                x2   += txtWidth + em4;
            }
        }
    }

    painter->setPen( QPen( Qt::black, 1 ) );
    painter->setBrush( Qt::NoBrush );
    if( !bFrameFound )
      painter->drawRect( _legendRect );
    

    painter->restore();
}


void adjustFromTo(int& from, int& to)
{
    if( abs(from) > abs(to) ){
        int n = from;
        from = to;
        to = n;
    }
}


bool KDChartPainter::axesOverlapping( int axis1, int axis2 )
{
    KDChartAxisParams::AxisPos basicPos = KDChartAxisParams::basicAxisPos( axis1 );
    if( basicPos != KDChartAxisParams::basicAxisPos( axis2 ) )
        // Only axes of the same position can be compared. (e.g. 2 left axes)
        return false;

    if( KDChartAxisParams::AxisPosLeft  != basicPos &&
            KDChartAxisParams::AxisPosRight != basicPos )
        // Available space usage only possible for (vertical) ordinate axes.
        return false;

    int f1 = params()->axisParams( axis1 ).axisUseAvailableSpaceFrom();
    int t1 = params()->axisParams( axis1 ).axisUseAvailableSpaceTo();
    int f2 = params()->axisParams( axis2 ).axisUseAvailableSpaceFrom();
    int t2 = params()->axisParams( axis2 ).axisUseAvailableSpaceTo();
    adjustFromTo(f1,t1);
    adjustFromTo(f2,t2);
    // give these values some meaning
    // to be able to compare mixed fixed and/or relative figures:
    const double guessedAxisHeightP1000 = _areaHeightP1000 * 80.0 / 100.0;
    if(f1 < 0) f1 = static_cast < int > ( f1 * -guessedAxisHeightP1000 );
    if(t1 < 0) t1 = static_cast < int > ( t1 * -guessedAxisHeightP1000 );
    if(f2 < 0) f2 = static_cast < int > ( f2 * -guessedAxisHeightP1000 );
    if(t2 < 0) t2 = static_cast < int > ( t2 * -guessedAxisHeightP1000 );
    const bool res = (f1 >= f2 && f1 < t2) || (f2 >= f1 && f2 < t1);
    return res;
}


void internSetAxisArea( KDChartParams* params, int axis,
        int x0, int y0, int w0, int h0 )
{
    // axis may never occupy more than 1000 per mille of the available space
    int nFrom = qMax(-1000, params->axisParams( axis ).axisUseAvailableSpaceFrom());
    int nTo   = qMax(-1000, params->axisParams( axis ).axisUseAvailableSpaceTo());
    adjustFromTo(nFrom,nTo);

    KDChartAxisParams::AxisPos basicPos = KDChartAxisParams::basicAxisPos( axis );
    int x, y, w, h;
    if( KDChartAxisParams::AxisPosBottom == basicPos ||
            KDChartAxisParams::AxisPosTop    == basicPos ){

        // Note: available space usage is ignored for abscissa axes!
        //
        //if( nFrom < 0 )
        //  x = x0 + w0*nFrom/-1000;
        //else
        //  x = x0 +    nFrom;
        //y = y0;
        //if( nTo < 0 )
        //  w = x0 + w0*nTo/-1000 - x;
        //else
        //  w = x0 +    nTo       - x;
        //h = h0;

        x = x0;
        y = y0;
        w = w0;
        h = h0;

    }else{
        x = x0;
        if( nTo < 0 )
            y = y0 + h0 - h0*nTo/-1000;
        else
            y = y0 + h0 -    nTo;
        w = w0;
        if( nFrom < 0 )
            h = y0 + h0 - h0*nFrom/-1000 - y;
        else
            h = y0 + h0 -    nFrom       - y;
    }

    params->setAxisArea( axis,
            QRect( x,
                y,
                w,
                h ) );
}


/**
  Paints the header and footers for the chart. The implementation
  in KDChartPainter draws a standard header that should be suitable
  for most chart types. Subclasses can provide their own implementations.

  \param painter the QPainter onto which the chart should be drawn
  \param data the data that will be displayed as a chart
  */
void KDChartPainter::paintHeaderFooter( QPainter* painter,
        KDChartTableDataBase* /*data*/ )
{
    const double averageValueP1000 = qMin(_areaWidthP1000, _areaHeightP1000);//( _areaWidthP1000 + _areaHeightP1000 ) / 2.0;

    painter->save();

    for( int iHdFt  = KDChartParams::HdFtPosSTART;
            iHdFt <= KDChartParams::HdFtPosEND;  ++iHdFt ){
        QString txt( params()->headerFooterText( iHdFt ) );
        if ( !txt.isEmpty() ) {
            QFont actFont( params()->headerFooterFont( iHdFt ) );
            if ( params()->headerFooterFontUseRelSize( iHdFt ) )
                actFont.setPixelSize( static_cast < int > (
                    params()->headerFooterFontRelSize( iHdFt ) * averageValueP1000 ) );
            painter->setPen( params()->headerFooterColor( iHdFt ) );
            painter->setFont( actFont );
            // Note: The alignment flags used here match the rect calculation
            //       done in KDChartPainter::setupGeometry().
            //       AlignTop is done to ensure that the hd/ft texts of the same
            //       group (e.g. Hd2L and Hd2 and Hd2R) have the same baselines.

            QRect rect( params()->headerFooterRect( iHdFt ) );
            int dXY = iHdFt < KDChartParams::HdFtPosFootersSTART
                ? _hdLeading/3
                : _ftLeading/3;
            rect.moveBy(dXY, dXY);
            rect.setWidth(  rect.width() -2*dXY +1 );
            rect.setHeight( rect.height()-2*dXY +1 );
            painter->drawText( rect,
                    Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine,
                    txt );
        }
    }
    painter->restore();
}


int KDChartPainter::calculateHdFtRects(
        QPainter* painter,
        double averageValueP1000,
        int  xposLeft,
        int  xposRight,
        bool bHeader,
        int& yposTop,
        int& yposBottom )
{
    int& leading = (bHeader ? _hdLeading : _ftLeading);
    leading = 0; // pixels between the header (or footer, resp.) text
    // and the border of the respective Hd/Ft area

    const int rangesCnt = 3;
    const int ranges[ rangesCnt ]
        = { bHeader ? KDChartParams::HdFtPosHeaders0START : KDChartParams::HdFtPosFooters0START,
            bHeader ? KDChartParams::HdFtPosHeaders1START : KDChartParams::HdFtPosFooters1START,
            bHeader ? KDChartParams::HdFtPosHeaders2START : KDChartParams::HdFtPosFooters2START };
    const int rangeSize = 3;
    QFontMetrics* metrics[rangesCnt * rangeSize];

    int i;
    for( i = 0; i < rangesCnt*rangeSize; ++i )
        metrics[ i ] = 0;

    int iRange;
    int iHdFt;
    for( iRange = 0; iRange < rangesCnt; ++iRange ){
        for( i = 0; i < rangeSize; ++i ){
            iHdFt = ranges[iRange] + i;
            QString txt( params()->headerFooterText( iHdFt ) );
            if ( !txt.isEmpty() ) {
                QFont actFont( params()->headerFooterFont( iHdFt ) );
                if ( params()->headerFooterFontUseRelSize( iHdFt ) ) {
                    actFont.setPixelSize( static_cast < int > (
                            params()->headerFooterFontRelSize( iHdFt ) * averageValueP1000 ) );
                }
                painter->setFont( actFont );
                metrics[ iRange*rangeSize + i ] = new QFontMetrics( painter->fontMetrics() );
                leading = qMax( leading, metrics[ iRange*rangeSize + i ]->lineSpacing() / 2 );
            }
        }
    }

    if( bHeader )
        ++yposTop;//yposTop += leading/3;
    //else
    //--yposBottom;//yposBottom -= leading/3;

    int leading23 = leading*2/3 +1;

    for( iRange =
            bHeader ? 0                  : rangesCnt-1;
            bHeader ? iRange < rangesCnt : iRange >= 0;
            bHeader ? ++iRange           : --iRange ){
        // Ascents and heights must be looked at to ensure that the hd/ft texts
        // of the same group (e.g. Hd2L and Hd2 and Hd2R) have equal baselines.
        int ascents[rangeSize];
        int heights[rangeSize];
        int widths[ rangeSize];
        int maxAscent = 0;
        int maxHeight = 0;
        for( i = 0; i < rangeSize; ++i ){
            iHdFt = ranges[iRange] + i;
            if ( metrics[ iRange*rangeSize + i ] ) {
                QFontMetrics& m = *metrics[ iRange*rangeSize + i ];
                ascents[i] = m.ascent();
                heights[i] = m.height() + leading23;

                // the following adds two spaces to work around a bug in Qt:
                // bounding rect sometimes is too small, if using italicized fonts
                widths[ i] = m.boundingRect( params()->headerFooterText( iHdFt )+"  " ).width() + leading23;

                maxAscent = qMax( maxAscent, ascents[i] );
                maxHeight = qMax( maxHeight, heights[i] );
            }else{
                heights[i] = 0;
            }
        }

        if( !bHeader )
            yposBottom -= maxHeight;

        for( i = 0; i < rangeSize; ++i ){
            if( heights[i] ){
                iHdFt = ranges[iRange] + i;
                int x1;
                switch( i ){
                    case 1:  x1 = xposLeft+1;
                            break;
                    case 2:  x1 = xposRight-widths[i]-1;
                            break;
                    default: x1 = xposLeft + (xposRight-xposLeft-widths[i]) / 2;
                }
                ((KDChartParams*)params())->__internalStoreHdFtRect( iHdFt,
                                                                    QRect( x1,
                                                                        bHeader
                                                                        ? yposTop    + maxAscent - ascents[i]
                                                                        : yposBottom + maxAscent - ascents[i],
                                                                        widths[ i],
                                                                        heights[i] - 1 ) );
            }
        }
        if( bHeader )
            yposTop    += leading + maxHeight;
        else
            yposBottom -= leading;
    }
    for( i = 0; i < rangesCnt*rangeSize; ++i )
        if( metrics[ i ] )
            delete metrics[ i ];
    return leading;
}



void KDChartPainter::findChartDatasets( KDChartTableDataBase* data,
                                        bool paint2nd,
                                        uint chart,
                                        uint& chartDatasetStart,
                                        uint& chartDatasetEnd )
{
    if(    params()->neverUsedSetChartSourceMode()
        || !params()->findDatasets( KDChartParams::DataEntry,
                                    KDChartParams::ExtraLinesAnchor,
                                    chartDatasetStart,
                                    chartDatasetEnd,
                                    chart ) ) {
        uint maxRow, maxRowMinus1;
        switch ( data->usedRows() ) {
            case 0:
                return ;
            case 1:
                maxRow = 0;
                maxRowMinus1 = 0;
                break;
            default:
                maxRow = data->usedRows() - 1;
                maxRowMinus1 = maxRow;
        }
        chartDatasetStart = paint2nd ? maxRow : 0;
        chartDatasetEnd = paint2nd
                        ? maxRow
                        : (   ( KDChartParams::NoType == params()->additionalChartType() )
                            ? maxRow
                            : maxRowMinus1 );

    }
}


void KDChartPainter::calculateAllAxesRects(
        QPainter* painter,
        bool finalPrecision,
        KDChartTableDataBase* data
        )
{
    const bool bIsAreaChart = KDChartParams::Area == params()->chartType();
    const bool bMultiRows = KDChartParams::Bar == params()->chartType() &&
        KDChartParams::BarMultiRows == params()->barChartSubType();

    const int trueWidth  = _outermostRect.width();
    const int trueHeight = _outermostRect.height();
    const double averageValueP1000 = qMin(_areaWidthP1000, _areaHeightP1000);//( _areaWidthP1000 + _areaHeightP1000 ) / 2.0;

    // store the axes' 0 offsets
    int nAxesLeft0   = _axesRect.left() - _outermostRect.left();
    int nAxesRight0  = _outermostRect.right() - _axesRect.right();
    int nAxesTop0    = _axesRect.top() - _outermostRect.top();
    int nAxesBottom0 = _outermostRect.bottom() - _axesRect.bottom();
    if( bMultiRows ){
        uint chartDatasetStart, chartDatasetEnd;
        findChartDatasets( data, false, 0, chartDatasetStart, chartDatasetEnd );
        const int datasets = chartDatasetEnd - chartDatasetStart + 1;
        int numValues = 0;
        if ( params()->numValues() != -1 )
            numValues = params()->numValues();
        else
            numValues = data->usedCols();
        if( datasets ){
            const int additionalGapWidth = static_cast < int > ( 1.0 * _axesRect.width() / (9.75*numValues + 4.0*datasets) * 4.0*datasets );
            nAxesRight0 += additionalGapWidth;
            nAxesTop0   += static_cast < int > ( additionalGapWidth * 0.52 );
            //const double widthFactor = additionalGapWidth*1.0 / _axesRect.width();
            //nAxesTop0   += static_cast < int > ( _axesRect.height() * widthFactor );
        }
    }
    // store the distances to be added to the axes' 0 offsets
    int nAxesLeftADD  =0;
    int nAxesRightADD =0;
    int nAxesTopADD   =0;
    int nAxesBottomADD=0;

    // determine whether the axes widths of one side should be added
    // or their maximum should be used
    bool bAddLeft = axesOverlapping( KDChartAxisParams::AxisPosLeft,
            KDChartAxisParams::AxisPosLeft2 );
    bool bAddRight = axesOverlapping( KDChartAxisParams::AxisPosRight,
            KDChartAxisParams::AxisPosRight2 );
    bool bAddTop = axesOverlapping( KDChartAxisParams::AxisPosTop,
            KDChartAxisParams::AxisPosTop2 );
    bool bAddBottom = axesOverlapping( KDChartAxisParams::AxisPosBottom,
            KDChartAxisParams::AxisPosBottom2 );
    // iterate over all axes
    uint iAxis;
    for ( iAxis = 0; iAxis < KDCHART_MAX_AXES; ++iAxis ) {

        const KDChartAxisParams& para = params()->axisParams( iAxis );
        int areaSize = 0;

        if ( para.axisVisible()
                && KDChartAxisParams::AxisTypeUnknown != para.axisType() ) {

            const KDChartAxisParams::AxisPos
                basicPos( KDChartAxisParams::basicAxisPos( iAxis ) );

            int areaMin = para.axisAreaMin();
            int areaMax = para.axisAreaMax();
            if ( 0 > areaMin )
                areaMin = static_cast < int > ( -1.0 * averageValueP1000 * areaMin );
            if ( 0 > areaMax )
                areaMax = static_cast < int > ( -1.0 * averageValueP1000 * areaMax );

            // make sure areaMin will not be too small
            // for the label texts
            switch ( basicPos ) {
                case KDChartAxisParams::AxisPosBottom:
                case KDChartAxisParams::AxisPosTop:
                    if ( para.axisLabelsVisible() ) {
                        int fntHeight;
                        if ( para.axisLabelsFontUseRelSize() )
                            fntHeight = static_cast < int > (
                                    para.axisLabelsFontRelSize()
                                    * averageValueP1000 );
                        else {
                            painter->setFont( para.axisLabelsFont() );
                            QFontMetrics metrics( painter->fontMetrics() );
                            fntHeight = metrics.height();
                        }
                        // adjust text height in case of formatted Date/Time values
                        uint dataDataset, dataDataset2;
                        if( !params()->findDataset( KDChartParams::DataEntry,
                                                    dataDataset,
                                                    dataDataset2,
                                                    KDCHART_ALL_CHARTS ) ) {
                            qDebug( "IMPLEMENTATION ERROR: findDataset( DataEntry, ... ) should *always* return true. (a)" );
                            dataDataset = KDCHART_ALL_DATASETS;
                        }
                        QVariant::Type valType = QVariant::Invalid;
                        const bool dataCellsHaveSeveralCoordinates =
                            (KDCHART_ALL_DATASETS == dataDataset)
                            ? data->cellsHaveSeveralCoordinates( &valType )
                            : data->cellsHaveSeveralCoordinates( dataDataset, dataDataset2, &valType );
                        QString format( para.axisLabelsDateTimeFormat() );
                        if(    dataCellsHaveSeveralCoordinates
                            && QVariant::DateTime == valType ){
                            if( KDCHART_AXIS_LABELS_AUTO_DATETIME_FORMAT == format )
                                areaMin = qMax( areaMin, static_cast < int > ( fntHeight * 6.75 ) );
                            else
                                areaMin = qMax( areaMin, fntHeight * ( 3 + format.count("\n") ) );
                        }
                        else
                            areaMin = qMax( areaMin, fntHeight * 3 );
                    }
                    break;
                case KDChartAxisParams::AxisPosLeft:
                case KDChartAxisParams::AxisPosRight:
                default:
                    break;
            }


            switch ( para.axisAreaMode() ) {
                case KDChartAxisParams::AxisAreaModeAutoSize:
                {
                    areaSize = areaMin;
                    switch ( basicPos ) {
                        case KDChartAxisParams::AxisPosBottom:
                        case KDChartAxisParams::AxisPosTop:
                            break;
                        case KDChartAxisParams::AxisPosLeft:
                        case KDChartAxisParams::AxisPosRight:
                            if( finalPrecision ){
                                internal__KDChart__CalcValues& cv = calcVal[iAxis];
                                const int nUsableAxisWidth = static_cast < int > (cv.pTextsW);
                                const KDChartAxisParams & para = params()->axisParams( iAxis );
                                QFont axisLabelsFont( para.axisLabelsFont() );
                                if ( para.axisLabelsFontUseRelSize() ) {
                                    axisLabelsFont.setPixelSize( static_cast < int > ( cv.nTxtHeight ) );
                                }
                                painter->setFont( para.axisLabelsFont() );
                                QFontMetrics axisLabelsFontMetrics( painter->fontMetrics() );
                                const int lenEM( axisLabelsFontMetrics.boundingRect("M").width() );
                                const QStringList* labelTexts = para.axisLabelTexts();
                                uint nLabels = ( 0 != labelTexts )
                                            ? labelTexts->count()
                                            : 0;
                                int maxLabelsWidth = 0;
                                for ( uint i = 0; i < nLabels; ++i )
                                    maxLabelsWidth =
                                        qMax( maxLabelsWidth,
                                              axisLabelsFontMetrics.boundingRect(labelTexts->at(i)).width() );
                                if( nUsableAxisWidth < maxLabelsWidth )
                                    areaSize = maxLabelsWidth
                                             + (para.axisTrueAreaRect().width() - nUsableAxisWidth)
                                             + lenEM;
                            }
                            break;
                        default:
                            break;
                    }
                }
                break;
                case KDChartAxisParams::AxisAreaModeMinMaxSize:
                {
                    qDebug( "Sorry, not implemented: AxisAreaModeMinMaxSize" );
                }

                //
                //
                //   F E A T U R E   P L A N N E D   F O R   F U T U R E . . .
                //
                //

                // break;

                case KDChartAxisParams::AxisAreaModeFixedSize:
                {
                    areaSize = areaMax ? qMin( areaMin, areaMax ) : areaMin;
                }
                break;
            }

            switch ( basicPos ) {
                case KDChartAxisParams::AxisPosBottom:
                    if( bAddBottom )
                        nAxesBottomADD += areaSize;
                    else
                        nAxesBottomADD = qMax( nAxesBottomADD, areaSize );
                    break;
                case KDChartAxisParams::AxisPosLeft:
                    if( bAddLeft )
                        nAxesLeftADD += areaSize;
                    else
                        nAxesLeftADD = qMax( nAxesLeftADD, areaSize );
                    break;
                case KDChartAxisParams::AxisPosTop:
                    if( bAddTop )
                        nAxesTopADD += areaSize;
                    else
                        nAxesTopADD = qMax( nAxesTopADD, areaSize );
                    break;
                case KDChartAxisParams::AxisPosRight:
                    if( bAddRight )
                        nAxesRightADD += areaSize;
                    else
                        nAxesRightADD = qMax( nAxesRightADD, areaSize );
                    break;
                default:
                    break;
            }
        }
        // Note: to prevent users from erroneously calling this
        //       function we do *not* provide a wrapper for it
        //       in the KDChartParams class but rather call it
        //       *directly* using a dirty typecast.
        ( ( KDChartAxisParams& ) para ).setAxisTrueAreaSize( areaSize );
    }
    int nMinDistance = static_cast < int > ( 30.0 * averageValueP1000 );

    int nAxesBottom = qMax( nAxesBottom0 + nAxesBottomADD, nMinDistance );

    // for micro alignment with the X axis, we adjust the Y axis - but not for Area Charts:
    // otherwise the areas drawn would overwrite the Y axis line.
    int nAxesLeft   = qMax( nAxesLeft0   + nAxesLeftADD,   nMinDistance )
                      - (bIsAreaChart ? 0 : 1);

    int nAxesTop    = qMax( nAxesTop0    + nAxesTopADD,    nMinDistance );

    int nAxesRight  = qMax( nAxesRight0  + nAxesRightADD,  nMinDistance );

    int nBottom  = params()->axisParams( KDChartAxisParams::AxisPosBottom ).axisTrueAreaSize();
    int nLeft    = params()->axisParams( KDChartAxisParams::AxisPosLeft ).axisTrueAreaSize();
    int nTop     = params()->axisParams( KDChartAxisParams::AxisPosTop ).axisTrueAreaSize();
    int nRight   = params()->axisParams( KDChartAxisParams::AxisPosRight ).axisTrueAreaSize();
    int nBottom2 = params()->axisParams( KDChartAxisParams::AxisPosBottom2 ).axisTrueAreaSize();
    int nLeft2   = params()->axisParams( KDChartAxisParams::AxisPosLeft2 ).axisTrueAreaSize();
    int nTop2    = params()->axisParams( KDChartAxisParams::AxisPosTop2 ).axisTrueAreaSize();
    int nRight2  = params()->axisParams( KDChartAxisParams::AxisPosRight2 ).axisTrueAreaSize();

    internSetAxisArea( _params,
            KDChartAxisParams::AxisPosBottom,
            _outermostRect.left() + nAxesLeft,
            _outermostRect.top()  + trueHeight - nAxesBottom,
            trueWidth - nAxesLeft - nAxesRight + 1,
            nBottom );
    internSetAxisArea( _params,
            KDChartAxisParams::AxisPosLeft,
            _outermostRect.left() + (bAddLeft ? nAxesLeft0 + nLeft2 : nAxesLeft0),
            _outermostRect.top()  + nAxesTop,
            nLeft,
            trueHeight - nAxesTop - nAxesBottom + 1 );

    internSetAxisArea( _params,
            KDChartAxisParams::AxisPosTop,
            _outermostRect.left() + nAxesLeft,
            _outermostRect.top()  + (bAddTop ? nAxesTop0 + nTop2 : nAxesTop0),
            trueWidth - nAxesLeft - nAxesRight + 1,
            nTop );
    internSetAxisArea( _params,
            KDChartAxisParams::AxisPosRight,
            _outermostRect.left() + trueWidth - nAxesRight,
            _outermostRect.top()  + nAxesTop,
            nRight,
            trueHeight - nAxesTop - nAxesBottom + 1 );

    internSetAxisArea( _params,
            KDChartAxisParams::AxisPosBottom2,
            _outermostRect.left() + nAxesLeft,
            _outermostRect.top()  + trueHeight - nAxesBottom + (bAddBottom ? nBottom : 0),
            trueWidth - nAxesLeft - nAxesRight + 1,
            nBottom2 );
    internSetAxisArea( _params,
            KDChartAxisParams::AxisPosLeft2,
            _outermostRect.left() + nAxesLeft0,
            _outermostRect.top()  + nAxesTop,
            nLeft2,
            trueHeight - nAxesTop - nAxesBottom + 1 );

    internSetAxisArea( _params,
            KDChartAxisParams::AxisPosTop2,
            _outermostRect.left() + nAxesLeft,
            _outermostRect.top()  + nAxesTop0,
            trueWidth - nAxesLeft - nAxesRight + 1,
            nTop2 );
    internSetAxisArea( _params,
            KDChartAxisParams::AxisPosRight2,
            _outermostRect.left() + trueWidth - nAxesRight + (bAddRight ? nRight : 0),
            _outermostRect.top()  + nAxesTop,
            nRight2,
            trueHeight - nAxesTop - nAxesBottom + 1 );

    _dataRect = QRect( _outermostRect.left() + nAxesLeft,
                       _outermostRect.top()  + nAxesTop,
                       trueWidth - nAxesLeft - nAxesRight + 1,
                       trueHeight - nAxesTop - nAxesBottom + 1 );
}



/**
  This method will be called whenever any parameters that affect
  geometry have been changed. It will compute the appropriate
  positions for the various parts of the chart (legend, axes, data
  area etc.). The implementation in KDChartPainter computes a
  standard geometry that should be suitable for most chart
  types. Subclasses can provide their own implementations.

  \param data the data that will be displayed as a chart
  \param drawRect the position and size of the area where the chart
  is to be displayed in
  */
void KDChartPainter::setupGeometry( QPainter* painter,
                                    KDChartTableDataBase* data,
                                    const QRect& drawRect )
{
  //qDebug("INVOKING: KDChartPainter::setupGeometry()");
    // avoid recursion from repaint() being called due to params() changed signals...
    const bool oldBlockSignalsState = params()->signalsBlocked();
    const_cast < KDChartParams* > ( params() )->blockSignals( true );

    _outermostRect = drawRect;

    int yposTop    = _outermostRect.topLeft().y();
    int xposLeft   = _outermostRect.topLeft().x();
    int yposBottom = _outermostRect.bottomRight().y();
    int xposRight  = _outermostRect.bottomRight().x();

    const int trueWidth  = _outermostRect.width();
    const int trueHeight = _outermostRect.height();

    // Temporary values used to calculate start values xposLeft, yposTop, xposRight, yposBottom.
    // They will be replaced immediately after these calculations.
    _areaWidthP1000    = trueWidth / 1000.0;
    _areaHeightP1000   = trueHeight / 1000.0;


    xposLeft   +=   0 < params()->globalLeadingLeft()
        ? params()->globalLeadingLeft()
        : static_cast < int > ( params()->globalLeadingLeft()  * -_areaWidthP1000 );
    yposTop    +=   0 < params()->globalLeadingTop()
        ? params()->globalLeadingTop()
        : static_cast < int > ( params()->globalLeadingTop()   * -_areaHeightP1000 );
    xposRight  -=   0 < params()->globalLeadingRight()
        ? params()->globalLeadingRight()
        : static_cast < int > ( params()->globalLeadingRight() * -_areaWidthP1000 );
    yposBottom -=   0 < params()->globalLeadingBottom()
        ? params()->globalLeadingBottom()
        : static_cast < int > ( params()->globalLeadingBottom()* -_areaHeightP1000 );

    _innermostRect = QRect( QPoint(xposLeft,  yposTop),
                            QPoint(xposRight, yposBottom) );

    _logicalWidth  = xposRight  - xposLeft;
    _logicalHeight = yposBottom - yposTop;

    // true values (having taken the global leadings into account)
    // to be used by all following functions
    _areaWidthP1000 =  _logicalWidth  / 1000.0;
    _areaHeightP1000 = _logicalHeight / 1000.0;

    double averageValueP1000 = qMin(_areaWidthP1000, _areaHeightP1000);//( _areaWidthP1000 + _areaHeightP1000 ) / 2.0;

    // new code design:
    //        1. now min-header-leading is text height/2
    //        2. leading or legendSpacing (whichever is larger)
    //           will be added if legend is below the header(s)
    //        3. leading will be added between header and data area
    //           in case there is no top legend but grid is to be shown.
    int headerLineLeading = calculateHdFtRects(
            painter,
            averageValueP1000,
            xposLeft, xposRight,
            false,
            yposTop, yposBottom );
    calculateHdFtRects(
            painter,
            averageValueP1000,
            xposLeft, xposRight,
            true,
            yposTop, yposBottom );

    // Calculate legend position. First check whether there is going
    // to be a legend at all:
    if ( params()->legendPosition() != KDChartParams::NoLegend ) {
        // Now calculate the size needed for the legend
        findLegendTexts( data );

        bool hasLegendTitle = false;
        if ( !params()->legendTitleText().isEmpty() )
            hasLegendTitle = true;

        _legendTitleWidth = 0;
        if( _legendTitle )
            delete _legendTitle;
        _legendTitle = 0;
        if ( hasLegendTitle ) {
            QFont actLegendTitleFont = params()->legendTitleFont();
            if ( params()->legendTitleFontUseRelSize() ) {
                int nTxtHeight =
                    static_cast < int > ( params()->legendTitleFontRelSize()
                                            * averageValueP1000 );
                actLegendTitleFont.setPixelSize( nTxtHeight );
                // qDebug("l-t-height %i",nTxtHeight);
                const_cast < KDChartParams* > ( params() )->setLegendTitleFont( actLegendTitleFont, false );
            }
            painter->setFont( actLegendTitleFont );
            QFontMetrics legendTitleMetrics( painter->fontMetrics() );
            _legendTitleMetricsHeight = legendTitleMetrics.height();

            _legendTitle = new KDChartTextPiece( painter,
                                                 params()->legendTitleText(),
                                                 actLegendTitleFont );
            _legendTitleWidth = _legendTitle->width();
            _legendTitleHeight = _legendTitle->height();
            // qDebug("1. _legendTitleHeight %i",_legendTitleHeight);
        }

        painter->setFont( trueLegendFont() );
        QFontMetrics legendMetrics( painter->fontMetrics() );
        _legendSpacing = legendMetrics.lineSpacing();
        _legendHeight = legendMetrics.height();
        _legendLeading = legendMetrics.leading();

        _legendEMSpace = legendMetrics.width( 'M' );

        int sizeX = 0;
        int sizeY = 0;

        for ( int dataset = 0; dataset < _numLegendTexts; dataset++ ) {
            sizeX = qMax( sizeX, legendMetrics.width( _legendTexts[ dataset ] ) );
            if( !_legendTexts[ dataset ].isEmpty() )
                sizeY += _legendSpacing;
        }
        // add space below the legend's bottom line
        sizeY += _legendEMSpace - _legendLeading;
        // add space for the legend title if any was set
        if ( hasLegendTitle )
            sizeY += legendTitleVertGap();

        // assume 4 em spaces: before the color box, the color box, after the
        // color box and after the legend text
        sizeX += ( _legendEMSpace * 4 );

        // We cannot setup the title width earlier as the title does
        // not have a color box. The two em spaces are before the
        // color box (where the title does not start yet, it is
        // left-aligned with the color boxes) and after the title (to
        // have some space before the boundary line comes).
        sizeX = qMax( sizeX, _legendTitleWidth + _legendEMSpace*2 );

	//qDebug("setupGeometry  mustDrawVerticalLegend: %s", mustDrawVerticalLegend() ? "YES":"NO ");

	//     PENDING Michel: do that after having calculated the position
        if( !mustDrawVerticalLegend() ){	  
            QSize size;
            calculateHorizontalLegendSize( painter,
                                           size,
                                           _legendNewLinesStartAtLeft );
            sizeX = size.width();
            sizeY = size.height();
        }

        switch ( params()->legendPosition() ) {
            case KDChartParams::LegendTop:
                if ( headerLineLeading )
                    yposTop += qMax( (int)params()->legendSpacing(), headerLineLeading );
                _legendRect = QRect( xposLeft + ( (xposRight-xposLeft) - sizeX ) / 2,
                                     yposTop, sizeX, sizeY );
                yposTop = _legendRect.bottom() + params()->legendSpacing();
                //qDebug("A:  _legendRect:\n%i,%i\n%i,%i", _legendRect.left(),_legendRect.top(),_legendRect.right(),_legendRect.bottom() ); 
                break;
            case KDChartParams::LegendBottom:
                if ( params()->showGrid() )
                    yposTop += headerLineLeading;
                _legendRect = QRect( xposLeft + ( (xposRight-xposLeft) - sizeX ) / 2,
                                     yposBottom - sizeY,
                                     sizeX, sizeY );
                yposBottom = _legendRect.top() - params()->legendSpacing();
                break;
            case KDChartParams::LegendLeft:
                if ( params()->showGrid() )
                    yposTop += headerLineLeading;
                _legendRect = QRect( xposLeft + 1, ( yposBottom - yposTop - sizeY ) / 2 +
                                     yposTop,
                                     sizeX, sizeY );
                xposLeft = _legendRect.right() + params()->legendSpacing();
                break;
            case KDChartParams::LegendRight:
                if ( params()->showGrid() )
                    yposTop += headerLineLeading;
                _legendRect = QRect( xposRight - sizeX - 1,
                        ( yposBottom - yposTop - sizeY ) / 2 + yposTop,
                        sizeX, sizeY );
                xposRight = _legendRect.left() - params()->legendSpacing();
                break;
            case KDChartParams::LegendTopLeft:
                if ( headerLineLeading )
                    yposTop += qMax( (int)params()->legendSpacing(), headerLineLeading );
                _legendRect = QRect( xposLeft + 1, yposTop, sizeX, sizeY );
                yposTop = _legendRect.bottom() + params()->legendSpacing();
                xposLeft = _legendRect.right() + params()->legendSpacing();
                break;
            case KDChartParams::LegendTopLeftTop:
                if ( headerLineLeading )
                    yposTop += qMax( (int)params()->legendSpacing(), headerLineLeading );
                _legendRect = QRect( xposLeft + 1, yposTop, sizeX, sizeY );
                yposTop = _legendRect.bottom() + params()->legendSpacing();
                break;
            case KDChartParams::LegendTopLeftLeft:
                if ( headerLineLeading )
                    yposTop += qMax( (int)params()->legendSpacing(), headerLineLeading );
                _legendRect = QRect( xposLeft + 1, yposTop, sizeX, sizeY );
                xposLeft = _legendRect.right() + params()->legendSpacing();
                break;
            case KDChartParams::LegendTopRight:
                if ( headerLineLeading )
                    yposTop += qMax( (int)params()->legendSpacing(), headerLineLeading );
                _legendRect = QRect( xposRight - sizeX - 1,
                        yposTop, sizeX, sizeY );
                yposTop = _legendRect.bottom() + params()->legendSpacing();
                xposRight = _legendRect.left() - params()->legendSpacing();
                break;
            case KDChartParams::LegendTopRightTop:
                if ( headerLineLeading )
                    yposTop += qMax( (int)params()->legendSpacing(), headerLineLeading );
                _legendRect = QRect( xposRight - sizeX - 1,
                        yposTop, sizeX, sizeY );
                yposTop = _legendRect.bottom() + params()->legendSpacing();
                break;
            case KDChartParams::LegendTopRightRight:
                if ( headerLineLeading )
                    yposTop += qMax( (int)params()->legendSpacing(), headerLineLeading );
                _legendRect = QRect( xposRight - sizeX - 1,
                        yposTop, sizeX, sizeY );
                xposRight = _legendRect.left() - params()->legendSpacing();
                break;
            case KDChartParams::LegendBottomLeft:
                if ( params()->showGrid() )
                    yposTop += headerLineLeading;
                _legendRect = QRect( xposLeft + 1, yposBottom - sizeY, sizeX, sizeY );
                yposBottom = _legendRect.top() - params()->legendSpacing();
                xposLeft = _legendRect.right() + params()->legendSpacing();
                break;
            case KDChartParams::LegendBottomLeftBottom:
                if ( params()->showGrid() )
                    yposTop += headerLineLeading;
                _legendRect = QRect( xposLeft + 1, yposBottom - sizeY, sizeX, sizeY );
                yposBottom = _legendRect.top() - params()->legendSpacing();
                break;
            case KDChartParams::LegendBottomLeftLeft:
                if ( params()->showGrid() )
                    yposTop += headerLineLeading;
                _legendRect = QRect( xposLeft + 1, yposBottom - sizeY, sizeX, sizeY );
                xposLeft = _legendRect.right() + params()->legendSpacing();
                break;
            case KDChartParams::LegendBottomRight:
                if ( params()->showGrid() )
                    yposTop += headerLineLeading;
                _legendRect = QRect( xposRight - sizeX - 1,
                        yposBottom - sizeY, sizeX, sizeY );
                yposBottom = _legendRect.top() - params()->legendSpacing();
                xposRight = _legendRect.left() - params()->legendSpacing();
                break;
            case KDChartParams::LegendBottomRightBottom:
                if ( params()->showGrid() )
                    yposTop += headerLineLeading;
                _legendRect = QRect( xposRight - sizeX - 1,
                        yposBottom - sizeY, sizeX, sizeY );
                yposBottom = _legendRect.top() - params()->legendSpacing();
                break;
            case KDChartParams::LegendBottomRightRight:
                if ( params()->showGrid() )
                    yposTop += headerLineLeading;
                _legendRect = QRect( xposRight - sizeX - 1,
                        yposBottom - sizeY, sizeX, sizeY );
                xposRight = _legendRect.left() - params()->legendSpacing();
                break;
            default:
                // Should not be able to happen
                qDebug( "KDChart: Unknown legend position" );
        }
        _params->setLegendArea( _legendRect );

    }else{
      _params->setLegendArea( QRect(QPoint(0,0), QSize(0,0)) );
    }
     

    _axesRect = QRect( QPoint(xposLeft, yposTop), QPoint(xposRight, yposBottom) );

    // important rule: do *not* calculate axes areas for Polar charts!
    //                 (even if left and bottom axes might be set active)
    if( KDChartParams::Polar == params()->chartType() ) {
        _dataRect = _axesRect;
    } else {
        // 1st step: make a preliminary approximation of the axes sizes,
        //           as a basis of following label texts calculation
        calculateAllAxesRects( painter, false, data );
        // 2nd step: calculate all labels (preliminary data, will be
        //           overwritten by KDChartAxesPainter)
        //           to find out the longest possible axis labels
        double dblDummy;
        if( calculateAllAxesLabelTextsAndCalcValues(
                painter,
                data,
                _areaWidthP1000,
                _areaHeightP1000,
                dblDummy ) )
            // 3rd step: calculate the _true_ axes rects based upon
            //           the preliminary axes labels
            calculateAllAxesRects( painter, true, data );
    }
    _params->setDataArea( _dataRect );

    const_cast < KDChartParams* > ( params() )->blockSignals( oldBlockSignalsState );
}


/**
  This method implements the algorithm to find the texts for the legend.
  */
void KDChartPainter::findLegendTexts( KDChartTableDataBase* data )
{
    uint dataset;
    QVariant vValY;
    switch ( params()->legendSource() ) {
        case KDChartParams::LegendManual: {
            // The easiest case: Take manually set strings, no matter whether any
            // have been set.
            _numLegendTexts = numLegendFallbackTexts( data );
            for ( dataset = 0; dataset < static_cast<uint>(_numLegendTexts); dataset++ )
                _legendTexts[ dataset ] = params()->legendText( dataset );
            break;
        }
        case KDChartParams::LegendFirstColumn: {
            // Take whatever is in the first column
            for ( dataset = 0; dataset < data->usedRows(); dataset++ ){
                if( data->cellCoord( dataset, 0, vValY, 1 ) ){
                    if( QVariant::String == vValY.type() )
                        _legendTexts[ dataset ] = vValY.toString();
                    else
                        _legendTexts[ dataset ] = "";
                }
            }
            _numLegendTexts = data->usedRows();
            break;
        }
        case KDChartParams::LegendAutomatic: {
            // First, try the first row
            bool notfound = false;
            _numLegendTexts = numLegendFallbackTexts( data ); // assume this for cleaner
            // code below
            for ( dataset = 0; dataset < data->usedRows(); dataset++ ) {
                if( data->cellCoord( dataset, 0, vValY, 1 ) ){
                    if( QVariant::String == vValY.type() )
                        _legendTexts[ dataset ] = vValY.toString();
                    else
                        _legendTexts[ dataset ] = "";
                    if( _legendTexts[ dataset ].isEmpty() ){
                        notfound = true;
                        break;
                    }
                }
            }

            // If there were no entries for all the datasets, use the manually set
            // texts, and resort to Series 1, Series 2, ... where nothing has been
            // set.
            if ( notfound ) {
                for ( dataset = 0; dataset < numLegendFallbackTexts( data );
                        dataset++ ) {
                    _legendTexts[ dataset ] = params()->legendText( dataset );
                    if ( _legendTexts[ dataset ].isEmpty() || _legendTexts[ dataset ].isNull() ) {
                        _legendTexts[ dataset ] = fallbackLegendText( dataset );
                        // there
                        _numLegendTexts = numLegendFallbackTexts( data );
                    }
                }
            }
            break;
        }
        default:
            // Should not happen
            qDebug( "KDChart: Unknown legend source" );
    }
}


/**
  This method provides a fallback legend text for the specified
  dataset, if there was no other way to determine a legend text, but
  a legend should be shown nevertheless. The default is to return
  "Series" plus a dataset number (with datasets starting at 1 for
  this purpose; inherited painter implementations can override this.

  This method is only used when automatic legends are used, because
  manual and first-column legends do not need fallback texts.

  \param uint dataset the dataset number for which to generate a
  fallback text
  \return the fallback text to use for describing the specified
  dataset in the legend
  */
QString KDChartPainter::fallbackLegendText( uint dataset ) const
{
    return QObject::tr( "Series " ) + QString::number( dataset + 1 );
}


/**
  This methods returns the number of elements to be shown in the
  legend in case fallback texts are used. By default, this will be
  the number of datasets, but specialized painters can override this
  (e.g., painters that draw charts that can only display one dataset
  will return the number of values instead).

  This method is only used when automatic legends are used, because
  manual and first-column legends do not need fallback texts.

  \return the number of fallback texts to use
  */
uint KDChartPainter::numLegendFallbackTexts( KDChartTableDataBase* data ) const
{
    return data->usedRows();
}


/**
  Draws the marker for one data point according to the specified style, color, size.

  \param painter the painter to draw on
  \param style what kind of marker is drawn (square, diamond, circle, ...)
  \param color the color in which to draw the marker
  \param p the center of the marker
  \param size the width and height of the marker: both values must be positive.
  */
void KDChartPainter::drawMarker( QPainter* painter,
                                 int style,
                                 const QColor& color,
                                 const QPoint& p,
                                 const QSize& size,
                                 uint align )
{
    int width = size.width();
    int height = size.height();
    drawMarker( painter,
                0,
                0.0, 0.0,
                0,0,
                style,
                color,
                p,
                0,0,0,
                0,
                &width,
                &height,
                align );
}


/**
  Draws the marker for one data point according to the specified style.

  \param painter the painter to draw on
  \param style what kind of marker is drawn (square, diamond, circle, ...)
  \param color the color in which to draw the marker
  \param p the center of the marker
  \param dataset the dataset which this marker represents
  \param value the value which this marker represents
  \param regions a list of regions for data points, a new region for the new
  marker will be appended to this list if it is not 0

  \return pointer to the KDChartDataRegion that was appended to the regions list,
  or zero if if parameter regions was zero
  */
KDChartDataRegion* KDChartPainter::drawMarker( QPainter* painter,
                                               const KDChartParams* params,
                                               double areaWidthP1000,
                                               double areaHeightP1000,
                                               int deltaX,
                                               int deltaY,
                                               int style,
                                               const QColor& color,
                                               const QPoint& _p,
                                               uint dataset, uint value, uint chart,
                                               KDChartDataRegionList* regions,
                                               int* width,
                                               int* height,
                                               uint align )
{
    KDChartDataRegion* datReg = 0;
    const double areaSizeP1000 = qMin(areaWidthP1000, areaHeightP1000);
    int xsize  = width ? *width : (params ? params->lineMarkerSize().width() : 12);
    if( 0 > xsize )
        xsize = static_cast < int > (xsize * -areaSizeP1000);
    int ysize  = height ? *height : (params ? params->lineMarkerSize().height() : 12);
    if( 0 > ysize )
        ysize = static_cast < int > (ysize * -areaSizeP1000);
    if( KDChartParams::LineMarkerCross != style ){
        xsize = qMax( xsize, 4 );
        ysize = qMax( ysize, 4 );
    }
    uint xsize2 = xsize / 2;
    uint ysize2 = ysize / 2;
    uint xsize4 = xsize / 4;
    uint ysize4 = ysize / 4;
    uint xsize6 = xsize / 6;
    uint ysize6 = ysize / 6;
    painter->setPen( color );
    const uint xysize2 = qMin( xsize2, ysize2 );

    int x = _p.x();
    int y = _p.y();
    if( align & Qt::AlignLeft )
        x += xsize2;
    else if( align & Qt::AlignRight )
        x -= xsize2;
    if( align & Qt::AlignTop )
        y += ysize2;
    else if( align & Qt::AlignBottom )
        y -= ysize2;
    const QPoint p(x, y);

    switch ( style ) {
        case KDChartParams::LineMarkerSquare: {
                                                const QPen oldPen( painter->pen() );
                                                const QBrush oldBrush( painter->brush() );
                                                painter->setBrush( color );
                                                painter->setPen(   color );
                                                QRect rect( QPoint( p.x() - xsize2, p.y() - ysize2 ), QPoint( p.x() + xsize2, p.y() + ysize2 ) );
                                                painter->drawRect( rect );
                                                // Don't use rect for drawing after this!
                                                rect.moveBy( deltaX, deltaY );
                                                if ( regions ){
                                                    datReg =
                                                        new KDChartDataRegion(
                                                                dataset, value,
                                                                chart,   rect );
                                                    regions->append( datReg );
                                                }
                                                painter->setPen(   oldPen );
                                                painter->setBrush( oldBrush );
                                                break;
                                              }
        case KDChartParams::LineMarkerDiamond:{
                                                const QBrush oldBrush( painter->brush() );
                                                painter->setBrush( color );
                                                Q3PointArray points( 4 );
                                                points.setPoint( 0, p.x() - xsize2, p.y() );
                                                points.setPoint( 1, p.x(),          p.y() - ysize2 );
                                                points.setPoint( 2, p.x() + xsize2, p.y() );
                                                points.setPoint( 3, p.x(),          p.y() + ysize2 );
                                                painter->drawPolygon( points );
                                                // Don't use points for drawing after this!
                                                points.translate( deltaX, deltaY );
                                                if ( regions ){
                                                    datReg = new KDChartDataRegion(
                                                                    dataset, value,
                                                                    chart,   points );
                                                    regions->append( datReg  );
                                                }
                                                painter->setBrush( oldBrush );
                                                break;
                                              }
        case KDChartParams::LineMarker1Pixel: {
                                                QRect rect( p, p );
                                                painter->drawRect( rect );
                                                // Don't use rect for drawing after this!
                                                rect.moveBy( deltaX, deltaY );
                                                if ( regions ){
                                                    datReg = new KDChartDataRegion(
                                                                    dataset, value,
                                                                    chart, rect );
                                                    regions->append( datReg );
                                                }
                                                break;
                                              }
        case KDChartParams::LineMarker4Pixels:{
                                                QRect rect( p, QPoint( p.x()+1, p.y()+1 ) );
                                                painter->drawRect( rect );
                                                // Don't use rect for drawing after this!
                                                rect.moveBy( deltaX, deltaY );
                                                if ( regions ){
                                                    datReg = new KDChartDataRegion(
                                                                    dataset, value,
                                                                    chart, rect );
                                                    regions->append( datReg );
                                                }
                                                break;
                                              }
        case KDChartParams::LineMarkerRing:   {
                                                const QPen oldPen( painter->pen() );
                                                painter->setPen( QPen( color, qMin(xsize4, ysize4) ) );
                                                const QBrush oldBrush( painter->brush() );
                                                painter->setBrush( Qt::NoBrush );
                                                painter->drawEllipse( p.x() - xsize2, p.y() - ysize2, xsize, ysize );
                                                if ( regions ) {
                                                    Q3PointArray points;
                                                    points.makeEllipse( p.x() - xsize2, p.y() - ysize2, xsize, ysize );
                                                    // Don't use points for drawing after this!
                                                    points.translate( deltaX, deltaY );
                                                    if( points.size() > 0 ){
                                                        datReg = new KDChartDataRegion(
                                                                        dataset, value,
                                                                        chart,   points );
                                                        regions->append( datReg );
                                                    }
                                                }
                                                painter->setBrush( oldBrush );
                                                painter->setPen(   oldPen );
                                                break;
                                              }
        case KDChartParams::LineMarkerCross:  {
                                                const QPen oldPen( painter->pen() );
                                                painter->setPen( color );
                                                const QBrush oldBrush( painter->brush() );
                                                painter->setBrush( color );
                                                int numPoints = (ysize && xsize) ? 12 : 4;
                                                Q3PointArray points( numPoints );
                                                if( ysize && xsize ){
                                                    points.setPoint( 0, p.x() - xsize6, p.y() - ysize6 );
                                                    points.setPoint( 1, p.x() - xsize6, p.y() - ysize2 );
                                                    points.setPoint( 2, p.x() + xsize6, p.y() - ysize2 );
                                                    points.setPoint( 3, p.x() + xsize6, p.y() - ysize6 );
                                                    points.setPoint( 4, p.x() + xsize2, p.y() - ysize6 );
                                                    points.setPoint( 5, p.x() + xsize2, p.y() + ysize6 );
                                                    points.setPoint( 6, p.x() + xsize6, p.y() + ysize6 );
                                                    points.setPoint( 7, p.x() + xsize6, p.y() + ysize2 );
                                                    points.setPoint( 8, p.x() - xsize6, p.y() + ysize2 );
                                                    points.setPoint( 9, p.x() - xsize6, p.y() + ysize6 );
                                                    points.setPoint(10, p.x() - xsize2, p.y() + ysize6 );
                                                    points.setPoint(11, p.x() - xsize2, p.y() - ysize6 );
                                                }else if( ysize ){
                                                    points.setPoint( 0, p.x() - ysize6, p.y() - ysize2 );
                                                    points.setPoint( 1, p.x() + ysize6, p.y() - ysize2 );
                                                    points.setPoint( 2, p.x() + ysize6, p.y() + ysize2 );
                                                    points.setPoint( 3, p.x() - ysize6, p.y() + ysize2 );
                                                }else{
                                                    points.setPoint( 0, p.x() - xsize2, p.y() - xsize6 );
                                                    points.setPoint( 1, p.x() + xsize2, p.y() - xsize6 );
                                                    points.setPoint( 2, p.x() + xsize2, p.y() + xsize6 );
                                                    points.setPoint( 3, p.x() - xsize2, p.y() + xsize6 );
                                                }
                                                painter->drawPolygon( points );
                                                // Don't use points for drawing after this!
                                                points.translate( deltaX, deltaY );
                                                if( regions ){
                                                    datReg = new KDChartDataRegion(
                                                                    dataset, value,
                                                                    chart,   points );
                                                    regions->append( datReg );
                                                }
                                                painter->setBrush( oldBrush );
                                                painter->setPen(   oldPen );
                                                break;
                                              }
        case KDChartParams::LineMarkerFastCross: {
                                                const QPen oldPen( painter->pen() );
                                                painter->setPen( color );
                                                painter->drawLine( QPoint(p.x() - xysize2, p.y()),
                                                                   QPoint(p.x() + xysize2, p.y()) );
                                                painter->drawLine( QPoint(p.x(), p.y() - xysize2),
                                                                   QPoint(p.x(), p.y() + xysize2) );
                                                QRect rect( QPoint( p.x() - 2, p.y() - 2 ),
                                                            QPoint( p.x() + 2, p.y() + 2 ) );
                                                // Don't use rect for drawing after this!
                                                rect.moveBy( deltaX, deltaY );
                                                if ( regions ){
                                                    datReg =
                                                        new KDChartDataRegion(
                                                                dataset, value,
                                                                chart,   rect );
                                                    regions->append( datReg );
                                                }
                                                painter->setPen(   oldPen );
                                                break;
                                              }
        case KDChartParams::LineMarkerCircle:
        default:                              {
                                                const QBrush oldBrush( painter->brush() );
                                                painter->setBrush( color );
                                                painter->drawEllipse( p.x() - xsize2, p.y() - ysize2, xsize, ysize );
                                                if ( regions ) {
                                                    Q3PointArray points;
                                                    points.makeEllipse( p.x() - xsize2, p.y() - ysize2, xsize, ysize );
                                                    // Don't use points for drawing after this!
                                                    points.translate( deltaX, deltaY );
                                                    if( points.size() > 0 ){
                                                        datReg = new KDChartDataRegion(
                                                                        dataset, value,
                                                                        chart,   points );
                                                        regions->append( datReg );
                                                    }
                                                }
                                                painter->setBrush( oldBrush );
                                              }
    }
    return datReg;
}


void KDChartPainter::drawExtraLinesAndMarkers(
        KDChartPropertySet& propSet,
        const QPen& defaultPen,
        const KDChartParams::LineMarkerStyle& defaultMarkerStyle,
        int myPointX,
        int myPointY,
        QPainter* painter,
        const KDChartAxisParams* abscissaPara,
        const KDChartAxisParams* ordinatePara,
        const double areaWidthP1000,
        const double areaHeightP1000,
        bool bDrawInFront )
{

    // we can safely call the following functions and ignore their
    // return values since they will touch the parameters' values
    // if the propSet *contains* corresponding own values only.
    int  iDummy;
    uint extraLinesAlign = 0;
    if( propSet.hasOwnExtraLinesAlign( iDummy, extraLinesAlign )
        && ( extraLinesAlign
            & ( Qt::AlignLeft | Qt::AlignRight  | Qt::AlignHCenter |
                Qt::AlignTop  | Qt::AlignBottom | Qt::AlignVCenter ) ) ){
        bool extraLinesInFront = false;
        propSet.hasOwnExtraLinesInFront( iDummy, extraLinesInFront );
        if( bDrawInFront == extraLinesInFront ){
            const double areaSizeP1000 = qMin(areaWidthP1000, areaHeightP1000);
            int          extraLinesLength = -20;
            int          extraLinesWidth = defaultPen.width();
            QColor       extraLinesColor = defaultPen.color();
            Qt::PenStyle extraLinesStyle = defaultPen.style();
            uint         extraMarkersAlign = 0;
            propSet.hasOwnExtraLinesLength( iDummy, extraLinesLength );
            propSet.hasOwnExtraLinesWidth(  iDummy, extraLinesWidth  );
            propSet.hasOwnExtraLinesColor(  iDummy, extraLinesColor  );
            propSet.hasOwnExtraLinesStyle(  iDummy, extraLinesStyle  );
            const int horiLenP2 = (0 > extraLinesLength)
                                ? static_cast<int>(areaWidthP1000  * extraLinesLength) / 2
                                : extraLinesLength / 2;
            const int vertLenP2 = (0 > extraLinesLength)
                                ? static_cast<int>(areaHeightP1000 * extraLinesLength) / 2
                                : extraLinesLength / 2;
            // draw the extra line(s)
            QPoint pL( (Qt::AlignLeft == (extraLinesAlign & Qt::AlignLeft))
                    ? 0
                    : (Qt::AlignHCenter == (extraLinesAlign & Qt::AlignHCenter))
                        ? myPointX - horiLenP2
                        : myPointX,
                    myPointY );
            QPoint pR( (Qt::AlignRight == (extraLinesAlign & Qt::AlignRight))
                    ? abscissaPara->axisTrueAreaRect().width()
                    : (Qt::AlignHCenter == (extraLinesAlign & Qt::AlignHCenter))
                        ? myPointX + horiLenP2
                        : myPointX,
                    myPointY );
            QPoint pT( myPointX,
                    (Qt::AlignTop == (extraLinesAlign & Qt::AlignTop))
                    ? 0
                    : (Qt::AlignVCenter == (extraLinesAlign & Qt::AlignVCenter))
                        ? myPointY - vertLenP2
                        : myPointY );
            QPoint pB( myPointX,
                    (Qt::AlignBottom == (extraLinesAlign & Qt::AlignBottom))
                    ? ordinatePara->axisTrueAreaRect().height()
                    : (Qt::AlignVCenter == (extraLinesAlign & Qt::AlignVCenter))
                        ? myPointY + vertLenP2
                        : myPointY );
            const QPen extraPen( extraLinesColor,
                                0 > extraLinesWidth
                                ? static_cast < int > ( areaSizeP1000 * -extraLinesWidth )
                                : extraLinesWidth,
                                extraLinesStyle );
            const QPen oldPen( painter->pen() );
            painter->setPen( extraPen );
            if( extraLinesAlign & ( Qt::AlignLeft | Qt::AlignRight | Qt::AlignHCenter ) )
                painter->drawLine( pL, pR );
            if( extraLinesAlign & ( Qt::AlignTop | Qt::AlignBottom | Qt::AlignVCenter ) )
                painter->drawLine( pT, pB );
            painter->setPen( oldPen );
            // draw the marker(s) of the extra line(s)
            propSet.hasOwnExtraMarkersAlign( iDummy, extraMarkersAlign );
            if( extraMarkersAlign
                    & ( Qt::AlignLeft | Qt::AlignRight |
                        Qt::AlignTop  | Qt::AlignBottom ) ){
                QSize  extraMarkersSize  = params()->lineMarkerSize();
                QColor extraMarkersColor = extraLinesColor;
                int    extraMarkersStyle = defaultMarkerStyle;
                propSet.hasOwnExtraMarkersSize(  iDummy, extraMarkersSize );
                propSet.hasOwnExtraMarkersColor( iDummy, extraMarkersColor );
                propSet.hasOwnExtraMarkersStyle( iDummy, extraMarkersStyle );
                // draw the extra marker(s)
                int w = extraMarkersSize.width();
                int h = extraMarkersSize.height();
                if( w < 0 )
                    w = static_cast < int > (w * -areaSizeP1000);
                if( h < 0 )
                    h = static_cast < int > (h * -areaSizeP1000);
                if( extraMarkersAlign & Qt::AlignLeft )
                    drawMarker( painter,
                                params(),
                                _areaWidthP1000, _areaHeightP1000,
                                _dataRect.x(), _dataRect.y(),
                                (KDChartParams::LineMarkerStyle)extraMarkersStyle,
                                extraMarkersColor,
                                pL,
                                0, 0, 0, 0,
                                &w, &h,
                                Qt::AlignCenter );
                if( extraMarkersAlign & Qt::AlignRight )
                    drawMarker( painter,
                                params(),
                                _areaWidthP1000, _areaHeightP1000,
                                _dataRect.x(), _dataRect.y(),
                                (KDChartParams::LineMarkerStyle)extraMarkersStyle,
                                extraMarkersColor,
                                pR,
                                0, 0, 0, 0,
                                &w, &h,
                                Qt::AlignCenter );
                if( extraMarkersAlign & Qt::AlignTop )
                    drawMarker( painter,
                                params(),
                                _areaWidthP1000, _areaHeightP1000,
                                _dataRect.x(), _dataRect.y(),
                                (KDChartParams::LineMarkerStyle)extraMarkersStyle,
                                extraMarkersColor,
                                pT,
                                0, 0, 0, 0,
                                &w, &h,
                                Qt::AlignCenter );
                if( extraMarkersAlign & Qt::AlignBottom )
                    drawMarker( painter,
                                params(),
                                _areaWidthP1000, _areaHeightP1000,
                                _dataRect.x(), _dataRect.y(),
                                (KDChartParams::LineMarkerStyle)extraMarkersStyle,
                                extraMarkersColor,
                                pB,
                                0, 0, 0, 0,
                                &w, &h,
                                Qt::AlignCenter );
            }
        }
    }
}


