/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008, 2010 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_TESTPROXYMODEL_H
#define KCHART_TESTPROXYMODEL_H

// KoChart
#include "ChartProxyModel.h"
#include "ChartTableModel.h"
#include "TableSource.h"

using namespace KoChart;

class TestProxyModel : public QObject
{
    Q_OBJECT

public:
    TestProxyModel();

private Q_SLOTS:
    void init();

    void testWithoutLabels();
    void testFirstRowAsLabel();
    void testFirstColumnAsLabel();
    void testFirstRowAndColumnAsLabels();
    void testRegionOrder();
    void testComplexRegions();
    void testTwoDimensions();
    void testThreeDimensions();

private:
    // m_source must be initialized before m_proxyModel
    TableSource m_source;
    ChartProxyModel m_proxyModel;
    ChartTableModel m_sourceModel;
    Table *m_table;
};

#endif // KCHART_TESTPROXYMODEL_H
