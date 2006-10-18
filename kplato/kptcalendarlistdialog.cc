/* This file is part of the KDE project
   Copyright (C) 2004 - 2005 Dag Andersen <danders@get2net.dk>

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

#include "kptcalendarlistdialog.h"
#include "kptproject.h"
#include "kptcalendar.h"
#include "kptcommand.h"
#include "kptpart.h"

#include <QPushButton>
#include <QComboBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QTreeWidget>

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

    Calendar *baseCalendar() {
        if (state & Deleted) return 0;
        return original ? original : calendar;
    }
    bool hasBaseCalendar(CalendarListViewItem *item) {
        if (!base) return false;
        return base == item || base->hasBaseCalendar(item);
    }
    KMacroCommand *buildCommand(Part *part, Project &p) {
        KMacroCommand *macro=0;
        if (state & New) {
            if (macro == 0) macro = new KMacroCommand("");
            //kDebug()<<k_funcinfo<<"add: "<<calendar->name()<<" p="<<&p<<endl;
            base ? calendar->setParent(base->baseCalendar()) : calendar->setParent(0);
            macro->addCommand(new CalendarAddCmd(part, &p, calendar));
            calendar = 0;
        } else if (state & Modified) {
            //kDebug()<<k_funcinfo<<"modified: "<<calendar->name()<<endl;
            if (original->name() != calendar->name()) {
                if (macro == 0) macro = new KMacroCommand("");
                macro->addCommand(new CalendarModifyNameCmd(part, original, calendar->name()));
            }
            Calendar *c = base ? base->baseCalendar() : 0;
            if (c != original->parent()) {
                if (macro == 0) macro = new KMacroCommand("");
                macro->addCommand(new CalendarModifyParentCmd(part, original, c));
                //kDebug()<<k_funcinfo<<"Base modified: "<<c->name()<<endl;
            }

            //kDebug()<<k_funcinfo<<"Check for days deleted: "<<calendar->name()<<endl;
            foreach (CalendarDay *day, original->days()) {
                if (calendar->findDay(day->date()) == 0) {
                    if (macro == 0) macro = new KMacroCommand("");
                    macro->addCommand(new CalendarRemoveDayCmd(part, original, day->date()));
                    //kDebug()<<k_funcinfo<<"Removed day"<<endl;
                }
            }

            //kDebug()<<k_funcinfo<<"Check for days added or modified: "<<calendar->name()<<endl;
            foreach (CalendarDay *org, original->days()) {
                CalendarDay *day = original->findDay(org->date());
                if (day == 0) {
                    if (macro == 0) macro = new KMacroCommand("");
                    // added
                    //kDebug()<<k_funcinfo<<"Added day"<<endl;
                    macro->addCommand(new CalendarAddDayCmd(part, original, new CalendarDay(org)));
                } else if (*day != org) {
                    if (macro == 0) macro = new KMacroCommand("");
                    // modified
                    //kDebug()<<k_funcinfo<<"Modified day"<<endl;
                    macro->addCommand(new CalendarModifyDayCmd(part, original, new CalendarDay(org)));
                }
            }
            //kDebug()<<k_funcinfo<<"Check for weekdays modified: "<<calendar->name()<<endl;
            CalendarDay *day = 0, *org = 0;
            for (int i=0; i < 7; ++i) {
                day = calendar->weekdays()->weekday(i);
                org = original->weekdays()->weekday(i);
                if (day && org) {
                    if (*org != *day) {
                        if (macro == 0) macro = new KMacroCommand("");
                        //kDebug()<<k_funcinfo<<"Weekday["<<i<<"] modified"<<endl;
                        macro->addCommand(new CalendarModifyWeekdayCmd(part, original, i, new CalendarDay(day)));
                    }
                } else if (day) {
                    // shouldn't happen: hmmm, add day to original??
                    kError()<<k_funcinfo<<"Should always have 7 weekdays"<<endl;
                } else if (org) {
                    // shouldn't happen: set org to default??
                    kError()<<k_funcinfo<<"Should always have 7 weekdays"<<endl;
                }
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
        //kDebug()<<k_funcinfo<<endl;
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
CalendarListDialog::CalendarListDialog(Project &p, QWidget *parent, const char *name)
    : KDialog( parent),
      project(p)
{
    setCaption( i18n("Calendar's Settings") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );
    //kDebug()<<k_funcinfo<<&p<<endl;
    dia = new CalendarListDialogImpl(p, this);
    QList<Calendar*> list = p.calendars();
    foreach (Calendar *org, list) {
        Calendar *c = new Calendar(org);
        //c->setProject(&p);
        new CalendarListViewItem(*dia, dia->calendarList, c, org);
    }
    dia->setBaseCalendars();

    QTreeWidgetItem *f = dia->calendarList->topLevelItem(0);
    if (f) {
        f->setSelected(true);
    }
    //kDebug()<<"size="<<size().width()<<"x"<<size().height()<<" hint="<<sizeHint().width()<<"x"<<sizeHint().height()<<endl;
    resize(QSize(725, 388).expandedTo(minimumSizeHint()));

    setMainWidget(dia);
    enableButtonOk(false);

    connect(this, SIGNAL(okClicked()), SLOT(slotOk()));
    connect(dia, SIGNAL(enableButtonOk(bool)), SLOT(enableButtonOk(bool)));
}

KCommand *CalendarListDialog::buildCommand(Part *part) {
    //kDebug()<<k_funcinfo<<endl;
    KMacroCommand *cmd = 0;
    int c = dia->calendarList->topLevelItemCount();
    for (int i=0; i < c; ++i) {
        CalendarListViewItem *item = static_cast<CalendarListViewItem *>(dia->calendarList->topLevelItem(i));
        KMacroCommand *c = item->buildCommand(part, project);
        if (c != 0) {
            if (cmd == 0) cmd = new KMacroCommand("");
            cmd->addCommand(c);
        }
    }
    foreach (CalendarListViewItem *item, dia->deletedItems()) {
        //kDebug()<<k_funcinfo<<"deleted: "<<item->calendar->name()<<endl;
        if (item->original) {
            if (cmd == 0) cmd = new KMacroCommand("");
            cmd->addCommand(new CalendarDeleteCmd(part, item->original));
        }
    }
    if (cmd) {
        cmd->setName(i18n("Modify Calendars"));
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

    connect(bDelete, SIGNAL(clicked()), SLOT(slotDeleteClicked()));
    connect(bAdd, SIGNAL(clicked()), SLOT(slotAddClicked()));
    //connect(editName, SIGNAL(returnPressed()), SLOT(slotAddClicked()));

    connect(calendarList, SIGNAL(itemSelectionChanged()), SLOT(slotSelectionChanged()));
    connect(calendarList, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(slotListDoubleClicked(const QModelIndex &)));

    connect (baseCalendar, SIGNAL(activated(int)), SLOT(slotBaseCalendarActivated(int)));

    connect(this, SIGNAL(selectionChanged()), SLOT(slotSelectionChanged()));
    
    connect(calendarList, SIGNAL(itemChanged(QTreeWidgetItem*, int)), SLOT(slotItemChanged(QTreeWidgetItem*, int)));
}

CalendarListDialogImpl::~CalendarListDialogImpl() {
    if (!m_deletedItems.isEmpty())
        delete m_deletedItems.takeFirst();
}
void CalendarListDialogImpl::setBaseCalendars() {
    int c = calendarList->topLevelItemCount();
    for (int i = 0; i < c; ++i) {
        CalendarListViewItem *item = static_cast<CalendarListViewItem *>(calendarList->topLevelItem(i));
        item->base = findItem(item->calendar->parent());
    }
}

void CalendarListDialogImpl::slotItemChanged(QTreeWidgetItem*, int) {
    kDebug()<<k_funcinfo<<endl;
    slotEnableButtonOk(true);
}

void CalendarListDialogImpl::slotEnableButtonOk(bool on) {
    emit enableButtonOk(on);
}

void CalendarListDialogImpl::slotBaseCalendarActivated(int id) {
    QList<QTreeWidgetItem*> lst = calendarList->selectedItems();
    if (lst.count() == 1) {
        CalendarListViewItem *item = static_cast<CalendarListViewItem*>(lst[0]);
        item->base = baseCalendarList.at(id);
        item->setState(CalendarListViewItem::Modified);
        slotEnableButtonOk(true);
    } else {
        kError()<<k_funcinfo<<"No CalendarListViewItem (or too many)"<<endl;
    }
}

void CalendarListDialogImpl::slotSelectionChanged() {
    //kDebug()<<k_funcinfo<<endl;
    QList<QTreeWidgetItem *> lst = calendarList->selectedItems();

    bDelete->setEnabled(lst.count() > 0);
    bAdd->setEnabled(true);
    if (lst.count() > 0)
        slotSelectionChanged(lst[0]); // we only handle single selection!!
    else
        slotSelectionChanged(0);
}

void CalendarListDialogImpl::slotSelectionChanged(QTreeWidgetItem *listItem) {
    //kDebug()<<k_funcinfo<<endl;
    baseCalendarList.clear();
    baseCalendar->clear();
    baseCalendar->setEnabled(false);
    CalendarListViewItem *cal = dynamic_cast<CalendarListViewItem *>(listItem);
    if (cal) {
        setCalendar(cal->calendar);
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
                    //kDebug()<<k_funcinfo<<"item="<<item<<": cal="<<cal->calendar->name()<<" has parent "<<cal->base->calendar->name()<<endl;
                }
            }
        }
        baseCalendar->setCurrentIndex(me);
        baseCalendar->setEnabled(true);
        return;
    }
    calendar->clear();
}
void CalendarListDialogImpl::setCalendar(Calendar *cal) {
    calendar->setCalendar(cal);
    calendar->setEnabled(true);
}

void CalendarListDialogImpl::slotCalendarModified() {
    CalendarListViewItem *item = dynamic_cast<CalendarListViewItem*>(calendarList->currentItem());
    if (item) {
        item->setState(CalendarListViewItem::Modified);
        //kDebug()<<k_funcinfo<<"("<<item->calendar<<")"<<endl;
    }
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
    Calendar *cal = new Calendar();
    cal->setProject(&project);
    CalendarListViewItem *item = new CalendarListViewItem(*this, calendarList, cal);
    item->setState(CalendarListViewItem::New);
    calendarList->clearSelection();
    item->setSelected(true);
    calendarList->editItem(item, 0);

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
            //kDebug()<<k_funcinfo<<"Found: "<<cal->name()<<endl;
            return item;
        }
    }
    return 0;
}

void CalendarListDialogImpl::slotListDoubleClicked(const QModelIndex &index) {
    slotListDoubleClicked(calendarList->itemAt(index.row(), index.column()), index.column());
}

void CalendarListDialogImpl::slotListDoubleClicked(QTreeWidgetItem *item, int col) {
    //kDebug()<<k_funcinfo<<(item?item->text(0):"")<<endl;
}

}  //KPlato namespace

#include "kptcalendarlistdialog.moc"
