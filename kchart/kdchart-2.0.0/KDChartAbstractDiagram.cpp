/****************************************************************************
 ** Copyright (C) 2006 Klarävdalens Datakonsult AB.  All rights reserved.
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
#include <QAbstractProxyModel>
#include <QStandardItemModel>
#include <QSizeF>

#include "KDChartAbstractCoordinatePlane.h"
#include "KDChartChart.h"
#include "KDChartDataValueAttributes.h"
#include "KDChartTextAttributes.h"
#include "KDChartMarkerAttributes.h"
#include "KDChartAbstractDiagram.h"
#include "KDChartAbstractDiagram_p.h"
#include "KDChartAttributesModel.h"
#include "KDChartAbstractThreeDAttributes.h"
#include "KDChartThreeDLineAttributes.h"
#include "KDChartPainterSaver_p.h"

#include <KDABLibFakes>


using namespace KDChart;

namespace KDChart {
  class PrivateAttributesModel : public AttributesModel {
    Q_OBJECT
  public:
        explicit PrivateAttributesModel( QAbstractItemModel* model, QObject * parent = 0 )
	  : AttributesModel(model,parent) {}
  };
}

AbstractDiagram::Private::Private()
  : plane( 0 )
  , attributesModel( new PrivateAttributesModel(0,0) )
  , allowOverlappingDataValueTexts( false )
  , antiAliasing( true )
  , percent( false )
  , datasetDimension( 1 )
  , databoundariesDirty(true)
{
}

AbstractDiagram::Private::~Private()
{
  if( attributesModel && qobject_cast<PrivateAttributesModel*>(attributesModel) )
    delete attributesModel;
}

void AbstractDiagram::Private::init()
{
}

void AbstractDiagram::Private::init( AbstractCoordinatePlane* newPlane )
{
    plane = newPlane;
}

void AbstractDiagram::Private::setAttributesModel( AttributesModel* amodel )
{
    if( !attributesModel.isNull() &&
	qobject_cast<PrivateAttributesModel*>(attributesModel) ) {
	delete attributesModel;
    }
    attributesModel = amodel;
}

AbstractDiagram::Private::Private( const AbstractDiagram::Private& rhs ) :
    // Do not copy the plane
    plane( 0 ),
    attributesModelRootIndex( QModelIndex() ),
    attributesModel( rhs.attributesModel ),
    allowOverlappingDataValueTexts( rhs.allowOverlappingDataValueTexts ),
    antiAliasing( rhs.antiAliasing ),
    percent( rhs.percent ),
    datasetDimension( rhs.datasetDimension )
{
    attributesModel = new PrivateAttributesModel( 0, 0);
    attributesModel->initFrom( rhs.attributesModel );
}

#define d d_func()

AbstractDiagram::AbstractDiagram ( QWidget* parent, AbstractCoordinatePlane* plane )
    : QAbstractItemView ( parent ), _d( new Private() )
{
    _d->init( plane );
}

AbstractDiagram::~AbstractDiagram()
{
    delete _d;
}

void AbstractDiagram::init()
{
}


AbstractCoordinatePlane* AbstractDiagram::coordinatePlane() const
{
    return d->plane;
}

const QPair<QPointF, QPointF> AbstractDiagram::dataBoundaries () const
{
    if( d->databoundariesDirty ){
        d->databoundaries = calculateDataBoundaries ();
        d->databoundariesDirty = false;
    }
    return d->databoundaries;
}

void AbstractDiagram::setDataBoundariesDirty() const
{
    d->databoundariesDirty = true;
}

void AbstractDiagram::setModel ( QAbstractItemModel * newModel )
{
  QAbstractItemView::setModel( newModel );
  AttributesModel* amodel = new PrivateAttributesModel( newModel, this );
  amodel->initFrom( d->attributesModel );
  d->setAttributesModel(amodel);
  scheduleDelayedItemsLayout();
  d->databoundariesDirty = true;
  emit modelsChanged();
}

/*! Sets an external AttributesModel on this diagram. By default, a diagram has it's
  own internal set of attributes, but an external one can be set. This can be used to
  share attributes between several diagrams. The diagram does not take ownership of the
  attributesmodel.
*/
void AbstractDiagram::setAttributesModel( AttributesModel* amodel )
{
    if( amodel->sourceModel() != model() ) {
	qWarning("KDChart::AbstractDiagram::setAttributesModel() failed: "
		 "Trying to set an attributesmodel which works on a different "
		 "model than the diagram.");
	return;
    }
    if( qobject_cast<PrivateAttributesModel*>(amodel) ) {
	qWarning("KDChart::AbstractDiagram::setAttributesModel() failed: "
		 "Trying to set an attributesmodel that is private to another diagram.");
	return;
    }
    d->setAttributesModel(amodel);
    scheduleDelayedItemsLayout();
    d->databoundariesDirty = true;
    emit modelsChanged();
}

