/* This file is part of the KDE project
  Copyright (C) 2007, 2012 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version..

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#ifndef WORKPACKAGEMODEL_H
#define WORKPACKAGEMODEL_H

#include "kplatomodels_export.h"

#include <QSortFilterProxyModel>

class QModelIndex;
class QVariant;

/// The main namespace
namespace KPlato
{

class WorkPackage;
class Node;
class Task;
class Project;
class ScheduleManager;
class NodeItemModel;

class KPLATOMODELS_EXPORT WorkPackageModel : public QObject
{
    Q_OBJECT
    Q_ENUMS( Properties )
public:
    explicit WorkPackageModel( QObject *parent = 0 )
        : QObject( parent )
     {}
    ~WorkPackageModel() {}
    
    QVariant data( const WorkPackage *wp, int column, int role = Qt::DisplayRole ) const;

protected:
    QVariant nodeName(const WorkPackage *wp, int role ) const;
    QVariant ownerName(const WorkPackage *wp, int role ) const;
    QVariant transmitionStatus(const WorkPackage *wp, int role ) const;
    QVariant transmitionTime(const WorkPackage *wp, int role ) const;

    QVariant completion( const WorkPackage *wp, int role ) const;
};

/**
  The WPSortFilterProxyModel only accepts scheduled tasks.
*/
class WPSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    WPSortFilterProxyModel( QObject *parent = 0 ) : QSortFilterProxyModel( parent ) {}
protected:
    /// Only accept scheduled tasks
    bool filterAcceptsRow(int source_row, const QModelIndex &sourceParent) const;
};

/**
  The WorkPackageProxyModel offers a flat list of tasks with workpackage log entries as children.

  The tasks is fetched from the WPSortFilterProxyModel, the work packages is added by this model.

  It uses the NodeItemModel to get the tasks, the FlatProxyModel to convert to a flat list,
  and the WPSortFilterProxyModel to accept only scheduled tasks.
  It depends on the fact that the WPSortFilterProxyModel holds a flat list.
*/
class KPLATOMODELS_EXPORT WorkPackageProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    WorkPackageProxyModel( QObject *parent = 0 );

    Qt::ItemFlags flags(const QModelIndex &index) const;
    void setSourceModel( QAbstractItemModel *sourceModel );
    bool hasChildren(const QModelIndex &parent) const;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const;
    QModelIndex parent( const QModelIndex &child ) const;
    QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
    QVariant data( const QModelIndex &idx, int role = Qt::DisplayRole ) const;

    NodeItemModel *baseModel() const;

    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;

    Task *taskFromIndex( const QModelIndex &idx ) const;
    QModelIndex indexFromTask( const Node *node ) const;

public slots:
    void setProject( Project *project );
    void setScheduleManager( ScheduleManager *sm );

protected slots:
    void sourceDataChanged(const QModelIndex& start, const QModelIndex& end);
    void sourceModelAboutToBeReset();
    void sourceModelReset();
    void sourceRowsAboutToBeInserted(const QModelIndex&, int, int );
    void sourceRowsInserted(const QModelIndex&, int, int end);
    void sourceRowsAboutToBeRemoved(const QModelIndex& parent, int start, int end);
    void sourceRowsRemoved(const QModelIndex& parent, int start, int);
    void sourceRowsAboutToBeMoved( const QModelIndex&, int sourceStart, int sourceEnd, const QModelIndex&, int destStart );
    void sourceRowsMoved( const QModelIndex&, int , int , const QModelIndex&, int );
    void workPackageToBeAdded(Node*, int);
    void workPackageAdded(Node*);
    void workPackageToBeRemoved(Node*, int);
    void workPackageRemoved(Node*);

protected:
    QModelIndex mapFromBaseModel( const QModelIndex &idx ) const;
    void detachTasks( Task *task = 0 );
    void attachTasks( Task *task = 0 );

    inline bool isTaskIndex( const QModelIndex &idx ) const {
        return idx.isValid() && ! idx.internalPointer();
    }
    inline bool isWorkPackageIndex( const QModelIndex &idx ) const {
        return idx.isValid() && idx.internalPointer();
    }

private:
    WorkPackageModel m_model;
    NodeItemModel *m_nodemodel;
    QList<QAbstractProxyModel*> m_proxies;
};

} //namespace KPlato

#endif //WORKPACKAGEMODEL_H
