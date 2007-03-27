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

#ifndef TASKSTATUSVIEW_H
#define TASKSTATUSVIEW_H

#include "kptitemmodelbase.h"
#include "kptviewbase.h"

class QTreeWidgetItem;

class KAction;

namespace KPlato
{

class Project;
class Node;
class Task;

typedef QList<Node*> NodeList;

class TaskStatusItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    explicit TaskStatusItemModel( Part *part, QObject *parent = 0 );
    ~TaskStatusItemModel();
    
    virtual void setProject( Project *project );
    
    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
    
    virtual QModelIndex parent( const QModelIndex & index ) const;
    virtual bool hasChildren( const QModelIndex & parent = QModelIndex() ) const;
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
    bool dropAllowed( Node *on, const QMimeData *data );
    
    void clear();
    void refresh();
    
    void setNow( const QDate &date ) { m_now = date; }
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
    QVariant name( const Node *node, int role ) const;
    QVariant status( const Node *node, int role ) const;
    QVariant completed( const Task *node, int role ) const;
    QVariant startTime( const Node *node, int role ) const;
    QVariant endTime( const Node *node, int role ) const;
    QVariant plannedEffortTo( const Node *node, int role ) const;
    QVariant actualEffortTo( const Node *node, int role ) const;
    QVariant plannedCostTo( const Node *node, int role ) const;
    QVariant actualCostTo( const Node *node, int role ) const;
    QVariant note( const Node *node, int role ) const;

private:
    QStringList m_topNames;
    QList<NodeList*> m_top;
    NodeList m_notstarted;
    NodeList m_running;
    NodeList m_finished;
    NodeList m_upcomming;
    
    long m_id; // schedule id
    QDate m_now;
    int m_period;
};


class TaskStatusTreeView : public TreeViewBase
{
    Q_OBJECT
public:
    TaskStatusTreeView( Part *part, QWidget *parent );
    
    //void setSelectionModel( QItemSelectionModel *selectionModel );

    TaskStatusItemModel *itemModel() const { return static_cast<TaskStatusItemModel*>( model() ); }
    
    Project *project() const { return itemModel()->project(); }
    void setProject( Project *project ) { itemModel()->setProject( project ); }
    
signals:
    void currentChanged( const QModelIndex& );
    void currentColumnChanged( QModelIndex, QModelIndex );
    void selectionChanged( const QModelIndexList );
    void contextMenuRequested( Node *node, const QPoint &pos );
    
protected slots:
    void headerContextMenuRequested( const QPoint &pos );
    void slotActivated( const QModelIndex index );
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    virtual void currentChanged ( const QModelIndex & current, const QModelIndex & previous );
    
protected:
    void contextMenuEvent( QContextMenuEvent * event );
    void dragMoveEvent(QDragMoveEvent *event);

};


class TaskStatusView : public ViewBase
{
    Q_OBJECT
public:
    TaskStatusView( Part *part, QWidget *parent );
    
    void setupGui();
    virtual void draw( Project &project );
    virtual void draw();

    virtual void updateReadWrite( bool /*readwrite*/ ) {};
    virtual Node *currentNode() const;
    
signals:
    void requestPopupMenu( const QString&, const QPoint & );
    void openNode();

public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

protected:
    void updateActionsEnabled( bool on );

private slots:
    void slotContextMenuRequested( Node *node, const QPoint& pos );
    
    void slotAddTask();

private:
    Project *m_project;
    int m_id;
    TaskStatusTreeView *m_view;

    KAction *actionAddTask;

};


} //namespace KPlato


#endif
