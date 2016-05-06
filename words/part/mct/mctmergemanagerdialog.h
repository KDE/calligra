/* This file is part of the KDE project
 * Copyright (C) 2015-2016 MultiRacio Ltd. <multiracio@multiracio.com> (S.Schliszka, F.Novak, P.Rakyta)
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

#ifndef MCTMERGEMANAGERDIALOG_H
#define MCTMERGEMANAGERDIALOG_H

#include <QDialog>

#include <MctUndoGraph.h>
#include <MctRedoGraph.h>
#include <MctUndoClass.h>
#include <MctRedoClass.h>
#include "MctManagerBase.h"
#include "MctRevisionManager.h"

namespace Ui {
class MctMergeManagerDialog;
}

class MctMergeManagerDialog : public QDialog, public MctManagerBase
{
    Q_OBJECT

public:
    explicit MctMergeManagerDialog(QWidget *parent = 0, MctUndoClass *undoop = nullptr, MctRedoClass *redoop = nullptr);
    ~MctMergeManagerDialog();

private slots:
    void on_mergeRevButton_clicked();
    void on_browseButton_clicked();
    void on_tabCurrent_currentChanged(int index);
    void on_tabOpened_currentChanged(int index);

    void loadTreesOfOpened();

    void on_treeUndoOpened_itemSelectionChanged();

    void on_treeRedoOpened_itemSelectionChanged();

signals:
    void adjustListOfRevisions();
    void normalizeBuffer();

private:
    Ui::MctMergeManagerDialog *ui;
    MctUndoGraph *undoGraphOpened;
    MctRedoGraph *redoGraphOpened;
    MctUndoClass *undoop;
    MctRedoClass *redoop;
    KoTextDocument *doc2merge;

    void loadGraph();
    void mergingGraphs(ChangeType type, QTreeWidget *undoTreeWidget, QTreeWidget *redoTreeWidget, MctUndoGraph *undoGraph, MctRedoGraph *redoGraph);
    void checkChangesetNodesInGraph(QList<MctChangeset*> *changesetNodes, ChangeType type);
    void checkMergeButtonState(const QTreeWidget& treeOpenedWid, const QTreeWidget& treeCurrentWid);
};

#endif // MCTMERGEMANAGERDIALOG_H
