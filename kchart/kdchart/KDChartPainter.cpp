/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <KDChartPainter.h>
#include <KDChartParams.h>
#include <KDChartTable.h>
#include <KDChartUnknownTypeException.h>
#include <KDChartNotEnoughSpaceException.h>
#include <KDChartBarPainter.h>
#include <KDChartAreaPainter.h>
#include <KDChartLinesPainter.h>
#include <KDChartPiePainter.h>
#include <KDChartRingPainter.h>
#include <KDChartHiLoPainter.h>
#include <KDChartTextPiece.h>

#include <qpainter.h>
#include <qpaintdevicemetrics.h>

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
    _params( params ),
    _legendTitle( 0 )
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
void KDChartPainter::registerPainter( const QString& painterName,
                                      KDChartPainter* painter )
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
void KDChartPainter::unregisterPainter( const QString& painterName )
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
                            KDChartTableData* data,
                            bool paint2nd,
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
    if ( !paint2nd ) {
        qDebug( "KDChartPainter::paint() 1" );
        paintHeader( painter, data, rect );
        qDebug( "KDChartPainter::paint() 2" );
        paintFooter( painter, data, rect );
        qDebug( "KDChartPainter::paint() 3" );
        paintLegend( painter, data, actLegendFont, actLegendTitleFont );
        qDebug( "KDChartPainter::paint() 4" );
        paintAxes( painter, data );
        qDebug( "KDChartPainter::paint() 5" );
    }
    qDebug( "KDChartPainter::paint() 6" );
    paintData( painter, data, paint2nd, regions );
    qDebug( "KDChartPainter::paint() 7" );
}


