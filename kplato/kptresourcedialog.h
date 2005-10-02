/* This file is part of the KDE project
   Copyright (C) 2003, 2004 Dag Andersen <kplato@kde.org>

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

#ifndef KPTRESOURCEDIALOG_H
#define KPTRESOURCEDIALOG_H

#include "resourcedialogbase.h"

#include <kdialogbase.h>

#include <qmap.h>
#include <qcombobox.h>

class QTime;
class QString;

namespace KPlato
{

class KPTProject;
class KPTResource;
class KPTCalendar;

class KPTResourceDialogImpl : public ResourceDialogBase {
    Q_OBJECT
public:
    KPTResourceDialogImpl (QWidget *parent);

public slots:
    void slotChanged();
    void slotCalculationNeeded(const QString&);
    void slotChooseResource();
    void slotEditCalendarClicked();
    
signals:
    void changed();
    void calculate();
};

class KPTResourceDialog : public KDialogBase {
    Q_OBJECT
public:
    KPTResourceDialog(KPTProject &project, KPTResource &resource, QWidget *parent=0, const char *name=0);

    bool calculationNeeded() {  return m_calculationNeeded; }

    KPTCalendar *calendar() { return m_calendars[dia->calendarList->currentItem()]; }
    
protected slots:
    void enableButtonOk();
    void slotCalculationNeeded();
    void slotOk();
    void slotCalendarChanged(int);

private:
    KPTResource &m_resource;
    KPTResourceDialogImpl *dia;
    bool m_calculationNeeded;
    
    QMap<int, KPTCalendar*> m_calendars;
};

} //KPlato namespace

#endif
