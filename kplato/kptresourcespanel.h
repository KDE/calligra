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

#ifndef KPTPRESOURCESPANEL_H
#define KPTPRESOURCESPANEL_H

#include "kptresource.h"
#include "resourcespanelbase.h"

#include <qlistbox.h>
class KPTProject;

class KPTGroupItem : public QListBoxText {
public:
    KPTGroupItem(KPTResourceGroup *item)
        : QListBoxText(item->name()) { m_group = item; }

    void setName(const QString &newName) {
        m_group->setName(newName);
        setText(newName);
    }
    KPTResourceGroup *m_group;
};

class KPTResourceItem : public QListBoxText {
public:
    KPTResourceItem(KPTResource *item)
        : QListBoxText(item->name()) { m_resource = item; m_group = NULL; }

    void setName(const QString &newName) {
        m_resource->setName(newName);
        setText(newName);
    }

    KPTResource *m_resource;
    KPTResourceGroup *m_group; // The group this resource belongs to. (For delete)
};

class KPTResourcesPanel : public ResourcesPanelBase {
    Q_OBJECT
public:
    KPTResourcesPanel (QWidget *parent, KPTProject *project);

	void ok();

protected slots:
    void slotAddGroup();
    void slotDeleteGroup();
    void slotAddResource();
    void slotDeleteResource();

    void slotGroupChanged(QListBoxItem * item);
    void slotGroupRename( const QString &newName);
    void slotResourceRename(const QString &newName);
    void slotResourceChanged( QListBoxItem*);

signals:
	void changed();

private:
    KPTProject *project;
    KPTGroupItem *m_groupItem;
	QPtrList<KPTResourceGroup> m_deletedGroups;
	QPtrList<KPTResourceItem> m_deletedResources;
};

#endif // KPTPRESOURCESPANEL_H
