/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2009 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "TestKChartModel.h"

#include <QAbstractItemModel>
#include <QObject>
#include <QRect>

#include "CellRegion.h"
#include "DataSet.h"

using namespace KoChart;

TestKChartModel::TestKChartModel()
    : m_model(nullptr)
    , m_testModel(nullptr)
    , m_source()
    , m_table(nullptr)
{
}

void TestKChartModel::initTestCase()
{
    m_table = m_source.add("Table1", &m_itemModel);

    m_itemModel.setRowCount(3);
    m_itemModel.setColumnCount(11);

    // Vertical header data
    m_itemModel.setData(m_itemModel.index(1, 0), "Row 1");
    m_itemModel.setData(m_itemModel.index(2, 0), "Row 2");

    // Horizontal header data
    m_itemModel.setData(m_itemModel.index(0, 1), "Column 1");
    m_itemModel.setData(m_itemModel.index(0, 2), "Column 2");
    m_itemModel.setData(m_itemModel.index(0, 3), "Column 3");
    m_itemModel.setData(m_itemModel.index(0, 4), "Column 4");
    m_itemModel.setData(m_itemModel.index(0, 5), "Column 5");
    m_itemModel.setData(m_itemModel.index(0, 6), "Column 6");
    m_itemModel.setData(m_itemModel.index(0, 7), "Column 7");
    m_itemModel.setData(m_itemModel.index(0, 8), "Column 8");
    m_itemModel.setData(m_itemModel.index(0, 9), "Column 9");
    m_itemModel.setData(m_itemModel.index(0, 10), "Column 10");

    // First row
    m_itemModel.setData(m_itemModel.index(1, 1), 7.2);
    m_itemModel.setData(m_itemModel.index(1, 2), 1.8);
    m_itemModel.setData(m_itemModel.index(1, 3), 9.4);
    m_itemModel.setData(m_itemModel.index(1, 4), 1.5);
    m_itemModel.setData(m_itemModel.index(1, 5), 8.4);
    m_itemModel.setData(m_itemModel.index(1, 6), 2.9);
    m_itemModel.setData(m_itemModel.index(1, 7), 3.7);
    m_itemModel.setData(m_itemModel.index(1, 8), 5.5);
    m_itemModel.setData(m_itemModel.index(1, 9), 2.9);
    m_itemModel.setData(m_itemModel.index(1, 10), 5.3);

    // Second row
    m_itemModel.setData(m_itemModel.index(2, 1), 8.2);
    m_itemModel.setData(m_itemModel.index(2, 2), 2.8);
    m_itemModel.setData(m_itemModel.index(2, 3), 10.4);
    m_itemModel.setData(m_itemModel.index(2, 4), 2.5);
    m_itemModel.setData(m_itemModel.index(2, 5), 9.4);
    m_itemModel.setData(m_itemModel.index(2, 6), 3.9);
    m_itemModel.setData(m_itemModel.index(2, 7), 4.7);
    m_itemModel.setData(m_itemModel.index(2, 8), 6.5);
    m_itemModel.setData(m_itemModel.index(2, 9), 3.9);
    m_itemModel.setData(m_itemModel.index(2, 10), 6.3);
}

void TestKChartModel::init()
{
    m_model = new KChartModel(nullptr);
    m_testModel = new ModelObserver(m_model);
}

void TestKChartModel::cleanup()
{
    delete m_model;
    delete m_testModel;
}

void TestKChartModel::testDataSetInsertion()
{
    DataSet dataSet1(0);
    DataSet dataSet2(1);
    dataSet1.setYDataRegion(CellRegion(m_table, QRect(1, 1, 10, 1)));
    dataSet2.setYDataRegion(CellRegion(m_table, QRect(2, 1, 10, 1)));
    m_model->addDataSet(&dataSet1);
    m_model->addDataSet(&dataSet2);
    QCOMPARE(m_testModel->m_numRows, m_model->rowCount());
    QCOMPARE(m_testModel->m_numCols, m_model->columnCount());
}

