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

#include "kptaccountsview.h"

#include "kptaccountsviewconfigdialog.h"
#include "kptcontext.h"
#include "kptdatetime.h"
#include "kptproject.h"
#include "kptview.h"
#include "kpteffortcostmap.h"

#include <qcombobox.h>
#include <qdatetime.h>
#include <qdatetimeedit.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qpushbutton.h>
#include <qsplitter.h>
#include <qvaluelist.h>
#include <qpopupmenu.h>
#include <qsizepolicy.h>
#include <qhbox.h>

#include <kcalendarsystem.h>
#include <kglobal.h>
#include <klistview.h>
#include <klocale.h>
#include <kprinter.h>

#include <kdebug.h>

namespace KPlato
{

AccountsView::AccountPeriodItem::AccountPeriodItem(AccountsView::AccountItem *o, QListView *parent, QListViewItem *after, bool _highlight)
    : KListViewItem(parent, after),
      owner(o),
      value(0.0),
      highlight(_highlight),
      costMap() {
    setExpandable(o->isExpandable());
    setOpen(o->isOpen());
    //kdDebug()<<k_funcinfo<<o->text(0)<<" toplevel"<<endl;
}
AccountsView::AccountPeriodItem::AccountPeriodItem(AccountsView::AccountItem *o, QListViewItem *parent, QListViewItem *after, bool _highlight)
    : KListViewItem(parent, after),
    owner(o),
    value(0.0),
    highlight(_highlight),
    costMap() {
    setExpandable(o->isExpandable());
    setOpen(o->isOpen());
    //kdDebug()<<"AccountPeriodItem "<<o->text(0)<<" parent="<<static_cast<AccountsView::AccountPeriodItem*>(parent)->owner->text(0)<<endl;
}
AccountsView::AccountPeriodItem::~AccountPeriodItem() {
    //kdDebug()<<k_funcinfo<<endl;
    if (owner)
        owner->periodDeleted();
}

void AccountsView::AccountPeriodItem::clearColumn(int col) {
    if (col >= listView()->columns()) {
        return;
    }
    listView()->setColumnText(col, "");
    setText(col, "");
    costMap[col] = 0;
}
void AccountsView::AccountPeriodItem::setColumn(int col, double cost) {
    if (col < listView()->columns()) {
        setText(col, KGlobal::locale()->formatMoney(cost, "", 0));
        costMap.replace(col, cost);
        //kdDebug()<<k_funcinfo<<owner->text(0)<<": period "<<col<<"="<<cost<<endl;
    }
}

void AccountsView::AccountPeriodItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align) {
    //kdDebug()<<k_funcinfo<<"c="<<column<<endl;
    QColorGroup g = cg;
    if (highlight) {
        if (value < 0.0) {
            g.setColor(QColorGroup::Text, QColor(red));
        } else if (value > 0.0) {
            g.setColor(QColorGroup::Text, QColor(green));
        }
    }
    KListViewItem::paintCell(p, g, column, width, align);
}

AccountsView::AccountItem::AccountItem(Account *a, QListView *parent, bool _highlight)
    : KListViewItem(parent, a->name(), a->description()),
      account(a),
      period(0),
      value(0.0),
      highlight(_highlight) {
    
    //kdDebug()<<k_funcinfo<<endl;
}
AccountsView::AccountItem::AccountItem(Account *a, QListViewItem *parent, bool _highlight)
    : KListViewItem(parent, a->name(), a->description()),
      account(a),
      period(0),
      value(0.0),
      highlight(_highlight) {
    
    //kdDebug()<<k_funcinfo<<endl;
}

AccountsView::AccountItem::AccountItem(QString text, Account *a, QListViewItem *parent, bool _highlight)
    : KListViewItem(parent, text),
      account(a),
      period(0),
      value(0.0),
      highlight(_highlight) {
    
    //kdDebug()<<k_funcinfo<<endl;
}
        
AccountsView::AccountItem::~AccountItem() {
    if (period)
        period->owner = 0;
}

void AccountsView::AccountItem::createPeriods(QListView *lv, QListViewItem *after) {
    //kdDebug()<<k_funcinfo<<text(0)<<endl;
    if (period) {
        kdError()<<k_funcinfo<<"Period allready exists"<<endl;
    }
    if (parent() == 0) {
        period = new AccountsView::AccountPeriodItem(this, lv, after);
    } else {
        period = new AccountsView::AccountPeriodItem(this, static_cast<AccountsView::AccountItem*>(parent())->period, after);
    }
    AccountsView::AccountPeriodItem *prev = 0;
    for (QListViewItem *item = firstChild(); item; item = item->nextSibling()) {
        static_cast<AccountsView::AccountItem*>(item)->createPeriods(lv, prev);
        prev = static_cast<AccountsView::AccountItem*>(item)->period;
    }

}
void AccountsView::AccountItem::periodDeleted() {
    setTotal(0);
    period = 0;
}