/**
   Paints the axes for the chart. The implementation in KDChartPainter
   does nothing; subclasses for chart types that have axes will
   provide the appropriate drawing code here. This method serves as a
   fallback for chart types that do not have axes (like pies).

   \param painter the QPainter onto which the chart should be drawn
   \param data the data that will be displayed as a chart
*/
void KDChartPainter::paintAxes( QPainter* painter, KDChartTableData* data )
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
                                  KDChartTableData* data,
                                  const QFont& actLegendFont,
                                  const QFont& actLegendTitleFont )
{
    if ( params()->legendPosition() == KDChartParams::NoLegend )
        return ; // do not draw legend

    painter->save();
    // start out with a rectangle around the legend
    painter->setPen( QPen( Qt::black, 1 ) );
    painter->setBrush( QBrush::NoBrush );
    painter->drawRect( _legendRect );

    uint xpos = _legendRect.left() + _legendEMSpace,
         ypos = _legendRect.top();

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
    for ( uint dataset = 0; dataset < _numLegendTexts; dataset++ ) {
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
void KDChartPainter::paintHeader( QPainter* painter, KDChartTableData* data,
                                  const QRect* rect )
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
    QPaintDeviceMetrics metrics( painter->device() );

    QRect drawRect;
    if( rect )
        drawRect = *rect;
    else
        drawRect = QRect( 0, 0, metrics.width(), metrics.height() );
    
    double areaWidthP1000 = drawRect.width() / 1000.0;
    double areaHeightP1000 = drawRect.height() / 1000.0;
    double averageValueP1000 = ( areaWidthP1000 + areaHeightP1000 ) / 2.0;

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
void KDChartPainter::paintFooter( QPainter* painter, KDChartTableData* data,
                                  const QRect* rect )
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
    QPaintDeviceMetrics metrics( painter->device() );

    QRect drawRect;
    if( rect )
        drawRect = *rect;
    else
        drawRect = QRect( 0, 0, metrics.width(), metrics.height() );
    
    double areaWidthP1000 = drawRect.width() / 1000.0;
    double areaHeightP1000 = drawRect.height() / 1000.0;
    double averageValueP1000 = ( areaWidthP1000 + areaHeightP1000 ) / 2.0;

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
                                    KDChartTableData* data,
                                    QFont& actLegendFont,
                                    QFont& actLegendTitleFont,
                                    const QRect* rect )
#ifdef USE_EXCEPTIONS
throw( KDChartNotEnoughSpaceException )
#endif
{
    QPaintDeviceMetrics metrics( painter->device() );

    QRect drawRect;
    if( rect ) {
        drawRect = *rect;
    }
    else {
        drawRect = QRect( 0, 0,  metrics.width(), metrics.height() );
    }

    uint yposTop =  drawRect.top();
    uint xposLeft = drawRect.left();
    uint yposBottom = drawRect.height();
    uint xposRight = drawRect.width();

    double areaWidthP1000 = drawRect.width() / 1000.0;
    double areaHeightP1000 = drawRect.height() / 1000.0;
    double averageValueP1000 = ( areaWidthP1000 + areaHeightP1000 ) / 2.0;

    // Calculate header and footer positions: We only need to take the
    // vertical position into account since there will be nothing
    // neither to the left nor to the right of a header or footer.

    // KHZ->KD: I changed calculation of yposTop (or xposBottom, resp.)
    //          to enable support for different init values.
    //          (currently they are set to 0 (see above) but you never know)
    //
    //          Also optimized design:
    //            1. now min-header-leading is text height/2
    //            2. leading or legendSpacing (whichever is larger)
    //               will be added if legend is below the header(s)
    //            3. leading will be added between header and data area
    //               in case there is no top legend but grid is to be shown.
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
        _header1Rect = QRect( 0, yposTop, drawRect.width(),
                              header1Metrics.height() );
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
        _header2Rect = QRect( 0, yposTop,
                              drawRect.width(),
                              header2Metrics.height() );
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

        _footerRect = QRect( 0, drawRect.height() - footerMetrics.height(),
                             drawRect.width(),
                             footerMetrics.height() );
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

        uint legendTitleWidth = 0;
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

        uint sizeX = 0,
             sizeY = _legendSpacing * _numLegendTexts + _legendLeading;
        // add space for the legend title if any was set
        if ( hasLegendTitle )
            sizeY += _legendTitle->height();

        for ( uint dataset = 0; dataset < _numLegendTexts; dataset++ ) {
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
                yposTop += QMAX( params()->legendSpacing(), headerLineLeading );
            _legendRect = QRect( ( drawRect.width() - sizeX ) / 2,
                                 yposTop, sizeX, sizeY );
            yposTop = _legendRect.bottom() + params()->legendSpacing();
            break;
        case KDChartParams::LegendBottom:
            if ( params()->showGrid() )
                yposTop += headerLineLeading;
            _legendRect = QRect( ( drawRect.width() - sizeX ) / 2,
                                 yposBottom - sizeY,
                                 sizeX, sizeY );
            yposBottom = _legendRect.top() - params()->legendSpacing();
            break;
        case KDChartParams::LegendLeft:
            if ( params()->showGrid() )
                yposTop += headerLineLeading;
            _legendRect = QRect( 0, ( yposBottom - yposTop - sizeY ) / 2 +
                                 yposTop,
                                 sizeX, sizeY );
            xposLeft = _legendRect.right() + params()->legendSpacing();
            break;
        case KDChartParams::LegendRight:
            if ( params()->showGrid() )
                yposTop += headerLineLeading;
            _legendRect = QRect( drawRect.width() - sizeX,
                                 ( yposBottom - yposTop - sizeY ) / 2 + yposTop,
                                 sizeX, sizeY );
            xposRight = _legendRect.left() - params()->legendSpacing();
            break;
        case KDChartParams::LegendTopLeft:
            if ( headerLineLeading )
                yposTop += QMAX( params()->legendSpacing(), headerLineLeading );
            _legendRect = QRect( 0, yposTop, sizeX, sizeY );
            yposTop = _legendRect.bottom() + params()->legendSpacing();
            xposLeft = _legendRect.right() + params()->legendSpacing();
            break;
        case KDChartParams::LegendTopLeftTop:
            if ( headerLineLeading )
                yposTop += QMAX( params()->legendSpacing(), headerLineLeading );
            _legendRect = QRect( 0, yposTop, sizeX, sizeY );
            yposTop = _legendRect.bottom() + params()->legendSpacing();
            break;
        case KDChartParams::LegendTopLeftLeft:
            if ( headerLineLeading )
                yposTop += QMAX( params()->legendSpacing(), headerLineLeading );
            _legendRect = QRect( 0, yposTop, sizeX, sizeY );
            xposLeft = _legendRect.right() + params()->legendSpacing();
            break;
        case KDChartParams::LegendTopRight:
            if ( headerLineLeading )
                yposTop += QMAX( params()->legendSpacing(), headerLineLeading );
            _legendRect = QRect( drawRect.width() - sizeX,
                                 yposTop, sizeX, sizeY );
            yposTop = _legendRect.bottom() + params()->legendSpacing();
            xposRight = _legendRect.left() - params()->legendSpacing();
            break;
        case KDChartParams::LegendTopRightTop:
            if ( headerLineLeading )
                yposTop += QMAX( params()->legendSpacing(), headerLineLeading );
            _legendRect = QRect( drawRect.width() - sizeX,
                                 yposTop, sizeX, sizeY );
            yposTop = _legendRect.bottom() + params()->legendSpacing();
            break;
        case KDChartParams::LegendTopRightRight:
            if ( headerLineLeading )
                yposTop += QMAX( params()->legendSpacing(), headerLineLeading );
            _legendRect = QRect( drawRect.width() - sizeX,
                                 yposTop, sizeX, sizeY );
            xposRight = _legendRect.left() - params()->legendSpacing();
            break;
        case KDChartParams::LegendBottomLeft:
            if ( params()->showGrid() )
                yposTop += headerLineLeading;
            _legendRect = QRect( 0, yposBottom - sizeY, sizeX, sizeY );
            yposBottom = _legendRect.top() - params()->legendSpacing();
            xposLeft = _legendRect.right() + params()->legendSpacing();
            break;
        case KDChartParams::LegendBottomLeftBottom:
            if ( params()->showGrid() )
                yposTop += headerLineLeading;
            _legendRect = QRect( 0, yposBottom - sizeY, sizeX, sizeY );
            yposBottom = _legendRect.top() - params()->legendSpacing();
            break;
        case KDChartParams::LegendBottomLeftLeft:
            if ( params()->showGrid() )
                yposTop += headerLineLeading;
            _legendRect = QRect( 0, yposBottom - sizeY, sizeX, sizeY );
            xposLeft = _legendRect.right() + params()->legendSpacing();
            break;
        case KDChartParams::LegendBottomRight:
            if ( params()->showGrid() )
                yposTop += headerLineLeading;
            _legendRect = QRect( drawRect.width() - sizeX,
                                 yposBottom - sizeY, sizeX, sizeY );
            yposBottom = _legendRect.top() - params()->legendSpacing();
            xposRight = _legendRect.left() - params()->legendSpacing();
            break;
        case KDChartParams::LegendBottomRightBottom:
            if ( params()->showGrid() )
                yposTop += headerLineLeading;
            _legendRect = QRect( drawRect.width() - sizeX,
                                 yposBottom - sizeY, sizeX, sizeY );
            yposBottom = _legendRect.top() - params()->legendSpacing();
            break;
        case KDChartParams::LegendBottomRightRight:
            if ( params()->showGrid() )
                yposTop += headerLineLeading;
            _legendRect = QRect( drawRect.width() - sizeX,
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

    _logicalWidth = drawRect.width();
    _logicalHeight = drawRect.height();

    int nAxesLeft0 = _axesRect.left();
    int nAxesRight0 = _logicalWidth - _axesRect.right();
    int nAxesTop0 = _axesRect.top();
    int nAxesBottom0 = _logicalHeight - _axesRect.bottom();
    ;
    int nAxesLeft = nAxesLeft0;
    int nAxesRight = nAxesRight0;
    int nAxesTop = nAxesTop0;
    int nAxesBottom = nAxesBottom0;

    for ( uint i = 0; i < KDChartParams::KDCHART_MAX_AXES; ++i ) {

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
                        fntHeight = drawRect.height();
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
                                 _logicalHeight - nAxesBottom,
                                 _logicalWidth - nAxesLeft - nAxesRight + 1,
                                 nBottom ) );
    _params->setAxisArea( KDChartAxisParams::AxisPosLeft,
                          QRect( nAxesLeft0 + nLeft2,
                                 nAxesTop,
                                 nLeft,
                                 _logicalHeight - nAxesTop - nAxesBottom + 1 ) );

    _params->setAxisArea( KDChartAxisParams::AxisPosTop,
                          QRect( nAxesLeft,
                                 nAxesTop0 + nTop2,
                                 _logicalWidth - nAxesLeft - nAxesRight + 1,
                                 nTop ) );
    _params->setAxisArea( KDChartAxisParams::AxisPosRight,
                          QRect( _logicalWidth - nAxesRight,
                                 nAxesTop,
                                 nRight,
                                 _logicalHeight - nAxesTop - nAxesBottom + 1 ) );

    _params->setAxisArea( KDChartAxisParams::AxisPosBottom2,
                          QRect( nAxesLeft,
                                 _logicalHeight - nAxesBottom + nBottom,
                                 _logicalWidth - nAxesLeft - nAxesRight + 1,
                                 nBottom2 ) );
    _params->setAxisArea( KDChartAxisParams::AxisPosLeft2,
                          QRect( nAxesLeft0,
                                 nAxesTop,
                                 nLeft2,
                                 _logicalHeight - nAxesTop - nAxesBottom + 1 ) );

    _params->setAxisArea( KDChartAxisParams::AxisPosTop2,
                          QRect( nAxesLeft,
                                 nAxesTop0,
                                 _logicalWidth - nAxesLeft - nAxesRight + 1,
                                 nTop2 ) );
    _params->setAxisArea( KDChartAxisParams::AxisPosRight2,
                          QRect( _logicalWidth - nAxesRight + nRight,
                                 nAxesTop,
                                 nRight2,
                                 _logicalHeight - nAxesTop - nAxesBottom + 1 ) );

    _dataRect = QRect( nAxesLeft,
                       nAxesTop,
                       _logicalWidth - nAxesLeft - nAxesRight + 1,
                       _logicalHeight - nAxesTop - nAxesBottom + 1 );
}


/**
   This method implements the algorithm to find the texts for the legend.
*/
void KDChartPainter::findLegendTexts( KDChartTableData* data )
{
    switch ( params()->legendSource() ) {
    case KDChartParams::LegendManual: {
        // The easiest case: Take manually set strings, no matter whether any
        // have been set.
        for ( uint dataset = 0; dataset < data->usedRows(); dataset++ )
            _legendTexts[ dataset ] = params()->legendText( dataset );
        _numLegendTexts = data->usedRows();
        break;
    }
    case KDChartParams::LegendFirstColumn: {
        // Take whatever is in the first column
        for ( uint dataset = 0; dataset < data->usedRows(); dataset++ )
            _legendTexts[ dataset ] = data->cell( dataset, 0 ).stringValue();
        _numLegendTexts = data->usedRows();
        break;
    }
    case KDChartParams::LegendAutomatic: {
        // First, try the first row
        bool notfound = false;
        _numLegendTexts = data->usedRows(); // assume this for cleaner
        // code below
        for ( uint dataset = 0; dataset < data->usedRows(); dataset++ ) {
            _legendTexts[ dataset ] = data->cell( dataset, 0 ).stringValue();
            if ( _legendTexts[ dataset ].isEmpty() || _legendTexts[ dataset ].isNull() )
                notfound = true;
        }

        // If there were no entries for all the datasets, use the manually set
        // texts, and resort to Series 1, Series 2, ... where nothing has been
        // set.
        if ( notfound ) {
            for ( uint dataset = 0; dataset < numLegendFallbackTexts( data );
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
uint KDChartPainter::numLegendFallbackTexts( KDChartTableData* data ) const
{
    return data->usedRows();
}
