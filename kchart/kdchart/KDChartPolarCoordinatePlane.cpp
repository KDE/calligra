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

#include <math.h>

#include <QFont>
#include <QList>
#include <QtDebug>
#include <QPainter>

#include "KDChartChart.h"
#include "KDChartPaintContext.h"
#include "KDChartAbstractDiagram.h"
#include "KDChartAbstractPolarDiagram.h"
#include "KDChartPolarCoordinatePlane.h"
#include "KDChartPolarCoordinatePlane_p.h"
#include "KDChartPainterSaver_p.h"

#include <KDABLibFakes>

using namespace KDChart;

#define d d_func()

/*
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define DEGTORAD(d) (d)*M_PI/180

struct PolarCoordinatePlane::CoordinateTransformation
{
    // represents the distance of the diagram coordinate origin to the
    // origin of the coordinate plane space:
    QPointF originTranslation;
    double radiusUnit;
    double angleUnit;

    ZoomParameters zoom;

    static QPointF polarToCartesian( double R, double theta )
    {
        return QPointF( R * cos( DEGTORAD( theta  ) ), R * sin( DEGTORAD( theta ) ) );
    }

    inline const QPointF translate( const QPointF& diagramPoint ) const
    {
        // calculate the polar coordinates
        const double x = diagramPoint.x() * radiusUnit;
        const double y = ( diagramPoint.y() * angleUnit) - 90;
        // convert to cartesian coordinates
        QPointF cartesianPoint = polarToCartesian( x, y );
        cartesianPoint.setX( cartesianPoint.x() * zoom.xFactor );
        cartesianPoint.setY( cartesianPoint.y() * zoom.yFactor );

        QPointF newOrigin = originTranslation;
        double minOrigin = qMin( newOrigin.x(), newOrigin.y() );
        newOrigin.setX( newOrigin.x() + minOrigin * ( 1 - zoom.xCenter * 2 ) * zoom.xFactor );
        newOrigin.setY( newOrigin.y() + minOrigin * ( 1 - zoom.yCenter * 2 ) * zoom.yFactor );

        return newOrigin + cartesianPoint;
    }

    inline const QPointF translatePolar( const QPointF& diagramPoint ) const
    {
        return QPointF( diagramPoint.x() * angleUnit, diagramPoint.y() * radiusUnit );
    }
};

class PolarCoordinatePlane::Private
{
public:
    Private()
        :currentTransformation(0),
        initialResizeEventReceived(false )
        {}


    // the coordinate plane will calculate coordinate transformations for all
    // diagrams and store them here:
    CoordinateTransformationList coordinateTransformations;
    // when painting, this pointer selects the coordinate transformation for
    // the current diagram:
    CoordinateTransformation* currentTransformation;
    // the reactangle occupied by the diagrams, in plane coordinates
    QRectF contentRect;
    // true after the first resize event came in
    bool initialResizeEventReceived;
};
*/

PolarCoordinatePlane::PolarCoordinatePlane ( Chart* parent )
    : AbstractCoordinatePlane ( new Private(), parent )
{
    // this bloc left empty intentionally
}

PolarCoordinatePlane::~PolarCoordinatePlane()
{
    // this bloc left empty intentionally
}

void PolarCoordinatePlane::init()
{
    // this bloc left empty intentionally
}

void PolarCoordinatePlane::addDiagram ( AbstractDiagram* diagram )
{
    Q_ASSERT_X ( dynamic_cast<AbstractPolarDiagram*> ( diagram ),
                 "PolarCoordinatePlane::addDiagram", "Only polar"
                 "diagrams can be added to a polar coordinate plane!" );
    AbstractCoordinatePlane::addDiagram ( diagram );
    connect ( diagram,  SIGNAL ( layoutChanged ( AbstractDiagram* ) ),
              SLOT ( slotLayoutChanged ( AbstractDiagram* ) ) );

}

void PolarCoordinatePlane::paint ( QPainter* painter )
{
    AbstractDiagramList diags = diagrams();
    if ( d->coordinateTransformations.size() == diags.size() )
    {
        PaintContext ctx;
        ctx.setPainter ( painter );
        ctx.setCoordinatePlane ( this );
        ctx.setRectangle ( geometry() /*d->contentRect*/ );

        // paint the coordinate system rulers:
        d->currentTransformation = & ( d->coordinateTransformations[0] );
        d->grid->drawGrid( &ctx );

        // paint the diagrams:
        for ( int i = 0; i < diags.size(); i++ )
        {
            d->currentTransformation = & ( d->coordinateTransformations[i] );
            PainterSaver painterSaver( painter );
            diags[i]->paint ( &ctx );
        }
        d->currentTransformation = 0;
    } // else: diagrams have not been set up yet
}

