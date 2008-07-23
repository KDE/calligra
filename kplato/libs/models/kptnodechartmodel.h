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

#ifndef KPTNODECHARTMODEL_H
#define KPTNODECHARTMODEL_H

#include "kplatomodels_export.h"

#include "kptabstractchartmodel.h"

#include "kpteffortcostmap.h"

namespace KPlato
{
    
class Project;
class ScheduleManager;
class Node;

/**
The NodeChartModel class provides the data interface to chart data.

*/

class KPLATOMODELS_EXPORT NodeChartModel : public AbstractChartModel
{
    Q_OBJECT
public:
    NodeChartModel( QObject *parent = 0 );
    virtual ~NodeChartModel() {}
    
    enum UserId { None = 0, AxisSet = None, BCWS, BCWP, ACWP, X_AxisSet, Y_AxisSet, X_Axis, Y_Axis };
    
    void setProject( Project *project );
    
    /// Return the number of data sets that shall be plotted against the axis set @p index
    virtual int dataSetCount( const ChartAxisIndex &index ) const;
    virtual int childCount( const ChartDataIndex &index ) const;
    /// Return data for @p role (e.g. a list of values to plot) for the data set @p index
    virtual QVariant data( const ChartDataIndex &index, int role = Qt::DisplayRole ) const;
    /// Create an index for the @p dataset in the axis set @p index
    virtual QVariant data( const ChartDataIndex &index, const ChartAxisIndex &axis, int role = Qt::DisplayRole ) const;
    virtual ChartDataIndex index( int dataset, const ChartAxisIndex &index ) const;
    /// Create an index for the @p number in the axis set @p index
    ChartDataIndex index( int number, const ChartDataIndex &idx ) const;
    
    /// Return the parent ChartAxisIndex of @p index
    virtual ChartAxisIndex parent( const ChartAxisIndex &index ) const;
    /// Return the number if axis sets with @p parent in this model
    virtual int axisCount( const ChartAxisIndex &parent = ChartAxisIndex() ) const;
    /// Return data for @p role for the axis set @p index
    virtual QVariant axisData( const ChartAxisIndex &index, int role = Qt::DisplayRole ) const;
    /// Create an index for @p axisset with @p parent
    virtual ChartAxisIndex axisIndex( int axisset, const ChartAxisIndex &parent = ChartAxisIndex() ) const;

    const EffortCostMap &bcwp() const { return m_bcwp; }
    const EffortCostMap &acwp() const { return m_acwp; }
    
    void setNodes( const QList<Node*> &nodes );
    void addNode( Node *node );
    void clearNodes();
    
    QDate startDate() const;
    QDate endDate() const;
    
    struct DataShown {
        bool showCost;
        bool showBCWSCost;
        bool showBCWPCost;
        bool showACWPCost;
        
        bool showEffort;
        bool showBCWSEffort;
        bool showBCWPEffort;
        bool showACWPEffort;
    };

    void setDataShown( const DataShown &show );
    DataShown dataShown() const { return m_dataShown; }
            
public slots:
    void setScheduleManager( ScheduleManager *sm );
    void slotNodeChanged( Node *node );
    
protected:
    void calculate();
    
private:
    Project *m_project;
    ScheduleManager *m_manager;
    QList<Node*> m_nodes;
    
    EffortCostMap m_bcwp;
    EffortCostMap m_acwp;
    
    DataShown m_dataShown;

};


} //namespace KPlato

#endif
