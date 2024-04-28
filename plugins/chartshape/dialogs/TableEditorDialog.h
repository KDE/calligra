/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
   SPDX-FileCopyrightText: 2009 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_TABLE_EDITOR_DIALOG_H
#define KCHART_TABLE_EDITOR_DIALOG_H

#include <QDialog>

#include "ui_ChartTableEditor.h"

class QAbstractItemModel;
class QModelIndex;
class QAction;

namespace KoChart
{

class ChartTableView;
class ChartProxyModel;

class TableEditorDialog : public QDialog, public Ui::ChartTableEditor
{
    Q_OBJECT

public:
    TableEditorDialog();
    ~TableEditorDialog();

    void init();
    void setProxyModel(ChartProxyModel *proxyModel);
    void setModel(QAbstractItemModel *model);

protected Q_SLOTS:
    void slotUpdateDialog();
    void slotInsertRowAbovePressed();
    void slotInsertRowBelowPressed();
    void slotInsertColumnLeftPressed();
    void slotInsertColumnRightPressed();
    void slotDeleteSelectionPressed();

    void deleteSelectedRowsOrColumns(Qt::Orientation orientation);
    void slotCurrentIndexChanged(const QModelIndex &index);
    void slotSelectionChanged();
    void slotDataSetsInRowsToggled(bool enabled);

private:
    ChartProxyModel *m_proxyModel;
    ChartTableView *const m_tableView;

    QAction *m_deleteSelectionAction;
    QAction *m_insertRowAboveAction;
    QAction *m_insertColumnLeftAction;
    QAction *m_insertRowBelowAction;
    QAction *m_insertColumnRightAction;
};

} // Namespace KoChart

#endif // KCHART_TABLE_EDITOR_DIALOG_H
