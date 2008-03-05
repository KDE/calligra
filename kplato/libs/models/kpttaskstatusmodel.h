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

typedef QList<Node*> NodeList;

class KPLATOMODELS_EXPORT TaskStatusItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    explicit TaskStatusItemModel( QObject *parent = 0 );
    ~TaskStatusItemModel();
    
    /// Returns a column number/- name map for this model
    virtual const QMetaEnum columnMap() const { return m_nodemodel.columnMap(); }

    virtual void setProject( Project *project );
    void setManager( ScheduleManager *sm );
    
    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
    
    virtual QModelIndex parent( const QModelIndex & index ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex index( const Node *node ) const;
    virtual QModelIndex index( const NodeList *lst ) const;
    
    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const; 
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const; 
    
    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const; 
    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    
    virtual QMimeData * mimeData( const QModelIndexList & indexes ) const;
    virtual QStringList mimeTypes () const;
    virtual Qt::DropActions supportedDropActions() const;
    virtual bool dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent );

    NodeList *list( const QModelIndex &index ) const;
    Node *node( const QModelIndex &index ) const;
    QItemDelegate *createDelegate( int column, QWidget *parent ) const;
    
    NodeList nodeList( QDataStream &stream );
    static NodeList removeChildNodes( const NodeList nodes );
    using ItemModelBase::dropAllowed;
    bool dropAllowed( Node *on, const QMimeData *data );
    
    void clear();
    void refresh();
    
    void setNow( const QDate &date ) { m_nodemodel.setNow( date ); }
    void setPeriod( int days ) { m_period = days; }
    
protected slots:
    void slotAboutToBeReset();
    void slotReset();

    void slotNodeChanged( Node* );
    void slotNodeToBeInserted( Node *node, int row );
    void slotNodeInserted( Node *node );
    void slotNodeToBeRemoved( Node *node );
    void slotNodeRemoved( Node *node );

protected:
    QVariant alignment( int column ) const;
    
    QVariant name( int row, int role ) const;

private:
    NodeModel m_nodemodel;
    QStringList m_topNames;
    QList<NodeList*> m_top;
    NodeList m_notstarted;
    NodeList m_running;
    NodeList m_finished;
    NodeList m_upcoming;
    
    long m_id; // schedule id
    int m_period;
};

} //namespace KPlato


#endif
