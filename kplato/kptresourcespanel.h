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
#include "ui_resourcespanelbase.h"

#include <QString>

class KCommand;

namespace KPlato
{

class Project;
class GroupItem;
class ResourcesPanelResourceItem;
class ResourcesPanelGroupLVItem;
class Part;

class ResourcesPanelBase : public QWidget, public Ui::ResourcesPanelBase
{
public:
  ResourcesPanelBase( QWidget *parent ) : QWidget( parent ) {
    setupUi( this );
  }
};


class ResourcesPanel : public ResourcesPanelBase {
    Q_OBJECT
public:
    ResourcesPanel (QWidget *parent, Project *project);
    ~ResourcesPanel();
    
    bool ok();
    KCommand *buildCommand(Part *part);

    void sendChanged();
    
protected slots:
    void slotAddGroup();
    void slotDeleteGroup();

    void slotAddResource();
    void slotEditResource();
    void slotDeleteResource();

    void slotGroupChanged(QTreeWidgetItem *item);
    void slotGroupChanged();
    void slotResourceRename(const QString &newName);
    void slotResourceChanged();
    void slotCurrentChanged(QListWidgetItem*);

signals:
    void changed();
    void selectionChanged();
    
private:
    Project *project;
    ResourcesPanelGroupLVItem *m_groupItem;

    QList<GroupItem*> m_groupItems;
    QList<GroupItem*> m_deletedGroupItems;

    bool m_blockResourceRename;
};

} //KPlato namespace

#endif // PRESOURCESPANEL_H
