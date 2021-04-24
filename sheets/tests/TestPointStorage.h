// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef CALLIGRA_SHEETS_POINT_STORAGE_TEST
#define CALLIGRA_SHEETS_POINT_STORAGE_TEST

#include <QObject>

namespace Calligra
{
namespace Sheets
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
    void testColumnIteration();
    void testRowIteration();
    void testDimension();
    void testSubStorage();
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_POINT_STORAGE_TEST
