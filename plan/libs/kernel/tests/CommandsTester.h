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

#ifndef COMMANDSTESTER_H
#define COMMANDSTESTER_H

#include <QtTest/QtTest>
#include <QObject>

namespace KPlato {

class Project;

class CommandsTester : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void testNamedCommand();

    void testCalendarAddCmd();
    void testCalendarRemoveCmd();
    void testCalendarMoveCmd();
    void testCalendarModifyNameCmd();
    void testCalendarModifyParentCmd();
    void testCalendarModifyTimeZoneCmd();
    void testCalendarAddDayCmd();
    void testCalendarRemoveDayCmd();
    void testCalendarModifyDayCmd();
    void testCalendarModifyStateCmd();
    void testCalendarModifyTimeIntervalCmd();
    void testCalendarAddTimeIntervalCmd();
    void testCalendarRemoveTimeIntervalCmd();
    void testCalendarModifyWeekdayCmd();
    void testCalendarModifyDateCmd();
    void testProjectModifyDefaultCalendarCmd();

private:
    Project *m_project;
};

} // namespace KPlato

#endif // COMMANDSTESTER_H
