/* This file is part of the KDE project
   Copyright (C) 2003 Dag Andersen <danders@get2net.dk>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kptrequestresourcespanel.h"
#include "kpttask.h"
#include "kptproject.h"
#include "kptresource.h"
#include "kptdurationwidget.h"
#include "kptcalendar.h"

#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kcommand.h>

#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qstring.h>
#include <qspinbox.h>
#include <qvalidator.h>
#include <qcombobox.h>
#include <qdatetime.h>
#include <qdatetimeedit.h>


KPTResourceTableItem::KPTResourceTableItem(KPTResource *resource, bool check) {
    m_resource = resource;
    m_checked = check;
    m_checkitem = 0;
    m_units = 100;
    //kdDebug()<<k_funcinfo<<"Added: '"<<resource->name()<<"' checked="<<m_checked<<endl;
}

KPTResourceTableItem::~KPTResourceTableItem() {
    //kdDebug()<<k_funcinfo<<endl;
}

void KPTResourceTableItem::clear() {
    if (m_checkitem)
        m_checked = m_checkitem->isChecked();

    m_checkitem = 0;
}
void KPTResourceTableItem::ok(KPTResourceGroupRequest *group) {
    // assume old request is cleared, so we just create new one
    if (m_checked) {
        group->addResourceRequest(new KPTResourceRequest(m_resource, m_units)); 
        //kdDebug()<<k_funcinfo<<"Resource request to "<<m_resource->name()<<" added"<<endl;
    }
}


void KPTResourceTableItem::insert(QTable *table, int row) {
    //kdDebug()<<k_funcinfo<<endl;
    m_checkitem = new QCheckTableItem(table, m_resource->name());
    m_checkitem->setChecked(m_checked);
    table->setItem(row, 0, m_checkitem);
    //kdDebug()<<k_funcinfo<<"Added: '"<<m_resource->name()<<"' checked="<<m_checked<<endl;
}

KPTGroupLVItem::KPTGroupLVItem(QListView *parent, KPTResourceGroup *group, KPTTask &task)
    : QListViewItem(parent, group->name(), QString("%1").arg(group->units())),
      m_group(group),
      m_units(0)
{

    m_request = task.resourceGroupRequest(group);
    if (m_request) {
        m_units = m_request->units();
    }
    bool check = false;
    QPtrListIterator<KPTResource> it(group->resources());
    for (; it.current(); ++it) {
        m_request  ? check =  m_request->find(it.current()) : check = false;
        m_resources.append(new KPTResourceTableItem(it.current(), check));
    }
}

KPTGroupLVItem::~KPTGroupLVItem() {
    //kdDebug()<<k_funcinfo<<m_group->name()<<endl;
    m_resources.clear();
}

void KPTGroupLVItem::clear() {
    QPtrListIterator<KPTResourceTableItem> it(m_resources);
    for (; it.current(); ++it) {
        it.current()->clear();
    }
}

void KPTGroupLVItem::insert(QTable *table) {

    // clear the table, must be a better way!
    for (int i = table->numRows(); i > 0; --i)
        table->removeRow(i-1);

    if (m_group->numResources() == 0) {
        table->setNumRows(1);
        table->setItem(0, 0, new QCheckTableItem(table,i18n("None")));
    } else {
        table->setNumRows(m_group->numResources());
        QPtrListIterator<KPTResourceTableItem> it(m_resources);
        for (int i = 0; it.current(); ++it, ++i) {
            it.current()->insert(table, i);
        }
    }
    table->adjustColumn(0);
}

void KPTGroupLVItem::ok(KPTTask &task) {
    //kdDebug()<<k_funcinfo<<"numRequests="<<numRequests()<<endl;
    // assume old requests are cleared, so we just create new ones
    if (numRequests() > 0) {
        KPTResourceGroupRequest *g = new KPTResourceGroupRequest(m_group, m_units);
        QPtrListIterator<KPTResourceTableItem> it(m_resources);
        for (; it.current(); ++it) {
            it.current()->ok(g);
        }
        task.addRequest(g);
    }
}

int KPTGroupLVItem::numRequests() {
    //kdDebug()<<k_funcinfo<<endl;
    int value = m_units;
    QPtrListIterator<KPTResourceTableItem> it(m_resources);
    for (; it.current(); ++it) {
        value += it.current()->numRequests();
    }
    return value;
}

KPTRequestResourcesPanel::KPTRequestResourcesPanel(QWidget *parent, KPTTask &task)
    : KPTTaskResourcesPanelBase(parent),
      m_task(task),
      selectedGroup(0),
      m_blockChanged(false) {

    KPTProject *p = dynamic_cast<KPTProject*>(task.projectNode());
    if (p) {
        QPtrListIterator<KPTResourceGroup> git(p->resourceGroups());
        for(int i=0; git.current(); ++git, ++i) {
            KPTResourceGroup *grp = git.current();
            KPTGroupLVItem *grpitem = new KPTGroupLVItem(groupList, grp, task);
            groupList->insertItem(grpitem);
            //kdDebug()<<k_funcinfo<<" Added group: "<<grp->name()<<endl;
        }
    }
    QListViewItem *item = groupList->firstChild();
    if (item) {
        groupList->setSelected(item, true);
        groupChanged(item);
    }

    if (p) {
        KPTStandardWorktime *wt = p->standardWorktime();
        if (wt) {
            //FIXME handle decimals
            effort->setFieldRightscale(0, wt->durationDay().hours()); 
            effort->setFieldLeftscale(1, wt->durationDay().hours());
        }
    }
    effort->setValue(task.effort()->expected());

    //TODO: calc optimistic/pessimistic
    optimisticValue->setValue(task.effort()->optimisticRatio());
    pessimisticValue->setValue(task.effort()->pessimisticRatio());

    effortType->setCurrentItem(task.effort()->type());

    connect(groupList, SIGNAL(selectionChanged(QListViewItem*)),  SLOT(groupChanged(QListViewItem*)));
    connect(resourceTable, SIGNAL(valueChanged(int, int)), SLOT(resourceChanged(int, int)));
//    connect(numUnits, SIGNAL(valueChanged(int)), SLOT(unitsChanged(int)));


    connect(effort, SIGNAL(valueChanged()), SLOT(sendChanged()));
    connect(effortType, SIGNAL(activated(int)), SLOT(sendChanged()));
    connect(optimisticValue, SIGNAL(valueChanged(int)), SLOT(sendChanged()));
    connect(pessimisticValue, SIGNAL(valueChanged(int)), SLOT(sendChanged()));
    connect(risk, SIGNAL(activated(int)), SLOT(sendChanged()));
    
}

void KPTRequestResourcesPanel::groupChanged(QListViewItem *item) {
    //kdDebug()<<k_funcinfo<<endl;
    KPTGroupLVItem *grp = dynamic_cast<KPTGroupLVItem *>(item);
    if (grp == 0)
        return;

    if (selectedGroup) {
        selectedGroup->clear();
    }
    selectedGroup = grp;

/*    m_blockChanged = true;
    numUnits->setMaxValue(grp->m_group->units());
    numUnits->setValue(grp->m_units);
    m_blockChanged = false;*/
    grp->insert(resourceTable);
}

