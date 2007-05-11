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

#include <QMap>

#include "KDChartAbstractPieDiagram.h"
#include "KDChartAbstractPieDiagram_p.h"
#include "KDChartAttributesModel.h"
#include "KDChartPieAttributes.h"
#include "KDChartThreeDPieAttributes.h"

#include <KDABLibFakes>


using namespace KDChart;

AbstractPieDiagram::Private::Private() :
    startPosition( 0.0 ),
    granularity( 1.0 )
{
}

AbstractPieDiagram::Private::~Private() {}

AbstractPieDiagram::AbstractPieDiagram( QWidget* parent, PolarCoordinatePlane *plane ) :
    AbstractPolarDiagram( new Private(), parent, plane )
{
    init();
}

AbstractPieDiagram::~AbstractPieDiagram()
{
}


void AbstractPieDiagram::init()
{
}


#define d d_func()

void AbstractPieDiagram::setStartPosition( qreal degrees )
{
    d->startPosition = degrees;
}

qreal AbstractPieDiagram::startPosition() const
{
    return d->startPosition;
}


void AbstractPieDiagram::setGranularity( qreal value )
{
    d->granularity = value;
}

qreal AbstractPieDiagram::granularity() const
{
    return (d->granularity < 0.05 || d->granularity > 36.0)
            ? 1.0
    : d->granularity;
}


void AbstractPieDiagram::setPieAttributes( const PieAttributes & attrs )
{
    d->attributesModel->setModelData( qVariantFromValue( attrs ), PieAttributesRole );
    emit layoutChanged( this );
}

void AbstractPieDiagram::setPieAttributes( int column, const PieAttributes & attrs )
{
    d->attributesModel->setHeaderData(
        column, Qt::Vertical, qVariantFromValue( attrs ), PieAttributesRole );
    emit layoutChanged( this );
}

// Note: Our users NEED this method - even if
//       we do not need it at drawing time!
//       (khz, 2006-07-28)
PieAttributes AbstractPieDiagram::pieAttributes() const
{
    return qVariantValue<PieAttributes>(
        d->attributesModel->data( PieAttributesRole ) );
}

// Note: Our users NEED this method - even if
//       we do not need it at drawing time!
//       (khz, 2006-07-28)
PieAttributes AbstractPieDiagram::pieAttributes( int column ) const
{
    return qVariantValue<PieAttributes>(
        d->attributesModel->data(
            d->attributesModel->mapFromSource( columnToIndex( column ) ).column(),
            PieAttributesRole ) );
}

PieAttributes AbstractPieDiagram::pieAttributes( const QModelIndex & index ) const
{
    return qVariantValue<PieAttributes>(
        d->attributesModel->data(
            d->attributesModel->mapFromSource( index ),
            PieAttributesRole ) );
}


void AbstractPieDiagram::setThreeDPieAttributes( const ThreeDPieAttributes & tda )
{
    d->attributesModel->setModelData( qVariantFromValue( tda ), ThreeDPieAttributesRole );
    emit layoutChanged( this );
}

void AbstractPieDiagram::setThreeDPieAttributes( int column, const ThreeDPieAttributes & tda )
{
    d->attributesModel->setHeaderData(
        column, Qt::Vertical, qVariantFromValue( tda ), ThreeDPieAttributesRole );
    emit layoutChanged( this );
}

void AbstractPieDiagram::setThreeDPieAttributes( const QModelIndex & index, const ThreeDPieAttributes & tda )
{
    model()->setData( index, qVariantFromValue( tda ), ThreeDPieAttributesRole );
    emit layoutChanged( this );
}

// Note: Our users NEED this method - even if
//       we do not need it at drawing time!
//       (khz, 2006-07-28)
ThreeDPieAttributes AbstractPieDiagram::threeDPieAttributes() const
{
    return qVariantValue<ThreeDPieAttributes>(
        d->attributesModel->data( ThreeDPieAttributesRole ) );
}

// Note: Our users NEED this method - even if
//       we do not need it at drawing time!
//       (khz, 2006-07-28)
ThreeDPieAttributes AbstractPieDiagram::threeDPieAttributes( int column ) const
{
    return qVariantValue<ThreeDPieAttributes>(
        d->attributesModel->data(
            d->attributesModel->mapFromSource( columnToIndex( column ) ).column(),
            ThreeDPieAttributesRole ) );
}

ThreeDPieAttributes AbstractPieDiagram::threeDPieAttributes( const QModelIndex & index ) const
{
    return qVariantValue<ThreeDPieAttributes>(
        d->attributesModel->data(
            d->attributesModel->mapFromSource( index ),
            ThreeDPieAttributesRole ) );
}

