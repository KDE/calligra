/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2005-2007 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Chart licenses may use this file in
 ** accordance with the KD Chart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdchart for
 **   information about KD Chart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#include "KDChartLeveyJenningsDiagram.h"
#include "KDChartDataValueAttributes.h"

#include "KDChartLeveyJenningsDiagram_p.h"

using namespace KDChart;

LeveyJenningsDiagram::Private::Private( const Private& rhs )
    : LineDiagram::Private( rhs ),
      lotChangedPosition( rhs.lotChangedPosition ),
      fluidicsPackChangedPosition( rhs.fluidicsPackChangedPosition ),
      sensorChangedPosition( rhs.sensorChangedPosition ),
      fluidicsPackChanges( rhs.fluidicsPackChanges ),
      sensorChanges( rhs.sensorChanges ),
      scanLinePen( rhs.scanLinePen ),
      icons( rhs.icons ),
      expectedMeanValue( rhs.expectedMeanValue ),
      expectedStandardDeviation( rhs.expectedStandardDeviation )
{
}

void LeveyJenningsDiagram::Private::setYAxisRange() const
{
    CartesianCoordinatePlane* const plane = static_cast< CartesianCoordinatePlane* >( diagram->coordinatePlane() );
    if( plane == 0 )
        return;

    plane->setVerticalRange( QPair< qreal, qreal >( expectedMeanValue - 4 * expectedStandardDeviation, 
                                                    expectedMeanValue + 4 * expectedStandardDeviation ) );
}
