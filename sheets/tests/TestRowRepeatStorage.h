/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef TESTROWREPEATSTORAGE_H
#define TESTROWREPEATSTORAGE_H

#include <QObject>

namespace Calligra
{
namespace Sheets
{

class TestRowRepeatStorage : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testEmptyStorage();
    void testSimpleSetRowRepeat();
    void testOverlappingRanges_data();
    void testOverlappingRanges();
    void testComplexSetRowRepeat();
    void testInsertRowsEmpty();
    void testInsertRowsBetween();
    void testInsertRowsMiddle();
    void testRemoveRowsEmpty();
    void testRemoveRowsBetween();
    void testRemoveRowsOverlap();
    void testInsertShiftDown1();
    void testInsertShiftDown2();
    void testInsertShiftDown3();
    void testInsertShiftDown4();
    void testRemoveShiftUp1();
    void testRemoveShiftUp2();
    void testRemoveShiftUp3();
    void testRemoveShiftUp4();
    void testInsertShiftRight();
    void testRemoveShiftLeft();
};

} // namespace Sheets
} // namespace Calligra

#endif // TESTROWREPEATSTORAGE_H