/*
void PolarCoordinatePlane::paintEvent ( QPaintEvent* )
{
    AbstractDiagramList diags = diagrams();
    if ( d->coordinateTransformations.size() == diags.size() )
    {
        QPainter painter ( this );
        PaintContext ctx;
        ctx.setPainter ( &painter );
        ctx.setCoordinatePlane ( this );
        ctx.setRectangle ( d->contentRect );

        // paint the coordinate system rulers:
        d->grid->drawGrid( &ctx );

        // paint the diagrams:
        for ( int i = 0; i < diags.size(); i++ )
        {
            d->currentTransformation = & ( d->coordinateTransformations[i] );
            PainterSaver painterSaver( &painter );
            diags[i]->paint ( &ctx );
        }
        d->currentTransformation = 0;
    } // else: diagrams have not been set up yet
}
*/
/*
void PolarCoordinatePlane::paintGrid( PaintContext* ctx )
{
    if ( d->coordinateTransformations.size () <= 0 ) return;

    // FIXME: we paint the rulers to the settings of the first diagram for now:
    AbstractPolarDiagram* dgr = dynamic_cast<AbstractPolarDiagram*> (diagrams().first() );
    Q_ASSERT ( dgr ); // only polar diagrams are allowed here

    ctx->painter()->setPen ( QColor ( Qt::lightGray ) );
    QPointF origin = translate( QPointF( 0,0 ) );
    const int numberOfSpokes = ( int ) ( 360 / d->currentTransformation->angleUnit );
    const double r = dgr->dataBoundaries().second.y(); // use the full extents
    for ( int i = 0; i < numberOfSpokes ; ++i ) {
        ctx->painter()->drawLine( origin, d->currentTransformation->translate( QPointF( r, i ) ) );
    }
    const int numberOfGridRings = ( int ) dgr->numberOfGridRings();
    for ( int i = 0; i < numberOfGridRings; ++i ) {
        const double rad = ( ( i + 1) * r / numberOfGridRings );

        if ( rad == 0 )
            continue;

        QRectF rect;
        QPointF topLeftPoint;
        QPointF bottomRightPoint;

        topLeftPoint = d->currentTransformation->translate( QPointF( rad, 0 ) );
        topLeftPoint.setX( d->currentTransformation->translate( QPointF( rad, 90 / d->currentTransformation->angleUnit ) ).x() );
        bottomRightPoint = d->currentTransformation->translate( QPointF( rad, 180 / d->currentTransformation->angleUnit ) );
        bottomRightPoint.setX( d->currentTransformation->translate( QPointF( rad, 270 / d->currentTransformation->angleUnit ) ).x() );

        rect.setTopLeft( topLeftPoint );
        rect.setBottomRight( bottomRightPoint );

        ctx->painter()->drawEllipse( rect );
    }
}
*/

void PolarCoordinatePlane::resizeEvent ( QResizeEvent* )
{
    d->initialResizeEventReceived = true;
    layoutDiagrams();
}

void PolarCoordinatePlane::layoutDiagrams()
{
    // the rectangle the diagrams cover in the *plane*:
    // (Why -3? We save 1px on each side for the antialiased drawing, and
    // respect the way QPainter calculates the width of a painted rect (the
    // size is the rectangle size plus the pen width). This way, most clipping
    // for regular pens should be avoided. When pens with a penWidth or larger
    // than 1 are used, this may not b sufficient.
    const QRect rect( areaGeometry() );
    d->contentRect = QRectF ( 1, 1, rect.width() - 3, rect.height() - 3 );

    // FIXME distribute space according to options:
    d->coordinateTransformations.clear();
    Q_FOREACH( AbstractDiagram* diagram, diagrams() )
        {
            AbstractPolarDiagram *polarDiagram = dynamic_cast<AbstractPolarDiagram*>( diagram );
            Q_ASSERT( polarDiagram );
            QPair<QPointF, QPointF> dataBoundariesPair = polarDiagram->dataBoundaries();

            const double angleUnit = 360 / polarDiagram->valueTotals();
//qDebug() << "--------------------------------------------------------";
            const double radius = dataBoundariesPair.second.y();
//qDebug() << radius <<"="<<dataBoundariesPair.second.y();
            const double diagramWidth = radius * 2; // == height
            const double planeWidth = d->contentRect.width();
            const double planeHeight = d->contentRect.height();
            const double radiusUnit = qMin( planeWidth, planeHeight ) / diagramWidth;
//qDebug() << radiusUnit <<"=" << "qMin( "<<planeWidth<<","<< planeHeight <<") / "<<diagramWidth;
            QPointF coordinateOrigin = QPointF ( planeWidth / 2, planeHeight / 2 );
            coordinateOrigin += d->contentRect.topLeft();

            CoordinateTransformation diagramTransposition;
            diagramTransposition.originTranslation = coordinateOrigin;
            diagramTransposition.radiusUnit = radiusUnit;
            diagramTransposition.angleUnit = angleUnit;
            diagramTransposition.zoom = ZoomParameters();
            d->coordinateTransformations.append( diagramTransposition );
        }
}

