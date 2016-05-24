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

#ifndef MCTREVISIONMANAGER_H
#define MCTREVISIONMANAGER_H

#include <QDialog>
#include <QMap>
#include <QTreeView>
#include <QScreen>
#include <MctChangeset.h>
#include <MctUndoGraph.h>
#include <MctUndoClass.h>
#include <MctRedoClass.h>

class MctAbstractGraph;
class MctChangeset;

namespace Ui {
class MctRevisionManager;
}

class MctRevisionManager : public QDialog
{
    Q_OBJECT

public:
    explicit MctRevisionManager(QWidget *parent = 0,  MctUndoClass *m_undoop = nullptr, MctRedoClass *m_redoop = nullptr);
    ~MctRevisionManager();          

private slots:
    void on_undoButton_clicked();
    void on_deleteButton_clicked();
    void on_redoButton_clicked();
    void on_clearButton_clicked();

    void on_revTree_expanded(const QModelIndex &index);
    void on_undidRevTree_expanded(const QModelIndex &index);

signals:
    void clearingProcess();
    void adjustListOfRevisions();
    void normalizeBuffer();

private:
    Ui::MctRevisionManager *m_ui;
    QScreen* sc;

    QMap<QString, MctChangeset*> *m_undochangesetNodes;
    QMap<QString, MctChangeset*> *m_redochangesetNodes;
    MctUndoClass *m_undoop;
    MctRedoClass *m_redoop;

    QMap<QString, MctChangeset*>* fillRevTree(MctAbstractGraph *graph, QTreeView *view, bool isRedo = false);
    QString createNodeString(MctChangeset* changesetNode);
    QString getUndoChildren(QList<MctChangeset*> selection, QList<MctChangeset*>* changesetNodes, QList<ulong>* childListAll);
    QString getRedoChildren(QList<MctChangeset*> selection, QList<MctChangeset*>* changesetNodes, QList<ulong>* childListAll);
    void checkChildren(MctChangeset *changesetNode, QList<ulong> *childlist, MctAbstractGraph* graph);
    void undoRevs(QList<MctChangeset*> selection);
    void redoRevs(QList<MctChangeset*> selection);
    void delUndoRev(QList<MctChangeset*> selection);
    void delRedoRev(QList<MctChangeset*> selection);

    void adjustButtonAccess();

    void printChanges(QString type);
};

#endif // MCTREVISIONMANAGER_H
