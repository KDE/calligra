/****************************************************************************
 ** Copyright (C) 2006 Klar�vdalens Datakonsult AB.  All rights reserved.
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
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/


#include <QPainter>
#include "KDChartAttributesModel.h"
#include "KDChartPaintContext.h"
#include "KDChartPolarDiagram.h"
#include "KDChartPolarDiagram_p.h"
#include "KDChartPainterSaver_p.h"
#include "KDChartDataValueAttributes.h"

#include <KDABLibFakes>

using namespace KDChart;

PolarDiagram::Private::Private() :
    zeroDegreePosition( 0 ),
    rotateCircularLabels( false )
{
}

PolarDiagram::Private::~Private() {}

#define d d_func()

PolarDiagram::PolarDiagram( QWidget* parent, PolarCoordinatePlane* plane ) :
    AbstractPolarDiagram( new Private( ), parent, plane )
{
}

PolarDiagram::~PolarDiagram()
{
}


void PolarDiagram::init()
{
    setShowDelimitersAtPosition( Position::NorthWest, false );
    setShowDelimitersAtPosition( Position::North, true );
    setShowDelimitersAtPosition( Position::NorthEast, false );
    setShowDelimitersAtPosition( Position::West, false );
    setShowDelimitersAtPosition( Position::East, false );
    setShowDelimitersAtPosition( Position::SouthWest, false );
    setShowDelimitersAtPosition( Position::South, true );
    setShowDelimitersAtPosition( Position::SouthEast, false );
    setShowLabelsAtPosition( Position::NorthWest, false );
    setShowLabelsAtPosition( Position::North, true );
    setShowLabelsAtPosition( Position::NorthEast, false );
    setShowLabelsAtPosition( Position::West, false );
    setShowLabelsAtPosition( Position::East, false );
    setShowLabelsAtPosition( Position::SouthWest, false );
    setShowLabelsAtPosition( Position::South, true );
    setShowLabelsAtPosition( Position::SouthEast, false );
}




PolarDiagram * PolarDiagram::clone() const
{
    PolarDiagram* newDiagram = new PolarDiagram( new Private( *d ) );
    // This needs to be copied after the fact
    newDiagram->d->showDelimitersAtPosition = d->showDelimitersAtPosition;
    newDiagram->d->showLabelsAtPosition = d->showLabelsAtPosition;
    return newDiagram;
}

const QPair<QPointF, QPointF> PolarDiagram::calculateDataBoundaries () const
{
    if ( !checkInvariants() ) return QPair<QPointF, QPointF>( QPointF( 0, 0 ), QPointF( 0, 0 ) );
    const int rowCount = model()->rowCount(rootIndex());
    const int colCount = model()->columnCount(rootIndex());
    double xMin = 0.0;
    double xMax = colCount;
    double yMin = 0, yMax = 0;
    for ( int j=0; j<colCount; ++j ) {
        for ( int i=0; i< rowCount; ++i ) {
            double value = model()->data( model()->index( i, j, rootIndex() ) ).toDouble();
            yMax = qMax( yMax, value );
        }
    }
    QPointF bottomLeft ( QPointF( xMin, yMin ) );
    QPointF topRight ( QPointF( xMax, yMax ) );
    return QPair<QPointF, QPointF> ( bottomLeft,  topRight );
}



void PolarDiagram::paintEvent ( QPaintEvent*)
{
    QPainter painter ( viewport() );
    PaintContext ctx;
    ctx.setPainter ( &painter );
    ctx.setRectangle( QRectF ( 0, 0, width(), height() ) );
    paint ( &ctx );
}

void PolarDiagram::resizeEvent ( QResizeEvent*)
{
}

void PolarDiagram::paintPolarMarkers( PaintContext* ctx, const QPolygonF& polygon )
{
    const double markerSize = 4; // FIXME use real markers
    for ( int i=0; i<polygon.size(); ++i ) {
        QPointF p = polygon.at( i );
        p.setX( p.x() - markerSize/2 );
        p.setY( p.y() - markerSize/2 );
        ctx->painter()->drawRect( QRectF( p, QSizeF( markerSize, markerSize ) ) );
    }
}

void PolarDiagram::paint( PaintContext* ctx )
{
    if ( !checkInvariants() )
        return;
    const int rowCount = model()->rowCount( rootIndex() );
    const int colCount = model()->columnCount( rootIndex() );
    DataValueTextInfoList list;

    for ( int j=0; j < colCount; ++j ) {
        QBrush brush = qVariantValue<QBrush>( attributesModel()->headerData( j, Qt::Vertical, KDChart::DatasetBrushRole ) );
        QPolygonF polygon;
        for ( int i=0; i < rowCount; ++i ) {
            QModelIndex index = model()->index( i, j, rootIndex() );
            const double value = model()->data( index ).toDouble();
            QPointF point = coordinatePlane()->translate( QPointF( value, i ) );
            polygon.append( point );
            list.append( DataValueTextInfo( index, point, value ) );
        }
        PainterSaver painterSaver( ctx->painter() );
        ctx->painter()->setRenderHint ( QPainter::Antialiasing );
        ctx->painter()->setBrush( brush );
        QPen p( ctx->painter()->pen() );
        p.setColor( brush.color() ); // FIXME use DatasetPenRole
        p.setWidth( 2 );// FIXME properties
        ctx->painter()->setPen( p );
        polygon.translate( ctx->rectangle().topLeft() );
        ctx->painter()->drawPolyline( polygon );
        paintPolarMarkers( ctx, polygon );
    }
    DataValueTextInfoListIterator it( list );
    while ( it.hasNext() ) {
        const DataValueTextInfo& info = it.next();
        paintDataValueText( ctx->painter(), info.index, info.pos, info.value );
    }
}

void PolarDiagram::resize ( const QSizeF& )
{
}

/*virtual*/
double PolarDiagram::valueTotals () const
{
    return model()->rowCount(rootIndex());
}

/*virtual*/
double PolarDiagram::numberOfValuesPerDataset() const
{
    return model()->rowCount(rootIndex());
}

/*virtual*/
double PolarDiagram::numberOfGridRings() const
{
    return 5; // FIXME
}

void PolarDiagram::setZeroDegreePosition( int degrees )
{
    d->zeroDegreePosition = degrees;
}

int PolarDiagram::zeroDegreePosition() const
{
    return d->zeroDegreePosition;
}

void PolarDiagram::setRotateCircularLabels( bool rotateCircularLabels )
{
    d->rotateCircularLabels = rotateCircularLabels;
}

bool PolarDiagram::rotateCircularLabels() const
{
    return d->rotateCircularLabels;
}

void PolarDiagram::setShowDelimitersAtPosition( Position position,
                                                       bool showDelimiters )
{
    d->showDelimitersAtPosition[position.value()] = showDelimiters;
}

void PolarDiagram::setShowLabelsAtPosition( Position position,
                                                   bool showLabels )
{
    d->showLabelsAtPosition[position.value()] = showLabels;
}

bool PolarDiagram::showDelimitersAtPosition( Position position ) const
{
    return d->showDelimitersAtPosition[position.value()];
}

bool PolarDiagram::showLabelsAtPosition( Position position ) const
{
    return d->showLabelsAtPosition[position.value()];
}



