/* This file is part of the KDE project
   Copyright (C) 2003 - 2004 Dag Andersen <danders@get2net.dk>

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

#include "kpttask.h"
#include "kptcanvasitem.h"
#include "kptrelation.h"
#include "kptganttview.h"

#include <klocale.h>
#include <qpainter.h>
#include <q3pointarray.h>
#include <q3ptrlist.h>
#include <QPoint>

#include <kdebug.h>

namespace KPlato
{


////////////////////   ItemBase   //////////////////////////
KDGanttViewTaskLink::LinkType ItemBase::kdLinkType(int relationType) {
    switch (relationType) {
        case Relation::FinishStart:
            return KDGanttViewTaskLink::FinishStart;
            break;
        case Relation::FinishFinish:
            return KDGanttViewTaskLink::FinishFinish;
            break;
        case Relation::StartStart:
            return KDGanttViewTaskLink::StartStart;
            break;
        default:
            break;
    }
    return KDGanttViewTaskLink::None;
}

////////////////////   GanttViewSummaryItem   //////////////////////////


GanttViewSummaryItem::GanttViewSummaryItem(KDGanttView *parent, Node *node)
    : KDGanttViewSummaryItem(parent, node->name()),
      m_node(node),
      m_view(parent)
{
    setExpandable(true);
    setOpen(true);
}

GanttViewSummaryItem::GanttViewSummaryItem(KDGanttViewItem *parent, Node *node)
    : KDGanttViewSummaryItem(parent, node->name()),
      m_node(node),
      m_view(0)
{
    m_drawn = false;
    GanttViewSummaryItem *p = dynamic_cast<GanttViewSummaryItem*>(parent);
    if (p)
        m_view = p->ganttView();
    setExpandable(true);
    setOpen(true);
}

void GanttViewSummaryItem::insertRelations(GanttView *view)
{
    //kDebug()<<k_funcinfo<<endl;

    foreach (Relation *r, m_node->dependChildNodes())
    {
        KDGanttViewItem *child = find(m_view->firstChild(), r->child());
        if (child)
        {
            KDGanttViewTaskLink *link = new KDGanttViewTaskLink(this, child, kdLinkType(r->type()));
            //TODO i18n
            QString t = i18n("From: %1", this->listViewText(0));
            t += '\n' + i18n("To: %1", child->listViewText(0));
            if (r->lag() > Duration::zeroDuration) {
                t += '\n' + i18n("Lag:  %1", r->lag().toString(Duration::Format_i18nDayTime));
            }
            link->setTooltipText(t);
            view->addTaskLink(link);
        }
    }
}

KDGanttViewItem *GanttViewSummaryItem::find(Node *node)
{
    //kDebug()<<k_funcinfo<<endl;
    if (m_node == node)
        return this;

    KDGanttViewItem *item = find(firstChild(), node);
    if (item)
        return item;

    return find(nextSibling(), node);
}


KDGanttViewItem *GanttViewSummaryItem::find(KDGanttViewItem *item, Node *node)
{
    if (!item)
        return 0;

    if (item->type() == Event)
    {
        GanttViewEventItem *i = static_cast<GanttViewEventItem *>(item);
        return i->find(node);
    }
    else if (item->type() == Task)
    {
        GanttViewTaskItem *i = static_cast<GanttViewTaskItem *>(item);
        return i->find(node);
    }
    else if (item->type() == Summary)
    {
        GanttViewSummaryItem *i = static_cast<GanttViewSummaryItem *>(item);
        return i->find(node);
    }
    return 0;
}

////////////////////   GanttViewTaskItem   //////////////////////////


GanttViewTaskItem::GanttViewTaskItem(KDGanttView *parent, KPlato::Task *task)
    : KDGanttViewTaskItem(parent, task->name()),
      m_task(task),
      m_view(parent)
{
}

GanttViewTaskItem::GanttViewTaskItem(KDGanttViewItem *parent, KPlato::Task *task)
    : KDGanttViewTaskItem(parent, task->name()),
      m_task(task),
      m_view()
{
    m_drawn = false;
    GanttViewSummaryItem *p = dynamic_cast<GanttViewSummaryItem*>(parent);
    if (p)
        m_view = p->ganttView();
}

void GanttViewTaskItem::insertRelations(GanttView *view)
{
    //kDebug()<<k_funcinfo<<endl;

    foreach (Relation *r, m_task->dependChildNodes())
    {
        KDGanttViewItem *child = find(m_view->firstChild(), r->child());
        if (child)
        {
            KDGanttViewTaskLink *link = new KDGanttViewTaskLink(this, child,  kdLinkType(r->type()));
            QString t = i18n("From: %1", this->listViewText(0));
            t += '\n' + i18n("To: %1", child->listViewText(0));
            if (r->lag() > Duration::zeroDuration) {
                t += '\n' + i18n("Lag:  %1", r->lag().toString(Duration::Format_i18nDayTime));
            }
            link->setTooltipText(t);
            view->addTaskLink(link);
        }
    }
}

KDGanttViewItem *GanttViewTaskItem::find(Node *node)
{
    //kDebug()<<k_funcinfo<<endl;
    if (m_task == node)
        return this;

    KDGanttViewItem *item = find(firstChild(), node);
    if (item)
        return item;

    return find(nextSibling(), node);
}


KDGanttViewItem *GanttViewTaskItem::find(KDGanttViewItem *item, Node *node)
{
    if (!item)
        return 0;

    if (item->type() == Event)
    {
        GanttViewEventItem *i = static_cast<GanttViewEventItem *>(item);
        return i->find(node);
    }
    else if (item->type() == Task)
    {
        GanttViewTaskItem *i= static_cast<GanttViewTaskItem *>(item);
        return i->find(node);
    }
    else if (item->type() == Summary)
    {
        GanttViewSummaryItem *i = static_cast<GanttViewSummaryItem *>(item);
        return i->find(node);
    }
    return 0; // avoid warning
}

////////////////////   GanttViewEventItem   //////////////////////////


GanttViewEventItem::GanttViewEventItem(KDGanttView *parent, KPlato::Task *task)
    : KDGanttViewEventItem(parent, task->name()),
      m_task(task),
      m_view(parent)
{
}

GanttViewEventItem::GanttViewEventItem(KDGanttViewItem *parent, KPlato::Task *task)
    : KDGanttViewEventItem(parent, task->name()),
      m_task(task),
      m_view()
{
    m_drawn = false;
    GanttViewSummaryItem *p = dynamic_cast<GanttViewSummaryItem*>(parent);
    if (p)
        m_view = p->ganttView();
}


void GanttViewEventItem::insertRelations(GanttView *view)
{
    //kDebug()<<k_funcinfo<<endl;

    foreach (Relation *r, m_task->dependChildNodes())
    {
        KDGanttViewItem *child = find(m_view->firstChild(), r->child());
        if (child)
        {
            KDGanttViewTaskLink *link = new KDGanttViewTaskLink(this, child, kdLinkType(r->type()));

            QString t = i18n("From: %1",this->listViewText(0));
            t += '\n' + i18n("To: %1",child->listViewText(0));
            if (r->lag() > Duration::zeroDuration) {
                t += '\n' + i18n("Lag:  %1",r->lag().toString(Duration::Format_i18nDayTime));
            }
            link->setTooltipText(t);
            view->addTaskLink(link);
        }
    }
}

KDGanttViewItem *GanttViewEventItem::find(Node *node)
{
    //kDebug()<<k_funcinfo<<endl;
    if (m_task == node)
        return this;

    KDGanttViewItem *item = find(firstChild(), node);
    if (item)
        return item;

    return find(nextSibling(), node);
}


KDGanttViewItem *GanttViewEventItem::find(KDGanttViewItem *item, Node *node)
{
    if (!item)
        return 0;

    if (item->type() == Event)
    {
        GanttViewEventItem *i = static_cast<GanttViewEventItem *>(item);
        return i->find(node);
    }
    else if (item->type() == Task)
    {
        GanttViewTaskItem *i = static_cast<GanttViewTaskItem *>(item);
        return i->find(node);
    }
    else if (item->type() == Summary)
    {
        GanttViewSummaryItem *i = static_cast<GanttViewSummaryItem *>(item);
        return i->find(node);
    }
    return 0;
}

}  //KPlato namespace
