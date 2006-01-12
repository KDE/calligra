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
#include "kptresourceappointmentsview.h"
#include "kptview.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"
#include "kptdatetime.h"
#include "kptcontext.h"

#include <qpopupmenu.h>


#include <klistview.h>
#include <klocale.h>
#include <kglobal.h>
#include <kprinter.h>

#include <kdebug.h>

namespace KPlato
{

class ResourceItemPrivate : public KListViewItem {
public:
    ResourceItemPrivate(Resource *r, QListViewItem *parent)
        : KListViewItem(parent, r->name()),
        resource(r) {}

    Resource *resource;
};

class NodeItemPrivate : public KListViewItem {
public:
    NodeItemPrivate(Task *n, QListView *parent)
    : KListViewItem(parent, n->name()),
      node(n) {
        init();
    }

    NodeItemPrivate(QString name, QListView *parent)
    : KListViewItem(parent, name),
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
        KListViewItem::paintCell(p, g, column, width, align);
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

class AppointmentItem : public KListViewItem {
public:
    AppointmentItem(Appointment *a, QDate &d, QListViewItem *parent)
        : KListViewItem(parent),
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
    resList->setColumnAlignment(2, AlignHCenter);
    resList->addColumn(i18n("Initials"));
    resList->setColumnAlignment(3, AlignLeft);
    resList->addColumn(i18n("Email"));
    resList->setColumnAlignment(4, AlignRight);
    resList->addColumn(i18n("Available From"));
    resList->setColumnAlignment(5, AlignRight);
    resList->addColumn(i18n("Available Until"));
    resList->setColumnAlignment(6, AlignRight);
    resList->addColumn(i18n("Normal Rate"));
    resList->setColumnAlignment(7, AlignRight);
    resList->addColumn(i18n("Overtime Rate"));

    m_appview = new ResourceAppointmentsView(view, this);

    draw(view->getProject());

    //connect(resList, SIGNAL(selectionChanged(QListViewItem*)), SLOT(resSelectionChanged(QListViewItem*)));
    connect(resList, SIGNAL(selectionChanged()), SLOT(resSelectionChanged()));
    connect(resList, SIGNAL( contextMenuRequested(QListViewItem*, const QPoint&, int)), SLOT(popupMenuRequested(QListViewItem*, const QPoint&, int)));
    //NOTE: using doubleClicked, not executed() to be consistent with ganttview
    connect(resList, SIGNAL(doubleClicked(QListViewItem*, const QPoint&, int)), SLOT(slotItemDoubleClicked(QListViewItem*)));

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
    m_appview->clear();
    m_selectedItem = 0;

    QPtrListIterator<ResourceGroup> it(project.resourceGroups());
    for (; it.current(); ++it) {
        KListViewItem *item = new KListViewItem(resList, it.current()->name());
        item->setOpen(true);
        drawResources(item, it.current());
    }
    if (m_selectedItem) {
        resList->setSelected(m_selectedItem, true);
    }
    resSelectionChanged(m_selectedItem);
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
        item->setText(2, r->initials());
        item->setText(3, r->email());
        item->setText(4, KGlobal::locale()->formatDateTime(r->availableFrom()));
        item->setText(5, KGlobal::locale()->formatDateTime(r->availableUntil()));
        item->setText(6, KGlobal::locale()->formatMoney(r->normalRate()));
        item->setText(7, KGlobal::locale()->formatMoney(r->overtimeRate()));
        if (!m_selectedItem) {
            m_selectedItem = item;
        }
    }
}


void ResourceView::resSelectionChanged() {
    //kdDebug()<<k_funcinfo<<endl;
    resSelectionChanged(resList->selectedItem());
}

void ResourceView::resSelectionChanged(QListViewItem *item) {
    //kdDebug()<<k_funcinfo<<item<<endl;
    ResourceItemPrivate *ritem = dynamic_cast<ResourceItemPrivate *>(item);
    if (ritem) {
        m_selectedItem = ritem;
        m_appview->draw(ritem->resource, m_mainview->getProject().startTime().date(), m_mainview->getProject().endTime().date());
        return;
    }
    m_selectedItem = 0;
    m_appview->clear();
}


void ResourceView::slotItemDoubleClicked(QListViewItem*) {
    emit itemDoubleClicked();
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

void ResourceView::print(KPrinter &printer) {
    kdDebug()<<k_funcinfo<<endl;

}

bool ResourceView::setContext(Context::Resourceview &context) {
    //kdDebug()<<k_funcinfo<<endl;
    return true;
}

void ResourceView::getContext(Context::Resourceview &context) const {
    //kdDebug()<<k_funcinfo<<endl;
}


}  //KPlato namespace

#include "kptresourceview.moc"