void AccountsView::AccountItem::setTotal(double tot) {
    value = tot;
    setText(2, KGlobal::locale()->formatMoney(value, "", 0));
    //kdDebug()<<k_funcinfo<<text(0)<<"="<<tot<<endl;
}

void AccountsView::AccountItem::addToTotal(double v) {
    value += v;
    setText(2, KGlobal::locale()->formatMoney(value, "", 0));
}

double AccountsView::AccountItem::calcTotal() {
    double tot=0.0;
    QListViewItem *item=firstChild();
    if (!item) {
        tot = value;
    } else {
        for (; item; item = item->nextSibling()) {
            tot += static_cast<AccountsView::AccountItem*>(item)->calcTotal();
        }
    }
    setTotal(tot);
    return tot;
}

void AccountsView::AccountItem::setPeriod(int col, double cost) {
    if (period) {
        period->setColumn(col, cost);
    }
}

void AccountsView::AccountItem::clearColumn(int col) {
    for (QListViewItem *item=firstChild(); item; item=item->nextSibling()) {
        static_cast<AccountsView::AccountItem*>(item)->clearColumn(col);
    }
    setTotal(0);
    if (period == 0) {
        kdError()<<k_funcinfo<<"No period"<<endl;
        return;
    }
    period->clearColumn(0);
}

void AccountsView::AccountItem::calcPeriods() {
    if (period == 0 || period->listView() == 0) {
        kdError()<<k_funcinfo<<"No period or period->listView()"<<endl;
        return;
    }
    int cols = period->listView()->columns();
    for (int i = 0; i < cols; ++i) {
        calcPeriod(i);
    }
}

double AccountsView::AccountItem::calcPeriod(int col) {
    if (period == 0)
        return 0.0;
    QListViewItem *item=firstChild();
    if (!item) {
        return period->costMap[col];
    }
    double tot=0.0;
    for (; item; item = item->nextSibling()) {
        tot += static_cast<AccountsView::AccountItem*>(item)->calcPeriod(col);
    }
    //kdDebug()<<k_funcinfo<<text(0)<<" "<<col<<"="<<tot<<endl;
    setPeriod(col, tot);
    return tot;
}

void AccountsView::AccountItem::add(int col, const QDate &date, const EffortCost &ec) {
    EffortCost &cm = costMap.add(date, ec);
    if (period)
        period->setText(col, KGlobal::locale()->formatMoney(cm.cost(), "", 0));
}

void AccountsView::AccountItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align) {
    //kdDebug()<<k_funcinfo<<"c="<<column<<" prio="<<(columnPrio.contains(column)?columnPrio[column]:0)<<endl;
    QColorGroup g = cg;
    if (highlight && column == 1) { //Total
        if (value < 0.0) {
            g.setColor(QColorGroup::Text, QColor(red));
        } else if (value > 0.0) {
            g.setColor(QColorGroup::Text, QColor(green));
        }
    }
    KListViewItem::paintCell(p, g, column, width, align);
}

