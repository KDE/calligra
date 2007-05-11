/****************************************************************************
 ** Copyright (C) 2006 Klarävdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundtion and appearing in the file LICENSE.GPL included in the
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

#include <QDebug>
#include <QPainter>
#include <QString>
#include <QPen>
#include <QVector>

#include "KDChartLineDiagram.h"
#include "KDChartLineDiagram_p.h"
#include "KDChartPaintContext.h"
#include "KDChartPalette.h"
#include "KDChartTextAttributes.h"
#include "KDChartThreeDLineAttributes.h"
#include "KDChartAttributesModel.h"
#include "KDChartPainterSaver_p.h"
#include "KDChartAbstractGrid.h"
#include "KDChartDataValueAttributes.h"

#include <KDABLibFakes>


using namespace KDChart;

LineDiagram::Private::Private()
  :lineType ( Normal )
{
}

LineDiagram::Private::~Private() {}


#define d d_func()


LineDiagram::LineDiagram( QWidget* parent, CartesianCoordinatePlane* plane ) :
    AbstractCartesianDiagram( new Private(), parent, plane )
{
    init();
}

void LineDiagram::init()
{
}

LineDiagram::~LineDiagram()
{
}

LineDiagram * LineDiagram::clone() const
{
    return new LineDiagram( new Private( *d ) );
}

void LineDiagram::setType( const LineType type )
{
   if ( d->lineType == type ) return;
   if ( type != LineDiagram::Normal && datasetDimension() > 1 ) {
       Q_ASSERT_X ( false, "setType()",
                    "This line chart type can't be used with multi-dimensional data." );
       return;
   }
   d->lineType = type;
   // AbstractAxis settings - see AbstractDiagram and CartesianAxis
   setPercentMode( type == LineDiagram::Percent );
   setDataBoundariesDirty();
   emit layoutChanged( this );
   emit propertiesChanged();
}

LineDiagram::LineType LineDiagram::type() const
{
   return d->lineType;
}

void LineDiagram::setLineAttributes( const LineAttributes & ta )
{
     d->attributesModel->setModelData(
        qVariantFromValue( ta ),
        LineAttributesRole );
   emit propertiesChanged();
}

void LineDiagram::setLineAttributes(
    int column,
    const LineAttributes & ta )
{
    d->attributesModel->setHeaderData(
        column,
        Qt::Vertical,
        qVariantFromValue( ta ),
        LineAttributesRole );
   emit propertiesChanged();
}

void LineDiagram::setLineAttributes(
    const QModelIndex & index,
    const LineAttributes & ta )
{
    d->attributesModel->setData(
        d->attributesModel->mapFromSource(index),
        qVariantFromValue( ta ),
        LineAttributesRole );
   emit propertiesChanged();
}

LineAttributes LineDiagram::lineAttributes() const
{
    return qVariantValue<LineAttributes>(
        d->attributesModel->data( KDChart::LineAttributesRole ) );
}

LineAttributes LineDiagram::lineAttributes( int column ) const
{
    return qVariantValue<LineAttributes>(
        d->attributesModel->data(
            d->attributesModel->mapFromSource( columnToIndex( column ) ),
            KDChart::LineAttributesRole ) );
}

LineAttributes LineDiagram::lineAttributes(
    const QModelIndex & index ) const
{
    return qVariantValue<LineAttributes>(
        d->attributesModel->data(
            d->attributesModel->mapFromSource(index),
            KDChart::LineAttributesRole ) );
}

void LineDiagram::setThreeDLineAttributes(
    const ThreeDLineAttributes & ta )
{
    setDataBoundariesDirty();
    d->attributesModel->setModelData(
        qVariantFromValue( ta ),
        ThreeDLineAttributesRole );
   emit propertiesChanged();
}

void LineDiagram::setThreeDLineAttributes(
    int column,
    const ThreeDLineAttributes & ta )
{
    setDataBoundariesDirty();
    d->attributesModel->setHeaderData(
        column,
        Qt::Vertical,
        qVariantFromValue( ta ),
        ThreeDLineAttributesRole );
   emit propertiesChanged();
}

void LineDiagram::setThreeDLineAttributes(
    const QModelIndex & index,
    const ThreeDLineAttributes & ta )
{
    setDataBoundariesDirty();
    d->attributesModel->setData(
        d->attributesModel->mapFromSource(index),
        qVariantFromValue( ta ),
        ThreeDLineAttributesRole );
   emit propertiesChanged();
}

ThreeDLineAttributes LineDiagram::threeDLineAttributes() const
{
    return qVariantValue<ThreeDLineAttributes>(
        d->attributesModel->data( KDChart::ThreeDLineAttributesRole ) );
}

ThreeDLineAttributes LineDiagram::threeDLineAttributes( int column ) const
{
    return qVariantValue<ThreeDLineAttributes>(
        d->attributesModel->data(
            d->attributesModel->mapFromSource( columnToIndex( column ) ),
            KDChart::ThreeDLineAttributesRole ) );
}

ThreeDLineAttributes LineDiagram::threeDLineAttributes(
    const QModelIndex & index ) const
{
    return qVariantValue<ThreeDLineAttributes>(
        d->attributesModel->data(
            d->attributesModel->mapFromSource( index ),
            KDChart::ThreeDLineAttributesRole ) );
}

double LineDiagram::threeDItemDepth( const QModelIndex & index ) const
{
    return threeDLineAttributes( index ).validDepth();
}

double LineDiagram::threeDItemDepth( int column ) const
{
    return qVariantValue<ThreeDLineAttributes>(
        d->attributesModel->headerData (
            column,
            Qt::Vertical,
            KDChart::ThreeDLineAttributesRole ) ).validDepth();
}

void LineDiagram::resizeEvent ( QResizeEvent* )
{
}

const QPair<QPointF, QPointF> LineDiagram::calculateDataBoundaries() const
{
    if ( !checkInvariants( true ) ) return QPair<QPointF, QPointF>( QPointF( 0, 0 ), QPointF( 0, 0 ) );

    const int rowCount = d->attributesModel->rowCount(attributesModelRootIndex());
    const int colCount = d->attributesModel->columnCount(attributesModelRootIndex());
    double xMin = 0;
    double xMax = rowCount -1;
    double yMin = 0, yMax = 0;
    bool bStarting = true;
    bool bOK;

    // calculate boundaries for different line types Normal - Stacked - Percent - Default Normal
    switch ( type() ){
    case LineDiagram::Normal:
    {
        for( int i = datasetDimension()-1; i < colCount; i += datasetDimension() ) {
            for ( int j=0; j< rowCount; ++j ) {
                const double value = valueForCellTesting( j, i, bOK );
                double xvalue;
                if( datasetDimension() > 1 && bOK )
                    xvalue = valueForCellTesting( j, i-1, bOK );
                if( bOK ){
                    if( bStarting ){
                        yMin = value;
                        yMax = value;
                    }else{
                        yMin = qMin( yMin, value );
                        yMax = qMax( yMax, value );
                    }
                    if ( datasetDimension() > 1 ) {
                        if( bStarting ){
                            xMin = xvalue;
                            xMax = xvalue;
                        }else{
                            xMin = qMin( xMin, xvalue );
                            xMax = qMax( xMax, xvalue );
                        }
                    }
                    bStarting = false;
                }
            }
        }

        if( yMin > 0 && yMax / yMin >= 2.0 )
            yMin = 0;
        else if( yMax < 0 && yMax / yMin <= 0.5 )
            yMax = 0;
    }
    break;
    case LineDiagram::Stacked:
    {
        double tmpValue = 0;

        for ( int j=0; j< rowCount; ++j ) {
            // calculate sum of values per column - Find out stacked Min/Max
            double stackedValues = 0;
            for( int i = datasetDimension()-1; i < colCount; i += datasetDimension() ) {
                const double value = valueForCellTesting( j, i, bOK );
                if( bOK )
                    stackedValues += value;
            }
            if( bStarting ){
                yMin = stackedValues;
                yMax = stackedValues;
            }else{
                // Pending Michel:
                // I am taking in account all values negatives or positives
                // take in account all stacked values
                yMin = qMin( qMin( yMin,  tmpValue ), stackedValues );
                yMax = qMax( yMax, stackedValues );
            }
            bStarting = false;
        }
    }
    break;
    case LineDiagram::Percent:
    {
        for( int i = datasetDimension()-1; i < colCount; i += datasetDimension() ) {
            for ( int j=0; j< rowCount; ++j ) {
                // Ordinate should begin at 0 the max value being the 100% pos
                const double value = valueForCellTesting( j, i, bOK );
                if( bOK ){
                    if( bStarting )
                        yMax = value;
                    else
                        yMax = qMax( yMax, value );
                    bStarting = false;
                }
            }
        }
    }
    break;
    default:
        Q_ASSERT_X ( false, "calculateDataBoundaries()",
                     "Type item does not match a defined line chart Type." );
    }

    QPointF bottomLeft ( QPointF( xMin, yMin ) );
    QPointF topRight ( QPointF( xMax, yMax ) );

    //FIXME(khz): Verify, if this code is right: We are taking
    //            ThreeDBarAttributes that might have been set
    //            at a header (using the setter that takes a column as parameter),
    //            but we are ignoring 'any' ThreeDBarAttributes, that might have
    //            been specified for an individual cell.
    // see: BarDiagram::calculateDataBoundaries ()
    //
    //Pending Michel: We don't need that any more with the new Grid Painting
    //I am commenting the code temporarely - will clean up after testing more
    //accurately.
    /*
      bool threeDBoundaries = false;
      for ( int i=0; i<colCount; ++i ) {
      QModelIndex index = model()->index( 0, i, rootIndex() );
      const ThreeDLineAttributes tda( threeDLineAttributes( index ) );
      if ( tda.isEnabled() ) {
      threeDBoundaries = true;
      QPointF projLeft ( project(QPointF( xMin, 0.0 ), QPointF( xMin, 0.0 ), tda.depth()/10 , index ) );
      QPointF projRight( project(QPointF( xMax, yMax ), QPointF( xMax, yMax), tda.depth()/10, index ) );
      projRight.x() > topRight.x()  ? topRightThreeD.setX( projRight.x() ):
      topRightThreeD.setX( topRight.x() );
      projRight.y() > topRight.y()  ? topRightThreeD.setY( projRight.y() ):
      topRightThreeD.setY( topRight.y() );
      projLeft.x() > bottomLeft.x() ? bottomLeftThreeD.setX( bottomLeft.x() ):
      bottomLeftThreeD.setX( projLeft.x() );
      projLeft.y() > bottomLeft.y() ? bottomLeftThreeD.setY( bottomLeft.y() ):
      bottomLeftThreeD.setY( projLeft.y() );
      }
      }

      return threeDBoundaries ? QPair<QPointF, QPointF> ( bottomLeftThreeD ,  topRightThreeD ):
      QPair<QPointF, QPointF> ( bottomLeft, topRight );
    */
    return QPair<QPointF, QPointF> ( bottomLeft, topRight );
}


