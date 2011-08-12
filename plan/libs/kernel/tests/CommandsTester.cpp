/* This file is part of the KDE project
  Copyright (C) 2011 Pierre Stirnweiss <pstirnweiss@googlemail.com>

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

#include "CommandsTester.h"

#include <kptcommand.h>

#include <kptcalendar.h>
#include <kptproject.h>
#include <kptresource.h>

#include <qtest_kde.h>
#include <kdebug.h>

namespace KPlato {

void CommandsTester::initTestCase()
{
    m_project = new Project();
    m_project->setId( m_project->uniqueNodeId() );
    m_project->registerNodeId(m_project); //->rootTask());
    // standard worktime defines 8 hour day as default
    QVERIFY( m_project->standardWorktime() );
    QCOMPARE( m_project->standardWorktime()->day(), 8.0 );
/*    m_calendar = new Calendar();
    m_calendar->setDefault( true );
    QTime t1( 9, 0, 0 );
    QTime t2 ( 17, 0, 0 );
    int length = t1.msecsTo( t2 );
    for ( int i=1; i <= 7; ++i ) {
        CalendarDay *d = m_calendar->weekday( i );
        d->setState( CalendarDay::Working );
        d->addInterval( t1, length );
    }
    m_project->addCalendar( m_calendar );
*/
}

void CommandsTester::cleanupTestCase()
{
    delete m_project;
}

void CommandsTester::testNamedCommand()
{
    //todo: test the schedule handling
}

void CommandsTester::testCalendarAddCmd()
{
    Calendar *calendar1 = new Calendar();
    Calendar *calendar2 = new Calendar();

    CalendarAddCmd *cmd1 = new CalendarAddCmd(m_project, calendar1, 0, 0);
    cmd1->execute();
    QVERIFY(m_project->calendarAt(0) == calendar1);
    CalendarAddCmd *cmd2 = new CalendarAddCmd(m_project, calendar2, 0, calendar1);
    cmd2->execute();
    QVERIFY(calendar1->childAt(0) == calendar2);
    cmd2->unexecute();
    QVERIFY(!calendar1->childCount());
    cmd1->unexecute();
    QVERIFY(!m_project->calendarCount());

    delete cmd1;
    delete cmd2; //These also delete the calendars
}

void CommandsTester::testCalendarRemoveCmd()
{
    Calendar *calendar1 = new Calendar();
    Calendar *calendar2 = new Calendar();
    Calendar *calendar3 = new Calendar();
    Calendar *calendar4 = new Calendar();

    ResourceGroup *group1 = new ResourceGroup();
    Resource *resource1 = new Resource();

    m_project->addCalendar(calendar1);
    m_project->addCalendar(calendar2, calendar1);
    m_project->addCalendar(calendar3);
    m_project->addCalendar(calendar4);
    m_project->setDefaultCalendar(calendar3);
    m_project->addResourceGroup(group1);
    m_project->addResource(group1, resource1);
    resource1->setCalendar(calendar4);
    QVERIFY(m_project->calendarAt(0) == calendar1);
    QVERIFY(calendar1->childAt(0) == calendar2);
    QVERIFY(m_project->calendarAt(1) == calendar3);
    QVERIFY(m_project->calendarAt(2) == calendar4);
    QVERIFY(m_project->resourceGroupAt(0) == group1);
    QVERIFY(group1->resourceAt(0) == resource1);
    QVERIFY(m_project->defaultCalendar() == calendar3);
    QVERIFY(resource1->calendar(true) == calendar4);

    CalendarRemoveCmd *cmd1 = new CalendarRemoveCmd(m_project, calendar1); //calendar with one child calendar
    CalendarRemoveCmd *cmd2 = new CalendarRemoveCmd(m_project, calendar2); //simple calendar
    CalendarRemoveCmd *cmd3 = new CalendarRemoveCmd(m_project, calendar3); //default project calendar
    CalendarRemoveCmd *cmd4 = new CalendarRemoveCmd(m_project, calendar4); //calendar used by a resource

    cmd3->execute();
    QVERIFY(!m_project->calendars().contains(calendar3));
    QVERIFY(m_project->defaultCalendar() != calendar3);

    cmd4->execute();
    QVERIFY(!m_project->calendars().contains(calendar4));
    QVERIFY(resource1->calendar(true) != calendar4);

    cmd2->execute();
    QVERIFY(!calendar1->calendars().contains(calendar2));

    cmd2->unexecute();
    QVERIFY(calendar1->calendars().contains(calendar2));

    cmd1->execute();
    QVERIFY(!m_project->calendars().contains(calendar1));

    cmd1->unexecute();
    QVERIFY(m_project->calendars().contains(calendar1));
    QVERIFY(calendar1->calendars().contains(calendar2));

    cmd3->unexecute();
    QVERIFY(m_project->calendars().contains(calendar3));
    QVERIFY(m_project->defaultCalendar() == calendar3);

    cmd4->unexecute();
    QVERIFY(m_project->calendars().contains(calendar4));
    QVERIFY(resource1->calendar(true) == calendar4);

    m_project->takeCalendar(calendar4);
    m_project->takeCalendar(calendar3);
    m_project->takeCalendar(calendar2);
    m_project->takeCalendar(calendar1);
    m_project->takeResource(group1, resource1);
    m_project->takeResourceGroup(group1);
    delete cmd1;
    delete cmd2;
    delete cmd3;
    delete cmd4;
    delete calendar4;
    delete calendar3;
    delete calendar2;
    delete calendar1;
    delete resource1;
    delete group1;
}

