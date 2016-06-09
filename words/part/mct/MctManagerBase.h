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

#ifndef MCTMANAGERBASE_H
#define MCTMANAGERBASE_H

class MctChangeset;
class MctChange;

#include <QMap>
#include <QList>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <MctUndoGraph.h>
#include <MctRedoGraph.h>

using RevisionIterator = QMap<QDateTime, MctChangeset*>::iterator;
using ChangeIterator = QList<MctChange*>::iterator;
using ChangeSetMap = QMap<QDateTime, MctChangeset*>;
typedef unsigned long ulong;

enum ChangeType {UNDOCHANGE, REDOCHANGE};

class MctManagerBase
{
public:
    MctManagerBase();
    ~MctManagerBase();

protected:
    void loadRevisionTrees(QTreeWidget *undoTreeWidget, QTreeWidget *redoTreeWidget,
                           ChangeSetMap *undoMap = nullptr, ChangeSetMap *redoMap = nullptr);
    void addRevision2Tree(QTreeWidget *parent, RevisionIterator changeset, const QString &index, bool isCurrent = false);
    void addChange2Tree(QTreeWidgetItem *parent, ChangeIterator change);
    QString lookForDependingParents(QList<QTreeWidgetItem*> selection, QList<MctChangeset*> *changesetNodes, QList<ulong> *childListAll, ChangeType type, MctUndoGraphXMLfilter *graphToMerge);
    QString lookForDependingChildren(QList<QTreeWidgetItem*> selection, QList<MctChangeset*> *changesetNodes, QList<ulong> *childListAll, ChangeType type, MctUndoGraphXMLfilter *graphToMerge);
    void checkParents(MctChangeset *changesetNode, QList<ulong> *parentlist, MctUndoGraphXMLfilter *graphToMerge);
    void checkChildren(MctChangeset *changesetNode, QList<ulong> *childlist, MctUndoGraphXMLfilter *graphToMerge);
    void filterChangesetNodes(QList<ulong> *changesetIdList, MctUndoGraphXMLfilter *graph, MctUndoGraphXMLfilter *graphToMerge);
    QString createNodeString(MctChangeset *changesetNode);
    QList<QDomNode> treeNodes(QList<QString> displayValues, MctUndoGraphXMLfilter *treeModel);

};

#endif // MCTMANAGERBASE_H