void TestKChartModel::testDataSetInsertionAndRemoval()
{
    m_model->setDataDirection(Qt::Vertical);
    DataSet dataSet1(0);
    DataSet dataSet2(1);
    dataSet1.setYDataRegion(CellRegion(m_table, QRect(1, 1, 10, 1)));
    dataSet2.setYDataRegion(CellRegion(m_table, QRect(2, 1, 10, 1)));
    m_model->addDataSet(&dataSet1);
    m_model->addDataSet(&dataSet2);
    QCOMPARE(m_testModel->m_numRows, 10);
    QCOMPARE(m_testModel->m_numCols, 2);
    QCOMPARE(m_model->rowCount(), 10);
    QCOMPARE(m_model->columnCount(), 2);
    m_model->removeDataSet(&dataSet1);
    qInfo() << 4.1 << m_model->rowCount() << m_model->columnCount();
    m_model->removeDataSet(&dataSet2);
    QCOMPARE(m_testModel->m_numRows, 0);
    QCOMPARE(m_testModel->m_numCols, 0);
    QCOMPARE(m_model->columnCount(), 0);
    QCOMPARE(m_model->rowCount(), 0);
    m_model->addDataSet(&dataSet1);
    m_model->addDataSet(&dataSet2);
    QCOMPARE(m_testModel->m_numRows, 10);
    QCOMPARE(m_testModel->m_numCols, 2);
    QCOMPARE(m_model->rowCount(), 10);
    QCOMPARE(m_model->columnCount(), 2);

    DataSet dataSet3(2);
    dataSet3.setYDataRegion(CellRegion(m_table, QRect(3, 1, 10, 1)));
    m_model->addDataSet(&dataSet3);
    QCOMPARE(m_testModel->m_numRows, 10);
    QCOMPARE(m_testModel->m_numCols, 3);
    m_model->removeDataSet(&dataSet2);
    QCOMPARE(m_testModel->m_numRows, 10);
    QCOMPARE(m_testModel->m_numCols, 2);
    m_model->removeDataSet(&dataSet1);
    QCOMPARE(m_testModel->m_numRows, 10);
    QCOMPARE(m_testModel->m_numCols, 1);
    m_model->removeDataSet(&dataSet3);
    QCOMPARE(m_testModel->m_numRows, 0);
    QCOMPARE(m_testModel->m_numCols, 0);

    m_model->setDataDirection(Qt::Horizontal);
    QVERIFY(m_model->dataDirection() == Qt::Horizontal);
    m_model->addDataSet(&dataSet1);
    QCOMPARE(m_testModel->m_numRows, 1);
    QCOMPARE(m_testModel->m_numCols, 10);
    m_model->addDataSet(&dataSet2);
    QCOMPARE(m_testModel->m_numRows, 2);
    QCOMPARE(m_testModel->m_numCols, 10);
    m_model->addDataSet(&dataSet3);
    QCOMPARE(m_testModel->m_numRows, 3);
    QCOMPARE(m_testModel->m_numCols, 10);
    m_model->removeDataSet(&dataSet2);
    QCOMPARE(m_testModel->m_numRows, 2);
    QCOMPARE(m_testModel->m_numCols, 10);
    m_model->removeDataSet(&dataSet1);
    QCOMPARE(m_testModel->m_numRows, 1);
    QCOMPARE(m_testModel->m_numCols, 10);
    m_model->removeDataSet(&dataSet3);
    QCOMPARE(m_testModel->m_numRows, 0);
    QCOMPARE(m_testModel->m_numCols, 0);
}

