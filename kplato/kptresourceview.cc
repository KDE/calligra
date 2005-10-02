/* This file is part of the KDE project
   Copyright (C) 2003 - 2004 Dag Andersen kplato@kde.org>

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

#include "kptresourceview.h"

#include "kptpart.h"
#include "kptview.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"
#include "kptdatetime.h"
#include "kptcontext.h"

#include <klistview.h>

#include <qtabwidget.h>
#include <qpopupmenu.h>


#include <klocale.h>
#include <kglobal.h>
#include <kprinter.h>

#include <kdebug.h>

namespace KPlato
{

class ResourceItemPrivate : public QListViewItem {
public:
    ResourceItemPrivate(KPTResource *r, QListViewItem *parent)
        : QListViewItem(parent, r->name()),
        resource(r) {}

    KPTResource *resource;
};

class NodeItemPrivate : public QListViewItem {
public:
    NodeItemPrivate(KPTNode *n, QListView *parent)
        : QListViewItem(parent, n->name()),
        node(n) {}

    KPTNode *node;
};

class AppointmentItem : public QListViewItem {
public:
    AppointmentItem(KPTAppointment *a, QDate &d, QListViewItem *parent)
        : QListViewItem(parent),
        appointment(a),
        date(d) {}

    KPTAppointment *appointment;
    QDate date;
};

KPTResourceView::KPTResourceView(KPTView *view, QWidget *parent)
    : QSplitter(parent, "Resource view"),
    m_mainview(view),
    m_selectedItem(0),
    m_currentNode(0)
{
    setOrientation(QSplitter::Vertical);

    resList = new KListView(this, "Resource list");
    resList->setItemMargin(2);
    resList->setRootIsDecorated(true);
    resList->addColumn(i18n("Name"));
    resList->setColumnAlignment(1, AlignHCenter);
    resList->addColumn(i18n("Type"));
    resList->setColumnAlignment(2, AlignRight);
    resList->addColumn(i18n("Normal Rate"));
    resList->setColumnAlignment(3, AlignRight);
    resList->addColumn(i18n("Overtime Rate"));
    resList->setColumnAlignment(4, AlignRight);
    resList->addColumn(i18n("Fixed Cost"));

    draw(view->getPart()->getProject());

    QTabWidget *app = new QTabWidget(this);

    appList = new KListView(app, "Appointments view");
    appList->setItemMargin(2);
    appList->setRootIsDecorated(true);
    appList->addColumn(i18n("Task"));
    appList->addColumn(i18n("Planned Effort"));
    appList->setColumnAlignment(1, AlignRight);
    appList->addColumn(i18n("Planned Cost"));
    appList->setColumnAlignment(2, AlignRight);
    appList->addColumn(i18n("Actual Effort"));
    appList->setColumnAlignment(3, AlignRight);
    appList->addColumn(i18n("Actual Cost"));
    appList->setColumnAlignment(4, AlignRight);

    appList->setSortColumn(100);
    
    app->addTab(appList, i18n("Appointments"));

    connect(resList, SIGNAL(selectionChanged(QListViewItem*)), SLOT(resSelectionChanged(QListViewItem*)));
    connect(resList, SIGNAL( contextMenuRequested(QListViewItem*, const QPoint&, int)), SLOT(popupMenuRequested(QListViewItem*, const QPoint&, int)));

    connect(appList, SIGNAL( contextMenuRequested(QListViewItem*, const QPoint&, int)), SLOT(appListMenuRequested(QListViewItem*, const QPoint&, int)));
    
    connect(appList, SIGNAL( itemRenamed(QListViewItem*, int, const QString&)), SLOT(slotItemRenamed(QListViewItem*, int, const QString&)));
}

void KPTResourceView::zoom(double zoom)
{
}

KPTResource *KPTResourceView::currentResource() {
    if (m_selectedItem)
        return m_selectedItem->resource;
    return 0;
}

void KPTResourceView::draw(KPTProject &project)
{
    //kdDebug()<<k_funcinfo<<endl;
	resList->clear();
    m_selectedItem = 0;

    QPtrListIterator<KPTResourceGroup> it(project.resourceGroups());
    for (; it.current(); ++it) {
        QListViewItem *item = new QListViewItem(resList, it.current()->name());
	    item->setOpen(true);
        drawResources(item, it.current());
    }
    if (m_selectedItem) {
        resList->setSelected(m_selectedItem, true);
    }
}


void KPTResourceView::drawResources(QListViewItem *parent, KPTResourceGroup *group)
{
    //kdDebug()<<k_funcinfo<<"group: "<<group->name()<<" ("<<group<<")"<<endl;
    QPtrListIterator<KPTResource> it(group->resources());
    for (; it.current(); ++it) {
        KPTResource *r = it.current();
        ResourceItemPrivate *item = new ResourceItemPrivate(r, parent);
        switch (r->type()) {
            case KPTResource::Type_Work:
                item->setText(1, i18n("Work"));
                break;
            case KPTResource::Type_Material:
                item->setText(1, i18n("Material"));
                break;
            default:
                item->setText(1, i18n("Undefined"));
                break;
        }
        item->setText(2, KGlobal::locale()->formatMoney(r->normalRate()));
        item->setText(3, KGlobal::locale()->formatMoney(r->overtimeRate()));
        item->setText(4, KGlobal::locale()->formatMoney(r->fixedCost()));
        if (!m_selectedItem) {
            m_selectedItem = item;
        }
    }
}


void KPTResourceView::resSelectionChanged(QListViewItem *item) {
    //kdDebug()<<k_funcinfo<<endl;
    if (item == 0)
        return;

    appList->clear();
    ResourceItemPrivate *ritem = dynamic_cast<ResourceItemPrivate *>(item);
    if (ritem) {
        m_selectedItem = ritem;
        drawAppointments(ritem->resource);
    }
}

void KPTResourceView::drawAppointments(KPTResource *resource)
{
    //kdDebug()<<k_funcinfo<<"Appointments for resource: "<<resource->name()<<endl;
    QPtrListIterator<KPTAppointment> it(resource->appointments());
    for (it.toLast(); it.current(); --it) {
        KPTTask *t = dynamic_cast<KPTTask*>(it.current()->node());
        if (t == 0) continue;

        QListViewItem *item = new NodeItemPrivate(t, appList);
        KPTAppointment *app = t->findAppointment(resource);
        item->setText(1, app->plannedEffort().toString(KPTDuration::Format_HourFraction));
        item->setText(2, KGlobal::locale()->formatMoney(app->plannedCost()));
        item->setText(3, app->actualEffort().toString(KPTDuration::Format_HourFraction));
        item->setText(4, KGlobal::locale()->formatMoney(app->actualCost()));
        
        QDate startDate = t->startTime().date();
        QDate date = t->endTime().date();
        for (; date >= startDate; date = date.addDays(-1)) {
            QListViewItem *i = new AppointmentItem(app, date, item);
            i->setText(0, date.toString(Qt::ISODate));
            i->setText(1, app->plannedEffort(date).toString(KPTDuration::Format_HourFraction));
            i->setText(2, KGlobal::locale()->formatMoney(app->plannedCost(date)));
            i->setText(3, app->actualEffort(date).toString(KPTDuration::Format_HourFraction));
            i->setText(4, KGlobal::locale()->formatMoney(app->actualCost(date)));
            
            i->setRenameEnabled(3, true);
        }
    }
}

void KPTResourceView::popupMenuRequested(QListViewItem* item, const QPoint & pos, int)
{
    ResourceItemPrivate *ritem = dynamic_cast<ResourceItemPrivate *>(item);
    if (ritem) {
        if (ritem != m_selectedItem)
            resSelectionChanged(ritem);
        QPopupMenu *menu = m_mainview->popupMenu("resource_popup");
        if (menu)
        {
            int id = menu->exec(pos);
            //kdDebug()<<k_funcinfo<<"id="<<id<<endl;
        }
        else
            kdDebug()<<k_funcinfo<<"No menu!"<<endl;
    }
}

void KPTResourceView::appListMenuRequested(QListViewItem * item, const QPoint & pos, int)
{
/*    NodeItemPrivate *nitem = dynamic_cast<NodeItemPrivate *>(item);
    if (nitem == 0) {
        return;
    }
    m_currentNode = nitem->node;
    QPopupMenu *menu = m_mainview->popupMenu("node_popup");
    if (menu)
    {
        int id = menu->exec(pos);
        //kdDebug()<<k_funcinfo<<"id="<<id<<endl;
    } else
        kdDebug()<<k_funcinfo<<"No menu!"<<endl;*/
}

