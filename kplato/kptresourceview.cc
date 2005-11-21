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

#include "kptcalendar.h"
#include "kptduration.h"
#include "kptpart.h"
#include "kptview.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"
#include "kptdatetime.h"
#include "kptcontext.h"

#include <klistview.h>

#include <qmap.h>
#include <qtabwidget.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qpopupmenu.h>


#include <klocale.h>
#include <kglobal.h>
#include <kprinter.h>

#include <kdebug.h>

namespace KPlato
{

class ResourceItemPrivate : public QListViewItem {
public:
    ResourceItemPrivate(Resource *r, QListViewItem *parent)
        : QListViewItem(parent, r->name()),
        resource(r) {}

    Resource *resource;
};

class NodeItemPrivate : public QListViewItem {
public:
    NodeItemPrivate(Task *n, QListView *parent)
    : QListViewItem(parent, n->name()),
      node(n) {
        init();
    }

    NodeItemPrivate(QString name, QListView *parent)
    : QListViewItem(parent, name),
      node(0) {
        init();
    }

    void setPriority(int col, int prio) {
        if (prioColors.contains(prio)) {
            columnPrio.insert(col, prio);
        } else {
            columnPrio.remove(col);
        }
    }
    int priority(int col) {
        if (columnPrio.contains(col)) {
            return columnPrio[col];
        }
        return 0;
    }
        
    virtual void paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align ) {
        //kdDebug()<<k_funcinfo<<"c="<<column<<" prio="<<(columnPrio.contains(column)?columnPrio[column]:0)<<endl;
        QColorGroup g = cg;
        if (columnPrio.contains(column)) {
            g.setColor(QColorGroup::Base, prioColors[columnPrio[column]]);
        }
        QListViewItem::paintCell(p, g, column, width, align);
    }
    
    Task *node;
private:
    void init() {
        prioColors.insert(1, QColor(gray));
        prioColors.insert(2, QColor(green));
        prioColors.insert(3, QColor(yellow));
        prioColors.insert(4, QColor(red));
    }
    QMap<int, QColor> prioColors;
    QMap<int, int> columnPrio;
};

class AppointmentItem : public QListViewItem {
public:
    AppointmentItem(Appointment *a, QDate &d, QListViewItem *parent)
        : QListViewItem(parent),
        appointment(a),
        date(d) {}

    Appointment *appointment;
    QDate date;
};

ResourceView::ResourceView(View *view, QWidget *parent)
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

    draw(view->getProject());

    //QTabWidget *app = new QTabWidget(this);

    appList = new KListView(this, "Appointments view");
    appList->setItemMargin(2);
    appList->setRootIsDecorated(true);
    appList->setSortColumn(-1);
    appList->addColumn(i18n("Task"));
    
    // Add columns for all days in the project
    m_start = view->getProject().startTime().date();
    m_end = view->getProject().endTime().date();
    int c=1;
    for (QDate dt = m_start; dt <= m_end; dt = dt.addDays(1)) {
        appList->addColumn(dt.toString("dd/MM")); // FIXME I18n
        //appList->setColumnAlignment(c, AlignRight);
        c++;
    }
    
    //app->addTab(appList, i18n("Appointments"));

    connect(resList, SIGNAL(selectionChanged(QListViewItem*)), SLOT(resSelectionChanged(QListViewItem*)));
    connect(resList, SIGNAL( contextMenuRequested(QListViewItem*, const QPoint&, int)), SLOT(popupMenuRequested(QListViewItem*, const QPoint&, int)));

    connect(appList, SIGNAL( contextMenuRequested(QListViewItem*, const QPoint&, int)), SLOT(appListMenuRequested(QListViewItem*, const QPoint&, int)));
    
    connect(appList, SIGNAL( itemRenamed(QListViewItem*, int, const QString&)), SLOT(slotItemRenamed(QListViewItem*, int, const QString&)));
}

void ResourceView::zoom(double zoom)
{
}

Resource *ResourceView::currentResource() {
    if (m_selectedItem)
        return m_selectedItem->resource;
    return 0;
}

void ResourceView::draw(Project &project)
{
    //kdDebug()<<k_funcinfo<<endl;
	resList->clear();
    m_selectedItem = 0;

    QPtrListIterator<ResourceGroup> it(project.resourceGroups());
    for (; it.current(); ++it) {
        QListViewItem *item = new QListViewItem(resList, it.current()->name());
	    item->setOpen(true);
        drawResources(item, it.current());
    }
    if (m_selectedItem) {
        resList->setSelected(m_selectedItem, true);
    }
}


