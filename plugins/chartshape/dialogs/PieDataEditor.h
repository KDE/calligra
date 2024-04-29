/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KCHART_PIEDATAEDITOR_H
#define KCHART_PIEDATAEDITOR_H

#include <KoDialog.h>

#include "ui_PieDataEditor.h"

class QSortFilterProxyModel;
class QAbstractItemModel;
class QModelIndex;
class QAction;

namespace KoChart
{

class PieDataEditor : public KoDialog
{
    Q_OBJECT

public:
    PieDataEditor(QWidget *parent = nullptr);
    ~PieDataEditor();

    void setModel(QAbstractItemModel *model);

protected Q_SLOTS:
    void slotInsertRow();
    void slotDeleteSelection();

    void slotCurrentIndexChanged(const QModelIndex &index);

private:
    QSortFilterProxyModel *m_proxyModel;

    QAction *m_insertAction;
    QAction *m_deleteAction;

    Ui::PieDataEditor m_ui;
};

} // Namespace KoChart

#endif // KCHART_PIEDATAEDITOR_H
