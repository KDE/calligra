/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_TESTDATASET_H
#define KCHART_TESTDATASET_H

// KoChart
#include "ChartProxyModel.h"
#include "ChartTableModel.h"
#include "TableSource.h"

using namespace KoChart;

class TestDataSet : public QObject
{
    Q_OBJECT

public:
    TestDataSet();

private Q_SLOTS:
    void initTestCase();

    // Tests DataSet::*Data() methods
    void testFooData();
    void testFooDataMultipleTables();

private:
    // m_source must be initialized before m_proxyModel
    TableSource m_source;
    ChartProxyModel m_proxyModel;
    ChartTableModel m_sourceModel1, m_sourceModel2;
    Table *m_table1, *m_table2;
};

#endif // KCHART_TESTDATASET_H
