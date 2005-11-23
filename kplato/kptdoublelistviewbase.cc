/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen kplato@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#include "kptdoublelistviewbase.h"

#include "kptproject.h"
#include "kptview.h"

#include <qheader.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qsplitter.h>
#include <qstring.h>
#include <qvaluelist.h>

#include <kcalendarsystem.h>
#include <kglobal.h>
#include <klistview.h>
#include <klocale.h>
#include <kprinter.h>

#include <kdebug.h>

namespace KPlato
{

DoubleListViewBase::SlaveDLVItem::SlaveDLVItem(DoubleListViewBase::MasterDLVItem *master, QListView *parent, QListViewItem *after, bool highlight)
    : KListViewItem(parent, after),
      m_masterItem(master),
      m_value(0.0),
      m_highlight(highlight),
      m_valueMap() {
    
    setExpandable(master->isExpandable());
    setOpen(master->isOpen());
    //kdDebug()<<"SlaveDLVItem "<<master->text(0)<<" parent="<<static_cast<DoubleListViewBase::SlaveDLVItem*>(parent)->m_masterItem->text(0)<<endl;
}
DoubleListViewBase::SlaveDLVItem::SlaveDLVItem(DoubleListViewBase::MasterDLVItem *master, QListViewItem *parent, QListViewItem *after, bool highlight)
    : KListViewItem(parent, after),
      m_masterItem(master),
      m_value(0.0),
      m_highlight(highlight),
      m_valueMap() {
    
    setExpandable(master->isExpandable());
    setOpen(master->isOpen());
    //kdDebug()<<"SlaveDLVItem "<<master->text(0)<<" parent="<<static_cast<DoubleListViewBase::SlaveDLVItem*>(parent)->m_masterItem->text(0)<<endl;
}
DoubleListViewBase::SlaveDLVItem::~SlaveDLVItem() {
    //kdDebug()<<k_funcinfo<<endl;
    if (m_masterItem)
        m_masterItem->slaveItemDeleted();
}

void DoubleListViewBase::SlaveDLVItem::clearColumn(int col) {
    if (col >= listView()->columns()) {
        return;
    }
    listView()->setColumnText(col, "");
    setText(col, "");
    m_valueMap[col] = 0;
}
void DoubleListViewBase::SlaveDLVItem::setColumn(int col, double value) {
    if (col < listView()->columns()) {
        setText(col, QString("%1").arg(value, 0, 'f', 0));
        m_valueMap.replace(col, value);
        //kdDebug()<<k_funcinfo<<m_masterItem->text(0)<<": column["<<col<<"]="<<value<<endl;
    }
}

void DoubleListViewBase::SlaveDLVItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align) {
    //kdDebug()<<k_funcinfo<<"c="<<column<<endl;
    QColorGroup g = cg;
    if (m_highlight) {
        if (m_value < 0.0) {
            g.setColor(QColorGroup::Text, QColor(red));
        } else if (m_value > 0.0) {
            g.setColor(QColorGroup::Text, QColor(green));
        }
    }
    KListViewItem::paintCell(p, g, column, width, align);
}

DoubleListViewBase::MasterDLVItem::MasterDLVItem(QListView *parent, bool highlight)
    : KListViewItem(parent),
      m_slaveItem(0),
      m_value(0.0),
      m_highlight(highlight) {
    
    //kdDebug()<<k_funcinfo<<endl;
}

DoubleListViewBase::MasterDLVItem::MasterDLVItem(QListViewItem *parent, QString text,   bool highlight)
    : KListViewItem(parent, text),
      m_slaveItem(0),
      m_value(0.0),
      m_highlight(highlight) {

    //kdDebug()<<k_funcinfo<<endl;
}

DoubleListViewBase::MasterDLVItem::MasterDLVItem(QListViewItem *parent, QString text, QString text2, bool highlight)
    : KListViewItem(parent, text, text2),
      m_slaveItem(0),
      m_value(0.0),
      m_highlight(highlight) {
    
    //kdDebug()<<k_funcinfo<<endl;
}
        
