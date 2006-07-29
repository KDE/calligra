/* This file is part of the KDE project
   Copyright (C) 2003 - 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTREQUESTRESOURCESPANEL_H
#define KPTREQUESTRESOURCESPANEL_H

#include "kpttaskresourcespanelbase.h"
#include "kptduration.h"

#include <qlistview.h>
#include <qstring.h>
#include <qtable.h>
//class QCheckTableItem;

class KCommand;
class KMacroCommand;

namespace KPlato
{

class Account;
class Accounts;
class Task;
class ResourceGroup;
class Resource;
class ResourceGroupRequest;
class ResourceRequest;
class StandardWorktime;
class Part;
class Duration;

class ResourceTableItem {
public:
    ResourceTableItem(Resource *resource, ResourceRequest *request, bool check = false);
    ~ResourceTableItem() ;

    void update();
    void insert(QTable *table, int row);
    void ok(ResourceGroupRequest *group);

    bool isChecked() const { return m_checked; }
    bool isOrigChecked() const { return m_origChecked; }
    Resource *resource() { return m_resource; }
    ResourceRequest *request() { return m_request; }
    int numRequests() const { return m_checked ? 1 : 0; }
    int units() const { return m_units; }

    Resource *m_resource;
    int m_units, m_origUnits;
    bool m_checked, m_origChecked;
    QCheckTableItem *m_checkitem;
    ResourceRequest *m_request;
    int m_curAccountItem;
    QString m_curAccountText;
};

class GroupLVItem : public QListViewItem {
public:
    GroupLVItem(QListView *parent, ResourceGroup *group, Task &task);
    ~GroupLVItem();

    void update();
    void insert(QTable *table);
    const QPtrList<ResourceTableItem> &resources() const { return m_resources; }
    void ok(Task &task);

    int numRequests();
    bool isNull() const;
    
    ResourceGroup *m_group;
    int m_units;
    QPtrList<ResourceTableItem> m_resources;
    ResourceGroupRequest *m_request;
};


class RequestResourcesPanel : public TaskResourcesPanelBase {
    Q_OBJECT
public:
    RequestResourcesPanel(QWidget *parent, Task &task, bool baseline=false);

    KCommand *buildCommand(Part *part);
    
    bool ok();
    
private slots:
    void sendChanged();

    void groupChanged(QListViewItem *item);
    void resourceChanged(int, int);
    void unitsChanged(int);
    
signals:
    void changed();

private:
    Task &m_task;
    StandardWorktime *m_worktime;
    GroupLVItem *selectedGroup;
    bool m_blockChanged;
    
};

}  //KPlato namespace

#endif
