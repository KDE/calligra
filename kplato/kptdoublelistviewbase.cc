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
#include <qmap.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qptrvector.h>
#include <qsplitter.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qpoint.h>

#include <kcalendarsystem.h>
#include <kglobal.h>
#include <klocale.h>
#include <kprinter.h>
#include <qrect.h>

#include <kdebug.h>

namespace KPlato
{
void ListView::paintToPrinter(QPainter * p, int cx, int cy, int cw, int ch) {
    //kdDebug()<<k_funcinfo<<QRect(cx, cy, cw, ch)<<endl;
    // draw header labels
    p->save();
    QRegion r = p->clipRegion(QPainter::CoordPainter);
    p->setClipRegion(r.intersect(QRegion(cx, 0, cw, ch)), QPainter::CoordPainter);
    QColor bgc(193, 223, 255);
    QBrush bg(bgc);
    p->setBackgroundMode(Qt::OpaqueMode);
    p->setBackgroundColor(bgc);
    QHeader *h = header();
    int hei = 0;
    for (int s = 0; s < h->count(); ++s) {
        QRect r = h->sectionRect(s);
        //kdDebug()<<s<<": "<<h->label(s)<<" "<<r<<endl;
        int x, y;
        viewportToContents(r.x(), r.y(), x, y);
        QRect sr(x, y, r.width(), r.height());
        //kdDebug()<<s<<": "<<h->label(s)<<" "<<sr<<endl;
        if (sr.x()+sr.width() <= cx || sr.x() >= cx+cw) {
            //kdDebug()<<s<<": "<<h->label(s)<<" "<<sr<<": continue"<<endl;
            continue;
        }
        QRect tr = sr;
        if (sr.x() < cx) {
            tr.setX(cx);
            //kdDebug()<<s<<": "<<h->label(s)<<" "<<tr<<endl;
        }
        p->eraseRect(tr);
        p->drawText(tr, columnAlignment(s)|Qt::AlignVCenter, h->label(s), -1);
        hei = QMAX(tr.height(), hei);
    }
    r = p->clipRegion(QPainter::CoordPainter);
    p->restore();
//    p->drawRect(r.boundingRect());
    p->save();
    p->translate(0, hei+2);
    r = p->clipRegion(QPainter::CoordPainter);
    // FIXME: Doesn't clip correctly, haven't figured out why
    p->setClipRegion(r.intersect(QRegion(cx, cy, cw, ch)), QPainter::CoordPainter);
    drawContentsOffset(p, 0, 0, cx, cy, cw, ch);
//    p->drawRect(r.boundingRect());
    p->restore();
}

DoubleListViewBase::SlaveListItem::SlaveListItem(DoubleListViewBase::MasterListItem *master, QListView *parent, QListViewItem *after, bool highlight)
    : KListViewItem(parent, after),
      m_masterItem(master),
      m_value(0.0),
      m_highlight(highlight),
      m_valueMap() {
    
    setFormat();
    setExpandable(master->isExpandable());
    setOpen(master->isOpen());
    //kdDebug()<<"DoubleListViewBase::SlaveListItem "<<master->text(0)<<" parent="<<static_cast<DoubleListViewBase::SlaveListItem*>(parent)->m_masterItem->text(0)<<endl;
}
DoubleListViewBase::SlaveListItem::SlaveListItem(DoubleListViewBase::MasterListItem *master, QListViewItem *parent, QListViewItem *after, bool highlight)
    : KListViewItem(parent, after),
      m_masterItem(master),
      m_value(0.0),
      m_highlight(highlight),
      m_valueMap() {
    
    setFormat();
    setExpandable(master->isExpandable());
    setOpen(master->isOpen());
    //kdDebug()<<"DoubleListViewBase::SlaveListItem "<<master->text(0)<<" parent="<<static_cast<DoubleListViewBase::SlaveListItem*>(parent)->m_masterItem->text(0)<<endl;
}
DoubleListViewBase::SlaveListItem::~SlaveListItem() {
    //kdDebug()<<k_funcinfo<<endl;
    if (m_masterItem)
        m_masterItem->slaveItemDeleted();
}

void DoubleListViewBase::SlaveListItem::clearColumn(int col) {
    if (col >= listView()->columns()) {
        return;
    }
    listView()->setColumnText(col, "");
    setText(col, "");
    m_valueMap[col] = 0;
}
void DoubleListViewBase::SlaveListItem::setColumn(int col, double value) {
    if (col < listView()->columns()) {
        //setText(col, QString("%1").arg(value, m_fieldwidth, m_fmt, m_prec));
        setText(col, KGlobal::locale()->formatNumber(value, m_prec));
        m_valueMap.replace(col, value);
        //kdDebug()<<k_funcinfo<<m_masterItem->text(0)<<": column["<<col<<"]="<<value<<endl;
    }
}

void DoubleListViewBase::SlaveListItem::setLimit(int col, double limit) {
    m_limitMap[col] = limit;
}

void DoubleListViewBase::SlaveListItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align) {
    //kdDebug()<<k_funcinfo<<"c="<<column<<endl;
    QColorGroup g = cg;
    if (m_highlight) {
        if (m_limitMap.contains(column)) {
            if (m_valueMap[column] > m_limitMap[column]) {
                g.setColor(QColorGroup::Text, QColor(red));
            } else if (m_valueMap[column] < m_limitMap[column]) {
                g.setColor(QColorGroup::Text, QColor(green));
            }
        }
    }
    KListViewItem::paintCell(p, g, column, width, align);
}

