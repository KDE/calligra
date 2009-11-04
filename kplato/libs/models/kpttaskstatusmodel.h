/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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

#ifndef TASKSTATUSMODEL_H
#define TASKSTATUSMODEL_H

#include "kplatomodels_export.h"

#include "kptitemmodelbase.h"
#include "kptnodeitemmodel.h"

class KAction;

namespace KPlato
{

class Project;
class Node;
class Task;

typedef QMap<QString, Node*> NodeMap;

class KPLATOMODELS_EXPORT TaskStatusItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    explicit TaskStatusItemModel( QObject *parent = 0 );
    ~TaskStatusItemModel();
    
    enum PeriodType { UseCurrentDate, UseWeekday };
    int periodType() const { return m_periodType; }
    void setPeriodType( int type ) { m_periodType = type; }
    
    /// Returns a column number/- name map for this model
    virtual const QMetaEnum columnMap() const { return m_nodemodel.columnMap(); }

    virtual void setProject( Project *project );
    void setManager( ScheduleManager *sm );
    
    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
    
    virtual QModelIndex parent( const QModelIndex & index ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex index( const Node *node ) const;
    virtual QModelIndex index( const NodeMap *lst ) const;
    
    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const; 
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const; 
    
    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const; 
    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    
    virtual QMimeData * mimeData( const QModelIndexList & indexes ) const;
    virtual QStringList mimeTypes () const;
    virtual Qt::DropActions supportedDropActions() const;
    virtual bool dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent );

    NodeMap *list( const QModelIndex &index ) const;
    Node *node( const QModelIndex &index ) const;
    QAbstractItemDelegate *createDelegate( int column, QWidget *parent ) const;
    
    NodeMap nodeList( QDataStream &stream );
    static NodeMap removeChildNodes( const NodeMap nodes );
    using ItemModelBase::dropAllowed;
    bool dropAllowed( Node *on, const QMimeData *data );
    
    void clear();
    
    void setNow();
    void setPeriod( int days ) { m_period = days; }
    int period() const { return m_period; }
    void setWeekday( int day ) { m_weekday = day; }
    int weekday() const { return m_weekday; }
    
public slots:
    void refresh();

protected slots:
    void slotAboutToBeReset();
    void slotReset();

    void slotNodeChanged( Node* );
    void slotNodeToBeInserted( Node *node, int row );
    void slotNodeInserted( Node *node );
    void slotNodeToBeRemoved( Node *node );
    void slotNodeRemoved( Node *node );

    void slotWbsDefinitionChanged();
    void slotLayoutChanged();

protected:
    QVariant alignment( int column ) const;
    
    QVariant name( int row, int role ) const;

    bool setCompletion( Node *node, const QVariant &value, int role );
    bool setRemainingEffort( Node *node, const QVariant &value, int role );
    bool setActualEffort( Node *node, const QVariant &value, int role );
    bool setStartedTime( Node *node, const QVariant &value, int role );
    bool setFinishedTime( Node *node, const QVariant &value, int role );

private:
    NodeModel m_nodemodel;
    QStringList m_topNames;
    QStringList m_topTips;
    QList<NodeMap*> m_top;
    NodeMap m_notstarted;
    NodeMap m_running;
    NodeMap m_finished;
    NodeMap m_upcoming;
    
    long m_id; // schedule id
    int m_period; // days
    int m_periodType;
    int m_weekday;

};

} //namespace KPlato


#endif