/*! \returns a pointer to the AttributesModel currently used by this diagram. */
AttributesModel* AbstractDiagram::attributesModel() const
{
    return d->attributesModel;
}

/*! \reimpl */
void AbstractDiagram::setRootIndex ( const QModelIndex& idx )
{
    QAbstractItemView::setRootIndex(idx);
    setAttributesModelRootIndex( d->attributesModel->mapFromSource(idx) );
}

/*! \internal */
void AbstractDiagram::setAttributesModelRootIndex( const QModelIndex& idx )
{
  d->attributesModelRootIndex=idx;
  d->databoundariesDirty = true;
  scheduleDelayedItemsLayout();
}

/*! returns a QModelIndex pointing into the AttributesModel that corresponds to the
  root index of the diagram. */
QModelIndex AbstractDiagram::attributesModelRootIndex() const
{
    if ( !d->attributesModelRootIndex.isValid() )
        d->attributesModelRootIndex = d->attributesModel->mapFromSource( rootIndex() );
  return d->attributesModelRootIndex;
}

QModelIndex AbstractDiagram::columnToIndex( int column ) const
{
    if( model() )
        return QModelIndex( model()->index( 0, column, rootIndex() ) );
    return QModelIndex();
}

void AbstractDiagram::setCoordinatePlane( AbstractCoordinatePlane* parent )
{
    d->plane = parent;
}

void AbstractDiagram::doItemsLayout()
{
    if ( d->plane ) {
        d->plane->layoutDiagrams();
        update();
    }
    QAbstractItemView::doItemsLayout();
}

void AbstractDiagram::dataChanged( const QModelIndex &topLeft,
                                   const QModelIndex &bottomRight )
{
  // We are still too dumb to do intelligent updates...
  d->databoundariesDirty = true;
  scheduleDelayedItemsLayout();
}

void AbstractDiagram::setDataValueAttributes( const QModelIndex & index,
                                              const DataValueAttributes & a )
{
    d->attributesModel->setData(
        d->attributesModel->mapFromSource( index ),
        qVariantFromValue( a ),
        DataValueLabelAttributesRole );
    emit propertiesChanged();
}


void AbstractDiagram::setDataValueAttributes( int column, const DataValueAttributes & a )
{

    d->attributesModel->setHeaderData(
        column, Qt::Vertical,
        qVariantFromValue( a ), DataValueLabelAttributesRole );
    emit propertiesChanged();
}

DataValueAttributes AbstractDiagram::dataValueAttributes() const
{
    return qVariantValue<DataValueAttributes>( KDChart::DataValueLabelAttributesRole );
}

DataValueAttributes AbstractDiagram::dataValueAttributes( int column ) const
{
    return qVariantValue<DataValueAttributes>(
        attributesModel()->data( attributesModel()->mapFromSource(columnToIndex( column )),
        KDChart::DataValueLabelAttributesRole ) );
}

DataValueAttributes AbstractDiagram::dataValueAttributes( const QModelIndex & index ) const
{
    return qVariantValue<DataValueAttributes>(
        attributesModel()->data( attributesModel()->mapFromSource(index),
        KDChart::DataValueLabelAttributesRole ) );
}

void AbstractDiagram::setDataValueAttributes( const DataValueAttributes & a )
{
    d->attributesModel->setModelData( qVariantFromValue( a ), DataValueLabelAttributesRole );
    emit propertiesChanged();
}

void AbstractDiagram::setAllowOverlappingDataValueTexts( bool allow )
{
    d->allowOverlappingDataValueTexts = allow;
    emit propertiesChanged();
}

bool AbstractDiagram::allowOverlappingDataValueTexts() const
{
    return d->allowOverlappingDataValueTexts;
}

void AbstractDiagram::setAntiAliasing( bool enabled )
{
    d->antiAliasing = enabled;
    emit propertiesChanged();
}

