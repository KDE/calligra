/* This file is part of the KDE project
  Copyright (C) 2003 - 2006 Frederic BECQUIER <frederic.becquier@gmail.com>

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

#include "kptviewbase.h"
#include "kptcontext.h"

#include <q3ptrlist.h>
#include <QTreeWidget>

class QLayout;
class Q3ListViewItem;
class QPoint;
class Q3ListView;
class QLineEdit;
class QSpinBox;
class QSplitter;

class QTreeWidgetItem;

class KAction;
class KPrinter;

namespace KPlato
{
    class ResourcesList;

    class ResourceItemPrivate;

    class ResourcesList : public QTreeWidget
    {
        Q_OBJECT

    public:
        ResourcesList( QWidget * parent = 0 );
    };

    class ResourceAssignmentView : public ViewBase
    {
        Q_OBJECT

        public:
            ResourceAssignmentView( Part *part, QWidget *parent);
            void draw( Project &project );
            void drawResourcesName( QTreeWidgetItem *parent, ResourceGroup *group );
            void drawTasksAttributedToAResource (Resource *res, QTreeWidgetItem *parent);
            void drawTasksAttributedToAGroup (ResourceGroup *group, QTreeWidgetItem *parent);

        private:
            QSplitter *m_splitter;
            ResourcesList *m_resList;
            ResourcesList *m_taskList;
            QTreeWidgetItem *m_selectedItem;
            QTreeWidgetItem *m_tasktreeroot;


            Part *m_part;

            void updateTasks();

        protected slots:
            void resSelectionChanged();
            void resSelectionChanged( QTreeWidgetItem *item );

    };


}  //KPlato namespace

#endif

