/* This file is part of the KDE project
   Copyright (C) 2017 Dag Andersen <danders@get2net.dk>

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

#ifndef KPlato_AccountsCommandTester_h
#define KPlato_AccountsCommandTester_h

#include <QObject>

namespace KPlato
{

class Task;
class Project;
class Resource;
class Calendar;
class ScheduleManager;

class AccountsCommandTester : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void init();
    void cleanup();

    void addAccount();
    void removeAccount();
    void costPlace();

private:
    void printDebug( long id ) const;
    void printSchedulingLog( const ScheduleManager &sm ) const;

    Project *m_project;
    Calendar *m_calendar;
    Task *m_task;
    Resource *m_resource;

};

} //namespace KPlato

#endif