AccountsView::AccountsView(Project &project, View *view, QWidget *parent)
    : QWidget(parent, "Accounts view"),
      m_mainview(view),
      m_project(project),
      m_accounts(project.accounts()) {
    
    m_date = QDate::currentDate();
    m_period = 0;
    m_periodTexts<<i18n("Day")<<i18n("Week")<<i18n("Month");
    m_cumulative = false;

    QVBoxLayout *lay1 = new QVBoxLayout(this, 11, 6);
    
    QHBoxLayout *lay2 = new QHBoxLayout(0, 0, 6);
    m_label = new QLabel(this);
    m_label->setFrameShape(QLabel::StyledPanel);
    m_label->setFrameShadow(QLabel::Sunken);
    m_label->setAlignment(int(QLabel::WordBreak | QLabel::AlignVCenter));
    lay2->addWidget(m_label);
    m_changeBtn = new QPushButton(i18n("Configure..."), this);
    m_changeBtn->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_changeBtn->sizePolicy().hasHeightForWidth()));
    lay2->addWidget(m_changeBtn);
    lay1->addLayout(lay2);

    m_splitter = new QSplitter(this);
    m_splitter->setOrientation(QSplitter::Horizontal);
    m_splitter->setHandleWidth(QMIN(2, m_splitter->handleWidth()));
    
    m_accList = new KListView(m_splitter, "Accounts list");
    m_accList->setSelectionMode(QListView::NoSelection);
    m_accList->setItemMargin(2);
    m_accList->setRootIsDecorated(true);
    m_accList->setSortColumn(-1); // Disable sort!!
    m_accList->addColumn(i18n("Account"));
    m_accList->addColumn(i18n("Description"));
    m_accList->setColumnAlignment(1, AlignLeft);
    m_accList->addColumn(i18n("Total"));
    m_accList->setColumnAlignment(2, AlignRight);
    m_accList->setVScrollBarMode(QScrollView::AlwaysOff);
    m_accList->setHScrollBarMode(QScrollView::AlwaysOn);
    m_accList->header()->setStretchEnabled(true, 1);
    
    m_periodList = new KListView(m_splitter, "Period list");
    m_periodList->setSelectionMode(QListView::NoSelection);
    m_periodList->setItemMargin(2);
    m_periodList->setSortColumn(-1); // Disable sort!!
    m_periodList->setTreeStepSize(0);
    m_periodList->setHScrollBarMode(QScrollView::AlwaysOn);
    
    init();
    
    lay1->addWidget(m_splitter);
    
    connect(m_periodList->verticalScrollBar(), SIGNAL(valueChanged(int)),
            m_accList->verticalScrollBar(), SLOT(setValue(int)));
    
    connect(m_accList, SIGNAL(expanded(QListViewItem*)), SLOT(slotExpanded(QListViewItem*)));
    connect(m_accList, SIGNAL(collapsed(QListViewItem*)), SLOT(slotCollapsed(QListViewItem*)));
    
    connect(this, SIGNAL(update()), SLOT(slotUpdate()));
    connect(m_changeBtn, SIGNAL(clicked()), SLOT(slotConfigure()));
    
    QValueList<int> list = m_splitter->sizes();
    int tot = list[0] + list[1];
    list[0] = QMIN(35, tot);
    list[1] = tot-list[0];
    m_splitter->setSizes(list);
}

void AccountsView::zoom(double zoom) {
    Q_UNUSED(zoom);
}

void AccountsView::init() {
    m_date = QDate::currentDate();
    m_period = 0;
    initAccList(m_accounts.accountList());
}

void AccountsView::draw() {
    //kdDebug()<<k_funcinfo<<endl;
    initAccList(m_accounts.accountList());
    slotUpdate();
}

void AccountsView::initAccList(const AccountList &list) {
    m_accList->clear();
    clearPeriods();
    AccountListIterator it = list;
    for (it.toLast(); it.current(); --it) {
        AccountsView::AccountItem *a = new AccountsView::AccountItem(it.current(), m_accList);
        AccountsView::AccountPeriodItem *i = new AccountsView::AccountPeriodItem(a, m_periodList, 0);
        a->period = i;
    
        initAccSubItems(it.current(), a);
    }
}

void AccountsView::initAccSubItems(Account *acc, AccountsView::AccountItem *parent) {
    if (!acc->accountList().isEmpty()) {
/*        AccountsView::AccountItem *a = new AccountsView::AccountItem(i18n("Subaccounts"), acc, parent);
        AccountsView::AccountPeriodItem *i = new AccountsView::AccountPeriodItem(a, parent->period);
        a->period = i;*/
    
        initAccList(acc->accountList(), parent);
    }
//     AccountsView::AccountItem *a = new AccountsView::AccountItem(i18n("Variance"), acc, parent, true);
//     AccountsView::AccountPeriodItem *i = new AccountsView::AccountPeriodItem(a, parent->period, true);
//     a->period = i;
// 
//     a = new AccountsView::AccountItem(i18n("Actual"), acc, parent);
//     i = new AccountsView::AccountPeriodItem(a, parent->period);
//     a->period = i;
// 
//     a = new AccountsView::AccountItem(i18n("Planned"), acc, parent);
//     i = new AccountsView::AccountPeriodItem(a, parent->period);
//     a->period = i;

}

