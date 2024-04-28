/* This file is part of the KDE project

   @@COPYRIGHT@@

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_TESTLOADING_H_ @ @UPPERCASE_NAME @ @
#define KCHART_TESTLOADING_H_ @ @UPPERCASE_NAME @ @

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
    void testLegend();
    // More tests go here, e.g.
    // void testPlotArea();
};

} // namespace KoChart

#endif // KCHART_TESTLOADING_H_@@UPPERCASE_NAME@@
