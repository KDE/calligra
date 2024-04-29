/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCHART_STOCKDATAEDITOR_H
#define KOCHART_STOCKDATAEDITOR_H

#include <KoDialog.h>

#include "ui_StockDataEditor.h"

class QModelIndex;
class QAction;

namespace KoChart
{

class ChartShape;
class DataProxy;

class StockDataEditor : public KoDialog
{
    Q_OBJECT

public:
    StockDataEditor(ChartShape *chart, QWidget *parent = nullptr);
    ~StockDataEditor();

protected Q_SLOTS:
    void slotInsertRowAbove();
    void slotInsertRowBelow();
    void slotDeleteSelection();

    void enableActions();

private:
    ChartShape *m_chart;

    DataProxy *m_dataModel;
    QAction *m_insertRowAboveAction;
    QAction *m_insertRowBelowAction;
    QAction *m_deleteAction;

    Ui::StockDataEditor m_ui;
};

} // Namespace KoChart

#endif // KCHART_STOCKDATAEDITOR_H
