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

#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qlineedit.h>
#include <qdatetimeedit.h>
#include <qdatetime.h>
#include <qtabwidget.h>
#include <qtextbrowser.h>

#include <klocale.h>

#include <kabc/addressee.h>
#include <kabc/addresseedialog.h>

#include <kdebug.h>

namespace KPlato
{

class CalendarListViewItem : public QListViewItem
{
public:
    CalendarListViewItem(QListView *lv, Calendar *cal,  Calendar *orig=0)
        : QListViewItem(lv, cal->name()) {

        calendar = cal;
        original = orig;
        state = None;
        base = 0;
    }
    ~CalendarListViewItem() {
        if (calendar) {
            //kdDebug()<<k_funcinfo<<"Delete copy: "<<calendar->name()<<endl;
            delete calendar;
        }
    }
    enum State { None, New, Modified, Deleted };

    void setState(State s) { if (state == None || s == Deleted) state = s; }
    
    void deleteOriginal() {
        if (original)
            original->setDeleted(true);
    }
    Calendar *baseCalendar() {
        if (state == Deleted) return 0;
        return original ? original : calendar;
    }
    bool hasBaseCalendar(CalendarListViewItem *item) {
        if (!base) return false;
        return base == item || base->hasBaseCalendar(item);
    }
    void ok(Part *part, Project &p, KMacroCommand *macro) {
        if (state == New) {
            //kdDebug()<<k_funcinfo<<"add: "<<calendar->name()<<" p="<<&p<<endl;
            base ? calendar->setParent(base->baseCalendar()) : calendar->setParent(0);
            macro->addCommand(new CalendarAddCmd(part, &p, calendar));
            calendar = 0;
        } else if (state == Modified) {
            //kdDebug()<<k_funcinfo<<"modified: "<<calendar->name()<<endl;
            Calendar *c = base ? base->baseCalendar() : 0;
            if (c != original->parent()) {
                original->setParent(c);
                //kdDebug()<<k_funcinfo<<"Base modified: "<<c->name()<<endl;
            }
            if (calendar->days() != original->days()) {
                //kdDebug()<<k_funcinfo<<"Days modified: "<<calendar->name()<<endl;
                QPtrListIterator<CalendarDay> it = calendar->days();
                for (; it.current(); ++it) {
                    CalendarDay *day = original->findDay(it.current()->date());
                    if (day) {
                        if (it.current()->state() != day->state()) {
                            day->setState(it.current()->state());
                            //kdDebug()<<k_funcinfo<<calendar->name()<<": state="<<day->state()<<endl;
                        }
                        if (it.current()->workingIntervals() != day->workingIntervals()) {
                            day->setIntervals(it.current()->workingIntervals());
                        }
                    } else {
                        original->addDay(new CalendarDay(it.current()));
                        //kdDebug()<<k_funcinfo<<"Day added: "<<it.current()->date().toString()<<endl;
                    }
                }
            }
            if (calendar->weeks() != original->weeks()) {
                //kdDebug()<<k_funcinfo<<"Weeks modified: "<<calendar->name()<<endl;
                original->weeks()->setWeeks(calendar->weeks()->weeks());
            }
            if (calendar->weekdays() != original->weekdays()) {
                //kdDebug()<<k_funcinfo<<"Weekdays modified: "<<calendar->name()<<endl;
                CalendarDay *day = 0, *org = 0;
                for (int i=0; i < 7; ++i) {
                    day = calendar->weekdays()->weekday(i);
                    org = original->weekdays()->weekday(i);
                    if (day && org) {
                        if (org->state() != day->state()) {
                            org->setState(day->state());
                        }
                        if (org->workingIntervals() != day->workingIntervals()) {
                            org->setIntervals(day->workingIntervals());
                        }
                    } else if (day) {
                        // shouldn't happen: hmmm, add day to original??
                        kdError()<<k_funcinfo<<"Should always have 7 weekdays"<<endl;
                    } else if (org) {
                        // shouldn't happen: set org to default??
                        kdError()<<k_funcinfo<<"Should always have 7 weekdays"<<endl;
                    }
                }
            }
        }
    }