void KPTResourceView::print(KPrinter &printer) {
    kdDebug()<<k_funcinfo<<endl;

}

bool KPTResourceView::setContext(KPTContext &context) {
    kdDebug()<<k_funcinfo<<endl;
    return true;
}

void KPTResourceView::getContext(KPTContext &context) const {
    kdDebug()<<k_funcinfo<<endl;
}

//FIXME
void KPTResourceView::slotItemRenamed(QListViewItem* item, int col, const QString& text) {
    kdDebug()<<k_funcinfo<<item->text(0)<<" ("<<col<<"): "<<text<<endl;
    bool ok;
    KPTDuration v = KPTDuration::fromString(text, KPTDuration::Format_HourFraction, &ok);
    AppointmentItem *i = static_cast<AppointmentItem*>(item);
    if (ok) {
        i->appointment->addActualEffort(i->date, v);
        item->setText(4, KGlobal::locale()->formatMoney(i->appointment->actualCost(i->date)));
        // and update parent
        NodeItemPrivate *n = static_cast<NodeItemPrivate*>(item->parent());
        if (n) {
            n->setText(3, i->appointment->actualEffort().toString(KPTDuration::Format_HourFraction));
            n->setText(4, KGlobal::locale()->formatMoney(i->appointment->actualCost()));
        }
    }
    item->setText(col, i->appointment->actualEffort(i->date).toString(KPTDuration::Format_HourFraction));
}

}  //KPlato namespace

#include "kptresourceview.moc"
