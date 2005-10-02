/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTSTANDARDWORKTIMEDIALOG_H
#define KPTSTANDARDWORKTIMEDIALOG_H

#include "standardworktimedialogbase.h"
#include "kptcalendar.h"

#include <kdialogbase.h>

#include <qstring.h>

class KMacroCommand;

namespace KPlato
{

class KPTProject;
class KPTPart;

class KPTStandardWorktimeDialogImpl : public StandardWorktimeDialogBase {
    Q_OBJECT
public:
    KPTStandardWorktimeDialogImpl ( KPTStandardWorktime *std, QWidget *parent);

    KPTStandardWorktime *standardWorktime() { return m_std; }
    
private slots:
    void slotCheckAllFieldsFilled();
    void slotEnableButtonOk(bool on);

    void slotYearChanged(int);
    void slotMonthChanged(int);
    
    void slotEditDayClicked();
    
    void slotEditMondayClicked();
    void slotEditTuesdayClicked();
    void slotEditWednesdayClicked();
    void slotEditThursdayClicked();
    void slotEditFridayClicked();
    void slotEditSaturdayClicked();
    void slotEditSundayClicked();
    
    void slotStateMondayActivated(int);
    void slotStateTuesdayActivated(int);
    void slotStateWednesdayActivated(int);
    void slotStateThursdayActivated(int);
    void slotStateFridayActivated(int);
    void slotStateSaturdayActivated(int);
    void slotStateSundayActivated(int);
    
    void slotMondayTimeChanged(const QTime& time);
    void slotTuesdayTimeChanged(const QTime& time);
    void slotWednesdayTimeChanged(const QTime& time);
    void slotThursdayTimeChanged(const QTime& time);
    void slotFridayTimeChanged(const QTime& time);
    void slotSaturdayTimeChanged(const QTime& time);
    void slotSundayTimeTimeChanged(const QTime& time);

signals:
    void obligatedFieldsFilled(bool yes);
    void enableButtonOk(bool);

private:
    KPTStandardWorktime *m_std;
};

class KPTStandardWorktimeDialog : public KDialogBase {
    Q_OBJECT
public:
    KPTStandardWorktimeDialog(KPTProject &project, QWidget *parent=0, const char *name=0);
    
    KMacroCommand *buildCommand(KPTPart *part);

protected slots:
    void slotOk();
    
private:
    KPTProject &project;
    KPTStandardWorktimeDialogImpl *dia;
    KPTStandardWorktime *m_original;
};

} //KPlato namespace

#endif // KPTSTANDARDWORKTIMEDIALOG_H
