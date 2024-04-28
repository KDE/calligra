/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2009 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_TESTKDCHARTMODEL_H
#define KCHART_TESTKDCHARTMODEL_H

#include <QStandardItemModel>
#include <QTest>

#include "KChartModel.h"
#include "ModelObserver.h"
#include "TableSource.h"

using namespace KoChart;

class TestKChartModel : public QObject
{
    Q_OBJECT

public:
    TestKChartModel();

private Q_SLOTS:
    void init();
    void cleanup();
    void initTestCase();

    void testDataSetInsertion();
    void testDataSetInsertionAndRemoval();
    void testData();
    void testDataChanges();
    void testDataChangesWithTwoDimensions();

private:
    KChartModel *m_model;
    ModelObserver *m_testModel;

    // These are all only dummies, but we need them for valid CellRegions
    QStandardItemModel m_itemModel;
    TableSource m_source;
    Table *m_table;
};

#endif // KCHART_TESTKDCHARTMODEL_H
