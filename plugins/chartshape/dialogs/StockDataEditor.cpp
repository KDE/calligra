/* This file is part of the KDE project
 *
 * Copyright 2018 Dag Andersen <danders@get2net.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "StockDataEditor.h"

#include <QSortFilterProxyModel>
#include <QAbstractTableModel>
#include <QAction>

#include <KoIcon.h>

#include "ChartShape.h"
#include "CellRegion.h"
#include "ChartTableModel.h"
#include "ChartDebug.h"



namespace KoChart {

class DataProxy : public QSortFilterProxyModel
{
public:
    ChartShape *chart;

    DataProxy(QObject *parent = 0) : QSortFilterProxyModel(parent), chart(0) {}

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
    bool filterAcceptsColumn(int source_column, const QModelIndex &/*source_parent*/) const override
    {
        return true;
    }
    bool filterAcceptsRow(int source_row, const QModelIndex &/*source_parent*/) const override
    {
        return source_row >= 0;
    }
    bool insertRows(int row, int count, const QModelIndex &parent) override
    {
        debugChartUiStock<<row;
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

    //m_ui.tableView->verticalHeader()->hide();

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

    connect(m_ui.insertRowAbove, SIGNAL(clicked()), this, SLOT(slotInsertRowAbove()));
    connect(m_ui.insertRowBelow, SIGNAL(clicked()), this, SLOT(slotInsertRowBelow()));
    connect(m_ui.deleteSelection,SIGNAL(clicked()), this, SLOT(slotDeleteSelection()));

    connect(m_insertRowAboveAction, SIGNAL(triggered()), this, SLOT(slotInsertRowAbove()));
    connect(m_insertRowBelowAction, SIGNAL(triggered()), this, SLOT(slotInsertRowBelow()));
    connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(slotDeleteSelection()));

    connect(m_ui.tableView, SIGNAL(currentIndexChanged(QModelIndex)), this, SLOT(enableActions()));

    m_ui.tableView->setModel(m_dataModel);

    connect(m_ui.tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)), this, SLOT(enableActions()));

    enableActions();

    resize(sizeHint().expandedTo(QSize(600, 300)));

}

StockDataEditor::~StockDataEditor()
{
}

void StockDataEditor::slotInsertRowAbove()
{
    debugChartUiStock<<m_ui.tableView->currentIndex();
    int pos = m_ui.tableView->currentIndex().row();
    if (pos < 0) {
        pos = 0;
    }
    m_dataModel->insertRow(pos);
}

void StockDataEditor::slotInsertRowBelow()
{
    debugChartUiStock<<m_ui.tableView->currentIndex();
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
