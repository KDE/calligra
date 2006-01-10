/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klar�vdalens Datakonsult AB.  All rights reserved.
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

/**
  \dontinclude KDChartPainter.h
  */
#include <KDChart.h>
#include <KDChartPainter.h>
#include <KDChartParams.h>
#include <KDChartGlobal.h>
#include <KDChartAxisParams.h>

#include <qglobal.h>
#include <qpainter.h>
#include <qpaintdevice.h>
#include <qpaintdevicemetrics.h>

/**
  \class KDChart KDChart.h

  \brief Provides a single entry-point to the charting engine for
  applications that wish to provide their own QPainter.

  It is not useful to instantiate this class as it contains
  static methods only.

  \note If for some reason you are NOT using the
  KDChartWidget class but calling the painting methods of KDChart directly,
  you probably will also use the KDChartDataRegionList class:
  This class is derived from QPtrList, so all of the Qt documentation
  for this class is valid for KDChartDataRegionList too, e.g. freeing
  of the pointers stored can either be done automatically or
  manually - so PLEASE take the time to read the reference information for this class!

  \sa KDChartWidget, KDChartDataRegionList
  */

KDChartParams* KDChart::oldParams = 0;
KDChartPainter* KDChart::cpainter = 0;
KDChartPainter* KDChart::cpainter2 = 0;
KDChartParams::ChartType KDChart::cpainterType = KDChartParams::NoType;
KDChartParams::ChartType KDChart::cpainterType2 = KDChartParams::NoType;

/**
  A global function that cleans up possible KDChartPainter objects at
  application shutdown.
  */
void cleanupPainter();


bool hasCartesianAxes( KDChartParams::ChartType chartType )
{
    switch( chartType ){
        case KDChartParams::NoType:     return false;
        case KDChartParams::Bar:        return true;
        case KDChartParams::Line:       return true;
        case KDChartParams::Area:       return true;
        case KDChartParams::Pie:        return false;
        case KDChartParams::HiLo:       return true;
        case KDChartParams::Ring:       return false;
        case KDChartParams::Polar:      return false; // Polar axes are NO cartesian axes!
        case KDChartParams::BoxWhisker: return true;
        default:
                                        qDebug("\n\n\n\nKDCHART ERROR: Type missing in KDChart.cpp hasCartesianAxes()\n"
                                                "=============================================================\n"
                                                "=============================================================\n\n\n\n");
    }
    return false;
}


/**
  Calculates the drawing area from a given QPainter.

  Use this function to get a QRect that you may pass to
  KDChart::setupGeometry() if you need to know the positions and
  sizes of the axis areas and/or the data area *before* drawing
  the chart.  After calling KDChart::setupGeometry() you may use
  KDChartParams::axisArea() and/or KDChartParams::dataArea()
  to retrieve the desired information.

  \return True if the painter was valid and the drawing area
  could be calculated successfully, else false.
  */
bool KDChart::painterToDrawRect( QPainter* painter, QRect& drawRect )
{
    if( painter ){
        QPaintDeviceMetrics painterMetrics( painter->device() );
        drawRect = QRect( 0, 0, painterMetrics.width(), painterMetrics.height() );
        drawRect.setWidth(  drawRect.width() -2 );
        drawRect.setHeight( drawRect.height()-2 );
        return true;
    }else{
        drawRect = QRect( QPoint(0,0), QSize(0,0) );
        qDebug("ERROR: KDChartPainter::painterToDrawRect() was called with *no* painter.");
        return false;
    }
}


/**
  Calculates the axis and data area rects of a chart with the
  specified parameters on the specified painter.

  \note Call this function if you need to know the positions and
  sizes of the axis areas and/or the data area *before* drawing
  the chart.  After calling this function you may use
  KDChartParams::axisArea() and/or KDChartParams::dataArea()
  to retrieve the desired information.

  To get the right drawing area from a given QPainter please
  use the static method KDChart::painterToDrawRect().

  \param painter the painter that is eventually to be used for drawing
  \param params the parameters defining the chart
  \param data the data that should be displayed as a chart
  \param drawRect the position and size of the drawing area to be used
  */
