/* This file is part of the KDE project
   Copyright (C) 2003 Thomas Zander <zander@kde.org>

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

#include "kptresourcespanel.h"
#include "kptproject.h"

#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>

#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qlineedit.h>


void KPTGroupItem::setState(State s) {
    if (m_state == NEW)
        return; // A new is allways new

    m_state = s;
}

void KPTGroupItem::setName(const QString &newName) {
    m_name = newName;
    if (m_state == NEW)
        m_group->setName(newName);
    if (m_state == NONE)
        m_state = MODIFIED;

    //kdDebug()<<k_funcinfo<<"New name: '"<<newName<<"', group name: '"<<m_group->name()<<"' state="<<m_state<<endl;
}


void KPTGroupItem::deleteResource(KPTResourceItem *item) {

    //kdDebug()<<k_funcinfo<<" Deleted: "<<item->m_name<<" ("<<item<<")"<<endl;
    m_resourceItems.removeRef(item);
    if (item->m_state == KPTResourceItem::NEW)
        delete item;
    else
        m_deletedItems.append(item);
    //kdDebug()<<k_funcinfo<<"No of items now="<<m_resourceItems.count()<<", no of deleted items="<<m_deletedItems.count()<<endl;
}

void KPTResourceItem::setState(State s) {
    if (m_state == NEW)
        return; // A new is allways new

    m_state = s;
}

KPTResourcesPanel::KPTResourcesPanel(QWidget *parent, KPTProject *p) : ResourcesPanelBase(parent) {
    project = p;
    m_groupItem = NULL;

	QPtrListIterator<KPTResourceGroup> git(project->resourceGroups());
	for(; git.current(); ++git) {
		KPTResourceGroup *grp = dynamic_cast<KPTResourceGroup *>(git.current());
		KPTGroupItem *groupItem = new KPTGroupItem(grp);
        //kdDebug()<<k_funcinfo<<" Added group: "<<groupItem->m_name<<" ("<<groupItem<<")"<<endl;
        QPtrListIterator<KPTResource> rit(grp->resources());
        for(; rit.current(); ++rit) {
            KPTResource *res = dynamic_cast<KPTResource *>(rit.current());
            KPTResourceItem *ritem = new KPTResourceItem(res);
            groupItem->m_resourceItems.append(ritem);
            //kdDebug()<<k_funcinfo<<"      Added resource: "<<ritem->m_name<<" ("<<ritem<<")"<<endl;
        }
        m_groupItems.append(groupItem);
		listOfGroups->insertItem(new KPTGroupLBItem(groupItem));
	}
	slotGroupChanged(listOfGroups->firstItem());

    connect( bAdd, SIGNAL( clicked() ), this, SLOT( slotAddGroup() ) );
    connect (bRemove, SIGNAL( clicked() ), this, SLOT ( slotDeleteGroup() ));
    connect (listOfGroups, SIGNAL( selectionChanged( QListBoxItem*) ), this, SLOT( slotGroupChanged( QListBoxItem*) ));
    connect (bAddResouce, SIGNAL( clicked() ), this, SLOT ( slotAddResource() ));
    connect (bRemoveResource, SIGNAL( clicked() ), this, SLOT ( slotDeleteResource() ));
    connect (listOfResources, SIGNAL( selectionChanged( QListBoxItem*) ), this, SLOT ( slotResourceChanged( QListBoxItem*) ));
    connect (resourceName, SIGNAL( textChanged( const QString&) ), this, SLOT ( slotResourceRename( const QString&) ));
    connect (groupName, SIGNAL( textChanged( const QString&) ), this, SLOT ( slotGroupRename( const QString&) ));
}

void KPTResourcesPanel::slotAddGroup() {
    if(groupName->text().isEmpty()) return;

    KPTResourceGroup *r = new KPTResourceGroup(project);
    r->setName(groupName->text());
    KPTGroupItem *gitem = new KPTGroupItem(r, KPTGroupItem::NEW);
    m_groupItems.append(gitem);
    KPTGroupLBItem *groupItem = new KPTGroupLBItem(gitem);
    listOfGroups->insertItem(groupItem);
    listOfGroups->setCurrentItem(groupItem);

	emit changed();
}

void KPTResourcesPanel::slotDeleteGroup() {
    KPTGroupLBItem *groupItem = dynamic_cast<KPTGroupLBItem*> (listOfGroups->selectedItem());
    if(groupItem == NULL) return;

    m_groupItems.removeRef(groupItem->m_group);
    if (groupItem->m_group->m_state == KPTGroupItem::NEW)
        delete groupItem->m_group;
    else
        m_deletedGroupItems.append(groupItem->m_group);

	listOfResources->clear();
    listOfGroups->removeItem(listOfGroups->currentItem());

    //kdDebug()<<k_funcinfo<<" No of deleted groups="<<m_deletedGroupItems.count()<<", now "<<m_groupItems.count()<<" groups left"<<endl;

    emit changed();
}

void KPTResourcesPanel::slotAddResource() {
    if(resourceName->text().isEmpty()) return;
    if (!m_groupItem) {
        KMessageBox::sorry(this, i18n("Resources belong to resource groups, select the group first to add a new resource to"));
        return;
    }

    KPTResource *res = new KPTResource(project);
    res->setName(resourceName->text());
   
    //res->addWorkingHour(); // TODO
    //m_groupItem->m_group->addResource(res, 0); //TODO only add to project when OK is pressed
    KPTResourceItem *resourceItem = new KPTResourceItem(res, KPTResourceItem::NEW);
    m_groupItem->m_group->m_resourceItems.append(resourceItem);
    //kdDebug()<<k_funcinfo<<" Added: "<<resourceItem->m_name<<" ("<<resourceItem<<")"<<endl;

    listOfResources->insertItem(new KPTResourceLBItem(resourceItem));
    resourceName->clear();

	emit changed();
}

void KPTResourcesPanel::slotDeleteResource() {
    //kdDebug()<<k_funcinfo<<endl;
    KPTResourceLBItem *item = dynamic_cast<KPTResourceLBItem*> (listOfResources->selectedItem());
    if(item == NULL) return;

    //Can't delete resource from unselected group
    if(m_groupItem == NULL) return;

    m_groupItem->m_group->deleteResource(item->m_resource);
    listOfResources->removeItem(listOfResources->currentItem());

    emit changed();
}

/* Selected another group */
void KPTResourcesPanel::slotGroupChanged( QListBoxItem *item) {
    //kdDebug()<<k_funcinfo<<endl;
    if (!item)
        return;

    resourceName->clear();

    m_groupItem = (KPTGroupLBItem *)item;
    listOfResources->clear();

    QPtrListIterator<KPTResourceItem> it(m_groupItem->m_group->m_resourceItems);
    for ( ; it.current(); ++it ) {
        listOfResources->insertItem(new KPTResourceLBItem(it.current()));
        //kdDebug()<<k_funcinfo<<"Insert resource item: "<<it.current()->m_name<<endl;
    }
    //kdDebug()<<k_funcinfo<<" Resources now: "<<it.count()<<endl;
}

