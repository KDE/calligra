/* This file is part of the KDE project
   Copyright (C) 2003 Thomas Zander <zander@kde.org>
   Copyright (C) 2004, 2005 Dag Andersen <danders@get2net.dk>

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

#include "kptresourcespanel.h"
#include "kptproject.h"
#include "kptresourcedialog.h"
#include "kptcommand.h"

#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>

#include <QListWidget>
#include <QPushButton>
#include <QList>
#include <QHeaderView>
////////////////////////////   Private classes   //////////////////////////////////

namespace KPlato
{

class GroupItem;
class ResourcesPanelGroupLVItem;
class ResourcesPanelResourceItem;

class ResourcesPanelResourceItem {
public:
    enum State {None, Modified, New};

    ResourcesPanelResourceItem(Resource *res, State state = None) 
        : m_originalResource(0),
          m_state(state) {
        if (state == New) {
            m_resource = res;
        } else {
            m_originalResource = res;
            m_resource = new Resource(res);
        }
        //kDebug()<<"("<<this<<")"<<" orgres="<<m_originalResource<<" newres="<<m_resource;
    }
    ~ResourcesPanelResourceItem() {
        //kDebug()<<"("<<this<<") state="<<m_state;
        delete m_resource;
    }
    void setState(State s) {
        if (m_state == New)
            return; // A new is always new
        m_state = s;
    }
    QString name() { return m_resource->name(); }
    void setName(const QString &newName) { 
        m_resource->setName(newName);
        setState(Modified);
    }
    Resource *takeResource() {
        Resource *r = m_resource;
        m_resource = 0;
        return r;
    }
    MacroCommand *saveResource(ResourceGroup *group);

    Resource *m_originalResource;
    Resource *m_resource; // work on a local copy
    State m_state;
};
MacroCommand *ResourcesPanelResourceItem::saveResource(ResourceGroup *group) {
    MacroCommand *m=0;
    if (m_state == New) {
        //kDebug()<<"Add resource:"<<m_resource->name();
        if (!m) m = new MacroCommand("Add resource");
        m->addCommand(new AddResourceCmd(group, takeResource()));
    } else if (m_state == Modified) {
        //kDebug()<<"Modify resource:"<<m_originalResource->name();
        MacroCommand *cmd = ResourceDialog::buildCommand(m_originalResource, *m_resource);
        if (cmd) {
            if (!m) m = new MacroCommand("Modify resource");
            m->addCommand(cmd);
        }
    }
    return m;
}

class ResourceLBItem : public QListWidgetItem {
public:
    ResourceLBItem(ResourcesPanelResourceItem *item) { 
        m_resourceItem = item; setText(item->name());
    }
    QString name() { return m_resourceItem->name(); }
    void setName(const QString &newName) {
        setText(newName);
        m_resourceItem->setName(newName);
    }

    ResourcesPanelResourceItem *m_resourceItem;
};


//-------------------
class GroupItem {
public:
    enum State {None=0, Modified=1, New=2}; //bitmap

    GroupItem(ResourceGroup *group, State state = None) {
        m_group = group;
        m_name = group->name();
        m_state = state;
        //kDebug()<<"("<<this<<")";
    }
    ~GroupItem() {
        //kDebug()<<"("<<this<<")";
        if (m_state & New) {
            delete m_group;
        }
        while (!m_resourceItems.isEmpty())
            delete m_resourceItems.takeFirst();
        
        while (!m_deletedItems.isEmpty())
            delete m_deletedItems.takeFirst();
    }
    void setState(State s) { m_state |= s; }
    void setName(const QString &newName) {
        m_name = newName;
        if (m_state & New)
            m_group->setName(newName);
        setState(Modified);
        //kDebug()<<"New name: '"<<newName<<"', group name: '"<<m_group->name()<<"' state="<<m_state;
    }
    void addResource(ResourcesPanelResourceItem *item) {
        //kDebug()<<" add:"<<(item?item->name():"")<<" ("<<item<<")";
        m_resourceItems.append(item);
    }
    void deleteResource(ResourcesPanelResourceItem *item) {
        //kDebug()<<" Deleted:"<<item->m_name<<" ("<<item<<")";
        if (item == 0)
            return;
        int i = m_resourceItems.indexOf(item);
        if (i != -1)
            m_resourceItems.removeAt(i);
        if (item->m_state == ResourcesPanelResourceItem::New)
            delete item;
        else
            m_deletedItems.append(item);
        //kDebug()<<"No of items now="<<m_resourceItems.count()<<", no of deleted items="<<m_deletedItems.count();
    }
    ResourceGroup *takeGroup() {
        //kDebug()<<"("<<m_group<<")";
        ResourceGroup *g = m_group;
        m_group = 0;
        return g;
    }
    void saveResources() {
        while (!m_resourceItems.isEmpty()) {
            ResourcesPanelResourceItem *item = m_resourceItems.takeFirst();
            //kDebug()<<item->m_resource->name();
            m_group->addResource(-1, item->takeResource(), 0);
            delete item;
        }
    }
    ResourceGroup *m_group;
    QString m_name;
    QList<ResourcesPanelResourceItem*> m_resourceItems;
    QList<ResourcesPanelResourceItem*> m_deletedItems;
    int m_state;
};

class ResourcesPanelGroupLVItem : public QTreeWidgetItem {
public:
    ResourcesPanelGroupLVItem(ResourcesPanel &pan, QTreeWidget *lv, GroupItem *item)
    : QTreeWidgetItem(lv),
      m_group(item),
      panel(pan) {
       
        setText(0, item->m_name);
        setFlags(flags() | Qt::ItemIsEditable);
        //kDebug()<<"("<<this<<")";
    }
    ~ResourcesPanelGroupLVItem() {
        //kDebug()<<"("<<this<<")";
    }
    void setName(const QString &newName) {
        setText(0, newName);
        m_group->setName(newName);
    }
    void deleteGroup() {
        delete m_group;
        m_group = 0;
    }
    GroupItem *takeGroup() {
        //kDebug()<<"("<<m_group<<")";
        GroupItem *g = m_group;
        m_group = 0;
        return g;
    }
    GroupItem *m_group;
    ResourcesPanel &panel;
    QString oldText;

};

////////////////////   ResourcesPanel   //////////////////////

ResourcesPanel::ResourcesPanel(QWidget *parent, Project *p) : ResourcesPanelBase(parent) {
    project = p;
    m_groupItem = 0;

    bEditResource->setEnabled(false);
    bRemoveResource->setEnabled(false);
    
    listOfGroups->setHeaderLabel(i18n("Group"));
    listOfGroups->header()->setStretchLastSection(true);
    listOfGroups->setSortingEnabled(true);
    listOfGroups->setSelectionMode(QAbstractItemView::SingleSelection);
    
    bAdd->setEnabled(true);
    
    
    foreach (ResourceGroup *grp, project->resourceGroups()) {
        GroupItem *groupItem = new GroupItem(grp);
        //kDebug()<<" Added group:"<<groupItem->m_name<<" ("<<groupItem<<")";
        foreach (Resource *res, grp->resources()) {
            ResourcesPanelResourceItem *ritem = new ResourcesPanelResourceItem(res);
            groupItem->addResource(ritem);
            //kDebug()<<"      Added resource:"<<ritem->m_name<<" ("<<ritem<<")";
        }
        m_groupItems.append(groupItem);
        new ResourcesPanelGroupLVItem(*this, listOfGroups, groupItem);
    }
    if (listOfGroups->topLevelItemCount() > 0)
        listOfGroups->topLevelItem(0)->setSelected(true);
    
    slotGroupSelectionChanged();

    connect(bAdd, SIGNAL(clicked()), SLOT(slotAddGroup()));
    connect(bRemove, SIGNAL(clicked()), SLOT(slotDeleteGroup()));
    connect(listOfGroups, SIGNAL(itemSelectionChanged()), SLOT(slotGroupSelectionChanged()));

    connect(bAddResource, SIGNAL( clicked() ), this, SLOT ( slotAddResource() ));
    connect(bEditResource, SIGNAL( clicked() ), this, SLOT ( slotEditResource() ));
    connect(bRemoveResource, SIGNAL( clicked() ), this, SLOT ( slotDeleteResource() ));
    connect(listOfResources, SIGNAL(itemSelectionChanged()), SLOT(slotResourceChanged()));
    connect(listOfResources, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), SLOT(slotCurrentChanged(QListWidgetItem*, QListWidgetItem*)));
    
    connect(listOfGroups, SIGNAL(itemChanged(QTreeWidgetItem*, int)), SLOT(slotGroupChanged(QTreeWidgetItem*, int)));

}

ResourcesPanel::~ResourcesPanel()
{
    while (!m_groupItems.isEmpty())
        delete m_groupItems.takeFirst();
    while (!m_deletedGroupItems.isEmpty())
        delete m_deletedGroupItems.takeFirst();
}

void ResourcesPanel::slotAddGroup() {
    //kDebug();
    listOfGroups->clearSelection();
    ResourceGroup *r = new ResourceGroup();
    GroupItem *gitem = new GroupItem(r, GroupItem::New);
    m_groupItems.append(gitem);
    ResourcesPanelGroupLVItem *groupItem = new ResourcesPanelGroupLVItem(*this, listOfGroups, gitem);
    groupItem->setSelected(true);
    listOfGroups->editItem(groupItem);
}

void ResourcesPanel::slotDeleteGroup() {
    //kDebug();
    QList<QTreeWidgetItem*> lst = listOfGroups->selectedItems();
    foreach (QTreeWidgetItem *i, lst) {
        listOfResources->clear();
        listOfGroups->takeTopLevelItem(listOfGroups->indexOfTopLevelItem(i));
        ResourcesPanelGroupLVItem *groupLVItem = static_cast<ResourcesPanelGroupLVItem*>(i);
        m_groupItems.takeAt(m_groupItems.indexOf(groupLVItem->m_group)); // remove GroupItem from active list
        m_deletedGroupItems.append(groupLVItem->takeGroup()); // remove GroupItem from GroupLVItem and add to deleted list
    
        //kDebug()<<" No of deleted groups="<<m_deletedGroupItems.count()<<", now"<<m_groupItems.count()<<" groups left";
    
        delete groupLVItem; // delete GroupLVItem (but not GroupItem)
    }
    emit changed();
}

void ResourcesPanel::slotAddResource() {
    if (!m_groupItem) {
        KMessageBox::sorry(this, i18n("Resources belong to resource groups, select the group first to add a new resource to"));
        return;
    }
    listOfResources->clearSelection();
    Resource *r = new Resource();
    ResourceDialog *dia = new ResourceDialog(*project, r);
    if (dia->exec()) {
        MacroCommand *cmd = dia->buildCommand();
        if (cmd) {
            cmd->execute(); // modifications -> r
            delete cmd;
        }
        ResourcesPanelResourceItem *resourceItem = new ResourcesPanelResourceItem(r, ResourcesPanelResourceItem::New);
        m_groupItem->m_group->addResource(resourceItem);
        ResourceLBItem *item = new ResourceLBItem(resourceItem);
        listOfResources->addItem(item);
        item->setSelected(true);
        emit changed();
        //kDebug()<<" Added:"<<resourceItem->name()<<" to"<<m_groupItem->m_group->m_name;
    } else {
        delete r;
    }
    delete dia;
}

void ResourcesPanel::slotEditResource() {
    //kDebug();
    QList<QListWidgetItem*> lst = listOfResources->selectedItems();
    if (lst.isEmpty()) {
        return;
    }
    ResourceLBItem *item = static_cast<ResourceLBItem*>(lst[0]);
    Resource *r = item->m_resourceItem->m_resource;
    ResourceDialog *dia = new ResourceDialog(*project, r);
    if (dia->exec()) {
        MacroCommand *cmd = dia->buildCommand();
        if (cmd) {
            cmd->execute(); // modifications -> r
            delete cmd;
        }
        item->m_resourceItem->setState(ResourcesPanelResourceItem::Modified);
        item->setName(r->name()); // refresh list
        emit changed();
    }
    delete dia;
}

void ResourcesPanel::slotDeleteResource() {
    //kDebug();
    QList<QListWidgetItem*> lst = listOfResources->selectedItems();
    if (lst.isEmpty()) {
        return;
    }
    ResourceLBItem *item = static_cast<ResourceLBItem*>(lst[0]);
    //Can't delete resource from unselected group
    if(m_groupItem == 0) return;

    m_groupItem->m_group->deleteResource(item->m_resourceItem);
    listOfResources->takeItem(listOfResources->row(lst[0]));

    emit changed();
}

/* Select another resource */
void ResourcesPanel::slotResourceChanged() {
    QList<QListWidgetItem*> lst = listOfResources->selectedItems();
    if (listOfResources->selectedItems().count() == 0) {
        bEditResource->setEnabled(false);
        bRemoveResource->setEnabled(false);
        return;
    }
    bEditResource->setEnabled(true);
    bRemoveResource->setEnabled(true);
}