const QPointF PolarCoordinatePlane::translate( const QPointF& diagramPoint ) const
{
    Q_ASSERT_X ( d->currentTransformation != 0, "PolarCoordinatePlane::translate",
                 "Only call translate() from within paint()." );
    return  d->currentTransformation->translate ( diagramPoint );
}

const QPointF PolarCoordinatePlane::translatePolar( const QPointF& diagramPoint ) const
{
    Q_ASSERT_X ( d->currentTransformation != 0, "PolarCoordinatePlane::translate",
                 "Only call translate() from within paint()." );
    return  d->currentTransformation->translatePolar ( diagramPoint );
}

qreal PolarCoordinatePlane::angleUnit() const
{
    Q_ASSERT_X ( d->currentTransformation != 0, "PolarCoordinatePlane::translate",
                 "Only call translate() from within paint()." );
    return  d->currentTransformation->angleUnit;
}

void PolarCoordinatePlane::slotLayoutChanged ( AbstractDiagram* )
{
    if ( d->initialResizeEventReceived ) layoutDiagrams();
}

double PolarCoordinatePlane::zoomFactorX() const
{
    return d->coordinateTransformations[0].zoom.xFactor;
}

double PolarCoordinatePlane::zoomFactorY() const
{
    return d->coordinateTransformations[0].zoom.yFactor;
}

void PolarCoordinatePlane::setZoomFactorX( double factor )
{
    d->coordinateTransformations[0].zoom.xFactor = factor;
}

void PolarCoordinatePlane::setZoomFactorY( double factor )
{
    d->coordinateTransformations[0].zoom.yFactor = factor;
}

QPointF PolarCoordinatePlane::zoomCenter() const
{
    return QPointF( d->coordinateTransformations[0].zoom.xCenter, d->coordinateTransformations[0].zoom.yCenter );
}

void PolarCoordinatePlane::setZoomCenter( QPointF center )
{
    d->coordinateTransformations[0].zoom.xCenter = center.x();
    d->coordinateTransformations[0].zoom.yCenter = center.y();
}

DataDimensionsList PolarCoordinatePlane::getDataDimensionsList() const
{
    DataDimensionsList l;

    //FIXME(khz): do the real calculation

    return l;
}

void KDChart::PolarCoordinatePlane::setGridAttributes(
    bool circular,
    const GridAttributes& a )
{
    if( circular )
        d->gridAttributesCircular = a;
    else
        d->gridAttributesSagittal = a;
    setHasOwnGridAttributes( circular, true );
    update();
    emit propertiesChanged();
}

void KDChart::PolarCoordinatePlane::resetGridAttributes(
    bool circular )
{
    setHasOwnGridAttributes( circular, false );
    update();
}

GridAttributes KDChart::PolarCoordinatePlane::gridAttributes(
    bool circular ) const
{
    if( hasOwnGridAttributes( circular ) ){
        if( circular )
            return d->gridAttributesCircular;
        else
            return d->gridAttributesSagittal;
    }else{
        return globalGridAttributes();
    }
}

void KDChart::PolarCoordinatePlane::setHasOwnGridAttributes(
    bool circular, bool on )
{
    if( circular )
        d->hasOwnGridAttributesCircular = on;
    else
        d->hasOwnGridAttributesSagittal = on;
    emit propertiesChanged();
}

bool KDChart::PolarCoordinatePlane::hasOwnGridAttributes(
    bool circular ) const
{
    return
        ( circular )
        ? d->hasOwnGridAttributesCircular
        : d->hasOwnGridAttributesSagittal;
}