bool KDChart::setupGeometry( QPainter* painter,
                             KDChartParams* params,
                             KDChartTableDataBase* data,
                             const QRect& drawRect )
{
//qDebug("INVOKING: KDChart::setupGeometry()");
    if( !params ){
        qDebug("ERROR: setupGeometry::paint() was called with *no* params.");
        return false;
    }
    if( !data ){
        qDebug("ERROR: setupGeometry::paint() was called with *no* data.");
        return false;
    }
    // don't crash due to memory problems when running on windows
#ifdef Q_WS_WIN
    QPixmap::setDefaultOptimization(QPixmap::MemoryOptim);
#endif

    // Install a cleanup routine that is called when the Qt
    // application shuts down and cleans up any potentially still
    // existing painters. Only do this once.
    static bool bFirstCleanUpInstall = true;
    if( bFirstCleanUpInstall ) {
        bFirstCleanUpInstall = false;
        qAddPostRoutine( cleanupPainter );
    }

    // Check whether last call of this methode gave us the same params pointer.
    // If params changed we must create new painter(s).
    bool paramsHasChanged = ( params != oldParams );
    if( paramsHasChanged )
        oldParams = params;

    // Check whether there already is painter and, if that is the
    // case, whether the painter still has the correct type (the chart
    // type might have changed in the meantime).
    if ( paramsHasChanged || !cpainter || cpainterType != params->chartType() )
    {
        delete cpainter; /* save, since always 0 if there was not yet
                            a chart painter */
        // create a new painter
        cpainter = KDChartPainter::create( params, false );
        cpainterType = params->chartType();
    }

    // Check whether there already is a 2nd painter and, if that is the
    // case, whether the painter still has the correct type (the
    // additional chart type might have changed in the meantime).
    if ( paramsHasChanged || !cpainter2 || cpainterType2 != params->additionalChartType() )
    {
        delete cpainter2; /* save, since always 0 if there was not yet
                             a chart painter */
        // create a new painter
        if( hasCartesianAxes( params->chartType() )
                && hasCartesianAxes( params->additionalChartType() ) ){
            cpainter2 = KDChartPainter::create( params, true );
            cpainterType2 = params->additionalChartType();
        }else{
            cpainter2 = 0;
            cpainterType2 = KDChartParams::NoType;
        }
    }

    if ( cpainter ){  // can be 0 if no exceptions are used
        cpainter->setupGeometry( painter, data, drawRect );
    }

    if ( cpainter2 ){  // can be 0 if no exceptions are used
        cpainter2->setupGeometry( painter, data, drawRect );
    }

    return true;
}

/**
  Paints a chart with the specified parameters on the specified
  painter.

  \param painter the QPainter onto which the chart should be painted
  \param params the parameters defining the chart
  \param data the data that should be displayed as a chart
  \param regions if not null, this points to a
  KDChartDataRegionList that will be filled with the regions
  of the data segments. This information is needed internally for both
  recognizing the data segment when reporting mouse clicks and
  for finding the correct position to draw the respective data value texts.
  \param rect the position and size of the drawing area to be used,
  if this parameter is zero the painter's device metrics will be used.
  \param mustCalculateGeometry may be set to false if paint() is called
  immediately after a previous call of setupGeometry() to save some
  time in case you have specified a lot of data cells.
  */
