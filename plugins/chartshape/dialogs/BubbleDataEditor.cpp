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

#include "BubbleDataEditor.h"

#include <QSortFilterProxyModel>
#include <QAbstractTableModel>
#include <QAction>

#include <KoIcon.h>

#include "ChartShape.h"
#include "ChartProxyModel.h"
#include "ChartTableView.h"
#include "ChartTableModel.h"
#include "DataSet.h"
#include "ChartDebug.h"



namespace KoChart {

namespace Bubble {

class DataProxy : public QSortFilterProxyModel
{
public:
    DataProxy(QObject *parent = 0) : QSortFilterProxyModel(parent) {}

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if (role == Qt::DisplayRole) {
            if (orientation == Qt::Horizontal) {
                return CellRegion::columnName(section + 2);
            } else {
                return section + 1;
            }
        }
        return QSortFilterProxyModel::headerData(section, orientation, role);
    }
    bool filterAcceptsColumn(int source_column, const QModelIndex &/*source_parent*/) const override
    {
        return source_column != 0; // skip categories
    }
    bool insertColumns(int column, int count, const QModelIndex &parent) override
    {
        debugChartUiBubble<<column;
        Q_UNUSED(count);
        Q_UNUSED(parent);

        QAbstractItemModel *model = sourceModel();
        int scolumn = mapToSource(index(0, column)).column();
        if (scolumn < 0) {
            scolumn = model->columnCount();
        }
        debugChartUiBubble<<column<<':'<<scolumn;
        if (!model->insertColumns(scolumn, 1)) {
            return false;
        }
        for (int r = 0; r < model->rowCount(); ++r) {
            if (r == 0) {
                QModelIndex idx = model->index(r, scolumn);
                model->setData(idx, i18n("Column %1", scolumn));
            } else {
                QModelIndex idx = model->index(r, scolumn);
                model->setData(idx, (double)r);
            }
        }
        return true;
    }
    bool insertRows(int row, int count, const QModelIndex &parent) override
    {
        debugChartUiBubble<<row;
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

} // namespace Bubble

using namespace Bubble;

BubbleDataEditor::BubbleDataEditor(ChartShape *chart, QWidget *parent)
    : KoDialog(parent)
    , m_chart(chart)
{
    setCaption(i18n("Bubble Data Editor"));
    setButtons(KoDialog::Ok | KoDialog::Cancel);
    QWidget *w = new QWidget(this);
    m_ui.setupUi(w);
    setMainWidget(w);

    m_insertColumnBeforeAction = new QAction(m_ui.insertColumnBefore->icon(), i18n("Insert Column Before"), m_ui.tableView);
    m_insertColumnAfterAction = new QAction(m_ui.insertColumnAfter->icon(), i18n("Insert Column After"), m_ui.tableView);
    m_insertRowAboveAction = new QAction(m_ui.insertRowAbove->icon(), i18n("Insert Row Above"), m_ui.tableView);
    m_insertRowBelowAction = new QAction(m_ui.insertRowBelow->icon(), i18n("Insert Row Below"), m_ui.tableView);
    m_deleteAction = new QAction(m_ui.deleteSelection->icon(), i18n("Delete"), m_ui.tableView);


    m_ui.tableView->addAction(m_insertColumnBeforeAction);
    m_ui.tableView->addAction(m_insertColumnAfterAction);
    m_ui.tableView->addAction(m_insertRowAboveAction);
    m_ui.tableView->addAction(m_insertRowBelowAction);
    m_ui.tableView->addAction(m_deleteAction);

    m_ui.tableView->setContextMenuPolicy(Qt::ActionsContextMenu);

    m_ui.deleteSelection->setEnabled(false);
    m_deleteAction->setEnabled(false);

//     m_dataSetModel = new DataSetTableModel(m_ui.dataSetView);
    m_dataSetModel.tableSource = m_chart->tableSource();
    connect(m_ui.addDataSetBefore, SIGNAL(clicked()), this, SLOT(slotAddDataSetBefore()));
    connect(m_ui.addDataSetAfter, SIGNAL(clicked()), this, SLOT(slotAddDataSetAfter()));
    connect(m_ui.removeDataSet, SIGNAL(clicked()), this, SLOT(slotRemoveDataSet()));

    m_dataSetModel.setModel(m_chart->proxyModel());
//     connect(&m_dataSetModel, &DataSetTableModel::dataChanged, this, &BubbleDataEditor::slotDataChanged);

    m_dataModel = new DataProxy(m_ui.tableView);
    m_dataModel->setSourceModel(m_chart->internalModel());

    connect(m_ui.insertColumnBefore, SIGNAL(clicked()), this, SLOT(slotInsertColumnBefore()));
    connect(m_ui.insertColumnAfter, SIGNAL(clicked()), this, SLOT(slotInsertColumnAfter()));
    connect(m_ui.insertRowAbove, SIGNAL(clicked()), this, SLOT(slotInsertRowAbove()));
    connect(m_ui.insertRowBelow, SIGNAL(clicked()), this, SLOT(slotInsertRowBelow()));
    connect(m_ui.deleteSelection,SIGNAL(clicked()), this, SLOT(slotDeleteSelection()));

    connect(m_insertColumnBeforeAction, SIGNAL(triggered()), this, SLOT(slotInsertColumnBefore()));
    connect(m_insertColumnAfterAction, SIGNAL(triggered()), this, SLOT(slotInsertColumnAfter()));
    connect(m_insertRowAboveAction, SIGNAL(triggered()), this, SLOT(slotInsertRowAbove()));
    connect(m_insertRowBelowAction, SIGNAL(triggered()), this, SLOT(slotInsertRowBelow()));
    connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(slotDeleteSelection()));

