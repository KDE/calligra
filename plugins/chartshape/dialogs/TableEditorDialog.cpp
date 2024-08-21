/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "TableEditorDialog.h"

// Qt
#include <QAbstractItemModel>
#include <QAction>

// Calligra
#include <KoIcon.h>

// KoChart
#include "ChartDebug.h"
#include "ChartProxyModel.h"
#include "ChartTableView.h"

// Other
#include <algorithm>

using namespace KoChart;

TableEditorDialog::TableEditorDialog()
    : QDialog(nullptr)
    , m_tableView(new ChartTableView)
{
    setupUi(this);

    m_proxyModel = nullptr;
    init();
}

TableEditorDialog::~TableEditorDialog()
{
    delete m_tableView;
}

void TableEditorDialog::init()
{
    tableViewContainer->addWidget(m_tableView);

    const QIcon insertRowAboveIcon = koIcon("edit-table-insert-row-above");
    const QIcon insertRowBelowIcon = koIcon("edit-table-insert-row-below");
    const QIcon insertColLeftIcon = koIcon("edit-table-insert-column-left");
    const QIcon insertColRightIcon = koIcon("edit-table-insert-column-right");
    const QIcon deleteSelectionIcon = koIcon("edit-delete");

    // Create actions.
    m_insertRowAboveAction = new QAction(insertRowAboveIcon, i18n("Insert Above"), m_tableView);
    m_insertRowBelowAction = new QAction(insertRowBelowIcon, i18n("Insert Below"), m_tableView);
    m_insertColumnLeftAction = new QAction(insertColLeftIcon, i18n("Insert Before"), m_tableView);
    m_insertColumnRightAction = new QAction(insertColRightIcon, i18n("Insert After"), m_tableView);
    m_deleteSelectionAction = new QAction(deleteSelectionIcon, i18n("Delete Selection"), m_tableView);

    // Initially, no index is selected. Deletion only works with legal
    // selections.  They will automatically be enabled when an index
    // is selected.
    deleteSelection->setEnabled(false);

    // Buttons
    connect(insertRowAbove, &QAbstractButton::pressed, this, &TableEditorDialog::slotInsertRowAbovePressed);
    connect(insertRowBelow, &QAbstractButton::pressed, this, &TableEditorDialog::slotInsertRowBelowPressed);
    connect(insertColumnLeft, &QAbstractButton::pressed, this, &TableEditorDialog::slotInsertColumnLeftPressed);
    connect(insertColumnRight, &QAbstractButton::pressed, this, &TableEditorDialog::slotInsertColumnRightPressed);
    connect(deleteSelection, &QAbstractButton::pressed, this, &TableEditorDialog::slotDeleteSelectionPressed);

    // Context Menu Actions
    connect(m_insertRowAboveAction, &QAction::triggered, this, &TableEditorDialog::slotInsertRowAbovePressed);
    connect(m_insertRowBelowAction, &QAction::triggered, this, &TableEditorDialog::slotInsertRowBelowPressed);
    connect(m_insertColumnLeftAction, &QAction::triggered, this, &TableEditorDialog::slotInsertColumnLeftPressed);
    connect(m_insertColumnRightAction, &QAction::triggered, this, &TableEditorDialog::slotInsertColumnRightPressed);
    connect(m_deleteSelectionAction, &QAction::triggered, this, &TableEditorDialog::slotDeleteSelectionPressed);
    connect(m_tableView, &ChartTableView::currentIndexChanged, this, &TableEditorDialog::slotCurrentIndexChanged);

    // We only need to connect one of the data direction buttons, since they are mutually exclusive.
    connect(dataSetsInRows, &QAbstractButton::toggled, this, &TableEditorDialog::slotDataSetsInRowsToggled);

    // FIXME: QAction to create a separator??
    QAction *separator = new QAction(m_tableView);
    separator->setSeparator(true);

    // Add all the actions to the view.
    m_tableView->addAction(m_insertRowAboveAction);
    m_tableView->addAction(m_insertRowBelowAction);
    m_tableView->addAction(m_insertColumnLeftAction);
    m_tableView->addAction(m_insertColumnRightAction);
    m_tableView->addAction(m_deleteSelectionAction);

    m_tableView->setContextMenuPolicy(Qt::ActionsContextMenu);

    // Initialize the contents of the controls
    slotUpdateDialog();
}

