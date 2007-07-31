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
#include "KDChartAbstractCoordinatePlane.h"
#include "KDChartBarDiagram.h"
#include "KDChartChart.h"
#include "KDChartLineDiagram.h"

// KOffice
#include <KoViewConverter.h>

// KDE
#include <kdebug.h>

// Qt
#include <QAbstractItemModel>
#include <QPainter>

using namespace KChart;

class ChartShape::Private
{
public:
    KDChart::Chart* chart;
    KDChart::AbstractDiagram* diagram;
    QAbstractItemModel* model;
};


ChartShape::ChartShape()
    : d( new Private )
{
    d->chart = new KDChart::Chart();
    d->diagram = new KDChart::LineDiagram();
    d->model = 0;

    d->chart->coordinatePlane()->replaceDiagram( d->diagram );
}

ChartShape::~ChartShape()
{
    delete d->model;
    delete d->diagram;
    delete d->chart;
    delete d;
}

KDChart::Chart* ChartShape::chart() const
{
    return d->chart;
}

void ChartShape::setModel( QAbstractItemModel* model )
{
    kDebug() << k_funcinfo;
    d->model = model;
    d->chart->coordinatePlane()->takeDiagram( d->diagram );
    d->diagram->setModel( model );
    d->chart->coordinatePlane()->replaceDiagram( d->diagram );
    kDebug() << k_funcinfo <<" END";
}

void ChartShape::paint( QPainter& painter, const KoViewConverter& converter )
{
    const QRectF paintRect = QRectF( QPointF( 0.0, 0.0 ), size() );

    applyConversion( painter, converter );
    painter.setClipRect( paintRect, Qt::IntersectClip );

    // painting chart contents
    d->chart->paint( &painter, paintRect.toRect() );
}

void ChartShape::saveOdf( KoShapeSavingContext & context ) const
{
}

bool ChartShape::loadOdf( const KoXmlElement & element, KoShapeLoadingContext &context ) {
    return false; // TODO
}