void DoubleListViewBase::SlaveListItem::setFormat(int fieldwidth, char fmt, int prec) {
    m_fieldwidth = fieldwidth;
    m_fmt = fmt;
    m_prec = prec;
}

//----------------------------
DoubleListViewBase::MasterListItem::MasterListItem(QListView *parent, bool highlight)
    : KListViewItem(parent),
      m_slaveItem(0),
      m_value(0.0),
      m_limit(0.0),
      m_highlight(highlight) {
    
    setFormat();
    //kdDebug()<<k_funcinfo<<endl;
}

DoubleListViewBase::MasterListItem::MasterListItem(QListView *parent, QString text,   bool highlight)
    : KListViewItem(parent, text),
      m_slaveItem(0),
      m_value(0.0),
      m_limit(0.0),
      m_highlight(highlight) {

    setFormat();
    //kdDebug()<<k_funcinfo<<endl;
}

DoubleListViewBase::MasterListItem::MasterListItem(QListViewItem *parent, bool highlight)
    : KListViewItem(parent),
      m_slaveItem(0),
      m_value(0.0),
      m_limit(0.0),
      m_highlight(highlight) {
    
    setFormat();
    //kdDebug()<<k_funcinfo<<endl;
}

DoubleListViewBase::MasterListItem::MasterListItem(QListViewItem *parent, QString text,   bool highlight)
    : KListViewItem(parent, text),
      m_slaveItem(0),
      m_value(0.0),
      m_limit(0.0),
      m_highlight(highlight) {

    setFormat();
    //kdDebug()<<k_funcinfo<<endl;
}

DoubleListViewBase::MasterListItem::~MasterListItem() {
    if (m_slaveItem)
        m_slaveItem->masterItemDeleted();
}

void DoubleListViewBase::MasterListItem::createSlaveItems(QListView *lv, QListViewItem *after) {
    //kdDebug()<<k_funcinfo<<text(0)<<endl;
    if (m_slaveItem) {
        kdError()<<k_funcinfo<<"Slave item allready exists"<<endl;
    } else {
        if (parent() == 0) {
            m_slaveItem = new DoubleListViewBase::SlaveListItem(this, lv, after);
        } else {
            m_slaveItem = new DoubleListViewBase::SlaveListItem(this, static_cast<DoubleListViewBase::MasterListItem*>(parent())->m_slaveItem, after);
        }
    }
    DoubleListViewBase::SlaveListItem *prev = 0;
    for (QListViewItem *item = firstChild(); item; item = item->nextSibling()) {
        static_cast<DoubleListViewBase::MasterListItem*>(item)->createSlaveItems(lv, prev);
        prev = static_cast<DoubleListViewBase::MasterListItem*>(item)->m_slaveItem;
    }
}

void DoubleListViewBase::MasterListItem::setSlaveOpen(bool on) {
    if (m_slaveItem)
        m_slaveItem->setOpen(on);
}

void DoubleListViewBase::MasterListItem::slaveItemDeleted() {
    setTotal(0);
    m_slaveItem = 0;
}

void DoubleListViewBase::MasterListItem::setTotal(double tot) {
    m_value = tot;
    //setText(1, QString("%1").arg(tot, m_fieldwidth, m_fmt, m_prec));
    setText(1, KGlobal::locale()->formatNumber(tot, m_prec));
    //kdDebug()<<k_funcinfo<<text(0)<<"="<<tot<<endl;
}

void DoubleListViewBase::MasterListItem::addToTotal(double v) {
    m_value += v;
    //setText(1, QString("%1").arg(m_value, m_fieldwidth, m_fmt, m_prec));
    setText(1, KGlobal::locale()->formatNumber(m_value, m_prec));
}

