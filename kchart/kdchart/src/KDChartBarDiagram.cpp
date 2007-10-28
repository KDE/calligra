/****************************************************************************
 ** Copyright (C) 2007 Klaralvdalens Datakonsult AB.  All rights reserved.
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
#include <QDebug>

#include "KDChartBarDiagram.h"
#include "KDChartBarDiagram_p.h"
#include "KDChartThreeDBarAttributes.h"
#include "KDChartPosition.h"
#include "KDChartAttributesModel.h"
#include "KDChartAbstractGrid.h"

#include <KDABLibFakes>

#include "KDChartBarDiagram_p.h"
#include "KDChartNormalBarDiagram_p.h"
#include "KDChartStackedBarDiagram_p.h"
#include "KDChartPercentBarDiagram_p.h"


using namespace KDChart;

BarDiagram::Private::Private()
{
}

BarDiagram::Private::~Private() {}

#define d d_func()


BarDiagram::BarDiagram( QWidget* parent, CartesianCoordinatePlane* plane ) :
    AbstractCartesianDiagram( new Private(), parent, plane )
{
    init();
}

void BarDiagram::init()
{
    d->diagram = this;
    d->normalDiagram = new NormalBarDiagram( this );
    d->stackedDiagram = new StackedBarDiagram( this );
    d->percentDiagram = new PercentBarDiagram( this );
    d->implementor = d->normalDiagram;
    d->compressor.setModel( attributesModel() );
}

BarDiagram::~BarDiagram()
{
}

/**
  * Creates an exact copy of this diagram.
  */
BarDiagram * BarDiagram::clone() const
{

    BarDiagram* newDiagram = new BarDiagram( new Private( *d ) );
    newDiagram->setType( type() );
    return newDiagram;
}

bool BarDiagram::compare( const BarDiagram* other )const
{
    if( other == this ) return true;
    if( ! other ){
        return false;
    }

    return  // compare the base class
            ( static_cast<const AbstractCartesianDiagram*>(this)->compare( other ) ) &&
            // compare own properties
            (type() == other->type());
}

/**
  * Sets the bar diagram's type to \a type
  * \sa BarDiagram::BarType
  */
void BarDiagram::setType( const BarType type )
{
    //if ( type == d->barType ) return;
     if ( d->implementor->type() == type ) return;

     switch( type ) {
     case Normal:
         d->implementor = d->normalDiagram;
         break;
     case Stacked:
         d->implementor = d->stackedDiagram;
         break;
     case Percent:
         d->implementor = d->percentDiagram;
         break;
     default:
         Q_ASSERT_X( false, "BarDiagram::setType", "unknown diagram subtype" );
     };

   Q_ASSERT( d->implementor->type() == type );

   //d->barType = type;
    // AbstractAxis settings - see AbstractDiagram and CartesianAxis
    setPercentMode( type == BarDiagram::Percent );
    setDataBoundariesDirty();
    emit layoutChanged( this );
    emit propertiesChanged();
}

/**
  * @return the type of the line diagram
  */
BarDiagram::BarType BarDiagram::type() const
{
    return d->implementor->type();
}

/**
  * Sets the global bar attributes to \a ba
  */
void BarDiagram::setBarAttributes( const BarAttributes& ba )
{
    d->attributesModel->setModelData( qVariantFromValue( ba ), BarAttributesRole );
    emit propertiesChanged();
}

/**
  * Sets the bar attributes of data set \a column to \a ba
  */
void BarDiagram::setBarAttributes( int column, const BarAttributes& ba )
{
    d->attributesModel->setHeaderData(
        column, Qt::Vertical,
        qVariantFromValue( ba ),
        BarAttributesRole );
    emit propertiesChanged();
}

/**
  * Sets the line attributes for the model index \a index to \a ba
  */
void BarDiagram::setBarAttributes( const QModelIndex& index, const BarAttributes& ba )
{
    attributesModel()->setData(
        d->attributesModel->mapFromSource( index ),
        qVariantFromValue( ba ),
        BarAttributesRole );
    emit propertiesChanged();
}

/**
  * @return the global bar attribute set
  */
BarAttributes BarDiagram::barAttributes() const
{
    return qVariantValue<BarAttributes>(
        d->attributesModel->data( KDChart::BarAttributesRole ) );
}

/**
  * @return the bar attribute set of data set \a column
  */
BarAttributes BarDiagram::barAttributes( int column ) const
{
    return qVariantValue<BarAttributes>(
        d->attributesModel->data(
            d->attributesModel->mapFromSource( columnToIndex( column ) ),
            KDChart::BarAttributesRole ) );
}

/**
  * @return the bar attribute set of the model index \a index
  */
BarAttributes BarDiagram::barAttributes( const QModelIndex& index ) const
{
    return qVariantValue<BarAttributes>(
        d->attributesModel->data(
            d->attributesModel->mapFromSource( index ),
            KDChart::BarAttributesRole ) );
}