void LineDiagram::paintEvent ( QPaintEvent*)
{
//qDebug() << "starting LineDiagram::paintEvent ( QPaintEvent*)";
    QPainter painter ( viewport() );
    PaintContext ctx;
    ctx.setPainter ( &painter );
    ctx.setRectangle ( QRectF ( 0, 0, width(), height() ) );
    paint ( &ctx );
//qDebug() << "         LineDiagram::paintEvent ( QPaintEvent*) ended.";
}


double LineDiagram::valueForCellTesting( int row, int column, bool& bOK ) const
{
    double value =
        d->attributesModel->data(
            d->attributesModel->index( row, column, attributesModelRootIndex() )
        ).toDouble( &bOK );
    if( ! bOK )
        value = 0.0;

    return value;
}


LineAttributes::MissingValuesPolicy LineDiagram::getCellValues(
      int row, int column,
      double& valueX, double& valueY ) const
{
    LineAttributes::MissingValuesPolicy policy;

    bool bOK = true;
    valueX = ( datasetDimension() > 1 && column > 0 )
           ? valueForCellTesting(row, column-1, bOK)
           : row;
    if( bOK )
        valueY = valueForCellTesting(row, column, bOK);
    if( bOK ){
        policy = LineAttributes::MissingValuesPolicyIgnored;
    }else{
        // missing value: find out the policy
        QModelIndex index = model()->index( row, column, rootIndex() );
        LineAttributes la = lineAttributes( index );
        policy = la.missingValuesPolicy();
    }
    return policy;
}

