/* This file is part of the Calligra project
 * Copyright (c) 2008 Dag Andersen <danders@get2net.dk>
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

#include "kptitemmodelbase.h"

#include "kpteffortcostmap.h"

#include <QSortFilterProxyModel>

#include "kptdebug.h"

#include "KDChartGlobal"

namespace KPlato
{

class Resource;
class Project;
class ScheduleManager;
class Node;

class KPLATOMODELS_EXPORT ChartProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ChartProxyModel(QObject *parent = 0) : QSortFilterProxyModel( parent ) {}

    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const {
        //if ( role == Qt::DisplayRole && orientation == Qt::Vertical ) kDebug()<<"fetch:"<<orientation<<section<<mapToSource( index(0, section) ).column()<<m_rejects;
        return QSortFilterProxyModel::headerData( section, orientation, role );
    }

    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const {
        if ( role == Qt::DisplayRole && ! m_zerocolumns.isEmpty() ) {
            int column = mapToSource( index ).column();
            if ( m_zerocolumns.contains( column ) ) {
                //kDebug()<<"zero:"<<index.column()<<mapToSource( index ).column();
                return QVariant();
            }
        }
        //if ( role == Qt::DisplayRole ) kDebug()<<"fetch:"<<index.column()<<mapToSource( index ).column()<<m_rejects;
        QVariant v = QSortFilterProxyModel::data( index, role );
        //if ( role == Qt::DisplayRole ) kDebug(planDbg())<<index.row()<<","<<index.column()<<"("<<columnCount()<<")"<<v;
        return v;
    }
    void setRejectColumns( const QList<int> &columns ) { m_rejects = columns; invalidateFilter(); }
    QList<int> rejectColumns() const { return m_rejects; }
    void setZeroColumns( const QList<int> &columns ) { m_zerocolumns = columns; }
    QList<int> zeroColumns() const { return m_zerocolumns; }

    void reset() { QSortFilterProxyModel::reset(); }

protected:
    bool filterAcceptsColumn ( int source_column, const QModelIndex &/*source_parent */) const {
        //kDebug()<<this<<source_column<<m_rejects<<(! m_rejects.contains( source_column ));
        return ! m_rejects.contains( source_column );
    }

private:
    QList<int> m_rejects;
    QList<int> m_zerocolumns;
};

class KPLATOMODELS_EXPORT ChartItemModel : public ItemModelBase
{
    Q_OBJECT
    Q_ENUMS( Properties )
public:
    enum Properties {
        BCWSCost,
        BCWPCost,
        ACWPCost,
        BCWSEffort,
        BCWPEffort,
        ACWPEffort,
        SPICost,
        CPICost,
        SPIEffort,
        CPIEffort
    };
    const QMetaEnum columnMap() const;

    explicit ChartItemModel(QObject *parent = 0);


//    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

    virtual QModelIndex parent( const QModelIndex & index ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;

    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const;
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const;

    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;

    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;


    const EffortCostMap &bcwp() const { return m_bcws; }
    const EffortCostMap &acwp() const { return m_acwp; }

    void setProject( Project *project );

    void setNodes( const QList<Node*> &nodes );
    void addNode( Node *node );
    void clearNodes();
    QDate startDate() const;
    QDate endDate() const;
    void calculate();

    void setLocalizeValues( bool on );

public slots:
    void setScheduleManager( ScheduleManager *sm );
    void slotNodeRemoved( Node *node );
    void slotNodeChanged( Node *node );
    void slotResourceChanged( Resource *resource );
    void slotResourceChanged( const Resource *resource );

protected:
    double bcwsEffort( int day ) const;
    double bcwpEffort( int day ) const;
    double acwpEffort( int day ) const;
    double bcwsCost( int day ) const;
    double bcwpCost( int day ) const;
    double acwpCost( int day ) const;
    double spiEffort( int day ) const;
    double cpiEffort( int day ) const;
    double spiCost( int day ) const;
    double cpiCost( int day ) const;

protected:
    QList<Node*> m_nodes;
    EffortCostMap m_bcws;
    EffortCostMap m_acwp;
    bool m_localizeValues;
};

class KPLATOMODELS_EXPORT PerformanceDataCurrentDateModel : public ChartItemModel
{
    Q_OBJECT
public:
    explicit PerformanceDataCurrentDateModel(QObject *parent);

    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
    QVariant data( const QModelIndex &proxyIndex, int role = Qt::DisplayRole ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    QModelIndex mapIndex( const QModelIndex &idx ) const;
};

} //namespace KPlato

#endif
