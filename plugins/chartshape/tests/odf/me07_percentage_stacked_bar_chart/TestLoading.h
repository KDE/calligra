/* This file is part of the KDE project

   @@COPYRIGHT@@

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_TESTLOADING_H_ME07_PERCENTAGE_STACKED_BAR_CHART
#define KCHART_TESTLOADING_H_ME07_PERCENTAGE_STACKED_BAR_CHART

#include "../TestLoadingBase.h"

#include <QStandardItemModel>

namespace KoChart
{

class TestLoading : public TestLoadingBase
{
    Q_OBJECT

public:
    TestLoading();

private Q_SLOTS:
    void initTestCase() override;
    void testInternalTable();
    void testDataSets();
    void testLegend();

private:
    /// Faked data model of sheet embedding this chart
    QStandardItemModel m_sheet;
};

} // namespace KoChart

#endif // KCHART_TESTLOADING_H_ME07_PERCENTAGE_STACKED_BAR_CHART