/*commenting this include: used for testing
notice: Windows compilers need this include to
be written before the #define d(d_func()) above*/
//#include <QTime>

void LineDiagram::paint( PaintContext* ctx )
{
//qDebug() << "    start diag::paint()";
    if ( !checkInvariants( true ) ) return;
    if ( !AbstractGrid::isBoundariesValid(dataBoundaries()) ) return;

    //QTime t = QTime::currentTime();

    //calculates and stores the values
    const int rowCount = d->attributesModel->rowCount(attributesModelRootIndex());
    const int colCount = d->attributesModel->columnCount(attributesModelRootIndex());
    DataValueTextInfoList list;
    LineAttributesInfoList lineList;
    LineAttributes::MissingValuesPolicy policy;

    // paint different line types Normal - Stacked - Percent - Default Normal
    switch ( type() )
    {
        case LineDiagram::Normal:
        {
            for( int iColumn = datasetDimension()-1;
                     iColumn < colCount;  iColumn += datasetDimension() ) {
                QPolygonF area;
                bool bValuesFound = false;
                double lastValueX, lastValueY;
                double valueX, valueY;
                for ( int iRow = 0; iRow < rowCount; ++iRow ) {
                    bool skipThisCell = false;
                    // trying to find a fromPoint
                    policy = getCellValues( iRow, iColumn, valueX, valueY );
                    switch( policy ){
                        case LineAttributes::MissingValuesAreBridged:
                            if( bValuesFound ){
                                valueX = lastValueX;
                                valueY = lastValueY;
                            }else{
                                skipThisCell = true;
                            }
                            break;
                        case LineAttributes::MissingValuesHideSegments:
                            skipThisCell = true;
                            break;
                        case LineAttributes::MissingValuesShownAsZero:
                            // fall through intended
                        case LineAttributes::MissingValuesPolicyIgnored:
                            lastValueX = valueX;
                            lastValueY = valueY;
                            bValuesFound = true;
                            break;
                    }
                    if( ! skipThisCell ){
                        // trying to find a toPoint
                        double nextValueX, nextValueY;
                        bool foundToPoint = false;
                        int iNextRow = iRow+1;
                        while ( ! (foundToPoint || skipThisCell || iNextRow >= rowCount) ) {
                            policy = getCellValues( iNextRow, iColumn, nextValueX, nextValueY );
                            switch( policy ){
                                case LineAttributes::MissingValuesAreBridged:
                                    // The cell has no valid value, so we  make sure that
                                    // this cell will not be processed by the next iteration
                                    // of the iRow loop:
                                    ++iRow;
                                    break;
                                case LineAttributes::MissingValuesHideSegments:
                                    // The cell has no valid value, so we  make sure that
                                    // this cell will not be processed by the next iteration
                                    // of the iRow loop:
                                    skipThisCell = true;
                                    ++iRow;
                                    break;
                                case LineAttributes::MissingValuesShownAsZero:
                                    // fall through intended
                                case LineAttributes::MissingValuesPolicyIgnored:
                                    foundToPoint = true;
                                    break;
                            }
                            ++iNextRow;
                        }
                        if( ! skipThisCell ){
                            QModelIndex index = model()->index( iRow, iColumn, rootIndex() );
                            QPointF fromPoint = coordinatePlane()->translate( QPointF( valueX, valueY ) );
                            area.append( fromPoint );
                            if( foundToPoint ){
                                QPointF toPoint = coordinatePlane()->translate( QPointF( nextValueX, nextValueY ) );
                                lineList.append( LineAttributesInfo( index, fromPoint, toPoint ) );
                            }
                            list.append( DataValueTextInfo( index, fromPoint, valueY ) );
                        }
                    }
                }
                //area can be set by column
                QModelIndex index = model()->index( 0, iColumn, rootIndex() );
                LineAttributes laa = lineAttributes( index );
                if ( laa.displayArea() )
                    paintAreas( ctx, index, area, laa.transparency() );
            }
        }
            break;
        case LineDiagram::Stacked:
        {
            //FIXME(khz): add LineAttributes::MissingValuesPolicy support for LineDiagram::Stacked
            for( int i = datasetDimension()-1; i < colCount; i += datasetDimension() ) {
                QPolygonF area;
                for ( int j = 0; j< rowCount; ++j ) {
                    QModelIndex index = model()->index( j, i, rootIndex() );
                    double stackedValues = 0, nextValues = 0;
                    for ( int k = i; k >= datasetDimension()-1 ; k -= datasetDimension() ) {
                        stackedValues += valueForCell( j, k );
                        if ( j+1 < rowCount )
                            nextValues += valueForCell( j+1, k );
                    }
                    QPointF nextPoint = coordinatePlane()->translate( QPointF( j, stackedValues ) );
                    area.append( nextPoint );
                    QPointF toPoint = coordinatePlane()->translate( QPointF( j+1, nextValues ) );
                    if ( j+1 < rowCount )
                        lineList.append( LineAttributesInfo( index, nextPoint, toPoint ) );
                    list.append( DataValueTextInfo( index, nextPoint, valueForCell( j, i ) ) );
                }
                QModelIndex index = model()->index( 0, i, rootIndex() );
                LineAttributes laa = lineAttributes( index );
                if ( laa.displayArea() )
                    paintAreas( ctx, index, area, laa.transparency() );
            }
        }
            break;
        case LineDiagram::Percent:
        {
            //FIXME(khz): add LineAttributes::MissingValuesPolicy support for LineDiagram::Percent
            double maxValue = 100; // always 100%
            double sumValues = 0;
            QVector <double > sumValuesVector;

            //calculate sum of values for each column and store
            for ( int j=0; j<rowCount ; ++j ) {
                for( int i = datasetDimension()-1; i < colCount; i += datasetDimension() ) {
                    double tmpValue = valueForCell( j, i );
                    if ( tmpValue > 0 )
                        sumValues += tmpValue;
                    if ( i == colCount-1 ) {
                        sumValuesVector <<  sumValues ;
                        sumValues = 0;
                    }
                }
            }
            // calculate stacked percent value
            for( int i = datasetDimension()-1; i < colCount; i += datasetDimension() ) {
                QPolygonF area;
                for ( int j=0; j<rowCount ; ++j ) {
                    double stackedValues = 0, nextValues = 0;
		    QModelIndex index = model()->index( j, i, rootIndex() );
                    //calculate stacked percent value- we only take in account positives values for now.
                    for ( int k = i; k >= 0 ; --k ) {
                        double val = valueForCell( j, k );
                        if ( val > 0)
                            stackedValues += val;
                        if ( j+1 < rowCount ){
                            val = valueForCell( j+1, k);
                            if ( val > 0 )
                                nextValues += val;
                        }
                    }
                    double y = 0;
                    if ( sumValuesVector.at(j) != 0  )
                        y = stackedValues/sumValuesVector.at(j) * maxValue;
                    QPointF nextPoint = coordinatePlane()->translate( QPointF( j, y ) );
                    area.append( nextPoint );
                    if ( j+1 < rowCount ) {
                        double y = 0;
                        if ( sumValuesVector.at(j+1) != 0  )
                            y = nextValues/sumValuesVector.at(j+1) * maxValue;
                        QPointF toPoint = coordinatePlane()->translate( QPointF( j+1, y ) );
                        lineList.append( LineAttributesInfo( index, nextPoint, toPoint ) );
                    }
                    list.append( DataValueTextInfo( index, nextPoint, valueForCell( j, i ) ) );
                }
                QModelIndex index = model()->index( 0, i, rootIndex() );
                LineAttributes laa = lineAttributes( index );
                if ( laa.displayArea() )
                    paintAreas( ctx, index, area, laa.transparency() );
            }
            break;
        }
        default:
            Q_ASSERT_X ( false, "paint()",
                         "Type item does not match a defined line chart Type." );
    }
    // paint all lines and their attributes
    {
        PainterSaver painterSaver( ctx->painter() );
        if ( antiAliasing() )
            ctx->painter()->setRenderHint ( QPainter::Antialiasing );
        LineAttributesInfoListIterator itline ( lineList );
        //qDebug() << "Rendering 1 in: " << t.msecsTo( QTime::currentTime() ) << endl;
        while ( itline.hasNext() ) {
            const LineAttributesInfo& lineInfo = itline.next();
            paintLines( ctx,lineInfo.index, lineInfo.value, lineInfo.nextValue );
        }
    }
    // paint all data value texts and the point markers
    {
        PainterSaver painterSaver( ctx->painter() );
        DataValueTextInfoListIterator it( list );
        while ( it.hasNext() ) {
            const DataValueTextInfo& info = it.next();
            paintDataValueText( ctx->painter(), info.index, info.pos, info.value );
            paintMarker( ctx->painter(), info.index, info.pos );
        }
    }
    //qDebug() << "Rendering 2 in: " << t.msecsTo( QTime::currentTime() ) << endl;
}

