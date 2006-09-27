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

#include <qapplication.h>
#include <QComboBox>
#include <qdatetime.h>
#include <q3datetimeedit.h>
#include <q3header.h>
#include <QLabel>
#include <QLayout>
#include <qpainter.h>
#include <qpalette.h>
#include <QPushButton>
#include <q3valuelist.h>
#include <q3popupmenu.h>
#include <qsizepolicy.h>
#include <q3hbox.h>
#include <q3paintdevicemetrics.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

#include <kcalendarsystem.h>
#include <kglobal.h>
#include <k3listview.h>
#include <klocale.h>
#include <kprinter.h>

#include <kdebug.h>

namespace KPlato
{

AccountsView::AccountItem::AccountItem(Account *a, Q3ListView *parent, bool highlight)
    : DoubleListViewBase::MasterListItem(parent, a->name(), highlight),
      account(a) {
    if (parent->columns() >= 3) {
        setText(2, a->description());
    }
    //kDebug()<<k_funcinfo<<endl;
}
AccountsView::AccountItem::AccountItem(Account *a, Q3ListViewItem *p, bool highlight)
    : DoubleListViewBase::MasterListItem(p, a->name(), highlight),
      account(a) {
    if (listView() && listView()->columns() >= 3) {
        setText(2, a->description());
    }
    //kDebug()<<k_funcinfo<<endl;
}

AccountsView::AccountItem::AccountItem(QString text, Account *a, Q3ListViewItem *parent, bool highlight)
    : DoubleListViewBase::MasterListItem(parent, text, highlight),
      account(a) {
    //kDebug()<<k_funcinfo<<endl;
}

void AccountsView::AccountItem::add(int col, const QDate &date, const EffortCost &ec) {
    EffortCost &cm = costMap.add(date, ec);
    if (m_slaveItem)
        m_slaveItem->setText(col, KGlobal::locale()->formatMoney(cm.cost(), "", 0));
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

    Q3VBoxLayout *lay1 = new Q3VBoxLayout(this, 0, KDialog::spacingHint());

    Q3HBoxLayout *lay2 = new Q3HBoxLayout(0, 0, KDialog::spacingHint());
    m_label = new QLabel(this);
    m_label->setFrameShape(QLabel::StyledPanel);
    m_label->setFrameShadow(QLabel::Sunken);
    m_label->setAlignment(int(Qt::TextWordWrap | Qt::AlignVCenter));
    lay2->addWidget(m_label);
    m_changeBtn = new QPushButton(i18n("Configure..."), this);
    m_changeBtn->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_changeBtn->sizePolicy().hasHeightForWidth()));
    lay2->addWidget(m_changeBtn);
    lay1->addLayout(lay2);

    m_dlv = new DoubleListViewBase(this, true);
    m_dlv->setNameHeader(i18n("Account"));

    init();

    lay1->addWidget(m_dlv);

    connect(this, SIGNAL(update()), SLOT(slotUpdate()));
    connect(m_changeBtn, SIGNAL(clicked()), SLOT(slotConfigure()));

    Q3ValueList<int> list = m_dlv->sizes();
    int tot = list[0] + list[1];
    list[0] = qMin(35, tot);
    list[1] = tot-list[0];
    //m_dlv->setSizes(list);
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
    //kDebug()<<k_funcinfo<<endl;
    Context::Accountsview context;
    getContextClosedItems(context, m_dlv->masterListView()->firstChild());
    initAccList(m_accounts.accountList());
    setContextClosedItems(context);
    slotUpdate();
}

void AccountsView::initAccList(const AccountList &list) {
    m_dlv->clearLists();
    AccountListIterator it = list;
    for (it.toLast(); it.current(); --it) {
        AccountsView::AccountItem *a = new AccountsView::AccountItem(it.current(), m_dlv->masterListView());
        a->setOpen(true);
        a->setExpandable(!it.current()->isElement());
        initAccSubItems(it.current(), a);
    }
    createPeriods();
}

void AccountsView::initAccSubItems(Account *acc, AccountsView::AccountItem *parent) {
    if (!acc->accountList().isEmpty()) {
/*        AccountsView::AccountItem *a = new AccountsView::AccountItem(i18n("Subaccounts"), acc, parent);
        DoubleListViewBase::SlaveListItem *i = new DoubleListViewBase::SlaveListItem(a, parent->period);
        a->period = i;*/

        initAccList(acc->accountList(), parent);
    }
//     AccountsView::AccountItem *a = new AccountsView::AccountItem(i18n("Variance"), acc, parent, true);
//     DoubleListViewBase::SlaveListItem *i = new DoubleListViewBase::SlaveListItem(a, parent->period, true);
//     a->period = i;
//
//     a = new AccountsView::AccountItem(i18n("Actual"), acc, parent);
//     i = new DoubleListViewBase::SlaveListItem(a, parent->period);
//     a->period = i;
//
//     a = new AccountsView::AccountItem(i18n("Planned"), acc, parent);
//     i = new DoubleListViewBase::SlaveListItem(a, parent->period);
//     a->period = i;

}

