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
#include "TableEditorDialog.h"

// Qt
#include <QDebug>

// KChart
#include "ChartProxyModel.h"
#include "ChartTableView.h"


using namespace KChart;

TableEditorDialog::TableEditorDialog()
    : QDialog( 0 )
    , tableView( new ChartTableView )
{
    setupUi( this );

    proxyModel = 0;
    init();
}

TableEditorDialog::~TableEditorDialog()
{
    delete tableView;
}

void TableEditorDialog::setProxyModel( ChartProxyModel* proxyModel )
{
    if ( this->proxyModel == proxyModel )
        return;

    // Disconnect the old proxy model.
    if ( this->proxyModel ) {
        disconnect( this->proxyModel,   SIGNAL( modelReset() ), this, SLOT( update() ) );
        disconnect( firstRowIsLabel,    SIGNAL( clicked( bool ) ),
                    this->proxyModel,   SLOT( setFirstRowIsLabel( bool ) ) );
        disconnect( firstColumnIsLabel, SIGNAL( clicked( bool ) ),
                    this->proxyModel,   SLOT( setFirstColumnIsLabel( bool ) ) );
    }

    this->proxyModel = proxyModel;

    // Connect the new proxy model.
    if ( proxyModel ) {
        tableView->setModel( proxyModel->sourceModel() );

        connect( proxyModel,         SIGNAL( modelReset() ), this, SLOT( update() ) );
        connect( firstRowIsLabel,    SIGNAL( clicked( bool ) ),
                 proxyModel,         SLOT( setFirstRowIsLabel( bool ) ) );
        connect( firstColumnIsLabel, SIGNAL( clicked( bool ) ),
                 proxyModel,         SLOT( setFirstColumnIsLabel( bool ) ) );
    }

    update();
}

void TableEditorDialog::init()
{
    tableViewContainer->addWidget( tableView );

    KIcon insertRowIcon = KIcon( "insert_table_row" );
    KIcon deleteRowIcon = KIcon( "delete_table_row" );
    KIcon insertColIcon = KIcon( "insert_table_col" );
    KIcon deleteColIcon = KIcon( "delete_table_col" );

    // Create actions.
    insertRowsAction    = new QAction( insertRowIcon, i18n( "Insert Rows" ), tableView );
    deleteRowsAction    = new QAction( deleteRowIcon, i18n( "Delete Rows" ), tableView );
    insertColumnsAction = new QAction( insertColIcon, i18n( "Insert Columns" ), tableView );
    deleteColumnsAction = new QAction( deleteColIcon, i18n( "Delete Columns" ), tableView );

    // Set icons on buttons(?).
    insertRow->setIcon( insertRowIcon );
    deleteRow->setIcon( deleteRowIcon );
    insertColumn->setIcon( insertColIcon );
    deleteColumn->setIcon( deleteColIcon );

    // Initially, no index is selected. Deletion only works with legal
    // selections.  They will automatically be enabled when an index
    // is selected.
    deleteRow->setEnabled( false );
    deleteColumn->setEnabled( false );

    // Buttons
    connect( insertRow,    SIGNAL( pressed() ), this, SLOT( slotInsertRowPressed() ) );
    connect( insertColumn, SIGNAL( pressed() ), this, SLOT( slotInsertColumnPressed() ) );
    connect( deleteRow,    SIGNAL( pressed() ), this, SLOT( slotDeleteRowPressed() ) );
    connect( deleteColumn, SIGNAL( pressed() ), this, SLOT( slotDeleteColumnPressed() ) );

    // Context Menu Actions
    connect( insertRowsAction,    SIGNAL( triggered() ), this, SLOT( slotInsertRowPressed() ) );
    connect( insertColumnsAction, SIGNAL( triggered() ), this, SLOT( slotInsertColumnPressed() ) );
    connect( deleteRowsAction,    SIGNAL( triggered() ), this, SLOT( slotDeleteRowPressed() ) );
    connect( deleteColumnsAction, SIGNAL( triggered() ), this, SLOT( slotDeleteColumnPressed() ) );
    connect( tableView,    SIGNAL( currentIndexChanged( const QModelIndex& ) ),
             this,         SLOT( slotCurrentIndexChanged( const QModelIndex& ) ) );
    // We only need to connect one of the data direction buttons, since
    // they are mutually exclusive.
    connect( dataSetsInRows, SIGNAL( toggled( bool ) ),
             this,           SLOT( slotDataSetsInRowsToggled( bool ) ) );

    QAction *separator = new QAction( tableView );
    separator->setSeparator( true );

    // Add all the actions to the view.
    tableView->addAction( deleteRowsAction );
    tableView->addAction( insertRowsAction );
    tableView->addAction( separator );
    tableView->addAction( deleteColumnsAction );
    tableView->addAction( insertColumnsAction );

    tableView->setContextMenuPolicy( Qt::ActionsContextMenu );
}