/* Select another resource */
void ResourcesPanel::slotCurrentChanged(QListWidgetItem *curr, QListWidgetItem* ) {
    if (curr && !curr->isSelected()) {
        curr->setSelected(true);
    }
}

bool ResourcesPanel::ok() {
    return true;
}

MacroCommand *ResourcesPanel::buildCommand() {
    MacroCommand *m=0;

    QString cmdName = "Modify resourcegroups";
    foreach (GroupItem *gitem, m_deletedGroupItems) {
        if (!(gitem->m_state & GroupItem::New)) {
            if (!m) m = new MacroCommand(cmdName);
            //kDebug()<<"Remove group: '"<<gitem->m_name<<"'";
            m->addCommand(new RemoveResourceGroupCmd(project, gitem->takeGroup()));
        }
    }
    foreach (GroupItem *gitem, m_groupItems) {
        //kDebug()<<"Group:"<<gitem->m_name<<" has"<<gitem->m_resourceItems.count()<<" resources"<<" and"<<gitem->m_deletedItems.count()<<" deleted resources";
        //First remove deleted resources from group
        foreach (ResourcesPanelResourceItem *ditem, gitem->m_deletedItems) {
            if (!m) m = new MacroCommand(cmdName);
            //kDebug()<<" Deleting resource: '"<<ditem->m_originalResource->name()<<"'";
            m->addCommand(new RemoveResourceCmd(gitem->m_group, ditem->m_originalResource));
        }
        // Now add/modify group/resources
        if (gitem->m_state & GroupItem::New) {
            if (!m) m = new MacroCommand(cmdName);
            //kDebug()<<" Adding group: '"<<gitem->m_name<<"'";
            gitem->saveResources();
            m->addCommand(new AddResourceGroupCmd(project, gitem->takeGroup()));
            continue;
        }
        ResourceGroup *rg = gitem->takeGroup();
        if (gitem->m_state & GroupItem::Modified) {
            if (gitem->m_name != rg->name()) {
                if (!m) m = new MacroCommand(cmdName);
                //kDebug()<<" Modifying group: '"<<gitem->m_name<<"'";
                m->addCommand(new ModifyResourceGroupNameCmd(rg, gitem->m_name));
            }
        }
        foreach (ResourcesPanelResourceItem *item, gitem->m_resourceItems) {
            MacroCommand *cmd = item->saveResource(rg);
            if (cmd) {
                if (!m) m = new MacroCommand(cmdName);
                m->addCommand(cmd);
            }
        }
    }
    return m;
}

