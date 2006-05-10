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
#include <k3listview.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>

#include <q3groupbox.h>
#include <q3header.h>
#include <q3listbox.h>
#include <QLineEdit>
#include <q3listview.h>
#include <QPushButton>
//Added by qt3to4:
#include <Q3PtrList>

////////////////////////////   Private classes   //////////////////////////////////

namespace KPlato
{

class GroupItem;
class ResourcesPanelGroupLVItem;
class ResourcesPanelResourceItem;
class Part;

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
        //kDebug()<<k_funcinfo<<"("<<this<<")"<<" orgres="<<m_originalResource<<" newres="<<m_resource<<endl;
    }
    ~ResourcesPanelResourceItem() {
        //kDebug()<<k_funcinfo<<"("<<this<<") state="<<m_state<<endl;
        delete m_resource;
    }
    void setState(State s) {
        if (m_state == New)
            return; // A new is allways new
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
    KCommand *saveResource(Part *part, ResourceGroup *group);

    Resource *m_originalResource;
    Resource *m_resource; // work on a local copy
    State m_state;
};
KCommand *ResourcesPanelResourceItem::saveResource(Part *part, ResourceGroup *group) {
    KMacroCommand *m=0;
    if (m_state == New) {
        //kDebug()<<k_funcinfo<<"Add resource: "<<m_resource->name()<<endl;
        if (!m) m = new KMacroCommand("Add resource");
        m->addCommand(new AddResourceCmd(part, group, takeResource()));
    } else if (m_state == Modified) {
        //kDebug()<<k_funcinfo<<"Modify resource: "<<m_originalResource->name()<<endl;
        KCommand *cmd = ResourceDialog::buildCommand(m_originalResource, *m_resource, part);
        if (cmd) {
            if (!m) m = new KMacroCommand("Modify resource");
            m->addCommand(cmd);
        }
    }
    return m;
}

class ResourceLBItem : public Q3ListBoxText {
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
        m_resourceItems.setAutoDelete(true);
        m_deletedItems.setAutoDelete(true);
        //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    }
    ~GroupItem() {
        //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
        if (m_state & New) {
            delete m_group;
        }
    }
    void setState(State s) { m_state |= s; }
    void setName(const QString &newName) {
        m_name = newName;
        if (m_state & New)
            m_group->setName(newName);
        setState(Modified);
        //kDebug()<<k_funcinfo<<"New name: '"<<newName<<"', group name: '"<<m_group->name()<<"' state="<<m_state<<endl;
    }
    void addResource(ResourcesPanelResourceItem *item) {
        //kDebug()<<k_funcinfo<<" add: "<<(item?item->name():"")<<" ("<<item<<")"<<endl;
        m_resourceItems.append(item);
    }
    void deleteResource(ResourcesPanelResourceItem *item) {
        //kDebug()<<k_funcinfo<<" Deleted: "<<item->m_name<<" ("<<item<<")"<<endl;
        m_resourceItems.take(m_resourceItems.findRef(item));
        if (item->m_state == ResourcesPanelResourceItem::New)
            delete item;
        else
            m_deletedItems.append(item);
        //kDebug()<<k_funcinfo<<"No of items now="<<m_resourceItems.count()<<", no of deleted items="<<m_deletedItems.count()<<endl;
    }
    ResourceGroup *takeGroup() {
        //kDebug()<<k_funcinfo<<"("<<m_group<<")"<<endl;
        ResourceGroup *g = m_group;
        m_group = 0;
        return g;
    }
    void saveResources() {
        ResourcesPanelResourceItem *item = m_resourceItems.first();
        while ((item = m_resourceItems.take())) {
            //kDebug()<<k_funcinfo<<item->m_resource->name()<<endl;
            m_group->addResource(item->takeResource(), 0);
            delete item;
        }
    }
    ResourceGroup *m_group;
    QString m_name;
    Q3PtrList<ResourcesPanelResourceItem> m_resourceItems;
    Q3PtrList<ResourcesPanelResourceItem> m_deletedItems;
    int m_state;
};

