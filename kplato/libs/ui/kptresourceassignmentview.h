/* This file is part of the KDE project
  Copyright (C) 2006 - 2007 Frederic BECQUIER <frederic.becquier@gmail.com>

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

#ifndef KPTRESOURCEASSIGNMENTVIEW_H
#define KPTRESOURCEASSIGNMENTVIEW_H

#include "kplatoui_export.h"

#include "kptviewbase.h"

#include <QTreeWidget>

#include <QList>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QTreeWidget>

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QSpinBox>
#include <QtGui/QLineEdit>

#include <qwidget.h>
#include <kptitemmodelbase.h>

#include <kactionselector.h>

#include "kpttask.h"
#include "kpttaskeditor.h"

#include "kptcommand.h"

#include <kicon.h>
#include <kglobal.h>
#include <klocale.h>
#include <kxmlguifactory.h>
#include <kactioncollection.h>

#include <kdebug.h>

#include "ui_kptresourceassignmentview.h"


class KoDocument;

class QPoint;
class QLineEdit;
class QSpinBox;
class QSplitter;

class QTreeWidgetItem;


 
namespace KPlato
{

class ResourcesList;


class KPLATOUI_EXPORT ResourcesList : public QTreeWidget
{
    Q_OBJECT

public:
    ResourcesList( QWidget * parent = 0 );

};

class KPLATOUI_EXPORT ResourceAssignmentView : public ViewBase
{
    Q_OBJECT

public:
    ResourceAssignmentView( KoDocument *part, QWidget *parent);
    using ViewBase::draw;
    Project *project() const { return m_project; }
    void draw( Project &project );
    void drawResourcesName( QTreeWidgetItem *parent, ResourceGroup *group );
    void drawTasksAttributedToAResource (Resource *res, QTreeWidgetItem *parent);
    void drawTasksAttributedToAGroup (ResourceGroup *group, QTreeWidgetItem *parent);

private:
    QSplitter *m_splitter;
    ResourcesList *m_resList;
    Project *m_project;
    ResourcesList *m_taskList;
    QTreeWidgetItem *m_selectedItem;
    QTreeWidgetItem *m_tasktreeroot;
    KoDocument *m_part;
    ::Ui::ResourceAssignmentView widget;

    void updateTasks();

protected slots:
    void resSelectionChanged();
    void resSelectionChanged( QTreeWidgetItem *item );

private slots:
    void slotRequestPopupMenu( const QPoint &p );
void slotUpdate();

public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

};


}  //KPlato namespace

#endif