bool AbstractDiagram::antiAliasing() const
{
    return d->antiAliasing;
}

void AbstractDiagram::setPercentMode ( bool percent )
{
    d->percent = percent;
    emit propertiesChanged();
}

bool AbstractDiagram::percentMode() const
{
    return d->percent;
}

void AbstractDiagram::paintDataValueText( QPainter* painter,
                                          const QModelIndex& index,
                                          const QPointF& pos,
                                          double value )
{
    // paint one data series
    DataValueAttributes a = dataValueAttributes(index);
    if ( !a.isVisible() ) return;

    // handle decimal digits
    int decimalDigits = a.decimalDigits();
    int decimalPos = QString::number(  value ).indexOf( QLatin1Char( '.' ) );
    QString roundedValue;
    if ( a.dataLabel().isNull() ) {
        if ( decimalPos > 0 && value != 0 )
            roundedValue =  roundValues ( value, decimalPos, decimalDigits );
        else
            roundedValue = QString::number(  value );
    } else
        roundedValue = a.dataLabel();
        // handle prefix and suffix
    if ( !a.prefix().isNull() )
        roundedValue.prepend( a.prefix() );

    if ( !a.suffix().isNull() )
        roundedValue.append( a.suffix() );

    PainterSaver painterSaver( painter );
    // FIXME draw the non-text bits, background, etc
    const TextAttributes &ta = a.textAttributes();
    if ( ta.isVisible() ) {
        painter->setPen( ta.pen() );
        painter->setFont( ta.font() );
        painter->translate( pos );
        painter->rotate( ta.rotation() );
        painter->drawText( QPointF(0, 0), roundedValue );
    }
}

QString  AbstractDiagram::roundValues( double value,
                                       const int decimalPos,
                                       const int decimalDigits ) const {

    QString digits( QString::number( value ).mid( decimalPos+1 ) );
    QString num( QString::number( value ) );
    num.truncate( decimalPos );
    int count = 0;
        for (  int i = digits.length(); i >= decimalDigits ; --i ) {
            count += 1;
            int lastval = QString( digits.data() [i] ).toInt();
            int val = QString( digits.data() [i-1] ) .toInt();
            if ( lastval >= 5 ) {
                val += 1;
                digits.replace( digits.length() - count,1 , QString::number( val ) );
            }
        }

    digits.truncate( decimalDigits );
    num.append( QLatin1Char( '.' ) + digits );

    return num;

}

void AbstractDiagram::paintDataValueTexts( QPainter* painter )
{
    if ( !checkInvariants() ) return;
    const int rowCount = model()->rowCount(rootIndex());
    const int columnCount = model()->columnCount(rootIndex());
    for ( int i=datasetDimension()-1; i<columnCount; i += datasetDimension() ) {
       for ( int j=0; j< rowCount; ++j ) {
           const QModelIndex index = model()->index( j, i, rootIndex() );
           double value = model()->data( index ).toDouble();
           const QPointF pos = coordinatePlane()->translate( QPointF( j, value ) );
           paintDataValueText( painter, index, pos, value );
       }
    }
}


void AbstractDiagram::paintMarker( QPainter* painter,
                                   const QModelIndex& index,
                                   const QPointF& pos )
{

    if ( !checkInvariants() ) return;
    DataValueAttributes a = dataValueAttributes(index);
    if ( !a.isVisible() ) return;
    const MarkerAttributes &ma = a.markerAttributes();
    if ( !ma.isVisible() ) return;

    PainterSaver painterSaver( painter );
    QSizeF maSize( ma.markerSize() );
    QBrush indexBrush( brush( index ) );
    QPen indexPen( ma.pen() );
    if ( ma.markerColor().isValid() )
        indexBrush.setColor( ma.markerColor() );

    paintMarker( painter, ma, indexBrush, indexPen, pos, maSize );
}