double DoubleListViewBase::MasterListItem::calcTotal() {
    double tot=0.0;
    QListViewItem *item=firstChild();
    if (!item) {
        tot = m_value;
    } else {
        for (; item; item = item->nextSibling()) {
            tot += static_cast<DoubleListViewBase::MasterListItem*>(item)->calcTotal();
        }
    }
    setTotal(tot);
    return tot;
}

void DoubleListViewBase::MasterListItem::setSlaveItem(int col, double value) {
    if (m_slaveItem) {
        m_slaveItem->setColumn(col, value);
    }
}

void DoubleListViewBase::MasterListItem::clearColumn(int col) {
    for (QListViewItem *item=firstChild(); item; item=item->nextSibling()) {
        static_cast<DoubleListViewBase::MasterListItem*>(item)->clearColumn(col);
    }
    setTotal(0);
    if (m_slaveItem == 0) {
        kdError()<<k_funcinfo<<"No m_slaveItem"<<endl;
        return;
    }
    m_slaveItem->clearColumn(0);
}

void DoubleListViewBase::MasterListItem::calcSlaveItems() {
    if (m_slaveItem == 0 || m_slaveItem->listView() == 0) {
        kdError()<<k_funcinfo<<"No m_slaveItem or m_slaveItem->listView()"<<endl;
        return;
    }
    int cols = m_slaveItem->listView()->columns();
    for (int i = 0; i < cols; ++i) {
        calcSlaveItems(i);
    }
}

double DoubleListViewBase::MasterListItem::calcSlaveItems(int col) {
    if (m_slaveItem == 0)
        return 0.0;
    QListViewItem *item=firstChild();
    if (!item) {
        return m_slaveItem->value(col);
    }
    double tot=0.0;
    for (; item; item = item->nextSibling()) {
        tot += static_cast<DoubleListViewBase::MasterListItem*>(item)->calcSlaveItems(col);
    }
    //kdDebug()<<k_funcinfo<<text(0)<<" "<<col<<"="<<tot<<endl;
    setSlaveItem(col, tot);
    return tot;
}

void DoubleListViewBase::MasterListItem::setSlaveLimit(int col, double limit) {
    if (m_slaveItem) {
        m_slaveItem->setLimit(col, limit);
    }
}

void DoubleListViewBase::MasterListItem::setSlaveHighlight(bool on) {
    if (m_slaveItem) {
        m_slaveItem->setHighlight(on);
    }
}

void DoubleListViewBase::MasterListItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align) {
    //kdDebug()<<k_funcinfo<<"c="<<column<<" value="<<m_value<<" limit="<<m_limit<<endl;
    QColorGroup g = cg;
    if (column == 1 && m_highlight) {
        if (m_value > m_limit) {
            g.setColor(QColorGroup::Text, QColor(red));
        } else if (m_value < m_limit) {
            g.setColor(QColorGroup::Text, QColor(green));
        }
    }
    KListViewItem::paintCell(p, g, column, width, align);
}

void DoubleListViewBase::MasterListItem::setFormat(int fieldwidth, char fmt, int prec) {
    m_fieldwidth = fieldwidth;
    m_fmt = fmt;
    m_prec = prec;
}