void KDChart::paint( QPainter*              painter,
                     KDChartParams*         paraParams,
                     KDChartTableDataBase*  paraData,
                     KDChartDataRegionList* regions,
                     const QRect*           rect,
                     bool                   mustCalculateGeometry )
{
//qDebug("KDChart::paint() mustCalculateGeometry: "+QString(mustCalculateGeometry?"TRUE":"FALSE") );
#if defined KDAB_EVAL
    EvalDialog::checkEvalLicense( "KD Chart" );
#endif

    KDChartParams*        params = paraParams;
    KDChartTableDataBase* data   = paraData;
    if( !paraParams && !paraData ){
        qDebug("-----");
        qDebug("Note:  KDChart::paint() was called without \"params\" and without \"data\".");
        qDebug("-----  Showing a default bar chart.");
        params = new KDChartParams();
        params->setDatasetGap(3 * params->valueBlockGap());
        params->setPrintDataValues( false );
        params->setLegendPosition( KDChartParams::NoLegend );
        params->setAxisLabelsVisible( KDChartAxisParams::AxisPosBottom, false );
        params->setAxisShowGrid( KDChartAxisParams::AxisPosBottom, false );
        params->setHeader1Text( "KDChartWidget" );
        data = new KDChartTableData( 3, 1 );
        // 1st series
        data->setCell( 0, 0,    12.5   );
        // 2nd series
        data->setCell( 1, 0,     8.0   );
        // 3rd series
        data->setCell( 2, 0,    15.0   );
    }

    QRect drawRect;
    bool bOk = true;
    if( mustCalculateGeometry || !cpainter || cpainter->outermostRect().isNull() ){
        if( rect )
            drawRect = *rect;
        else if( !painterToDrawRect( painter, drawRect ) ){
            qDebug("ERROR: KDChart::paint() could not calculate a drawing area.");
            bOk = false;
        }
        //qDebug("xxx" );
        if( (params || data) && !setupGeometry( painter, params, data, drawRect ) ){
            qDebug("ERROR: KDChart::paint() could not calculate the chart geometry.");
            bOk = false;
        }
    }else{
        drawRect = cpainter->outermostRect();
    }

    //qDebug("yyy" );

    if( bOk ){
        // Note: the following *must* paint the main-chart first
        //       and the additional chart afterwards
        //       since all axes computations are only done when
        //       the first chart is painted but will be needed for both of course.
        //
        bool paintFirst = true;
        bool paintLast  = ! ( cpainter && cpainter2 );
        if ( cpainter ) {  // can be 0 if no exceptions are used
            //qDebug("zzz" );
            cpainter->paint( painter, data, paintFirst, paintLast, regions, &drawRect, false );

            paintFirst = false;
        }
        paintLast = true;
        if ( cpainter2 )   // can be 0 if no exceptions are used
            cpainter2->paint( painter, data, paintFirst, paintLast, regions, &drawRect, false );
    }

    if( !paraParams && !paraData ){
        delete params;
        delete data;
    }
    KDChartAutoColor::freeInstance(); // stuff that memory leak
}


/**
  Paints a chart with the specified parameters on the specified
  painter which should use a QPrinter as it's output device.

  This method is provided for your convenience, it behaves
  like the paint() method described above but additionally
  it takes care for the output mode flag: Before painting is
  started the internal optimizeOutputForScreen flag is set
  to FALSE and after painting is done it is restored to
  it's previous value.

  \sa paint
  */
void KDChart::print( QPainter* painter, KDChartParams* params,
        KDChartTableDataBase* data,
        KDChartDataRegionList* regions,
        const QRect* rect,
        bool mustCalculateGeometry )
{
    bool oldOpt=true;
    if( params ){
        oldOpt = params->optimizeOutputForScreen();
        params->setOptimizeOutputForScreen( false );
    }
    paint( painter, params, data, regions, rect, mustCalculateGeometry );
    if( params )
        params->setOptimizeOutputForScreen( oldOpt );
}


/*
   This method is called at application shut-down and cleans up the
   last created painter.
   */
void cleanupPainter()
{
    delete KDChart::cpainter;
    delete KDChart::cpainter2;
    KDChart::oldParams = 0;
}
