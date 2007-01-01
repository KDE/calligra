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


#include <QGridLayout>

#include "KDChartChart.h"
#include "KDChartAbstractCoordinatePlane.h"
#include "KDChartAbstractCoordinatePlane_p.h"
#include "KDChartGridAttributes.h"

#include <KDABLibFakes>


using namespace KDChart;

#define d d_func()

AbstractCoordinatePlane::Private::Private()
    : AbstractArea::Private()
    , parent( 0 )
    , grid( 0 )
    , referenceCoordinatePlane( 0 )
{
    // this bloc left empty intentionally
}


AbstractCoordinatePlane::AbstractCoordinatePlane ( KDChart::Chart* parent )
    : AbstractArea ( new Private() )
{
    d->parent = parent;
    d->init();
}

AbstractCoordinatePlane::~AbstractCoordinatePlane()
{
    emit destroyedCoordinatePlane( this );
}

void AbstractCoordinatePlane::init()
{
    d->initialize();  // virtual method to init the correct grid: cartesian, polar, ...
}

void AbstractCoordinatePlane::addDiagram ( AbstractDiagram* diagram )
{
    // diagrams are invisible and paint through their paint() method
    diagram->hide();

    d->diagrams.append( diagram );
    diagram->setParent( d->parent );
    diagram->setCoordinatePlane( this );
    layoutDiagrams();
    layoutPlanes(); // there might be new axes, etc
    update();
}

/*virtual*/
void AbstractCoordinatePlane::replaceDiagram ( AbstractDiagram* diagram, AbstractDiagram* oldDiagram_ )
{
    if( diagram && oldDiagram_ != diagram ){
        AbstractDiagram* oldDiagram = oldDiagram_;
        if( d->diagrams.count() ){
            if( ! oldDiagram )
                oldDiagram = d->diagrams.first();
            takeDiagram( oldDiagram );
        }
        delete oldDiagram;
        addDiagram( diagram );
        layoutDiagrams();
        layoutPlanes(); // there might be new axes, etc
        update();
    }
}

/*virtual*/
void AbstractCoordinatePlane::takeDiagram ( AbstractDiagram* diagram )
{
    const int idx = d->diagrams.indexOf( diagram );
    if( idx != -1 ){
        d->diagrams.removeAt( idx );
        diagram->setParent( 0 );
        diagram->setCoordinatePlane( 0 );
        layoutDiagrams();
        update();
    }
}


AbstractDiagram* AbstractCoordinatePlane::diagram()
{
    if ( d->diagrams.isEmpty() )
    {
        return 0;
    } else {
        return d->diagrams.first();
    }
}

AbstractDiagramList AbstractCoordinatePlane::diagrams()
{
    return d->diagrams;
}

ConstAbstractDiagramList AbstractCoordinatePlane::diagrams() const
{
    ConstAbstractDiagramList list;
#ifndef QT_NO_STL
    qCopy( d->diagrams.begin(), d->diagrams.end(), std::back_inserter( list ) );
#else
    Q_FOREACH( AbstractDiagram * a, d->diagrams )
        list.push_back( a );
#endif
    return list;
}

QSize KDChart::AbstractCoordinatePlane::minimumSizeHint() const
{
    return QSize( 200, 200 );
}


QSizePolicy KDChart::AbstractCoordinatePlane::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
}

void KDChart::AbstractCoordinatePlane::setGlobalGridAttributes( const GridAttributes& a )
{
    d->gridAttributes = a;
    update();
}

GridAttributes KDChart::AbstractCoordinatePlane::globalGridAttributes() const
{
    return d->gridAttributes;
}

KDChart::DataDimensionsList KDChart::AbstractCoordinatePlane::gridDimensionsList()
{
    return d->grid->updateData( this );
}

void KDChart::AbstractCoordinatePlane::setReferenceCoordinatePlane( AbstractCoordinatePlane * plane )
{
    d->referenceCoordinatePlane = plane;
}

AbstractCoordinatePlane * KDChart::AbstractCoordinatePlane::referenceCoordinatePlane( ) const
{
    return d->referenceCoordinatePlane;
}

void KDChart::AbstractCoordinatePlane::setParent( KDChart::Chart* parent )
{
    d->parent = parent;
}

const KDChart::Chart* KDChart::AbstractCoordinatePlane::parent() const
{
    return d->parent;
}

KDChart::Chart* KDChart::AbstractCoordinatePlane::parent()
{
    return d->parent;
}

/* pure virtual in QLayoutItem */
bool KDChart::AbstractCoordinatePlane::isEmpty() const
{
    return false; // never empty!
    // coordinate planes with no associated diagrams
    // are showing a default grid of ()1..10, 1..10) stepWidth 1
}
/* pure virtual in QLayoutItem */
Qt::Orientations KDChart::AbstractCoordinatePlane::expandingDirections() const
{
    return Qt::Vertical | Qt::Horizontal;
}
/* pure virtual in QLayoutItem */
QSize KDChart::AbstractCoordinatePlane::maximumSize() const
{
    if( d->parent )
        return d->parent->size();
    // Note: At external layut calculation time the coord.plane *will*
    //       have a parent widget, so returning a default size here
    //       will not affect its real drawing size.
    return QSize(1000, 1000);
}
/* pure virtual in QLayoutItem */
QSize KDChart::AbstractCoordinatePlane::minimumSize() const
{
    return QSize(60, 60); // this default can be overwritten by derived classes
}
/* pure virtual in QLayoutItem */
QSize KDChart::AbstractCoordinatePlane::sizeHint() const
{
    // we return our maxiumu (which is the full size of the Chart)
    // even if we know the plane will be smaller
    return maximumSize();
}
/* pure virtual in QLayoutItem */
void KDChart::AbstractCoordinatePlane::setGeometry( const QRect& r )
{
//    qDebug() << "KDChart::AbstractCoordinatePlane::setGeometry(" << r << ") called";
    if( d->geometry != r ){
        d->geometry = r;
        // Note: We do *not* call update() here
        //       because it would invoke KDChart::update() recursively.
    }
}
/* pure virtual in QLayoutItem */
QRect KDChart::AbstractCoordinatePlane::geometry() const
{
    return d->geometry;
}

void KDChart::AbstractCoordinatePlane::update()
{
    //qDebug("KDChart::AbstractCoordinatePlane::update() called");
    emit needUpdate();
}

void KDChart::AbstractCoordinatePlane::relayout()
{
    //qDebug("KDChart::AbstractCoordinatePlane::relayout() called");
    emit needRelayout();
}

void KDChart::AbstractCoordinatePlane::layoutPlanes()
{
    //qDebug("KDChart::AbstractCoordinatePlane::relayout() called");
    emit needLayoutPlanes();
}

#undef d
