/* -*- Mode: C++ -*-
   $Id$
   KDChart - a multi-platform charting engine
*/

/****************************************************************************
** Copyright (C) 2001-2002 Klarälvdalens Datakonsult AB.  All rights reserved.
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
** See http://www.klaralvdalens-datakonsult.se/Public/products/ for
**   information about KDChart Commercial License Agreements.
**
** Contact info@klaralvdalens-datakonsult.se if any conditions of this
** licensing are not clear to you.
**
**********************************************************************/
#include <KDChartParams.h>
#ifdef __WINDOWS__
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
#include <KDChartTextPiece.h>

#include <qpainter.h>
#include <qpaintdevicemetrics.h>

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
  _legendTitle( 0 ),
    _params( params )
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

/**
   Creates an object of a concrete subclass of KDChartPainter that
   KDChart::paint() (and consequently, the application) can use to
   have charts painted. The subclass is determined on the base of the
   params parameter which among other things indicates the type of the
   chart.

   \param params the parameter set which is used to determine the
   painter implementation to be used
   \return a pointer to an object of a subclass of KDChartPainter that
   can be used to draw charts as defined by the \a params parameter
   \throw KDChartUnknownTypeException if there is no registered
   KDChartPainter subclass for the type specified in \a params. This
   can only happen with user-defined chart types.
*/
KDChartPainter* KDChartPainter::create( KDChartParams* params, bool make2nd )
#ifdef USE_EXCEPTIONS
throw ( KDChartUnknownTypeException )
#endif
{
    KDChartParams::ChartType cType = make2nd
                                     ? params->additionalChartType()
                                     : params->chartType();
    switch ( cType )
        {
        case KDChartParams::Bar:
            return new KDChartBarPainter( params );
            break;
        case KDChartParams::Line:
            return new KDChartLinesPainter( params );
            break;
        case KDChartParams::Area:
            return new KDChartAreaPainter( params );
            break;
        case KDChartParams::Pie:
            return new KDChartPiePainter( params );
            break;
        case KDChartParams::Ring:
            return new KDChartRingPainter( params );
            break;
        case KDChartParams::HiLo:
            return new KDChartHiLoPainter( params );
	case KDChartParams::Polar:
	  return new KDChartPolarPainter( params );
        case KDChartParams::NoType:
        default:
#ifdef USE_EXCEPTIONS
            throw KDChartUnknownTypeException( QString::number( cType ) );
#else
            return 0;
#endif
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
   \throw KDChartUnknownTypeException if no painter implementation is
   registered under the specified name
*/
void KDChartPainter::unregisterPainter( const QString& /*painterName*/ )
#ifdef USE_EXCEPTIONS
throw( KDChartUnknownTypeException )
#endif
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
                            const QRect* rect )
{
    // Protect against non-existing data
    if ( data->usedCols() == 0 && data->usedRows() == 0 )
        return ;

    QFont actLegendFont;
    QFont actLegendTitleFont;
    setupGeometry
        ( painter, data, actLegendFont, actLegendTitleFont, rect );

    // Note: In addition to the below paintArea calls there might be several
    //       other paintArea calls regarding to the BASE areas (AreaAxisBASE,
    //       AreaHdFtBASE, AreaCustomBoxesBASE).
    //       These additional calls result in smaler areas being drawn inside
    //       on the larger ones specifies here.
    if ( paintFirst ) {
        paintArea(   painter, KDChartEnums::AreaOutermost );
        paintArea(   painter, KDChartEnums::AreaInnermost );
        paintArea(   painter, KDChartEnums::AreaDataAxesLegendHeadersFooters );
        paintArea(   painter, KDChartEnums::AreaHeaders );
        paintHeader( painter, data );
        paintArea(   painter, KDChartEnums::AreaFooters );
        paintFooter( painter, data );
        paintArea(   painter, KDChartEnums::AreaDataAxesLegend );
        paintArea(   painter, KDChartEnums::AreaDataAxes );
        paintArea(   painter, KDChartEnums::AreaAxes );
        paintArea(   painter, KDChartEnums::AreaData );
        paintAxes(   painter, data );
    }

    painter->save();
    paintData( painter, data, !paintFirst, regions );
    painter->restore();

    if ( paintLast ) {
        paintDataValues( painter, data, regions );
        paintArea(   painter, KDChartEnums::AreaLegend );
        paintLegend( painter, data, actLegendFont, actLegendTitleFont );
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
    uint maskBASE = KDChartEnums::AreaBASEMask & area;
    //bool allCustomBoxes = false;
    bool bFound;
    const KDChartParams::KDChartFrameSettings* settings =
                                params()->frameSettings( area, bFound );

    // in case no settings for a single custom box were found try to retrieve
    // global custom box settings:
    if( !bFound && (KDChartEnums::AreaCustomBoxesBASE == maskBASE) )
        settings = params()->frameSettings( KDChartEnums::AreasCustomBoxes, bFound );
    if( bFound ) {
        bool allCustomBoxes;

        QRect rect( calculateAreaRect( allCustomBoxes,
                                       area,
                                       dataRow, dataCol, data3rd, regions ) );

        if( allCustomBoxes ) {
            uint idx;
            for( idx = 0; idx <= params()->maxCustomBoxIdx(); ++idx ) {
                const KDChartCustomBox* box = params()->customBox( idx );
                if( box ) {
                    const KDChartParams::KDChartFrameSettings * singleSettings
                        = params()->frameSettings(
                            KDChartEnums::AreaCustomBoxesBASE + idx, bFound );

                    rect = box->trueRect( calculateAnchor( *box ),
                                          _areaWidthP1000,
                                          _areaHeightP1000 );
                    paintAreaWithGap( painter, rect,
                                      bFound ? *singleSettings : *settings );
                }
            }
        } else
            paintAreaWithGap( painter, rect, *settings );
    }
}


/**
    Paints an area frame.
    This methode is called internally by KDChartPainter::paintArea.
*/
void KDChartPainter::paintAreaWithGap( QPainter* painter,
                                       QRect rect,
                                       const KDChartParams::KDChartFrameSettings& settings )
{
    if( rect.isValid() ) {
        rect.moveBy( -settings.innerGapX(), -settings.innerGapY() );
        rect.setWidth(  rect.width()  + 2*settings.innerGapX() );
        rect.setHeight( rect.height() + 2*settings.innerGapY() );
        settings.frame().paint( painter, KDFrame::PaintAll, rect );
    }
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

        painter->save();

        QFont font0( params()->dataValuesFont( 0 ) );
        if( params()->dataValuesUseFontRelSize(  0 ) ) {
            float size = _areaWidthP1000 * params()->dataValuesFontRelSize( 0 );
            if ( 9.0 > size )
                size = 9.0;
            font0.setPointSizeFloat( size );
        }
        QFontMetrics fm0( font0 );
        double fm0HeightP100( fm0.height() / 100.0 );
        QFont font1( params()->dataValuesFont( 1 ) );
        if( params()->dataValuesUseFontRelSize(  1 ) ) {
            float size = _areaWidthP1000 * params()->dataValuesFontRelSize( 1 );
            if ( 9.0 > size )
                size = 9.0;
            font1.setPointSizeFloat( size );
        }
        QFontMetrics fm1( font1 );
        double fm1HeightP100( fm1.height() / 100.0 );
        bool lastDigitIrrelevant0 = true;
        bool lastDigitIrrelevant1 = true;
        // get and format the texts
        for ( region=regions->first();
              region != 0;
              region = regions->next() ) {
            const KDChartData& cell = data->cell( region->row, region->col );
            if (     cell.isString()
                && !cell.stringValue().isEmpty()
                && !cell.stringValue().isNull() )
                region->text = cell.stringValue();
            else {
                double value( cell.doubleValue() );
                region->negative = 0.0 > value;
                double divi( pow( 10, params()->dataValuesDivPow10( region->chart ) ) );
                if ( 1.0 != divi )
                    value /= divi;
                int digits( params()->dataValuesDigitsBehindComma( region->chart ) );
                bool autoDigits( KDChartParams::DATA_VALUE_AUTO_DIGITS == digits );
                if( autoDigits ) {
                    if( 10 < digits )
                        digits = 10;
                } else
                    (   region->chart
                    ? lastDigitIrrelevant1
                    : lastDigitIrrelevant0 ) = false;
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
        QFontMetrics actFM( painter->font() );
        QFont* oldFont = 0;
        int oldRotation = 0;
        uint oldChart = UINT_MAX;
        uint oldDatacolorNo = UINT_MAX;
        for ( region=regions->first();
            region != 0;
            region = regions->next() ) {
            if ( region->text.length() ) {
                bool zero(    0.0 == data->cell( region->row, region->col ).doubleValue()
                           || 0   == data->cell( region->row, region->col ).doubleValue() );
                uint align( params()->dataValuesAnchorAlign( region->chart,
                                                             region->negative ) );
                KDChartParams::ChartType cType = region->chart
                                               ? params()->additionalChartType()
                                               : params()->chartType();
                bool rectangular = (    KDChartParams::Bar   == cType
                                     || KDChartParams::Line  == cType
                                     || KDChartParams::HiLo  == cType
                                     || KDChartParams::Gantt == cType );
                bool circular    = (    KDChartParams::Pie   == cType
                                     || KDChartParams::Ring  == cType
                                     || KDChartParams::Polar == cType );

                KDChartEnums::PositionFlag anchorPos(
                                params()->dataValuesAnchorPosition( region->chart,
                                                                    region->negative ) );
                QPoint anchor(
                    rectangular
                    ? KDChartEnums::positionFlagToPoint( region->region.boundingRect(),
                                                         anchorPos )
                    : KDChartEnums::positionFlagToPoint( region->points,
                                                         anchorPos ) );
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
                double anchorDX( params()->dataValuesAnchorDeltaX( region->chart,
                                                                   region->negative )
                                 * fmHeightP100 );
                double anchorDY( params()->dataValuesAnchorDeltaY( region->chart,
                                                                   region->negative )
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
                        double radialFactor( ( radialLen - anchorDY ) / radialLen );
                        anchor.setX( static_cast < int > ( pM.x() - dX * radialFactor ) );
                        anchor.setY( static_cast < int > ( pM.y() - dY * radialFactor ) );
                    }
                } else {
                    anchor.setX( anchor.x() + static_cast < int > ( anchorDX ) );
                    anchor.setY( anchor.y() + static_cast < int > ( anchorDY ) );
                }

                int rotation( params()->dataValuesRotation( region->chart,
                                                            region->negative ) );
                if ( rotation ) {
                    if (    KDChartParams::SAGGITAL_ROTATION   == rotation
                         || KDChartParams::TANGENTIAL_ROTATION == rotation ) {
                        rotation =  (   KDChartParams::TANGENTIAL_ROTATION == rotation
                                      ? -1440
                                      : 0 )
                                    + angle;
                        rotation /= 16;
                        if ( 360 <= rotation )
                            rotation -= 360;
                        else if ( 0 > rotation )
                            rotation += 360;
                        rotation = 360 - rotation;
                    }

                    if( rotation != oldRotation ) {
                        painter->rotate( rotation - oldRotation );
                        oldRotation = rotation;
                    }

                    QFont* actFont = region->chart ? &font1 : &font0;
                    if( oldFont != actFont ) {
                        painter->setFont( *actFont );
                        oldFont = actFont;
                        actFM = QFontMetrics( painter->font() );
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
                    if( allowOverlapping )
                        drawThisOne = true;
                    else {
                       QRegion sectReg( infosKDD.region.intersect( lastRegionDone ) );
                        drawThisOne = sectReg.isEmpty();
                    }
                    if( drawThisOne ) {
                        lastRegionDone     = lastRegionDone.unite( infosKDD.region );
                        region->textRegion = infosKDD.region;


                        /*

                          NOTE: The following will be REMOVED again once
                                the layout policy feature is implemented !!!

                        */
                        if( params()->dataValuesAutoColor( region->chart ) ) {
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
                        else if( oldChart != region->chart ) {
                            oldChart = region->chart;
                            painter->setPen( params()->dataValuesColor( region->chart ) );
                        }


                        painter->drawText( infosKDD.x, infosKDD.y,
                                          infosKDD.width, infosKDD.height,
                                          Qt::AlignLeft | Qt::AlignTop | Qt::SingleLine,
                                          region->text );
                    } // if not intersect

                } else {

                    // no rotation:
                    painter->rotate( -oldRotation );
                    oldRotation = 0;
                    QFontMetrics & fm = region->chart ? fm1 : fm0;
                    int w = fm.width( region->text );

                    // subtract right bearing of last char from total text width
                    int tLen = region->text.length();
                    if ( 0 < tLen )
                        w -= fm.rightBearing( region->text[ tLen-1 ] );

                    int h  = fm.height();
                    int dx = 0;
                    int dy = 0;
                    switch( align & ( Qt::AlignLeft | Qt::AlignRight | Qt::AlignHCenter ) ) {
                    case Qt::AlignRight:
                                dx = -w+1;
                                break;
                    case Qt::AlignHCenter:
                                dx = -w / 2;
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
                        lastRegionDone     = lastRegionDone.unite( thisRegion );
                        region->textRegion = thisRegion;


                        /*
                        // for testing:
                        QRect rect( region->textRegion.boundingRect() );
                        //painter->drawRect( rect );
                        */


                        /*

                          NOTE: The following will be REMOVED again once
                                the layout policy feature is implemented !!!

                        */
                        painter->setFont( region->chart ? font1 : font0 );
                        if( params()->dataValuesAutoColor( region->chart ) ) {
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
                        else
                            painter->setPen( params()->dataValuesColor( region->chart ) );
                        QRect rect( region->textRegion.boundingRect() );
                        painter->drawText( rect.left(),rect.top(),rect.width(),rect.height(),
                                          Qt::AlignLeft | Qt::AlignTop, region->text );
                    }


                }
            }
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
    // paint all of the custom boxes
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
            // paint content
            box->paint( painter, calculateAnchor( *box, regions ),
                                 _areaWidthP1000,
                                 _areaHeightP1000 );
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
                        rect = current->region.boundingRect();
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


QPoint KDChartPainter::pointOnCircle( const QRect& rect, int angle )
{
    // There are two ways of computing this: The simple, but slow one
    // is to use QPointArray.makeArc() and take the first point. The
    // more advanced, but faster one is to do the trigonometric
    // computionations ourselves. Since the comments in
    // QPointArray::makeArc() very often say that the code there is
    // "poor", we'd better do it outselves...

    double normAngle = angle / 16;
    double normAngleRad = DEGTORAD( normAngle );
    double cosAngle = cos( normAngleRad );
    double sinAngle = -sin( normAngleRad );
    double posX = floor( cosAngle * ( double ) rect.width() / 2.0 + 0.5 );
    double posY = floor( sinAngle * ( double ) rect.height() / 2.0 + 0.5 );
    return QPoint( ( int ) posX + rect.center().x(),
                   ( int ) posY + rect.center().y() );
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


/**
 Paints the legend for the chart. The implementation in KDChartPainter
 draws a standard legend that should be suitable for most chart
 types. Subclasses can provide their own implementations.

   \param painter the QPainter onto which the chart should be drawn
   \param data the data that will be displayed as a chart
   \param actLegendFont the font in which the legend will be drawn
   \param actLegendTitleFont the font in which the legend title will
   be drawn
*/
void KDChartPainter::paintLegend( QPainter* painter,
                                  KDChartTableDataBase* /*data*/,
                                  const QFont& actLegendFont,
                                  const QFont& /*actLegendTitleFont*/ )
{
    if ( params()->legendPosition() == KDChartParams::NoLegend )
        return ; // do not draw legend

    painter->save();

    // start out with a rectangle around the legend
    painter->setPen( QPen( Qt::black, 1 ) );
    painter->setBrush( QBrush::NoBrush );

    bool bFrameFound;
    params()->frameSettings( KDChartEnums::AreaLegend, bFrameFound );
    if( !bFrameFound )
        painter->drawRect( _legendRect );

    int xpos = _legendRect.left() + _legendEMSpace,
        ypos = _legendRect.top() +
                static_cast < int > ( _legendTitle->height() * 0.15 );

    // first paint the title, if any
    if ( _legendTitle != 0 ) { // shows that there is a title
        _legendTitle->draw( painter, xpos, ypos,
                            QRegion( xpos, ypos,
                                     _legendRect.width() - ( xpos - _legendRect.left() ),
                                     _legendTitle->height() ),
                            params()->legendTitleTextColor() );
        ypos += _legendTitle->height();
    }

    // loop over all the datasets, each one has one row in the legend
    // if its data are to be used in at least one of the charts drawn
    painter->setFont( actLegendFont );
    int dataset;
    for ( dataset = 0; dataset < _numLegendTexts; dataset++ ) {
        /*
        if( KDChartParams::DataEntry == params()->chartSourceMode( dataset ) ) {
*/
        painter->setBrush( QBrush( params()->dataColor( dataset ),
                                   QBrush::SolidPattern ) );
        painter->setPen( Qt::black );
        painter->drawRect( xpos,
                           ( _legendHeight - _legendEMSpace ) / 2 + ypos,
                           _legendEMSpace, _legendEMSpace );
        xpos += _legendEMSpace; // the box
        xpos += _legendEMSpace; // the space between the box and the text
        painter->setPen( params()->legendTextColor() );
        painter->drawText( xpos, ypos, _legendRect.width() - ( xpos - _legendRect.left() ),
                           _legendHeight, Qt::AlignLeft | Qt::AlignVCenter, _legendTexts[ dataset ] );

        xpos = _legendRect.left() + _legendEMSpace;
        ypos += _legendSpacing;
        /*
        }
*/
    }

    painter->restore();
}


/**
 Paints the header for the chart. The implementation in KDChartPainter
 draws a standard header that should be suitable for most chart
 types. Subclasses can provide their own implementations.

   \param painter the QPainter onto which the chart should be drawn
   \param data the data that will be displayed as a chart
*/
void KDChartPainter::paintHeader( QPainter* painter,
                                  KDChartTableDataBase* /*data*/ )
{
    /*
    //
    //
    //   CODE - REORGANISATION   P L A N N E D . . .
    //
    //   ( NOT RELEVANT FOR 1st RELEASE )


    N a t u e r l i c h     wird der folgende Abschnitt

    komplett umgestellt und durch eine schlanke Funktion
    ersetzt, die alle Header/Footer Sektionen behandelt.


*/
    const double averageValueP1000 = ( _areaWidthP1000 + _areaHeightP1000 ) / 2.0;

    painter->save();

    QString h1 = params()->header1Text();
    if ( !h1.isEmpty() ) {
        QFont actFont( params()->header1Font() );
        if ( params()->headerFooterFontUseRelSize(
                                                  KDChartParams::HdFtPosHeader ) ) {
            actFont.setPointSizeFloat(
                                      params()->headerFooterFontRelSize(
                                                                        KDChartParams::HdFtPosHeader ) * averageValueP1000 );
        }
        painter->setPen( params()->headerFooterColor( KDChartParams::HdFtPosHeader ) );
        painter->setFont( actFont );
        painter->drawText( _header1Rect,
                           Qt::AlignCenter | Qt::SingleLine, h1 );
    }
    QString h2 = params()->header2Text();
    if ( !h2.isEmpty() ) {
        QFont actFont( params()->header2Font() );
        if ( params()->headerFooterFontUseRelSize(
                                                  KDChartParams::HdFtPosHeader2 ) ) {
            actFont.setPointSizeFloat(
                                      params()->headerFooterFontRelSize(
                                                                        KDChartParams::HdFtPosHeader2 ) * averageValueP1000 );
        }
        painter->setPen( params()->headerFooterColor( KDChartParams::HdFtPosHeader2 ) );
        painter->setFont( actFont );
        painter->drawText( _header2Rect,
                           Qt::AlignCenter | Qt::SingleLine, h2 );
    }
    painter->restore();
}

/**
 Paints the footer for the chart. The implementation in KDChartPainter
 draws a standard footer that should be suitable for most chart
 types. Subclasses can provide their own implementations.

   \param painter the QPainter onto which the chart should be drawn
   \param data the data that will be displayed as a chart
*/
void KDChartPainter::paintFooter( QPainter* painter,
                                  KDChartTableDataBase* /*data*/ )
{
    /*
    //
    //
    //   CODE - REORGANISATION   P L A N N E D . . .
    //
    //   ( NOT RELEVANT FOR 1st RELEASE )


    N a t u e r l i c h     wird der folgende Abschnitt

    komplett umgestellt und durch eine schlanke Funktion
    ersetzt, die alle Header/Footer Sektionen behandelt.


*/
    const double averageValueP1000 = ( _areaWidthP1000 + _areaHeightP1000 ) / 2.0;

    painter->save();

    QString f = params()->footerText();
    if ( !f.isEmpty() ) {
        QFont actFont( params()->footerFont() );
        if ( params()->headerFooterFontUseRelSize(
                                                  KDChartParams::HdFtPosFooter ) ) {
            actFont.setPointSizeFloat(
                                      params()->headerFooterFontRelSize(
                                                                        KDChartParams::HdFtPosFooter ) * averageValueP1000 );
        }
        painter->setPen( params()->headerFooterColor( KDChartParams::HdFtPosFooter ) );
        painter->setFont( actFont );
        painter->drawText( _footerRect,
                           Qt::AlignCenter | Qt::SingleLine, f );
    }
    painter->restore();
}


/**
   This method will be called whenever any parameters that affect
   geometry have been changed. It will compute the appropriate
   positions for the various parts of the chart (legend, axes, data
   area etc.). The implementation in KDChartPainter computes a
   standard geometry that should be suitable for most chart
   types. Subclasses can provide their own implementations.

   \param painter the QPainter onto which the chart should be
   drawn. Only used to determine the size and default values for fonts
   etc.
   \param data the data that will be displayed as a chart
   \param actLegendFont after return from this method, this parameter
   will contain the font that is to be used for the legend texts
   \param actLegendTitleFont after return from this method, this
   parameter will contain the font that is to be used for the legend
   title (unless there is no legend title in which case the value of
   this parameter will be undefined)
   \throw KDChartNotEnoughSpaceException if it is not possible to draw
   all parts of the chart in the available space (only if KDChart is
   compiled to use exceptions).
*/
void KDChartPainter::setupGeometry( QPainter* painter,
                                    KDChartTableDataBase* data,
                                    QFont& actLegendFont,
                                    QFont& actLegendTitleFont,
                                    const QRect* rect )
#ifdef USE_EXCEPTIONS
throw( KDChartNotEnoughSpaceException )
#endif
{
    QPaintDeviceMetrics painterMetrics( painter->device() );
    QRect drawRect;
    if( rect )
        drawRect = *rect;
    else
        drawRect = QRect( 0, 0, painterMetrics.width(), painterMetrics.height() );

    /*
      Philipp:
      Hack to display the surrounding frame.
      Frames are displayed by the rect (rect-1,rect-1,width+1,height+1) See KDFrame::paintEdges
      The QMAX in it makes it even worth.
      As the primary rect "drawRect" doesn't contain these coordinates, it would be displayed wrong.
      So I'm deducting on each side 1 pixel to enable the display of the surrounding frame
    */
    drawRect.moveBy( 1,1 );
    drawRect.setWidth( drawRect.width()-2 );
    drawRect.setHeight( drawRect.height()-2 );

    int yposTop    = drawRect.top();
    int xposLeft   = drawRect.left();
    int yposBottom = drawRect.height();
    int xposRight  = drawRect.width();

    int trueWidth  = drawRect.width();
    int trueHeight = drawRect.height();

    // Temporary values used to calculate start values xposLeft, yposTop, xposRight, yposBottom.
    // They will be replaced immediately after these calculations.
    _areaWidthP1000    = trueWidth / 1000.0;
    _areaHeightP1000   = trueHeight / 1000.0;


    _outermostRect = drawRect;

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

    _innermostRect = QRect( xposLeft,           yposTop,
                            xposRight-xposLeft, yposBottom-yposTop );


    _logicalWidth  = xposRight  - xposLeft;
    _logicalHeight = yposBottom - yposTop;


    // true values (having taken the global leadings into account)
    // to be used by all following functions
    _areaWidthP1000 =  _logicalWidth  / 1000.0;
    _areaHeightP1000 = _logicalHeight / 1000.0;

    double averageValueP1000 = ( _areaWidthP1000 + _areaHeightP1000 ) / 2.0;

    // new code design:
    //        1. now min-header-leading is text height/2
    //        2. leading or legendSpacing (whichever is larger)
    //           will be added if legend is below the header(s)
    //        3. leading will be added between header and data area
    //           in case there is no top legend but grid is to be shown.
    int headerLineLeading = 0;
    bool hasHeader = !params()->header1Text().isEmpty();



    /*
    //
    //
    //   CODE - REORGANISATION   P L A N N E D . . .
    //
    //   ( NOT RELEVANT FOR 1st RELEASE )


    N a t u e r l i c h     wird der folgende Abschnitt

    komplett umgestellt: statt dreier, fast identischer Codebloecke
    wird eine Schleife verwendet ... am Montag.  :)


*/





    if ( hasHeader ) {

        /* old code:
       QFontMetrics header1Metrics( params()->header1Font() );
       end of old code */

        /* new code: QUICK AND DIRTY DUE TO LIMITED TIME */
        QFont actFont( params()->header1Font() );
        if ( params()->headerFooterFontUseRelSize(
                                                  KDChartParams::HdFtPosHeader ) ) {
            actFont.setPointSizeFloat(
                                      params()->headerFooterFontRelSize(
                                                                        KDChartParams::HdFtPosHeader ) * averageValueP1000 );
        }
        QFontMetrics header1Metrics( actFont );
        /* end of new code */

        //headerLineLeading = header1Metrics.leading();
        //if( !headerLineLeading )
        headerLineLeading = header1Metrics.lineSpacing() / 2;
        yposTop += headerLineLeading;
        _header1Rect = QRect( xposLeft, yposTop, xposRight-xposLeft,
                              header1Metrics.height() );

        // this command line is ONLY A TEMPORARY HACK - later not needed anymore
        ((KDChartParams*)params())->__internalStoreHdFtRect( KDChartParams::HdFtPosHeader, _header1Rect );

        yposTop += _header1Rect.height();
    }


    if ( !params()->header2Text().isEmpty() ) {

        /* old code:
       QFontMetrics header2Metrics( params()->header2Font() );
       end of old code */

        /* new code: QUICK AND DIRTY DUE TO LIMITED TIME */
        QFont actFont( params()->header2Font() );
        if ( params()->headerFooterFontUseRelSize(
                KDChartParams::HdFtPosHeader2 ) ) {
            actFont.setPointSizeFloat(
                params()->headerFooterFontRelSize(
                    KDChartParams::HdFtPosHeader2 ) * averageValueP1000 );
        }
        QFontMetrics header2Metrics( actFont );
        /* end of new code */

        if ( !hasHeader ) {
            headerLineLeading = header2Metrics.lineSpacing() / 2;
        }
        yposTop += headerLineLeading;
        _header2Rect = QRect( xposLeft, yposTop, xposRight-xposLeft,
                              header2Metrics.height() );

        // this command line is ONLY A TEMPORARY HACK - later not needed anymore
        ((KDChartParams*)params())->__internalStoreHdFtRect( KDChartParams::HdFtPosHeader2A, _header2Rect );

        yposTop += _header2Rect.height();
        hasHeader = true;
    }


    if ( !params()->footerText().isEmpty() ) {

        /* old code:
       QFontMetrics footerMetrics( params()->footerFont() );
       end of old code */

        /* new code: QUICK AND DIRTY DUE TO LIMITED TIME */
        QFont actFont( params()->footerFont() );
        if ( params()->headerFooterFontUseRelSize(
                                                  KDChartParams::HdFtPosFooter ) ) {
            actFont.setPointSizeFloat(
                                      params()->headerFooterFontRelSize(
                                                                        KDChartParams::HdFtPosFooter ) * averageValueP1000 );
        }
        QFontMetrics footerMetrics( actFont );
        /* end of new code */

        _footerRect = QRect( xposLeft, yposBottom - footerMetrics.height(),
                             xposRight-xposLeft,
                             footerMetrics.height() );

        // this command line is ONLY A TEMPORARY HACK - later not needed anymore
        ((KDChartParams*)params())->__internalStoreHdFtRect( KDChartParams::HdFtPosFooter, _footerRect );

        yposBottom -= _footerRect.height();
    }


    // Calculate legend position. First check whether there is going
    // to be a legend at all:
    if ( params()->legendPosition() != KDChartParams::NoLegend ) {
        // Now calculate the size needed for the legend
        findLegendTexts( data );

        bool hasLegendTitle = false;
        if ( params()->legendTitleText() != QString::null &&
             params()->legendTitleText() != "" )
            hasLegendTitle = true;

        int legendTitleWidth = 0;
        if( _legendTitle )
            delete _legendTitle;
        _legendTitle = 0;
        if ( hasLegendTitle ) {
            actLegendTitleFont = params()->legendTitleFont();
            if ( params()->legendTitleFontUseRelSize() ) {
                int nTxtHeight =
                    static_cast < int > ( params()->legendTitleFontRelSize()
                                          * averageValueP1000 );
                actLegendTitleFont.setPointSizeFloat( nTxtHeight );
            }

            _legendTitle = new KDChartTextPiece( params()->legendTitleText(),
                                                 actLegendTitleFont );
            //            _legendTitleHeight = _legendTitle->height();
            legendTitleWidth = _legendTitle->width();
        }

        actLegendFont = params()->legendFont();
        if ( params()->legendFontUseRelSize() ) {
            int nTxtHeight =
                static_cast < int > ( params()->legendFontRelSize()
                                      * averageValueP1000 );
            actLegendFont.setPointSizeFloat( nTxtHeight );
        }
        QFontMetrics legendMetrics( actLegendFont );
        _legendSpacing = legendMetrics.lineSpacing();
        _legendHeight = legendMetrics.height();
        _legendLeading = legendMetrics.leading();

        int sizeX = 0;
        int sizeY = static_cast < int > ( _legendSpacing * (0.5 + _numLegendTexts) );
        // add space for the legend title if any was set
        if ( hasLegendTitle )
            sizeY += static_cast < int > ( _legendTitle->height() * 1.15 );

        int dataset;
        for ( dataset = 0; dataset < _numLegendTexts; dataset++ ) {
            sizeX = QMAX( sizeX, legendMetrics.width( _legendTexts[ dataset ] ) );
        }

        // assume 4 em spaces: before the color box, the color box, after the
        // color box and after the legend text
        _legendEMSpace = legendMetrics.width( 'M' );
        sizeX += ( _legendEMSpace * 4 );

        // We cannot setup the title width earlier as the title does
        // not have a color box. The two em spaces are before the
        // color box (where the title does not start yet, it is
        // left-aligned with the color boxes) and after the title (to
        // have some space before the boundary line comes).
        sizeX = QMAX( sizeX, legendTitleWidth + _legendEMSpace*2 );

        switch ( params()->legendPosition() ) {
        case KDChartParams::LegendTop:
            if ( headerLineLeading )
                yposTop += QMAX( (int)params()->legendSpacing(), headerLineLeading );
            _legendRect = QRect( xposLeft + ( (xposRight-xposLeft) - sizeX ) / 2,
                                 yposTop, sizeX, sizeY );
            yposTop = _legendRect.bottom() + params()->legendSpacing();
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
            _legendRect = QRect( xposLeft, ( yposBottom - yposTop - sizeY ) / 2 +
                                 yposTop,
                                 sizeX, sizeY );
            xposLeft = _legendRect.right() + params()->legendSpacing();
            break;
        case KDChartParams::LegendRight:
            if ( params()->showGrid() )
                yposTop += headerLineLeading;
            _legendRect = QRect( xposRight - sizeX,
                                 ( yposBottom - yposTop - sizeY ) / 2 + yposTop,
                                 sizeX, sizeY );
            xposRight = _legendRect.left() - params()->legendSpacing();
            break;
        case KDChartParams::LegendTopLeft:
            if ( headerLineLeading )
                yposTop += QMAX( (int)params()->legendSpacing(), headerLineLeading );
            _legendRect = QRect( xposLeft, yposTop, sizeX, sizeY );
            yposTop = _legendRect.bottom() + params()->legendSpacing();
            xposLeft = _legendRect.right() + params()->legendSpacing();
            break;
        case KDChartParams::LegendTopLeftTop:
            if ( headerLineLeading )
                yposTop += QMAX( (int)params()->legendSpacing(), headerLineLeading );
            _legendRect = QRect( xposLeft, yposTop, sizeX, sizeY );
            yposTop = _legendRect.bottom() + params()->legendSpacing();
            break;
        case KDChartParams::LegendTopLeftLeft:
            if ( headerLineLeading )
                yposTop += QMAX( (int)params()->legendSpacing(), headerLineLeading );
            _legendRect = QRect( xposLeft, yposTop, sizeX, sizeY );
            xposLeft = _legendRect.right() + params()->legendSpacing();
            break;
        case KDChartParams::LegendTopRight:
            if ( headerLineLeading )
                yposTop += QMAX( (int)params()->legendSpacing(), headerLineLeading );
            _legendRect = QRect( xposRight - sizeX,
                                 yposTop, sizeX, sizeY );
            yposTop = _legendRect.bottom() + params()->legendSpacing();
            xposRight = _legendRect.left() - params()->legendSpacing();
            break;
        case KDChartParams::LegendTopRightTop:
            if ( headerLineLeading )
                yposTop += QMAX( (int)params()->legendSpacing(), headerLineLeading );
            _legendRect = QRect( xposRight - sizeX,
                                 yposTop, sizeX, sizeY );
            yposTop = _legendRect.bottom() + params()->legendSpacing();
            break;
        case KDChartParams::LegendTopRightRight:
            if ( headerLineLeading )
                yposTop += QMAX( (int)params()->legendSpacing(), headerLineLeading );
            _legendRect = QRect( xposRight - sizeX,
                                 yposTop, sizeX, sizeY );
            xposRight = _legendRect.left() - params()->legendSpacing();
            break;
        case KDChartParams::LegendBottomLeft:
            if ( params()->showGrid() )
                yposTop += headerLineLeading;
            _legendRect = QRect( xposLeft, yposBottom - sizeY, sizeX, sizeY );
            yposBottom = _legendRect.top() - params()->legendSpacing();
            xposLeft = _legendRect.right() + params()->legendSpacing();
            break;
        case KDChartParams::LegendBottomLeftBottom:
            if ( params()->showGrid() )
                yposTop += headerLineLeading;
            _legendRect = QRect( xposLeft, yposBottom - sizeY, sizeX, sizeY );
            yposBottom = _legendRect.top() - params()->legendSpacing();
            break;
        case KDChartParams::LegendBottomLeftLeft:
            if ( params()->showGrid() )
                yposTop += headerLineLeading;
            _legendRect = QRect( xposLeft, yposBottom - sizeY, sizeX, sizeY );
            xposLeft = _legendRect.right() + params()->legendSpacing();
            break;
        case KDChartParams::LegendBottomRight:
            if ( params()->showGrid() )
                yposTop += headerLineLeading;
            _legendRect = QRect( xposRight - sizeX,
                                 yposBottom - sizeY, sizeX, sizeY );
            yposBottom = _legendRect.top() - params()->legendSpacing();
            xposRight = _legendRect.left() - params()->legendSpacing();
            break;
        case KDChartParams::LegendBottomRightBottom:
            if ( params()->showGrid() )
                yposTop += headerLineLeading;
            _legendRect = QRect( xposRight - sizeX,
                                 yposBottom - sizeY, sizeX, sizeY );
            yposBottom = _legendRect.top() - params()->legendSpacing();
            break;
        case KDChartParams::LegendBottomRightRight:
            if ( params()->showGrid() )
                yposTop += headerLineLeading;
            _legendRect = QRect( xposRight - sizeX,
                                 yposBottom - sizeY, sizeX, sizeY );
            xposRight = _legendRect.left() - params()->legendSpacing();
            break;
        default:
            // Should not be able to happen
            qDebug( "KDChart: Unknown legend position" );
        }
    }

    _axesRect = QRect( xposLeft, yposTop,
                       xposRight - xposLeft,
                       yposBottom - yposTop );

    // important rule: do *not* calculate axes areas for Polar charts!
    //                 (even if left and bottom axes might be set active)
    if( KDChartParams::Polar == params()->chartType() ) {
        _dataRect = _axesRect;
    } else {
        int nAxesLeft0   = _axesRect.left();
        int nAxesRight0  = trueWidth - _axesRect.right();
        int nAxesTop0    = _axesRect.top();
        int nAxesBottom0 = trueHeight - _axesRect.bottom();

        int nAxesLeft    = nAxesLeft0;
        int nAxesRight   = nAxesRight0;
        int nAxesTop     = nAxesTop0;
        int nAxesBottom  = nAxesBottom0;
        int i;
        for ( i = 0; i < (int)KDChartParams::KDCHART_MAX_AXES; ++i ) {

            const KDChartAxisParams& para = params()->axisParams( i );
            int areaSize = 0;

            if ( para.axisVisible()
                && KDChartAxisParams::AxisTypeUnknown != para.axisType() ) {

                const KDChartAxisParams::AxisPos
                    basicPos( KDChartAxisParams::basicAxisPos( i ) );

                int areaMin = para.axisAreaMin();
                int areaMax = para.axisAreaMin();

                switch ( basicPos ) {
                case KDChartAxisParams::AxisPosBottom:
                case KDChartAxisParams::AxisPosTop:
                    if ( 0 > areaMin )
                        areaMin = static_cast < int > (
                                                    -0.75 * averageValueP1000 * areaMin );
                    // make sure areaMin will not be too small
                    // for the label texts
                    if ( para.axisLabelsVisible() ) {
                        int fntHeight;
                        if ( para.axisLabelsFontUseRelSize() )
                            fntHeight = static_cast < int > (
                                                            para.axisLabelsFontRelSize()
                                                            * averageValueP1000 );
                        else {
                            QFontMetrics metrics( para.axisLabelsFont() );
                            fntHeight = metrics.height();
                        }
                        areaMin = QMAX( areaMin, 3 * fntHeight );
                    }
                    if ( 0 > areaMax )
                        areaMax = static_cast < int > ( -1.0 * averageValueP1000
                                                        * areaMax );
                    break;
                case KDChartAxisParams::AxisPosLeft:
                case KDChartAxisParams::AxisPosRight:
                    if ( 0 > areaMin )
                        areaMin = static_cast < int > ( -1.0 * averageValueP1000
                                                        * areaMin );
                    if ( 0 > areaMax )
                        areaMax = static_cast < int > ( -1.0 * averageValueP1000
                                                        * areaMax );
                    break;
                default:
                    break;
                }

                switch ( para.axisAreaMode() ) {
                case KDChartAxisParams::AxisAreaModeAutoSize: {
                    qDebug( "Sorry, not implemented: AxisAreaModeAutoSize" );
                }

                //
                //
                //   F E A T U R E   P L A N N E D   F O R   F U T U R E . . .
                //
                //

                // break;

                case KDChartAxisParams::AxisAreaModeMinMaxSize: {
                    qDebug( "Sorry, not implemented: AxisAreaModeMinMaxSize" );
                }

                //
                //
                //   F E A T U R E   P L A N N E D   F O R   F U T U R E . . .
                //
                //

                // break;

                case KDChartAxisParams::AxisAreaModeFixedSize: {
                    areaSize = areaMin;
                }
                break;
                }

                switch ( basicPos ) {
                case KDChartAxisParams::AxisPosBottom:
                    nAxesBottom += areaSize;
                    break;
                case KDChartAxisParams::AxisPosLeft:
                    nAxesLeft += areaSize;
                    break;
                case KDChartAxisParams::AxisPosTop:
                    nAxesTop += areaSize;
                    break;
                case KDChartAxisParams::AxisPosRight:
                    nAxesRight += areaSize;
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
        nAxesBottom = QMAX( nAxesBottom, nMinDistance );
        nAxesLeft = QMAX( nAxesLeft, nMinDistance );
        nAxesTop = QMAX( nAxesTop, nMinDistance );
        nAxesRight = QMAX( nAxesRight, nMinDistance );

        int nBottom = params()->axisParams( KDChartAxisParams::AxisPosBottom ).axisTrueAreaSize();
        int nLeft = params()->axisParams( KDChartAxisParams::AxisPosLeft ).axisTrueAreaSize();
        int nTop = params()->axisParams( KDChartAxisParams::AxisPosTop ).axisTrueAreaSize();
        int nRight = params()->axisParams( KDChartAxisParams::AxisPosRight ).axisTrueAreaSize();
        int nBottom2 = params()->axisParams( KDChartAxisParams::AxisPosBottom2 ).axisTrueAreaSize();
        int nLeft2 = params()->axisParams( KDChartAxisParams::AxisPosLeft2 ).axisTrueAreaSize();
        int nTop2 = params()->axisParams( KDChartAxisParams::AxisPosTop2 ).axisTrueAreaSize();
        int nRight2 = params()->axisParams( KDChartAxisParams::AxisPosRight2 ).axisTrueAreaSize();

        _params->setAxisArea( KDChartAxisParams::AxisPosBottom,
                            QRect( nAxesLeft,
                                    trueHeight - nAxesBottom,
                                    trueWidth - nAxesLeft - nAxesRight + 1,
                                    nBottom ) );
        _params->setAxisArea( KDChartAxisParams::AxisPosLeft,
                            QRect( nAxesLeft0 + nLeft2,
                                    nAxesTop,
                                    nLeft,
                                    trueHeight - nAxesTop - nAxesBottom + 1 ) );

        _params->setAxisArea( KDChartAxisParams::AxisPosTop,
                            QRect( nAxesLeft,
                                    nAxesTop0 + nTop2,
                                    trueWidth - nAxesLeft - nAxesRight + 1,
                                    nTop ) );
        _params->setAxisArea( KDChartAxisParams::AxisPosRight,
                            QRect( trueWidth - nAxesRight,
                                    nAxesTop,
                                    nRight,
                                    trueHeight - nAxesTop - nAxesBottom + 1 ) );

        _params->setAxisArea( KDChartAxisParams::AxisPosBottom2,
                            QRect( nAxesLeft,
                                    trueHeight - nAxesBottom + nBottom,
                                    trueWidth - nAxesLeft - nAxesRight + 1,
                                    nBottom2 ) );
        _params->setAxisArea( KDChartAxisParams::AxisPosLeft2,
                            QRect( nAxesLeft0,
                                    nAxesTop,
                                    nLeft2,
                                    trueHeight - nAxesTop - nAxesBottom + 1 ) );

        _params->setAxisArea( KDChartAxisParams::AxisPosTop2,
                            QRect( nAxesLeft,
                                    nAxesTop0,
                                    trueWidth - nAxesLeft - nAxesRight + 1,
                                    nTop2 ) );
        _params->setAxisArea( KDChartAxisParams::AxisPosRight2,
                            QRect( trueWidth - nAxesRight + nRight,
                                    nAxesTop,
                                    nRight2,
                                    trueHeight - nAxesTop - nAxesBottom + 1 ) );

        _dataRect = QRect( nAxesLeft,
                        nAxesTop,
                        trueWidth - nAxesLeft - nAxesRight + 1,
                        trueHeight - nAxesTop - nAxesBottom + 1 );
    }
}


/**
   This method implements the algorithm to find the texts for the legend.
*/
void KDChartPainter::findLegendTexts( KDChartTableDataBase* data )
{
    uint dataset;
    switch ( params()->legendSource() ) {
    case KDChartParams::LegendManual: {
        // The easiest case: Take manually set strings, no matter whether any
        // have been set.
        for ( dataset = 0; dataset < data->usedRows(); dataset++ )
            _legendTexts[ dataset ] = params()->legendText( dataset );
        _numLegendTexts = data->usedRows();
        break;
    }
    case KDChartParams::LegendFirstColumn: {
        // Take whatever is in the first column
        for ( dataset = 0; dataset < data->usedRows(); dataset++ )
            _legendTexts[ dataset ] = data->cell( dataset, 0 ).stringValue();
        _numLegendTexts = data->usedRows();
        break;
    }
    case KDChartParams::LegendAutomatic: {
        // First, try the first row
        bool notfound = false;
        _numLegendTexts = data->usedRows(); // assume this for cleaner
        // code below
        for ( dataset = 0; dataset < data->usedRows(); dataset++ ) {
            _legendTexts[ dataset ] = data->cell( dataset, 0 ).stringValue();
            if ( _legendTexts[ dataset ].isEmpty() || _legendTexts[ dataset ].isNull() )
                notfound = true;
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
