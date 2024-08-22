/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "PieDataEditor.h"

#include <QAbstractItemModel>
#include <QAction>
#include <QSortFilterProxyModel>

#include <KoIcon.h>

#include "ChartDebug.h"
#include "ChartProxyModel.h"
#include "ChartTableView.h"

namespace KoChart
{
class PieProxy : public QSortFilterProxyModel
{
public:
    PieProxy(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent)
    {
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
            switch (section) {
            case 0:
                return i18n("Category");
            case 1:
                return i18n("Value");
            }
        }
        return QSortFilterProxyModel::headerData(section, orientation, role);
    }
    bool filterAcceptsColumn(int source_column, const QModelIndex & /*source_parent*/) const override
    {
        return source_column < 2;
    }
    bool filterAcceptsRow(int source_row, const QModelIndex & /*source_parent*/) const override
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

    connect(m_ui.insertRow, &QAbstractButton::pressed, this, &PieDataEditor::slotInsertRow);
    connect(m_ui.deleteSelection, &QAbstractButton::pressed, this, &PieDataEditor::slotDeleteSelection);

    connect(m_insertAction, &QAction::triggered, this, &PieDataEditor::slotInsertRow);
    connect(m_deleteAction, &QAction::triggered, this, &PieDataEditor::slotDeleteSelection);

    connect(m_ui.tableView, &ChartTableView::currentIndexChanged, this, &PieDataEditor::slotCurrentIndexChanged);

    m_ui.tableView->addAction(m_insertAction);
    m_ui.tableView->addAction(m_deleteAction);

    m_ui.tableView->setContextMenuPolicy(Qt::ActionsContextMenu);

    m_ui.deleteSelection->setEnabled(false);
    m_deleteAction->setEnabled(false);
}

PieDataEditor::~PieDataEditor() = default;

void PieDataEditor::setModel(QAbstractItemModel *model)
{
    m_proxyModel->setSourceModel(model);
    m_ui.tableView->setModel(m_proxyModel);
    qInfo() << Q_FUNC_INFO << m_ui.tableView->itemDelegate() << m_ui.tableView->itemDelegateForColumn(1);
}

void PieDataEditor::slotInsertRow()
{
    // Workaround: using proxymodel directly does not work
    int row = m_proxyModel->mapToSource(m_ui.tableView->currentIndex()).row() + 1; // insert after
    QAbstractItemModel *model = m_proxyModel->sourceModel();
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
