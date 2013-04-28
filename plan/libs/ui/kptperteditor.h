/* This file is part of the KDE project
  Copyright (C) 2007 Florian Piquemal <flotueur@yahoo.fr>
  Copyright (C) 2007 Alexis Ménard <darktears31@gmail.com>
  Copyright (C) 2007 Dag Andersen <danders@get2net>

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

#ifndef KPTPERTEDITOR_H
#define KPTPERTEDITOR_H

#include "kplatoui_export.h"

#include "kptviewbase.h"
#include "kptitemmodelbase.h"

#include <QList>
#include <QTreeWidget>
#include <QTableWidget>
#include <QFont>

#include <QVariant>

#include <QWidget>

#include <kactionselector.h>

#include "kpttaskeditor.h"
#include <ui_kptperteditor.h>

#include "kptcommand.h"

#include "kptnode.h"

#include <kglobal.h>
#include <klocale.h>
#include <kxmlguifactory.h>
#include <kactioncollection.h>

#include <kdebug.h>

class KoDocument;

class QTreeWidgetItem;
class QModelIndex;
class KUndo2Command;

namespace KPlato
{

class View;
class Project;
class RelationTreeView;

class KPLATOUI_EXPORT PertEditor : public ViewBase
{
    Q_OBJECT
public:

    enum Roles { NodeRole = Qt::UserRole + 1, EnabledRole };
    
    PertEditor(KoPart *part, KoDocument *doc, QWidget *parent);
    virtual void updateReadWrite( bool readwrite );
    void setProject( Project *project );
    Project *project() const { return m_project; }
    void draw( Project &project );
    void draw();
    void drawSubTasksName( QTreeWidgetItem *parent,Node * currentNode);
    void clearRequiredList();
    void loadRequiredTasksList(Node * taskNode);
    Node *itemToNode( QTreeWidgetItem *item );
    QTreeWidgetItem *nodeToItem( Node *node, QTreeWidgetItem *item );
    QList<Node*> listNodeNotView(Node * node);

    void updateAvailableTasks( QTreeWidgetItem *item = 0 );
    void setAvailableItemEnabled( QTreeWidgetItem *item );
    void setAvailableItemEnabled( Node *node );
    
signals:
    void executeCommand( KUndo2Command* );

protected:
    bool isInRequiredList( Node *node );
    QTreeWidgetItem *findNodeItem( Node *node, QTreeWidgetItem *item );
    QTableWidgetItem *findRequiredItem( Node *node );
    
private slots:
    void slotNodeAdded( Node* );
    void slotNodeRemoved( Node* );
    void slotNodeMoved( Node* );
    void slotNodeChanged( Node* );
    void slotRelationAdded( Relation *rel );
    void slotRelationRemoved( Relation *rel );
    
    void dispAvailableTasks();
    void dispAvailableTasks( Node *parent, Node *selectedTask );
    void dispAvailableTasks( Relation *rel );
    void addTaskInRequiredList(QTreeWidgetItem * currentItem);
    void removeTaskFromRequiredList();
    void slotUpdate();

    void slotCurrentTaskChanged( QTreeWidgetItem *curr, QTreeWidgetItem *prev );
    void slotAvailableChanged( QTreeWidgetItem *item );
    void slotRequiredChanged( const QModelIndex &index );
    void slotAddClicked();
    void slotRemoveClicked();

private:
    Project * m_project;
    QTreeWidget *m_tasktree;
    QTreeWidget *m_availableList;
    RelationTreeView *m_requiredList;
    
    Ui::PertEditor widget;
};

}  //KPlato namespace

#endif
