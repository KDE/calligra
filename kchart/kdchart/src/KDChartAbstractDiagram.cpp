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
#include <QApplication>
#include <QAbstractProxyModel>
#include <QAbstractTextDocumentLayout>
#include <QStandardItemModel>
#include <QSizeF>
#include <QTextDocument>

#include "KDChartAbstractCoordinatePlane.h"
#include "KDChartChart.h"
#include "KDChartDataValueAttributes.h"
#include "KDChartTextAttributes.h"
#include "KDChartMarkerAttributes.h"
#include "KDChartAbstractDiagram.h"
#include "KDChartAbstractDiagram_p.h"
#include "KDChartAbstractThreeDAttributes.h"
#include "KDChartThreeDLineAttributes.h"

#include <KDABLibFakes>


using namespace KDChart;

AbstractDiagram::Private::Private()
  : plane( 0 )
  , attributesModel( new PrivateAttributesModel(0,0) )
  , allowOverlappingDataValueTexts( false )
  , antiAliasing( true )
  , percent( false )
  , datasetDimension( 1 )
  , databoundariesDirty(true)
  , lastRoundedValue()
  , lastX( 0 )
  , mCachedFontMetrics( QFontMetrics( qApp->font() ) )
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

bool AbstractDiagram::Private::usesExternalAttributesModel()const
{
    return ( ! attributesModel.isNull() ) &&
           ( ! qobject_cast<PrivateAttributesModel*>(attributesModel) );
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
    datasetDimension( rhs.datasetDimension ),
    mCachedFontMetrics( rhs.cachedFontMetrics() )
{
    attributesModel = new PrivateAttributesModel( 0, 0);
    attributesModel->initFrom( rhs.attributesModel );
}

#define d d_func()

AbstractDiagram::AbstractDiagram ( QWidget* parent, AbstractCoordinatePlane* plane )
    : QAbstractItemView ( parent ), _d( new Private() )
{
    _d->init( plane );
    init();
}

AbstractDiagram::~AbstractDiagram()
{
    delete _d;
}

void AbstractDiagram::init()
{
    d->reverseMapper.setDiagram( this );
}


bool AbstractDiagram::compare( const AbstractDiagram* other )const
{
    if( other == this ) return true;
    if( ! other ){
        //qDebug() << "AbstractDiagram::compare() cannot compare to Null pointer";
        return false;
    }
    /*
    qDebug() << "\n             AbstractDiagram::compare() QAbstractScrollArea:";
            // compare QAbstractScrollArea properties
    qDebug() <<
            ((horizontalScrollBarPolicy() == other->horizontalScrollBarPolicy()) &&
            (verticalScrollBarPolicy()    == other->verticalScrollBarPolicy()));
    qDebug() << "AbstractDiagram::compare() QFrame:";
            // compare QFrame properties
    qDebug() <<
            ((frameShadow() == other->frameShadow()) &&
            (frameShape()   == other->frameShape()) &&
            (frameWidth()   == other->frameWidth()) &&
            (lineWidth()    == other->lineWidth()) &&
            (midLineWidth() == other->midLineWidth()));
    qDebug() << "AbstractDiagram::compare() QAbstractItemView:";
            // compare QAbstractItemView properties
    qDebug() <<
            ((alternatingRowColors() == other->alternatingRowColors()) &&
            (hasAutoScroll()         == other->hasAutoScroll()) &&
#if QT_VERSION > 0x040199
            (dragDropMode()          == other->dragDropMode()) &&
            (dragDropOverwriteMode() == other->dragDropOverwriteMode()) &&
            (horizontalScrollMode()  == other->horizontalScrollMode ()) &&
            (verticalScrollMode()    == other->verticalScrollMode()) &&
#endif
            (dragEnabled()           == other->dragEnabled()) &&
            (editTriggers()          == other->editTriggers()) &&
            (iconSize()              == other->iconSize()) &&
            (selectionBehavior()     == other->selectionBehavior()) &&
            (selectionMode()         == other->selectionMode()) &&
            (showDropIndicator()     == other->showDropIndicator()) &&
            (tabKeyNavigation()      == other->tabKeyNavigation()) &&
            (textElideMode()         == other->textElideMode()));
    qDebug() << "AbstractDiagram::compare() AttributesModel: ";
            // compare all of the properties stored in the attributes model
    qDebug() << attributesModel()->compare( other->attributesModel() );
    qDebug() << "AbstractDiagram::compare() own:";
            // compare own properties
    qDebug() <<
            ((rootIndex().column()            == other->rootIndex().column()) &&
            (rootIndex().row()                == other->rootIndex().row()) &&
            (allowOverlappingDataValueTexts() == other->allowOverlappingDataValueTexts()) &&
            (antiAliasing()                   == other->antiAliasing()) &&
            (percentMode()                    == other->percentMode()) &&
            (datasetDimension()               == other->datasetDimension()));
    */
    return  // compare QAbstractScrollArea properties
            (horizontalScrollBarPolicy() == other->horizontalScrollBarPolicy()) &&
            (verticalScrollBarPolicy()   == other->verticalScrollBarPolicy()) &&
            // compare QFrame properties
            (frameShadow()  == other->frameShadow()) &&
            (frameShape()   == other->frameShape()) &&
// frameWidth is a read-only property defined by the style, it should not be in here:
            // (frameWidth()   == other->frameWidth()) &&
            (lineWidth()    == other->lineWidth()) &&
            (midLineWidth() == other->midLineWidth()) &&
            // compare QAbstractItemView properties
            (alternatingRowColors()  == other->alternatingRowColors()) &&
            (hasAutoScroll()         == other->hasAutoScroll()) &&
#if QT_VERSION > 0x040199
            (dragDropMode()          == other->dragDropMode()) &&
            (dragDropOverwriteMode() == other->dragDropOverwriteMode()) &&
            (horizontalScrollMode()  == other->horizontalScrollMode ()) &&
            (verticalScrollMode()    == other->verticalScrollMode()) &&
#endif
            (dragEnabled()           == other->dragEnabled()) &&
            (editTriggers()          == other->editTriggers()) &&
            (iconSize()              == other->iconSize()) &&
            (selectionBehavior()     == other->selectionBehavior()) &&
            (selectionMode()         == other->selectionMode()) &&
            (showDropIndicator()     == other->showDropIndicator()) &&
            (tabKeyNavigation()      == other->tabKeyNavigation()) &&
            (textElideMode()         == other->textElideMode()) &&
            // compare all of the properties stored in the attributes model
            attributesModel()->compare( other->attributesModel() ) &&
            // compare own properties
            (rootIndex().column()             == other->rootIndex().column()) &&
            (rootIndex().row()                == other->rootIndex().row()) &&
            (allowOverlappingDataValueTexts() == other->allowOverlappingDataValueTexts()) &&
            (antiAliasing()                   == other->antiAliasing()) &&
            (percentMode()                    == other->percentMode()) &&
            (datasetDimension()               == other->datasetDimension());
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

void AbstractDiagram::setModel( QAbstractItemModel * newModel )
{
  QAbstractItemView::setModel( newModel );
  AttributesModel* amodel = new PrivateAttributesModel( newModel, this );
  amodel->initFrom( d->attributesModel );
  d->setAttributesModel(amodel);
  scheduleDelayedItemsLayout();
  setDataBoundariesDirty();
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
    setDataBoundariesDirty();
    emit modelsChanged();
}

bool AbstractDiagram::usesExternalAttributesModel()const
{
    return d->usesExternalAttributesModel();
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
    setDataBoundariesDirty();
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
{   // FIXME (Mirko): shouldn't this be headerData? instead of the index for the first row?
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
    Q_UNUSED( topLeft );
    Q_UNUSED( bottomRight );
    // We are still too dumb to do intelligent updates...
    setDataBoundariesDirty();
    scheduleDelayedItemsLayout();
}


void AbstractDiagram::setHidden( const QModelIndex & index, bool hidden )
{
    d->attributesModel->setData(
        d->attributesModel->mapFromSource( index ),
        qVariantFromValue( hidden ),
        DataHiddenRole );
    emit dataHidden();
}

void AbstractDiagram::setHidden( int column, bool hidden )
{
    d->attributesModel->setHeaderData(
        column, Qt::Vertical,
        qVariantFromValue( hidden ),
        DataHiddenRole );
    emit dataHidden();
}

void AbstractDiagram::setHidden( bool hidden )
{
    d->attributesModel->setModelData(
        qVariantFromValue( hidden ),
        DataHiddenRole );
    emit dataHidden();
}

bool AbstractDiagram::isHidden() const
{
    return qVariantValue<bool>(
        attributesModel()->modelData( DataHiddenRole ) );
}

bool AbstractDiagram::isHidden( int column ) const
{
    return qVariantValue<bool>(
        attributesModel()->data(
            attributesModel()->mapFromSource(columnToIndex( column )),
            DataHiddenRole ) );
}
bool AbstractDiagram::isHidden( const QModelIndex & index ) const
{
    return qVariantValue<bool>(
        attributesModel()->data(
            attributesModel()->mapFromSource(index),
            DataHiddenRole ) );
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
    return qVariantValue<DataValueAttributes>(
        attributesModel()->modelData( KDChart::DataValueLabelAttributesRole ) );
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
    const DataValueAttributes a( dataValueAttributes(index) );
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

    const TextAttributes ta( a.textAttributes() );
    // FIXME draw the non-text bits, background, etc
    if ( ta.isVisible() ) {

        QPointF pt( pos );
        /* for debugging:
        PainterSaver painterSaver( painter );
        painter->setPen( Qt::black );
        painter->drawLine( pos - QPointF( 1,1), pos + QPointF( 1,1) );
        painter->drawLine( pos - QPointF(-1,1), pos + QPointF(-1,1) );
        */

        QTextDocument doc;
        if( Qt::mightBeRichText( roundedValue ) )
            doc.setHtml( roundedValue );
        else
            doc.setPlainText( roundedValue );

        const RelativePosition relPos( a.position( value >= 0.0 ) );
        const Qt::Alignment alignBottomLeft = Qt::AlignBottom | Qt::AlignLeft;
        const QFont calculatedFont( ta.calculatedFont( d->plane, KDChartEnums::MeasureOrientationMinimum ) );
        const QRectF boundRect( d->cachedFontMetrics( calculatedFont, painter->device() )->boundingRect( doc.toPlainText() ) );

        // To place correctly
        pt.ry() -= boundRect.height();

        //qDebug() << "calculatedFont's point size:" << calculatedFont.pointSizeF();
        // adjust the text start point position, if alignment is not Bottom/Left
        if( (relPos.alignment() & alignBottomLeft) != alignBottomLeft ){
            if( relPos.alignment() & Qt::AlignRight )
                pt.rx() -= boundRect.width();
            else if( relPos.alignment() & Qt::AlignHCenter )
                pt.rx() -= 0.5 * boundRect.width();

            if( relPos.alignment() & Qt::AlignTop )
                pt.ry() += boundRect.height();
            else if( relPos.alignment() & Qt::AlignVCenter )
                pt.ry() += 0.5 * boundRect.height();
        }

        // FIXME draw the non-text bits, background, etc

        if ( a.showRepetitiveDataLabels() ||
             pos.x() <= d->lastX ||
             d->lastRoundedValue != roundedValue ) {
            d->lastRoundedValue = roundedValue;
            d->lastX = pos.x();
            PainterSaver painterSaver( painter );

            doc.setDefaultFont( calculatedFont );
            QAbstractTextDocumentLayout::PaintContext context;
            context.palette = palette();
            context.palette.setColor(QPalette::Text, ta.pen().color() );

            painter->translate( pt );
            painter->rotate( ta.rotation() );

            QAbstractTextDocumentLayout* layout = doc.documentLayout();
            layout->draw( painter, context );
        }
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

    // workaround: BC cannot be changed, otherwise we would pass the
    // index down to next-lower paintMarker function. So far, we
    // basically save a circle of radius maSize at pos in the
    // reverseMapper. This means that ^^^ this version of paintMarker
    // needs to be called to reverse-map the marker.
    d->reverseMapper.addCircle( index.row(), index.column(), pos, 2 * maSize );
}

void AbstractDiagram::paintMarker( QPainter* painter,
                                   const MarkerAttributes& markerAttributes,
                                   const QBrush& brush,
                                   const QPen& pen,
                                   const QPointF& pos,
                                   const QSizeF& maSize )
{
    const QPen oldPen( painter->pen() );
    // Pen is used to paint 4Pixels - 1 Pixel - Ring and FastCross types.
    // make sure to use the brush color - see above in those cases.
    const bool isFourPixels = (markerAttributes.markerStyle() == MarkerAttributes::Marker4Pixels);
    if( isFourPixels || (markerAttributes.markerStyle() == MarkerAttributes::Marker1Pixel) ){
        // for high-performance point charts with tiny point markers:
        painter->setPen( QPen( brush.color().light() ) );
        if( isFourPixels ){
            const qreal x = pos.x();
            const qreal y = pos.y();
            painter->drawLine( QPointF(x-1.0,y-1.0),
                               QPointF(x+1.0,y-1.0) );
            painter->drawLine( QPointF(x-1.0,y),
                               QPointF(x+1.0,y) );
            painter->drawLine( QPointF(x-1.0,y+1.0),
                               QPointF(x+1.0,y+1.0) );
        }
        painter->drawPoint( pos );
    }else{
        PainterSaver painterSaver( painter );
        // we only a solid line surrounding the markers
        QPen painterPen( pen );
        painterPen.setStyle( Qt::SolidLine );
        painter->setPen( painterPen );
        painter->setBrush( brush );
        painter->setRenderHint ( QPainter::Antialiasing );
        painter->translate( pos );
        switch ( markerAttributes.markerStyle() ) {
            case MarkerAttributes::MarkerCircle:
                painter->drawEllipse( QRectF( 0 - maSize.height()/2, 0 - maSize.width()/2,
                            maSize.height(), maSize.width()) );
                break;
            case MarkerAttributes::MarkerSquare:
                {
                    QRectF rect( 0 - maSize.width()/2, 0 - maSize.height()/2,
                                maSize.width(), maSize.height() );
                    painter->drawRect( rect );
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
            // both handled on top of the method:
            case MarkerAttributes::Marker1Pixel:
            case MarkerAttributes::Marker4Pixels:
                    break;
            case MarkerAttributes::MarkerRing:
                {
                    painter->setPen( QPen( brush.color() ) );
                    painter->setBrush( Qt::NoBrush );
                    painter->drawEllipse( QRectF( 0 - maSize.height()/2, 0 - maSize.width()/2,
                                        maSize.height(), maSize.width()) );
                    break;
                }
            case MarkerAttributes::MarkerCross:
                {
                    QRectF rect( maSize.width()*-0.5, maSize.height()*-0.2,
                                 maSize.width(), maSize.height()*0.4 );
                    painter->drawRect( rect );
                    rect.setTopLeft(QPointF( maSize.width()*-0.2, maSize.height()*-0.5 ));
                    rect.setSize(QSizeF( maSize.width()*0.4, maSize.height() ));
                    painter->drawRect( rect );
                    break;
                }
            case MarkerAttributes::MarkerFastCross:
                {
                    QPointF left, right, top, bottom;
                    left  = QPointF( -maSize.width()/2, 0 );
                    right = QPointF( maSize.width()/2, 0 );
                    top   = QPointF( 0, -maSize.height()/2 );
                    bottom= QPointF( 0, maSize.height()/2 );
                    painter->setPen( QPen( brush.color() ) );
                    painter->drawLine( left, right );
                    painter->drawLine(  top, bottom );
                    break;
                }
            default:
                Q_ASSERT_X ( false, "paintMarkers()",
                            "Type item does not match a defined Marker Type." );
        }
    }
    painter->setPen( oldPen );
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
    if( datasetDimension() > 1 )
        return setPen( index.column(), pen );
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
    if( datasetDimension() > 1 )
        column *= datasetDimension();
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
    if( datasetDimension() > 1 )
        dataset /= datasetDimension();

    return qVariantValue<QPen>(
        attributesModel()->data(
            attributesModel()->mapFromSource( columnToIndex( dataset ) ),
            DatasetPenRole ) );
}

QPen AbstractDiagram::pen( const QModelIndex& index ) const
{
    if( datasetDimension() > 1 )
        return pen( index.column() );
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
    if( datasetDimension() > 1 )
        dataset /= datasetDimension();
    return qVariantValue<QBrush>(
        attributesModel()->data(
            attributesModel()->mapFromSource( columnToIndex( dataset ) ),
            DatasetBrushRole ) );
}

QBrush AbstractDiagram::brush( const QModelIndex& index ) const
{
    if( datasetDimension() > 1 )
        return brush( index.column() );
    return qVariantValue<QBrush>(
        attributesModel()->data(
            attributesModel()->mapFromSource( index ),
            DatasetBrushRole ) );
}

/**
  * Sets the unit prefix for one value
  * @param prefix the prefix to be set
  * @param column the value using that prefix
  * @param orientation the orientantion of the axis to set
  */
void AbstractDiagram::setUnitPrefix( const QString& prefix, int column, Qt::Orientation orientation )
{
    d->unitPrefixMap[ column ][ orientation ]= prefix;
}

/**
  * Sets the unit prefix for all values
  * @param prefix the prefix to be set
  * @param orientation the orientantion of the axis to set
  */
void AbstractDiagram::setUnitPrefix( const QString& prefix, Qt::Orientation orientation )
{
    d->unitPrefix[ orientation ] = prefix;
}

/**
  * Sets the unit suffix for one value
  * @param suffix the suffix to be set
  * @param column the value using that suffix
  * @param orientation the orientantion of the axis to set
  */
void AbstractDiagram::setUnitSuffix( const QString& suffix, int column, Qt::Orientation orientation )
{
    d->unitSuffixMap[ column ][ orientation ]= suffix;
}

/**
  * Sets the unit suffix for all values
  * @param suffix the suffix to be set
  * @param orientation the orientantion of the axis to set
  */
void AbstractDiagram::setUnitSuffix( const QString& suffix, Qt::Orientation orientation )
{
    d->unitSuffix[ orientation ] = suffix;
}

/**
  * Returns the unit prefix for a special value
  * @param column the value which's prefix is requested
  * @param orientation the orientation of the axis
  * @param fallback if true, the global prefix is return when no specific one is set for that value
  * @return the unit prefix
  */
QString AbstractDiagram::unitPrefix( int column, Qt::Orientation orientation, bool fallback ) const
{
    if( !fallback || d->unitPrefixMap[ column ].contains( orientation ) )
        return d->unitPrefixMap[ column ][ orientation ];
    return d->unitPrefix[ orientation ];
}

/** Returns the global unit prefix
  * @param orientation the orientation of the axis
  * @return the unit prefix
  */
QString AbstractDiagram::unitPrefix( Qt::Orientation orientation ) const
{
    return d->unitPrefix[ orientation ];
}

/**
  * Returns the unit suffix for a special value
  * @param column the value which's suffix is requested
  * @param orientation the orientation of the axis
  * @param fallback if true, the global suffix is return when no specific one is set for that value
  * @return the unit suffix
  */
QString AbstractDiagram::unitSuffix( int column, Qt::Orientation orientation, bool fallback ) const
{
    if( !fallback || d->unitSuffixMap[ column ].contains( orientation ) )
        return d->unitSuffixMap[ column ][ orientation ];
    return d->unitSuffix[ orientation ];
}

/** Returns the global unit suffix
  * @param orientation the orientation of the axis
  * @return the unit siffix
  */
QString AbstractDiagram::unitSuffix( Qt::Orientation orientation ) const
{
    return d->unitSuffix[ orientation ];
}

// implement QAbstractItemView:
QRect AbstractDiagram::visualRect(const QModelIndex &) const
{
    return QRect();
}

void AbstractDiagram::scrollTo(const QModelIndex &, ScrollHint )
{}

// indexAt ... down below

QModelIndex AbstractDiagram::moveCursor(CursorAction, Qt::KeyboardModifiers )
{ return QModelIndex(); }

int AbstractDiagram::horizontalOffset() const
{ return 0; }

int AbstractDiagram::verticalOffset() const
{ return 0; }

bool AbstractDiagram::isIndexHidden(const QModelIndex &) const
{ return true; }

void AbstractDiagram::setSelection(const QRect& rect , QItemSelectionModel::SelectionFlags command )
{
    const QModelIndexList indexes = d->indexesIn( rect );
    QItemSelection selection;
    KDAB_FOREACH( const QModelIndex& index, indexes )
    {
        selection.append( QItemSelectionRange( index ) );
    }
    selectionModel()->select( selection, command );
}

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
    if( model() ){
        //qDebug() << "AbstractDiagram::itemRowLabels(): " << attributesModel()->rowCount(attributesModelRootIndex()) << "entries";
        const int rowCount = attributesModel()->rowCount(attributesModelRootIndex());
        for( int i = 0; i < rowCount; ++i ){
            //qDebug() << "item row label: " << attributesModel()->headerData( i, Qt::Vertical, Qt::DisplayRole ).toString();
            ret << unitPrefix( i, Qt::Horizontal, true ) +
                   attributesModel()->headerData( i, Qt::Vertical, Qt::DisplayRole ).toString() +
                   unitSuffix( i, Qt::Horizontal, true );
        }
    }
    return ret;
}

QStringList AbstractDiagram::datasetLabels() const
{
    QStringList ret;
    if( model() == 0 )
        return ret;
    
    const int columnCount = attributesModel()->columnCount(attributesModelRootIndex());
    for( int i = 0; i < columnCount / datasetDimension(); ++i )
        ret << attributesModel()->headerData( i, Qt::Horizontal, Qt::DisplayRole ).toString();
    
    return ret;
}

QList<QBrush> AbstractDiagram::datasetBrushes() const
{
    QList<QBrush> ret;
    if( model() == 0 )
        return ret;

    const int columnCount = attributesModel()->columnCount(attributesModelRootIndex());
    for( int i = 0; i < columnCount / datasetDimension(); ++i )
        ret << qVariantValue<QBrush>( attributesModel()->headerData( i, Qt::Vertical, DatasetBrushRole ) );

    return ret;
}

QList<QPen> AbstractDiagram::datasetPens() const
{
    QList<QPen> ret;
    if( model() == 0 )
        return ret;
    
    const int columnCount = attributesModel()->columnCount(attributesModelRootIndex());
    for( int i = 0; i < columnCount / datasetDimension(); ++i )
        ret << qVariantValue<QPen>( attributesModel()->headerData( i, Qt::Vertical, DatasetPenRole ) );
    
    return ret;
}

QList<MarkerAttributes> AbstractDiagram::datasetMarkers() const
{
    QList<MarkerAttributes> ret;
    if( model() == 0 )
        return ret;
    
    const int columnCount = attributesModel()->columnCount(attributesModelRootIndex());
    for( int i = 0; i < columnCount / datasetDimension(); ++i )
    {
        const DataValueAttributes a =
            qVariantValue<DataValueAttributes>( attributesModel()->headerData( i, Qt::Vertical, DataValueLabelAttributesRole ) );
        ret << a.markerAttributes();
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
    setDataBoundariesDirty();
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

QModelIndex AbstractDiagram::indexAt( const QPoint& point ) const
{
    return d->indexAt( point );
}

QModelIndexList AbstractDiagram::indexesAt( const QPoint& point ) const
{
    return d->indexesAt( point );
}

