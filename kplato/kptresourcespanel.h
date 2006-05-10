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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTPRESOURCESPANEL_H
#define KPTPRESOURCESPANEL_H

#include "kptresource.h"
#include "resourcespanelbase.h"

#include <q3listbox.h>
#include <QString>
//Added by qt3to4:
#include <Q3PtrList>

class Q3ListViewItem;
class KCommand;

namespace KPlato
{

class Project;
class GroupItem;
class ResourcesPanelResourceItem;
class ResourcesPanelGroupLVItem;
class Part;

class ResourcesPanel : public ResourcesPanelBase {
    Q_OBJECT
public:
    ResourcesPanel (QWidget *parent, Project *project);
    
    bool ok();
    KCommand *buildCommand(Part *part);

    void sendChanged();

    void renameStopped(Q3ListViewItem* item);
    
protected slots:
    void slotAddGroup();
    void slotDeleteGroup();

    void slotAddResource();
    void slotEditResource();
    void slotDeleteResource();

    void slotGroupChanged(Q3ListViewItem *item);
    void slotGroupChanged();
    void slotResourceRename(const QString &newName);
    void slotResourceChanged( Q3ListBoxItem*);
    void slotCurrentChanged( Q3ListBoxItem*);

    void slotListDoubleClicked(Q3ListViewItem*, const QPoint&, int);
    void slotItemRenamed(Q3ListViewItem *item, int col);
    void slotRenameStarted(Q3ListViewItem *item, int col);
    void slotStartRename(Q3ListViewItem *item, int col);
signals:
    void changed();
    void selectionChanged();
    void startRename(Q3ListViewItem *item, int col);
    void renameStarted(Q3ListViewItem *item, int col);
    
private:
    Project *project;
    ResourcesPanelGroupLVItem *m_groupItem;

    Q3PtrList<GroupItem> m_groupItems;
    Q3PtrList<GroupItem> m_deletedGroupItems;

    bool m_blockResourceRename;
    Q3ListViewItem *m_renameItem;
};

} //KPlato namespace

#endif // PRESOURCESPANEL_H
