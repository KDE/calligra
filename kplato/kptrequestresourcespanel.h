/* This file is part of the KDE project
   Copyright (C) 2003 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTREQUESTRESOURCESPANEL_H
#define KPTREQUESTRESOURCESPANEL_H

#include "kpttaskresourcespanelbase.h"


#include <qlistview.h>
#include <qstring.h>
#include <qtable.h>
//class QCheckTableItem;

class KPTTask;
class KPTResourceGroup;
class KPTResource;
class KPTResourceGroupRequest;
class KPTResourceRequest;
class KMacroCommand;

class KPTResourceTableItem {
public:
    KPTResourceTableItem(KPTResource *resource, bool check = false);
    ~KPTResourceTableItem() ;

    void clear();
    void insert(QTable *table, int row);
    void ok(KPTResourceGroupRequest *group);

    int numRequests() { return m_checked ? 1 : 0; }

    KPTResource *m_resource;
    int m_units;
    bool m_checked;
    QCheckTableItem *m_checkitem;
    KPTResourceRequest *m_request;
};

class KPTGroupLVItem : public QListViewItem {
public:
    KPTGroupLVItem(QListView *parent, KPTResourceGroup *group, KPTTask &task);
    ~KPTGroupLVItem();

    void clear();
    void insert(QTable *table);
    void ok(KPTTask &task);

    int numRequests();

    KPTResourceGroup *m_group;
    int m_units;
    QPtrList<KPTResourceTableItem> m_resources;
    KPTResourceGroupRequest *m_request;
};


class KPTRequestResourcesPanel : public KPTTaskResourcesPanelBase {
    Q_OBJECT
public:
    KPTRequestResourcesPanel (QWidget *parent, KPTTask &task, QPtrList<KPTResourceGroup> &resourceGroups);

    KMacroCommand *buildCommand();
    
public slots:
	void slotOk();

private slots:
    void sendChanged();

    void groupChanged(QListViewItem *item);
    void resourceChanged(int, int);
    void unitsChanged(int);

signals:
    void changed();

private:
    KPTTask &m_task;
    QPtrList<KPTResourceGroup> &m_resourceGroups;

    KPTGroupLVItem *selectedGroup;
    bool m_blockChanged;
};

#endif
