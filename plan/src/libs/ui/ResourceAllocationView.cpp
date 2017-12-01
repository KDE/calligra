/* This file is part of the KDE project
  Copyright (C) 2017 Dag Andersen <danders@get2net.dk>

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
  Boston, MA 02110-1301, USA.
*/

#include "ResourceAllocationView.h"

#include "kptnode.h"
#include "kptresource.h"
#include "kptnodeitemmodel.h"
#include "kptresourcemodel.h"
#include "kptcommand.h"

#include <KoDocument.h>

#include <KLocalizedString>

#include <QAction>
#include <QMenu>
#include <QModelIndexList>
#include <QContextMenuEvent>
#include <QDebug>


namespace KPlato
{

ResourceAllocationView::ResourceAllocationView(KoDocument *doc, QWidget *parent)
    : QTreeView(parent)
    , m_doc(doc)
{
    m_allocateAction = new QAction(i18n("Allocate"), this);
    connect(m_allocateAction, SIGNAL(triggered(bool)), this, SLOT(slotAllocate()));
}

QList<Resource*> ResourceAllocationView::selectedResources() const
{
    QList<Resource*> resources;
    ResourceItemModel *m = qobject_cast<ResourceItemModel*>(model());
    if (m) {
        for (const QModelIndex &idx : selectionModel()->selectedRows()) {
            Resource *r = m->resource(idx);
            if (r) {
                resources << r;
            }
        }
    }
    return resources;
}

void ResourceAllocationView::setSelectedTasks(const QItemSelection &selected, const QItemSelection &deselected)
{
    for (QModelIndex &idx : deselected.indexes()) {
        if (m_tasks.contains(idx)) {
            m_tasks.removeAt(m_tasks.indexOf(idx));
        }
    }
    QModelIndexList tasks = selected.indexes();
    if (tasks.isEmpty()) {
        return;
    }
    const NodeItemModel *m = qobject_cast<const NodeItemModel*>(tasks.first().model());
    if (!m) {
        return;
    }
    for (const QModelIndex &idx : tasks) {
        if (idx.column() != NodeModel::NodeAllocation) {
            continue;
        }
        Node *n = m->node(idx);
        if (n->type() != Node::Type_Task) {
            continue;
        }
        m_tasks << QPersistentModelIndex(idx);
    }
}

void ResourceAllocationView::contextMenuEvent(QContextMenuEvent *event)
{
    if (m_tasks.isEmpty()) {
        return;
    }
    if (selectedResources().isEmpty()) {
        return;
    }
    QMenu menu;
    menu.move(event->globalPos());
    menu.addAction(m_allocateAction);
    menu.exec();
    return;    
}

void ResourceAllocationView::slotAllocate()
{
    if (!m_doc) {
        warnPlan<<"ResourceAllocationView has no document, commands cannot be executed";
        return;
    }
    QList<QPersistentModelIndex> lst;
    for (QPersistentModelIndex &idx : m_tasks) {
        if (idx.isValid()) {
            lst << idx;
        }
    }
    if (lst.isEmpty()) {
        return;
    }
    QList<Resource*> resources = selectedResources();
    if (resources.isEmpty()) {
        return;
    }
    const NodeItemModel *m = qobject_cast<const NodeItemModel*>(lst.first().model());
    MacroCommand *cmd = new MacroCommand();
    for (QPersistentModelIndex &idx : lst) {
        Node *n = m->node(idx);
        if (!n || n->type() != Node::Type_Task) {
            continue;
        }
        Task *t = static_cast<Task*>(n);
        // remove any requests before adding new ones
        for (ResourceGroupRequest *r : t->requests().requests()) {
            RemoveResourceGroupRequestCmd *c = new RemoveResourceGroupRequestCmd(r);
            c->execute(); // need to remove everyting before we add anything
            cmd->addCommand(c);
        }
        QMap<ResourceGroup*, ResourceGroupRequest*> groups;
        for (Resource *r : resources) {
            if (!groups.contains(r->parentGroup())) {
                groups[r->parentGroup()] = new ResourceGroupRequest(r->parentGroup());
                AddResourceGroupRequestCmd *c = new AddResourceGroupRequestCmd(*t, groups[r->parentGroup()]);
                c->execute();
                cmd->addCommand(c);
            }
            ResourceRequest *rr = new ResourceRequest(r);
            rr->setUnits(100); // defaults to 100%
            AddResourceRequestCmd *c = new AddResourceRequestCmd(groups[r->parentGroup()], rr);
            c->execute();
            cmd->addCommand(c);
        }
    }
    if (cmd->isEmpty()) {
        delete cmd;
    } else {
        MacroCommand *m = new MacroCommand(kundo2_i18n("Modify resource allocations"));
        m_doc->addCommand(m);
        m->addCommand(cmd);
    }
}

} // namespace KPlato
