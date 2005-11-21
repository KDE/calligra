/* This file is part of the KDE project
   Copyright (C) 2003 Thomas Zander <zander@kde.org>
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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

#include <qpushbutton.h>
#include <qlistbox.h>
#include <qlineedit.h>

////////////////////////////   Private classes   //////////////////////////////////

namespace KPlato
{

class GroupLBItem;
class ResourceItem;
class Part;

class GroupItem {
public:
    enum State {None, Modified, New};

    GroupItem(ResourceGroup *item, State state = None)
        { m_group = item; m_name = item->name(); m_state = state; }
    ~GroupItem() { if (m_state == New) delete m_group; }

    void setState(State s);

    void setName(const QString &NewName);

    void deleteResource(ResourceItem *item);

    ResourceGroup *m_group;
    QString m_name;
    QPtrList<ResourceItem> m_resourceItems;
    QPtrList<ResourceItem> m_deletedItems;
    State m_state;
};

class GroupLBItem : public QListBoxText {
public:
    GroupLBItem(GroupItem *item) {m_group = item; setText(item->m_name); }

    void setName(const QString &newName) {
        setText(newName);
        m_group->setName(newName);
    }

    GroupItem *m_group;
};

class ResourceItem {
public:
    enum State {None, Modified, New};

    ResourceItem(Resource *item, State state = None);
    ~ResourceItem() { if (m_state == New) delete m_originalResource; }

    void setState(State s);

    QString name() { return m_resource->name(); }
    void setName(const QString &newName) { m_resource->setName(newName); setState(Modified); }

    KCommand *saveResource(Part *part, GroupItem *gitem);

    Resource *m_originalResource;
    Resource *m_resource; // work on a local copy
    State m_state;
};

class ResourceLBItem : public QListBoxText {
public:
    ResourceLBItem(ResourceItem *item) { m_resourceItem = item; setText(item->name());}

    QString name() { return m_resourceItem->name(); }
    void setName(const QString &newName) {
        setText(newName);
        m_resourceItem->setName(newName);
    }

    ResourceItem *m_resourceItem;
};

//-------------

void GroupItem::setState(State s) {
    if (m_state == New)
        return; // A new is allways new

    m_state = s;
}

void GroupItem::setName(const QString &newName) {
    m_name = newName;
    if (m_state == New)
        m_group->setName(newName);
    if (m_state == None)
        m_state = Modified;

    //kdDebug()<<k_funcinfo<<"New name: '"<<newName<<"', group name: '"<<m_group->name()<<"' state="<<m_state<<endl;
}


void GroupItem::deleteResource(ResourceItem *item) {

    //kdDebug()<<k_funcinfo<<" Deleted: "<<item->m_name<<" ("<<item<<")"<<endl;
    m_resourceItems.removeRef(item);
    if (item->m_state == ResourceItem::New)
        delete item;
    else
        m_deletedItems.append(item);
    //kdDebug()<<k_funcinfo<<"No of items now="<<m_resourceItems.count()<<", no of deleted items="<<m_deletedItems.count()<<endl;
}

ResourceItem::ResourceItem(Resource *item, State state) {
    m_originalResource = item;
    m_resource = new Resource(item);
    m_state = state;
}

void ResourceItem::setState(State s) {
    if (m_state == New)
        return; // A new is allways new

    m_state = s;
}

KCommand *ResourceItem::saveResource(Part *part, GroupItem *gitem) {
    KMacroCommand *m=0;
    if (m_state == New) {
        //kdDebug()<<k_funcinfo<<"Add resource: "<<m_resource->name()<<endl;
        if (!m)
            m = new KMacroCommand("Add resource");
        m->addCommand(new AddResourceCmd(part, gitem->m_group, m_resource));
    } else if (m_state == Modified) {
        //kdDebug()<<k_funcinfo<<"Modify resource: "<<m_originalResource->name()<<endl;
        if (!m)
            m = new KMacroCommand("Modify resource");
        
        if (m_resource->name() != m_originalResource->name()) {
            m->addCommand(new ModifyResourceNameCmd(part, m_originalResource, m_resource->name()));
        }
        if (m_resource->initials() != m_originalResource->initials()) {
            m->addCommand(new ModifyResourceInitialsCmd(part, m_originalResource, m_resource->initials()));
        }
        if (m_resource->email() != m_originalResource->email()) {
            m->addCommand(new ModifyResourceEmailCmd(part, m_originalResource, m_resource->email()));
        }
        if (m_resource->type() != m_originalResource->type()) {
            m->addCommand(new ModifyResourceTypeCmd(part, m_originalResource, m_resource->type()));
        }
        if (m_resource->normalRate() != m_originalResource->normalRate()) {
            m->addCommand(new ModifyResourceNormalRateCmd(part, m_originalResource, m_resource->normalRate()));
        }
        if (m_resource->overtimeRate() != m_originalResource->overtimeRate()) {
            m->addCommand(new ModifyResourceOvertimeRateCmd(part, m_originalResource, m_resource->overtimeRate()));
        }
        if (m_resource->fixedCost() != m_originalResource->fixedCost()) {
            m->addCommand(new ModifyResourceFixedCostCmd(part, m_originalResource, m_resource->fixedCost()));
        }
        m_originalResource->copy(m_resource);
    }
    return m;
}

////////////////////   ResourcesPanel   //////////////////////

ResourcesPanel::ResourcesPanel(QWidget *parent, Project *p) : ResourcesPanelBase(parent) {
    project = p;
    m_groupItem = NULL;
    m_blockResourceRename = false;

    bEditResource->setEnabled(false);
    bRemoveResource->setEnabled(false);
    resourceName->setEnabled(false);
    
    QPtrListIterator<ResourceGroup> git(project->resourceGroups());
	for(; git.current(); ++git) {
		ResourceGroup *grp = git.current();
		GroupItem *groupItem = new GroupItem(grp);
        //kdDebug()<<k_funcinfo<<" Added group: "<<groupItem->m_name<<" ("<<groupItem<<")"<<endl;
        QPtrListIterator<Resource> rit(grp->resources());
        for(; rit.current(); ++rit) {
            Resource *res = rit.current();
            ResourceItem *ritem = new ResourceItem(res);
            groupItem->m_resourceItems.append(ritem);
            //kdDebug()<<k_funcinfo<<"      Added resource: "<<ritem->m_name<<" ("<<ritem<<")"<<endl;
        }
        m_groupItems.append(groupItem);
		listOfGroups->insertItem(new GroupLBItem(groupItem));
	}
	slotGroupChanged(listOfGroups->firstItem());

    connect( bAdd, SIGNAL( clicked() ), this, SLOT( slotAddGroup() ) );
    connect (bRemove, SIGNAL( clicked() ), this, SLOT ( slotDeleteGroup() ));
    connect (listOfGroups, SIGNAL( selectionChanged( QListBoxItem*) ), this, SLOT( slotGroupChanged( QListBoxItem*) ));

    connect (bAddResource, SIGNAL( clicked() ), this, SLOT ( slotAddResource() ));
    connect (bEditResource, SIGNAL( clicked() ), this, SLOT ( slotEditResource() ));
    connect (bRemoveResource, SIGNAL( clicked() ), this, SLOT ( slotDeleteResource() ));
    connect (listOfResources, SIGNAL( selectionChanged( QListBoxItem*) ), this, SLOT ( slotResourceChanged( QListBoxItem*) ));
    connect (listOfResources, SIGNAL( currentChanged( QListBoxItem*) ), this, SLOT ( slotCurrentChanged( QListBoxItem*) ));
    connect (resourceName, SIGNAL( textChanged( const QString&) ), this, SLOT ( slotResourceRename( const QString&) ));
    connect (groupName, SIGNAL( textChanged( const QString&) ), this, SLOT ( slotGroupRename( const QString&) ));
}

void ResourcesPanel::slotAddGroup() {
    if(groupName->text().isEmpty()) return;

    ResourceGroup *r = new ResourceGroup(project);
    r->setName(groupName->text());
    GroupItem *gitem = new GroupItem(r, GroupItem::New);
    m_groupItems.append(gitem);
    GroupLBItem *groupItem = new GroupLBItem(gitem);
    listOfGroups->insertItem(groupItem);
    listOfGroups->setCurrentItem(groupItem);

	emit changed();
}

void ResourcesPanel::slotDeleteGroup() {
    GroupLBItem *groupItem = dynamic_cast<GroupLBItem*> (listOfGroups->selectedItem());
    if(groupItem == NULL) return;

    m_groupItems.removeRef(groupItem->m_group);
    if (groupItem->m_group->m_state == GroupItem::New)
        delete groupItem->m_group;
    else
        m_deletedGroupItems.append(groupItem->m_group);

	listOfResources->clear();
    listOfGroups->removeItem(listOfGroups->currentItem());

    //kdDebug()<<k_funcinfo<<" No of deleted groups="<<m_deletedGroupItems.count()<<", now "<<m_groupItems.count()<<" groups left"<<endl;

    emit changed();
}

void ResourcesPanel::slotAddResource() {
    if (!m_groupItem) {
        KMessageBox::sorry(this, i18n("Resources belong to resource groups, select the group first to add a new resource to"));
        return;
    }
    listOfResources->setSelected(listOfResources->selectedItem(), false);
    Resource *r = new Resource(project);
    ResourceDialog *dia = new ResourceDialog(*project, *r);
    if (dia->exec()) {
        ResourceItem *resourceItem = new ResourceItem(r, ResourceItem::New);
        m_groupItem->m_group->m_resourceItems.append(resourceItem);
        ResourceLBItem *item = new ResourceLBItem(resourceItem);
        listOfResources->insertItem(item);
        resourceName->clear();
        listOfResources->setSelected(item, true);
        emit changed();
        //kdDebug()<<k_funcinfo<<" Added: "<<resourceItem->name()<<" to "<<m_groupItem->m_group->m_name<<endl;
    } else {
        delete r;
    }
    delete dia;
}

void ResourcesPanel::slotEditResource() {
    //kdDebug()<<k_funcinfo<<endl;
    ResourceLBItem *item = dynamic_cast<ResourceLBItem*> (listOfResources->selectedItem());
    if(item == 0) return;
    Resource *r = item->m_resourceItem->m_resource;
    ResourceDialog *dia = new ResourceDialog(*project, *r);
    if (dia->exec()) {
        resourceName->setText(r->name());
        item->m_resourceItem->setState(ResourceItem::Modified);
        item->setName(r->name()); // refresh list
        listOfResources->triggerUpdate(false);
        emit changed();
    }
    delete dia;
}

void ResourcesPanel::slotDeleteResource() {
    //kdDebug()<<k_funcinfo<<endl;
    ResourceLBItem *item = dynamic_cast<ResourceLBItem*> (listOfResources->selectedItem());
    if(item == NULL) return;

    //Can't delete resource from unselected group
    if(m_groupItem == NULL) return;

    m_groupItem->m_group->deleteResource(item->m_resourceItem);
    listOfResources->removeItem(listOfResources->currentItem());

    emit changed();
}

/* Selected another group */
void ResourcesPanel::slotGroupChanged( QListBoxItem *item) {
    //kdDebug()<<k_funcinfo<<endl;
    if (!item)
        return;

    m_blockResourceRename = true;
    resourceName->clear();
    resourceName->setEnabled(false);
    m_blockResourceRename = false;

    m_groupItem = (GroupLBItem *)item;
    listOfResources->clear();

    QPtrListIterator<ResourceItem> it(m_groupItem->m_group->m_resourceItems);
    for ( ; it.current(); ++it ) {
        listOfResources->insertItem(new ResourceLBItem(it.current()));
        //kdDebug()<<k_funcinfo<<"Insert resource item: "<<it.current()->name()<<endl;
    }
}

