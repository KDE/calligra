/* This file is part of the KDE project
  Copyright (C) 2006 -2007 Dag Andersen <danders@get2net.dk>

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

#ifndef TASKEDTIOR_H
#define TASKEDTIOR_H

#include "kplatoui_export.h"

#include "kptglobal.h"
#include "kptnodeitemmodel.h"
#include "kptviewbase.h"


#include <klocale.h>

#include "KoView.h"

class KoDocument;

class QModelIndex;

class KAction;

namespace KPlato
{

class Project;
class Node;
class NodeItemModel;

class KPLATOUI_EXPORT NodeTreeView : public DoubleTreeViewBase
{
    Q_OBJECT
public:
    NodeTreeView( QWidget *parent );
    
    //void setSelectionModel( QItemSelectionModel *selectionModel );

    NodeItemModel *model() const { return static_cast<NodeItemModel*>( DoubleTreeViewBase::model() ); }
    
    Project *project() const { return model()->project(); }
    void setProject( Project *project ) { model()->setProject( project ); }
    
signals:
    void currentColumnChanged( QModelIndex, QModelIndex );
    
protected slots:
    void slotActivated( const QModelIndex index );
    
};

class KPLATOUI_EXPORT TaskEditor : public ViewBase
{
    Q_OBJECT
public:
    TaskEditor( KoDocument *part, QWidget *parent );
    
    void setupGui();
    virtual void draw( Project &project );
    virtual void draw();

    virtual Node *currentNode() const;
    QList<Node*> selectedNodes() const ;
    Node *selectedNode() const;

    virtual void updateReadWrite( bool readwrite );

    NodeItemModel *model() const { return m_view->model(); }

    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &/*context*/ ) const;

signals:
    void requestPopupMenu( const QString&, const QPoint & );
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

    void slotCurrentScheduleManagerChanged( ScheduleManager *sm );
    
protected:
    void updateActionsEnabled( bool on );
    int selectedNodeCount() const;

private slots:
    void slotSelectionChanged( const QModelIndexList );
    void slotCurrentChanged( const QModelIndex&, const QModelIndex& );
    void slotContextMenuRequested( const QModelIndex &index, const QPoint& pos );
    
    void slotEnableActions();

    void slotAddTask();
    void slotAddSubtask();
    void slotAddMilestone();
    void slotDeleteTask();
    void slotIndentTask();
    void slotUnindentTask();
    void slotMoveTaskUp();
    void slotMoveTaskDown();

    void slotSplitView();
    void slotOptions();
    
    void slotHeaderContextMenuRequested( const QPoint& );
    
private:
    void edit( QModelIndex index );

private:
    NodeTreeView *m_view;

    KAction *actionAddTask;
    KAction *actionAddMilestone;
    KAction *actionAddSubtask;
    KAction *actionDeleteTask;
    KAction *actionMoveTaskUp;
    KAction *actionMoveTaskDown;
    KAction *actionIndentTask;
    KAction *actionUnindentTask;

    // View options context menu
    KAction *actionOptions;
};

class KPLATOUI_EXPORT TaskView : public ViewBase
{
    Q_OBJECT
public:
    TaskView( KoDocument *part, QWidget *parent );

    void setupGui();
    virtual void draw( Project &project );
    virtual void draw();

    virtual Node *currentNode() const;
    QList<Node*> selectedNodes() const ;
    Node *selectedNode() const;

    virtual void updateReadWrite( bool readwrite );

    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &/*context*/ ) const;

signals:
    void requestPopupMenu( const QString&, const QPoint & );
    void openNode();

public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

    void slotCurrentScheduleManagerChanged( ScheduleManager *sm );

protected:
    void updateActionsEnabled( bool on );
    int selectedNodeCount() const;

private slots:
    void slotSelectionChanged( const QModelIndexList );
    void slotCurrentChanged( const QModelIndex&, const QModelIndex& );
    void slotContextMenuRequested( const QModelIndex &index, const QPoint& pos );

    void slotEnableActions();

    void slotSplitView();
    void slotOptions();

    void slotHeaderContextMenuRequested( const QPoint& );

private:
    NodeTreeView *m_view;

    // View options context menu
    KAction *actionSplitView;
    KAction *actionOptions;
};

} //namespace KPlato


#endif
