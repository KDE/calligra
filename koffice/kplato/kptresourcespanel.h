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

#include <qlistbox.h>
#include <qstring.h>

class QListViewItem;
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

    void renameStopped(QListViewItem* item);
    
protected slots:
    void slotAddGroup();
    void slotDeleteGroup();

    void slotAddResource();
    void slotEditResource();
    void slotDeleteResource();

    void slotGroupChanged(QListViewItem *item);
    void slotGroupChanged();
    void slotResourceRename(const QString &newName);
    void slotResourceChanged( QListBoxItem*);
    void slotCurrentChanged( QListBoxItem*);

    void slotListDoubleClicked(QListViewItem*, const QPoint&, int);
    void slotItemRenamed(QListViewItem *item, int col);
    void slotRenameStarted(QListViewItem *item, int col);
    void slotStartRename(QListViewItem *item, int col);
signals:
    void changed();
    void selectionChanged();
    void startRename(QListViewItem *item, int col);
    void renameStarted(QListViewItem *item, int col);
    
private:
    Project *project;
    ResourcesPanelGroupLVItem *m_groupItem;

    QPtrList<GroupItem> m_groupItems;
    QPtrList<GroupItem> m_deletedGroupItems;

    bool m_blockResourceRename;
    QListViewItem *m_renameItem;
};

} //KPlato namespace

#endif // PRESOURCESPANEL_H
