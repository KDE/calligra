/* This file is part of the KDE project
  Copyright (C) 2006 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation;
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

#ifndef TASKEDTIOR_H
#define TASKEDTIOR_H

#include "kptglobal.h"
#include "kptitemmodelbase.h"
#include "kptviewbase.h"

#include <QAbstractItemModel>
#include <QFrame>
#include <QItemDelegate>
#include <QTreeView>

#include <klocale.h>

#include "KoView.h"

class QListWidget;
class QModelIndex;
class QTreeWidgetItem;

class KAction;

namespace KPlato
{

class Project;
class Node;

class NodeItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    explicit NodeItemModel( Part *part, QObject *parent = 0 );
    ~NodeItemModel();
    
    virtual void setProject( Project *project );
    
    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
    
    virtual QModelIndex parent( const QModelIndex & index ) const;
    virtual bool hasChildren( const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    
    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const; 
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const; 
    virtual bool insertRows( int row, int count, const QModelIndex & parent = QModelIndex() );
    virtual bool removeRows( int row, int count, const QModelIndex & parent = QModelIndex() );
    
    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const; 
    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    
    virtual void sort( int column, Qt::SortOrder order = Qt::AscendingOrder );
    
    virtual QMimeData * mimeData( const QModelIndexList & indexes ) const;
    virtual QStringList mimeTypes () const;
    
    Node *node( const QModelIndex &index ) const;
    QItemDelegate *createDelegate( int column, QWidget *parent ) const;

protected slots:
    void slotNodeChanged( Node* );
    
protected:
    QVariant name( const Node *node, int role ) const;
    bool setName( Node *node, const QVariant &value, int role );
    QVariant leader( const Node *node, int role ) const;
    bool setLeader( Node *node, const QVariant &value, int role );
    QVariant description( const Node *node, int role ) const;
    bool setDescription( Node *node, const QVariant &value, int role );
    QVariant type( const Node *node, int role ) const;
    bool setType( Node *node, const QVariant &value, int role );
    QVariant constraint( const Node *node, int role ) const;
    bool setConstraint( Node *node, const QVariant &value, int role );
    QVariant constraintStartTime( const Node *node, int role ) const;
    bool setConstraintStartTime( Node *node, const QVariant &value, int role );
    QVariant constraintEndTime( const Node *node, int role ) const;
    bool setConstraintEndTime( Node *node, const QVariant &value, int role );
    QVariant estimateType( const Node *node, int role ) const;
    bool setEstimateType( Node *node, const QVariant &value, int role );
    QVariant estimate( const Node *node, int role ) const;
    bool setEstimate( Node *node, const QVariant &value, int role );
    QVariant optimisticRatio( const Node *node, int role ) const;
    bool setOptimisticRatio( Node *node, const QVariant &value, int role );
    QVariant pessimisticRatio( const Node *node, int role ) const;
    bool setPessimisticRatio( Node *node, const QVariant &value, int role );
    
    QVariant test( const Node *node, int role ) const;
    bool setTest( Node *node, const QVariant &value, int role );

};

class NodeTreeView : public QTreeView
{
    Q_OBJECT
public:
    NodeTreeView( Part *part, QWidget *parent );
    
    //void setSelectionModel( QItemSelectionModel *selectionModel );

    NodeItemModel *itemModel() const { return static_cast<NodeItemModel*>( model() ); }
    
    void setArrowKeyNavigation( bool on ) { m_arrowKeyNavigation = on; }
    bool arrowKeyNavigation() const { return m_arrowKeyNavigation; }
    
    Project *project() const { return itemModel()->project(); }
    void setProject( Project *project ) { itemModel()->setProject( project ); }
    
signals:
    void currentChanged( const QModelIndex& );
    void currentColumnChanged( QModelIndex, QModelIndex );
    void selectionChanged( const QModelIndexList );
    
protected slots:
    void headerContextMenuRequested( const QPoint &pos );
    void slotActivated( const QModelIndex index );
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    virtual void currentChanged ( const QModelIndex & current, const QModelIndex & previous );
    
protected:
    void keyPressEvent(QKeyEvent *event);
    QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index, const QEvent *event) const;

private:
    bool m_arrowKeyNavigation;
};

class TaskEditor : public ViewBase
{
    Q_OBJECT
public:
    TaskEditor( Part *part, QWidget *parent );
    
    void setupGui();
    virtual void draw( Project &project );
    virtual void draw();

    virtual Node *currentNode() const;
    QList<Node*> selectedNodes() const ;
    Node *selectedNode() const;

    virtual void updateReadWrite( bool /*readwrite*/ ) {};

signals:
    void openNode();
    void addTask();
    void addMilestone();
    void addSubtask();
    void deleteTaskList( QList<Node*> );
    void moveTaskUp();
    void moveTaskDown();
    void indentTask();
    void unindentTask();

public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

protected:
    void updateActionsEnabled( bool on );
    int selectedNodeCount() const;
    
private slots:
    void slotSelectionChanged( const QModelIndexList );
    void slotCurrentChanged( const QModelIndex& );
    void slotEnableActions();

    void slotAddTask();
    void slotAddSubtask();
    void slotAddMilestone();
    void slotDeleteTask();
    void slotIndentTask();
    void slotUnindentTask();
    void slotMoveTaskUp();
    void slotMoveTaskDown();

private:
    NodeTreeView *m_editor;

    KAction *actionAddTask;
    KAction *actionAddMilestone;
    KAction *actionAddSubtask;
    KAction *actionDeleteTask;
    KAction *actionMoveTaskUp;
    KAction *actionMoveTaskDown;
    KAction *actionIndentTask;
    KAction *actionUnindentTask;

};


} //namespace KPlato


#endif
