/* This file is part of the KDE project
   Copyright (C) 2003, 2004 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTREQUESTRESOURCESPANEL_H
#define KPTREQUESTRESOURCESPANEL_H

#include "kpttaskresourcespanelbase.h"
#include "kptduration.h"

#include <qlistview.h>
#include <qstring.h>
#include <qtable.h>
//class QCheckTableItem;

class KCommand;
class KMacroCommand;

namespace KPlato
{

class KPTAccount;
class KPTTask;
class KPTResourceGroup;
class KPTResource;
class KPTResourceGroupRequest;
class KPTResourceRequest;
class KPTStandardWorktime;
class KPTPart;
class KPTDuration;

class KPTResourceTableItem {
public:
    KPTResourceTableItem(KPTResource *resource, KPTResourceRequest *request, bool check = false);
    ~KPTResourceTableItem() ;

    void update();
    void insert(QTable *table, int row, QStringList &accounts);
    void ok(KPTResourceGroupRequest *group);

    bool isChecked() const { return m_checked; }
    bool isOrigChecked() const { return m_origChecked; }
    KPTResource *resource() { return m_resource; }
    KPTResourceRequest *request() { return m_request; }
    int numRequests() const { return m_checked ? 1 : 0; }
    int units() const { return m_units; }

    KPTResource *m_resource;
    int m_units, m_origUnits;
    bool m_checked, m_origChecked;
    QCheckTableItem *m_checkitem;
    KPTResourceRequest *m_request;
    QComboTableItem *m_accountitem;
    KPTAccount *m_account;
    int m_curAccountItem;
    QString m_curAccountText;
};

class KPTGroupLVItem : public QListViewItem {
public:
    KPTGroupLVItem(QListView *parent, KPTResourceGroup *group, KPTTask &task);
    ~KPTGroupLVItem();

    void update();
    void insert(QTable *table, QStringList &accounts);
    const QPtrList<KPTResourceTableItem> &resources() const { return m_resources; }
    void ok(KPTTask &task);

    int numRequests();
    bool isNull() const;
    
    KPTResourceGroup *m_group;
    int m_units;
    QPtrList<KPTResourceTableItem> m_resources;
    KPTResourceGroupRequest *m_request;
};


class KPTRequestResourcesPanel : public KPTTaskResourcesPanelBase {
    Q_OBJECT
public:
    KPTRequestResourcesPanel(QWidget *parent, KPTTask &task, bool baseline=false);

    KCommand *buildCommand(KPTPart *part);
    
    bool ok();
    
private slots:
    void sendChanged();

    void groupChanged(QListViewItem *item);
    void resourceChanged(int, int);
    void unitsChanged(int);
    
signals:
    void changed();

private:
    KPTTask &m_task;
    KPTStandardWorktime *m_worktime;
    KPTGroupLVItem *selectedGroup;
    bool m_blockChanged;
    QStringList m_accounts;
    
};

}  //KPlato namespace

#endif
