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

#include "MctManagerBase.h"

#include "MctStaticData.h"
#include "MctChangeset.h"
#include "MctChange.h"
#include "MctUndoGraph.h"
#include "MctRedoGraph.h"
#include "MctAuthor.h"

MctManagerBase::MctManagerBase()
{

}

MctManagerBase::~MctManagerBase()
{

}

/**
 * @brief MctManagerBase::loadRevisionTrees Load nodes from revision tree into the ui container
 * @param undoTreeWidget ui element that will shows the undo revisions
 * @param redoTreeWidget ui element that will shows the redo revisions
 * @param undoMap optional, default is the revision map of the current document
 * @param redoMap optional, default is the revision map of the current document
 * @note ChangeSetMap = QMap<QDateTime, MctChangeset*>
 */
void MctManagerBase::loadRevisionTrees(QTreeWidget *undoTreeWidget, QTreeWidget *redoTreeWidget,
                                       ChangeSetMap *undoMap, ChangeSetMap *redoMap)
{
    undoTreeWidget->clear();
    redoTreeWidget->clear();

    if (!undoMap && !redoMap) {
        undoMap = MctStaticData::instance()->getUndoGraph()->changesetNodes();
        redoMap = MctStaticData::instance()->getRedoGraph()->changesetNodes();
    }

    undoTreeWidget->setColumnCount(2);
    redoTreeWidget->setColumnCount(2);

    undoTreeWidget->setHeaderHidden(false);
    redoTreeWidget->setHeaderHidden(false);

    // ez az alap allapot
    QTreeWidgetItem *initialState = new QTreeWidgetItem(undoTreeWidget);
    initialState->setText(0, QString::number(0));
    initialState->setText(1, "Initial state");
    initialState->setDisabled(true);

    if (undoMap->isEmpty()) {
        initialState->setTextColor(0, Qt::green);
        initialState->setTextColor(1, Qt::green);
    }

    int idx = 1;
    for (RevisionIterator it = undoMap->begin(); it != undoMap->end(); ++it) {

        bool isCurrent = false;
        if (it == --undoMap->end()) // utolso -> jelolt
            isCurrent = true;

        addRevision2Tree(undoTreeWidget, it, QString::number(idx++), isCurrent);
    }

    idx = 1;
    for (RevisionIterator it = redoMap->begin(); it != redoMap->end(); ++it) {      
        addRevision2Tree(redoTreeWidget, it, MctStaticData::REDOCHAR + QString::number(idx++));
    }

    undoTreeWidget->resizeColumnToContents(0);
    redoTreeWidget->resizeColumnToContents(0);
}

void MctManagerBase::addRevision2Tree(QTreeWidget* parent, RevisionIterator changeset, QString index, bool isCurrent)
{
    // date of revision
    QTreeWidgetItem *revTreeItem = new QTreeWidgetItem(parent);
    revTreeItem->setText(0, index); // revision string index
    revTreeItem->setText(1, changeset.key().toString()); // date of revision

    if ( isCurrent ) {
        revTreeItem->setTextColor(0, Qt::green);
        revTreeItem->setTextColor(1, Qt::green);
    }

    // + author
    QString author = changeset.value()->getAuthor()->getName();
    if (!author.isEmpty()) {
        QTreeWidgetItem *authorTreeItem = new QTreeWidgetItem(revTreeItem);
        authorTreeItem->setText(1, "by " + author);
        authorTreeItem->setDisabled(true);
    }

    // + comment
    QString comment = changeset.value()->getComment();
    if (!comment.isEmpty()) {
        QTreeWidgetItem *commTreeItem = new QTreeWidgetItem(revTreeItem);
        commTreeItem->setText(1, comment);
        commTreeItem->setDisabled(true);
    }

    // + changes
    QList<MctChange*> *changeList = changeset.value()->getChanges();
    for (ChangeIterator changeIt = changeList->begin(); changeIt != changeList->end(); ++changeIt) {
        addChange2Tree(revTreeItem, changeIt);
    }
}

