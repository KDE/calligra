/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_POINT_STORAGE_BENCHMARK
#define CALLIGRA_SHEETS_POINT_STORAGE_BENCHMARK

#include <QObject>

namespace Calligra
{
namespace Sheets
{

class PointStorageBenchmark : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testInsertionPerformance_loadingLike();
    void testInsertionPerformance_singular();
    void testLookupPerformance_data();
    void testLookupPerformance();
    void testInsertColumnsPerformance();
    void testDeleteColumnsPerformance();
    void testInsertRowsPerformance();
    void testDeleteRowsPerformance();
    void testShiftLeftPerformance();
    void testShiftRightPerformance();
    void testShiftUpPerformance();
    void testShiftDownPerformance();
    void testIterationPerformance_data();
    void testIterationPerformance();
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_POINT_STORAGE_BENCHMARK