/**
  * Sets the global 3D bar attributes to \a threeDAttrs
  */
void BarDiagram::setThreeDBarAttributes( const ThreeDBarAttributes& threeDAttrs )
{
    setDataBoundariesDirty();
    d->attributesModel->setModelData( qVariantFromValue( threeDAttrs ), ThreeDBarAttributesRole );
    //emit layoutChanged( this );
     emit propertiesChanged();
}

/**
  * Sets the 3D bar attributes of dataset \a column to \a threeDAttrs
  */
void BarDiagram::setThreeDBarAttributes( int column, const ThreeDBarAttributes& threeDAttrs )
{
    setDataBoundariesDirty();
    d->attributesModel->setHeaderData(
        column, Qt::Vertical,
        qVariantFromValue( threeDAttrs ),
        ThreeDBarAttributesRole );
    //emit layoutChanged( this );
    emit propertiesChanged();

}

/**
  * Sets the 3D line attributes of model index \a index to \a threeDAttrs
  */
void BarDiagram::setThreeDBarAttributes( const QModelIndex& index, const ThreeDBarAttributes& threeDAttrs )
{
    setDataBoundariesDirty();
    d->attributesModel->setData(
        d->attributesModel->mapFromSource(index),
        qVariantFromValue( threeDAttrs ),
        ThreeDBarAttributesRole );
    //emit layoutChanged( this );
    emit propertiesChanged();
}

/**
  * @return the global 3D bar attributes
  */
ThreeDBarAttributes BarDiagram::threeDBarAttributes() const
{
    return qVariantValue<ThreeDBarAttributes>(
        d->attributesModel->data( KDChart::ThreeDBarAttributesRole ) );
}

/**
  * @return the 3D bar attributes of data set \a column
  */
ThreeDBarAttributes BarDiagram::threeDBarAttributes( int column ) const
{
    return qVariantValue<ThreeDBarAttributes>(
        d->attributesModel->data(
            d->attributesModel->mapFromSource( columnToIndex( column ) ),
            KDChart::ThreeDBarAttributesRole ) );
}

/**
  * @return the 3D bar attributes of the model index \a index
  */
ThreeDBarAttributes BarDiagram::threeDBarAttributes( const QModelIndex& index ) const
{
    return qVariantValue<ThreeDBarAttributes>(
        d->attributesModel->data(
            d->attributesModel->mapFromSource(index),
            KDChart::ThreeDBarAttributesRole ) );
}

double BarDiagram::threeDItemDepth( const QModelIndex& index ) const
{
    return threeDBarAttributes( index ).validDepth();
}

double BarDiagram::threeDItemDepth( int column ) const
{
    return qVariantValue<ThreeDBarAttributes>(
        d->attributesModel->headerData (
            column,
            Qt::Vertical,
            KDChart::ThreeDBarAttributesRole ) ).validDepth();
}

void BarDiagram::resizeEvent ( QResizeEvent*)
{

}

const QPair<QPointF, QPointF> BarDiagram::calculateDataBoundaries() const
{
    if ( !checkInvariants(true) ) return QPair<QPointF, QPointF>( QPointF( 0, 0 ), QPointF( 0, 0 ) );

    // note: calculateDataBoundaries() is ignoring the hidden flags.
    // That's not a bug but a feature: Hiding data does not mean removing them.
    // For totally removing data from KD Chart's view people can use e.g. a proxy model
    // calculate boundaries for different line types Normal - Stacked - Percent - Default Normal
    return d->implementor->calculateDataBoundaries();
}

void BarDiagram::paintEvent ( QPaintEvent*)
{
    QPainter painter ( viewport() );
    PaintContext ctx;
    ctx.setPainter ( &painter );
    ctx.setRectangle( QRectF ( 0, 0, width(), height() ) );
    paint ( &ctx );
}

void BarDiagram::paint( PaintContext* ctx )
{
    if ( !checkInvariants( true ) ) return;
    if ( !AbstractGrid::isBoundariesValid(dataBoundaries()) ) return;
    const PainterSaver p( ctx->painter() );
    if( model()->rowCount() == 0 || model()->columnCount() == 0 )
        return; // nothing to paint for us

    AbstractCoordinatePlane* const plane = ctx->coordinatePlane();
    ctx->setCoordinatePlane( plane->sharedAxisMasterPlane( ctx->painter() ) );

    // paint different bar types Normal - Stacked - Percent - Default Normal
    d->implementor->paint( ctx );

    ctx->setCoordinatePlane( plane );
}

void BarDiagram::resize( const QSizeF& size )
{
    d->compressor.setResolution( static_cast< int >( size.width() ),
                                 static_cast< int >( size.height() ) );
    setDataBoundariesDirty();
}

const int BarDiagram::numberOfAbscissaSegments () const
{
    return d->attributesModel->rowCount(attributesModelRootIndex());
}

const int BarDiagram::numberOfOrdinateSegments () const
{
    return d->attributesModel->columnCount(attributesModelRootIndex());
}

//#undef d
