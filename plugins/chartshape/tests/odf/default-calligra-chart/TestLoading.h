/* This file is part of the KDE project

   @@COPYRIGHT@@

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_TESTLOADING_H_DEFAULT_CALLIGRA_CHART
#define KCHART_TESTLOADING_H_DEFAULT_CALLIGRA_CHART

#include "../TestLoadingBase.h"

namespace KoChart
{

class TestLoading : public TestLoadingBase
{
    Q_OBJECT

public:
    TestLoading();

private Q_SLOTS:
    /// Tests title, subtitle and footer
    void testLabels();
    void testInternalTable();
    void testDataSets();
    void testPlotArea();
    void testLegend();
    void testAxes();
};

} // namespace KoChart

#endif // KCHART_TESTLOADING_H_DEFAULT_CALLIGRA_CHART
