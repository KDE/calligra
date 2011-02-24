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

#ifndef KPlato_PerformanceTester_h
#define KPlato_PerformanceTester_h

#include <QtTest/QtTest>

#include "kptdatetime.h"
#include "kptproject.h"
#include "kptduration.h"

namespace QTest
{
    template<>
    char *toString(const KPlato::DateTime &dt)
    {
        return toString( dt.toString() );
    }
    template<>
    char *toString(const KPlato::Duration &d)
    {
        return toString( d.toString() );
    }
}

namespace KPlato
{
class Task;

class PerformanceTester : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void bcwsPrDayTask();
    void bcwpPrDayTask();
    void acwpPrDayTask();
    void bcwsMilestone();
    void bcwpMilestone();
    void acwpMilestone();

    void bcwsPrDayTaskMaterial();
    void bcwpPrDayTaskMaterial();
    void acwpPrDayTaskMaterial();

    void bcwsPrDayProject();
    void bcwpPrDayProject();
    void acwpPrDayProject();

private:
    Project *p1;
    Resource *r1;
    Resource *r2; // material
    Resource *r3; // material
    Task *s1;
    Task *t1;
    Task *s2;
    Task *m1;
};

} //namespace KPlato

#endif
