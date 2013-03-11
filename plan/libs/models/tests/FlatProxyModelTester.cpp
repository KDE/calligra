/* This file is part of the KDE project
   Copyright (C) 2010 Dag Andersen <danders@get2net.dk>

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
#include "FlatProxyModelTester.h"


#include <QModelIndex>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSortFilterProxyModel>

#include <qtest_kde.h>
#include <kdebug.h>

namespace KPlato
{

void FlatProxyModelTester::emptyModel()
{
    QCOMPARE( m_flatmodel.columnCount(), 0 );
    QCOMPARE( m_flatmodel.rowCount(), 0 );
}

void FlatProxyModelTester::test()
{
    qDebug()<<"set 1 row, 1 column";
    m_standardmodel.setColumnCount( 1 );
    m_standardmodel.setRowCount( 1 );

    m_standardmodel.setHorizontalHeaderLabels( QStringList() << "Column 1" );
    m_flatmodel.setSourceModel( &m_standardmodel );
    
    QCOMPARE( m_flatmodel.columnCount(), 2 ); // it adds an extra column
    QCOMPARE( m_flatmodel.rowCount(), 1 );
    QCOMPARE( m_flatmodel.headerData( 0, Qt::Horizontal ), QVariant( "Column 1" ) );
    
    m_standardmodel.setData( m_standardmodel.index( 0, 0 ), "Index 0,0" );

    QModelIndex idx = m_flatmodel.index( 0, 0 );
    QVERIFY( idx.isValid() );
    qDebug()<<"Index 0,0:"<<idx.data();

    QCOMPARE( idx.data(), QVariant( "Index 0,0" ) );
    
    qDebug()<<"1 row, set 2 columns";
    m_standardmodel.setColumnCount( 2 );
    QCOMPARE( m_flatmodel.columnCount(), 3 ); // it adds an extra column
    m_flatmodel.setHeaderData( 1, Qt::Horizontal, "Column 2" );
    QCOMPARE( m_flatmodel.headerData( 1, Qt::Horizontal ), QVariant( "Column 2" ) );
    m_standardmodel.setData( m_standardmodel.index( 0, 1 ), "Index 0,1" );
    idx = m_flatmodel.index( 0, 1 );
    QVERIFY( idx.isValid() );
    qDebug()<<"Index 0,1:"<<idx.data();
    QCOMPARE( idx.data(), QVariant( "Index 0,1" ) );

    qDebug()<<"Set 2 rows, 2 columns";
    m_standardmodel.setRowCount( 2 );
    QCOMPARE( m_flatmodel.rowCount(), 2 );
    m_standardmodel.setData( m_standardmodel.index( 1, 0 ), "Index 1,0" );
    idx = m_flatmodel.index( 1, 0 );
    QVERIFY( idx.isValid() );
    qDebug()<<"Index 1,0:"<<idx.data();
    QCOMPARE( idx.data(), QVariant( "Index 1,0" ) );

    m_standardmodel.setData( m_standardmodel.index( 1, 1 ), "Index 1,1" );
    idx = m_flatmodel.index( 1, 1 );
    QVERIFY( idx.isValid() );
    qDebug()<<"Index 1,1:"<<idx.data();
    QCOMPARE( idx.data(), QVariant( "Index 1,1" ) );
    
    qDebug()<<"Add child on last index, adds a new row 1 in the flat model";

    // NOTE: m_standardmodel.insertRow( 0, m_standardmodel.index( 1, 0 ) );
    // does not work as there will be no columns and thus no valid indexes for this row
    QStandardItem *item = m_standardmodel.itemFromIndex( m_standardmodel.index( 1, 0 ) );
    QList<QStandardItem*> items;
    items << new QStandardItem( "Child last column 1" ) << new QStandardItem( "Child last column 2" );
    item->appendRow( items );
    QCOMPARE( m_flatmodel.rowCount(), 3 );
    idx = m_flatmodel.index( 2, 0 );
    QCOMPARE( idx.data(), QVariant( "Child last column 1" ) );
    idx = m_flatmodel.index( 2, 1 );
    QCOMPARE( idx.data(), QVariant( "Child last column 2" ) );

    qDebug()<<"add child on first index";
    
    item = m_standardmodel.itemFromIndex( m_standardmodel.index( 0, 0 ) );
    items.clear();
    items << new QStandardItem( "Child first column 1" ) << new QStandardItem( "Child first column 2" );
    item->appendRow( items );
    QCOMPARE( m_flatmodel.rowCount(), 4 );
    idx = m_flatmodel.index( 1, 0 );
    QCOMPARE( idx.data(), QVariant( "Child first column 1" ) );
    idx = m_flatmodel.index( 1, 1 );
    QCOMPARE( idx.data(), QVariant( "Child first column 2" ) );

    qDebug()<<"add row (2) on top level between first and last";
    
    item = m_standardmodel.invisibleRootItem();
    items.clear();
    items << new QStandardItem( "New index 1,0" ) << new QStandardItem( "New index 1,1" );
    item->insertRow( 1, items );
    QCOMPARE( m_flatmodel.rowCount(), 5 );
    idx = m_flatmodel.index( 2, 0 );
    QCOMPARE( idx.data(), QVariant( "New index 1,0" ) );
    idx = m_flatmodel.index( 2, 1 );
    QCOMPARE( idx.data(), QVariant( "New index 1,1" ) );

    qDebug()<<"Add child on middle index, adds row 3 to flat model";
    
    item = m_standardmodel.itemFromIndex( m_standardmodel.index( 1, 0 ) );
    items.clear();
    items << new QStandardItem( "Child middle column 1" ) << new QStandardItem( "Child middle column 2" );
    item->appendRow( items );
    QCOMPARE( m_flatmodel.rowCount(), 6 );
    idx = m_flatmodel.index( 3, 0 );
    QCOMPARE( idx.data().toString(), QVariant( "Child middle column 1" ).toString() );
    idx = m_flatmodel.index( 3, 1 );
    QCOMPARE( idx.data(), QVariant( "Child middle column 2" ) );

    qDebug()<<"Add child on middle index's child, adds row 4 to flat model";
    
    QModelIndex parent = m_standardmodel.index( 1, 0 );
    QCOMPARE( parent.data().toString(), QString( "New index 1,0" ) );
    idx = m_standardmodel.index( 0, 0, parent );
    QCOMPARE( idx.data().toString(), QString( "Child middle column 1" ) );
    item = m_standardmodel.itemFromIndex( idx );
    items.clear();
    items << new QStandardItem( "Child of middle child column 1" ) << new QStandardItem( "Child of middle child column 2" );
    item->appendRow( items );
    QCOMPARE( m_flatmodel.rowCount(), 7 );
    idx = m_flatmodel.index( 4, 0 );
    QCOMPARE( idx.data().toString(), QVariant( "Child of middle child column 1" ).toString() );
    idx = m_flatmodel.index( 4, 1 );
    QCOMPARE( idx.data(), QVariant( "Child of middle child column 2" ) );

    qDebug()<<"Add child on middle index at row 0, adds row 3 to flat model";
    
    idx = m_standardmodel.index( 1, 0 );
    QCOMPARE( idx.data().toString(), QString( "New index 1,0" ) );
    item = m_standardmodel.itemFromIndex( idx );
    items.clear();
    items << new QStandardItem( "Index 1,0 -> Index 0,0" ) << new QStandardItem( "Index 1,0 -> Index 0,1" );
    item->insertRow( 0, items );
    QCOMPARE( m_flatmodel.rowCount(), 8 );
    idx = m_flatmodel.index( 3, 0 );
    QCOMPARE( idx.data().toString(), QVariant( "Index 1,0 -> Index 0,0" ).toString() );
    idx = m_flatmodel.index( 3, 1 );
    QCOMPARE( idx.data(), QVariant( "Index 1,0 -> Index 0,1" ) );

    qDebug()<<"Add child on middle index at row 1, adds row 4 to flat model";
    
    idx = m_standardmodel.index( 1, 0 );
    QCOMPARE( idx.data().toString(), QString( "New index 1,0" ) );
    item = m_standardmodel.itemFromIndex( idx );
    items.clear();
    items << new QStandardItem( "Index 1,0 -> Index 1,0" ) << new QStandardItem( "Index 1,0 -> Index 1,1" );
    item->insertRow( 1, items );
    QCOMPARE( m_flatmodel.rowCount(), 9 );
    idx = m_flatmodel.index( 4, 0 );
    QCOMPARE( idx.data().toString(), QVariant( "Index 1,0 -> Index 1,0" ).toString() );
    idx = m_flatmodel.index( 4, 1 );
    QCOMPARE( idx.data(), QVariant( "Index 1,0 -> Index 1,1" ) );

    qDebug()<<"Add child on middle index at last row (4), adds row 8 to flat model";
    
    idx = m_standardmodel.index( 1, 0 );
    QCOMPARE( idx.data().toString(), QString( "New index 1,0" ) );
    item = m_standardmodel.itemFromIndex( idx );
    items.clear();
    items << new QStandardItem( "Index 1,0 -> Index 4,0" ) << new QStandardItem( "Index 1,0 -> Index 4,1" );
    item->insertRow( 3, items );
    QCOMPARE( m_standardmodel.rowCount( m_standardmodel.index( 1, 0 ) ), 4 );
    QCOMPARE( m_flatmodel.rowCount(), 10 );
    idx = m_flatmodel.index( 7, 0 );
    QCOMPARE( idx.data().toString(), QVariant( "Index 1,0 -> Index 4,0" ).toString() );
    idx = m_flatmodel.index( 7, 1 );
    QCOMPARE( idx.data(), QVariant( "Index 1,0 -> Index 4,1" ) );
}

void FlatProxyModelTester::testInsertRemoveTop()
{
    QSortFilterProxyModel sf;
    FlatProxyModel fm;
    QStandardItemModel sm;

    sf.setSourceModel( &fm );
    fm.setSourceModel( &sm );

    sm.setHeaderData( 0, Qt::Horizontal, "Column 0" );
    QCOMPARE( sm.rowCount(), 0 );

    // insert
    sm.insertRow( 0, new QStandardItem( "First" ) );
    QCOMPARE( sm.rowCount(), 1 );
    QCOMPARE( fm.rowCount(), 1 );
    QCOMPARE( sf.rowCount(), 1 );
    QCOMPARE( sm.index( 0, 0 ).data(), sf.index( 0, 0 ).data() );
    QCOMPARE( sf.index( 0, 0 ).data(), QVariant( "First" ) );

    sm.insertRow( 1, new QStandardItem( "Second" ) );
    QCOMPARE( sm.rowCount(), 2 );
    QCOMPARE( fm.rowCount(), 2 );
    QCOMPARE( sf.rowCount(), 2 );
    QCOMPARE( sm.index( 0, 0 ).data(), sf.index( 0, 0 ).data() );
    QCOMPARE( sm.index( 1, 0 ).data(), sf.index( 1, 0 ).data() );
    QCOMPARE( sf.index( 0, 0 ).data(), QVariant( "First" ) );
    QCOMPARE( sf.index( 1, 0 ).data(), QVariant( "Second" ) );

    sm.insertRow( 1, new QStandardItem( "In between" ) );
    QCOMPARE( sm.rowCount(), 3 );
    QCOMPARE( fm.rowCount(), 3 );
    QCOMPARE( sf.rowCount(), 3 );
    QCOMPARE( sm.index( 0, 0 ).data(), sf.index( 0, 0 ).data() );
    QCOMPARE( sm.index( 1, 0 ).data(), sf.index( 1, 0 ).data() );
    QCOMPARE( sm.index( 2, 0 ).data(), sf.index( 2, 0 ).data() );
    QCOMPARE( sf.index( 0, 0 ).data(), QVariant( "First" ) );
    QCOMPARE( sf.index( 1, 0 ).data(), QVariant( "In between" ) );
    QCOMPARE( sf.index( 2, 0 ).data(), QVariant( "Second" ) );

    sm.insertRow( 0, new QStandardItem( "Before first" ) );
    QCOMPARE( sm.rowCount(), 4 );
    QCOMPARE( fm.rowCount(), 4 );
    QCOMPARE( sf.rowCount(), 4 );
    QCOMPARE( sm.index( 0, 0 ).data(), sf.index( 0, 0 ).data() );
    QCOMPARE( sm.index( 1, 0 ).data(), sf.index( 1, 0 ).data() );
    QCOMPARE( sm.index( 2, 0 ).data(), sf.index( 2, 0 ).data() );
    QCOMPARE( sm.index( 3, 0 ).data(), sf.index( 3, 0 ).data() );
    QCOMPARE( sf.index( 0, 0 ).data(), QVariant( "Before first" ) );
    QCOMPARE( sf.index( 1, 0 ).data(), QVariant( "First" ) );
    QCOMPARE( sf.index( 2, 0 ).data(), QVariant( "In between" ) );
    QCOMPARE( sf.index( 3, 0 ).data(), QVariant( "Second" ) );

    // remove
    sm.removeRow( 0 );
    QCOMPARE( sm.rowCount(), 3 );
    QCOMPARE( fm.rowCount(), 3 );
    QCOMPARE( sf.rowCount(), 3 );
    QCOMPARE( sm.index( 0, 0 ).data(), sf.index( 0, 0 ).data() );
    QCOMPARE( sm.index( 1, 0 ).data(), sf.index( 1, 0 ).data() );
    QCOMPARE( sm.index( 2, 0 ).data(), sf.index( 2, 0 ).data() );
    QCOMPARE( sf.index( 0, 0 ).data(), QVariant( "First" ) );
    QCOMPARE( sf.index( 1, 0 ).data(), QVariant( "In between" ) );
    QCOMPARE( sf.index( 2, 0 ).data(), QVariant( "Second" ) );

    sm.removeRow( 1 );
    QCOMPARE( sm.rowCount(), 2 );
    QCOMPARE( fm.rowCount(), 2 );
    QCOMPARE( sf.rowCount(), 2 );
    QCOMPARE( sm.index( 0, 0 ).data(), sf.index( 0, 0 ).data() );
    QCOMPARE( sm.index( 1, 0 ).data(), sf.index( 1, 0 ).data() );
    QCOMPARE( sf.index( 0, 0 ).data(), QVariant( "First" ) );
    QCOMPARE( sf.index( 1, 0 ).data(), QVariant( "Second" ) );

    sm.removeRow( 1 );
    QCOMPARE( sm.rowCount(), 1 );
    QCOMPARE( fm.rowCount(), 1 );
    QCOMPARE( sf.rowCount(), 1 );
    QCOMPARE( sm.index( 0, 0 ).data(), sf.index( 0, 0 ).data() );
    QCOMPARE( sf.index( 0, 0 ).data(), QVariant( "First" ) );

    sm.removeRow( 0 );
    QCOMPARE( sm.rowCount(), 0 );
    QCOMPARE( fm.rowCount(), 0 );
    QCOMPARE( sf.rowCount(), 0 );
}

void FlatProxyModelTester::testInsertRemoveChildren()
{
    QSortFilterProxyModel sf;
    FlatProxyModel fm;
    QStandardItemModel sm;

    sf.setSourceModel( &fm );
    fm.setSourceModel( &sm );

    sm.setHeaderData( 0, Qt::Horizontal, "Column 0" );
    QCOMPARE( sm.rowCount(), 0 );

    QStandardItem *pitem = new QStandardItem( "Parent" );
    sm.insertRow( 0, pitem );
    QCOMPARE( sm.rowCount(), 1 );
    QCOMPARE( fm.rowCount(), 1 );
    QCOMPARE( sf.rowCount(), 1 );
    QCOMPARE( sm.index( 0, 0 ).data(), sf.index( 0, 0 ).data() );
    QCOMPARE( sf.index( 0, 0 ).data(), QVariant( "Parent" ) );

    QModelIndex parent = sm.index( 0, 0 );
    QVERIFY( parent.isValid() );

    QStandardItem *item = new QStandardItem( "First child" );
    pitem->insertRow( 0, item );
    QCOMPARE( sm.rowCount( parent ), 1 );
    QCOMPARE( fm.rowCount(), 2 );
    QCOMPARE( sf.rowCount(), 2 );
    QCOMPARE( sm.index( 0, 0, parent ).data(), sf.index( 1, 0 ).data() );
    QCOMPARE( sf.index( 1, 0 ).data(), QVariant( "First child" ) );

    item = new QStandardItem( "Second child" );
    pitem->insertRow( 1, item );
    QCOMPARE( sm.rowCount( parent ), 2 );
    QCOMPARE( fm.rowCount(), 3 );
    QCOMPARE( sf.rowCount(), 3 );
    QCOMPARE( sm.index( 0, 0, parent ).data(), sf.index( 1, 0 ).data() );
    QCOMPARE( sm.index( 1, 0, parent ).data(), sf.index( 2, 0 ).data() );
    QCOMPARE( sf.index( 1, 0 ).data(), QVariant( "First child" ) );
    QCOMPARE( sf.index( 2, 0 ).data(), QVariant( "Second child" ) );

    item = new QStandardItem( "In between" );
    pitem->insertRow( 1, item );
    QCOMPARE( sm.rowCount( parent ), 3 );
    QCOMPARE( fm.rowCount(), 4 );
    QCOMPARE( sf.rowCount(), 4 );
    QCOMPARE( sm.index( 0, 0, parent ).data(), sf.index( 1, 0 ).data() );
    QCOMPARE( sm.index( 1, 0, parent ).data(), sf.index( 2, 0 ).data() );
    QCOMPARE( sm.index( 2, 0, parent ).data(), sf.index( 3, 0 ).data() );
    QCOMPARE( sf.index( 1, 0 ).data(), QVariant( "First child" ) );
    QCOMPARE( sf.index( 2, 0 ).data(), QVariant( "In between" ) );
    QCOMPARE( sf.index( 3, 0 ).data(), QVariant( "Second child" ) );

    item = new QStandardItem( "Before first" );
    pitem->insertRow( 0, item );
    QCOMPARE( sm.rowCount( parent ), 4 );
    QCOMPARE( fm.rowCount(), 5 );
    QCOMPARE( sf.rowCount(), 5 );
    QCOMPARE( sm.index( 0, 0, parent ).data(), sf.index( 1, 0 ).data() );
    QCOMPARE( sm.index( 1, 0, parent ).data(), sf.index( 2, 0 ).data() );
    QCOMPARE( sm.index( 2, 0, parent ).data(), sf.index( 3, 0 ).data() );
    QCOMPARE( sm.index( 3, 0, parent ).data(), sf.index( 4, 0 ).data() );
    QCOMPARE( sf.index( 1, 0 ).data(), QVariant( "Before first" ) );
    QCOMPARE( sf.index( 2, 0 ).data(), QVariant( "First child" ) );
    QCOMPARE( sf.index( 3, 0 ).data(), QVariant( "In between" ) );
    QCOMPARE( sf.index( 4, 0 ).data(), QVariant( "Second child" ) );

    sm.removeRow( 0, parent );
    QCOMPARE( sm.rowCount( parent ), 3 );
    QCOMPARE( fm.rowCount(), 4 );
    QCOMPARE( sf.rowCount(), 4 );
    QCOMPARE( sm.index( 0, 0, parent ).data(), sf.index( 1, 0 ).data() );
    QCOMPARE( sm.index( 1, 0, parent ).data(), sf.index( 2, 0 ).data() );
    QCOMPARE( sm.index( 2, 0, parent ).data(), sf.index( 3, 0 ).data() );
    QCOMPARE( sf.index( 1, 0 ).data(), QVariant( "First child" ) );
    QCOMPARE( sf.index( 2, 0 ).data(), QVariant( "In between" ) );
    QCOMPARE( sf.index( 3, 0 ).data(), QVariant( "Second child" ) );

    sm.removeRow( 1, parent );
    QCOMPARE( sm.rowCount( parent ), 2 );
    QCOMPARE( fm.rowCount(), 3 );
    QCOMPARE( sf.rowCount(), 3 );
    QCOMPARE( sm.index( 0, 0, parent ).data(), sf.index( 1, 0 ).data() );
    QCOMPARE( sm.index( 1, 0, parent ).data(), sf.index( 2, 0 ).data() );
    QCOMPARE( sf.index( 1, 0 ).data(), QVariant( "First child" ) );
    QCOMPARE( sf.index( 2, 0 ).data(), QVariant( "Second child" ) );

    sm.removeRow( 1, parent );
    QCOMPARE( sm.rowCount( parent ), 1 );
    QCOMPARE( fm.rowCount(), 2 );
    QCOMPARE( sf.rowCount(), 2 );
    QCOMPARE( sm.index( 0, 0, parent ).data(), sf.index( 1, 0 ).data() );
    QCOMPARE( sf.index( 1, 0 ).data(), QVariant( "First child" ) );

    sm.removeRow( 0, parent );
    QCOMPARE( sm.rowCount( parent ), 0 );
    QCOMPARE( fm.rowCount(), 1 );
    QCOMPARE( sf.rowCount(), 1 );
    QCOMPARE( sm.index( 0, 0 ).data(), sf.index( 0, 0 ).data() );

    sm.removeRow( 0 );
    QCOMPARE( sm.rowCount(), 0 );
    QCOMPARE( fm.rowCount(), 0 );
    QCOMPARE( sf.rowCount(), 0 );
}

void FlatProxyModelTester::testInsertRemoveGrandChildren()
{
    QSortFilterProxyModel sf;
    FlatProxyModel fm;
    QStandardItemModel sm;

    sf.setSourceModel( &fm );
    fm.setSourceModel( &sm );

    sm.setHeaderData( 0, Qt::Horizontal, "Column 0" );
    QCOMPARE( sm.rowCount(), 0 );

    QStandardItem *pitem = new QStandardItem( "Parent" );
    sm.insertRow( 0, pitem );
    QCOMPARE( sm.rowCount(), 1 );
    QCOMPARE( fm.rowCount(), 1 );
    QCOMPARE( sf.rowCount(), 1 );
    QCOMPARE( sm.index( 0, 0 ).data(), sf.index( 0, 0 ).data() );
    QCOMPARE( sf.index( 0, 0 ).data(), QVariant( "Parent" ) );

    QModelIndex grandparent = sm.index( 0, 0 );
    QVERIFY( grandparent.isValid() );

    QStandardItem *gitem = new QStandardItem( "First child" );
    pitem->insertRow( 0, gitem );
    QCOMPARE( sm.rowCount( grandparent ), 1 );
    QCOMPARE( fm.rowCount(), 2 );
    QCOMPARE( sf.rowCount(), 2 );
    QCOMPARE( sm.index( 0, 0, grandparent ).data(), sf.index( 1, 0 ).data() );
    QCOMPARE( sf.index( 1, 0 ).data(), QVariant( "First child" ) );

    QModelIndex parent = sm.index( 0, 0, grandparent );
    QVERIFY( parent.isValid() );

    QStandardItem *item = new QStandardItem( "First grandchild" );
    gitem->insertRow( 0, item );
    QCOMPARE( sm.rowCount( parent ), 1 );
    QCOMPARE( fm.rowCount(), 3 );
    QCOMPARE( sf.rowCount(), 3 );
    QCOMPARE( sm.index( 0, 0, parent ).data(), sf.index( 2, 0 ).data() );
    QCOMPARE( sf.index( 2, 0 ).data(), QVariant( "First grandchild" ) );

    item = new QStandardItem( "Second grandchild" );
    gitem->insertRow( 1, item );
    QCOMPARE( sm.rowCount( parent ), 2 );
    QCOMPARE( fm.rowCount(), 4 );
    QCOMPARE( sf.rowCount(), 4 );
    QCOMPARE( sm.index( 0, 0, parent ).data(), sf.index( 2, 0 ).data() );
    QCOMPARE( sm.index( 1, 0, parent ).data(), sf.index( 3, 0 ).data() );
    QCOMPARE( sf.index( 2, 0 ).data(), QVariant( "First grandchild" ) );
    QCOMPARE( sf.index( 3, 0 ).data(), QVariant( "Second grandchild" ) );

    item = new QStandardItem( "In between" );
    gitem->insertRow( 1, item );
    QCOMPARE( sm.rowCount( parent ), 3 );
    QCOMPARE( fm.rowCount(), 5 );
    QCOMPARE( sf.rowCount(), 5 );
    QCOMPARE( sm.index( 0, 0, parent ).data(), sf.index( 2, 0 ).data() );
    QCOMPARE( sm.index( 1, 0, parent ).data(), sf.index( 3, 0 ).data() );
    QCOMPARE( sm.index( 2, 0, parent ).data(), sf.index( 4, 0 ).data() );
    QCOMPARE( sf.index( 2, 0 ).data(), QVariant( "First grandchild" ) );
    QCOMPARE( sf.index( 3, 0 ).data(), QVariant( "In between" ) );
    QCOMPARE( sf.index( 4, 0 ).data(), QVariant( "Second grandchild" ) );

    item = new QStandardItem( "Before first" );
    gitem->insertRow( 0, item );
    QCOMPARE( sm.rowCount( parent ), 4 );
    QCOMPARE( fm.rowCount(), 6 );
    QCOMPARE( sf.rowCount(), 6 );
    QCOMPARE( sm.index( 0, 0, parent ).data(), sf.index( 2, 0 ).data() );
    QCOMPARE( sm.index( 1, 0, parent ).data(), sf.index( 3, 0 ).data() );
    QCOMPARE( sm.index( 2, 0, parent ).data(), sf.index( 4, 0 ).data() );
    QCOMPARE( sm.index( 3, 0, parent ).data(), sf.index( 5, 0 ).data() );
    QCOMPARE( sf.index( 2, 0 ).data(), QVariant( "Before first" ) );
    QCOMPARE( sf.index( 3, 0 ).data(), QVariant( "First grandchild" ) );
    QCOMPARE( sf.index( 4, 0 ).data(), QVariant( "In between" ) );
    QCOMPARE( sf.index( 5, 0 ).data(), QVariant( "Second grandchild" ) );

    sm.removeRow( 0, parent );
    QCOMPARE( sm.rowCount( parent ), 3 );
    QCOMPARE( fm.rowCount(), 5 );
    QCOMPARE( sf.rowCount(), 5 );
    QCOMPARE( sm.index( 0, 0, parent ).data(), sf.index( 2, 0 ).data() );
    QCOMPARE( sm.index( 1, 0, parent ).data(), sf.index( 3, 0 ).data() );
    QCOMPARE( sm.index( 2, 0, parent ).data(), sf.index( 4, 0 ).data() );
    QCOMPARE( sf.index( 2, 0 ).data(), QVariant( "First grandchild" ) );
    QCOMPARE( sf.index( 3, 0 ).data(), QVariant( "In between" ) );
    QCOMPARE( sf.index( 4, 0 ).data(), QVariant( "Second grandchild" ) );

    sm.removeRow( 1, parent );
    QCOMPARE( sm.rowCount( parent ), 2 );
    QCOMPARE( fm.rowCount(), 4 );
    QCOMPARE( sf.rowCount(), 4 );
    QCOMPARE( sm.index( 0, 0, parent ).data(), sf.index( 2, 0 ).data() );
    QCOMPARE( sm.index( 1, 0, parent ).data(), sf.index( 3, 0 ).data() );
    QCOMPARE( sf.index( 2, 0 ).data(), QVariant( "First grandchild" ) );
    QCOMPARE( sf.index( 3, 0 ).data(), QVariant( "Second grandchild" ) );

    sm.removeRow( 1, parent );
    QCOMPARE( sm.rowCount( parent ), 1 );
    QCOMPARE( fm.rowCount(), 3 );
    QCOMPARE( sf.rowCount(), 3 );
    QCOMPARE( sm.index( 0, 0, parent ).data(), sf.index( 2, 0 ).data() );
    QCOMPARE( sf.index( 2, 0 ).data(), QVariant( "First grandchild" ) );

    sm.removeRow( 0, parent );
    QCOMPARE( sm.rowCount( parent ), 0 );
    QCOMPARE( fm.rowCount(), 2 );
    QCOMPARE( sf.rowCount(), 2 );
    QCOMPARE( sm.index( 0, 0, grandparent ).data(), sf.index( 1, 0 ).data() );
    QCOMPARE( sf.index( 1, 0 ).data(), QVariant( "First child" ) );

    QCOMPARE( sm.rowCount( grandparent ), 1 );
    sm.removeRow( 0, grandparent );
    QCOMPARE( sm.rowCount( grandparent ), 0 );
    QCOMPARE( fm.rowCount(), 1 );
    QCOMPARE( sf.rowCount(), 1 );
    QCOMPARE( sm.index( 0, 0 ).data(), sf.index( 0, 0 ).data() );
    QCOMPARE( sf.index( 0, 0 ).data(), QVariant( "Parent" ) );

    QCOMPARE( sm.rowCount(), 1 );
    sm.removeRow( 0 );
    QCOMPARE( sm.rowCount(), 0 );
    QCOMPARE( fm.rowCount(), 0 );
    QCOMPARE( sf.rowCount(), 0 );
}

} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::FlatProxyModelTester )

#include "FlatProxyModelTester.moc"
