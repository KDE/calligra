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

#ifndef KDCHARTABSTRACTDIAGRAM_P_H
#define KDCHARTABSTRACTDIAGRAM_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the KD Chart API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "KDChartAbstractCoordinatePlane.h"

#include <QPoint>
#include <QPointer>

#include <KDABLibFakes>


namespace KDChart {
  class AttributesModel;

/**
 * \internal
 */
class KDChart::AbstractDiagram::Private
{
    friend class AbstractDiagram;
public:
    explicit Private();
    virtual ~Private();

    Private( const Private& rhs );

    void setAttributesModel( AttributesModel* );

protected:
    void init();
    void init( AbstractCoordinatePlane* plane );
    QPointer<AbstractCoordinatePlane> plane;
    mutable QModelIndex attributesModelRootIndex;
    QPointer<AttributesModel> attributesModel;
    bool allowOverlappingDataValueTexts;
    bool antiAliasing;
    bool percent;
    int datasetDimension;
    mutable QPair<QPointF,QPointF> databoundaries;
    mutable bool databoundariesDirty;
};

inline AbstractDiagram::AbstractDiagram( Private * p ) : _d( p )
{
    init();
}
inline AbstractDiagram::AbstractDiagram(
    Private * p, QWidget* parent, AbstractCoordinatePlane* plane )
  : QAbstractItemView( parent ), _d( p )
{
    _d->init( plane );
}

class DataValueTextInfo {
public:
  DataValueTextInfo(){}
  DataValueTextInfo( const QModelIndex& _index, const QPointF& _pos, double _value )
    :index( _index ), pos( _pos ), value( _value )
    {}
  DataValueTextInfo( const DataValueTextInfo& other )
    :index( other.index ), pos( other.pos ), value( other.value ) {}
  QModelIndex index;
  QPointF pos;
  double value;
};

typedef QVector<DataValueTextInfo> DataValueTextInfoList;
typedef QVectorIterator<DataValueTextInfo> DataValueTextInfoListIterator;

class LineAttributesInfo {
public :
  LineAttributesInfo() {}
  LineAttributesInfo( const QModelIndex _index, const QPointF& _value, const QPointF& _nextValue )
    :index( _index ), value ( _value ), nextValue ( _nextValue )  {}

  QModelIndex index;
  QPointF value;
  QPointF nextValue;
};

typedef QVector<LineAttributesInfo> LineAttributesInfoList;
typedef QVectorIterator<LineAttributesInfo> LineAttributesInfoListIterator;

class DataValueMarkerInfo {
public:
  DataValueMarkerInfo(){}
  DataValueMarkerInfo( const QModelIndex& _index, const QPointF& _pos, double _value )
    :index( _index ), pos( _pos ), value( _value )
    {}
  DataValueMarkerInfo( const DataValueMarkerInfo& other )
    :index( other.index ), pos( other.pos ), value( other.value ) {}
  QModelIndex index;
  QPointF pos;
  double value;
};

typedef QVector<DataValueMarkerInfo> DataValueMarkerInfoList;
typedef QVectorIterator<DataValueMarkerInfo> DataValueMarkerInfoListIterator;

}
#endif /* KDCHARTDIAGRAM_P_H */
