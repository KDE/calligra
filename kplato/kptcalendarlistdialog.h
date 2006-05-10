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

#ifndef KPTCALENDARLISTDIALOG_H
#define KPTCALENDARLISTDIALOG_H

#include "kptcalendarlistdialogbase.h"
#include "kptcalendaredit.h"
#include "kptcalendar.h"

#include <kdialogbase.h>

#include <QString>
//Added by qt3to4:
#include <Q3PtrList>

class KCommand;

namespace KPlato
{

class CalendarListViewItem;
class Project;
class Part;

class CalendarListDialogImpl : public CalendarListDialogBase {
    Q_OBJECT
public:
    CalendarListDialogImpl(Project &project, QWidget *parent);

    void setBaseCalendars();
    
    Calendar *getCalendar() { return calendar->getCalendar(); }
    void setCalendar(Calendar *cal);

    Q3PtrList<CalendarListViewItem> &deletedItems();

    void renameStopped(Q3ListViewItem *item);

public slots:
    void slotSelectionChanged();
    void slotSelectionChanged(Q3ListViewItem *listItem);

private slots:
    void slotBaseCalendarActivated(int id);
    void slotCalendarModified();
    void slotDeleteClicked();
    void slotAddClicked();
    void slotEnableButtonOk(bool on);
    void slotItemRenamed(Q3ListViewItem *item, int col);
    void slotListDoubleClicked(Q3ListViewItem *item, const QPoint&, int col);
    void slotStartRename(Q3ListViewItem *item, int col);
    void slotRenameStarted(Q3ListViewItem *item, int col);

signals:
    void obligatedFieldsFilled(bool yes);
    void calendarModified();
    void calendarChanged();
    void enableButtonOk(bool on);

    //internal
    void renameStarted(Q3ListViewItem *, int);
    void startRename(Q3ListViewItem *item, int col);
    void selectionChanged();

protected:
    CalendarListViewItem *findItem(Calendar *cal);
    
private:
    Project &project;
    Q3PtrList<CalendarListViewItem> m_deletedItems;
    Q3PtrList<CalendarListViewItem> baseCalendarList;
    Q3ListViewItem *m_renameItem;
};

class CalendarListDialog : public KDialogBase {
    Q_OBJECT
public:
    CalendarListDialog(Project &project, QWidget *parent=0, const char *name=0);
    
    KCommand *buildCommand(Part *part);

protected slots:
    void slotOk();
    
private:
    Project &project;
    CalendarListDialogImpl *dia;
};

}  //KPlato namespace

#endif // CALENDARLISTDIALOG_H
