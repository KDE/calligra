/* This file is part of the KDE project

   Copyright 2008 Johannes Simon <johannes.simon@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KCHART_KDCHARTMODEL_H
#define KCHART_KDCHARTMODEL_H


// Qt
#include <QAbstractItemModel>

// KChart
#include "ChartShape.h"


namespace KChart {

class CHARTSHAPELIB_EXPORT KDChartModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    KDChartModel( QObject *parent = 0 );
    ~KDChartModel();
    
    void setDataDirection( Qt::Orientation direction );
    Qt::Orientation dataDirection() const;

public slots:
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    void dataChanged( const QModelIndex& topLeft, const QModelIndex& bottomRight );
    void dataSetSizeChanged( DataSet *dataSet, int newSize );
    void slotColumnsInserted( const QModelIndex& parent, int start, int end );
    
    QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
    QModelIndex parent( const QModelIndex &index ) const;

    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    
    void setDataDimensions( int dataDimensions );
    int dataDimensions() const;

    void addDataSet( DataSet *dataSet, bool silent = false );
    void removeDataSet( DataSet *dataSet, bool silent = false );
    QList<DataSet*> dataSets() const;
    
    void emitReset();

private:
    class Private;
    Private *const d;
};

} // namespace KChart

#endif // KCHART_KDCHARTMODEL_H