void CommandsTester::testCalendarMoveCmd()
{
    Calendar *calendar1 = new Calendar();
    Calendar *calendar2 = new Calendar();
    m_project->addCalendar(calendar1);
    m_project->addCalendar(calendar2);
    QVERIFY(m_project->calendarCount() == 2);

    CalendarMoveCmd *cmd1 = new CalendarMoveCmd(m_project, calendar1, 0, calendar2);
    cmd1->execute();
    QVERIFY(calendar2->childAt(0) == calendar1);
    cmd1->unexecute();
    QVERIFY(!calendar2->childCount());

    delete cmd1;
    m_project->takeCalendar(calendar1);
    m_project->takeCalendar(calendar2);
    delete calendar1;
    delete calendar2;
}

void CommandsTester::testCalendarModifyNameCmd()
{
    Calendar *calendar1 = new Calendar(QString("test1"));
    m_project->addCalendar(calendar1);
    QVERIFY(m_project->calendars().contains(calendar1));
    QVERIFY(calendar1->name() == "test1");

    CalendarModifyNameCmd *cmd1 = new CalendarModifyNameCmd(calendar1, QString("test2"));
    cmd1->execute();
    QVERIFY(calendar1->name() == "test2");
    cmd1->unexecute();
    QVERIFY(calendar1->name() == "test1");

    m_project->takeCalendar(calendar1);

    delete cmd1;
    delete calendar1;
}

void CommandsTester::testCalendarModifyParentCmd()
{
    Calendar *calendar1 = new Calendar();
    calendar1->setTimeZone(KTimeZone("t1"));
    Calendar *calendar2 = new Calendar();
    calendar2->setTimeZone(KTimeZone("t2"));
    m_project->addCalendar(calendar1);
    m_project->addCalendar(calendar2);
    QVERIFY(m_project->calendarCount() == 2);
    QVERIFY(calendar1->timeZone().name() == "t1");
    QVERIFY(calendar2->timeZone().name() == "t2");

    CalendarModifyParentCmd *cmd1 = new CalendarModifyParentCmd(m_project, calendar1, calendar2);
    cmd1->execute();
    QVERIFY(calendar2->childAt(0) == calendar1);
    QVERIFY(calendar1->timeZone().name() == "t2");
    cmd1->unexecute();
    QVERIFY(!calendar2->childCount());
    QVERIFY(calendar1->timeZone().name() == "t1");

    delete cmd1;
    m_project->takeCalendar(calendar1);
    m_project->takeCalendar(calendar2);
    delete calendar1;
    delete calendar2;
}

void CommandsTester::testCalendarModifyTimeZoneCmd()
{
    Calendar *calendar1 = new Calendar();
    calendar1->setTimeZone(KTimeZone("t1"));
    m_project->addCalendar(calendar1);
    QVERIFY(m_project->calendarCount() == 1);
    QVERIFY(calendar1->timeZone().name() == "t1");

    CalendarModifyTimeZoneCmd *cmd1 = new CalendarModifyTimeZoneCmd(calendar1, KTimeZone("t2"));
    cmd1->execute();
    QVERIFY(calendar1->timeZone().name() == "t2");
    cmd1->unexecute();
    QVERIFY(calendar1->timeZone().name() == "t1");

    delete cmd1;
    m_project->takeCalendar(calendar1);
    delete calendar1;
}

