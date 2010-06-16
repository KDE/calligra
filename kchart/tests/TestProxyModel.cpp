/* This file is part of the KDE project

   Copyright 2008 Johannes Simon <johannes.simon@gmail.com>

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

// Own
#include "TestProxyModel.h"

// Qt
#include <QRect>
#include <QPoint>
#include <QSize>
#include <QDebug>

// KChart
#include "DataSetFactory.h"
#include "DataSet.h"
#include "ChartTableModel.h"
#include "CellRegion.h"


namespace QTest {
    template<>
    char *toString( const CellRegion &region )
    {
        if ( region.rects().isEmpty() )
        {
            QByteArray result( "CellRegion()" );
            return qstrdup( result.data() );
        }
        else if ( region.rects().size() == 1 )
        {
            const QRect rect = region.rects().first();
            const QPoint topLeft = rect.topLeft();
            const QSize size = rect.size();
            
            QByteArray result( "CellRegion(" );
            result += QByteArray::number( topLeft.x() );
            result += ",";
            result += QByteArray::number( topLeft.y() );
            result += " ";
            result += QByteArray::number( size.width() );
            result += "x";
            result += QByteArray::number( size.height() );
            result += ")";
            return qstrdup( result.data() );
        }
        return 0;
    }
}

using namespace KChart;

TestProxyModel::TestProxyModel()
{
    m_proxyModel = 0;
    m_sourceModel = 0;
}

void TestProxyModel::initTestCase()
{
    m_sourceModel = new ChartTableModel;
    m_sourceModel->setRowCount( 4 );
    m_sourceModel->setColumnCount( 5 );
    
    // Vertical header data
    m_sourceModel->setData( m_sourceModel->index( 1, 0 ), "Row 1" );
    m_sourceModel->setData( m_sourceModel->index( 2, 0 ), "Row 2" );
    m_sourceModel->setData( m_sourceModel->index( 3, 0 ), "Row 3" );
    
    // Horizontal header data
    m_sourceModel->setData( m_sourceModel->index( 0, 1 ), "Column 1" );
    m_sourceModel->setData( m_sourceModel->index( 0, 2 ), "Column 2" );
    m_sourceModel->setData( m_sourceModel->index( 0, 3 ), "Column 3" );
    m_sourceModel->setData( m_sourceModel->index( 0, 4 ), "Column 4" );
    
    // First row
    m_sourceModel->setData( m_sourceModel->index( 1, 1 ), 7.2 );
    m_sourceModel->setData( m_sourceModel->index( 1, 2 ), 1.8 );
    m_sourceModel->setData( m_sourceModel->index( 1, 3 ), 9.4 );
    m_sourceModel->setData( m_sourceModel->index( 1, 4 ), 1.5 );
    
    // Second row
    m_sourceModel->setData( m_sourceModel->index( 2, 1 ), 8.4 );
    m_sourceModel->setData( m_sourceModel->index( 2, 2 ), 2.9 );
    m_sourceModel->setData( m_sourceModel->index( 2, 3 ), 3.7 );
    m_sourceModel->setData( m_sourceModel->index( 2, 4 ), 5.5 );
    
    // Third row
    m_sourceModel->setData( m_sourceModel->index( 3, 1 ), 2.9 );
    m_sourceModel->setData( m_sourceModel->index( 3, 2 ), 5.3 );
    m_sourceModel->setData( m_sourceModel->index( 3, 3 ), 6.4 );
    m_sourceModel->setData( m_sourceModel->index( 3, 4 ), 2.1 );
    
    QRect selection( QPoint( 1, 1 ),
                     QSize( m_sourceModel->columnCount(), m_sourceModel->rowCount() ) );
    
    m_proxyModel = new ChartProxyModel;
    m_proxyModel->setSourceModel( m_sourceModel, QVector<QRect>( 1, selection ) );
}

void TestProxyModel::testWithoutLabels()
{
    QList<DataSet*> dataSets;

    // Horizontal data direction
    m_proxyModel->setDataDirection( Qt::Horizontal );
    m_proxyModel->setFirstColumnIsLabel( false );
    m_proxyModel->setFirstRowIsLabel( false );
    
    dataSets = m_proxyModel->dataSets();
    
    QCOMPARE( dataSets.size(), 4 );
    QCOMPARE( dataSets[0]->size(), 5 );
    QCOMPARE( dataSets[1]->size(), 5 );
    QCOMPARE( dataSets[2]->size(), 5 );
    QCOMPARE( dataSets[3]->size(), 5 );
    
    QCOMPARE( dataSets[0]->yDataRegion(), CellRegion( QPoint( 1, 1 ), QSize( 5, 1 ) ) );
    QCOMPARE( dataSets[0]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->labelDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->categoryDataRegion(), CellRegion() );
    
    QCOMPARE( dataSets[1]->yDataRegion(), CellRegion( QPoint( 1, 2 ), QSize( 5, 1 ) ) );
    QCOMPARE( dataSets[1]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->labelDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->categoryDataRegion(), CellRegion() );
    
    QCOMPARE( dataSets[2]->yDataRegion(), CellRegion( QPoint( 1, 3 ), QSize( 5, 1 ) ) );
    QCOMPARE( dataSets[2]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->labelDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->categoryDataRegion(), CellRegion() );
    
    QCOMPARE( dataSets[3]->yDataRegion(), CellRegion( QPoint( 1, 4 ), QSize( 5, 1 ) ) );
    QCOMPARE( dataSets[3]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[3]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[3]->labelDataRegion(), CellRegion() );
    QCOMPARE( dataSets[3]->categoryDataRegion(), CellRegion() );
    
    
    // Vertical data direction
    m_proxyModel->setDataDirection( Qt::Vertical );
    
    dataSets = m_proxyModel->dataSets();
    
    QCOMPARE( dataSets.size(), 5 );
    QCOMPARE( dataSets[0]->size(), 4 );
    QCOMPARE( dataSets[1]->size(), 4 );
    QCOMPARE( dataSets[2]->size(), 4 );
    QCOMPARE( dataSets[3]->size(), 4 );
    QCOMPARE( dataSets[4]->size(), 4 );
    
    QCOMPARE( dataSets[0]->yDataRegion(), CellRegion( QPoint( 1, 1 ), QSize( 1, 4 ) ) );
    QCOMPARE( dataSets[0]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->labelDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->categoryDataRegion(), CellRegion() );
    
    QCOMPARE( dataSets[1]->yDataRegion(), CellRegion( QPoint( 2, 1 ), QSize( 1, 4 ) ) );
    QCOMPARE( dataSets[1]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->labelDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->categoryDataRegion(), CellRegion() );
    
    QCOMPARE( dataSets[2]->yDataRegion(), CellRegion( QPoint( 3, 1 ), QSize( 1, 4 ) ) );
    QCOMPARE( dataSets[2]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->labelDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->categoryDataRegion(), CellRegion() );
    
    QCOMPARE( dataSets[3]->yDataRegion(), CellRegion( QPoint( 4, 1 ), QSize( 1, 4 ) ) );
    QCOMPARE( dataSets[3]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[3]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[3]->labelDataRegion(), CellRegion() );
    QCOMPARE( dataSets[3]->categoryDataRegion(), CellRegion() );
    
    QCOMPARE( dataSets[4]->yDataRegion(), CellRegion( QPoint( 5, 1 ), QSize( 1, 4 ) ) );
    QCOMPARE( dataSets[4]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[4]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[4]->labelDataRegion(), CellRegion() );
    QCOMPARE( dataSets[4]->categoryDataRegion(), CellRegion() );
}

void TestProxyModel::testFirstRowAsLabel()
{
    QList<DataSet*> dataSets;

    // Horizontal data direction
    m_proxyModel->setDataDirection( Qt::Horizontal );
    
    m_proxyModel->setFirstColumnIsLabel( false );
    
    // With first row as category data
    m_proxyModel->setFirstRowIsLabel( true );
    dataSets = m_proxyModel->dataSets();
    
    QCOMPARE( dataSets.size(), 3 );
    QCOMPARE( dataSets[0]->size(), 5 );
    QCOMPARE( dataSets[1]->size(), 5 );
    QCOMPARE( dataSets[2]->size(), 5 );
    
    QCOMPARE( dataSets[0]->yDataRegion(), CellRegion( QPoint( 1, 2 ), QSize( 5, 1 ) ) );
    QCOMPARE( dataSets[0]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->labelDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->categoryDataRegion(), CellRegion( QPoint( 1, 1 ), QSize( 5, 1 ) ) );
    
    QCOMPARE( dataSets[1]->yDataRegion(), CellRegion( QPoint( 1, 3 ), QSize( 5, 1 ) ) );
    QCOMPARE( dataSets[1]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->labelDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->categoryDataRegion(), CellRegion( QPoint( 1, 1 ), QSize( 5, 1 ) ) );
    
    QCOMPARE( dataSets[2]->yDataRegion(), CellRegion( QPoint( 1, 4 ), QSize( 5, 1 ) ) );
    QCOMPARE( dataSets[2]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->labelDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->categoryDataRegion(), CellRegion( QPoint( 1, 1 ), QSize( 5, 1 ) ) );
    
    
    // Vertical data direction
    m_proxyModel->setDataDirection( Qt::Vertical );
    
    dataSets = m_proxyModel->dataSets();
    
    QCOMPARE( dataSets.size(), 5 );
    QCOMPARE( dataSets[0]->size(), 3 );
    QCOMPARE( dataSets[1]->size(), 3 );
    QCOMPARE( dataSets[2]->size(), 3 );
    
    QCOMPARE( dataSets[0]->yDataRegion(), CellRegion( QPoint( 1, 2 ), QSize( 1, 3 ) ) );
    QCOMPARE( dataSets[0]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->labelDataRegion(), CellRegion( QPoint( 1, 1 ), QSize( 1, 1 ) ) );
    QCOMPARE( dataSets[0]->categoryDataRegion(), CellRegion() );
    
    QCOMPARE( dataSets[1]->yDataRegion(), CellRegion( QPoint( 2, 2 ), QSize( 1, 3 ) ) );
    QCOMPARE( dataSets[1]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->labelDataRegion(), CellRegion( QPoint( 2, 1 ), QSize( 1, 1 ) ) );
    QCOMPARE( dataSets[1]->categoryDataRegion(), CellRegion() );
    
    QCOMPARE( dataSets[2]->yDataRegion(), CellRegion( QPoint( 3, 2 ), QSize( 1, 3 ) ) );
    QCOMPARE( dataSets[2]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->labelDataRegion(), CellRegion( QPoint( 3, 1 ), QSize( 1, 1 ) ) );
    QCOMPARE( dataSets[2]->categoryDataRegion(), CellRegion() );
    
    QCOMPARE( dataSets[3]->yDataRegion(), CellRegion( QPoint( 4, 2 ), QSize( 1, 3 ) ) );
    QCOMPARE( dataSets[3]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[3]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[3]->labelDataRegion(), CellRegion( QPoint( 4, 1 ), QSize( 1, 1 ) ) );
    QCOMPARE( dataSets[3]->categoryDataRegion(), CellRegion() );
    
    QCOMPARE( dataSets[4]->yDataRegion(), CellRegion( QPoint( 5, 2 ), QSize( 1, 3 ) ) );
    QCOMPARE( dataSets[4]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[4]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[4]->labelDataRegion(), CellRegion( QPoint( 5, 1 ), QSize( 1, 1 ) ) );
    QCOMPARE( dataSets[4]->categoryDataRegion(), CellRegion() );
}

void TestProxyModel::testFirstColumnAsLabel()
{
    QList<DataSet*> dataSets;

    // Horizontal data direction
    m_proxyModel->setDataDirection( Qt::Horizontal );
    
    m_proxyModel->setFirstRowIsLabel( false );
    
    // With first column as label data
    m_proxyModel->setFirstColumnIsLabel( true );
    dataSets = m_proxyModel->dataSets();
    
    QCOMPARE( dataSets.size(), 4 );
    QCOMPARE( dataSets[0]->size(), 4 );
    QCOMPARE( dataSets[1]->size(), 4 );
    QCOMPARE( dataSets[2]->size(), 4 );
    QCOMPARE( dataSets[3]->size(), 4 );
    
    QCOMPARE( dataSets[0]->yDataRegion(), CellRegion( QPoint( 2, 1 ), QSize( 4, 1 ) ) );
    QCOMPARE( dataSets[0]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->labelDataRegion(), CellRegion( QPoint( 1, 1 ), QSize( 1, 1 ) ) );
    QCOMPARE( dataSets[0]->categoryDataRegion(), CellRegion() );
    
    QCOMPARE( dataSets[1]->yDataRegion(), CellRegion( QPoint( 2, 2 ), QSize( 4, 1 ) ) );
    QCOMPARE( dataSets[1]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->labelDataRegion(), CellRegion( QPoint( 1, 2 ), QSize( 1, 1 ) ) );
    QCOMPARE( dataSets[1]->categoryDataRegion(), CellRegion() );
    
    QCOMPARE( dataSets[2]->yDataRegion(), CellRegion( QPoint( 2, 3 ), QSize( 4, 1 ) ) );
    QCOMPARE( dataSets[2]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->labelDataRegion(), CellRegion( QPoint( 1, 3 ), QSize( 1, 1 ) ) );
    QCOMPARE( dataSets[2]->categoryDataRegion(), CellRegion() );
    
    QCOMPARE( dataSets[3]->yDataRegion(), CellRegion( QPoint( 2, 4 ), QSize( 4, 1 ) ) );
    QCOMPARE( dataSets[3]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[3]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[3]->labelDataRegion(), CellRegion( QPoint( 1, 4 ), QSize( 1, 1 ) ) );
    QCOMPARE( dataSets[3]->categoryDataRegion(), CellRegion() );
    
    
    // Vertical data direction
    m_proxyModel->setDataDirection( Qt::Vertical );
    
    dataSets = m_proxyModel->dataSets();
    
    QCOMPARE( dataSets.size(), 4 );
    QCOMPARE( dataSets[0]->size(), 4 );
    QCOMPARE( dataSets[1]->size(), 4 );
    QCOMPARE( dataSets[2]->size(), 4 );
    QCOMPARE( dataSets[3]->size(), 4 );
    
    QCOMPARE( dataSets[0]->yDataRegion(), CellRegion( QPoint( 2, 1 ), QSize( 1, 4 ) ) );
    QCOMPARE( dataSets[0]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->labelDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->categoryDataRegion(), CellRegion( QPoint( 1, 1 ), QSize( 1, 4 ) ) );
    
    QCOMPARE( dataSets[1]->yDataRegion(), CellRegion( QPoint( 3, 1 ), QSize( 1, 4 ) ) );
    QCOMPARE( dataSets[1]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->labelDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->categoryDataRegion(), CellRegion( QPoint( 1, 1 ), QSize( 1, 4 ) ) );
    
    QCOMPARE( dataSets[2]->yDataRegion(), CellRegion( QPoint( 4, 1 ), QSize( 1, 4 ) ) );
    QCOMPARE( dataSets[2]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->labelDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->categoryDataRegion(), CellRegion( QPoint( 1, 1 ), QSize( 1, 4 ) ) );
    
    QCOMPARE( dataSets[3]->yDataRegion(), CellRegion( QPoint( 5, 1 ), QSize( 1, 4 ) ) );
    QCOMPARE( dataSets[3]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[3]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[3]->labelDataRegion(), CellRegion() );
    QCOMPARE( dataSets[3]->categoryDataRegion(), CellRegion( QPoint( 1, 1 ), QSize( 1, 4 ) ) );
}

void TestProxyModel::testFirstRowAndColumnAsLabels()
{
    QList<DataSet*> dataSets;

    // Horizontal data direction
    m_proxyModel->setDataDirection( Qt::Horizontal );
    
    // With first row as category data
    m_proxyModel->setFirstRowIsLabel( true );
    // ...and first column as label data
    m_proxyModel->setFirstColumnIsLabel( true );
    
    dataSets = m_proxyModel->dataSets();
    
    QCOMPARE( dataSets.size(), 3 );
    QCOMPARE( dataSets[0]->size(), 4 );
    QCOMPARE( dataSets[1]->size(), 4 );
    QCOMPARE( dataSets[2]->size(), 4 );
    
    QCOMPARE( dataSets[0]->yDataRegion(), CellRegion( QPoint( 2, 2 ), QSize( 4, 1 ) ) );
    QCOMPARE( dataSets[0]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->labelDataRegion(), CellRegion( QPoint( 1, 2 ), QSize( 1, 1 ) ) );
    QCOMPARE( dataSets[0]->categoryDataRegion(), CellRegion( QPoint( 2, 1 ), QSize( 4, 1 ) ) );
    
    QCOMPARE( dataSets[1]->yDataRegion(), CellRegion( QPoint( 2, 3 ), QSize( 4, 1 ) ) );
    QCOMPARE( dataSets[1]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->labelDataRegion(), CellRegion( QPoint( 1, 3 ), QSize( 1, 1 ) ) );
    QCOMPARE( dataSets[0]->categoryDataRegion(), CellRegion( QPoint( 2, 1 ), QSize( 4, 1 ) ) );
    
    QCOMPARE( dataSets[2]->yDataRegion(), CellRegion( QPoint( 2, 4 ), QSize( 4, 1 ) ) );
    QCOMPARE( dataSets[2]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->labelDataRegion(), CellRegion( QPoint( 1, 4 ), QSize( 1, 1 ) ) );
    QCOMPARE( dataSets[0]->categoryDataRegion(), CellRegion( QPoint( 2, 1 ), QSize( 4, 1 ) ) );
    
    
    // Vertical data direction
    m_proxyModel->setDataDirection( Qt::Vertical );
    
    dataSets = m_proxyModel->dataSets();
    
    QCOMPARE( dataSets.size(), 4 );
    QCOMPARE( dataSets[0]->size(), 3 );
    QCOMPARE( dataSets[1]->size(), 3 );
    QCOMPARE( dataSets[2]->size(), 3 );
    QCOMPARE( dataSets[3]->size(), 3 );
    
    QCOMPARE( dataSets[0]->yDataRegion(), CellRegion( QPoint( 2, 2 ), QSize( 1, 3 ) ) );
    QCOMPARE( dataSets[0]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[0]->labelDataRegion(), CellRegion( QPoint( 2, 1 ), QSize( 1, 1 ) ) );
    QCOMPARE( dataSets[0]->categoryDataRegion(), CellRegion( QPoint( 1, 2 ), QSize( 1, 3 ) ) );
    
    QCOMPARE( dataSets[1]->yDataRegion(), CellRegion( QPoint( 3, 2 ), QSize( 1, 3 ) ) );
    QCOMPARE( dataSets[1]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[1]->labelDataRegion(), CellRegion( QPoint( 3, 1 ), QSize( 1, 1 ) ) );
    QCOMPARE( dataSets[1]->categoryDataRegion(), CellRegion( QPoint( 1, 2 ), QSize( 1, 3 ) ) );
    
    QCOMPARE( dataSets[2]->yDataRegion(), CellRegion( QPoint( 4, 2 ), QSize( 1, 3 ) ) );
    QCOMPARE( dataSets[2]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[2]->labelDataRegion(), CellRegion( QPoint( 4, 1 ), QSize( 1, 1 ) ) );
    QCOMPARE( dataSets[2]->categoryDataRegion(), CellRegion( QPoint( 1, 2 ), QSize( 1, 3 ) ) );
    
    QCOMPARE( dataSets[3]->yDataRegion(), CellRegion( QPoint( 5, 2 ), QSize( 1, 3 ) ) );
    QCOMPARE( dataSets[3]->xDataRegion(), CellRegion() );
    QCOMPARE( dataSets[3]->customDataRegion(), CellRegion() );
    QCOMPARE( dataSets[3]->labelDataRegion(), CellRegion( QPoint( 5, 1 ), QSize( 1, 1 ) ) );
    QCOMPARE( dataSets[3]->categoryDataRegion(), CellRegion( QPoint( 1, 2 ), QSize( 1, 3 ) ) );
}

QTEST_MAIN( TestProxyModel )
#include "TestProxyModel.moc"