DoubleListViewBase::MasterDLVItem::~MasterDLVItem() {
    if (m_slaveItem)
        m_slaveItem->masterItemDeleted();
}

void DoubleListViewBase::MasterDLVItem::createSlaveItems(QListView *lv, QListViewItem *after) {
    //kdDebug()<<k_funcinfo<<text(0)<<endl;
    if (m_slaveItem) {
        kdError()<<k_funcinfo<<"Slave item allready exists"<<endl;
    } else {
        if (parent() == 0) {
            m_slaveItem = new DoubleListViewBase::SlaveDLVItem(this, lv, after);
        } else {
            m_slaveItem = new DoubleListViewBase::SlaveDLVItem(this, static_cast<DoubleListViewBase::MasterDLVItem*>(parent())->m_slaveItem, after);
        }
    }
    DoubleListViewBase::SlaveDLVItem *prev = 0;
    for (QListViewItem *item = firstChild(); item; item = item->nextSibling()) {
        static_cast<DoubleListViewBase::MasterDLVItem*>(item)->createSlaveItems(lv, prev);
        prev = static_cast<DoubleListViewBase::MasterDLVItem*>(item)->m_slaveItem;
    }
}

void DoubleListViewBase::MasterDLVItem::setSlaveOpen(bool on) {
    if (m_slaveItem)
        m_slaveItem->setOpen(on);
}

void DoubleListViewBase::MasterDLVItem::slaveItemDeleted() {
    setTotal(0);
    m_slaveItem = 0;
}

void DoubleListViewBase::MasterDLVItem::setTotal(double tot) {
    m_value = tot;
    setText(1, QString("%1").arg(tot, 0, 'f', 0));
    //kdDebug()<<k_funcinfo<<text(0)<<"="<<tot<<endl;
}

void DoubleListViewBase::MasterDLVItem::addToTotal(double v) {
    m_value += v;
    setText(1, QString("%1").arg(m_value, 0, 'f', 0));
}

double DoubleListViewBase::MasterDLVItem::calcTotal() {
    double tot=0.0;
    QListViewItem *item=firstChild();
    if (!item) {
        tot = m_value;
    } else {
        for (; item; item = item->nextSibling()) {
            tot += static_cast<DoubleListViewBase::MasterDLVItem*>(item)->calcTotal();
        }
    }
    setTotal(tot);
    return tot;
}

void DoubleListViewBase::MasterDLVItem::setSlaveItem(int col, double value) {
    if (m_slaveItem) {
        m_slaveItem->setColumn(col, value);
    }
}

void DoubleListViewBase::MasterDLVItem::clearColumn(int col) {
    for (QListViewItem *item=firstChild(); item; item=item->nextSibling()) {
        static_cast<DoubleListViewBase::MasterDLVItem*>(item)->clearColumn(col);
    }
    setTotal(0);
    if (m_slaveItem == 0) {
        kdError()<<k_funcinfo<<"No m_slaveItem"<<endl;
        return;
    }
    m_slaveItem->clearColumn(0);
}

void DoubleListViewBase::MasterDLVItem::calcSlaveItems() {
    if (m_slaveItem == 0 || m_slaveItem->listView() == 0) {
        kdError()<<k_funcinfo<<"No m_slaveItem or m_slaveItem->listView()"<<endl;
        return;
    }
    int cols = m_slaveItem->listView()->columns();
    for (int i = 0; i < cols; ++i) {
        calcSlaveItems(i);
    }
}

double DoubleListViewBase::MasterDLVItem::calcSlaveItems(int col) {
    if (m_slaveItem == 0)
        return 0.0;
    QListViewItem *item=firstChild();
    if (!item) {
        return m_slaveItem->value(col);
    }
    double tot=0.0;
    for (; item; item = item->nextSibling()) {
        tot += static_cast<DoubleListViewBase::MasterDLVItem*>(item)->calcSlaveItems(col);
    }
    //kdDebug()<<k_funcinfo<<text(0)<<" "<<col<<"="<<tot<<endl;
    setSlaveItem(col, tot);
    return tot;
}

