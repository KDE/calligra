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

#ifndef KOCHART_SCATTERDATAEDITOR_H
#define KOCHART_SCATTERDATAEDITOR_H

#include <KoDialog.h>

#include "ui_ScatterDataEditor.h"

#include "ScatterDataSetTableModel.h"

class QSortFilterProxyModel;
class QAbstractItemModel;
class QModelIndex;
class QAction;


namespace KoChart {

class ChartShape;
namespace Scatter {
class DataProxy;
}
class DataSet;
class CellRegion;

class ScatterDataEditor : public KoDialog
{
    Q_OBJECT

public:
    ScatterDataEditor(ChartShape *chart, QWidget *parent = 0);
    ~ScatterDataEditor();

Q_SIGNALS:
    void labelChanged();
    void xDataChanged(DataSet *dataSet, const CellRegion &region);
    void yDataChanged(DataSet *dataSet, const CellRegion &region);
    void bubbleDataChanged(DataSet *dataSet, const CellRegion &region);

protected Q_SLOTS:
    void slotInsertColumnBefore();
    void slotInsertColumnAfter();
    void slotInsertRowAbove();
    void slotInsertRowBelow();
    void slotDeleteSelection();

    void slotAddDataSetBefore();
    void slotAddDataSetAfter();
    void slotRemoveDataSet();

    void enableActions();

//     void slotDataChanged(const QModelIndex &idx);

    void dataColumnsInserted(const QModelIndex&, int first, int last);
    void dataColumnsRemoved(const QModelIndex&, int first, int last);
    void dataRowCountChanged();

private:
    ChartShape *m_chart;

    QAction *m_insertColumnBeforeAction;
    QAction *m_insertColumnAfterAction;
    QAction *m_insertRowAboveAction;
    QAction *m_insertRowBelowAction;
    QAction *m_deleteAction;

    Scatter::DataSetTableModel m_dataSetModel;
    Scatter::DataProxy *m_dataModel;

    Ui::ScatterDataEditor m_ui;
};

} // Namespace KoChart

#endif // KOCHART_SCATTERDATAEDITOR_H

