/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2006 Klaraelvdalens Datakonsult AB.  All rights reserved.
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
 **   information about KD Chart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#include <QDebug>
#include <QPen>
#include <QPointer>

#include "KDChartAttributesModel.h"
#include "KDChartPalette.h"
#include "KDChartDataValueAttributes.h"
#include "KDChartGlobal.h"

#include <KDABLibFakes>


using namespace KDChart;

AttributesModel::AttributesModel( QAbstractItemModel* model, QObject * parent/* = 0 */ )
  : AbstractProxyModel( parent ),
    mPaletteType( PaletteTypeDefault )
{
  setSourceModel(model);
}

AttributesModel::~AttributesModel()
{
}

void AttributesModel::initFrom( const AttributesModel* other )
{
    if( other == this || ! other ) return;

    mDataMap = other->mDataMap;
    mHorizontalHeaderDataMap = other->mHorizontalHeaderDataMap;
    mVerticalHeaderDataMap = other->mVerticalHeaderDataMap;
    mModelDataMap = other->mModelDataMap;

    setPaletteType( other->paletteType() );
}

QVariant AttributesModel::headerData ( int section,
                                       Qt::Orientation orientation,
                                       int role/* = Qt::DisplayRole */ ) const
{
  QVariant sourceData = sourceModel()->headerData( section, orientation, role );
  if ( sourceData.isValid() ) return sourceData;
  // the source model didn't have data set, let's use our stored values
  const QMap<int, QMap<int, QVariant> >& map = orientation == Qt::Horizontal ? mHorizontalHeaderDataMap : mVerticalHeaderDataMap;
  if ( map.contains( section ) ) {
      const QMap<int, QVariant> &dataMap = map[ section ];
      if ( dataMap.contains( role ) ) {
          return dataMap[ role ];
      }
  }

  // Default values if nothing else matches
  switch ( role ) {
  case Qt::DisplayRole:
      return QLatin1String( orientation == Qt::Vertical ?  "Series " : "Item " ) + QString::number( section ) ;

  case KDChart::DatasetBrushRole: {
      if ( paletteType() == PaletteTypeSubdued )
          return Palette::subduedPalette().getBrush( section );
      else if ( paletteType() == PaletteTypeRainbow )
          return Palette::rainbowPalette().getBrush( section );
      else if ( paletteType() == PaletteTypeDefault )
          return Palette::defaultPalette().getBrush( section );
      else
          qWarning("Unknown type of fallback palette!");
  }
  case KDChart::DatasetPenRole: {
      // default to the color set for the brush (or it's defaults)
      // but only if no per model override was set
      if ( !modelData( role ).isValid() ) {
          QBrush brush = qVariantValue<QBrush>( headerData( section, orientation, DatasetBrushRole ) );
          return QPen( brush.color() );
      }
  }
  default:
      break;
  }

  return QVariant();
}


// Note: Our users NEED this method - even if
//       we do not need it at drawing time!
//       (khz, 2006-07-28)
QVariant AttributesModel::data( int role ) const
{
  if ( isKnownAttributesRole( role ) ) {
      // check if there is something set at global level
      QVariant v = modelData( role );

      // else return the default setting, if any
      if ( !v.isValid() )
          v = defaultsForRole( role );
      return v;
  }
  return QVariant();
}


// Note: Our users NEED this method - even if
//       we do not need it at drawing time!
//       (khz, 2006-07-28)
QVariant AttributesModel::data( int column, int role ) const
{
  if ( isKnownAttributesRole( role ) ) {
      // check if there is something set for the column (dataset)
      QVariant v;
      v = headerData( column, Qt::Vertical, role );

      // check if there is something set at global level
      if ( !v.isValid() )
          v = data( role ); // includes automatic fallback to default
      return v;
  }
  return QVariant();
}


QVariant AttributesModel::data( const QModelIndex& index, int role ) const
{
  //qDebug() << "AttributesModel::data(" << index << role << ")";
  if( index.isValid() ) {
    Q_ASSERT( index.model() == this );
  }
  QVariant sourceData = sourceModel()->data( mapToSource(index), role );
  if ( sourceData.isValid() )
      return sourceData;

  // check if we are storing a value for this role at this cell index
  if ( mDataMap.contains( index.column() ) ) {
      const QMap< int,  QMap< int, QVariant> > &colDataMap = mDataMap[ index.column() ];
      if ( colDataMap.contains( index.row() ) ) {
          const QMap<int, QVariant> &dataMap = colDataMap[ index.row() ];
          if ( dataMap.contains( role ) )
              return dataMap[ role ];
      }
  }
  // check if there is something set for the column (dataset), or at global level
  if( index.isValid() )
      return data( index.column(), role ); // includes automatic fallback to default

  return QVariant();
}


