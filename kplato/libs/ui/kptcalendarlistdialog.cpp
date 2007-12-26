/* This file is part of the KDE project
   Copyright (C) 2004 - 2007 Dag Andersen <danders@get2net.dk>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kptcalendarlistdialog.h"
#include "kptproject.h"
#include "kptcalendar.h"
#include "kptcommand.h"

#include <QPushButton>
#include <QList>
#include <QTreeWidget>
#include <QHeaderView>

#include <klocale.h>

#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>

#include <kdebug.h>

namespace KPlato
{

class CalendarListViewItem : public QTreeWidgetItem
{
public:
    CalendarListViewItem(CalendarListDialogImpl &pan, QTreeWidget *lv, Calendar *cal,  Calendar *orig=0)
    : QTreeWidgetItem(lv), panel(pan) {
        
        setText(0, cal->name());
        calendar = cal;
        original = orig;
        state = None;
        base = 0;
        
        setFlags(flags() | Qt::ItemIsEditable);
    }
    ~CalendarListViewItem() {
        delete calendar;
    }

    enum State { None=0, New=1, Modified=2, Deleted=4 };

    void setState(State s) { state |= s; }

    bool hasParent() const {
        return (bool)base;
    }
    QString timeZone() const {
        if ( hasParent() ) {
            return base->timeZone();
        }
        return calendar->timeZone().name();
    }
    Calendar *baseCalendar() const {
        if (state & Deleted) return 0;
        return original ? original : calendar;
    }
    bool hasBaseCalendar(CalendarListViewItem *item) const {
        if (!base) return false;
        return base == item || base->hasBaseCalendar(item);
    }
    MacroCommand *buildCommand(Project &p) {
        MacroCommand *macro=0;
        if (state & New) {
            if (macro == 0) macro = new MacroCommand("");
            //kDebug()<<"add:"<<calendar->name()<<" p="<<&p;
            Calendar *par = base ? base->baseCalendar() : 0;
            macro->addCommand(new CalendarAddCmd(&p, calendar, par));
            calendar = 0;
        } else if (state & Modified) {
            //kDebug()<<"modified:"<<calendar->name();
            if (original->name() != calendar->name()) {
                if (macro == 0) macro = new MacroCommand("");
                macro->addCommand(new CalendarModifyNameCmd(original, calendar->name()));
            }
            Calendar *c = base ? base->baseCalendar() : 0;
            if (c != original->parentCal()) {
                if (macro == 0) macro = new MacroCommand("");
                macro->addCommand(new CalendarModifyParentCmd(original->project(), original, c));
                //kDebug()<<"Base modified:"<<c->name();
            }

            //kDebug()<<"Check for days deleted:"<<calendar->name();
            foreach (CalendarDay *day, original->days()) {
                if (calendar->findDay(day->date()) == 0) {
                    if (macro == 0) macro = new MacroCommand("");
                    macro->addCommand(new CalendarRemoveDayCmd(original, day->date()));
                    //kDebug()<<"Removed day"<<day->date();
                }
            }
            //kDebug()<<"Check for days added or modified:"<<calendar->name();
            foreach (CalendarDay *c, calendar->days()) {
                CalendarDay *day = original->findDay(c->date());
                if (day == 0) {
                    if (macro == 0) macro = new MacroCommand("");
                    // added
                    //kDebug()<<"Added day"<<c->date();
                    macro->addCommand(new CalendarAddDayCmd(original, new CalendarDay(c)));
                } else if (*day != c) {
                    if (macro == 0) macro = new MacroCommand("");
                    // modified
                    //kDebug()<<"Modified day"<<c->date();
                    macro->addCommand(new CalendarModifyDayCmd(original, new CalendarDay(c)));
                }
            }
            //kDebug()<<"Check for weekdays modified:"<<calendar->name();
            CalendarDay *day = 0, *org = 0;
            for (int i=1; i <= 7; ++i) {
                day = calendar->weekdays()->weekday(i);
                org = original->weekdays()->weekday(i);
                if (day && org) {
                    if (*org != *day) {
                        if (macro == 0) macro = new MacroCommand("");
                        //kDebug()<<"Weekday["<<i<<"] modified";
                        macro->addCommand(new CalendarModifyWeekdayCmd(original, i, new CalendarDay(day)));
                    }
                } else if (day) {
                    // shouldn't happen: hmmm, add day to original??
                    kError()<<"Should always have 7 weekdays"<<endl;
                } else if (org) {
                    // shouldn't happen: set org to default??
                    kError()<<"Should always have 7 weekdays"<<endl;
                }
            }
            if ( original->timeZone() != calendar->timeZone() ) {
                if (macro == 0) macro = new MacroCommand("");
                macro->addCommand( new CalendarModifyTimeZoneCmd( original, calendar->timeZone() ) );
            }
        }
        return macro;
    }

    Calendar *calendar;
    Calendar *original;
    CalendarListViewItem* base;
    CalendarListDialogImpl &panel;
    QString oldText;

protected:
/*    virtual void cancelRename(int col) {
        //kDebug();
        if (col == 0 && oldText.isEmpty()) {
            return;
        }
        panel.renameStopped(this);
        QTreeWidgetItem::cancelRename(col);
        setRenameEnabled(col, false);
    }*/
