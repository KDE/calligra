/* This file is part of the KDE project
   Copyright (C) 2012 Dag Andersen <danders@get2net.dk>

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

#ifndef KPlato_WorkPackageProxyModelTester_h
#define KPlato_WorkPackageProxyModelTester_h

#include <QtTest/QtTest>

#include "kptworkpackagemodel.h"
#include "kptproject.h"

namespace KPlato
{

class Task;
class ScheduleManager;

class WorkPackageProxyModelTester : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void testInsert();
    void testRemove();
    void testMove();
    void testMoveChild();

private:
    Task *addTask( Node *parent, int after );
    void removeTask( Node *task );
    void moveTask( Node *task, Node *newParent, int newPos );

private:
    WorkPackageProxyModel m_model;
    Project project;
    ScheduleManager *sm;

};

} //namespace KPlato

#endif