void AccountsView::initAccList(const AccountList &list, AccountsView::AccountItem *parent) {
    AccountListIterator it = list;
    for (it.toLast(); it.current(); --it) {
        AccountsView::AccountItem *a = new AccountsView::AccountItem(it.current(), parent);
        AccountsView::AccountPeriodItem *i = new AccountsView::AccountPeriodItem(a, parent->period, 0);
        a->period = i;

        initAccSubItems(it.current(), a);
    }
}

void AccountsView::clearPeriods() {
    while (m_periodList->columns() > 0) {
        m_periodList->removeColumn(0); // removing the last one clears the list!!!
    }
    m_periodList->clear(); // to be safe
}

void AccountsView::createPeriods() {
    AccountsView::AccountPeriodItem *prev = 0;
    for (QListViewItem *item = m_accList->firstChild(); item; item = item->nextSibling()) {
        static_cast<AccountsView::AccountItem*>(item)->createPeriods(m_periodList, prev);
        prev = static_cast<AccountsView::AccountItem*>(item)->period;
    }
}

void AccountsView::slotUpdate() {
    //kdDebug()<<k_funcinfo<<endl;
    clearPeriods();
    createPeriods();
    KLocale *locale = KGlobal::locale();
    const KCalendarSystem *cal = locale->calendar();
    
    QString t;
    if (m_cumulative) {
        t += " <b>" + i18n("Cumulative") + "</b>  ";
    }
    t += i18n("Cut-off date:");
    t += "<b>" + locale->formatDate(m_date, true) + "</b>";
    t += " " + i18n("Periodicity:");
    t += "<b>" + periodText(m_period) + "</b>";
    m_label->setText(t);
    
    // Add columns for selected period/periods
    QDate start = m_project.startTime().date();
    QDate end = m_date;
    //kdDebug()<<k_funcinfo<<start.toString()<<" - "<<end.toString()<<endl;
    int c=0;
    if (m_period == 0) { //Daily
        for (QDate dt = start; dt <= end; dt = cal->addDays(dt, 1), ++c) {
            QString df = locale->formatDate(dt, true);
            m_periodList->addColumn(df);
            m_periodList->setColumnAlignment(c, AlignRight);
        }
        QListViewItemIterator it(m_accList);
        for (;it.current(); ++it) {
            AccountsView::AccountItem *item = dynamic_cast<AccountsView::AccountItem*>(it.current());
            if (!item || !item->account || !item->account->isElement()) {
                continue;
            }
            item->costMap = m_accounts.plannedCost(*(item->account), start, end);
            double cost = 0.0;
            int col=0;
            for (QDate d=start; d <= end; d = cal->addDays(d, 1), ++col) {
                EffortCost &ec = item->costMap.effortCostOnDate(d);
                cost = (m_cumulative ? cost + ec.cost() : ec.cost());
                item->setPeriod(col, cost);
                m_cumulative ? item->setTotal(cost) : item->addToTotal(cost);
            }
        }
        for (QListViewItem *lvi=m_accList->firstChild(); lvi; lvi = lvi->nextSibling()) {
            static_cast<AccountsView::AccountItem *>(lvi)->calcPeriods();
            static_cast<AccountsView::AccountItem *>(lvi)->calcTotal();
        }
       
        return;
    }
    if (m_period == 1) { //Weekly
        //TODO make this user controlled
        int weekStartDay = locale->weekStartDay();

        QDate dt = start;
        QDate pend = cal->addDays(dt, 7 + weekStartDay - 1 - cal->dayOfWeek(dt));
        for (; pend <= end; pend = cal->addDays(pend, 7), ++c) {
            int y;
            int w = cal->weekNumber(dt, &y);
            QString t = QString("%1-%2").arg(w).arg(y);
            m_periodList->addColumn(t);
            m_periodList->setColumnAlignment(c, AlignRight);
        
            dt = pend.addDays(1);
        }
        if (c == 0)
            return;
       
        QListViewItemIterator it(m_accList);
        for (;it.current(); ++it) {
            AccountsView::AccountItem *item = dynamic_cast<AccountsView::AccountItem*>(it.current());
            if (!item || !item->account || !item->account->isElement()) {
                continue;
            }
            item->costMap = m_accounts.plannedCost(*(item->account), start, end);
            double cost = 0.0;
            QDate d = start;
            QDate pend = cal->addDays(d, 7 + weekStartDay - 1 - cal->dayOfWeek(d));
            for (int col=0; pend <= end; pend = cal->addDays(pend, 7), ++col) {
                double cst = item->costMap.cost(d, d.daysTo(pend)+1);
                cost = (m_cumulative ? cost + cst : cst);
                item->setPeriod(col, cost);
                m_cumulative ? item->setTotal(cost) : item->addToTotal(cost);
                d = pend.addDays(1);
            }
        }
        for (QListViewItem *lvi=m_accList->firstChild(); lvi; lvi = lvi->nextSibling()) {
            static_cast<AccountsView::AccountItem *>(lvi)->calcPeriods();
            static_cast<AccountsView::AccountItem *>(lvi)->calcTotal();
        }
       
        return;
    }
    if (m_period == 2) { //Monthly
        //TODO make this user controlled
        QDate dt = start;
        QDate pend; 
        cal->setYMD(pend, dt.year(), dt.month(), dt.daysInMonth());
        for (; pend <= end; pend = cal->addDays(pend, dt.daysInMonth()), ++c) {
            //kdDebug()<<k_funcinfo<<dt.toString()<<"-"<<pend.toString()<<endl;
            QString m = cal->monthName(dt, true) + QString(" %1").arg( dt.year());
            m_periodList->addColumn(m);
            m_periodList->setColumnAlignment(c, AlignRight);
        
            dt = pend.addDays(1);
        }
        if (c == 0)
            return;
       
        QListViewItemIterator it(m_accList);
        for (;it.current(); ++it) {
            AccountsView::AccountItem *item = dynamic_cast<AccountsView::AccountItem*>(it.current());
            if (!item || !item->account || !item->account->isElement()) {
                continue;
            }
            item->costMap = m_accounts.plannedCost(*(item->account), start, end);
            double cost = 0.0;
            QDate d = start;
            cal->setYMD(pend, d.year(), d.month(), d.daysInMonth());
            for (int col=0; pend <= end; pend = cal->addDays(pend, d.daysInMonth()), ++col) {
                double cst = item->costMap.cost(d, d.daysTo(pend)+1);
                cost = (m_cumulative ? cost + cst : cst);
                item->setPeriod(col, cost);
                m_cumulative ? item->setTotal(cost) : item->addToTotal(cost);
                d = pend.addDays(1);
            }
        }
        for (QListViewItem *lvi=m_accList->firstChild(); lvi; lvi = lvi->nextSibling()) {
            static_cast<AccountsView::AccountItem *>(lvi)->calcPeriods();
            static_cast<AccountsView::AccountItem *>(lvi)->calcTotal();
        }
        return;
    }
}