class ResourcesPanelGroupLVItem : public K3ListViewItem {
public:
    ResourcesPanelGroupLVItem(ResourcesPanel &pan, K3ListView *lv, GroupItem *item)
    :  K3ListViewItem(lv, item->m_name),
       m_group(item),
       panel(pan) {
        
        setRenameEnabled(0, false);
        //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    }
    ~ResourcesPanelGroupLVItem() {
        //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
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
        //kDebug()<<k_funcinfo<<"("<<m_group<<")"<<endl;
        GroupItem *g = m_group;
        m_group = 0;
        return g;
    }
    GroupItem *m_group;
    ResourcesPanel &panel;
    QString oldText;

protected:
    virtual void cancelRename(int col) {
        //kDebug()<<k_funcinfo<<endl;
        if (col == 0 && oldText.isEmpty()){
            return;
        }
        panel.renameStopped(this);
        K3ListViewItem::cancelRename(col);
        setRenameEnabled(col, false);
    }
};

////////////////////   ResourcesPanel   //////////////////////

ResourcesPanel::ResourcesPanel(QWidget *parent, Project *p) : ResourcesPanelBase(parent) {
    project = p;
    m_groupItem = 0;
    m_blockResourceRename = false;
    m_renameItem = 0;

    bEditResource->setEnabled(false);
    bRemoveResource->setEnabled(false);
    resourceName->setEnabled(false);
    
    listOfGroups->header()->setStretchEnabled(true, 0);
    listOfGroups->setSorting(0);
    listOfGroups->setShowSortIndicator(true);
    listOfGroups->setDefaultRenameAction (Q3ListView::Accept);
    bAdd->setEnabled(true);
    
    m_groupItems.setAutoDelete(true);
    m_deletedGroupItems.setAutoDelete(true);
    
    Q3PtrListIterator<ResourceGroup> git(project->resourceGroups());
    for(; git.current(); ++git) {
        ResourceGroup *grp = git.current();
        GroupItem *groupItem = new GroupItem(grp);
        //kDebug()<<k_funcinfo<<" Added group: "<<groupItem->m_name<<" ("<<groupItem<<")"<<endl;
        Q3PtrListIterator<Resource> rit(grp->resources());
        for(; rit.current(); ++rit) {
            Resource *res = rit.current();
            ResourcesPanelResourceItem *ritem = new ResourcesPanelResourceItem(res);
            groupItem->addResource(ritem);
            //kDebug()<<k_funcinfo<<"      Added resource: "<<ritem->m_name<<" ("<<ritem<<")"<<endl;
        }
        m_groupItems.append(groupItem);
        new ResourcesPanelGroupLVItem(*this, listOfGroups, groupItem);
    }
    listOfGroups->setSelected(listOfGroups->firstChild(), true);
    slotGroupChanged();

    connect(bAdd, SIGNAL(clicked()), SLOT(slotAddGroup()));
    connect(bRemove, SIGNAL(clicked()), SLOT(slotDeleteGroup()));
    connect(listOfGroups, SIGNAL(selectionChanged()), SLOT(slotGroupChanged()));
    connect(listOfGroups, SIGNAL(doubleClicked(Q3ListViewItem*, const QPoint&, int)), SLOT(slotListDoubleClicked(Q3ListViewItem*, const QPoint&, int)));
    connect(listOfGroups, SIGNAL(itemRenamed(Q3ListViewItem*, int)), SLOT(slotItemRenamed(Q3ListViewItem*, int)));

    connect(bAddResource, SIGNAL( clicked() ), this, SLOT ( slotAddResource() ));
    connect(bEditResource, SIGNAL( clicked() ), this, SLOT ( slotEditResource() ));
    connect(bRemoveResource, SIGNAL( clicked() ), this, SLOT ( slotDeleteResource() ));
    connect(listOfResources, SIGNAL(selectionChanged(Q3ListBoxItem*)), SLOT(slotResourceChanged(Q3ListBoxItem*)));
    connect(listOfResources, SIGNAL(currentChanged(Q3ListBoxItem*)), SLOT(slotCurrentChanged(Q3ListBoxItem*)));
    connect(resourceName, SIGNAL(textChanged(const QString&)), SLOT(slotResourceRename(const QString&)));


    // Internal hacks, to get renaming to behave 
    // Uses signals to not get in the way of QListView
    connect(this, SIGNAL(renameStarted(Q3ListViewItem*, int)), SLOT(slotRenameStarted(Q3ListViewItem*, int)));
    connect(this, SIGNAL(startRename(Q3ListViewItem*, int)), SLOT(slotStartRename(Q3ListViewItem*, int)));
    connect(this, SIGNAL(selectionChanged()), SLOT(slotGroupChanged()));
}

void ResourcesPanel::slotAddGroup() {
    //kDebug()<<k_funcinfo<<endl;
    ResourceGroup *r = new ResourceGroup(project);
    GroupItem *gitem = new GroupItem(r, GroupItem::New);
    m_groupItems.append(gitem);
    ResourcesPanelGroupLVItem *groupItem = new ResourcesPanelGroupLVItem(*this, listOfGroups, gitem);

    slotListDoubleClicked(groupItem, QPoint(), 0);
}

void ResourcesPanel::slotDeleteGroup() {
    //kDebug()<<k_funcinfo<<endl;
    ResourcesPanelGroupLVItem *groupLVItem = dynamic_cast<ResourcesPanelGroupLVItem*> (listOfGroups->selectedItem());
    if (groupLVItem == 0)
        return;

    listOfResources->clear();
    
    listOfGroups->takeItem(groupLVItem); // remove from listbox
    m_groupItems.take(m_groupItems.findRef(groupLVItem->m_group)); // remove GroupItem from active list
    m_deletedGroupItems.append(groupLVItem->takeGroup()); // remove GroupItem from GroupLVItem and add to deleted list

    //kDebug()<<k_funcinfo<<" No of deleted groups="<<m_deletedGroupItems.count()<<", now "<<m_groupItems.count()<<" groups left"<<endl;

    delete groupLVItem; // delete GroupLVItem (but not GroupItem)
    emit changed();
}

void ResourcesPanel::slotAddResource() {
    if (!m_groupItem) {
        KMessageBox::sorry(this, i18n("Resources belong to resource groups, select the group first to add a new resource to"));
        return;
    }
    listOfResources->setSelected(listOfResources->selectedItem(), false);
    Resource *r = new Resource(project);
    ResourceDialog *dia = new ResourceDialog(*project, r);
    if (dia->exec()) {
        KCommand *cmd = dia->buildCommand();
        if (cmd) {
            cmd->execute(); // modifications -> r
            delete cmd;
        }
        ResourcesPanelResourceItem *resourceItem = new ResourcesPanelResourceItem(r, ResourcesPanelResourceItem::New);
        m_groupItem->m_group->addResource(resourceItem);
        ResourceLBItem *item = new ResourceLBItem(resourceItem);
        listOfResources->insertItem(item);
        resourceName->clear();
        listOfResources->setSelected(item, true);
        emit changed();
        //kDebug()<<k_funcinfo<<" Added: "<<resourceItem->name()<<" to "<<m_groupItem->m_group->m_name<<endl;
    } else {
        delete r;
    }
    delete dia;
}

void ResourcesPanel::slotEditResource() {
    //kDebug()<<k_funcinfo<<endl;
    ResourceLBItem *item = dynamic_cast<ResourceLBItem*> (listOfResources->selectedItem());
    if(item == 0) return;
    Resource *r = item->m_resourceItem->m_resource;
    ResourceDialog *dia = new ResourceDialog(*project, r);
    if (dia->exec()) {
        KCommand *cmd = dia->buildCommand();
        if (cmd) {
            cmd->execute(); // modifications -> r
            delete cmd;
        }
        resourceName->setText(r->name());
        item->m_resourceItem->setState(ResourcesPanelResourceItem::Modified);
        item->setName(r->name()); // refresh list
        listOfResources->triggerUpdate(false);
        emit changed();
    }
    delete dia;
}

void ResourcesPanel::slotDeleteResource() {
    //kDebug()<<k_funcinfo<<endl;
    ResourceLBItem *item = dynamic_cast<ResourceLBItem*> (listOfResources->selectedItem());
    if(item == 0) return;

    //Can't delete resource from unselected group
    if(m_groupItem == 0) return;

    m_groupItem->m_group->deleteResource(item->m_resourceItem);
    listOfResources->removeItem(listOfResources->currentItem());

    emit changed();
}

/* Select another resource */
void ResourcesPanel::slotResourceChanged( Q3ListBoxItem *item) {
    if (!item) {
        resourceName->setEnabled(false);
        bEditResource->setEnabled(false);
        bRemoveResource->setEnabled(false);
        return;
    }
    resourceName->setText( ((ResourceLBItem *)item)->name());
    resourceName->setEnabled(true);
    bEditResource->setEnabled(true);
    bRemoveResource->setEnabled(true);
}

/* Select another resource */
void ResourcesPanel::slotCurrentChanged( Q3ListBoxItem *item) {
    if (item && !item->isSelected()) {
        listOfResources->setSelected(item, true);
    }
}

void ResourcesPanel::slotResourceRename( const QString &newName) {
    Q3ListBoxItem *item = listOfResources->selectedItem();
    if(!item || m_blockResourceRename) return;

    ResourceLBItem *i = dynamic_cast<ResourceLBItem *>(item);
    if (i->name() == newName) return;

    i->setName(newName);
    listOfResources->triggerUpdate(false);

    emit changed();
}

bool ResourcesPanel::ok() {
    return true;
}

KCommand *ResourcesPanel::buildCommand(Part *part) {
    KMacroCommand *m=0;
    GroupItem *gitem;

    QString cmdName = "Modify resourcegroups";
    Q3PtrListIterator<GroupItem> dgit(m_deletedGroupItems);
    for (; (gitem = dgit.current()) != 0; ++dgit) {
        if (!(gitem->m_state & GroupItem::New)) {
            if (!m) m = new KMacroCommand(cmdName);
            //kDebug()<<k_funcinfo<<"Remove group: '"<<gitem->m_name<<"'"<<endl;
            m->addCommand(new RemoveResourceGroupCmd(part, gitem->takeGroup()));
        }
    }

    Q3PtrListIterator<GroupItem> git(m_groupItems);
    for (; (gitem = git.current()) != 0; ++git) {
        //kDebug()<<k_funcinfo<<"Group: "<<gitem->m_name<<" has "<<gitem->m_resourceItems.count()<<" resources"<<" and "<<gitem->m_deletedItems.count()<<" deleted resources"<<endl;
        //First remove deleted resources from group
        Q3PtrListIterator<ResourcesPanelResourceItem> dit(gitem->m_deletedItems);
        ResourcesPanelResourceItem *ditem;
        for (; (ditem = dit.current()) != 0; ++dit) {
            if (!m) m = new KMacroCommand(cmdName);
            //kDebug()<<k_funcinfo<<" Deleting resource: '"<<ditem->m_originalResource->name()<<"'"<<endl;
            m->addCommand(new RemoveResourceCmd(part, gitem->m_group, ditem->m_originalResource));
        }
        // Now add/modify group/resources
        if (gitem->m_state & GroupItem::New) {
            if (!m) m = new KMacroCommand(cmdName);
            //kDebug()<<k_funcinfo<<" Adding group: '"<<gitem->m_name<<"'"<<endl;
            gitem->saveResources();
            m->addCommand(new AddResourceGroupCmd(part, gitem->takeGroup()));
            continue;
        }
        ResourceGroup *rg = gitem->takeGroup();
        if (gitem->m_state & GroupItem::Modified) {
            if (gitem->m_name != rg->name()) {
                if (!m) m = new KMacroCommand(cmdName);
                //kDebug()<<k_funcinfo<<" Modifying group: '"<<gitem->m_name<<"'"<<endl;
                m->addCommand(new ModifyResourceGroupNameCmd(part, rg, gitem->m_name));
            }
        }
        Q3PtrListIterator<ResourcesPanelResourceItem> it(gitem->m_resourceItems);
        for (; it.current() != 0; ++it) {
            KCommand *cmd = it.current()->saveResource(part, rg);
            if (cmd) {
                if (!m) m = new KMacroCommand(cmdName);
                m->addCommand(cmd);
            }
        }
    }
    return m;
}

void ResourcesPanel::slotGroupChanged() {
    slotGroupChanged(listOfGroups->selectedItem());
}

void ResourcesPanel::slotGroupChanged(Q3ListViewItem *itm) {
    ResourcesPanelGroupLVItem *item = static_cast<ResourcesPanelGroupLVItem*>(itm);
    if (!item) {
        bAdd->setEnabled(true);
        bRemove->setEnabled(false);
        listOfResources->clear();
        resourceName->clear();
        resourceGroupBox->setEnabled(false);
        return;
    }
    m_blockResourceRename = true;
    resourceName->clear();
    resourceName->setEnabled(false);
    m_blockResourceRename = false;

    m_groupItem = item;
    listOfResources->clear();

    Q3PtrListIterator<ResourcesPanelResourceItem> it(m_groupItem->m_group->m_resourceItems);
    for ( ; it.current(); ++it ) {
        listOfResources->insertItem(new ResourceLBItem(it.current()));
        //kDebug()<<k_funcinfo<<"Insert resource item: "<<it.current()->name()<<endl;
    }
    bAdd->setEnabled(true);
    bRemove->setEnabled(true);
    slotResourceChanged(0);
    resourceGroupBox->setEnabled(true);
}

void ResourcesPanel::slotListDoubleClicked(Q3ListViewItem* item, const QPoint&, int col) {
    //kDebug()<<k_funcinfo<<(item?item->text(0):"")<<endl;
    if (m_renameItem)
        return;
    slotStartRename(item, col);
}

void ResourcesPanel::slotItemRenamed(Q3ListViewItem *item, int col) {
    //kDebug()<<k_funcinfo<<item->text(0)<<endl;
    item->setRenameEnabled(col, false);
    m_renameItem = 0;
    if (col != 0) {
        renameStopped(item);
        emit changed();
        return;
    }
    if (item->text(0).isEmpty()) {
        item->setText(0, static_cast<ResourcesPanelGroupLVItem*>(item)->oldText); // keep the old name
    }
    if (item->text(0).isEmpty()) {
        // Not allowed
        //kDebug()<<k_funcinfo<<"name empty"<<endl;
        emit startRename(item, 0);
        return;
    }
    static_cast<ResourcesPanelGroupLVItem*>(item)->setName(item->text(0));
    bRemove->setEnabled(listOfGroups->selectedItem());
    bAdd->setEnabled(listOfGroups->selectedItem());
    renameStopped(item);
    emit changed();
}

void ResourcesPanel::slotRenameStarted(Q3ListViewItem */*item*/, int /*col*/) {
    //kDebug()<<k_funcinfo<<(item?item->text(0):"")<<endl;
    if (listOfGroups->isRenaming()) {
        bRemove->setEnabled(false);
        bAdd->setEnabled(false);
    }
}

void ResourcesPanel::slotStartRename(Q3ListViewItem *item, int col) {
    //kDebug()<<k_funcinfo<<(item?item->text(0):"")<<endl;
    static_cast<ResourcesPanelGroupLVItem*>(item)->oldText = item->text(col);
    item->setRenameEnabled(col, true);
    item->startRename(col);
    m_renameItem = item;
    
    emit renameStarted(item, col);
}

// We don't get notified when rename is cancelled, this is called from the item
void ResourcesPanel::renameStopped(Q3ListViewItem *) {
    //kDebug()<<k_funcinfo<<endl;
    m_renameItem = 0;
    emit selectionChanged();
}


}  //KPlato namespace

#include "kptresourcespanel.moc"