void TableEditorDialog::setProxyModel(ChartProxyModel *proxyModel)
{
    if (m_proxyModel == proxyModel)
        return;

    // Disconnect the old proxy model.
    if (m_proxyModel) {
        m_proxyModel->disconnect(this);
    }

    m_proxyModel = proxyModel;

    // Connect the new proxy model.
    if (m_proxyModel) {
        connect(m_proxyModel, &QAbstractItemModel::modelReset, this, &TableEditorDialog::slotUpdateDialog);
    }

    slotUpdateDialog();
}

void TableEditorDialog::setModel(QAbstractItemModel *model)
{
    m_tableView->setModel(model);
    connect(m_tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &TableEditorDialog::slotSelectionChanged);
}

void TableEditorDialog::slotUpdateDialog()
{
    if (!m_proxyModel)
        return;

    switch (m_proxyModel->dataDirection()) {
    case Qt::Horizontal:
        dataSetsInRows->setChecked(true);
        break;
    case Qt::Vertical:
        dataSetsInColumns->setChecked(true);
        break;
    default:
        warnChart << "Unrecognized value for data direction: " << m_proxyModel->dataDirection();
    }
}

// ----------------------------------------------------------------
//                             slots

void TableEditorDialog::slotInsertRowAbovePressed()
{
    Q_ASSERT(m_tableView->model());

    QAbstractItemModel *model = m_tableView->model();
    QModelIndex currIndex = m_tableView->currentIndex();

    int row = 0;
    if (currIndex.isValid()) {
        row = currIndex.row();
    }
    if (model->rowCount() > 0 && row == 0) {
        row = 1;
    }
    if (model->insertRow(row)) {
        model->setData(model->index(row, 0), i18n("New Row"));
        for (int i = 1; i < model->columnCount(); ++i) {
            model->setData(model->index(row, i), 1.0);
        }
        m_tableView->scrollTo(model->index(row, 0));
        m_tableView->selectionModel()->setCurrentIndex(model->index(row, 0), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    }
}

void TableEditorDialog::slotInsertRowBelowPressed()
{
    Q_ASSERT(m_tableView->model());

    QAbstractItemModel *model = m_tableView->model();
    QModelIndex currIndex = m_tableView->currentIndex();

    int row = model->rowCount();
    if (currIndex.isValid()) {
        row = currIndex.row() + 1;
    }
    if (model->rowCount() > 0 && row == 0) {
        row = 1;
    }
    if (model->insertRow(row)) {
        model->setData(model->index(row, 0), i18n("New Row"));
        for (int i = 1; i < model->columnCount(); ++i) {
            model->setData(model->index(row, i), 1.0);
        }
        m_tableView->scrollTo(model->index(row, 0));
        m_tableView->selectionModel()->setCurrentIndex(model->index(row, 0), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    }
}

void TableEditorDialog::slotInsertColumnRightPressed()
{
    Q_ASSERT(m_tableView->model());

    QAbstractItemModel *model = m_tableView->model();
    QModelIndex currIndex = m_tableView->currentIndex();

    int col = model->columnCount();
    ;
    if (currIndex.isValid()) {
        col = currIndex.column() + 1;
    }
    if (model->columnCount() > 0 && col == 0) {
        col = 1;
    }
    if (model->insertColumn(col)) {
        model->setData(model->index(0, col), i18n("New Column"));
        for (int i = 1; i < model->rowCount(); ++i) {
            model->setData(model->index(i, col), 1.0);
        }
        m_tableView->scrollTo(model->index(0, col));
        m_tableView->selectionModel()->setCurrentIndex(model->index(0, col), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    }
}

void TableEditorDialog::slotInsertColumnLeftPressed()
{
    Q_ASSERT(m_tableView->model());

    QAbstractItemModel *model = m_tableView->model();
    QModelIndex currIndex = m_tableView->currentIndex();

    int col = 0;
    if (currIndex.isValid()) {
        col = currIndex.column();
    }
    if (model->columnCount() > 0 && col == 0) {
        col = 1;
    }
    if (model->insertColumn(col)) {
        model->setData(model->index(0, col), i18n("New Column"));
        for (int i = 1; i < model->rowCount(); ++i) {
            model->setData(model->index(i, col), 1.0);
        }
        m_tableView->scrollTo(model->index(0, col));
        m_tableView->selectionModel()->setCurrentIndex(model->index(0, col), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    }
}

void TableEditorDialog::slotDeleteSelectionPressed()
{
    if (!m_tableView->selectionModel()->selectedRows().isEmpty()) {
        deleteSelectedRowsOrColumns(Qt::Horizontal);
    } else if (!m_tableView->selectionModel()->selectedColumns().isEmpty()) {
        deleteSelectedRowsOrColumns(Qt::Vertical);
    }
}

void TableEditorDialog::deleteSelectedRowsOrColumns(Qt::Orientation orientation)
{
    // Note: In the following, both rows and columns will be referred to
    // as "row", for ease of reading this code.
    Q_ASSERT(m_tableView->model());

    const QModelIndexList selectedIndexes = m_tableView->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty())
        return;

    QList<int> rowsToBeRemoved;
    // Make sure we don't delete a row twice, as indexes can exist
    // multiple times for one row
    foreach (const QModelIndex &index, selectedIndexes) {
        const int row = orientation == Qt::Horizontal ? index.row() : index.column();
        if (!rowsToBeRemoved.contains(row))
            rowsToBeRemoved.append(row);
    }

    // Use std::greater<int>() as comparator to remove rows in reversed order
    // to not change the indexes of the selected rows
    std::sort(rowsToBeRemoved.begin(), rowsToBeRemoved.end(), std::greater<int>());

    foreach (int row, rowsToBeRemoved) {
        Q_ASSERT(row >= 0);
        if (orientation == Qt::Horizontal)
            m_tableView->model()->removeRow(row);
        else
            m_tableView->model()->removeColumn(row);
    }
    // Deselect the deleted rows
    m_tableView->setCurrentIndex(QModelIndex());
}

void TableEditorDialog::slotCurrentIndexChanged(const QModelIndex &index)
{
    qInfo() << Q_FUNC_INFO << index;
    if (!index.isValid()) {
        m_insertRowAboveAction->setEnabled(true);
        insertRowAbove->setEnabled(true);
        m_insertRowBelowAction->setEnabled(true);
        insertRowBelow->setEnabled(true);
        m_insertColumnLeftAction->setEnabled(true);
        insertColumnLeft->setEnabled(true);
        m_insertColumnRightAction->setEnabled(true);
        insertColumnRight->setEnabled(true);
        return;
    }
    if (index.row() == 0) {
        m_insertRowAboveAction->setEnabled(false);
        insertRowAbove->setEnabled(false);
        m_insertRowBelowAction->setEnabled(true);
        insertRowBelow->setEnabled(true);
    } else {
        m_insertRowAboveAction->setEnabled(true);
        insertRowAbove->setEnabled(true);
        m_insertRowBelowAction->setEnabled(true);
        insertRowBelow->setEnabled(true);
    }
    if (index.column() == 0) {
        m_insertColumnLeftAction->setEnabled(false);
        insertColumnLeft->setEnabled(false);
        m_insertColumnRightAction->setEnabled(true);
        insertColumnRight->setEnabled(true);
    } else {
        m_insertColumnLeftAction->setEnabled(true);
        insertColumnLeft->setEnabled(true);
        m_insertColumnRightAction->setEnabled(true);
        insertColumnRight->setEnabled(true);
    }
}

void TableEditorDialog::slotSelectionChanged()
{
    QModelIndexList rows = m_tableView->selectionModel()->selectedRows();
    QModelIndexList columns = m_tableView->selectionModel()->selectedColumns();
    bool disableDelete = rows.isEmpty() && columns.isEmpty();
    if (!disableDelete && !rows.isEmpty()) {
        for (const QModelIndex &idx : rows) {
            if (idx.row() == 0) {
                disableDelete = true;
            }
        }
    }
    if (!disableDelete && !columns.isEmpty()) {
        for (const QModelIndex &idx : columns) {
            if (idx.column() == 0) {
                disableDelete = true;
                break;
            }
        }
    }
    m_deleteSelectionAction->setEnabled(!disableDelete);
    deleteSelection->setEnabled(!disableDelete);
}

void TableEditorDialog::slotDataSetsInRowsToggled(bool enabled)
{
    Q_ASSERT(m_proxyModel);
    m_proxyModel->setDataDirection(enabled ? Qt::Horizontal : Qt::Vertical);
}