void TestKChartModel::testData()
{
    DataSet dataSet1(0);
    DataSet dataSet2(1);

    dataSet1.setYDataRegion(CellRegion(m_table, QRect(2, 2, 10, 1)));
    dataSet1.setLabelDataRegion(CellRegion(m_table, QPoint(1, 2)));
    dataSet1.setCategoryDataRegion(CellRegion(m_table, QRect(2, 1, 10, 1)));

    dataSet2.setYDataRegion(CellRegion(m_table, QRect(2, 3, 10, 1)));
    dataSet2.setLabelDataRegion(CellRegion(m_table, QPoint(1, 3)));
    dataSet2.setCategoryDataRegion(CellRegion(m_table, QRect(2, 1, 10, 1)));

    m_model->addDataSet(&dataSet1);
    m_model->addDataSet(&dataSet2);

    QCOMPARE(m_model->rowCount(), 10);
    QCOMPARE(m_model->columnCount(), 2);

    // category data
    QCOMPARE(m_model->headerData(0, Qt::Vertical), QVariant("Column 1"));
    QCOMPARE(m_model->headerData(1, Qt::Vertical), QVariant("Column 2"));
    QCOMPARE(m_model->headerData(2, Qt::Vertical), QVariant("Column 3"));
    QCOMPARE(m_model->headerData(3, Qt::Vertical), QVariant("Column 4"));
    QCOMPARE(m_model->headerData(4, Qt::Vertical), QVariant("Column 5"));
    QCOMPARE(m_model->headerData(5, Qt::Vertical), QVariant("Column 6"));
    QCOMPARE(m_model->headerData(6, Qt::Vertical), QVariant("Column 7"));
    QCOMPARE(m_model->headerData(7, Qt::Vertical), QVariant("Column 8"));
    QCOMPARE(m_model->headerData(8, Qt::Vertical), QVariant("Column 9"));
    QCOMPARE(m_model->headerData(9, Qt::Vertical), QVariant("Column 10"));

    // dataSet1

    // label data
    QCOMPARE(m_model->headerData(0, Qt::Horizontal), QVariant("Row 1"));
    // y data
    QCOMPARE(m_model->data(m_model->index(0, 0)), QVariant(7.2));
    QCOMPARE(m_model->data(m_model->index(1, 0)), QVariant(1.8));
    QCOMPARE(m_model->data(m_model->index(2, 0)), QVariant(9.4));
    QCOMPARE(m_model->data(m_model->index(3, 0)), QVariant(1.5));
    QCOMPARE(m_model->data(m_model->index(4, 0)), QVariant(8.4));
    QCOMPARE(m_model->data(m_model->index(5, 0)), QVariant(2.9));
    QCOMPARE(m_model->data(m_model->index(6, 0)), QVariant(3.7));
    QCOMPARE(m_model->data(m_model->index(7, 0)), QVariant(5.5));
    QCOMPARE(m_model->data(m_model->index(8, 0)), QVariant(2.9));
    QCOMPARE(m_model->data(m_model->index(9, 0)), QVariant(5.3));

    // dataSet2

    // label data
    QCOMPARE(m_model->headerData(1, Qt::Horizontal), QVariant("Row 2"));
    QCOMPARE(m_model->data(m_model->index(0, 1)), QVariant(8.2));
    QCOMPARE(m_model->data(m_model->index(1, 1)), QVariant(2.8));
    QCOMPARE(m_model->data(m_model->index(2, 1)), QVariant(10.4));
    QCOMPARE(m_model->data(m_model->index(3, 1)), QVariant(2.5));
    QCOMPARE(m_model->data(m_model->index(4, 1)), QVariant(9.4));
    QCOMPARE(m_model->data(m_model->index(5, 1)), QVariant(3.9));
    QCOMPARE(m_model->data(m_model->index(6, 1)), QVariant(4.7));
    QCOMPARE(m_model->data(m_model->index(7, 1)), QVariant(6.5));
    QCOMPARE(m_model->data(m_model->index(8, 1)), QVariant(3.9));
    QCOMPARE(m_model->data(m_model->index(9, 1)), QVariant(6.3));
}

