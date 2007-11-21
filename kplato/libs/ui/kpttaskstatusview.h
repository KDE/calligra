/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

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

#include "kplatoui_export.h"

#include "kptitemmodelbase.h"

#include "kptviewbase.h"

class KoDocument;

class KAction;

namespace KPlato
{

class Project;
class Node;
class Task;
class ScheduleManager;
class TaskStatusItemModel;

typedef QList<Node*> NodeList;


class KPLATOUI_EXPORT TaskStatusTreeView : public DoubleTreeViewBase
{
    Q_OBJECT
public:
    TaskStatusTreeView( QWidget *parent );
    
    //void setSelectionModel( QItemSelectionModel *selectionModel );

    TaskStatusItemModel *model() const;
    
    Project *project() const;
    void setProject( Project *project );
    
protected slots:
    void slotActivated( const QModelIndex index );
    
protected:
    void dragMoveEvent(QDragMoveEvent *event);

};


class KPLATOUI_EXPORT TaskStatusView : public ViewBase
{
    Q_OBJECT
public:
    TaskStatusView( KoDocument *part, QWidget *parent );
    
    void setupGui();
    virtual void setProject( Project *project );
    using ViewBase::draw;
    virtual void draw( Project &project );

    TaskStatusItemModel *model() const { return m_view->model(); }
    
    virtual void updateReadWrite( bool readwrite );
    virtual Node *currentNode() const;
    
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

private slots:
    void slotContextMenuRequested( const QModelIndex &index, const QPoint& pos );
    void slotContextMenuRequested( Node *node, const QPoint& pos );
    void slotHeaderContextMenuRequested( const QPoint& );
    void slotSplitView();
    void slotOptions();
    
private:
    Project *m_project;
    int m_id;
    TaskStatusTreeView *m_view;

    // View options context menu
    KAction *actionOptions;
};


} //namespace KPlato


#endif