//-------------------------------------
DoubleListViewBase::DoubleListViewBase(QWidget *parent, bool description)
    : QSplitter(parent),
      m_fieldwidth(0),
      m_fmt('f'),
      m_prec(0) {
    
    setOrientation(QSplitter::Horizontal);
    setHandleWidth(QMIN(2, handleWidth()));
    
    m_masterList = new ListView(this);
    m_masterList->setSelectionMode(QListView::NoSelection);
    m_masterList->setItemMargin(2);
    m_masterList->setRootIsDecorated(true);
#if KDE_IS_VERSION(3,3,9)
    m_masterList->setShadeSortColumn(false);
#endif
    m_masterList->setSortColumn(-1); // Disable sort!!
    m_masterList->addColumn(i18n("Name"));
    m_masterList->addColumn(i18n("Total"));
    m_masterList->setColumnAlignment(1, AlignRight);
    if (description) {
        m_masterList->addColumn(i18n("Description"));
        m_masterList->header()->moveSection(2, 1);
        m_masterList->header()->setStretchEnabled(true, 1);
    } else {
        m_masterList->header()->setStretchEnabled(true, 0);
    }
    m_masterList->setVScrollBarMode(QScrollView::AlwaysOff);
    m_masterList->setHScrollBarMode(QScrollView::AlwaysOn);
    
    m_slaveList = new ListView(this);
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

QSize DoubleListViewBase::sizeHint() const {
    //kdDebug()<<k_funcinfo<<minimumSizeHint()<<endl;
    return minimumSizeHint();  //HACK: koshell splitter problem
}

void DoubleListViewBase::clearSlaveList() {
    while (m_slaveList->columns() > 0) {
        m_slaveList->removeColumn(0); // removing the last one clears the list!!!
    }
    m_slaveList->clear(); // to be safe
}

void DoubleListViewBase::createSlaveItems() {
    clearSlaveList();
    DoubleListViewBase::SlaveListItem *prev = 0;
    for (QListViewItem *item = m_masterList->firstChild(); item; item = item->nextSibling()) {
        static_cast<DoubleListViewBase::MasterListItem*>(item)->createSlaveItems(m_slaveList, prev);
        prev = static_cast<DoubleListViewBase::MasterListItem*>(item)->slaveItem();
    }
}


void DoubleListViewBase::print(KPrinter &printer) {
    kdDebug()<<k_funcinfo<<endl;
    Q_UNUSED(printer);
}

void DoubleListViewBase::setOpen(QListViewItem *item, bool open) {
    //kdDebug()<<k_funcinfo<<endl;
    m_masterList->setOpen(item, open);
}

void DoubleListViewBase::slotExpanded(QListViewItem* item) {
    //kdDebug()<<k_funcinfo<<endl;
    if (item) {
        static_cast<DoubleListViewBase::MasterListItem*>(item)->setSlaveOpen(true);
    }
}

void DoubleListViewBase::slotCollapsed(QListViewItem*item) {
    //kdDebug()<<k_funcinfo<<endl;
    if (item) {
        static_cast<DoubleListViewBase::MasterListItem*>(item)->setSlaveOpen(false);
    }
}

void DoubleListViewBase::setDescriptionHeader(QString text) {
    m_masterList->setColumnText(1, text);
}

void DoubleListViewBase::setNameHeader(QString text) {
    m_masterList->setColumnText(0, text);
}

void DoubleListViewBase::setTotalHeader(QString text) {
    m_masterList->setColumnText(2, text);
}

void DoubleListViewBase::addSlaveColumn(QString text) {
    m_slaveList->addColumn(text);
    m_slaveList->setColumnAlignment(m_slaveList->columns()-1, AlignRight);
}

void DoubleListViewBase::calculate() {
    for (QListViewItem *lvi=m_masterList->firstChild(); lvi; lvi = lvi->nextSibling()) {
        static_cast<DoubleListViewBase::MasterListItem *>(lvi)->calcSlaveItems();
        static_cast<DoubleListViewBase::MasterListItem *>(lvi)->calcTotal();
    }
}

void DoubleListViewBase::clearLists() {
    m_slaveList->clear();
    m_masterList->clear();
}

void DoubleListViewBase::setMasterFormat(int fieldwidth, char fmt, int prec) {
    QListViewItemIterator it = m_masterList;
    for (; it.current(); ++it) {
        static_cast<DoubleListViewBase::MasterListItem*>(it.current())->setFormat(fieldwidth, fmt, prec);
    }
}
void DoubleListViewBase::setSlaveFormat(int fieldwidth, char fmt, int prec) {
    QListViewItemIterator it = m_slaveList;
    for (; it.current(); ++it) {
        static_cast<DoubleListViewBase::SlaveListItem*>(it.current())->setFormat(fieldwidth, fmt, prec);
    }
}

void DoubleListViewBase::setFormat(int fieldwidth, char fmt, int prec) {
    m_fieldwidth = fieldwidth;
    m_fmt = fmt;
    m_prec = prec;
    setMasterFormat(fieldwidth, fmt, prec);
    setSlaveFormat(fieldwidth, fmt, prec);
}

void DoubleListViewBase::paintContents(QPainter *p) {
    //kdDebug()<<k_funcinfo<<endl;
    QRect cm = m_masterList->contentsRect();
    QRect cs = m_slaveList->contentsRect();
    int mx, my, sx, sy;
    m_masterList->contentsToViewport(cm.x(), cm.y(), mx, my);
    m_slaveList->contentsToViewport(cs.x(), cs.y(), sx, sy);
    if (sizes()[0] > 0) {
        p->save();
        p->translate(mx, my);
        m_masterList->paintToPrinter(p, -mx, -my, cm.width(), cm.height());
        p->restore();
    }
    if (sizes()[1] > 0) {
        p->save();
        p->translate(cm.width() + 8 + sx, sy);
        m_slaveList->paintToPrinter(p, -sx, -sy, cs.width(), cs.height());
        //p->fillRect(-8, 0, 0, sy, Qt::white);
        p->restore();
    }
}

}  //KPlato namespace

#include "kptdoublelistviewbase.moc"
