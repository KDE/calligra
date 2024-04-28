/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2010 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
     Contact: Suresh Chande suresh.chande@nokia.com

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_TESTLOADING_BASE
#define KCHART_TESTLOADING_BASE

// Qt
#include <QObject>
#include <QString>
#include <QTest>

// KoChart
#include "CellRegion.h"

class KoShape;

namespace KoChart
{

class ChartShape;
class Table;
class TableSource;
class Axis;

/// Class to hold common test methods and data
class LoadingTests
{
public:
    LoadingTests();

    // Helper methods to be used by test functions

    // 0) Generics
    void testElementIsVisible(KoShape *element, bool shouldBeVisible);

    // 1) Legend
    void testLegendElements(QStringList labels);

    // 2) Data Sets
    void testDataSetCellRegions(int dataSetNr,
                                CellRegion yDataRegion,
                                CellRegion labelDataRegion = CellRegion(),
                                CellRegion categoryDataRegion = CellRegion(),
                                CellRegion xDataRegion = CellRegion(),
                                CellRegion customDataRegion = CellRegion());

    // 3) Internal Table
    void testHasOnlyInternalTable();
    void testInternalTableSize(int rowCount, int colCount);

    // 4) Title, Subtitle and Footer
    void testTitleText(const QString &text);
    void testSubTitleText(const QString &text);
    void testFooterText(const QString &text);

    // 5) Axes
    void testAxisTitle(Axis *axis, const QString &text);

    Table *internalTable();
    TableSource *tableSource();

    ChartShape *m_chart;
};

/**
 * Base class for every ODF-loading related unit test.
 *
 * The philosophy is basically to do as many tests as possible by
 * using a helper method from this base class. Lines like these:

 * testLegendElements(QStringList() << "Row 1" << "Row 2" << "Row 3");
 *
 * are much more readable than using flat code by using copy&paste and doing
 * the same gets and checks over again in multiple unit tests.
 *
 */
class TestLoadingBase : public QObject, public LoadingTests
{
    Q_OBJECT

public:
    TestLoadingBase();

protected Q_SLOTS:
    virtual void initTestCase();
    virtual void cleanupTestCase();
};

} // namespace KoChart

namespace QTest
{
template<>
char *toString(const KoChart::CellRegion &region);
}

#endif // KCHART_TESTLOADING_BASE
