/* This file is part of the KDE project
   Copyright (C) 2012 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef GLOBALSEARCHTEST_H
#define GLOBALSEARCHTEST_H

#include <QtCore/QObject>

class TestGlobalSearch : public QObject
{
    Q_OBJECT
public:
    TestGlobalSearch(int argc, char **argv, bool goToEventLoop);
private Q_SLOTS:
    void initTestCase();
    void testGlobalSearch();
    void cleanupTestCase();
private:
    const int m_argc;
    char **m_argv;
    bool m_goToEventLoop;
};

#endif
