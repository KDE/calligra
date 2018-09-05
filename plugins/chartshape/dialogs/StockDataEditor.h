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

#ifndef KOCHART_STOCKDATAEDITOR_H
#define KOCHART_STOCKDATAEDITOR_H

#include <KoDialog.h>

#include "ui_StockDataEditor.h"

class QModelIndex;
class QAction;


namespace KoChart {

class ChartShape;
class DataProxy;

class StockDataEditor : public KoDialog
{
    Q_OBJECT

public:
    StockDataEditor(ChartShape *chart, QWidget *parent = 0);
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

