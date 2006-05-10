/* This file is part of the KDE project
   Copyright (C) 2003 - 2005 Dag Andersen <kplato@kde.org>

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
#include "kptresource.h"

#include <kdialogbase.h>

#include <QMap>
#include <QComboBox>

class KCommand;

class QTime;
class QString;

namespace KPlato
{

class Part;
class Project;
class Resource;
class Calendar;

class ResourceDialogImpl : public ResourceDialogBase {
    Q_OBJECT
public:
    ResourceDialogImpl (QWidget *parent);

public slots:
    void slotChanged();
    void slotCalculationNeeded(const QString&);
    void slotChooseResource();
    
signals:
    void changed();
    void calculate();

protected slots:
    void slotAvailableFromChanged(const QDateTime& dt);
    void slotAvailableUntilChanged(const QDateTime& dt);
};

class ResourceDialog : public KDialogBase {
    Q_OBJECT
public:
    ResourceDialog(Project &project, Resource *resource, QWidget *parent=0, const char *name=0);

    bool calculationNeeded() {  return m_calculationNeeded; }

    Calendar *calendar() { return m_calendars[dia->calendarList->currentItem()]; }
    KCommand *buildCommand(Part *part = 0);
    
    static KCommand *buildCommand(Resource *original, Resource &resource, Part *part);
    
protected slots:
    void enableButtonOk();
    void slotCalculationNeeded();
    void slotOk();
    void slotCalendarChanged(int);

private:
    Resource *m_original;
    Resource m_resource;
    ResourceDialogImpl *dia;
    bool m_calculationNeeded;
    
    QMap<int, Calendar*> m_calendars;
};

} //KPlato namespace

#endif
