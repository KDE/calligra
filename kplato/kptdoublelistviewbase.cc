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

#include <QPainter>
#include <QLayout>
#include <QMap>
#include <QSplitter>
#include <QString>
#include <QList>
#include <QPoint>
#include <QTreeWidgetItem>
#include <QRect>
#include <QScrollBar>
#include <QHeaderView>

#include <kcalendarsystem.h>
#include <kglobal.h>
#include <klocale.h>
#include <kprinter.h>


#include <kdebug.h>

namespace KPlato
{
void ListView::paintToPrinter(QPainter * p, int cx, int cy, int cw, int ch) {
#if 0
    //kDebug()<<k_funcinfo<<QRect(cx, cy, cw, ch)<<endl;
    // draw header labels
    p->save();
    QRegion r = p->clipRegion();
    p->setClipRegion(r.intersect(QRegion(cx, 0, cw, ch)));
    QColor bgc(193, 223, 255);
    QBrush bg(bgc);
    p->setBackgroundMode(Qt::OpaqueMode);
    p->setBackgroundColor(bgc);
    Q3Header *h = header();
    int hei = 0;
    for (int s = 0; s < h->count(); ++s) {
        QRect r = h->sectionRect(s);
        //kDebug()<<s<<": "<<h->label(s)<<" "<<r<<endl;
        int x, y;
        viewportToContents(r.x(), r.y(), x, y);
        QRect sr(x, y, r.width(), r.height());
        //kDebug()<<s<<": "<<h->label(s)<<" "<<sr<<endl;
        if (sr.x()+sr.width() <= cx || sr.x() >= cx+cw) {
            //kDebug()<<s<<": "<<h->label(s)<<" "<<sr<<": continue"<<endl;
            continue;
        }
        QRect tr = sr;
        if (sr.x() < cx) {
            tr.setX(cx);
            //kDebug()<<s<<": "<<h->label(s)<<" "<<tr<<endl;
        }
        p->eraseRect(tr);
        p->drawText(tr, columnAlignment(s)|Qt::AlignVCenter, h->label(s), -1);
        hei = qMax(tr.height(), hei);
    }
    r = p->clipRegion();
    p->restore();
//    p->drawRect(r.boundingRect());
    p->save();
    p->translate(0, hei+2);
    r = p->clipRegion();
    // FIXME: Doesn't clip correctly, haven't figured out why
    p->setClipRegion(r.intersect(QRegion(cx, cy, cw, ch)));
    drawContentsOffset(p, 0, 0, cx, cy, cw, ch);
//    p->drawRect(r.boundingRect());
    p->restore();
#endif
}

DoubleListViewBase::SlaveListItem::SlaveListItem(DoubleListViewBase::MasterListItem *master, QTreeWidget *parent, QTreeWidgetItem *after, bool highlight)
    : QTreeWidgetItem(parent, after),
      m_masterItem(master),
      m_value(0.0),
      m_highlight(highlight),
      m_valueMap() {

    setFormat();
//    setExpandable(master->isExpandable());
//    setOpen(master->isOpen());
    //kDebug()<<"DoubleListViewBase::SlaveListItem "<<master->text(0)<<" parent="<<static_cast<DoubleListViewBase::SlaveListItem*>(parent)->m_masterItem->text(0)<<endl;
}
DoubleListViewBase::SlaveListItem::SlaveListItem(DoubleListViewBase::MasterListItem *master, QTreeWidgetItem *parent, QTreeWidgetItem *after, bool highlight)
    : QTreeWidgetItem(parent, after),
      m_masterItem(master),
      m_value(0.0),
      m_highlight(highlight),
      m_valueMap() {

    setFormat();
/*    setExpandable(master->isExpandable());
    setOpen(master->isOpen());*/
    //kDebug()<<"DoubleListViewBase::SlaveListItem "<<master->text(0)<<" parent="<<static_cast<DoubleListViewBase::SlaveListItem*>(parent)->m_masterItem->text(0)<<endl;
}
DoubleListViewBase::SlaveListItem::~SlaveListItem() {
    //kDebug()<<k_funcinfo<<endl;
    if (m_masterItem)
        m_masterItem->slaveItemDeleted();
}

void DoubleListViewBase::SlaveListItem::clearColumn(int col) {
    if (col >= columnCount()) {
        return;
    }
    treeWidget()->headerItem()->setText(col, "");
    setText(col, "");
    m_valueMap[col] = 0;
}
void DoubleListViewBase::SlaveListItem::setColumn(int col, double value) {
    //kDebug()<<k_funcinfo<<columnCount()<<", "<<col<<endl;
    setTextAlignment(col, Qt::AlignRight);
    setText(col, KGlobal::locale()->formatNumber(value, m_prec));
    m_valueMap.replace(col, value);
    //kDebug()<<k_funcinfo<<m_masterItem->text(0)<<": column["<<col<<"]="<<value<<endl;
}

void DoubleListViewBase::SlaveListItem::setLimit(int col, double limit) {
    m_limitMap[col] = limit;
}

void DoubleListViewBase::SlaveListItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align) {
    //kDebug()<<k_funcinfo<<"c="<<column<<endl;
    QColorGroup g = cg;
    if (m_highlight) {
        if (m_limitMap.contains(column)) {
            if (m_valueMap[column] > m_limitMap[column]) {
                g.setColor(QColorGroup::Text, QColor(Qt::red));
            } else if (m_valueMap[column] < m_limitMap[column]) {
                g.setColor(QColorGroup::Text, QColor(Qt::green));
            }
        }
    }
    //QTreeWidgetItem::paintCell(p, g, column, width, align);
}

