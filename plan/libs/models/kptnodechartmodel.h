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

#include <KDebug>

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
    ChartProxyModel( QObject *parent = 0 ) : QSortFilterProxyModel( parent ) {}

    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const {
        //if ( role == Qt::DisplayRole || role == KDChart::DatasetBrushRole ) kDebug()<<"fetch:"<<orientation<<section<<mapToSource( index(0, section) ).column()<<m_rejects;
        return QSortFilterProxyModel::headerData( section, orientation, role );
    }

    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const {
        if ( role == Qt::DisplayRole && ! m_zerocolumns.isEmpty() ) {
            if ( m_zerocolumns.contains( mapToSource( index ).column() ) ) {
                //kDebug()<<"zero:"<<index.column()<<mapToSource( index ).column();
                return QVariant();
            }
        }
        //if ( role == Qt::DisplayRole ) kDebug()<<"fetch:"<<index.column()<<mapToSource( index ).column()<<m_rejects;
        return QSortFilterProxyModel::data( index, role );
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
public:
    ChartItemModel( QObject *parent = 0 );

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

public slots:
    void setScheduleManager( ScheduleManager *sm );
    void slotNodeRemoved( Node *node );
    void slotNodeChanged( Node *node );
    void slotResourceChanged( Resource *resource );
    void slotResourceChanged( const Resource *resource );

protected:
    double bcwsEffort( int day ) const;
    QVariant bcwpEffort( int day ) const;
    double acwpEffort( int day ) const;
    double bcwsCost( int day ) const;
    QVariant bcwpCost( int day ) const;
    double acwpCost( int day ) const;

private:
    QList<Node*> m_nodes;
    EffortCostMap m_bcws;
    EffortCostMap m_acwp;
};


} //namespace KPlato

#endif