void TableEditorDialog::slotInsertRowPressed()
{
    Q_ASSERT( tableView->model() );

    QModelIndex currIndex = tableView->currentIndex();
    int selectedRow = -1;
    if ( !currIndex.isValid() )
        selectedRow = tableView->model()->rowCount() - 1;
    else
        selectedRow = currIndex.row();
    Q_ASSERT( selectedRow >= 0 );
    // Insert the row *after* the selection, thus +1
    tableView->model()->insertRow( selectedRow + 1 );
}

void TableEditorDialog::slotInsertColumnPressed()
{
    Q_ASSERT( tableView->model() );

    QModelIndex currIndex = tableView->currentIndex();
    int selectedColumn = -1;
    if ( !currIndex.isValid() )
        selectedColumn = tableView->model()->columnCount() - 1;
    else
        selectedColumn = currIndex.column();
    Q_ASSERT( selectedColumn >= 0 );
    // Insert the column *after* the selection, thus +1
    tableView->model()->insertColumn( selectedColumn + 1 );
}

void TableEditorDialog::slotDeleteRowPressed()
{
    deleteSelectedRowsOrColumns( Qt::Horizontal );
}

void TableEditorDialog::slotDeleteColumnPressed()
{
    deleteSelectedRowsOrColumns( Qt::Vertical );
}

void TableEditorDialog::deleteSelectedRowsOrColumns( Qt::Orientation orientation )
{
    // Note: In the following, both rows and columns will be referred to
    // as "row", for ease of reading this code.
    Q_ASSERT( tableView->model() );

    const QModelIndexList selectedIndexes = tableView->selectionModel()->selectedIndexes();
    if ( selectedIndexes.isEmpty() )
        return;

    QList<int> rowsToBeRemoved;
    // Make sure we don't delete a row twice, as indexes can exist
    // multiple times for one row
    foreach( const QModelIndex &index, selectedIndexes ) {
        const int row = orientation == Qt::Horizontal ? index.row() : index.column();
        if ( !rowsToBeRemoved.contains( row ) )
            rowsToBeRemoved.append( row );
    }

    // Use qGreater<int>() as comparator to remove rows in reversed order
    // to not change the indexes of the selected rows
    qSort( rowsToBeRemoved.begin(), rowsToBeRemoved.end(), qGreater<int>() );

    foreach( int row, rowsToBeRemoved ) {
        Q_ASSERT( row >= 0 );
        if ( orientation == Qt::Horizontal )
            tableView->model()->removeRow( row );
        else
            tableView->model()->removeColumn( row );
    }
    // Deselect the deleted rows
    tableView->setCurrentIndex( QModelIndex() );
}

void TableEditorDialog::slotCurrentIndexChanged( const QModelIndex &index )
{
    const bool isValid = index.isValid();
    deleteRowsAction->setEnabled( isValid );
    deleteColumnsAction->setEnabled( isValid );
    deleteRow->setEnabled( isValid );
    deleteColumn->setEnabled( isValid );
}

void TableEditorDialog::slotDataSetsInRowsToggled( bool enabled )
{
    Q_ASSERT( proxyModel );
    proxyModel->setDataDirection( enabled ? Qt::Horizontal : Qt::Vertical );
}

void TableEditorDialog::update()
{
    if ( !proxyModel )
        return;

    firstRowIsLabel->setChecked( proxyModel->firstRowIsLabel() );
    firstColumnIsLabel->setChecked( proxyModel->firstColumnIsLabel() );
    dataSetsInRows->setChecked( proxyModel->dataDirection() == Qt::Horizontal );
}
