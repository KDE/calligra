/* This file is part of the KDE project

   Copyright 2009 Johannes Simon <johannes.simon@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "TestKDChartModel.h"

#include <QObject>
#include <QAbstractItemModel>
#include <QRect>

#include "KDChartModel.h"
#include "ModelObserver.h"
#include "DataSet.h"
#include "CellRegion.h"


TestKDChartModel::TestKDChartModel()
{
}

void TestKDChartModel::init()
{
    m_model = new KChart::KDChartModel();
    m_testModel = new ModelObserver( m_model );

    connect( m_model, SIGNAL( rowsInserted( const QModelIndex&, int, int ) ),
             m_testModel, SLOT( slotRowsInserted( const QModelIndex&, int, int ) ) );
    connect( m_model, SIGNAL( columnsInserted( const QModelIndex&, int, int ) ),
             m_testModel, SLOT( slotColumnsInserted( const QModelIndex&, int, int ) ) );
    connect( m_model, SIGNAL( rowsRemoved( const QModelIndex&, int, int ) ),
             m_testModel, SLOT( slotRowsRemoved( const QModelIndex&, int, int ) ) );
    connect( m_model, SIGNAL( columnsRemoved( const QModelIndex&, int, int ) ),
             m_testModel, SLOT( slotColumnsRemoved( const QModelIndex&, int, int ) ) );
    connect( m_model, SIGNAL( modelReset() ),
             m_testModel, SLOT( slotModelReset() ) );
}

void TestKDChartModel::testDataSetInsertion()
{
    KChart::DataSet dataSet1( 0 );
    KChart::DataSet dataSet2( 0 );
    dataSet1.setYDataRegion( KChart::CellRegion( QRect( 0, 0, 1, 10 ) ) );
    dataSet2.setYDataRegion( KChart::CellRegion( QRect( 1, 0, 1, 10 ) ) );
    m_model->addDataSet( &dataSet1 );
    m_model->addDataSet( &dataSet2 );
    QCOMPARE( m_testModel->m_numRows, m_model->rowCount() );
    QCOMPARE( m_testModel->m_numCols, m_model->columnCount() );
}

void TestKDChartModel::testDataSetInsertionAndRemoval()
{
    KChart::DataSet dataSet1( 0 );
    KChart::DataSet dataSet2( 0 );
    dataSet1.setYDataRegion( KChart::CellRegion( QRect( 0, 0, 1, 10 ) ) );
    dataSet2.setYDataRegion( KChart::CellRegion( QRect( 1, 0, 1, 10 ) ) );
    m_model->addDataSet( &dataSet1 );
    m_model->addDataSet( &dataSet2 );
    QCOMPARE( m_testModel->m_numRows, 10 );
    QCOMPARE( m_testModel->m_numCols, 2 );
    QCOMPARE( m_model->rowCount(), 10 );
    QCOMPARE( m_model->columnCount(), 2 );
    m_model->removeDataSet( &dataSet1 );
    m_model->removeDataSet( &dataSet2 );
    QCOMPARE( m_testModel->m_numRows, 0 );
    QCOMPARE( m_testModel->m_numCols, 0 );
    QCOMPARE( m_model->rowCount(), 0 );
    QCOMPARE( m_model->columnCount(), 0 );
    m_model->addDataSet( &dataSet1 );
    m_model->addDataSet( &dataSet2 );
    QCOMPARE( m_testModel->m_numRows, 10 );
    QCOMPARE( m_testModel->m_numCols, 2 );
    QCOMPARE( m_model->rowCount(), 10 );
    QCOMPARE( m_model->columnCount(), 2 );
}

QTEST_MAIN( TestKDChartModel )