void AbstractDiagram::paintMarker( QPainter* painter,
                                   const MarkerAttributes& markerAttributes,
                                   const QBrush& brush,
                                   const QPen& pen,
                                   const QPointF& pos,
                                   const QSizeF& maSize )
{
    PainterSaver painterSaver( painter );
    painter->setBrush( brush );
    painter->setPen( pen );
    painter->setRenderHint ( QPainter::Antialiasing );
    painter->translate( pos );
    switch ( markerAttributes.markerStyle() ) {
        case MarkerAttributes::MarkerCircle:
            painter->drawEllipse( QRectF( 0 - maSize.height()/2, 0 - maSize.width()/2,
                        maSize.height(), maSize.width()) );
            break;
        case MarkerAttributes::MarkerSquare:
            {
                QRectF rect( 0 - maSize.height()/2, 0 - maSize.width()/2,
                            maSize.height(), maSize.width() );
                painter->drawRect( rect );
                painter->fillRect( rect, painter->brush() );
                break;
            }
        case MarkerAttributes::MarkerDiamond:
            {
                QVector <QPointF > diamondPoints;
                QPointF top, left, bottom, right;
                top    = QPointF( 0, 0 - maSize.height()/2 );
                left   = QPointF( 0 - maSize.width()/2, 0 );
                bottom = QPointF( 0, maSize.height()/2 );
                right  = QPointF( maSize.width()/2, 0 );
                diamondPoints << top << left << bottom << right;
                painter->drawPolygon( diamondPoints );
                break;
            }
        //Pending Michel: do we need that? Ask: What is the idea about
        // Marker1Pixel and Marker4Pixels.
        case MarkerAttributes::Marker1Pixel:
            {
                QSizeF pSize(4,4);
                QPen pen1Pixel;
                pen1Pixel.setColor( painter->background().color() );
                QRectF centerRect( -pSize.height()/2, -pSize.width()/2,
                                   pSize.height(), pSize.width() );
                painter->setPen( pen1Pixel );
                painter->drawEllipse( centerRect );
                break;
            }
        case MarkerAttributes::Marker4Pixels:
            {
                QSizeF pSize(8, 8);
                QPen pen4Pixel;
                pen4Pixel.setColor( painter->background().color() );
                QRectF centerRect( -pSize.height()/2, -pSize.width()/2,
                                   pSize.height(), pSize.width() );
                painter->setPen( pen4Pixel );
                painter->drawEllipse( centerRect );
                break;
            }
        case MarkerAttributes::MarkerRing:
            {
                painter->setBrush( Qt::NoBrush );
                painter->drawEllipse( QRectF( 0 - maSize.height()/2, 0 - maSize.width()/2,
                                      maSize.height(), maSize.width()) );
                break;
            }
        case MarkerAttributes::MarkerCross:
            {
                QVector <QPointF > crossPoints;
                QPointF leftTop, leftBottom, centerBottomLeft, bottomLeft, bottomRight,
                        centerBottomRight,rightBottom, rightTop, centerTopRight, topRight, topLeft,
                        centerTopLeft;
                leftTop           = QPointF( -maSize.width()/2, -maSize.height()/4 );
                leftBottom        = QPointF( -maSize.width()/2, maSize.height()/4 );
                centerBottomLeft  = QPointF( -maSize.width()/4, maSize.height()/4 );
                bottomLeft        = QPointF( -maSize.width()/4, maSize.height()/2 );
                bottomRight       = QPointF( maSize.width()/4,  maSize.height()/2 );
                centerBottomRight = QPointF( maSize.width()/4, maSize.height()/4 );
                rightBottom       = QPointF( maSize.width()/2, maSize.height()/4 );
                rightTop          = QPointF( maSize.width()/2, -maSize.height()/4 );
                centerTopRight    = QPointF( maSize.width()/4, -maSize.height()/4 );
                topRight          = QPointF( maSize.width()/4, -maSize.height()/2 );
                topLeft           = QPointF( -maSize.width()/4, -maSize.height()/2 );
                centerTopLeft     = QPointF( -maSize.width()/4, -maSize.height()/4 );

                crossPoints << leftTop << leftBottom << centerBottomLeft
                    << bottomLeft << bottomRight << centerBottomRight
                    << rightBottom << rightTop << centerTopRight
                    << topRight << topLeft << centerTopLeft;

                painter->drawPolygon( crossPoints );
                break;
            }
        case MarkerAttributes::MarkerFastCross:
            {
                QPointF left, right, top, bottom;
                left  = QPointF( -maSize.width()/2, 0 );
                right = QPointF( maSize.width()/2, 0 );
                top   = QPointF( 0, -maSize.height()/2 );
                bottom= QPointF( 0, maSize.height()/2 );
                painter->drawLine( left, right );
                painter->drawLine(  top, bottom );
                break;
            }
        default:
            Q_ASSERT_X ( false, "paintMarkers()",
                         "Type item does not match a defined Marker Type." );
    }
}

