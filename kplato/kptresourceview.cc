/* This file is part of the KDE project
   Copyright (C) 2003 Dag Andersen kplato@kde.org>

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

#include "kptresourceview.h"

#include "kptpart.h"
#include "kptview.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"
#include "kptdatetime.h"

#include <qlistview.h>
#include <qtabwidget.h>
#include <qpopupmenu.h>


#include <klocale.h>
#include <kglobal.h>
#include <kprinter.h>

#include <kdebug.h>

class ResourceItemPrivate : public QListViewItem {
public:
    ResourceItemPrivate(KPTResource *r, QListViewItem *parent)
        : QListViewItem(parent, r->name()),
        resource(r) {}

    KPTResource *resource;
};

KPTResourceView::KPTResourceView(KPTView *view, QWidget *parent)
    : QSplitter(parent, "Resource view"),
    m_mainview(view),
    m_selectedItem(0)
{
    setOrientation(QSplitter::Vertical);

    resList = new QListView(this, "Resource list");
    resList->setRootIsDecorated(true);
    resList->addColumn(i18n("Name"));
    resList->addColumn(i18n("Type"));
    resList->setColumnAlignment(1, AlignHCenter);
    resList->addColumn(i18n("Normal rate"));
    resList->setColumnAlignment(2, AlignRight);
    resList->addColumn(i18n("Overtime rate"));
    resList->setColumnAlignment(3, AlignRight);
    resList->addColumn(i18n("Fixed cost"));
    resList->setColumnAlignment(4, AlignRight);

    draw(view->getPart()->getProject());

    QTabWidget *app = new QTabWidget(this);

    appList = new QListView(app, "Appointments view");
    appList->addColumn(i18n("Task"));
    appList->addColumn(i18n("Responsible"));
    appList->addColumn(i18n("Start date"));
    appList->addColumn(i18n("End date"));
    appList->addColumn(i18n("Duration"));
    appList->setColumnAlignment(4, AlignRight);

    app->addTab(appList, i18n("Appointments"));

    connect(resList, SIGNAL(selectionChanged(QListViewItem*)), SLOT(resSelectionChanged(QListViewItem*)));
    connect(resList, SIGNAL(rightButtonPressed(QListViewItem*, const QPoint&, int)), SLOT(popupMenuRequested(QListViewItem*, const QPoint&, int)));
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
    for (; it.current(); ++it) {
        KPTTask *t = it.current()->task();
        if (t) {
            KPTDuration *dur = t->getExpectedDuration();
            QListViewItem *item = new QListViewItem(appList,
                    t->name(), t->leader(),
                    t->startTime().date().toString(), t->endTime().date().toString(),
                    dur->toString(KPTDuration::Format_Hour)); // FIXME
            delete dur;
        }
    }
}

void KPTResourceView::popupMenuRequested(QListViewItem * item, const QPoint & pos, int)
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

void KPTResourceView::print(KPrinter &printer) {
    kdDebug()<<k_funcinfo<<endl;

}

#include "kptresourceview.moc"