void CommandsTester::testCalendarAddDayCmd()
{
    Calendar *calendar1 = new Calendar();
    CalendarDay *day1 = new CalendarDay();
    m_project->addCalendar(calendar1);
    QVERIFY(m_project->calendarCount() == 1);
    QVERIFY(calendar1->days().isEmpty());

    CalendarAddDayCmd *cmd1 = new CalendarAddDayCmd(calendar1, day1);
    cmd1->execute();
    QVERIFY(calendar1->days().contains(day1));
    cmd1->unexecute();
    QVERIFY(calendar1->days().isEmpty());

    delete cmd1; //also deletes day1
    m_project->takeCalendar(calendar1);
    delete calendar1;
}

void CommandsTester::testCalendarRemoveDayCmd()
{
    Calendar *calendar1 = new Calendar();
    CalendarDay *day1 = new CalendarDay(QDate(1974,12,19));
    calendar1->addDay(day1);
    m_project->addCalendar(calendar1);
    QVERIFY(m_project->calendarCount() == 1);
    QVERIFY(calendar1->days().contains(day1));

    CalendarRemoveDayCmd *cmd1 = new CalendarRemoveDayCmd(calendar1, day1);
    cmd1->execute();
    QVERIFY(calendar1->days().isEmpty());
    cmd1->unexecute();
    QVERIFY(calendar1->days().contains(day1));

    CalendarRemoveDayCmd *cmd2 = new CalendarRemoveDayCmd(calendar1, QDate(1974, 12, 19));
    cmd2->execute();
    QVERIFY(calendar1->days().isEmpty());
    cmd2->unexecute();
    QVERIFY(calendar1->days().contains(day1));

    calendar1->takeDay(day1);
    delete cmd1;
    delete cmd2;
    delete day1;
    m_project->takeCalendar(calendar1);
    delete calendar1;
}

void CommandsTester::testCalendarModifyDayCmd()
{
    Calendar *calendar1 = new Calendar();
    CalendarDay *day1 = new CalendarDay(QDate(1974,12,19));
    CalendarDay *day2 = new CalendarDay(QDate(2011,03,11));
    calendar1->addDay(day1);
    m_project->addCalendar(calendar1);
    QVERIFY(m_project->calendarCount() == 1);
    QVERIFY(calendar1->days().contains(day1));

    CalendarModifyDayCmd *cmd1 = new CalendarModifyDayCmd(calendar1, day2);
    cmd1->execute();
    QVERIFY(calendar1->days().contains(day2));
    cmd1->unexecute();
    QVERIFY(calendar1->days().contains(day1));
    QVERIFY(!calendar1->days().contains(day2));

    calendar1->takeDay(day1);
    delete cmd1; //also deletes day2
    delete day1;
    m_project->takeCalendar(calendar1);
    delete calendar1;
}

void CommandsTester::testCalendarModifyStateCmd()
{
    Calendar *calendar1 = new Calendar();
    CalendarDay *day1 = new CalendarDay(QDate(1974,12,19));
    day1->setState(CalendarDay::Working);
    calendar1->addDay(day1);
    CalendarDay *day2 = new CalendarDay(QDate(2011,03,26));
    TimeInterval *interval1 = new TimeInterval(QTime(8,0), 8);
    day2->addInterval(interval1);
    day2->setState(CalendarDay::Working);
    calendar1->addDay(day2);
    m_project->addCalendar(calendar1);
    QVERIFY(m_project->calendarCount() == 1);
    QVERIFY(calendar1->days().contains(day1));
    QVERIFY(day1->state() == CalendarDay::Working);
    QVERIFY(calendar1->days().contains(day2));
    QVERIFY(day2->timeIntervals().contains(interval1));

    CalendarModifyStateCmd *cmd1 = new CalendarModifyStateCmd(calendar1, day1, CalendarDay::NonWorking);
    cmd1->execute();
    QVERIFY(day1->state() == CalendarDay::NonWorking);
    cmd1->unexecute();
    QVERIFY(day1->state() == CalendarDay::Working);

    CalendarModifyStateCmd *cmd2 = new CalendarModifyStateCmd(calendar1, day2, CalendarDay::NonWorking);
    cmd2->execute();
    QVERIFY(day2->state() == CalendarDay::NonWorking);
    QVERIFY(!day2->timeIntervals().count());
    cmd2->unexecute();
    QVERIFY(day2->timeIntervals().contains(interval1));
    QVERIFY(day2->state() == CalendarDay::Working);

    calendar1->takeDay(day1);
    calendar1->takeDay(day2);
    day2->clearIntervals();
    m_project->takeCalendar(calendar1);
    delete cmd1;
    delete cmd2;
    delete interval1;
    delete day1;
    delete day2;
    delete calendar1;
}