void AbstractDiagram::paintMarkers( QPainter* painter )
{
    if ( !checkInvariants() ) return;
    const int rowCount = model()->rowCount(rootIndex());
    const int columnCount = model()->columnCount(rootIndex());
    for ( int i=datasetDimension()-1; i<columnCount; i += datasetDimension() ) {
       for ( int j=0; j< rowCount; ++j ) {
           const QModelIndex index = model()->index( j, i, rootIndex() );
           double value = model()->data( index ).toDouble();
           const QPointF pos = coordinatePlane()->translate( QPointF( j, value ) );
           paintMarker( painter, index, pos );
       }
    }
}


void AbstractDiagram::setPen( const QModelIndex& index, const QPen& pen )
{
    attributesModel()->setData(
        attributesModel()->mapFromSource( index ),
        qVariantFromValue( pen ), DatasetPenRole );
    emit propertiesChanged();
}

void AbstractDiagram::setPen( const QPen& pen )
{
    attributesModel()->setModelData(
        qVariantFromValue( pen ), DatasetPenRole );
    emit propertiesChanged();
}

void AbstractDiagram::setPen( int column,const QPen& pen )
{
    attributesModel()->setHeaderData(
        column, Qt::Vertical,
        qVariantFromValue( pen ),
        DatasetPenRole );
    emit propertiesChanged();
}

QPen AbstractDiagram::pen() const
{
    return qVariantValue<QPen>(
        attributesModel()->data( DatasetPenRole ) );
}

QPen AbstractDiagram::pen( int dataset ) const
{
    return qVariantValue<QPen>(
        attributesModel()->data(
            attributesModel()->mapFromSource( columnToIndex( dataset ) ),
            DatasetPenRole ) );
}

QPen AbstractDiagram::pen( const QModelIndex& index ) const
{
    return qVariantValue<QPen>(
        attributesModel()->data(
            attributesModel()->mapFromSource( index ),
            DatasetPenRole ) );
}

void AbstractDiagram::setBrush( const QModelIndex& index, const QBrush& brush )
{
    attributesModel()->setData(
        attributesModel()->mapFromSource( index ),
        qVariantFromValue( brush ), DatasetBrushRole );
    emit propertiesChanged();
}

void AbstractDiagram::setBrush( const QBrush& brush )
{
    attributesModel()->setModelData(
        qVariantFromValue( brush ), DatasetBrushRole );
    emit propertiesChanged();
}

void AbstractDiagram::setBrush( int column, const QBrush& brush )
{
    attributesModel()->setHeaderData(
        column, Qt::Vertical,
        qVariantFromValue( brush ),
        DatasetBrushRole );
    emit propertiesChanged();
}

QBrush AbstractDiagram::brush() const
{
    return qVariantValue<QBrush>(
        attributesModel()->data( DatasetBrushRole ) );
}

QBrush AbstractDiagram::brush( int dataset ) const
{
    return qVariantValue<QBrush>(
        attributesModel()->data(
            attributesModel()->mapFromSource( columnToIndex( dataset ) ),
            DatasetBrushRole ) );
}

QBrush AbstractDiagram::brush( const QModelIndex& index ) const
{
    return qVariantValue<QBrush>(
        attributesModel()->data(
            attributesModel()->mapFromSource( index ),
            DatasetBrushRole ) );
}

// implement QAbstractItemView:
QRect AbstractDiagram::visualRect(const QModelIndex &) const
{
    return QRect();
}

void AbstractDiagram::scrollTo(const QModelIndex &, ScrollHint )
{}

QModelIndex AbstractDiagram::indexAt(const QPoint &) const
{ return QModelIndex(); }

QModelIndex AbstractDiagram::moveCursor(CursorAction, Qt::KeyboardModifiers )
{ return QModelIndex(); }

int AbstractDiagram::horizontalOffset() const
{ return 0; }

int AbstractDiagram::verticalOffset() const
{ return 0; }

bool AbstractDiagram::isIndexHidden(const QModelIndex &) const
{ return true; }

void AbstractDiagram::setSelection(const QRect &, QItemSelectionModel::SelectionFlags)
{}

QRegion AbstractDiagram::visualRegionForSelection(const QItemSelection &) const
{ return QRegion(); }


void KDChart::AbstractDiagram::useDefaultColors( )
{
    d->attributesModel->setPaletteType( AttributesModel::PaletteTypeDefault );
}

