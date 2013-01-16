/* This file is part of the KDE project
   Copyright (C) 2011 Dag Andersen <danders@get2net.dk>

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

#ifndef KPlato_TaskJuggler_h
#define KPlato_TaskJuggler_h

#include <QtTest>
#include <KTempDir>

namespace TJ {
    class Project;
}

namespace KPlato
{

class TaskJuggler : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void list();
    void projectTest();
    void oneTask();
    void oneResource();
    void allocation();
    void dependency();
    void scheduleResource();

    void scheduleDependencies();
    void scheduleConstraints();
    void resourceConflict();
    void units();

private:
    void initTimezone();
    void cleanupTimezone();

private:
    KTempDir m_tmp;
    TJ::Project *project;
};

} //namespace KPlato

#endif