private:
    int state;
};

//----------------------------------------------------
CalendarListDialog::CalendarListDialog(Project &p, QWidget *parent)
    : KDialog( parent),
      project(p)
{
    setCaption( i18n("Calendar's Settings") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    //kDebug()<<&p;
    dia = new CalendarListDialogImpl(p, this);
    QList<Calendar*> list = p.allCalendars();
    foreach (Calendar *org, list) {
        Calendar *c = new Calendar();
        *c = *org;
        //c->setProject(&p);
        new CalendarListViewItem(*dia, dia->calendarList, c, org);
    }
    dia->setBaseCalendars();

    QTreeWidgetItem *f = dia->calendarList->topLevelItem(0);
    if (f) {
        f->setSelected(true);
    }
    //kDebug()<<"size="<<size().width()<<"x"<<size().height()<<" hint="<<sizeHint().width()<<"x"<<sizeHint().height();
    resize(QSize(725, 450).expandedTo(minimumSizeHint()));

    setMainWidget(dia);
    enableButtonOk(false);

    connect(this, SIGNAL(okClicked()), SLOT(slotOk()));
    connect(dia, SIGNAL(enableButtonOk(bool)), SLOT(enableButtonOk(bool)));
    // Don't connect this until the list is filled
    connect(dia->calendarList, SIGNAL(itemChanged(QTreeWidgetItem*, int)), dia, SLOT(slotItemChanged(QTreeWidgetItem*, int)));
}

MacroCommand *CalendarListDialog::buildCommand() {
    //kDebug();
    MacroCommand *cmd = 0;
    int c = dia->calendarList->topLevelItemCount();
    for (int i=0; i < c; ++i) {
        CalendarListViewItem *item = static_cast<CalendarListViewItem *>(dia->calendarList->topLevelItem(i));
        //kDebug()<<item->text(0);
        MacroCommand *c = item->buildCommand(project);
        if (c != 0) {
            if (cmd == 0) cmd = new MacroCommand("");
            cmd->addCommand(c);
        }
    }
    foreach (CalendarListViewItem *item, dia->deletedItems()) {
        //kDebug()<<"deleted:"<<item->calendar->name();
        if (item->original) {
            if (cmd == 0) cmd = new MacroCommand("");
            cmd->addCommand(new CalendarRemoveCmd(&project, item->original));
        }
    }
    if (cmd) {
        cmd->setText(i18n("Modify Calendars"));
    }
    return cmd;
}

void CalendarListDialog::slotOk() {
    accept();
}

//--------------------------------------------------
CalendarListDialogImpl::CalendarListDialogImpl (Project &p, QWidget *parent)
    : CalendarListDialogBase(parent),
      project(p),
      m_renameItem(0) {

    calendarList->header()->setStretchLastSection(true);
    calendarList->setSortingEnabled(true);
    calendarList->sortByColumn(0);
    calendar->setEnabled(false);
    calendarList->setSelectionMode(QAbstractItemView::SingleSelection);
    
    slotSelectionChanged();

    connect(calendar, SIGNAL(obligatedFieldsFilled(bool)), SLOT(slotEnableButtonOk(bool)));
    connect(calendar, SIGNAL(applyClicked()), SLOT(slotCalendarModified()));
    connect(calendar, SIGNAL(timeZoneChanged()), SLOT(slotCalendarModified()));
    
    connect(bDelete, SIGNAL(clicked()), SLOT(slotDeleteClicked()));
    connect(bAdd, SIGNAL(clicked()), SLOT(slotAddClicked()));
    //connect(editName, SIGNAL(returnPressed()), SLOT(slotAddClicked()));

    connect(calendarList, SIGNAL(itemSelectionChanged()), SLOT(slotSelectionChanged()));
    connect(calendarList, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(slotListDoubleClicked(const QModelIndex &)));

    connect (baseCalendar, SIGNAL(activated(int)), SLOT(slotBaseCalendarActivated(int)));

    connect(this, SIGNAL(selectionChanged()), SLOT(slotSelectionChanged()));
    
}

CalendarListDialogImpl::~CalendarListDialogImpl() {
    if (!m_deletedItems.isEmpty())
        delete m_deletedItems.takeFirst();
}

void CalendarListDialogImpl::setBaseCalendars() {
    int c = calendarList->topLevelItemCount();
    for (int i = 0; i < c; ++i) {
        CalendarListViewItem *item = static_cast<CalendarListViewItem *>(calendarList->topLevelItem(i));
        if ( item->original )
            item->base = findItem(item->original->parentCal());
    }
}

void CalendarListDialogImpl::slotItemChanged(QTreeWidgetItem *ci, int) {
    if (ci == 0)
        return;
    //kDebug()<<ci->text(0)<<","<<col;
    CalendarListViewItem *item = static_cast<CalendarListViewItem*>(ci);
    item->calendar->setName(item->text(0));
    item->setState(CalendarListViewItem::Modified);
    slotEnableButtonOk(true);
}

void CalendarListDialogImpl::slotEnableButtonOk(bool on) {
    emit enableButtonOk(on);
}

void CalendarListDialogImpl::slotBaseCalendarActivated(int id) {
    //kDebug();
    QList<QTreeWidgetItem*> lst = calendarList->selectedItems();
    if (lst.count() == 1) {
        CalendarListViewItem *item = static_cast<CalendarListViewItem*>(lst[0]);
        item->base = baseCalendarList.at(id);
        item->setState(CalendarListViewItem::Modified);
        slotEnableButtonOk(true);
    } else {
        kError()<<"No CalendarListViewItem (or too many)"<<endl;
    }
}

void CalendarListDialogImpl::slotSelectionChanged() {
    //kDebug();
    QList<QTreeWidgetItem *> lst = calendarList->selectedItems();
    bDelete->setEnabled(lst.count() > 0);
    bAdd->setEnabled(true);
    if (lst.count() > 0)
        slotSelectionChanged(lst[0]); // we only handle single selection!!
    else
        slotSelectionChanged(0);
}

void CalendarListDialogImpl::slotSelectionChanged(QTreeWidgetItem *listItem) {
    //kDebug();
    baseCalendarList.clear();
    baseCalendar->clear();
    baseCalendar->setEnabled(false);
    CalendarListViewItem *cal = dynamic_cast<CalendarListViewItem *>(listItem);
    if (cal) {
        setCalendar(cal->calendar, cal->timeZone(), cal->hasParent());
        baseCalendar->addItem(i18n("None"));
        baseCalendarList.append(0);
        int me = 0, i = 0;
        int c = calendarList->topLevelItemCount();
        for (int it=0; it < c; ++it) {
            CalendarListViewItem *item = static_cast<CalendarListViewItem*>(calendarList->topLevelItem(it));
            if (cal != item && !item->hasBaseCalendar(cal)) {
                baseCalendar->addItem(item->text(0));
                baseCalendarList.append(item);
                i++;
                if (item == cal->base) {
                    me = i;
                    //kDebug()<<"item="<<item<<": cal="<<cal->calendar->name()<<" has parent"<<cal->base->calendar->name();
                }
            }
        }
        baseCalendar->setCurrentIndex(me);
        baseCalendar->setEnabled(true);
        return;
    }
    calendar->clear();
}
void CalendarListDialogImpl::setCalendar(Calendar *cal, const QString &tz, bool disable) {
    calendar->setCalendar(cal, tz, disable);
    calendar->setEnabled(true);
}

void CalendarListDialogImpl::slotCalendarModified() {
    QList<QTreeWidgetItem*> lst = calendarList->selectedItems();
    if (lst.count() == 0) {
        return;
    }
    CalendarListViewItem *item = static_cast<CalendarListViewItem*>(lst[0]);
    item->setState(CalendarListViewItem::Modified);
    //kDebug()<<"("<<item->calendar<<")";
    
    emit calendarModified();
}

void CalendarListDialogImpl::slotDeleteClicked() {
    QList<QTreeWidgetItem*> lst = calendarList->selectedItems();
    foreach (QTreeWidgetItem *i, lst) {
        CalendarListViewItem *item = static_cast<CalendarListViewItem*>(i);
        calendarList->takeTopLevelItem(calendarList->indexOfTopLevelItem(item));
        item->setState(CalendarListViewItem::Deleted);
        m_deletedItems.append(item);
    }
    if (lst.count() > 0)
        emit enableButtonOk(true);
}

void CalendarListDialogImpl::slotAddClicked() {
    disconnect(calendarList, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(slotItemChanged(QTreeWidgetItem*, int)));
    
    Calendar *cal = new Calendar();
    cal->setProject(&project);
    CalendarListViewItem *item = new CalendarListViewItem(*this, calendarList, cal);
    item->setState(CalendarListViewItem::New);
    calendarList->clearSelection();
    item->setSelected(true);
    calendarList->editItem(item, 0);
    
    connect(calendarList, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(slotItemChanged(QTreeWidgetItem*, int)));
}

QList<CalendarListViewItem*> &CalendarListDialogImpl::deletedItems() {
    return m_deletedItems;
}

CalendarListViewItem *CalendarListDialogImpl::findItem(Calendar *cal) {
    if (!cal)
        return 0;
    int c = calendarList->topLevelItemCount();
    for (int i=0; i < c; ++i) {
        CalendarListViewItem *item = static_cast<CalendarListViewItem *>(calendarList->topLevelItem(i));
        if (cal == item->original || cal == item->calendar) {
            //kDebug()<<"Found:"<<cal->name();
            return item;
        }
    }
    return 0;
}

void CalendarListDialogImpl::slotListDoubleClicked(const QModelIndex &index) {
    slotListDoubleClicked(calendarList->itemAt(index.row(), index.column()), index.column());
}

void CalendarListDialogImpl::slotListDoubleClicked(QTreeWidgetItem *item, int col) {
    //kDebug()<<(item?item->text(0):"");
}

}  //KPlato namespace

#include "kptcalendarlistdialog.moc"
