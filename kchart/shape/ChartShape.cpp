/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Local
#include "ChartShape.h"

// KChart
#include "kdchart/src/KDChartChart.h"

// KOffice
#include <KoViewConverter.h>

// KDE
#include <kdebug.h>

// Qt
#include <QPainter>

using namespace KChart;

class ChartShape::Private
{
public:
    KDChart::Chart* chart;
};


ChartShape::ChartShape( int columns, int rows )
    : d( new Private )
{
    d->chart = new KDChart::Chart();
}

ChartShape::~ChartShape()
{
    delete d->chart;
    delete d;
}

void ChartShape::paint( QPainter& painter, const KoViewConverter& converter )
{
    const QRectF paintRect = QRectF( QPointF( 0.0, 0.0 ), size() );

    applyConversion( painter, converter );
    painter.setClipRect( paintRect, Qt::IntersectClip );

    // painting chart contents
    d->chart->paint( &painter, /*converter.documentToView*/( paintRect ).toRect() );
}

void ChartShape::resize( const QSizeF& newSize )
{
    if ( size() == newSize )
        return;

    KoShape::resize( newSize );
}

void ChartShape::saveOdf( KoShapeSavingContext & context ) const
{
}

bool ChartShape::loadOdf( const KoXmlElement & element, KoShapeLoadingContext &context ) {
    return false; // TODO
}