void LineDiagram::paintLines( PaintContext* ctx, const QModelIndex& index,
                              const QPointF& from, const QPointF& to )
{
    ThreeDLineAttributes td = threeDLineAttributes(index);
    if ( td.isEnabled() ) {
        const double lineDepth = td.depth();
        paintThreeDLines( ctx, index, from, to, lineDepth );
    } else {
        ctx->painter()->setBrush( brush( index ) );
        ctx->painter()->setPen( pen( index ) );
        if ( index.row() + 1 < d->attributesModel->rowCount(attributesModelRootIndex()) ) {
            if ( ctx->rectangle().contains( from ) || ctx->rectangle().contains( to ) ) {
                ctx->painter()->drawLine( from, to );
            }
        }
    }
}

void LineDiagram::paintAreas( PaintContext* ctx, const QModelIndex& index, const QPolygonF& area, const uint transparency )
{
    QPolygonF pol ( area );
    const QPair<QPointF, QPointF> boundaries = dataBoundaries ();
    QPointF bottomLeft = boundaries.first;
    QPointF topRight = boundaries.second;
    QColor trans( brush(index).color() );
    QPen indexPen( pen(index) );
    trans.setAlpha( transparency );
    indexPen.setColor( trans );
    pol.insert( 0,  coordinatePlane()->translate( QPointF( bottomLeft.x(), 0 ) ) );
    pol.append( coordinatePlane()->translate( QPointF( topRight.x(),0 ) ) );
    PainterSaver painterSaver( ctx->painter() );
    if ( antiAliasing() )
        ctx->painter()->setRenderHint ( QPainter::Antialiasing );
    ctx->painter()->setPen( indexPen );
    ctx->painter()->setBrush( trans ) ;
    ctx->painter()->drawPolygon( pol );
}

