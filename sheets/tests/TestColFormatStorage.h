// This file is part of the KDE project
// SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>
// SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef TESTCOLFORMATSTORAGE_H
#define TESTCOLFORMATSTORAGE_H

#include <QObject>

namespace Calligra
{
namespace Sheets
{

class Map;
class Sheet;

class TestColFormatStorage : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testColWidth();
    void testHidden();
    void testFiltered();
    void testHiddenOrFiltered();
    void testVisibleWidth();
    void testTotalColWidth();
    void testTotalVisibleColWidth();
    void testColForPosition();
    void testPageBreak();
    void testIsDefaultCol();
    void testSetDefault();
    void testLastNonDefaultCol();
    void testColsAreEqual();
    void testInsertCols();
    void testRemoveCols();

private:
    Map *m_map;
    Sheet *m_sheet;
};

} // namespace Sheets
} // namespace Calligra

#endif // TESTCOLFORMATSTORAGE_H