void CommandsTester::testCalendarModifyTimeIntervalCmd()
{
    Calendar *calendar1 = new Calendar();
    CalendarDay *day1 = new CalendarDay(QDate(1974,12,19));
    TimeInterval *interval1 = new TimeInterval(QTime(8,0), 3600000);
    TimeInterval *interval2 = new TimeInterval(QTime(12,0),7200000);
    day1->addInterval(interval1);
    calendar1->addDay(day1);
    m_project->addCalendar(calendar1);
    QVERIFY(m_project->calendarCount() == 1);
    QVERIFY(calendar1->days().contains(day1));
    QVERIFY(day1->timeIntervals().contains(interval1));
    QVERIFY(interval1->startTime() == QTime(8,0));
    QVERIFY(interval1->hours() == 1);

    CalendarModifyTimeIntervalCmd *cmd1 = new CalendarModifyTimeIntervalCmd(calendar1, *interval2, interval1);
    cmd1->execute();
    QVERIFY(interval1->startTime() == QTime(12,0));
    QVERIFY(interval1->hours() == 2);
    cmd1->unexecute();
    QVERIFY(interval1->startTime() == QTime(8,0));
    QVERIFY(interval1->hours() == 1);

    day1->clearIntervals();
    calendar1->takeDay(day1);
    m_project->takeCalendar(calendar1);
    delete cmd1;
    delete interval1;
    delete interval2;
    delete day1;
    delete calendar1;
}

void CommandsTester::testCalendarAddTimeIntervalCmd()
{
    Calendar *calendar1 = new Calendar();
    CalendarDay *day1 = new CalendarDay(QDate(1974,12,19));
    TimeInterval *interval1 = new TimeInterval(QTime(8,0), 3600000);
    calendar1->addDay(day1);
    m_project->addCalendar(calendar1);
    QVERIFY(m_project->calendarCount() == 1);
    QVERIFY(calendar1->days().contains(day1));

    CalendarAddTimeIntervalCmd *cmd1 = new CalendarAddTimeIntervalCmd(calendar1, day1, interval1);
    cmd1->execute();
    QVERIFY(day1->timeIntervals().contains(interval1));
    cmd1->unexecute();
    QVERIFY(!day1->timeIntervals().contains(interval1));

    calendar1->takeDay(day1);
    m_project->takeCalendar(calendar1);
    delete cmd1; //also delete interval
    delete day1;
    delete calendar1;
}

void CommandsTester::testCalendarRemoveTimeIntervalCmd()
{
    Calendar *calendar1 = new Calendar();
    CalendarDay *day1 = new CalendarDay(QDate(1974,12,19));
    TimeInterval *interval1 = new TimeInterval(QTime(8,0), 3600000);
    calendar1->addDay(day1);
    calendar1->addWorkInterval(day1, interval1);
    m_project->addCalendar(calendar1);
    QVERIFY(m_project->calendarCount() == 1);
    QVERIFY(calendar1->days().contains(day1));
    QVERIFY(day1->timeIntervals().contains(interval1));

    CalendarRemoveTimeIntervalCmd *cmd1 = new CalendarRemoveTimeIntervalCmd(calendar1, day1, interval1);
    cmd1->execute();
    QVERIFY(!day1->timeIntervals().contains(interval1));
    cmd1->unexecute();
    QVERIFY(day1->timeIntervals().contains(interval1));

    day1->clearIntervals();
    calendar1->takeDay(day1);
    m_project->takeCalendar(calendar1);
    delete cmd1;
    delete interval1;
    delete day1;
    delete calendar1;
}

