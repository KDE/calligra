/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_TEST_REGION
#define KSPREAD_TEST_REGION

#include <QtGui>
#include <QtTest/QtTest>

namespace KSpread
{
class Map;

class TestRegion : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testComparison();
    void testFixation();
    void testSheet();
    void testExtrem();
    void cleanupTestCase();

private:
    Map* m_map;
};

} // namespace KSpread

#endif // KSPREAD_TEST_REGION