void KPTResourcesPanel::slotGroupRename( const QString &newName) {
    //kdDebug()<<k_funcinfo<<endl;
    QListBoxItem *item = listOfGroups->selectedItem();
    if(!item) return;

    ((KPTGroupLBItem *)item)->setName(newName);

    listOfGroups->triggerUpdate(false);

	emit changed();
}

/* Select another resource */
void KPTResourcesPanel::slotResourceChanged( QListBoxItem *item) {
    if (!item)
        return;
    resourceName->setText( ((KPTResourceLBItem *)item)->m_resource->m_name);
}

void KPTResourcesPanel::slotResourceRename( const QString &newName) {
    QListBoxItem *item = listOfResources->selectedItem();
    if(!item) return;

    KPTResourceLBItem *i = dynamic_cast<KPTResourceLBItem *>(item);
    if (i->m_resource->m_name == newName) return;

    //kdDebug()<<k_funcinfo<<" Old name: "<<i->m_resource->m_name<<" ("<<i<<") "<<"  New name: "<<newName<<endl;
    i->setName(newName);
    listOfResources->triggerUpdate(false);

    emit changed();
}

void KPTResourcesPanel::ok() {
    KPTGroupItem *gitem;

    QPtrListIterator<KPTGroupItem> dgit(m_deletedGroupItems);
    for (; (gitem = dgit.current()) != 0; ++dgit) {
        //kdDebug()<<k_funcinfo<<"Remove group: '"<<gitem->m_name<<"'"<<endl;
        project->removeResourceGroup(gitem->m_group); // remove group and its resources from project
    }

    QPtrListIterator<KPTGroupItem> git(m_groupItems);
    for (; (gitem = git.current()) != 0; ++git) {
        //kdDebug()<<k_funcinfo<<"Group: "<<gitem->m_name<<" has "<<gitem->m_resourceItems.count()<<" resources"<<" and "<<gitem->m_deletedItems.count()<<" deleted resources"<<endl;
        //First remove deleted resources from group
        QPtrListIterator<KPTResourceItem> dit( gitem->m_deletedItems );
        KPTResourceItem *ditem;
        for (; (ditem = dit.current()) != 0; ++dit) {
            //kdDebug()<<k_funcinfo<<" Deleting resource: '"<<ditem->m_name<<"'"<<endl;
            gitem->m_group->removeResource(ditem->m_resource); // remove resource from project
        }
        // Now add/modify group/resources
        if (gitem->m_state == KPTGroupItem::NEW) {
            //kdDebug()<<k_funcinfo<<" Adding group: '"<<gitem->m_name<<"'"<<endl;
            project->addResourceGroup(gitem->m_group);
        } else if (gitem->m_state == KPTGroupItem::MODIFIED) {
            //kdDebug()<<k_funcinfo<<" Modifying group: '"<<gitem->m_name<<"'"<<endl;
            gitem->m_group->setName(gitem->m_name);
        }
        QPtrListIterator<KPTResourceItem> it( gitem->m_resourceItems );
        KPTResourceItem *ritem;
        for (; (ritem = it.current()) != 0; ++it) {
            switch (ritem->m_state) {
                case KPTResourceItem::MODIFIED:
                    //kdDebug()<<k_funcinfo<<" Modifying resource: '"<<ritem->m_name<<"'"<<endl;
                    ritem->m_resource->setName(ritem->m_name);
                    break;
                case KPTResourceItem::NEW:
                    //kdDebug()<<k_funcinfo<<" Adding resource: '"<<ritem->m_resource->name()<<"' to group: '"<<gitem->m_group->name()<<"'"<<endl;
                    gitem->m_group->addResource(ritem->m_resource, 0);
                    break;
                default:
                    //kdDebug()<<k_funcinfo<<" No action for resource: '"<<ritem->m_name<<"' with state="<<ritem->m_state<<endl;
                    break;
            }
        }
    }
}

#include "kptresourcespanel.moc"
