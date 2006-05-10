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

#include "kptrequestresourcespanel.h"
#include "kpttask.h"
#include "kptproject.h"
#include "kptresource.h"
#include "kptcalendar.h"

#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kptcommand.h>

#include <q3listbox.h>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QSpinBox>
#include <qvalidator.h>
#include <QComboBox>
#include <qdatetime.h>
#include <q3datetimeedit.h>

namespace KPlato
{

ResourceTableItem::ResourceTableItem(Resource *resource, ResourceRequest *request, bool check) {
    m_resource = resource;
    m_request = request;
    m_checked = check;
    m_origChecked = check;
    m_checkitem = 0;
    m_units = 100;
    m_origUnits = 100;
    //kDebug()<<k_funcinfo<<"Added: '"<<resource->name()<<"' checked="<<m_checked<<endl;
}

ResourceTableItem::~ResourceTableItem() {
    //kDebug()<<k_funcinfo<<m_resource->name()<<endl;
}

void ResourceTableItem::update() {
    if (m_checkitem)
        m_checked = m_checkitem->isChecked();
    //kDebug()<<k_funcinfo<<m_resource->name()<<" checked="<<m_checked<<endl;
}

void ResourceTableItem::insert(Q3Table *table, int row) {
    //kDebug()<<k_funcinfo<<endl;
    m_checkitem = new Q3CheckTableItem(table, m_resource->name());
    m_checkitem->setChecked(m_checked);
    table->setItem(row, 0, m_checkitem);

    //kDebug()<<k_funcinfo<<"Added: '"<<m_resource->name()<<"' checked="<<m_checked<<endl;
}

GroupLVItem::GroupLVItem(Q3ListView *parent, ResourceGroup *group, Task &task)
    : Q3ListViewItem(parent, group->name(), QString("%1").arg(group->units())),
      m_group(group),
      m_units(0)
{

    m_request = task.resourceGroupRequest(group);
    if (m_request) {
        m_units = m_request->units();
    }
    Q3PtrListIterator<Resource> it(group->resources());
    for (; it.current(); ++it) {
        //kDebug()<<k_funcinfo<<"resource="<<it.current()->name()<<endl;
        ResourceRequest *req=0;
        if (m_request) {
            req = m_request->find(it.current());
        }
        m_resources.append(new ResourceTableItem(it.current(), req, (bool)req));
    }

    m_resources.setAutoDelete(true);
}

GroupLVItem::~GroupLVItem() {
    //kDebug()<<k_funcinfo<<m_group->name()<<endl;
}

void GroupLVItem::update() {
    Q3PtrListIterator<ResourceTableItem> it(m_resources);
    for (; it.current(); ++it) {
        it.current()->update();
    }
}

void GroupLVItem::insert(Q3Table *table) {

    // clear the table, must be a better way!
    for (int i = table->numRows(); i > 0; --i)
        table->removeRow(i-1);

    if (m_group->numResources() == 0) {
        table->setNumRows(1);
        table->setItem(0, 0, new Q3CheckTableItem(table,i18n("None")));
        table->setItem(0, 1, new Q3ComboTableItem(table,QStringList(i18n("None"))));
    } else {
        table->setNumRows(m_group->numResources());
        Q3PtrListIterator<ResourceTableItem> it(m_resources);
        for (int i = 0; it.current(); ++it, ++i) {
            it.current()->insert(table, i);
        }
    }
    table->adjustColumn(0);
}

int GroupLVItem::numRequests() {
    //kDebug()<<k_funcinfo<<endl;
    int value = m_units;
    Q3PtrListIterator<ResourceTableItem> it(m_resources);
    for (; it.current(); ++it) {
        value += it.current()->numRequests();
    }
    return value;
}

bool GroupLVItem::isNull() const {
    //kDebug()<<k_funcinfo<<endl;
    Q3PtrListIterator<ResourceTableItem> it(m_resources);
    for (; it.current(); ++it) {
        if (it.current()->isChecked())
            return false;
    }
    if (m_units > 0)
        return false;
    return true;
}

RequestResourcesPanel::RequestResourcesPanel(QWidget *parent, Task &task, bool baseline)
    : TaskResourcesPanelBase(parent),
      m_task(task),
      m_worktime(0),
      selectedGroup(0),
      m_blockChanged(false) {

    Project *p = dynamic_cast<Project*>(task.projectNode());
    if (p) {
        m_worktime = p->standardWorktime();

        Q3PtrListIterator<ResourceGroup> git(p->resourceGroups());
        for(int i=0; git.current(); ++git, ++i) {
            ResourceGroup *grp = git.current();
            GroupLVItem *grpitem = new GroupLVItem(groupList, grp, task);
            groupList->insertItem(grpitem);
            //kDebug()<<k_funcinfo<<" Added group: "<<grp->name()<<endl;
        }
    }
    Q3ListViewItem *item = groupList->firstChild();
    if (item) {
        groupList->setSelected(item, true);
        groupChanged(item);
    }

    resourceTable->setReadOnly(baseline);

    connect(groupList, SIGNAL(selectionChanged(Q3ListViewItem*)),  SLOT(groupChanged(Q3ListViewItem*)));
    connect(resourceTable, SIGNAL(valueChanged(int, int)), SLOT(resourceChanged(int, int)));
//    connect(numUnits, SIGNAL(valueChanged(int)), SLOT(unitsChanged(int)));

}

void RequestResourcesPanel::groupChanged(Q3ListViewItem *item) {
    //kDebug()<<k_funcinfo<<endl;
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
    //kDebug()<<k_funcinfo<<"("<<r<<","<<c<<")"<<endl;
    sendChanged();
}

void RequestResourcesPanel::unitsChanged(int units) {
    //kDebug()<<k_funcinfo<<endl;
    if (selectedGroup) {
        selectedGroup->m_units = units;
        sendChanged();
    }
}

KCommand *RequestResourcesPanel::buildCommand(Part *part) {
    //kDebug()<<k_funcinfo<<endl;
    KMacroCommand *cmd = 0;
    if (selectedGroup) {
        selectedGroup->update();
    }
    Q3ListViewItem *item = groupList->firstChild();
    for (; item; item = item->nextSibling()) {
        GroupLVItem *grp = static_cast<GroupLVItem*>(item);
        Q3PtrListIterator<ResourceTableItem> it = grp->resources();
        for (; it.current(); ++it) {
            if (it.current()->isChecked() != it.current()->isOrigChecked()) {
                if (!cmd) cmd = new KMacroCommand("");
                if (it.current()->isChecked()) {
                    if (!grp->m_request) {
                        grp->m_request = new ResourceGroupRequest(grp->m_group, grp->m_units);
                        cmd->addCommand(new AddResourceGroupRequestCmd(part, m_task, grp->m_request));
                    }
                    cmd->addCommand(new AddResourceRequestCmd(part, grp->m_request, new ResourceRequest(it.current()->resource(), it.current()->units())));

                    continue;
                }
                if (grp->m_request && it.current()->request()) {
                    cmd->addCommand(new RemoveResourceRequestCmd(part, grp->m_request, it.current()->request()));
                    if (grp->isNull()) {
                        cmd->addCommand(new RemoveResourceGroupRequestCmd(part, m_task, grp->m_request));
                    }
                } else {
                    kError()<<k_funcinfo<<"Remove failed"<<endl;
                }
                continue;
            }
            if (!it.current()->isChecked()) {
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