void DoubleListViewBase::MasterDLVItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align) {
    //kdDebug()<<k_funcinfo<<"c="<<column<<" prio="<<(columnPrio.contains(column)?columnPrio[column]:0)<<endl;
    QColorGroup g = cg;
    if (m_highlight && column == 1) { //Total
        if (m_value < 0.0) {
            g.setColor(QColorGroup::Text, QColor(red));
        } else if (m_value > 0.0) {
            g.setColor(QColorGroup::Text, QColor(green));
        }
    }
    KListViewItem::paintCell(p, g, column, width, align);
}

DoubleListViewBase::DoubleListViewBase(Project &project, View *view, QWidget *parent, bool description)
    : QSplitter(parent),
      m_project(project),
      m_mainview(view) {
    
    setOrientation(QSplitter::Horizontal);
    setHandleWidth(QMIN(2, handleWidth()));
    
    m_masterList = new KListView(this);
    m_masterList->setSelectionMode(QListView::NoSelection);
    m_masterList->setItemMargin(2);
    m_masterList->setRootIsDecorated(true);
    m_masterList->setSortColumn(-1); // Disable sort!!
    m_masterList->addColumn(i18n("Name"));
    m_masterList->addColumn(i18n("Total"));
    m_masterList->setColumnAlignment(1, AlignRight);
    if (description) {
        m_masterList->addColumn(i18n("Description"));
        m_masterList->setColumnAlignment(2, AlignLeft);
        m_masterList->header()->setStretchEnabled(true, 2);
        m_masterList->header()->moveSection(2, 1);
    } else {
        m_masterList->header()->setStretchEnabled(true, 0);
    }
    m_masterList->setVScrollBarMode(QScrollView::AlwaysOff);
    m_masterList->setHScrollBarMode(QScrollView::AlwaysOn);
    
    m_slaveList = new KListView(this);
    m_slaveList->setSelectionMode(QListView::NoSelection);
    m_slaveList->setItemMargin(2);
    m_slaveList->setSortColumn(-1); // Disable sort!!
    m_slaveList->setTreeStepSize(0);
    m_slaveList->setHScrollBarMode(QScrollView::AlwaysOn);
    
    
    connect(m_slaveList->verticalScrollBar(), SIGNAL(valueChanged(int)),
            m_masterList->verticalScrollBar(), SLOT(setValue(int)));
    
    connect(m_masterList, SIGNAL(expanded(QListViewItem*)), SLOT(slotExpanded(QListViewItem*)));
    connect(m_masterList, SIGNAL(collapsed(QListViewItem*)), SLOT(slotCollapsed(QListViewItem*)));
    
}

void DoubleListViewBase::clearSlaveList() {
    while (m_slaveList->columns() > 0) {
        m_slaveList->removeColumn(0); // removing the last one clears the list!!!
    }
    m_slaveList->clear(); // to be safe
}

void DoubleListViewBase::createSlaveItems() {
    clearSlaveList();
    DoubleListViewBase::SlaveDLVItem *prev = 0;
    for (QListViewItem *item = m_masterList->firstChild(); item; item = item->nextSibling()) {
        static_cast<DoubleListViewBase::MasterDLVItem*>(item)->createSlaveItems(m_slaveList, prev);
        prev = static_cast<DoubleListViewBase::MasterDLVItem*>(item)->slaveItem();
    }
}


void DoubleListViewBase::print(KPrinter &printer) {
    kdDebug()<<k_funcinfo<<endl;
    Q_UNUSED(printer);
}

void DoubleListViewBase::slotExpanded(QListViewItem* item) {
    if (item) {
        static_cast<DoubleListViewBase::MasterDLVItem*>(item)->setSlaveOpen(true);
    }
}

void DoubleListViewBase::slotCollapsed(QListViewItem*item) {
    if (item) {
        static_cast<DoubleListViewBase::MasterDLVItem*>(item)->setSlaveOpen(false);
    }
}


}  //KPlato namespace

#include "kptdoublelistviewbase.moc"