void ResourceView::drawResources(QListViewItem *parent, ResourceGroup *group)
{
    //kdDebug()<<k_funcinfo<<"group: "<<group->name()<<" ("<<group<<")"<<endl;
    QPtrListIterator<Resource> it(group->resources());
    for (; it.current(); ++it) {
        Resource *r = it.current();
        ResourceItemPrivate *item = new ResourceItemPrivate(r, parent);
        switch (r->type()) {
            case Resource::Type_Work:
                item->setText(1, i18n("Work"));
                break;
            case Resource::Type_Material:
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


void ResourceView::resSelectionChanged(QListViewItem *item) {
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

void ResourceView::drawAppointments(Resource *resource)
{
    //kdDebug()<<k_funcinfo<<"Appointments for resource: "<<resource->name()<<endl;
    Calendar *cal = resource->calendar();
    
    QPtrListIterator<Appointment> it(resource->appointments());
    if (it.current() == 0)
        return;
    new NodeItemPrivate(i18n("=Total (available)"), appList);
    for (it.toLast(); it.current(); --it) {
        Task *t = dynamic_cast<Task*>(it.current()->node());
        if (t == 0) continue;
        new NodeItemPrivate(t, appList);
    }
    int c = 1; //First date column
    Duration tot;
    for (QDate dt = m_start; dt <= m_end; dt = dt.addDays(1)) {
        QListViewItemIterator lit(appList);
        for (; lit.current(); ++lit) {
            NodeItemPrivate *item = dynamic_cast<NodeItemPrivate*>(lit.current());
            if (item->node == 0) continue;
            Appointment *app = item->node->findAppointment(resource);
        
            Duration d = app->plannedEffort(dt);
            item->setText(c, d.toString(Duration::Format_HourFraction));
            
            tot += d;
        }
        QListViewItemIterator qit(appList);
        for (; qit.current(); ++qit) {
            NodeItemPrivate *item = dynamic_cast<NodeItemPrivate*>(qit.current());
            Duration avail = cal->effort(dt, QTime(), QTime(23,59,59));
            if (item->node == 0) { // HACK: fix NodeItemPrivate
                item->setText(c, QString("%1(%2)").arg(tot.toString(Duration::Format_HourFraction), avail.toString(Duration::Format_HourFraction)));
            }
            if (avail == 0)
                item->setPriority(c, 1);
            else if (tot > avail)
                item->setPriority(c, 4);
            else if (tot == avail)
                item->setPriority(c, 3);
            else if (tot < avail)
                item->setPriority(c, 2);
            else
                item->setPriority(c, 0);
        }
        tot = Duration::zeroDuration;
        c++;
     }
}

void ResourceView::popupMenuRequested(QListViewItem* item, const QPoint & pos, int)
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

void ResourceView::appListMenuRequested(QListViewItem * item, const QPoint & pos, int)
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

void ResourceView::print(KPrinter &printer) {
    kdDebug()<<k_funcinfo<<endl;

}

bool ResourceView::setContext(Context &context) {
    kdDebug()<<k_funcinfo<<endl;
    return true;
}

void ResourceView::getContext(Context &context) const {
    kdDebug()<<k_funcinfo<<endl;
}

//FIXME
void ResourceView::slotItemRenamed(QListViewItem* item, int col, const QString& text) {
    kdDebug()<<k_funcinfo<<item->text(0)<<" ("<<col<<"): "<<text<<endl;
    bool ok;
    Duration v = Duration::fromString(text, Duration::Format_HourFraction, &ok);
    AppointmentItem *i = static_cast<AppointmentItem*>(item);
    if (ok) {
        i->appointment->addActualEffort(i->date, v);
        item->setText(4, KGlobal::locale()->formatMoney(i->appointment->actualCost(i->date)));
        // and update parent
        NodeItemPrivate *n = static_cast<NodeItemPrivate*>(item->parent());
        if (n) {
            n->setText(3, i->appointment->actualEffort().toString(Duration::Format_HourFraction));
            n->setText(4, KGlobal::locale()->formatMoney(i->appointment->actualCost()));
        }
    }
    item->setText(col, i->appointment->actualEffort(i->date).toString(Duration::Format_HourFraction));
}

}  //KPlato namespace

#include "kptresourceview.moc"