void AccountsView::initAccList(const AccountList &list, AccountsView::AccountItem *parent) {
    AccountListIterator it = list;
    for (it.toLast(); it.current(); --it) {
        AccountsView::AccountItem *a = new AccountsView::AccountItem(it.current(), parent);
        a->setOpen(true);
        a->setExpandable(!it.current()->isElement());
        initAccSubItems(it.current(), a);
    }
}

void AccountsView::clearPeriods() {
    m_dlv->clearSlaveList();
}

void AccountsView::createPeriods() {
    m_dlv->createSlaveItems();
}

void AccountsView::slotUpdate() {
    //kDebug()<<k_funcinfo<<endl;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    createPeriods();
    KLocale *locale = KGlobal::locale();
    const KCalendarSystem *cal = locale->calendar();

    QString t;
    if (m_cumulative) {
        t += " <b>" + i18n("Cumulative") + "</b>  ";
    }
    t += i18n("Cut-off date:%1", "<b>" + locale->formatDate(m_date, true) + "</b>");
    t += " " + i18n("Periodicity:%1", "<b>" + periodText(m_period) + "</b>");
    m_label->setText(t);

    // Add columns for selected period/periods
    QDate start = m_project.startTime().date();
    QDate end = m_date;
    //kDebug()<<k_funcinfo<<start.toString()<<" - "<<end.toString()<<endl;
    int c=0;
    if (m_period == 0) { //Daily
        for (QDate dt = start; dt <= end; dt = cal->addDays(dt, 1), ++c) {
            QString df = locale->formatDate(dt, true);
            m_dlv->addSlaveColumn(df);
        }
        Q3ListViewItemIterator it(m_dlv->masterListView());
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
                item->setSlaveItem(col, cost);
                m_cumulative ? item->setTotal(cost) : item->addToTotal(cost);
            }
        }
        m_dlv->calculate();
        QApplication::restoreOverrideCursor();
        return;
    }
    if (m_period == 1) { //Weekly
        //TODO make this user controlled
        int weekStartDay = locale->weekStartDay();

        QDate dt = start;
        QDate pend = cal->addDays(dt, 7 + weekStartDay - 1 - cal->dayOfWeek(dt));
        for (; pend <= end; ++c) {
            //kDebug()<<k_funcinfo<<c<<": "<<dt<<"-"<<pend<<" : "<<end<<endl;
            int y;
            int w = cal->weekNumber(dt, &y);
            QString t = i18nc("<week>-<year>", "%1-%2", w, y);
            m_dlv->addSlaveColumn(t);
            dt = pend.addDays(1);
            pend = cal->addDays(pend, 7);
            if ((pend.year() == end.year()) && (pend.weekNumber() == end.weekNumber())) {
                pend = end;
            }
        }
        if (c == 0) {
            QApplication::restoreOverrideCursor();
            return;
        }
        Q3ListViewItemIterator it(m_dlv->masterListView());
        for (;it.current(); ++it) {
            AccountsView::AccountItem *item = dynamic_cast<AccountsView::AccountItem*>(it.current());
            if (!item || !item->account || !item->account->isElement()) {
                continue;
            }
            item->costMap = m_accounts.plannedCost(*(item->account), start, end);
            double cost = 0.0;
            QDate d = start;
            QDate pend = cal->addDays(d, 7 + weekStartDay - 1 - cal->dayOfWeek(d));
            for (int col=0; pend <= end; ++col) {
                double cst = item->costMap.cost(d, d.daysTo(pend)+1);
                cost = (m_cumulative ? cost + cst : cst);
                item->setSlaveItem(col, cost);
                m_cumulative ? item->setTotal(cost) : item->addToTotal(cost);
                d = pend.addDays(1); // 1. next week
                pend = cal->addDays(pend, 7);
                if ((pend.year() == end.year()) && (pend.weekNumber() == end.weekNumber())) {
                    pend = end;
                }
            }
        }
        m_dlv->calculate();
        QApplication::restoreOverrideCursor();
        return;
    }
    if (m_period == 2) { //Monthly
        //TODO make this user controlled
        QDate dt = start;
        QDate pend;
        cal->setYMD(pend, dt.year(), dt.month(), dt.daysInMonth());
        for (; pend <= end; ++c) {
            //kDebug()<<k_funcinfo<<c<<": "<<dt<<"-"<<pend<<" : "<<end<<endl;
            QString m = cal->monthName(dt, true) + QString(" %1").arg( dt.year());
            m_dlv->addSlaveColumn(m);

            dt = pend.addDays(1); // 1. next month
            pend = cal->addDays(pend, dt.daysInMonth());
            if ((pend.year() == end.year()) && (pend.month() == end.month())) {
                pend = end;
            }
        }
        if (c == 0) {
            QApplication::restoreOverrideCursor();
            return;
        }
        Q3ListViewItemIterator it(m_dlv->masterListView());
        for (;it.current(); ++it) {
            AccountsView::AccountItem *item = dynamic_cast<AccountsView::AccountItem*>(it.current());
            if (!item || !item->account || !item->account->isElement()) {
                continue;
            }
            item->costMap = m_accounts.plannedCost(*(item->account), start, end);
            double cost = 0.0;
            QDate d = start;
            cal->setYMD(pend, d.year(), d.month(), d.daysInMonth());
            for (int col=0; pend <= end; ++col) {
                double cst = item->costMap.cost(d, d.daysTo(pend)+1);
                cost = (m_cumulative ? cost + cst : cst);
                item->setSlaveItem(col, cost);
                m_cumulative ? item->setTotal(cost) : item->addToTotal(cost);
                d = pend.addDays(1); // 1. next month
                pend = cal->addDays(pend, d.daysInMonth());
                if ((pend.year() == end.year()) && (pend.month() == end.month())) {
                    pend = end;
                }
            }
        }
        m_dlv->calculate();
        QApplication::restoreOverrideCursor();
        return;
    }
    QApplication::restoreOverrideCursor();
}

