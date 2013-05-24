/* This file is part of the KDE project
   Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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

#ifndef KPlato_InsertProjectTester_h
#define KPlato_InsertProjectTester_h

#include <QtTest>

namespace KPlato
{

class MainDocument;
class Account;
class ResourceGroup;
class Resource;
class Task;
class Relation;
class Calendar;

class InsertProjectTester : public QObject
{
    Q_OBJECT
private slots:
    void testAccount();
    void testCalendar();
    void testDefaultCalendar();
    void testResourceGroup();
    void testResource();
    void testTeamResource();
    void testResourceAccount();
    void testResourceCalendar();
    void testTask();
    void testGroupRequest();
    void testResourceRequest();
    void testTeamResourceRequest();
    void testDependencies();
    void testExistingResourceAccount();
    void testExistingResourceCalendar();
    void testExistingResourceRequest();
    void testExistingRequiredResourceRequest();
    void testExistingTeamResourceRequest();

private:
    Account *addAccount( MainDocument &part, Account *parent = 0 );
    Calendar *addCalendar( MainDocument &part );
    ResourceGroup *addResourceGroup( MainDocument &part );
    Resource *addResource( MainDocument &part, ResourceGroup *g = 0 );
    Task *addTask( MainDocument &part );
    void addGroupRequest( MainDocument &part );
    void addResourceRequest( MainDocument &part );
    Relation *addDependency( MainDocument &part, Task *t1, Task *t2 );
};

}

#endif
