// This file is part of the KDE project
// SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef TESTROWFORMATSTORAGE_H
#define TESTROWFORMATSTORAGE_H

#include <QObject>

namespace Calligra
{
namespace Sheets
{

class Map;
class Sheet;

class TestRowFormatStorage : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testRowHeight();
    void testHidden();
    void testFiltered();
    void testHiddenOrFiltered();
    void testVisibleHeight();
    void testTotalRowHeight();
    void testTotalVisibleRowHeight();
    void testRowForPosition();
    void testPageBreak();
    void testIsDefaultRow();
    void testSetDefault();
    void testLastNonDefaultRow();
    void testRowsAreEqual();
    void testInsertRows();
    void testRemoveRows();

private:
    Map *m_map;
    Sheet *m_sheet;
};

} // namespace Sheets
} // namespace Calligra

#endif // TESTROWFORMATSTORAGE_H