void AccountsView::print(KPrinter &printer) {
    kdDebug()<<k_funcinfo<<endl;
    Q_UNUSED(printer);
}

bool AccountsView::setContext(Context::Accountsview &context) {
    //kdDebug()<<k_funcinfo<<endl;
    
    QValueList<int> list;
    list << context.accountsviewsize << context.periodviewsize;
    m_splitter->setSizes(list);
    m_date = context.date;
    if (!m_date.isValid())
        m_date = QDate::currentDate();
    m_period = context.period;
    m_cumulative = context.cumulative;
    
    return true;
}

void AccountsView::getContext(Context::Accountsview &context) const {
    //kdDebug()<<k_funcinfo<<endl;
    context.accountsviewsize = m_splitter->sizes()[0];
    context.periodviewsize = m_splitter->sizes()[1];
    context.date = m_date;
    context.period = m_period;
    context.cumulative = m_cumulative;
    //kdDebug()<<k_funcinfo<<"sizes="<<sizes()[0]<<","<<sizes()[1]<<endl;
}

void AccountsView::slotExpanded(QListViewItem* item) {
    if (item) {
        static_cast<AccountsView::AccountItem*>(item)->period->setOpen(true);
    }
}

void AccountsView::slotCollapsed(QListViewItem*item) {
    if (item) {
        static_cast<AccountsView::AccountItem*>(item)->period->setOpen(false);
    }
}

void AccountsView::slotConfigure() {
    //kdDebug()<<k_funcinfo<<endl;
    AccountsviewConfigDialog *dia = new AccountsviewConfigDialog(m_date, m_period, m_periodTexts, m_cumulative, this);
    if (dia->exec()) {
        m_date = dia->date();
        m_period = dia->period();
        m_cumulative = dia->isCumulative();
        emit update();
    }
    delete dia;
}

QString AccountsView::periodText(int offset) {
    QString s;
    QStringList::const_iterator it = m_periodTexts.at(offset);
    if (it != m_periodTexts.constEnd()) {
        s = (*it);
    }
    return s;
}

}  //KPlato namespace

#include "kptaccountsview.moc"
