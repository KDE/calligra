// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef KSPREAD_CLUSTER_BENCHMARK
#define KSPREAD_CLUSTER_BENCHMARK

#include <QObject>

namespace KSpread
{

class ClusterBenchmark : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testInsertionPerformance();
    void testLookupPerformance();
    void testInsertColumnsPerformance();
    void testDeleteColumnsPerformance();
    void testInsertRowsPerformance();
    void testDeleteRowsPerformance();
    void testShiftLeftPerformance();
    void testShiftRightPerformance();
    void testShiftUpPerformance();
    void testShiftDownPerformance();
};

} // namespace KSpread

#endif // KSPREAD_CLUSTER_BENCHMARK
