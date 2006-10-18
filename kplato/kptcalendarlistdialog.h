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

#include <kdialog.h>

#include <QString>
#include <QList>

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
    ~CalendarListDialogImpl();
    void setBaseCalendars();
    
    Calendar *getCalendar() { return calendar->getCalendar(); }
    void setCalendar(Calendar *cal);

    QList<CalendarListViewItem*> &deletedItems();

    void renameStopped(QTreeWidgetItem *item);

public slots:
    void slotSelectionChanged();
    void slotSelectionChanged(QTreeWidgetItem *listItem);

private slots:
    void slotBaseCalendarActivated(int id);
    void slotCalendarModified();
    void slotDeleteClicked();
    void slotAddClicked();
    void slotEnableButtonOk(bool on);
    void slotListDoubleClicked(const QModelIndex &index);
    void slotListDoubleClicked(QTreeWidgetItem *item, int col);
    void slotItemChanged(QTreeWidgetItem*, int);

signals:
    void obligatedFieldsFilled(bool yes);
    void calendarModified();
    void calendarChanged();
    void enableButtonOk(bool on);

    void selectionChanged();

protected:
    CalendarListViewItem *findItem(Calendar *cal);
    
private:
    Project &project;
    QList<CalendarListViewItem*> m_deletedItems;
    QList<CalendarListViewItem*> baseCalendarList;
    QTreeWidgetItem *m_renameItem;
};

class CalendarListDialog : public KDialog {
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