void MctManagerBase::addChange2Tree(QTreeWidgetItem *parent, ChangeIterator change)
{
    QTreeWidgetItem *changeTreeItem = new QTreeWidgetItem();

    // This can be anything
    changeTreeItem->setText(1, MctStaticData::CHANGE_AS_STRING.value((*change)->getChangeType()));
    changeTreeItem->setDisabled(true);
    parent->addChild(changeTreeItem);
}

QString MctManagerBase::getParents(QList<QTreeWidgetItem *> selection, QList<MctChangeset*> *changesetNodes, QList<ulong> *childListAll, ChangeType type, MctUndoGraphXMLfilter *graphToMerge)
{
    QString additionalChangesets = "";
    int idx = selection.size() - 1;

    while (idx >= 0){
        QTreeWidgetItem *treeNode = selection.at(idx);
        idx--;
        QDateTime nodeDate = QDateTime::fromString(treeNode->text(1));
        MctChangeset* changesetNode = graphToMerge->getChangeset(nodeDate);
        if (changesetNode == nullptr)
            continue;

        changesetNodes->append(changesetNode);
        childListAll->append(changesetNode->getId());

    }

    if (changesetNodes->size() == 0)
        return additionalChangesets;

    for (auto it = changesetNodes->begin(); it != changesetNodes->end(); ++it){
        MctChangeset *cs = *it;
        QDateTime date = cs->getDate();
        QList<ulong> *parentlist = new QList<ulong>();

        MctChangeset *changesetNodeInGraph = nullptr;
        if (type == UNDOCHANGE)
            changesetNodeInGraph = MctStaticData::instance()->getUndoGraph()->getChangeset(date);
        else
            changesetNodeInGraph = MctStaticData::instance()->getRedoGraph()->getChangeset(date);

        if (changesetNodeInGraph != nullptr){
            changesetNodes->removeOne((*it));
        }

        this->checkParents(*it, parentlist, graphToMerge);

        foreach (ulong id, *parentlist){
            if (!childListAll->contains(id)){
                childListAll->append(id);
                QDateTime date = graphToMerge->getDateFromId(id);
                MctChangeset *changesetTemp = graphToMerge->getChangeset(date);
                QString tmpString = this->createNodeString(changesetTemp);
                additionalChangesets += tmpString + "\n";
                changesetNodes->append(changesetTemp);
            }
        }

        delete parentlist;
        parentlist = nullptr;
    }

    return additionalChangesets;
}

QString MctManagerBase::getChildren(QList<QTreeWidgetItem *> selection, QList<MctChangeset *> *changesetNodes, QList<ulong> *childListAll, ChangeType type, MctUndoGraphXMLfilter *graphToMerge)
{
    QString additionalChangesets = "";

    for (auto treeNode : selection){
        QDateTime nodeDate = QDateTime::fromString(treeNode->text(1));
        MctChangeset* changesetNode = graphToMerge->getChangeset(nodeDate);
        if (changesetNode == nullptr)
            continue;

        changesetNodes->append(changesetNode);
        childListAll->append(changesetNode->getId());
    }

    if (changesetNodes->size() == 0)
        return additionalChangesets;

    for (auto it = changesetNodes->begin(); it != changesetNodes->end(); ++it){
        QDateTime date = (*it)->getDate();
        QList<ulong> *childlist = new QList<ulong>();

        MctChangeset *changesetNodeInGraph = nullptr;
        if (type == UNDOCHANGE)
            changesetNodeInGraph = MctStaticData::instance()->getUndoGraph()->getChangeset(date);
        else
            changesetNodeInGraph = MctStaticData::instance()->getRedoGraph()->getChangeset(date);

        if (changesetNodeInGraph != nullptr){
            changesetNodes->removeOne(changesetNodeInGraph);
        }

        this->checkChildren(*it, childlist, graphToMerge);

        foreach (ulong id, *childlist){
            if (!childListAll->contains(id)){
                childListAll->append(id);
                QDateTime date = graphToMerge->getDateFromId(id);
                MctChangeset *changesetTemp = graphToMerge->getChangeset(date);
                QString tmpString = this->createNodeString(changesetTemp);
                additionalChangesets += tmpString + "\n";
                changesetNodes->append(changesetTemp);
            }
        }

        delete childlist;
        childlist = nullptr;
    }

    return additionalChangesets;
}

