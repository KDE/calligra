/* This file is part of the KDE project
   Copyright (C) 2003 - 2007 Dag Andersen <danders@get2net.dk>

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

#include "kptrequestresourcespanel.h"
#include "kpttask.h"
#include "kptproject.h"
#include "kptresource.h"
#include "kptcalendar.h"

#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kptcommand.h>

#include <QPushButton>
#include <QString>
#include <qvalidator.h>
//#include <QTableWidgetItem>
#include <qtablewidget.h>

#include <q3datetimeedit.h>

namespace KPlato
{

ResourceTableItem::ResourceTableItem(Resource *resource, ResourceRequest *request, bool check) {
    m_resource = resource;
    m_request = request;
    m_checked = check ? Qt::Checked : Qt::Unchecked;
    m_origChecked = check ? Qt::Checked : Qt::Unchecked;
    m_checkitem = 0;
    m_units = 100;
    m_origUnits = 100;
    //kDebug()<<"Added: '"<<resource->name()<<"' checked="<<m_checked;
}

ResourceTableItem::~ResourceTableItem() {
    //kDebug()<<m_resource->name();
}

void ResourceTableItem::update() {
    if (m_checkitem)
        m_checked = m_checkitem->checkState();
    //kDebug()<<m_resource->name()<<" checked="<<m_checked;
}

void ResourceTableItem::insert(QTableWidget *table, int row) {
    //kDebug();
    m_checkitem = new QTableWidgetItem(m_resource->name());
    m_checkitem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    m_checkitem->setCheckState(m_checked);
    table->setItem(row, 0, m_checkitem);

    //kDebug()<<"Added: '"<<m_resource->name()<<"' checked="<<m_checked;
}

GroupLVItem::GroupLVItem(QTreeWidget *parent, ResourceGroup *group, Task &task)
    : QTreeWidgetItem(parent),
      m_group(group),
      m_units(0)
{
    setText(0, group->name());
    setText(1, QString("%1").arg(group->units()));

    m_request = task.resourceGroupRequest(group);
    if (m_request) {
        m_units = m_request->units();
    }
    foreach (Resource *r, group->resources()) {
        //kDebug()<<"resource="<<r->name();
        ResourceRequest *req=0;
        if (m_request) {
            req = m_request->find(r);
        }
        m_resources.append(new ResourceTableItem(r, req, (bool)req));
    }
}

GroupLVItem::~GroupLVItem() {
    //kDebug()<<m_group->name();

    while (!m_resources.isEmpty())
        delete m_resources.takeFirst();
}

void GroupLVItem::update() {
    foreach (ResourceTableItem *i, m_resources) {
        i->update();
    }
}

void GroupLVItem::insert(QTableWidget *table) {

    table->clear();
    table->setColumnCount(1);
    if (m_group->numResources() == 0) {
        table->setRowCount(1);
        table->setItem(0, 0, new QTableWidgetItem(i18n("None")));
    } else {
        table->setRowCount(m_group->numResources());
        int i = 0;
        foreach (ResourceTableItem *r, m_resources) {
            r->insert(table, i++);
        }
    }
}

int GroupLVItem::numRequests() {
    //kDebug();
    int value = m_units;
    foreach (ResourceTableItem *r, m_resources) {
        value += r->numRequests();
    }
    return value;
}

bool GroupLVItem::isNull() const {
    //kDebug();
    foreach (ResourceTableItem *r, m_resources) {
        if (r->isChecked())
            return false;
    }
    if (m_units > 0)
        return false;
    return true;
}

RequestResourcesPanel::RequestResourcesPanel(QWidget *parent, Task &task, bool)
    : TaskResourcesPanelBase(parent),
      m_task(task),
      m_worktime(0),
      selectedGroup(0),
      m_blockChanged(false) {

    Project *p = dynamic_cast<Project*>(task.projectNode());
    if (p) {
        m_worktime = p->standardWorktime();
        foreach (ResourceGroup *grp, p->resourceGroups()) {
            GroupLVItem *grpitem = new GroupLVItem(groupList, grp, task);
            groupList-> addTopLevelItem(grpitem);
            //kDebug()<<" Added group:"<<grp->name();
        }
    }
    QTreeWidgetItem *item = groupList->topLevelItem(0);
    if (item) {
        item->setSelected(true);
        groupChanged(item);
    }

    connect(groupList, SIGNAL(itemSelectionChanged()),  SLOT(groupChanged()));
    connect(resourceTable, SIGNAL(cellChanged(int, int)), SLOT(resourceChanged(int, int)));
//    connect(numUnits, SIGNAL(valueChanged(int)), SLOT(unitsChanged(int)));

}

void RequestResourcesPanel::groupChanged() {
    //kDebug();
    QTreeWidgetItem *item = 0;
    QList<QTreeWidgetItem*> lst = groupList->selectedItems();
    if (lst.count() > 0)
        item = lst.first();
    groupChanged(item);
}

void RequestResourcesPanel::groupChanged(QTreeWidgetItem *item) {
    //kDebug();
    GroupLVItem *grp = dynamic_cast<GroupLVItem *>(item);
    if (grp == 0)
        return;

    if (selectedGroup) {
        selectedGroup->update();
    }
    selectedGroup = grp;

/*    m_blockChanged = true;
    numUnits->setMaxValue(grp->m_group->units());
    numUnits->setValue(grp->m_units);
    m_blockChanged = false;*/
    grp->insert(resourceTable);
}

void RequestResourcesPanel::resourceChanged(int /*r*/, int /*c*/) {
    //kDebug()<<"("<<r<<","<<c<<")";
    sendChanged();
}

void RequestResourcesPanel::unitsChanged(int units) {
    //kDebug();
    if (selectedGroup) {
        selectedGroup->m_units = units;
        sendChanged();
    }
}

MacroCommand *RequestResourcesPanel::buildCommand() {
    //kDebug();
    MacroCommand *cmd = 0;
    if (selectedGroup) {
        selectedGroup->update();
    }
    QTreeWidgetItem *item=0;
    for (int i = 0; (item = groupList->topLevelItem(i)) != 0; ++i) {
        GroupLVItem *grp = static_cast<GroupLVItem*>(item);
        foreach (ResourceTableItem *r, grp->resources()) {
            if (r->isChecked() != r->isOrigChecked()) {
                if (!cmd) cmd = new MacroCommand("");
                if (r->isChecked()) {
                    if (!grp->m_request) {
                        grp->m_request = new ResourceGroupRequest(grp->m_group, grp->m_units);
                        cmd->addCommand(new AddResourceGroupRequestCmd(m_task, grp->m_request));
                    }
                    cmd->addCommand(new AddResourceRequestCmd(grp->m_request, new ResourceRequest(r->resource(), r->units())));

                    continue;
                }
                if (grp->m_request && r->request()) {
                    cmd->addCommand(new RemoveResourceRequestCmd(grp->m_request, r->request()));
                    if (grp->isNull()) {
                        cmd->addCommand(new RemoveResourceGroupRequestCmd(m_task, grp->m_request));
                    }
                } else {
                    kError()<<"Remove failed"<<endl;
                }
                continue;
            }
            if (!r->isChecked()) {
                continue;
            }
        }
    }
    return cmd;
}

bool RequestResourcesPanel::ok() {
    if (selectedGroup)
        selectedGroup->update();
    return true;
}

void RequestResourcesPanel::sendChanged() {
    if (!m_blockChanged) emit changed();
}

}  //KPlato namespace

#include "kptrequestresourcespanel.moc"
