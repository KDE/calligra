/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_POINT_STORAGE_TEST
#define KSPREAD_POINT_STORAGE_TEST

#include <QtTest/QtTest>

namespace KSpread
{

class PointStorageTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testInsertion();
    void testLookup();
    void testDeletion();
    void testInsertColumns();
    void testDeleteColumns();
    void testInsertRows();
    void testDeleteRows();
    void testShiftLeft();
    void testShiftRight();
    void testShiftUp();
    void testShiftDown();
    void testShiftDownUp();
    void testFirstInColumn();
    void testFirstInRow();
    void testLastInColumn();
    void testLastInRow();
    void testNextInColumn();
    void testNextInRow();
    void testPrevInColumn();
    void testPrevInRow();
    void testIteration();
    void testSubStorage();
};

} // namespace KSpread

#endif // KSPREAD_POINT_STORAGE_TEST