void DoubleListViewBase::SlaveListItem::setFormat(int fieldwidth, char fmt, int prec) {
    m_fieldwidth = fieldwidth;
    m_fmt = fmt;
    m_prec = prec;
}

//----------------------------
DoubleListViewBase::MasterListItem::MasterListItem(QTreeWidget *parent, bool highlight)
    : QTreeWidgetItem(parent),
      m_slaveItem(0),
      m_value(0.0),
      m_limit(0.0),
      m_highlight(highlight) {

    setFormat();
    //kDebug()<<k_funcinfo<<endl;
}

DoubleListViewBase::MasterListItem::MasterListItem(QTreeWidget *parent, QString text,   bool highlight)
    : QTreeWidgetItem(parent),
      m_slaveItem(0),
      m_value(0.0),
      m_limit(0.0),
      m_highlight(highlight) {
    setText(0, text);
    setFormat();
    //kDebug()<<k_funcinfo<<endl;
}

DoubleListViewBase::MasterListItem::MasterListItem(QTreeWidgetItem *parent, bool highlight)
    : QTreeWidgetItem(parent),
      m_slaveItem(0),
      m_value(0.0),
      m_limit(0.0),
      m_highlight(highlight) {

    setFormat();
    //kDebug()<<k_funcinfo<<endl;
}

DoubleListViewBase::MasterListItem::MasterListItem(QTreeWidgetItem *parent, QString text,   bool highlight)
    : QTreeWidgetItem(parent),
      m_slaveItem(0),
      m_value(0.0),
      m_limit(0.0),
      m_highlight(highlight) {

    setFormat();
    setText(0, text);
    //kDebug()<<k_funcinfo<<endl;
}

DoubleListViewBase::MasterListItem::~MasterListItem() {
    if (m_slaveItem)
        m_slaveItem->masterItemDeleted();
}

void DoubleListViewBase::MasterListItem::createSlaveItems(QTreeWidget *lv, QTreeWidgetItem *after) {
    //kDebug()<<k_funcinfo<<text(0)<<endl;
    if (m_slaveItem) {
        kError()<<k_funcinfo<<"Slave item already exists"<<endl;
    } else {
        if (parent() == 0) {
            m_slaveItem = new DoubleListViewBase::SlaveListItem(this, lv, after);
        } else {
            m_slaveItem = new DoubleListViewBase::SlaveListItem(this, static_cast<DoubleListViewBase::MasterListItem*>(parent())->m_slaveItem, after);
        }
    }
    DoubleListViewBase::SlaveListItem *prev = 0;
    QTreeWidgetItem *item = 0;
    for (int i=0; (item = child(i)) != 0; ++i) {
        static_cast<DoubleListViewBase::MasterListItem*>(item)->createSlaveItems(lv, prev);
        prev = static_cast<DoubleListViewBase::MasterListItem*>(item)->m_slaveItem;
    }
}

