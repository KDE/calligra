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

#include "PieDataEditor.h"

#include <QSortFilterProxyModel>
#include <QAbstractItemModel>
#include <QAction>

#include <KoIcon.h>

#include "ChartProxyModel.h"
#include "ChartTableView.h"
#include "ChartDebug.h"



namespace KoChart {
class PieProxy : public QSortFilterProxyModel
{
public:
    PieProxy(QObject *parent = 0) : QSortFilterProxyModel(parent) {}

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
            switch (section) {
                case 0: return i18n("Category");
                case 1: return i18n("Value");
            }
        }
        return QSortFilterProxyModel::headerData(section, orientation, role);
    }
    bool filterAcceptsColumn(int source_column, const QModelIndex &/*source_parent*/) const override
    {
        return source_column < 2;
    }
    bool filterAcceptsRow(int source_row, const QModelIndex &/*source_parent*/) const override
    {
        return source_row != 0;
    }
    
};
}

using namespace KoChart;
PieDataEditor::PieDataEditor(QWidget *parent)
    : KoDialog(parent)
    , m_proxyModel(new PieProxy(this))
{
    setCaption(i18n("Pie Data Editor"));
    setButtons(KoDialog::Ok | KoDialog::Cancel);
    QWidget *w = new QWidget(this);
    m_ui.setupUi(w);
    setMainWidget(w);

    m_insertAction = new QAction(m_ui.insertRow->icon(), i18n("Insert"), m_ui.tableView);
    m_deleteAction = new QAction(m_ui.deleteSelection->icon(), i18n("Delete"), m_ui.tableView);

    connect(m_ui.insertRow, SIGNAL(pressed()), this, SLOT(slotInsertRow()));
    connect(m_ui.deleteSelection,SIGNAL(pressed()), this, SLOT(slotDeleteSelection()));

    connect(m_insertAction, SIGNAL(triggered()), this, SLOT(slotInsertRow()));
    connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(slotDeleteSelection()));

    connect(m_ui.tableView, SIGNAL(currentIndexChanged(QModelIndex)), this, SLOT(slotCurrentIndexChanged(QModelIndex)));

    m_ui.tableView->addAction(m_insertAction);
    m_ui.tableView->addAction(m_deleteAction);
    
    m_ui.tableView->setContextMenuPolicy(Qt::ActionsContextMenu);

    m_ui.deleteSelection->setEnabled(false);
    m_deleteAction->setEnabled(false);
}

PieDataEditor::~PieDataEditor()
{
}

void PieDataEditor::setModel(QAbstractItemModel *model)
{
    m_proxyModel->setSourceModel(model);
    m_ui.tableView->setModel(m_proxyModel);
    qInfo()<<Q_FUNC_INFO<<m_ui.tableView->itemDelegate()<<m_ui.tableView->itemDelegateForColumn(1);
}

void PieDataEditor::slotInsertRow()
{
    // Workaround: using proxymodel directly does not work
    int row = m_proxyModel->mapToSource(m_ui.tableView->currentIndex()).row() + 1; // insert after
    QAbstractItemModel *model =  m_proxyModel->sourceModel();
    model->insertRows(row, 1);
    QModelIndex idx = model->index(row, 1);
    model->setData(idx, 1.0);
}

void PieDataEditor::slotDeleteSelection()
{
    QModelIndexList rows = m_ui.tableView->selectionModel()->selectedRows();
    if (rows.isEmpty()) {
        rows << m_ui.tableView->currentIndex();
    }
    for (int i = rows.count() - 1; i >= 0; --i) {
        m_ui.tableView->model()->removeRow(rows.at(i).row());
    }
}

void PieDataEditor::slotCurrentIndexChanged(const QModelIndex &index)
{
    m_deleteAction->setEnabled(index.isValid());
    m_ui.deleteSelection->setEnabled(index.isValid());
}