void KPTRequestResourcesPanel::resourceChanged(int, int) {
    //kdDebug()<<k_funcinfo<<endl;
    sendChanged();
}

void KPTRequestResourcesPanel::unitsChanged(int units) {
    //kdDebug()<<k_funcinfo<<endl;
    if (selectedGroup) {
        selectedGroup->m_units = units;
        sendChanged();
    }
}

//FIXME
KMacroCommand *KPTRequestResourcesPanel::buildCommand() {
    slotOk();
    return 0;
}

void KPTRequestResourcesPanel::slotOk() {
    if (selectedGroup)
        selectedGroup->clear();

    m_task.clearResourceRequests();

    QListViewItemIterator it(groupList);
    for (; it.current(); ++it) {
        KPTGroupLVItem *item = dynamic_cast<KPTGroupLVItem *>(it.current());
        if (item)
            item->ok(m_task);
    }

    m_task.effort()->set(effort->value());

    switch(effortType->currentItem()) {
        case 0: // work based
            m_task.effort()->setType(KPTEffort::Type_WorkBased);
            break;
        case 1: // fixed duration
            m_task.effort()->setType(KPTEffort::Type_FixedDuration);
            break;
        default:
            break;
    }
}

void KPTRequestResourcesPanel::sendChanged() {
    if (!m_blockChanged) emit changed();
}

#include "kptrequestresourcespanel.moc"