void TestKChartModel::testDataChanges()
{
    DataSet dataSet1(0);
    DataSet dataSet2(1);

    dataSet1.setYDataRegion(CellRegion(m_table, QRect(2, 2, 10, 1)));
    dataSet2.setYDataRegion(CellRegion(m_table, QRect(2, 3, 10, 1)));

    m_model->addDataSet(&dataSet1);
    m_model->addDataSet(&dataSet2);

    // Test changing dataset-wide data

    dataSet1.setLabelDataRegion(CellRegion(m_table, QPoint(2, 2)));
    QVERIFY(m_testModel->m_lastHeaderDataChange.valid);
    QCOMPARE(m_testModel->m_lastHeaderDataChange.orientation, Qt::Vertical);
    QCOMPARE(m_testModel->m_lastHeaderDataChange.first, 0);
    QCOMPARE(m_testModel->m_lastHeaderDataChange.last, 0);

    // Forget the last change to test another one
    m_testModel->m_lastHeaderDataChange.valid = false;

    dataSet2.setLabelDataRegion(CellRegion(m_table, QPoint(2, 3)));
    QVERIFY(m_testModel->m_lastHeaderDataChange.valid);
    QCOMPARE(m_testModel->m_lastHeaderDataChange.orientation, Qt::Vertical);
    QCOMPARE(m_testModel->m_lastHeaderDataChange.first, 1);
    QCOMPARE(m_testModel->m_lastHeaderDataChange.last, 1);

    // Test changing data points

    // Forget the last change to test another one
    m_testModel->m_lastDataChange.valid = false;

    dataSet1.setYDataRegion(CellRegion(m_table, QRect(3, 2, 9, 1)));
    QVERIFY(m_testModel->m_lastHeaderDataChange.valid);
    QCOMPARE(m_testModel->m_lastDataChange.topLeft, m_model->index(0, 0));
    QCOMPARE(m_testModel->m_lastDataChange.bottomRight, m_model->index(9, 0));

    // Forget the last change to test another one
    m_testModel->m_lastDataChange.valid = false;

    dataSet2.setYDataRegion(CellRegion(m_table, QRect(3, 3, 9, 1)));
    QVERIFY(m_testModel->m_lastHeaderDataChange.valid);
    QCOMPARE(m_testModel->m_lastDataChange.topLeft, m_model->index(0, 1));
    // The number of rows (data points) is now reduced by one because
    // both y data regions have been reduced by one cell.
    QCOMPARE(m_testModel->m_lastDataChange.bottomRight, m_model->index(8, 1));
}

void TestKChartModel::testDataChangesWithTwoDimensions()
{
    DataSet dataSet1(0);
    DataSet dataSet2(1);

    dataSet1.setXDataRegion(CellRegion(m_table, QRect(2, 1, 10, 1)));
    dataSet1.setYDataRegion(CellRegion(m_table, QRect(2, 2, 10, 1)));
    dataSet1.setLabelDataRegion(CellRegion(m_table, QPoint(1, 2)));

    dataSet2.setXDataRegion(CellRegion(m_table, QRect(2, 1, 10, 1)));
    dataSet2.setYDataRegion(CellRegion(m_table, QRect(2, 3, 10, 1)));
    dataSet2.setLabelDataRegion(CellRegion(m_table, QPoint(1, 3)));

    m_model->setDataDimensions(2);
    m_model->addDataSet(&dataSet1);
    m_model->addDataSet(&dataSet2);

    // Test changing dataset-wide data

    dataSet1.setLabelDataRegion(CellRegion(m_table, QPoint(2, 2)));
    QVERIFY(m_testModel->m_lastHeaderDataChange.valid);
    QCOMPARE(m_testModel->m_lastHeaderDataChange.orientation, Qt::Vertical);
    QCOMPARE(m_testModel->m_lastHeaderDataChange.first, 0);
    QCOMPARE(m_testModel->m_lastHeaderDataChange.last, 1);

    // Forget the last change to test another one
    m_testModel->m_lastHeaderDataChange.valid = false;

    dataSet2.setLabelDataRegion(CellRegion(m_table, QPoint(2, 3)));
    QVERIFY(m_testModel->m_lastHeaderDataChange.valid);
    QCOMPARE(m_testModel->m_lastHeaderDataChange.orientation, Qt::Vertical);
    QCOMPARE(m_testModel->m_lastHeaderDataChange.first, 2);
    QCOMPARE(m_testModel->m_lastHeaderDataChange.last, 3);

    // Test changing data points

    // Forget the last change to test another one
    m_testModel->m_lastDataChange.valid = false;

    dataSet1.setYDataRegion(CellRegion(m_table, QRect(3, 2, 9, 1)));
    QVERIFY(m_testModel->m_lastHeaderDataChange.valid);
    QCOMPARE(m_testModel->m_lastDataChange.topLeft, m_model->index(0, 0));
    QCOMPARE(m_testModel->m_lastDataChange.bottomRight, m_model->index(9, 1));

    // Forget the last change to test another one
    m_testModel->m_lastDataChange.valid = false;

    dataSet2.setYDataRegion(CellRegion(m_table, QRect(3, 3, 9, 1)));
    QVERIFY(m_testModel->m_lastHeaderDataChange.valid);
    QCOMPARE(m_testModel->m_lastDataChange.topLeft, m_model->index(0, 2));
    QCOMPARE(m_testModel->m_lastDataChange.bottomRight, m_model->index(9, 3));
}

QTEST_MAIN(TestKChartModel)
