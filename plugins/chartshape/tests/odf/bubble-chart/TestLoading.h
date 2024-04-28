/* This file is part of the KDE project

   @@COPYRIGHT@@

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_TESTLOADING_H_BUBBLE_CHART
#define KCHART_TESTLOADING_H_BUBBLE_CHART

#include "../TestMultipleFiles.h"

namespace KoChart
{

class TestLoading : public TestMultipleFiles
{
    Q_OBJECT

public:
    TestLoading();

private Q_SLOTS:
    void testInvalidOOChart();
    void testValidOOChart();

private:
    void testLabels();
    void testInternalTable();
    void testDataSets();
    void testPlotArea();
    void testLegend();
    void testAxes();
};

} // namespace KoChart

#endif // KCHART_TESTLOADING_H_BUBBLE_CHART
