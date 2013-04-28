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

#ifndef KPlato_SchedulerTester_h
#define KPlato_SchedulerTester_h

#include <QtTest>
#include <ktempdir.h>

class KoXmlDocument;

namespace KPlato
{
class Node;

class SchedulerTester : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void test();

private:
    void initTimezone();
    void cleanupTimezone();
    void removeDir(const QString &dir);
    QStringList data();
    void testProject(const QString &fname, const KoXmlDocument &doc );
    void compare( const QString &fname, Node *n, long id1, long id2 );
};

} //namespace KPlato

#endif