void MctManagerBase::checkChildren(MctChangeset *changesetNode, QList<ulong> *childlist, MctUndoGraphXMLfilter *graphToMerge)
{
    QList<ulong> *children = changesetNode->getChilds();

    if (children->size() == 0)
        return;

    foreach (ulong id, *children){
        if (!childlist->contains(id))
            childlist->append(id);
    }

    foreach (ulong id, *children){
        QDateTime date = graphToMerge->getDateFromId(id);
        MctChangeset *changesetNodeChild = graphToMerge->getChangeset(date);

        if (changesetNodeChild == nullptr){
            qDebug() << "changesetNode_child is None with Id: " << id;
        }

        this->checkChildren(changesetNodeChild, childlist, graphToMerge);
    }
}

void MctManagerBase::checkParents(MctChangeset *changesetNode, QList<ulong> *parentlist, MctUndoGraphXMLfilter *graphToMerge)
{
    QList<ulong> *parents = changesetNode->getParents();

    if (parents->size() == 0)
        return;

    qDebug() << "processing other parents";

    if (graphToMerge->redoOrUndo() == "undo"){
        this->filterChangesetNodes(parents, MctStaticData::instance()->getUndoGraph(), graphToMerge);
    } else {
        this->filterChangesetNodes(parents, MctStaticData::instance()->getRedoGraph(), graphToMerge);
    }

    foreach (ulong id, *parents){
        QDateTime date = graphToMerge->getDateFromId(id);
        MctChangeset *changesetNodeParent = graphToMerge->getChangeset(date);

        if (changesetNodeParent == nullptr){
            qDebug() << "changesetNode_parent is None with Id: " << id;
        }

        this->checkParents(changesetNodeParent, parentlist, graphToMerge);
    }
}

void MctManagerBase::filterChangesetNodes(QList<ulong> *changesetIdList, MctUndoGraphXMLfilter *graph, MctUndoGraphXMLfilter *graphToMerge)
{
    foreach (ulong id, *changesetIdList){
        QDateTime date = graphToMerge->getDateFromId(id);
        MctChangeset *changesetNodeInGraph = graph->getChangeset(date);

        if (changesetNodeInGraph == nullptr){
            continue;
        } else {
            changesetIdList->removeOne(id);
        }
    }
}

QString MctManagerBase::createNodeString(MctChangeset *changesetNode)
{
    QDateTime date = changesetNode->getDate();
    QString dateString = date.toString("yyyy.MM.dd");
    QString timeString = date.toString("(hh:mm:ss)");
    QString nodeString = dateString +  " " + timeString + " by: " + changesetNode->getAuthor()->getName();
    return nodeString;
}

QList<QDomNode> MctManagerBase::getTreeNodes(QList<QString> displayValues, MctUndoGraphXMLfilter *treeModel)
{
    QDomElement rootNode = treeModel->root();
    QList<QDomNode> treeNodes;

    int revisionsNum = rootNode.childNodes().count();
    int idx = 0;
    while (idx < revisionsNum){
        QDomNode treeNode = rootNode.childNodes().at(idx);
        QString displayVal = treeNode.nodeName();
        foreach (QString dispVal, displayValues){
            if (dispVal == displayVal){
                treeNodes.append(treeNode);
                break;
            }
        }
        idx++;
    }

    return treeNodes;
}
