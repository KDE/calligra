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

#ifndef TASKWORKPACKAGEVIEW_H
#define TASKWORKPACKAGEVIEW_H

#include "kplatowork_export.h"

#include "kptitemmodelbase.h"

#include "kptviewbase.h"

#include <QSplitter>

class QTextBrowser;
class QItemSelection;

class KoDocument;

class KAction;

namespace KPlato
{

class Project;
class Node;
class ScheduleManager;
class Document;

}
using namespace KPlato;

namespace KPlatoWork
{
class Part;

class TaskWorkPackageModel;

class KPLATOWORK_EXPORT TaskWorkPackageTreeView : public DoubleTreeViewBase
{
    Q_OBJECT
public:
    TaskWorkPackageTreeView( Part *part, QWidget *parent );
    
    
    //void setSelectionModel( QItemSelectionModel *selectionModel );

    TaskWorkPackageModel *model() const;
    
    Project *project() const;
    void setProject( Project *project );
    
    QList<Node*> selectedNodes() const;

protected slots:
    void slotActivated( const QModelIndex index );
    
protected:
    void dragMoveEvent(QDragMoveEvent *event);
};


class KPLATOWORK_EXPORT TaskWorkPackageView : public QWidget, public ViewActionLists
{
    Q_OBJECT
public:
    TaskWorkPackageView( Part *part, QWidget *parent );
    
    void setupGui();

    TaskWorkPackageModel *model() const { return m_view->model(); }
    
    virtual void updateReadWrite( bool readwrite );
    Node *currentNode() const;
    Document *currentDocument() const;
    QList<Node*> selectedNodes() const;
    
    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &/*context*/ ) const;

    KoPrintJob *createPrintJob();
    
signals:
    void requestPopupMenu( const QString& name, const QPoint &pos );
    void selectionChanged();

public slots:
    void slotHeaderContextMenuRequested( const QPoint& );

protected slots:
    virtual void slotOptions();

protected:
    void updateActionsEnabled( bool on );

private slots:
    void slotContextMenuRequested( const QModelIndex &index, const QPoint& pos );
    void slotContextMenuRequested( Node *node, const QPoint& pos );
    void slotContextMenuRequested( Document *doc, const QPoint& pos );
    void slotSplitView();
    void slotSelectionChanged( const QModelIndexList lst );

private:
    TaskWorkPackageTreeView *m_view;

};


} //namespace KPlatoWork


#endif
