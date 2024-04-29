/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCHART_BUBBLEDATAEDITOR_H
#define KOCHART_BUBBLEDATAEDITOR_H

#include <KoDialog.h>

#include "ui_BubbleDataEditor.h"

#include "BubbleDataSetTableModel.h"

class QSortFilterProxyModel;
class QAbstractItemModel;
class QModelIndex;
class QAction;

namespace KoChart
{

class ChartShape;
class DataSet;
class CellRegion;
namespace Bubble
{
class DataProxy;
}

class BubbleDataEditor : public KoDialog
{
    Q_OBJECT

public:
    BubbleDataEditor(ChartShape *chart, QWidget *parent = nullptr);
    ~BubbleDataEditor();

Q_SIGNALS:
    void labelChanged();
    void xDataChanged(KoChart::DataSet *dataSet, const KoChart::CellRegion &region);
    void yDataChanged(KoChart::DataSet *dataSet, const KoChart::CellRegion &region);
    void bubbleDataChanged(KoChart::DataSet *dataSet, const KoChart::CellRegion &region);

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

    void dataColumnsInserted(const QModelIndex &, int first, int last);
    void dataColumnsRemoved(const QModelIndex &, int first, int last);
    void dataRowCountChanged();

private:
    ChartShape *m_chart;

    QAction *m_insertColumnBeforeAction;
    QAction *m_insertColumnAfterAction;
    QAction *m_insertRowAboveAction;
    QAction *m_insertRowBelowAction;
    QAction *m_deleteAction;

    Bubble::DataProxy *m_dataModel;
    Bubble::DataSetTableModel m_dataSetModel;

    Ui::BubbleDataEditor m_ui;
};

} // Namespace KoChart

#endif // KCHART_BUBBLEDATAEDITOR_H
