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
   Library General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

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

KDChartPainter* KDChart::cpainter = 0;
KDChartPainter* KDChart::cpainter2 = 0;
KDChartParams::ChartType KDChart::cpainterType = KDChartParams::NoType;
KDChartParams::ChartType KDChart::cpainterType2 = KDChartParams::NoType;

/**
   Paints a chart with the specified parameters on the specified
   painter.

   \param painter the QPainter onto which the chart should be painted
   \param params the parameters defining the chart
   \param data the data that should be displayed as a chart
   \param regions if not null, this points to a
   KDChartDataRegionList that will be filled with the regions
   of the data segments.
   \throw KDChartUnknownTypeException if there is no registered
   KDChartPainter subclass for the type specified in \a params. This
   can only happen with user-defined chart types.
*/
void KDChart::paint( QPainter* painter, KDChartParams* params,
                     KDChartTableData* data,
                     KDChartDataRegionList* regions )
#ifdef USE_EXCEPTIONS
throw( KDChartUnknownTypeException )
#endif
{
    // Check whether there already is painter and, if that is the
    // case, whether the painter still has the correct type (the chart
    // type might have changed in the meantime).
    if ( !cpainter || cpainterType != params->chartType() )
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
    if ( !cpainter2 || cpainterType2 != params->additionalChartType() )
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
    if ( cpainter )   // can be 0 if no exceptions are used
        cpainter->paint( painter, data, false, regions );
    if ( cpainter2 )  // can be 0 if no exceptions are used
        cpainter2->paint( painter, data, true, regions );
}



/*
          A static object that ensures that the last created painter is cleaned up.
        */
class KDChartCleanup
{
public:
    ~KDChartCleanup()
{
    delete KDChart::cpainter;
}
};

KDChartCleanup cleanup;