bool AttributesModel::isKnownAttributesRole( int role ) const
{
    bool oneOfOurs = false;
    switch( role ) {
      // fallthrough intended
      case DataValueLabelAttributesRole:
      case DatasetBrushRole:
      case DatasetPenRole:
      case ThreeDAttributesRole:
      case LineAttributesRole:
      case ThreeDLineAttributesRole:
      case BarAttributesRole:
      case ThreeDBarAttributesRole:
      case PieAttributesRole:
      case ThreeDPieAttributesRole:
          oneOfOurs = true;
        default:
        break;
    }
    return oneOfOurs;
}

QVariant AttributesModel::defaultsForRole( int role ) const
{
    switch ( role ) {
        case KDChart::DataValueLabelAttributesRole:
            return DataValueAttributes::defaultAttributesAsVariant();
            // for the below there isn't a per-value default, since there's a per-column one
        case KDChart::DatasetBrushRole:
        case KDChart::DatasetPenRole:
        default:
            break;
    }
    return QVariant();
}

bool AttributesModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
    if ( !isKnownAttributesRole( role ) ) {
        return sourceModel()->setData( mapToSource(index), value, role );
    } else {
        QMap< int,  QMap< int, QVariant> > &colDataMap = mDataMap[ index.column() ];
        QMap<int, QVariant> &dataMap = colDataMap[ index.row() ];
        //qDebug() <<  "AttributesModel::setData" <<"role" << role << "value" << value;
        dataMap.insert( role, value );
        emit attributesChanged( index, index );
        return true;
    }
}

bool AttributesModel::setHeaderData ( int section, Qt::Orientation orientation,
                                      const QVariant & value, int role )
{
    if ( !isKnownAttributesRole( role ) ) {
        return sourceModel()->setHeaderData( section, orientation, value, role );
    } else {
        QMap<int,  QMap<int, QVariant> > &sectionDataMap
            = orientation == Qt::Horizontal ? mHorizontalHeaderDataMap : mVerticalHeaderDataMap;
        QMap<int, QVariant> &dataMap = sectionDataMap[ section ];
        dataMap.insert( role, value );
        emit attributesChanged( index( 0, section, QModelIndex() ),
                                index( rowCount( QModelIndex() ), section, QModelIndex() ) );
        return true;
    }
}

void AttributesModel::setPaletteType( AttributesModel::PaletteType type )
{
    mPaletteType = type;
}

AttributesModel::PaletteType AttributesModel::paletteType() const
{
    return mPaletteType;
}

bool KDChart::AttributesModel::setModelData( const QVariant value, int role )
{
    mModelDataMap.insert( role, value );
    emit attributesChanged( index( 0, 0, QModelIndex() ),
                            index( rowCount( QModelIndex() ),
                                    columnCount( QModelIndex() ), QModelIndex() ) );
    return true;
}

QVariant KDChart::AttributesModel::modelData( int role ) const
{
    return mModelDataMap.value( role, QVariant() );
}

int AttributesModel::rowCount( const QModelIndex& index ) const
{
    Q_ASSERT(sourceModel());
    return sourceModel()->rowCount( mapToSource(index) );
}

int AttributesModel::columnCount( const QModelIndex& index ) const
{
    Q_ASSERT(sourceModel());
    return sourceModel()->columnCount( mapToSource(index) );
}

void AttributesModel::setSourceModel( QAbstractItemModel* sourceModel )
{
    if( this->sourceModel() != 0 )
        disconnect( this->sourceModel(), SIGNAL( dataChanged( const QModelIndex&, const QModelIndex&)),
                                  this, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex&)));
    QAbstractProxyModel::setSourceModel( sourceModel );
    if( this->sourceModel() != NULL )
        connect( this->sourceModel(), SIGNAL( dataChanged( const QModelIndex&, const QModelIndex&)),
                                this, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex&)));
}