    Calendar *calendar;
    Calendar *original;
    int state;
    CalendarListViewItem* base;
};

CalendarListDialog::CalendarListDialog(Project &p, QWidget *parent, const char *name)
    : KDialogBase( Swallow, i18n("Calendar's Settings"), Ok|Cancel, Ok, parent, name, true, true),
      project(p)
{
    //kdDebug()<<k_funcinfo<<&p<<endl;
    dia = new CalendarListDialogImpl(p, this);
    QPtrList<Calendar> list = p.calendars();
    QPtrListIterator<Calendar> it = list;
    for (; it.current(); ++it) {
        //kdDebug()<<k_funcinfo<<"Add calendar: "<<it.current()->name()<<" deleted="<<it.current()->isDeleted()<<endl;
//        if (!it.current()->isDeleted()) {
            Calendar *c = new Calendar(it.current());
            c->setProject(&p);
            new CalendarListViewItem(dia->calendarList, c, it.current());
//        }
    }    
    dia->setBaseCalendars();
    
    QListViewItem *f = dia->calendarList->firstChild();
    if (f) {
        f->setSelected(true);
        dia->slotSelectionChanged(f);
    }
    //kdDebug()<<"size="<<size().width()<<"x"<<size().height()<<" hint="<<sizeHint().width()<<"x"<<sizeHint().height()<<endl;
    resize(QSize(725, 388).expandedTo(minimumSizeHint()));

    setMainWidget(dia);
    enableButtonOK(false);

    connect(dia, SIGNAL(obligatedFieldsFilled(bool) ), SLOT(enableButtonOK(bool)));
    connect(dia, SIGNAL(enableButtonOk(bool)), SLOT(enableButtonOK(bool)));
}

KMacroCommand *CalendarListDialog::buildCommand(Part *part) {
    //kdDebug()<<k_funcinfo<<endl;
    KMacroCommand *cmd = new KMacroCommand(i18n("Modify Calendars"));
    bool modified = false;
    QPtrListIterator<CalendarListViewItem> it = dia->deletedItems();
    for (; it.current(); ++it) {
        //kdDebug()<<k_funcinfo<<"deleted: "<<it.current()->calendar->name()<<endl;
        if (it.current()->original) {
            cmd->addCommand(new CalendarDeleteCmd(part, it.current()->original));
            modified = true;
        }
    }
    QListViewItemIterator cit(dia->calendarList);
    for (;cit.current(); ++cit) {
        CalendarListViewItem *item = dynamic_cast<CalendarListViewItem *>(cit.current());
        if (item) {
            item->ok(part, project, cmd);
            modified = true;
        }
    }
    if (modified)
        return cmd;
    
    delete cmd;
    return 0;
}

void CalendarListDialog::slotOk() {
    accept();
}


CalendarListDialogImpl::CalendarListDialogImpl (Project &p, QWidget *parent) 
    : CalendarListDialogBase(parent),
      project(p) {

    m_deletedItems.setAutoDelete(true);
    calendar->setEnabled(false);

    connect(calendar, SIGNAL(obligatedFieldsFilled(bool)), SLOT(slotEnableButtonOk(bool)));
    connect(calendar, SIGNAL(applyClicked()), SLOT(slotCalendarModified()));

    connect(bDelete, SIGNAL(clicked()), SLOT(slotDeleteClicked()));
    connect(bAdd, SIGNAL(clicked()), SLOT(slotAddClicked()));
    //connect(editName, SIGNAL(returnPressed()), SLOT(slotAddClicked()));

    connect(calendarList, SIGNAL(clicked(QListViewItem *)), SLOT(slotSelectionChanged(QListViewItem *)));

    connect (baseCalendar, SIGNAL(activated(int)), SLOT(slotBaseCalendarActivated(int)));
}

void CalendarListDialogImpl::setBaseCalendars() {
    QListViewItemIterator it(calendarList);
    for (;it.current(); ++it) {
        CalendarListViewItem *item = dynamic_cast<CalendarListViewItem *>(it.current());
        if (item) {
            item->base = findItem(item->calendar->parent());
        }
    }
}

void CalendarListDialogImpl::slotEnableButtonOk(bool on) {
    emit enableButtonOk(on);
}

void CalendarListDialogImpl::slotCheckAllFieldsFilled() {
    emit obligatedFieldsFilled(!editName->text().isEmpty());
}

void CalendarListDialogImpl::slotBaseCalendarActivated(int id) {
    CalendarListViewItem *item = dynamic_cast<CalendarListViewItem*>(calendarList->selectedItem());
    if (item) {
        item->base = baseCalendarList.at(id);
        item->setState(CalendarListViewItem::Modified);
        slotEnableButtonOk(true);
    } else {
        kdError()<<k_funcinfo<<"No CalendarListViewItem"<<endl;
    }
}

void CalendarListDialogImpl::slotSelectionChanged(QListViewItem *listItem) {
    //kdDebug()<<k_funcinfo<<endl;
    baseCalendarList.clear();
    baseCalendar->clear();
    baseCalendar->setEnabled(false);
    CalendarListViewItem *cal = dynamic_cast<CalendarListViewItem *>(listItem);
    if (cal) {
        setCalendar(cal->calendar);
        baseCalendar->insertItem(i18n("None"));
        baseCalendarList.append(0);
        int me = 0, i = 0;
        QListViewItemIterator it(calendarList);
        for (; it.current(); ++it) {
            CalendarListViewItem *item = dynamic_cast<CalendarListViewItem*>(it.current());
            if (item && cal != item && !item->hasBaseCalendar(cal)) {
                baseCalendar->insertItem(item->text(0));
                baseCalendarList.append(item);
                i++;
                if (item == cal->base) {
                    me = i;
                    //kdDebug()<<k_funcinfo<<"item="<<item<<": cal="<<cal->calendar->name()<<" has parent "<<cal->base->calendar->name()<<endl;
                }
            }
        }
        baseCalendar->setCurrentItem(me);
        baseCalendar->setEnabled(true);
        return;
    }
    calendar->clear();
}
void CalendarListDialogImpl::setCalendar(Calendar *cal) {
    QPtrList<Calendar> list;
    calendar->setCalendar(cal); 
    calendar->setEnabled(true); 
}
 
void CalendarListDialogImpl::slotCalendarModified() {
    CalendarListViewItem *item = dynamic_cast<CalendarListViewItem*>(calendarList->currentItem());
    if (item) {
        item->setState(CalendarListViewItem::Modified);
        //kdDebug()<<k_funcinfo<<"("<<item->calendar<<")"<<endl;
    }
    emit calendarModified();
}

void CalendarListDialogImpl::slotDeleteClicked() {
    CalendarListViewItem *item = static_cast<CalendarListViewItem*>(calendarList->selectedItem());
    if (item) {
        slotSelectionChanged(0);
        calendarList->takeItem(item);
        item->setState(CalendarListViewItem::Deleted);
        m_deletedItems.append(item);

        emit enableButtonOk(true);
    }
    editName->clear();
}

void CalendarListDialogImpl::slotAddClicked() {
    if (editName->text().isEmpty())
        return;
    Calendar *cal = new Calendar(editName->text());
    cal->setProject(&project);
    CalendarListViewItem *item = new CalendarListViewItem(calendarList, cal);
    item->state = CalendarListViewItem::New;
    calendarList->setSelected(item, true);
    slotSelectionChanged(item);
    editName->clear();
    emit enableButtonOk(true);
}

QPtrList<CalendarListViewItem> &CalendarListDialogImpl::deletedItems() {
    return m_deletedItems;
}

CalendarListViewItem *CalendarListDialogImpl::findItem(Calendar *cal) {
    if (!cal)
        return 0;
    QListViewItemIterator it(calendarList);
    for (;it.current(); ++it) {
        CalendarListViewItem *item = dynamic_cast<CalendarListViewItem *>(it.current());
        if (item && (cal == item->original || cal == item->calendar)) {
            //kdDebug()<<k_funcinfo<<"Found: "<<cal->name()<<endl;
            return item;
        }
    }
    return 0;
}

}  //KPlato namespace

#include "kptcalendarlistdialog.moc"