void CommandsTester::testCalendarModifyWeekdayCmd()
{
    Calendar *calendar1 = new Calendar();
    CalendarDay *day1 = new CalendarDay(QDate(1974,12,19));
    TimeInterval *interval1 = new TimeInterval(QTime(8,0), 3600000);
    day1->setState(CalendarDay::Working);
    day1->addInterval(interval1);
    CalendarDay *day2 = new CalendarDay(QDate(2011,3,26));
    TimeInterval *interval2 = new TimeInterval(QTime(12,0), 7200000);
    day2->setState(CalendarDay::NonWorking);
    day2->addInterval(interval2);
    m_project->addCalendar(calendar1);
    calendar1->setWeekday(1, *day1);
    QVERIFY(m_project->calendarCount() == 1);
    QVERIFY(calendar1->weekday(1)->state() == day1->state());
    QVERIFY(calendar1->weekday(1)->intervalAt(0)->startTime() == day1->intervalAt(0)->startTime());
    QVERIFY(calendar1->weekday(1)->intervalAt(0)->hours() == day1->intervalAt(0)->hours());

    CalendarModifyWeekdayCmd *cmd1 = new CalendarModifyWeekdayCmd(calendar1, 1, day2);
    cmd1->execute();
    QVERIFY(calendar1->weekday(1)->state() == day2->state());
    QVERIFY(calendar1->weekday(1)->intervalAt(0)->startTime() == day2->intervalAt(0)->startTime());
    QVERIFY(calendar1->weekday(1)->intervalAt(0)->hours() == day2->intervalAt(0)->hours());
    cmd1->unexecute();
    QVERIFY(calendar1->weekday(1)->state() == day1->state());
    QVERIFY(calendar1->weekday(1)->intervalAt(0)->startTime() == day1->intervalAt(0)->startTime());
    QVERIFY(calendar1->weekday(1)->intervalAt(0)->hours() == day1->intervalAt(0)->hours());

    m_project->takeCalendar(calendar1);
    day1->clearIntervals();
    day2->clearIntervals();
    delete cmd1; //also deletes day2
    delete day1;
    delete interval1;
    delete interval2;
    delete calendar1;
}

void CommandsTester::testCalendarModifyDateCmd()
{
    Calendar *calendar1 = new Calendar();
    CalendarDay *day1 = new CalendarDay(QDate(1974,12,19));
    calendar1->addDay(day1);
    m_project->addCalendar(calendar1);
    QVERIFY(m_project->calendarCount() == 1);
    QVERIFY(calendar1->days().contains(day1));
    QVERIFY(day1->date() == QDate(1974,12,19));

    CalendarModifyDateCmd *cmd1 = new CalendarModifyDateCmd(calendar1, day1, QDate(2011,3,26));
    cmd1->execute();
    QVERIFY(day1->date() == QDate(2011,3,26));
    cmd1->unexecute();
    QVERIFY(day1->date() == QDate(1974,12,19));

    calendar1->takeDay(day1);
    m_project->takeCalendar(calendar1);
    delete cmd1;
    delete day1;
    delete calendar1;
}

void CommandsTester::testProjectModifyDefaultCalendarCmd()
{
    Calendar *calendar1 = new Calendar();
    Calendar *calendar2 = new Calendar();
    m_project->addCalendar(calendar1);
    m_project->addCalendar(calendar2);
    m_project->setDefaultCalendar(calendar1);
    QVERIFY(m_project->calendars().contains(calendar1));
    QVERIFY(m_project->calendars().contains(calendar2));
    QVERIFY(m_project->defaultCalendar() == calendar1);

    ProjectModifyDefaultCalendarCmd *cmd1 = new ProjectModifyDefaultCalendarCmd(m_project, calendar2);
    cmd1->execute();
    QVERIFY(m_project->defaultCalendar() == calendar2);
    cmd1->unexecute();
    QVERIFY(m_project->defaultCalendar() == calendar1);

    m_project->takeCalendar(calendar1);
    m_project->takeCalendar(calendar2);
    delete calendar1;
    delete calendar2;
    delete cmd1;
}
} // namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::CommandsTester)

#include "CommandsTester.moc"