    connect(m_ui.tableView, SIGNAL(currentIndexChanged(QModelIndex)), this, SLOT(enableActions()));

    m_ui.tableView->setModel(m_dataModel);
    m_ui.dataSetView->setModel(&m_dataSetModel);

    DataColumnDelegate *delegate = new DataColumnDelegate(m_ui.dataSetView);
    delegate->dataModel = m_dataModel;
    m_ui.dataSetView->setItemDelegateForColumn(0, delegate);
    m_ui.dataSetView->setItemDelegateForColumn(1, delegate);
    m_ui.dataSetView->setItemDelegateForColumn(2, delegate);
    m_ui.dataSetView->setItemDelegateForColumn(3, delegate);

    m_ui.dataSetView->verticalHeader()->hide();

    connect(m_ui.manualControl, SIGNAL(toggled(bool)), m_chart->proxyModel(), SLOT(setManualControl(bool)));

    connect(m_ui.tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)), this, SLOT(enableActions()));
    connect(m_ui.dataSetView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)), this, SLOT(enableActions()));
    connect(m_ui.manualControl, SIGNAL(clicked()), this, SLOT(enableActions()));

    chart->proxyModel()->setManualControl(m_ui.manualControl->isChecked());
    enableActions();

    connect(m_dataModel, SIGNAL(columnsInserted(const QModelIndex &, int, int)), this, SLOT(dataColumnsInserted(QModelIndex,int,int)));
    connect(m_dataModel, SIGNAL(columnsRemoved(const QModelIndex &, int, int)), this, SLOT(dataColumnsRemoved(QModelIndex,int,int)));
    connect(m_dataModel->sourceModel(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(dataRowCountChanged()));
    connect(m_dataModel->sourceModel(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(dataRowCountChanged()));

    resize(sizeHint().expandedTo(QSize(600, 300)));

}

BubbleDataEditor::~BubbleDataEditor()
{
}

void BubbleDataEditor::slotInsertColumnBefore()
{
    debugChartUiBubble<<m_ui.tableView->currentIndex();
    int pos = m_ui.tableView->currentIndex().column();
    if (pos < 0) {
        pos = 0;
    }
    m_dataModel->insertColumn(pos);
}

void BubbleDataEditor::slotInsertColumnAfter()
{
    debugChartUiBubble<<m_ui.tableView->currentIndex();
    int pos = m_ui.tableView->currentIndex().column() + 1;
    if (pos == 0) {
        pos = m_dataModel->columnCount();
    }
    m_dataModel->insertColumn(pos);
}

void BubbleDataEditor::slotInsertRowAbove()
{
    debugChartUiBubble<<m_ui.tableView->currentIndex();
    int pos = m_ui.tableView->currentIndex().row();
    if (pos < 0) {
        pos = 0;
    }
    m_dataModel->insertRow(pos);
}

void BubbleDataEditor::slotInsertRowBelow()
{
    debugChartUiBubble<<m_ui.tableView->currentIndex();
    int pos = m_ui.tableView->currentIndex().row() + 1;
    if (pos == 0) {
        pos = m_dataModel->rowCount();
    }
    m_dataModel->insertRow(pos);
}

void BubbleDataEditor::slotDeleteSelection()
{
    QAbstractItemModel *model = m_ui.tableView->model();
    QModelIndexList lst = m_ui.tableView->selectionModel()->selectedRows();
    if (!lst.isEmpty()) {
        for (int i = lst.count() - 1; i >= 0; --i) {
            model->removeRow(lst.at(i).row());
        }
    } else {
        QModelIndexList lst = m_ui.tableView->selectionModel()->selectedColumns();
        for (int i = lst.count() - 1; i >= 0; --i) {
            model->removeColumn(lst.at(i).column());
        }
    }
}

void BubbleDataEditor::slotAddDataSetBefore()
{
    debugChartUiBubble<<m_ui.tableView->currentIndex();
    int pos = m_ui.dataSetView->currentIndex().row();
    if (pos < 0) {
        pos = 0;
    }
    m_dataSetModel.model()->insertRow(pos);
}

void BubbleDataEditor::slotAddDataSetAfter()
{
    debugChartUiBubble<<m_ui.dataSetView->currentIndex();
    int pos = m_ui.dataSetView->currentIndex().row() + 1;
    if (pos == 0) {
        pos = m_dataSetModel.rowCount();
    }
    m_dataSetModel.model()->insertRow(pos);
}

void BubbleDataEditor::slotRemoveDataSet()
{
    int row = m_ui.dataSetView->selectionModel()->currentIndex().row();
    if (row > 0) {
        m_ui.dataSetView->model()->removeRow(row);
    }
}

void BubbleDataEditor::enableActions()
{
    QItemSelectionModel *smodel = m_ui.tableView->selectionModel();
    m_ui.insertRowAbove->setEnabled(smodel && smodel->currentIndex().row() > 0);
    m_ui.deleteSelection->setEnabled(smodel && (!smodel->selectedRows().isEmpty() || !smodel->selectedColumns().isEmpty()));
    m_deleteAction->setEnabled(m_ui.deleteSelection->isEnabled());

    smodel = m_ui.dataSetView->selectionModel();
    m_ui.addDataSetBefore->setEnabled(m_ui.manualControl->isChecked());
    m_ui.addDataSetAfter->setEnabled(m_ui.manualControl->isChecked());
    m_ui.removeDataSet->setEnabled(m_ui.manualControl->isChecked() && smodel && smodel->currentIndex().isValid());
}

// void BubbleDataEditor::slotDataChanged(const QModelIndex &idx);
// {
//     debugChartUiBubble<<idx<<m_ui.tableView->itemDelegate(idx);
//     DataSet *ds = m_chart->proxyModel()->dataSets().value(idx.column());
//     if (!ds) {
//         return;
//     }
//     QVariant value = idx.data();
//     switch (idx.row()) {
//         case 0:
//
//             break;
//         case 1: {
//             QString s = ds->customDataRegion().toString().split('.').value(0) + '.' + value.toString();
//             emit xDataChanged(ds, CellRegion(m_chart->tableSource(), s));
//             break;
//         }
//         case 2: {
//             QString s = ds->customDataRegion().toString().split('.').value(0) + '.' + value.toString();
//             emit yDataChanged(ds, CellRegion(m_chart->tableSource(), s));
//             break;
//         }
//         case 3: {
//             QString s = ds->customDataRegion().toString().split('.').value(0) + '.' + value.toString();
//             emit bubbleDataChanged(ds, CellRegion(m_chart->tableSource(), s));
//             break;
//         }
//     }
// }

void BubbleDataEditor::dataColumnsInserted(const QModelIndex&, int first, int last)
{
    Q_ASSERT(first == last);
    if (!m_ui.manualControl->isChecked() || first == m_dataModel->columnCount() - 1) {
        return;
    }
    first += 2; // column 0 is hidden and rects starts at 1 (A=1, B=2...)
    for (DataSet *ds : m_chart->proxyModel()->dataSets()) {
        CellRegion region = ds->xDataRegion();
        for (QRect r : region.rects()) {
            if (r.left() >= first) {
                ds->setXDataRegion(CellRegion(region.table(), r.adjusted(1, 0, 1, 0)));
                debugChartUiBubble<<"move X:"<<first<<':'<<r<<region.toString()<<':'<<ds->xDataRegion().toString();
                break;
            }
        }
        region = ds->yDataRegion();
        for (QRect r : region.rects()) {
            if (r.left() >= first) {
                ds->setYDataRegion(CellRegion(region.table(), r.adjusted(1, 0, 1, 0)));
                debugChartUiBubble<<"move Y:"<<first<<':'<<r<<region.toString()<<':'<<ds->xDataRegion().toString();
                break;
            }
        }
        region = ds->customDataRegion();
        for (QRect r : region.rects()) {
            if (r.left() >= first) {
                debugChartUiBubble<<"move Cust:"<<first<<':'<<r;
                ds->setCustomDataRegion(CellRegion(region.table(), r.adjusted(1, 0, 1, 0)));
                break;
            }
        }
        region = ds->categoryDataRegion();
        for (QRect r : region.rects()) {
            if (r.left() >= first) {
                debugChartUiBubble<<"move Cat:"<<first<<':'<<r;
                ds->setCategoryDataRegion(CellRegion(region.table(), r.adjusted(1, 0, 1, 0)));
                break;
            }
        }
        region = ds->labelDataRegion();
        for (QRect r : region.rects()) {
            if (r.left() >= first) {
                debugChartUiBubble<<"move Lab:"<<first<<':'<<r;
                ds->setLabelDataRegion(CellRegion(region.table(), r.adjusted(1, 0, 1, 0)));
                break;
            }
        }
    }
}

void BubbleDataEditor::dataColumnsRemoved(const QModelIndex&, int first, int last)
{
    if (!m_ui.manualControl->isChecked()) {
        return;
    }
    int count = last - first + 1;
    first += 2; // column 0 is hidden and rects starts at 1 (A=1, B=2...)
    for (DataSet *ds : m_chart->proxyModel()->dataSets()) {
        CellRegion region = ds->xDataRegion();
        for (QRect r : region.rects()) {
            if (r.left() >= first) {
                ds->setXDataRegion(CellRegion(region.table(), r.adjusted(-count, 0, -count, 0)));
                debugChartUiBubble<<"move X:"<<first<<':'<<r<<region.toString()<<':'<<ds->xDataRegion().toString();
                break;
            }
        }
        region = ds->yDataRegion();
        for (QRect r : region.rects()) {
            if (r.left() >= first) {
                ds->setYDataRegion(CellRegion(region.table(), r.adjusted(-count, 0, -count, 0)));
                debugChartUiBubble<<"move Y:"<<first<<':'<<r<<region.toString()<<':'<<ds->xDataRegion().toString();
                break;
            }
        }
        region = ds->customDataRegion();
        for (QRect r : region.rects()) {
            if (r.left() >= first) {
                debugChartUiBubble<<"move Cust:"<<first<<':'<<r;
                ds->setCustomDataRegion(CellRegion(region.table(), r.adjusted(-count, 0, -count, 0)));
                break;
            }
        }
        region = ds->categoryDataRegion();
        for (QRect r : region.rects()) {
            if (r.left() >= first) {
                debugChartUiBubble<<"move Cat:"<<first<<':'<<r;
                ds->setCategoryDataRegion(CellRegion(region.table(), r.adjusted(-count, 0, -count, 0)));
                break;
            }
        }
        region = ds->labelDataRegion();
        for (QRect r : region.rects()) {
            if (r.left() >= first) {
                debugChartUiBubble<<"move Lab:"<<first<<':'<<r;
                ds->setLabelDataRegion(CellRegion(region.table(), r.adjusted(-count, 0, -count, 0)));
                break;
            }
        }
    }
}

void BubbleDataEditor::dataRowCountChanged()
{
    if (!m_chart->proxyModel()->manualControl()) {
        debugChartUiBubble<<"Not manual control";
        return;
    }
    const QList<DataSet*> lst = m_chart->proxyModel()->dataSets();
    for (int i = 0; i < lst.count(); ++i) {
        DataSet *ds = lst.at(i);
        CellRegion region = ds->xDataRegion();
        if (!region.rects().isEmpty()) {
            QRect r = region.rects().first();
            r.setHeight(m_dataModel->rowCount());
            ds->setXDataRegion(CellRegion(region.table(), r));
        }
        region = ds->yDataRegion();
        if (!region.rects().isEmpty()) {
            QRect r = region.rects().first();
            r.setHeight(m_dataModel->rowCount());
            ds->setYDataRegion(CellRegion(region.table(), r));
        }
        region = ds->customDataRegion();
        if (!region.rects().isEmpty()) {
            QRect r = region.rects().first();
            r.setHeight(m_dataModel->rowCount());
            ds->setCustomDataRegion(CellRegion(region.table(), r));
        }
        region = ds->categoryDataRegion();
        if (!region.rects().isEmpty()) {
            QRect r = region.rects().first();
            r.setHeight(m_dataModel->rowCount());
            ds->setCategoryDataRegion(CellRegion(region.table(), r));
        }
        debugChartUiBubble<<ds;
    }
}

} // namespace KoChart