void DoubleListViewBase::MasterListItem::setSlaveOpen(bool on) {
    if (m_slaveItem)
        m_slaveItem->setExpanded(on);
}

void DoubleListViewBase::MasterListItem::slaveItemDeleted() {
    setTotal(0);
    m_slaveItem = 0;
}

void DoubleListViewBase::MasterListItem::setTotal(double tot) {
    m_value = tot;
    //setText(1, QString("%1").arg(tot, m_fieldwidth, m_fmt, m_prec));
    setTextAlignment(1, Qt::AlignRight);
    setText(1, KGlobal::locale()->formatNumber(tot, m_prec));
    //kDebug()<<k_funcinfo<<text(0)<<"="<<tot<<endl;
}

void DoubleListViewBase::MasterListItem::addToTotal(double v) {
    m_value += v;
    //setText(1, QString("%1").arg(m_value, m_fieldwidth, m_fmt, m_prec));
    setText(1, KGlobal::locale()->formatNumber(m_value, m_prec));
}

double DoubleListViewBase::MasterListItem::calcTotal() {
    //kDebug()<<k_funcinfo<<endl;
    double tot=0.0;
    QTreeWidgetItem *item=child(0);
    if (!item) {
        tot = m_value;
    } else {
        for (int i=0; (item = child(i)) != 0; ++i) {
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
    QTreeWidgetItem *item=0;
    for (int i=0; (item = child(i)) != 0; ++i) {
        static_cast<DoubleListViewBase::MasterListItem*>(item)->clearColumn(col);
    }
    setTotal(0);
    if (m_slaveItem == 0) {
        kError()<<k_funcinfo<<"No m_slaveItem"<<endl;
        return;
    }
    m_slaveItem->clearColumn(0);
}

void DoubleListViewBase::MasterListItem::calcSlaveItems() {
    if (m_slaveItem == 0 || m_slaveItem->treeWidget() == 0) {
        kError()<<k_funcinfo<<"No m_slaveItem or m_slaveItem->treeWidget()"<<endl;
        return;
    }
    int cols = m_slaveItem->columnCount();
    for (int i = 0; i < cols; ++i) {
        calcSlaveItems(i);
    }
}

double DoubleListViewBase::MasterListItem::calcSlaveItems(int col) {
    //kDebug()<<k_funcinfo<<col<<endl;
    if (m_slaveItem == 0)
        return 0.0;
    QTreeWidgetItem *item=child(0);
    if (!item) {
        return m_slaveItem->value(col);
    }
    double tot=0.0;
    for (int i=0; (item = child(i)) != 0; ++i) {
        tot += static_cast<DoubleListViewBase::MasterListItem*>(item)->calcSlaveItems(col);
    }
    //kDebug()<<k_funcinfo<<text(0)<<" "<<col<<"="<<tot<<endl;
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
    //kDebug()<<k_funcinfo<<"c="<<column<<" value="<<m_value<<" limit="<<m_limit<<endl;
    QColorGroup g = cg;
    if (column == 1 && m_highlight) {
        if (m_value > m_limit) {
            g.setColor(QColorGroup::Text, QColor(Qt::red));
        } else if (m_value < m_limit) {
            g.setColor(QColorGroup::Text, QColor(Qt::green));
        }
    }
//    QTreeWidgetItem::paintCell(p, g, column, width, align);
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

    setOrientation(Qt::Horizontal);
    setHandleWidth(qMin(2, handleWidth()));

    m_masterList = new ListView(this);
    QHeaderView *h = m_masterList->header();
    m_masterList->setSortingEnabled(false);
    m_masterList->setSelectionMode(QAbstractItemView::NoSelection);
    m_masterList->setHeaderLabels(QStringList()<<i18n("Name")<<i18n("Total")<<i18n("Description"));
    m_masterList->headerItem()->setTextAlignment(1, Qt::AlignRight);
    h->moveSection(2, 1);
    h->setResizeMode(2, QHeaderView::Stretch);
    h->setSectionHidden(2, !description);
    
    m_masterList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_masterList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    m_slaveList = new ListView(this);
    m_slaveList->header()->setDefaultAlignment(Qt::AlignRight);
    m_slaveList->setItemsExpandable(false);
    m_slaveList->setSelectionMode(QAbstractItemView::NoSelection);
    m_slaveList->setRootIsDecorated(false);
    m_slaveList->setIndentation(0);
    int left, top, right, bottom;
    m_slaveList->getContentsMargins(&left, &top, &right, &bottom);
    m_slaveList->setContentsMargins(2, top, right, bottom);
    
    m_slaveList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    connect(m_slaveList->verticalScrollBar(), SIGNAL(valueChanged(int)),  m_masterList->verticalScrollBar(), SLOT(setValue(int)));

    connect(m_masterList, SIGNAL(itemExpanded(QTreeWidgetItem*)), SLOT(slotExpanded(QTreeWidgetItem*)));
    connect(m_masterList, SIGNAL(itemCollapsed(QTreeWidgetItem*)), SLOT(slotCollapsed(QTreeWidgetItem*)));

}

QSize DoubleListViewBase::sizeHint() const {
    //kDebug()<<k_funcinfo<<minimumSizeHint()<<endl;
    return minimumSizeHint();  //HACK: koshell splitter problem
}

void DoubleListViewBase::clearSlaveList() {
/*    while (m_slaveList->columnCount() > 0) {
        m_slaveList->removeColumn(0); // removing the last one clears the list!!!
    }*/
    m_slaveList->clear();
}

void DoubleListViewBase::createSlaveItems() {
    clearSlaveList();
    DoubleListViewBase::SlaveListItem *prev = 0;
    QTreeWidgetItem *item=0;
    for (int i=0; (item=m_masterList->topLevelItem(i)) != 0; ++i) {
        static_cast<DoubleListViewBase::MasterListItem*>(item)->createSlaveItems(m_slaveList, prev);
        prev = static_cast<DoubleListViewBase::MasterListItem*>(item)->slaveItem();
    }
}


void DoubleListViewBase::print(KPrinter &printer) {
    kDebug()<<k_funcinfo<<endl;
    Q_UNUSED(printer);
}

void DoubleListViewBase::setOpen(QTreeWidgetItem *item, bool open) {
    //kDebug()<<k_funcinfo<<endl;
//    m_masterList->setOpen(item, open);
}

void DoubleListViewBase::slotExpanded(QTreeWidgetItem* item) {
    //kDebug()<<k_funcinfo<<endl;
    if (item) {
        static_cast<DoubleListViewBase::MasterListItem*>(item)->setSlaveOpen(true);
    }
}

void DoubleListViewBase::slotCollapsed(QTreeWidgetItem*item) {
    //kDebug()<<k_funcinfo<<endl;
    if (item) {
        static_cast<DoubleListViewBase::MasterListItem*>(item)->setSlaveOpen(false);
    }
}

void DoubleListViewBase::setDescriptionHeader(QString text) {
    m_masterList->headerItem()->setText(1, text);
}

void DoubleListViewBase::setNameHeader(QString text) {
    m_masterList->headerItem()->setText(0, text);
}

void DoubleListViewBase::setTotalHeader(QString text) {
    m_masterList->headerItem()->setText(2, text);
}

void DoubleListViewBase::setSlaveLabels(const QStringList &text) {
    m_slaveList->setHeaderLabels(text);
}

void DoubleListViewBase::calculate() {
    QTreeWidgetItem *lvi=0;
    for (int i=0; (lvi=m_masterList->topLevelItem(i)) != 0; ++i) {
        static_cast<DoubleListViewBase::MasterListItem *>(lvi)->calcSlaveItems();
        static_cast<DoubleListViewBase::MasterListItem *>(lvi)->calcTotal();
    }
}

void DoubleListViewBase::clearLists() {
    m_slaveList->clear();
    m_masterList->clear();
}

void DoubleListViewBase::setMasterFormat(int fieldwidth, char fmt, int prec) {
    foreach (QTreeWidgetItem *i, masterItems()) {
        static_cast<DoubleListViewBase::MasterListItem*>(i)->setFormat(fieldwidth, fmt, prec);
    }
}
void DoubleListViewBase::setSlaveFormat(int fieldwidth, char fmt, int prec) {
    foreach (QTreeWidgetItem *i, slaveItems()) {
        static_cast<DoubleListViewBase::SlaveListItem*>(i)->setFormat(fieldwidth, fmt, prec);
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

    kDebug()<<k_funcinfo<<endl;
#if 0
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
#endif
}

}  //KPlato namespace

#include "kptdoublelistviewbase.moc"
