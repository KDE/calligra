/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "StockDataEditor.h"

#include <QAbstractTableModel>
#include <QAction>
#include <QSortFilterProxyModel>

#include <KoIcon.h>

#include "CellRegion.h"
#include "ChartDebug.h"
#include "ChartShape.h"
#include "ChartTableModel.h"

namespace KoChart
{

class DataProxy : public QSortFilterProxyModel
{
public:
    ChartShape *chart;

    DataProxy(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent)
        , chart(nullptr)
    {
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        if (index.row() == 0 && index.column() == 0) {
            return QSortFilterProxyModel::flags(index) & ~Qt::ItemIsEditable;
        }
        return QSortFilterProxyModel::flags(index);
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if (role == Qt::DisplayRole) {
            if (orientation == Qt::Horizontal) {
                return CellRegion::columnName(section + 1);
            } else {
                return section + 1;
            }
        }
        return QSortFilterProxyModel::headerData(section, orientation, role);
    }
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const override
    {
        Q_UNUSED(source_column);
        Q_UNUSED(source_parent);
        return true;
    }
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override
    {
        Q_UNUSED(source_parent);
        return source_row >= 0;
    }
    bool insertRows(int row, int count, const QModelIndex &parent) override
    {
        debugChartUiStock << row;
        Q_UNUSED(count);
        Q_UNUSED(parent);

        QAbstractItemModel *model = sourceModel();
        int srow = mapToSource(index(row, 0)).row();
        if (srow < 0) {
            srow = model->rowCount();
        }
        if (!model->insertRows(srow, 1)) {
            return false;
        }
        for (int c = 1; c < model->columnCount(); ++c) {
            QModelIndex idx = model->index(srow, c);
            model->setData(idx, (double)c);
        }
        return true;
    }
};

StockDataEditor::StockDataEditor(ChartShape *chart, QWidget *parent)
    : KoDialog(parent)
    , m_chart(chart)
{
    setCaption(i18n("Stock Data Editor"));
    setButtons(KoDialog::Ok | KoDialog::Cancel);
    QWidget *w = new QWidget(this);
    m_ui.setupUi(w);
    setMainWidget(w);

    // m_ui.tableView->verticalHeader()->hide();

    m_insertRowAboveAction = new QAction(m_ui.insertRowAbove->icon(), i18n("Insert Row Above"), m_ui.tableView);
    m_insertRowBelowAction = new QAction(m_ui.insertRowBelow->icon(), i18n("Insert Row Below"), m_ui.tableView);
    m_deleteAction = new QAction(m_ui.deleteSelection->icon(), i18n("Delete Row"), m_ui.tableView);

    m_ui.tableView->addAction(m_insertRowAboveAction);
    m_ui.tableView->addAction(m_insertRowBelowAction);
    m_ui.tableView->addAction(m_deleteAction);

    m_ui.tableView->setContextMenuPolicy(Qt::ActionsContextMenu);

    m_ui.deleteSelection->setEnabled(false);
    m_deleteAction->setEnabled(false);

    m_dataModel = new DataProxy(m_ui.tableView);
    m_dataModel->chart = chart;
    m_dataModel->setSourceModel(m_chart->internalModel());

    connect(m_ui.insertRowAbove, &QAbstractButton::clicked, this, &StockDataEditor::slotInsertRowAbove);
    connect(m_ui.insertRowBelow, &QAbstractButton::clicked, this, &StockDataEditor::slotInsertRowBelow);
    connect(m_ui.deleteSelection, &QAbstractButton::clicked, this, &StockDataEditor::slotDeleteSelection);

    connect(m_insertRowAboveAction, &QAction::triggered, this, &StockDataEditor::slotInsertRowAbove);
    connect(m_insertRowBelowAction, &QAction::triggered, this, &StockDataEditor::slotInsertRowBelow);
    connect(m_deleteAction, &QAction::triggered, this, &StockDataEditor::slotDeleteSelection);

    connect(m_ui.tableView, &ChartTableView::currentIndexChanged, this, &StockDataEditor::enableActions);

    m_ui.tableView->setModel(m_dataModel);

    connect(m_ui.tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &StockDataEditor::enableActions);

    enableActions();

    resize(sizeHint().expandedTo(QSize(600, 300)));
}

StockDataEditor::~StockDataEditor() = default;

void StockDataEditor::slotInsertRowAbove()
{
    debugChartUiStock << m_ui.tableView->currentIndex();
    int pos = m_ui.tableView->currentIndex().row();
    if (pos < 0) {
        pos = 0;
    }
    m_dataModel->insertRow(pos);
}

void StockDataEditor::slotInsertRowBelow()
{
    debugChartUiStock << m_ui.tableView->currentIndex();
    int pos = m_ui.tableView->currentIndex().row() + 1;
    if (pos == 0) {
        pos = m_dataModel->rowCount();
    }
    m_dataModel->insertRow(pos);
}

void StockDataEditor::slotDeleteSelection()
{
    QAbstractItemModel *model = m_ui.tableView->model();
    QModelIndexList lst = m_ui.tableView->selectionModel()->selectedIndexes();
    QMap<int, int> rows;
    for (int i = 0; i < lst.count(); ++i) {
        rows.insert(lst.at(i).row(), lst.at(i).row());
    }
    while (!rows.isEmpty()) {
        model->removeRow(rows.take(rows.lastKey()));
    }
}

void StockDataEditor::enableActions()
{
    QItemSelectionModel *smodel = m_ui.tableView->selectionModel();
    m_ui.deleteSelection->setEnabled(smodel && smodel->hasSelection());
    m_deleteAction->setEnabled(m_ui.deleteSelection->isEnabled());
}

} // namespace KoChart
