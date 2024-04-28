// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_REGION
#define CALLIGRA_SHEETS_TEST_REGION

#include <QObject>

namespace Calligra
{
namespace Sheets
{
class MapBase;

class TestRegion : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testComparison();
    void testFixation();
    void testSheet();
    void testExtrem();
    void testOps();
    void cleanupTestCase();

private:
    MapBase *m_map;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_REGION