/*!
  Projects a point in a space defined by its x, y, and z coordinates
  into a point onto a plane, given two rotation angles around the x
  resp. y axis.
*/

const QPointF LineDiagram::project( QPointF point, QPointF maxLimits, double z, const QModelIndex& index ) const
{
  ThreeDLineAttributes td = threeDLineAttributes( index );

    //Pending Michel FIXME - the rotation does not work as expected atm
    double xrad = DEGTORAD( td.lineXRotation() );
    double yrad = DEGTORAD( td.lineYRotation() );
    QPointF ret = QPointF(point.x()*cos( yrad ) + z * sin( yrad ) ,  point.y()*cos( xrad ) - z * sin( xrad ) );
    return ret;
}

void LineDiagram::paintThreeDLines(PaintContext* ctx, const QModelIndex& index, const QPointF& from, const QPointF& to, const double depth  )
{
    // retrieve the boundaries
    const QPair<QPointF, QPointF> boundaries = dataBoundaries ();
    QPointF maxLimits = boundaries.second;
    QVector <QPointF > segmentPoints;
    QPointF topLeft = project( from, maxLimits, depth, index  );
    QPointF topRight = project ( to, maxLimits, depth, index  );

    segmentPoints << from << topLeft << topRight << to;
    QPolygonF segment ( segmentPoints );
    QBrush indexBrush ( brush( index ) );
    PainterSaver painterSaver( ctx->painter() );
    if ( antiAliasing() )
        ctx->painter()->setRenderHint ( QPainter::Antialiasing );
    ctx->painter()->setBrush( indexBrush );
    ctx->painter()->setPen( pen( index ) ) ;
    ctx->painter()->drawPolygon( segment );
}

void LineDiagram::resize ( const QSizeF& )
{
}

const int LineDiagram::numberOfAbscissaSegments () const
{
    return d->attributesModel->rowCount(attributesModelRootIndex());
}

const int LineDiagram::numberOfOrdinateSegments () const
{
    return d->attributesModel->columnCount(attributesModelRootIndex());
}

//#endif


