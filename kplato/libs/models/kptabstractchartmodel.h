/* This file is part of the KOffice project
 * Copyright (c) 2008 Dag Andersen <kplato@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KPTABSTRACTCHARTMODEL_H
#define KPTABSTRACTCHARTMODEL_H

#include "kplatomodels_export.h"

#include <QObject>
#include <QPair>
#include <QVariant>
#include <QDate>

#include <kdebug.h>

namespace KPlato
{
    
class ChartDataIndex;
class ChartAxisIndex;
    
/**
The ChartAxisIndex class describes an axis or an axis set.
An axis set can consist of an abitrary number of axis.
 */
class KPLATOMODELS_EXPORT ChartAxisIndex
{
public:
    /// Create an invalid axis index
    ChartAxisIndex();
    /// Create an index to an axis with @p number and @p userdata
    ChartAxisIndex( int number, int userdata );
    /// Destructor
    ~ChartAxisIndex();
    /// Return true if the data set is valid (m_number >= 0)
    bool isValid() const;
    /// Return the axis index number
    int number() const;
    /// Holds the user defined data
    int userData;

protected:
    int m_number;
};

/**
The ChartDataIndex class describes data sets to be ploted.
A data set can consist of several sections. Sections can have different
properties, like color.

 */
class KPLATOMODELS_EXPORT ChartDataIndex
{
public:
    /// Create an invalid data index
    ChartDataIndex();
    /// Create an index with index @p number and @p userdata
    ChartDataIndex( int number, int userdata = 0 );
    /// Destructor
    ~ChartDataIndex();
    
    /// Returns true if the index is valid. ( m_number >= 0)
    bool isValid() const;
    /// Returns the index number.
    int number() const;
    /// Holds the user defined data
    int userData;

protected:
    int m_number;
    friend class AbstractChartModel;
    ChartAxisIndex m_axisIndex;
};


/**
The AbstractChartModel class provides the abstract interface for chart model classes.

When subclassing AbstractChartModel, at the very least you must implement index(), childCount(), data(), axisCount(), axisData() and axisIndex().

ChartAxisIndex 
    !- ChartDataIndex
    !- ChartDataIndex
    !
ChartAxisIndex
    !
*/

class KPLATOMODELS_EXPORT AbstractChartModel : public QObject
{
    Q_OBJECT
public:
    /// Create a model with @p parent
    AbstractChartModel( QObject *parent = 0 );
    /// Destructor
    virtual ~AbstractChartModel();
    
    /// Data axcess roles. Extends Qt::ItemDataRole
    enum DataRole { 
        AxisPositionRole = Qt::UserRole+1,
        AxisMinRole,
        AxisMaxRole,
        AxisTypeRole,
        AxisDataTypeRole
    };
    
    /// Defines the axis' position
    enum AxisType { Axis_None = 0, Axis_X, Axis_Y };
    
    /// Return the number of data sets that shall be plotted against the axis set @p index
    virtual int dataSetCount( const ChartAxisIndex &index ) const = 0;
    /// Return data for @p role (e.g. a list of values to plot) for the data set @p index
    virtual QVariant data( const ChartDataIndex &index, int role = Qt::DisplayRole ) const = 0;
    /// Return data for @p role (e.g. a list of values to plot) for the data set @p index
    virtual QVariant data( const ChartDataIndex &index, const ChartAxisIndex &axis, int role = Qt::DisplayRole ) const = 0;
    /// Create an index for the @p dataset in the axis set @p index
    virtual ChartDataIndex index( int dataset, const ChartAxisIndex &index ) const = 0;
    /// Create an index for the child @p number in the data set @p parent.
    /// Implement if your data sets have children.
    virtual ChartDataIndex index( int dataset, const ChartDataIndex &parent ) const;
    /// Return the number of child data sets the @p parent has
    virtual int childCount( const ChartDataIndex &parent ) const;
    /// Return true if @p parent has child data sets
    virtual bool hasChildren( const ChartDataIndex &parent ) const;
    
    /// Return the number if axis with @p parent in this model
    virtual int axisCount( const ChartAxisIndex &parent = ChartAxisIndex() ) const = 0;
    /// Return data for @p role for the axis set @p index
    virtual QVariant axisData( const ChartAxisIndex &index, int role = Qt::DisplayRole ) const = 0;
    /// Create an index for @p axisset with @p parent
    virtual ChartAxisIndex axisIndex( int axisset, const ChartAxisIndex &parent = ChartAxisIndex() ) const = 0;
    /// Return true if @p parent has child axis sets
    virtual bool hasAxisChildren( const ChartAxisIndex &parent = ChartAxisIndex() ) const;
    
signals:
    /// Emitted when the model is reset
    void reset();
    /// Emitted when data in data set @index has changed
    void dataSetChanged( const ChartDataIndex &index );
    /// Emitted when data in axis set @index has changed
    void axisSetChanged( const ChartAxisIndex &index );

protected:
    ChartDataIndex createDataIndex( int number, const ChartAxisIndex &axisIndex, int userdata = 0 ) const;
    ChartDataIndex createDataIndex( int number, const ChartDataIndex &parent, int userdata = 0 ) const;

    ChartAxisIndex createAxisIndex( int number, const ChartAxisIndex &parent, int userdata = 0 ) const;
};


} //namespace KPlato

#endif
