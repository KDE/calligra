/* This file is part of the KDE project
 Copyright (C) 2010 Dag Andersen <danders@get2net.dk>

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
 Boston, MA 02110-1301, USA.
*/

#ifndef KPLATOXMLLOADERBASE_H
#define KPLATOXMLLOADERBASE_H

#include "kptaccount.h"
#include "kpttask.h"

#include <QObject>
#include <QString>

#define KPLATO_MAX_FILE_SYNTAX_VERSION "0.6.5"
#define KPLATOWORK_MAX_FILE_SYNTAX_VERSION "0.6.5"

class KoXmlElement;

namespace KPlato
{
    class XMLLoaderObject;
    class Project;
    class Task;
    class Calendar;
    class CalendarDay;
    class CalendarWeekdays;
    class StandardWorktime;
    class Relation;
    class ResourceGroup;
    class Resource;
    class Accounts;
    class Account;
    class ScheduleManager;
    class Schedule;
    class NodeSchedule;
    class WBSDefinition;
    class WorkPackage;
    class Documents;
    class Estimate;
    class ResourceGroupRequest;
    class ResourceRequest;
    class Completion;
    class Appointment;
    class AppointmentIntervalList;
    class AppointmentInterval;

class KPLATOKERNEL_EXPORT KPlatoXmlLoaderBase : public QObject
{
    Q_OBJECT
public:
    KPlatoXmlLoaderBase();
    virtual ~KPlatoXmlLoaderBase() {}

    bool load( Project *project, const KoXmlElement &element, XMLLoaderObject &status );
    bool load( Task *task, const KoXmlElement &element, XMLLoaderObject &status );
    bool load( Calendar *Calendar, const KoXmlElement &element, XMLLoaderObject &status );
    bool load( CalendarDay *day, const KoXmlElement &element, XMLLoaderObject &status );
    bool load( CalendarWeekdays *weekdays, const KoXmlElement& element, XMLLoaderObject& status );
    bool load( StandardWorktime *swt, const KoXmlElement &element, XMLLoaderObject &status );
    bool load( Relation *relation, const KoXmlElement &element, XMLLoaderObject &status );
    bool load( ResourceGroup *rg, const KoXmlElement &element, XMLLoaderObject &status );
    bool load( Resource *resource, const KoXmlElement &element, XMLLoaderObject &status );
    bool load( Accounts &accounts, const KoXmlElement &element, XMLLoaderObject &status );
    bool load( Account *account, const KoXmlElement &element, XMLLoaderObject &status );
    bool load( Account::CostPlace *cp, const KoXmlElement &element, XMLLoaderObject &status );
    bool load( ScheduleManager *manager, const KoXmlElement &element, XMLLoaderObject &status );
    bool load( Schedule *schedule, const KoXmlElement &element, XMLLoaderObject &status );
    MainSchedule *loadMainSchedule( ScheduleManager* manager, const KoXmlElement &element, XMLLoaderObject& status);
    bool loadMainSchedule( MainSchedule* ms, const KoXmlElement &element, XMLLoaderObject& status);
    bool loadNodeSchedule( NodeSchedule* sch, const KoXmlElement &element, XMLLoaderObject& status);
    bool load( WBSDefinition &def, const KoXmlElement &element, XMLLoaderObject &status );
    bool load( Documents &documents, const KoXmlElement &element, XMLLoaderObject &status );
    bool load( Document *document, const KoXmlElement &element, XMLLoaderObject &status );
    bool load( Estimate *estimate, const KoXmlElement &element, XMLLoaderObject &status );
    bool load( ResourceGroupRequest *gr, const KoXmlElement &element, XMLLoaderObject &status );
    bool load( ResourceRequest *rr, const KoXmlElement &element, XMLLoaderObject &status );
    bool load( WorkPackage& wp, const KoXmlElement& element, XMLLoaderObject& status );
    bool loadWpLog( WorkPackage* wp, KoXmlElement &element, XMLLoaderObject status);
    bool load( Completion& completion, const KoXmlElement& element, XMLLoaderObject& status );
    bool load( Completion::UsedEffort *ue, const KoXmlElement& element, XMLLoaderObject& status );
    bool load( Appointment *appointment, const KoXmlElement& element, XMLLoaderObject& status, Schedule &sch );
    bool load( AppointmentIntervalList &lst, const KoXmlElement& element, XMLLoaderObject& status );
    bool load( AppointmentInterval &interval, const KoXmlElement& element, XMLLoaderObject& status );

};

} // namespace KPlato

#endif // KPLATOXMLLOADERBASE_H
