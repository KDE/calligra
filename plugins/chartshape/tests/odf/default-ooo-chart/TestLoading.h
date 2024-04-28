/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2010 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
     Contact: Suresh Chande suresh.chande@nokia.com

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_TESTLOADING_H_DEFAULT_OOO_CHART
#define KCHART_TESTLOADING_H_DEFAULT_OOO_CHART

// Base
#include "../TestLoadingBase.h"

namespace KoChart
{

class TestLoading : public TestLoadingBase
{
    Q_OBJECT

public:
    TestLoading();

private Q_SLOTS:
    void testLabels();
    void testInternalTable();
    void testDataSets();
    void testPlotArea();
    void testLegend();
};

} // namespace KoChart

#endif // KCHART_TESTLOADING_H_DEFAULT_OOO_CHART