void ResourcesPanel::slotGroupRename( const QString &newName) {
    //kdDebug()<<k_funcinfo<<endl;
    QListBoxItem *item = listOfGroups->selectedItem();
    if(!item) return;

    ((GroupLBItem *)item)->setName(newName);

    listOfGroups->triggerUpdate(false);

	emit changed();
}

/* Select another resource */
void ResourcesPanel::slotResourceChanged( QListBoxItem *item) {
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
void ResourcesPanel::slotCurrentChanged( QListBoxItem *item) {
    if (item && !item->isSelected()) {
        listOfResources->setSelected(item, true);
    }
}

void ResourcesPanel::slotResourceRename( const QString &newName) {
    QListBoxItem *item = listOfResources->selectedItem();
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
    QPtrListIterator<GroupItem> dgit(m_deletedGroupItems);
    for (; (gitem = dgit.current()) != 0; ++dgit) {
        if (!m)
            m = new KMacroCommand(cmdName);
        
        //kdDebug()<<k_funcinfo<<"Remove group: '"<<gitem->m_name<<"'"<<endl;
        m->addCommand(new RemoveResourceGroupCmd(part, gitem->m_group));
    }

    QPtrListIterator<GroupItem> git(m_groupItems);
    for (; (gitem = git.current()) != 0; ++git) {
        //kdDebug()<<k_funcinfo<<"Group: "<<gitem->m_name<<" has "<<gitem->m_resourceItems.count()<<" resources"<<" and "<<gitem->m_deletedItems.count()<<" deleted resources"<<endl;
        //First remove deleted resources from group
        QPtrListIterator<ResourceItem> dit( gitem->m_deletedItems );
        ResourceItem *ditem;
        for (; (ditem = dit.current()) != 0; ++dit) {
            if (!m)
                m = new KMacroCommand(cmdName);
            //kdDebug()<<k_funcinfo<<" Deleting resource: '"<<ditem->m_originalResource->name()<<"'"<<endl;            
            m->addCommand(new RemoveResourceCmd(part, gitem->m_group, ditem->m_originalResource));
        }
        // Now add/modify group/resources
        if (gitem->m_state == GroupItem::New) {
            if (!m)
                m = new KMacroCommand(cmdName);
            //kdDebug()<<k_funcinfo<<" Adding group: '"<<gitem->m_name<<"'"<<endl;
            m->addCommand(new AddResourceGroupCmd(part, gitem->m_group));
        } else if (gitem->m_state == GroupItem::Modified) {
            if (!m)
                m = new KMacroCommand(cmdName);
            //kdDebug()<<k_funcinfo<<" Modifying group: '"<<gitem->m_name<<"'"<<endl;
            m->addCommand(new ModifyResourceGroupNameCmd(part, gitem->m_group, gitem->m_name));
        }
        QPtrListIterator<ResourceItem> it( gitem->m_resourceItems );
        for (; it.current() != 0; ++it) {
            KCommand *cmd = it.current()->saveResource(part, gitem);
            if (cmd) {
                if (!m)
                    m = new KMacroCommand("Modify Resources");
                m->addCommand(cmd);
            }
        }
    }
    return m;
}


}  //KPlato namespace

#include "kptresourcespanel.moc"
