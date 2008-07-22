/****************************************************************************
 ** Copyright (C) 2007 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License versions 2.0 or 3.0 as published by the Free Software
 ** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file.  Alternatively you may (at
 ** your option) use any later version of the GNU General Public
 ** License if such license has been publicly approved by
 ** Klarälvdalens Datakonsult AB (or its successors, if any).
 ** 
 ** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
 ** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE. Klarälvdalens Datakonsult AB reserves all rights
 ** not expressly granted herein.
 ** 
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 **********************************************************************/

#include <QPainter>

#include "KDChartAttributesModel.h"
#include "KDChartPaintContext.h"
#include "KDChartRingDiagram.h"
#include "KDChartRingDiagram_p.h"
#include "KDChartPainterSaver_p.h"
#include "KDChartPieAttributes.h"
#include "KDChartThreeDPieAttributes.h"
#include "KDChartDataValueAttributes.h"

#include <KDABLibFakes>

using namespace KDChart;

RingDiagram::Private::Private() :
    relativeThickness( false )
{
}

RingDiagram::Private::~Private() {}

#define d d_func()

RingDiagram::RingDiagram( QWidget* parent, PolarCoordinatePlane* plane ) :
    AbstractPieDiagram( new Private(), parent, plane )
{
    init();
}

RingDiagram::~RingDiagram()
{
}

void RingDiagram::init()
{
}

/**
  * Creates an exact copy of this diagram.
  */
RingDiagram * RingDiagram::clone() const
{
    return new RingDiagram( new Private( *d ) );
}

void RingDiagram::setRelativeThickness( bool relativeThickness )
{
    d->relativeThickness = relativeThickness;
}

bool RingDiagram::relativeThickness() const
{
    return d->relativeThickness;
}

const QPair<QPointF, QPointF> RingDiagram::calculateDataBoundaries () const
{
    if ( !checkInvariants(true) ) return QPair<QPointF, QPointF>( QPointF( 0, 0 ), QPointF( 0, 0 ) );

    QPointF bottomLeft ( QPointF( 0, 0 ) );
    QPointF topRight ( QPointF( 1, 1 ) );
    return QPair<QPointF, QPointF> ( bottomLeft,  topRight );
}

void RingDiagram::paintEvent( QPaintEvent* )
{
    QPainter painter ( viewport() );
    PaintContext ctx;
    ctx.setPainter ( &painter );
    ctx.setRectangle( QRectF ( 0, 0, width(), height() ) );
    paint ( &ctx );
}

void RingDiagram::resizeEvent ( QResizeEvent*)
{
}

void RingDiagram::paint( PaintContext* ctx )
{
    // note: Not having any data model assigned is no bug
    //       but we can not draw a diagram then either.
    if ( !checkInvariants(true) )
        return;

    const int colCount = model()->columnCount(rootIndex());
    DataValueTextInfoList list;
    for ( int j=0; j<colCount; ++j ) {
        QBrush brush = qVariantValue<QBrush>( attributesModel()->headerData( j, Qt::Vertical, KDChart::DatasetBrushRole ) );
        PainterSaver painterSaver( ctx->painter() );
        ctx->painter()->setRenderHint ( QPainter::Antialiasing );
        ctx->painter()->setBrush( brush );
        QPen p( ctx->painter()->pen() );
        p.setColor( brush.color() );
        p.setWidth( 2 );// FIXME properties, use DatasetPenRole
        ctx->painter()->setPen( PrintingParameters::scalePen( p ) );
        //ctx->painter()->drawPolyline( polygon );
    }
    d->clearListOfAlreadyDrawnDataValueTexts();
    DataValueTextInfoListIterator it( list );
    while ( it.hasNext() ) {
        const DataValueTextInfo& info = it.next();
        paintDataValueText( ctx->painter(), info.index, info.pos, info.value );
    }
}

void RingDiagram::resize ( const QSizeF& )
{
}

/*virtual*/
double RingDiagram::valueTotals () const
{
    double total = 0;
    const int colCount = model()->columnCount(rootIndex());
    for ( int j=0; j<colCount; ++j ) {
      total += model()->data( model()->index( 0, j, rootIndex() ) ).toDouble();
    }
    return total;
}

/*virtual*/
double RingDiagram::numberOfValuesPerDataset() const
{
    return model() ? model()->columnCount(rootIndex()) : 0.0;
}

/*virtual*/
double RingDiagram::numberOfGridRings() const
{
    return 1;
}

