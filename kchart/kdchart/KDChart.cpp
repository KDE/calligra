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
#include <KDChart.h>
#include <KDChartPainter.h>
#include <KDChartParams.h>

#include <qglobal.h>

/**
   \class KDChart KDChart.h

   \brief Provides a single entry-point to the charting engine for
   applications that wish to provide their own QPainter.

   It is not useful to instantiate this class as it only contains one
   static method.
*/

KDChartParams* KDChart::oldParams = 0;
KDChartPainter* KDChart::cpainter = 0;
KDChartPainter* KDChart::cpainter2 = 0;
KDChartParams::ChartType KDChart::cpainterType = KDChartParams::NoType;
KDChartParams::ChartType KDChart::cpainterType2 = KDChartParams::NoType;

void cleanupPainter();

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
   \throw KDChartUnknownTypeException if there is no registered
   KDChartPainter subclass for the type specified in \a params. This
   can only happen with user-defined chart types.
*/
void KDChart::paint( QPainter* painter, KDChartParams* params,
                     KDChartTableDataBase* data,
                     KDChartDataRegionList* regions,
                     const QRect* rect )
#ifdef USE_EXCEPTIONS
throw( KDChartUnknownTypeException )
#endif
{
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
            if ( ( KDChartParams::Bar == params->chartType()
                   && KDChartParams::Line == params->additionalChartType() )
                 || ( KDChartParams::Line == params->chartType()
                      && KDChartParams::Bar == params->additionalChartType() ) ) {
                cpainter2 = KDChartPainter::create( params, true );
                cpainterType2 = params->additionalChartType();
            } else {
                cpainter2 = 0;
                cpainterType2 = KDChartParams::NoType;
            }
        }

    if ( regions )
        regions->clear();

    // Note: the following *must* paint the main-chart first
    //       and the additional chart afterwards
    //       since all axes computations are only done when
    //       the first chart is painted but will be needed for both of course.
    //
    bool paintFirst = true;
    bool paintLast  = ! ( cpainter && cpainter2 );
    if ( cpainter ) {  // can be 0 if no exceptions are used
        cpainter->paint( painter, data, paintFirst, paintLast, regions, rect );

        paintFirst = false;
    }
    paintLast = true;
    if ( cpainter2 )   // can be 0 if no exceptions are used
        cpainter2->paint( painter, data, paintFirst, paintLast, regions, rect );
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