void AccountsView::print(KPrinter &printer) {
    //kDebug()<<k_funcinfo<<endl;
    uint top, left, bottom, right;
    printer.margins(&top, &left, &bottom, &right);
    //kDebug()<<m.width()<<"x"<<m.height()<<" : "<<top<<", "<<left<<", "<<bottom<<", "<<right<<" : "<<size()<<endl;
    QPainter p;
    p.begin(&printer);
    p.setViewport(left, top, printer.width()-left-right, printer.height()-top-bottom);
    p.setClipRect(left, top, printer.width()-left-right, printer.height()-top-bottom);
    QRect preg = p.clipRegion().boundingRect();
    //kDebug()<<"p="<<preg<<endl;
    //p.drawRect(preg.x(), preg.y(), preg.width()-1, preg.height()-1);
    double scale = qMin((double)preg.width()/(double)size().width(), (double)preg.height()/(double)(size().height()));
    //kDebug()<<"scale="<<scale<<endl;
    if (scale < 1.0) {
        p.scale(scale, scale);
    }
    QPixmap labelPixmap = QPixmap::grabWidget( m_label );
    p.drawPixmap( m_label->pos(), labelPixmap );
    p.translate(0, m_label->size().height());
    m_dlv->paintContents(&p);
    p.end();
}

bool AccountsView::setContext(Context::Accountsview &context) {
    //kDebug()<<k_funcinfo<<"---->"<<endl;
    Q3ValueList<int> list;
    list << context.accountsviewsize << context.periodviewsize;
    //m_dlv->setSizes(list); //NOTE: Doesn't always work!
    m_date = context.date;
    if (!m_date.isValid())
        m_date = QDate::currentDate();
    m_period = context.period;
    m_cumulative = context.cumulative;
    setContextClosedItems(context);
    //kDebug()<<k_funcinfo<<"<----"<<endl;
    return true;
}

void AccountsView::setContextClosedItems(Context::Accountsview &context) {
    for (QStringList::ConstIterator it = context.closedItems.begin(); it != context.closedItems.end(); ++it) {
        if (m_accounts.findAccount(*it)) {
            Q3ListViewItemIterator lit(m_dlv->masterListView());
            for (; lit.current(); ++lit) {
                if (lit.current()->text(0) == (*it)) {
                    m_dlv->setOpen(lit.current(), false);
                    break;
                }
            }
        }
    }
}

void AccountsView::getContext(Context::Accountsview &context) const {
    //kDebug()<<k_funcinfo<<endl;
    context.accountsviewsize = m_dlv->sizes()[0];
    context.periodviewsize = m_dlv->sizes()[1];
    context.date = m_date;
    context.period = m_period;
    context.cumulative = m_cumulative;
    //kDebug()<<k_funcinfo<<"sizes="<<sizes()[0]<<","<<sizes()[1]<<endl;

    getContextClosedItems(context, m_dlv->masterListView()->firstChild());
}


void AccountsView::getContextClosedItems(Context::Accountsview &context, Q3ListViewItem *item) const {
    if (item == 0)
        return;
    for (Q3ListViewItem *i = item; i; i = i->nextSibling()) {
        if (!i->isOpen()) {
            context.closedItems.append(i->text(0));
            //kDebug()<<k_funcinfo<<"add closed "<<i->text(0)<<endl;
        }
        getContextClosedItems(context, i->firstChild());
    }
}

void AccountsView::slotConfigure() {
    //kDebug()<<k_funcinfo<<endl;
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
    if ( offset < m_periodTexts.count() )
        return m_periodTexts[offset];
    return QString();
}

}  //KPlato namespace

#include "kptaccountsview.moc"
