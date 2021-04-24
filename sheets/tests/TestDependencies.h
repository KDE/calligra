// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_DEPENDENCIES
#define CALLIGRA_SHEETS_TEST_DEPENDENCIES

#include <QObject>

namespace Calligra
{
namespace Sheets
{
class CellStorage;
class Map;
class Sheet;

class TestDependencies : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testCircleRemoval();
    void testCircles();
    void testDepths();
    void cleanupTestCase();

private:
    CellStorage* m_storage;
    Map* m_map;
    Sheet* m_sheet;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_DEPENDENCIES