void ResourcesPanel::slotGroupChanged(QTreeWidgetItem *ci, int) {
    if (ci == 0)
        return;
    //kDebug()<<ci->text(0)<<","<<col;
    ResourcesPanelGroupLVItem *item = static_cast<ResourcesPanelGroupLVItem*>(ci);
    item->setName(item->text(0));
    emit changed();
}

void ResourcesPanel::slotGroupSelectionChanged() {
    QList<QTreeWidgetItem*> lst = listOfGroups->selectedItems();
    QTreeWidgetItem *i = 0;
    if (lst.count() > 0)
        i = lst[0]; // handle only single selection, really
    slotGroupSelectionChanged(i);
}

void ResourcesPanel::slotGroupSelectionChanged(QTreeWidgetItem *itm) {
    ResourcesPanelGroupLVItem *item = static_cast<ResourcesPanelGroupLVItem*>(itm);
    if (!item) {
        bAdd->setEnabled(true);
        bRemove->setEnabled(false);
        listOfResources->clear();
        resourceGroupBox->setEnabled(false);
        return;
    }
    m_groupItem = item;
    listOfResources->clear();

    foreach (ResourcesPanelResourceItem *i, m_groupItem->m_group->m_resourceItems) {
        listOfResources->addItem(new ResourceLBItem(i));
        //kDebug()<<"Insert resource item:"<<i->name();
    }
    bAdd->setEnabled(true);
    bRemove->setEnabled(true);
    slotResourceChanged();
    resourceGroupBox->setEnabled(true);
}

}  //KPlato namespace

#include "kptresourcespanel.moc"
