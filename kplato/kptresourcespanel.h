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
class KPTGroupLBItem;

class KPTResourcesPanel : public ResourcesPanelBase {
    Q_OBJECT
public:
    KPTResourcesPanel (QWidget *parent, KPTProject *project);

	void ok();

    void sendChanged();

protected slots:
    void slotAddGroup();
    void slotDeleteGroup();

    void slotChooseResource();
    void slotAddResource();
    void slotEditResource();
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

    bool m_blockResourceRename;
};

#endif // KPTPRESOURCESPANEL_H
