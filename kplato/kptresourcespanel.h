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
#include <qstring.h>
class KPTProject;
class KPTGroupItem;
class KPTResourceItem;

class KPTGroupItem {
public:
    enum State {NONE, MODIFIED, NEW};

    KPTGroupItem(KPTResourceGroup *item, State state = NONE)
        { m_group = item; m_name = item->name(); m_state = state; }
    ~KPTGroupItem() { if (m_state == NEW) delete m_group; }

    void setState(State s);

    void setName(const QString &newName);

    void deleteResource(KPTResourceItem *item);

    KPTResourceGroup *m_group;
    QString m_name;
    QPtrList<KPTResourceItem> m_resourceItems;
    QPtrList<KPTResourceItem> m_deletedItems;
    State m_state;
};

class KPTGroupLBItem : public QListBoxText {
public:
    KPTGroupLBItem(KPTGroupItem *item) {m_group = item; setText(item->m_name); }

    void setName(const QString &newName) {
        setText(newName);
        m_group->setName(newName);
    }

    KPTGroupItem *m_group;
};

class KPTResourceItem {
public:
    enum State {NONE, MODIFIED, NEW};

    KPTResourceItem(KPTResource *item, State state = NONE)
        { m_resource = item; m_name = item->name(); m_state = state; }
    ~KPTResourceItem() { if (m_state == NEW) delete m_resource; }

    void setState(State s);

    void setName(const QString &newName) {
        m_name = newName;
        if (m_state == NEW)
            m_resource->setName(newName);
        if (m_state == NONE)
            m_state = MODIFIED;
    }

    KPTResource *m_resource;
    QDateTime m_availabeFrom, m_availableUntil;
    QString m_name;
    State m_state;
};

class KPTResourceLBItem : public QListBoxText {
public:
    KPTResourceLBItem(KPTResourceItem *item) { m_resource = item; setText(item->m_name); }

    void setName(const QString &newName) {
        setText(newName);
        m_resource->setName(newName);
    }

    KPTResourceItem *m_resource;
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
    KPTGroupLBItem *m_groupItem;

    QPtrList<KPTGroupItem> m_groupItems;
    QPtrList<KPTGroupItem> m_deletedGroupItems;
};

#endif // KPTPRESOURCESPANEL_H
