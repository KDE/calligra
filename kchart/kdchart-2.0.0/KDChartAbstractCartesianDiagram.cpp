/****************************************************************************
** Copyright (C) 2006 Klarälvdalens Datakonsult AB.  All rights reserved.
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

#include "KDChartAbstractCartesianDiagram.h"
#include "KDChartAbstractCartesianDiagram_p.h"
#include "KDChartPaintContext.h"
#include <QDebug>
#include <QPainter>

#include <KDABLibFakes>


using namespace KDChart;

AbstractCartesianDiagram::Private::Private()
    : referenceDiagram( 0 )
{
}

AbstractCartesianDiagram::Private::~Private()
{

}

void AbstractCartesianDiagram::init()
{
}

#define d d_func()

AbstractCartesianDiagram::AbstractCartesianDiagram ( QWidget* parent, CartesianCoordinatePlane* plane )
    : AbstractDiagram ( new Private(), parent, plane )
{
}

KDChart::AbstractCartesianDiagram::~AbstractCartesianDiagram()
{
    Q_FOREACH( CartesianAxis* axis, d->axesList ) {
        axis->deleteObserver( this );
    }
    d->axesList.clear();
}

void AbstractCartesianDiagram::addAxis( CartesianAxis *axis )
{
    if ( !d->axesList.contains( axis ) ) {
        d->axesList.append( axis );
        axis->createObserver( this );
        layoutPlanes();
    }
}

void AbstractCartesianDiagram::takeAxis( CartesianAxis *axis )
{
    const int idx = d->axesList.indexOf( axis );
    if( idx != -1 )
        d->axesList.takeAt( idx );
    axis->deleteObserver( this );
    axis->setParentWidget( 0 );
    layoutPlanes();
}

KDChart::CartesianAxisList AbstractCartesianDiagram::axes( ) const
{
    return d->axesList;
}

void KDChart::AbstractCartesianDiagram::layoutPlanes()
{
    //qDebug() << "KDChart::AbstractCartesianDiagram::layoutPlanes()";
    AbstractCoordinatePlane* plane = coordinatePlane();
    if( plane ){
        plane->layoutPlanes();
        //qDebug() << "KDChart::AbstractCartesianDiagram::layoutPlanes() OK";
    }
}

void KDChart::AbstractCartesianDiagram::setCoordinatePlane( AbstractCoordinatePlane* plane )
{
    if( coordinatePlane() ) disconnect( coordinatePlane() );
    AbstractDiagram::setCoordinatePlane(plane);

    // show the axes, after all have been adjusted
    // (because they might be dependend on each other)
    /*
    if( plane )
        Q_FOREACH( CartesianAxis* axis, d->axesList )
            axis->show();
    else
        Q_FOREACH( CartesianAxis* axis, d->axesList )
            axis->hide();
    */
}

void AbstractCartesianDiagram::setReferenceDiagram( AbstractCartesianDiagram* diagram, const QPointF& offset )
{
    d->referenceDiagram = diagram;
    d->referenceDiagramOffset = offset;
}

AbstractCartesianDiagram* AbstractCartesianDiagram::referenceDiagram() const
{
    return d->referenceDiagram;
}

QPointF AbstractCartesianDiagram::referenceDiagramOffset() const
{
    return d->referenceDiagramOffset;
}