void KDChart::AbstractDiagram::useSubduedColors( )
{
    d->attributesModel->setPaletteType( AttributesModel::PaletteTypeSubdued );
}

void KDChart::AbstractDiagram::useRainbowColors( )
{
    d->attributesModel->setPaletteType( AttributesModel::PaletteTypeRainbow );
}

QStringList AbstractDiagram::itemRowLabels() const
{
    QStringList ret;
    //qDebug() << "AbstractDiagram::itemRowLabels(): " << attributesModel()->rowCount(attributesModelRootIndex()) << "entries";
    const int rowCount = attributesModel()->rowCount(attributesModelRootIndex());
    for( int i = 0; i < rowCount; ++i ){
        //qDebug() << "label: " << attributesModel()->headerData( i, Qt::Vertical, Qt::DisplayRole ).toString();
        ret << attributesModel()->headerData( i, Qt::Vertical, Qt::DisplayRole ).toString();
    }
    return ret;
}

QStringList AbstractDiagram::datasetLabels() const
{
    QStringList ret;
    //qDebug() << "AbstractDiagram::datasetLabels(): " << attributesModel()->columnCount(attributesModelRootIndex()) << "entries";
    const int columnCount = attributesModel()->columnCount(attributesModelRootIndex());
    for( int i = datasetDimension()-1; i < columnCount; i += datasetDimension() ){
        //qDebug() << "label: " << attributesModel()->headerData( i, Qt::Horizontal, Qt::DisplayRole ).toString();
        ret << attributesModel()->headerData( i, Qt::Horizontal, Qt::DisplayRole ).toString();
    }
    return ret;
}

QList<QBrush> AbstractDiagram::datasetBrushes() const
{
    QList<QBrush> ret;
    const int columnCount = attributesModel()->columnCount(attributesModelRootIndex());
    for( int i = datasetDimension()-1; i < columnCount; i += datasetDimension() ) {
        QBrush brush = qVariantValue<QBrush>( attributesModel()->headerData( i, Qt::Vertical, DatasetBrushRole ) );
        ret << brush;
    }

    return ret;
}

QList<QPen> AbstractDiagram::datasetPens() const
{
    QList<QPen> ret;
    const int columnCount = attributesModel()->columnCount(attributesModelRootIndex());
    for( int i = datasetDimension()-1; i < columnCount; i += datasetDimension() ) {
        QPen pen = qVariantValue<QPen>( attributesModel()->headerData( i, Qt::Vertical, DatasetPenRole ) );
        ret << pen;
    }
    return ret;
}

QList<MarkerAttributes> AbstractDiagram::datasetMarkers() const
{
    QList<MarkerAttributes> ret;
    const int columnCount = attributesModel()->columnCount(attributesModelRootIndex());
    for( int i = datasetDimension()-1; i < columnCount; i += datasetDimension() ) {
        DataValueAttributes a =
            qVariantValue<DataValueAttributes>( attributesModel()->headerData( i, Qt::Vertical, DataValueLabelAttributesRole ) );
        const MarkerAttributes &ma = a.markerAttributes();
        ret << ma;
    }
    return ret;
}

bool AbstractDiagram::checkInvariants( bool justReturnTheStatus ) const
{
    if( ! justReturnTheStatus ){
        Q_ASSERT_X ( model(), "AbstractDiagram::checkInvariants()",
                    "There is no usable model set, for the diagram." );

        Q_ASSERT_X ( coordinatePlane(), "AbstractDiagram::checkInvariants()",
                    "There is no usable coordinate plane set, for the diagram." );
    }
    return model() && coordinatePlane();
}

int AbstractDiagram::datasetDimension( ) const
{
    return d->datasetDimension;
}

void AbstractDiagram::setDatasetDimension( int dimension )
{
    if ( d->datasetDimension == dimension ) return;
    d->datasetDimension = dimension;
    d->databoundariesDirty = true;
    emit layoutChanged( this );
}

double AbstractDiagram::valueForCell( int row, int column ) const
{
    return d->attributesModel->data(
            d->attributesModel->index( row, column, attributesModelRootIndex() ) ).toDouble();
}

void AbstractDiagram::update() const
{
    //qDebug("KDChart::AbstractDiagram::update() called");
    if( d->plane )
        d->plane->update();
}


#include "KDChartAbstractDiagram.moc"
