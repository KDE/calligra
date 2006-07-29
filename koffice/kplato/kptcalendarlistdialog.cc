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
#include <qheader.h>
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

class CalendarListViewItem : public KListViewItem
{
public:
    CalendarListViewItem(CalendarListDialogImpl &pan, QListView *lv, Calendar *cal,  Calendar *orig=0)
        : KListViewItem(lv, cal->name()), panel(pan) {

        calendar = cal;
        original = orig;
        state = None;
        base = 0;
        setRenameEnabled(0, false);
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
            //kdDebug()<<k_funcinfo<<"add: "<<calendar->name()<<" p="<<&p<<endl;
            base ? calendar->setParent(base->baseCalendar()) : calendar->setParent(0);
            macro->addCommand(new CalendarAddCmd(part, &p, calendar));
            calendar = 0;
        } else if (state & Modified) {
            //kdDebug()<<k_funcinfo<<"modified: "<<calendar->name()<<endl;
            if (original->name() != calendar->name()) {
                if (macro == 0) macro = new KMacroCommand("");
                macro->addCommand(new CalendarModifyNameCmd(part, original, calendar->name()));
            }
            Calendar *c = base ? base->baseCalendar() : 0;
            if (c != original->parent()) {
                if (macro == 0) macro = new KMacroCommand("");
                macro->addCommand(new CalendarModifyParentCmd(part, original, c));
                //kdDebug()<<k_funcinfo<<"Base modified: "<<c->name()<<endl;
            }
            
            //kdDebug()<<k_funcinfo<<"Check for days deleted: "<<calendar->name()<<endl;
            QPtrListIterator<CalendarDay> oit = original->days();
            for (; oit.current(); ++oit) {
                if (calendar->findDay(oit.current()->date()) == 0) {
                    if (macro == 0) macro = new KMacroCommand("");
                    macro->addCommand(new CalendarRemoveDayCmd(part, original, oit.current()->date()));
                    //kdDebug()<<k_funcinfo<<"Removed day"<<endl;
                }
            }
        
            //kdDebug()<<k_funcinfo<<"Check for days added or modified: "<<calendar->name()<<endl;
            QPtrListIterator<CalendarDay> cit = calendar->days();
            for (; cit.current(); ++cit) {
                CalendarDay *day = original->findDay(cit.current()->date());
                if (day == 0) {
                    if (macro == 0) macro = new KMacroCommand("");
                    // added
                    //kdDebug()<<k_funcinfo<<"Added day"<<endl;
                    macro->addCommand(new CalendarAddDayCmd(part, original, new CalendarDay(cit.current())));
                } else if (*day != cit.current()) {
                    if (macro == 0) macro = new KMacroCommand("");
                    // modified
                    //kdDebug()<<k_funcinfo<<"Modified day"<<endl;
                    macro->addCommand(new CalendarModifyDayCmd(part, original, new CalendarDay(cit.current())));
                }
            }
            //kdDebug()<<k_funcinfo<<"Check for weekdays modified: "<<calendar->name()<<endl;
            CalendarDay *day = 0, *org = 0;
            for (int i=0; i < 7; ++i) {
                day = calendar->weekdays()->weekday(i);
                org = original->weekdays()->weekday(i);
                if (day && org) {
                    if (*org != *day) {
                        if (macro == 0) macro = new KMacroCommand("");
                        //kdDebug()<<k_funcinfo<<"Weekday["<<i<<"] modified"<<endl;
                        macro->addCommand(new CalendarModifyWeekdayCmd(part, original, i, new CalendarDay(day)));
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
        return macro;
    }

    Calendar *calendar;
    Calendar *original;
    CalendarListViewItem* base;
    CalendarListDialogImpl &panel;
    QString oldText;
    
protected:
    virtual void cancelRename(int col) {
        //kdDebug()<<k_funcinfo<<endl;
        if (col == 0 && oldText.isEmpty()) {
            return;
        }
        panel.renameStopped(this);
        KListViewItem::cancelRename(col);
        setRenameEnabled(col, false);
    }
private:
    int state;
};

//----------------------------------------------------
CalendarListDialog::CalendarListDialog(Project &p, QWidget *parent, const char *name)
    : KDialogBase( Swallow, i18n("Calendar's Settings"), Ok|Cancel, Ok, parent, name, true, true),
      project(p)
{
    //kdDebug()<<k_funcinfo<<&p<<endl;
    dia = new CalendarListDialogImpl(p, this);
    QPtrList<Calendar> list = p.calendars();
    QPtrListIterator<Calendar> it = list;
    for (; it.current(); ++it) {
        Calendar *c = new Calendar(it.current());
        //c->setProject(&p);
        new CalendarListViewItem(*dia, dia->calendarList, c, it.current());
    }
    dia->setBaseCalendars();
    
    QListViewItem *f = dia->calendarList->firstChild();
    if (f) {
        dia->calendarList->setSelected(f, true);
    }
    //kdDebug()<<"size="<<size().width()<<"x"<<size().height()<<" hint="<<sizeHint().width()<<"x"<<sizeHint().height()<<endl;
    resize(QSize(725, 388).expandedTo(minimumSizeHint()));

    setMainWidget(dia);
    enableButtonOK(false);

    connect(dia, SIGNAL(enableButtonOk(bool)), SLOT(enableButtonOK(bool)));
}

KCommand *CalendarListDialog::buildCommand(Part *part) {
    //kdDebug()<<k_funcinfo<<endl;
    KMacroCommand *cmd = 0;
    QListViewItemIterator cit(dia->calendarList);
    for (;cit.current(); ++cit) {
        CalendarListViewItem *item = dynamic_cast<CalendarListViewItem *>(cit.current());
        if (item) {
            KMacroCommand *c = item->buildCommand(part, project);
            if (c != 0) {
                if (cmd == 0) cmd = new KMacroCommand("");
                cmd->addCommand(c);
            }
        }
    }
    QPtrListIterator<CalendarListViewItem> it = dia->deletedItems();
    for (; it.current(); ++it) {
        //kdDebug()<<k_funcinfo<<"deleted: "<<it.current()->calendar->name()<<endl;
        if (it.current()->original) {
            if (cmd == 0) cmd = new KMacroCommand("");
            cmd->addCommand(new CalendarDeleteCmd(part, it.current()->original));
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

    calendarList->header()->setStretchEnabled(true, 0);
    calendarList->setShowSortIndicator(true);
    calendarList->setSorting(0);
    calendarList->setDefaultRenameAction(QListView::Accept);

    m_deletedItems.setAutoDelete(true);
    calendar->setEnabled(false);

    slotSelectionChanged();

    connect(calendar, SIGNAL(obligatedFieldsFilled(bool)), SLOT(slotEnableButtonOk(bool)));
    connect(calendar, SIGNAL(applyClicked()), SLOT(slotCalendarModified()));

    connect(bDelete, SIGNAL(clicked()), SLOT(slotDeleteClicked()));
    connect(bAdd, SIGNAL(clicked()), SLOT(slotAddClicked()));
    //connect(editName, SIGNAL(returnPressed()), SLOT(slotAddClicked()));

    connect(calendarList, SIGNAL(selectionChanged()), SLOT(slotSelectionChanged()));
    connect(calendarList, SIGNAL(doubleClicked(QListViewItem*, const QPoint&, int)), SLOT(slotListDoubleClicked(QListViewItem*, const QPoint&, int)));
    connect(calendarList, SIGNAL(itemRenamed(QListViewItem*, int)), SLOT(slotItemRenamed(QListViewItem*, int)));
    
    connect (baseCalendar, SIGNAL(activated(int)), SLOT(slotBaseCalendarActivated(int)));

    // Internal rename stuff
    connect(this, SIGNAL(renameStarted(QListViewItem*, int)), SLOT(slotRenameStarted(QListViewItem*, int)));
    connect(this, SIGNAL(startRename(QListViewItem*, int)), SLOT(slotStartRename(QListViewItem*, int)));
    connect(this, SIGNAL(selectionChanged()), SLOT(slotSelectionChanged()));
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

void CalendarListDialogImpl::slotSelectionChanged() {
    //kdDebug()<<k_funcinfo<<endl;
    QListViewItem *item = calendarList->selectedItem();
    bDelete->setEnabled((bool)item);
    bAdd->setEnabled(true);
    slotSelectionChanged(item);
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
        calendarList->takeItem(item);
        item->setState(CalendarListViewItem::Deleted);
        m_deletedItems.append(item);

        emit enableButtonOk(true);
    }
}

void CalendarListDialogImpl::slotAddClicked() {
    Calendar *cal = new Calendar();
    cal->setProject(&project);
    CalendarListViewItem *item = new CalendarListViewItem(*this, calendarList, cal);
    item->setState(CalendarListViewItem::New);
    
    slotListDoubleClicked(item, QPoint(), 0);
    
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

void CalendarListDialogImpl::slotItemRenamed(QListViewItem *itm, int col) {
    //kdDebug()<<k_funcinfo<<itm->text(0)<<endl;
    itm->setRenameEnabled(col, false);
    m_renameItem = 0;
    CalendarListViewItem *item = static_cast<CalendarListViewItem*>(itm);
    if (item->text(0).isEmpty()) {
        item->setText(0, item->oldText); // keep the old name
    }
    if (item->text(0).isEmpty()) {
        // Not allowed
        //kdDebug()<<k_funcinfo<<"name empty"<<endl;
        emit startRename(item, 0);
        return;
    }
    if (item->text(0) != item->oldText) {
        item->setState(CalendarListViewItem::Modified);
        item->calendar->setName(item->text(0));
    }
    renameStopped(item);
    slotEnableButtonOk(true);
}

// We don't get notified when rename is cancelled, this is called from the item
void CalendarListDialogImpl::renameStopped(QListViewItem */*item*/) {
    //kdDebug()<<k_funcinfo<<(item?item->text(0):"")<<endl;
    m_renameItem = 0;
    emit selectionChanged();
}

void CalendarListDialogImpl::slotListDoubleClicked(QListViewItem *item, const QPoint&, int col) {
    //kdDebug()<<k_funcinfo<<(item?item->text(0):"")<<endl;
    if (m_renameItem)
        return;
    slotStartRename(item, col);
}

void CalendarListDialogImpl::slotRenameStarted(QListViewItem */*item*/, int /*col*/) {
    //kdDebug()<<k_funcinfo<<(item?item->text(0):"")<<endl;
    if (calendarList->isRenaming()) {
        bDelete->setEnabled(false);
        bAdd->setEnabled(false);
    }
}

void CalendarListDialogImpl::slotStartRename(QListViewItem *item, int col) {
    //kdDebug()<<k_funcinfo<<(item?item->text(0):"")<<endl;
    static_cast<CalendarListViewItem*>(item)->oldText = item->text(col);
    item->setRenameEnabled(col, true);
    item->startRename(col);
    m_renameItem = item;
    
    emit renameStarted(item, col);
}

}  //KPlato namespace

#include "kptcalendarlistdialog.moc"
