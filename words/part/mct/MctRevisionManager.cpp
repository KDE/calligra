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

#include "MctRevisionManager.h"
#include "ui_MctRevisionManager.h"

#include "MctAbstractGraph.h"
#include "MctStaticData.h"
#include "MctUndoGraph.h"
#include "MctRedoGraph.h"
#include "MctChangeset.h"
#include "MctAuthor.h"
#include "KoDocument.h"
#include "MctMain.h"
#include "MctPosition.h"

#include <QStandardItemModel>
#include <QMessageBox>
#include <QToolTip>

MctRevisionManager::MctRevisionManager(QWidget *parent, MctUndoClass *undoop, MctRedoClass *redoop) :
    QDialog(parent),
    ui(new Ui::MctRevisionManager)
{
    ui->setupUi(this);

    //setWindowModality(Qt::WindowModal);

    this->undoop = undoop;
    this->redoop = redoop;

    undochangesetNodes = fillRevTree(MctStaticData::instance()->getUndoGraph(), ui->revTree, false);
    redochangesetNodes = fillRevTree(MctStaticData::instance()->getRedoGraph(), ui->undidRevTree, true);

    adjustButtonAccess();
}

MctRevisionManager::~MctRevisionManager()
{
    delete ui;
    delete undochangesetNodes;
    delete redochangesetNodes;
}

QMap<QString, MctChangeset*>* MctRevisionManager::fillRevTree(MctAbstractGraph *graph, QTreeView *view, bool isRedo)
{
    QMap<QString, MctChangeset*>* changesetNodesMap = new QMap<QString, MctChangeset*>();

    QMap<QDateTime, MctChangeset*>*  changesetNodes = graph->getChangesetNodes();
    QVector<QDateTime> *dates = graph->getDates();

    QStandardItemModel *model = new QStandardItemModel(this);
    model->setColumnCount(1);
    model->setRowCount(dates->size());
    model->setHeaderData(0, Qt::Horizontal, (isRedo)?("Disabled Revisions"):("Applied Revisions"));

    int i = 0;
    int j = dates->size() - 1;

    foreach (QDateTime date, *dates) {
        // item initialization
        MctChangeset* changesetNode = changesetNodes->value(date);
        QString nodestring = createNodeString(changesetNode);
        QStandardItem *item = new QStandardItem(nodestring);
        QStandardItem *comment = new QStandardItem("By: " + changesetNode->getAuthor()->getName() + " : " +changesetNode->getComment());
        item->setChild(0,comment);
        // tooltip to make debugging easier
        QList<MctChange*>* changes = changesetNode->getChanges();
        QListIterator<MctChange*> it(*changes);
        QString listOfChanges = "Changes:";
        while (it.hasNext()) {
            listOfChanges += "\n" + MctStaticData::CHANGE_AS_STRING.value(it.next()->getChangeType());
        }
        item->setToolTip(listOfChanges);
        // view setup (ascending order for redo revs)
        model->setItem((isRedo)?(j-i):(i),0, item);
        i++;
        changesetNodesMap->insert(nodestring, changesetNode);
    }

    view->setModel(model);
    return changesetNodesMap;
}

QString MctRevisionManager::createNodeString(MctChangeset *changesetNode)
{
    QDateTime date = changesetNode->getDate();
    QString datestr = date.toString("yyyy.M.d. (H:m:s)");
    return datestr;
    //return datestr + " by: " + changesetNode->getAuthor()->getName();
}

void MctRevisionManager::on_undoButton_clicked()
{
    QList<MctChangeset*> selection;
    QModelIndexList list = ui->revTree->selectionModel()->selectedIndexes();
    int row = -1;
    foreach (QModelIndex index, list)
    {
        if (index.row()!=row && index.column()==0)
        {
            qDebug() << index.data().toString();
            row = index.row();
            selection.append(undochangesetNodes->value(index.data().toString()));
        }
    }
    this->undoRevs(selection);
    emit normalizeBuffer();
    MctStaticData::instance()->clearChanges();
    qDebug() << "Change size: " << MctStaticData::instance()->getChanges()->size();
}

void MctRevisionManager::undoRevs(QList<MctChangeset*> selection)
{
    QList<MctChangeset*>* changesetNodes = new QList<MctChangeset*>;
    QList<ulong>* childListAll = new QList<ulong>;
    QString additionalChangesets = this->getUndoChildren(selection, changesetNodes, childListAll);
    qCritical() << "Something is missing..." <<additionalChangesets;

    qCritical() << "UndoChanges before undo";
    this->printChanges(MctStaticData::UNDOCHANGES);
    qCritical() << "RedoChanges before undo";
    this->printChanges(MctStaticData::REDOCHANGES);

    if (additionalChangesets != ""){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this, tr("Warning"), tr("The following dependent changesets need to be merged in addition: \n") + additionalChangesets, QMessageBox::Ok|QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel){
            return;
        }
    }

    qDebug() << "Undo Changesets: " << changesetNodes->size();

    auto it = changesetNodes->end();
    while (it != changesetNodes->begin()){
        it--;
        MctChangeset *changesetNode = new MctChangeset(*(*it));

        MctStaticData::instance()->getUndoGraph()->removeChangeset(*it);
        MctStaticData::instance()->getUndoGraph()->correctChangesetNodeListWithDate(changesetNode, MctAbstractGraph::DATE_LATER, false);
        changesetNode->printChangeset();

        undoop->undoChangeset(changesetNode, false);

        MctStaticData::instance()->getRedoGraph()->correctChangesetNode(changesetNode, MctAbstractGraph::DATE_EARLIER, true);
        MctStaticData::instance()->getRedoGraph()->addchangesetFromUndo(changesetNode);

        qCritical() << "Changes after undo";
        this->printChanges(MctStaticData::UNDOCHANGES);
        qCritical() << "RedoChanges after undo";
        this->printChanges(MctStaticData::REDOCHANGES);

        delete changesetNode;
    }

    qDebug() << "Exporting graphs and saving document";

    delete changesetNodes;
    changesetNodes = NULL;
    delete childListAll;
    childListAll = NULL;

    MctStaticData::instance()->getKoDocument()->save();
    MctStaticData::instance()->exportGraphs();

    undochangesetNodes = fillRevTree(MctStaticData::instance()->getUndoGraph(), ui->revTree, false);
    redochangesetNodes = fillRevTree(MctStaticData::instance()->getRedoGraph(), ui->undidRevTree, true);

    emit adjustListOfRevisions();
    adjustButtonAccess();
}

QString MctRevisionManager::getUndoChildren(QList<MctChangeset*> selection, QList<MctChangeset*>* changesetNodes, QList<ulong>* childListAll)
{
    QString additionalChangeset = "";

    int idx = selection.size() - 1;

    while (idx >= 0){
        MctChangeset* treeNode = selection.at(idx);
        idx--;
        QString nodeString = createNodeString(treeNode);
        MctChangeset* changesetNode = undochangesetNodes->value(nodeString);
        if (changesetNode == nullptr)
            continue;

        changesetNodes->append(changesetNode);
        childListAll->append(changesetNode->getId());
    }

    if (changesetNodes->size() == 0)
        return additionalChangeset;

    for (int i = 0; i < changesetNodes->size(); i++){
        QList<ulong> childlist;
        this->checkChildren(changesetNodes->at(i), &childlist, MctStaticData::instance()->getUndoGraph());

        foreach (ulong id, childlist){
            if (!childListAll->contains(id)){
                childListAll->append(id);
                QDateTime date = MctStaticData::instance()->getUndoGraph()->getDateFromId(id);
                MctChangeset* changesetNodeTemp = MctStaticData::instance()->getUndoGraph()->getChangeset(date);
                if (changesetNodeTemp == nullptr){
                    qDebug() << "Child is already undone.";
                    continue;
                }
                QString tempString = this->createNodeString(changesetNodeTemp);
                additionalChangeset += tempString + "\n";
                changesetNodes->append(changesetNodeTemp);
            }
        }
    }

    return additionalChangeset;
}

void MctRevisionManager::checkChildren(MctChangeset *changesetNode, QList<ulong> *childlist, MctAbstractGraph* graph)
{
    QList<ulong> *childs = changesetNode->getChilds();

    if (childs->size() == 0)
        return;

    for (int i = 0; i < childs->size(); i++){
        ulong id = childs->at(i);
        if (!childlist->contains(id))
            childlist->append(id);
    }

    for (int i = 0; i < childs->size(); i++){
        ulong id = childs->at(i);
        QDateTime date = graph->getDateFromId(id);
        MctChangeset *changesetNodeChild = graph->getChangeset(date);

        if (changesetNodeChild == nullptr){
            qDebug() << "changesetNode_child is None with Id:" << id;
            continue;
        }

        this->checkChildren(changesetNodeChild, childlist, graph);
    }
}

void MctRevisionManager::on_redoButton_clicked()
{
    QList<MctChangeset*> selection;
    QModelIndexList list = ui->undidRevTree->selectionModel()->selectedIndexes();
    int row = -1;
    foreach (QModelIndex index, list)
    {
        if (index.row()!=row && index.column()==0)
        {
            qDebug() << index.data().toString();
            row = index.row();
            selection.append(redochangesetNodes->value(index.data().toString()));
        }
    }
    this->redoRevs(selection);
    emit normalizeBuffer();
    MctStaticData::instance()->clearChanges();
    qDebug() << "Change size: " << MctStaticData::instance()->getChanges()->size();
}

void MctRevisionManager::redoRevs(QList<MctChangeset *> selection)
{
    QList<MctChangeset*>* changesetNodes = new QList<MctChangeset*>;
    QList<ulong>* childListAll = new QList<ulong>;
    QString additionalChangesets = this->getRedoChildren(selection, changesetNodes, childListAll);

    qCritical() << "Changes before redo";
    this->printChanges(MctStaticData::UNDOCHANGES);
    qCritical() << "RedoChanges before undo";
    this->printChanges(MctStaticData::REDOCHANGES);

    if (additionalChangesets != ""){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this, tr("Warning"), tr("The following dependent changesets need to be redid in addition: \n") + additionalChangesets, QMessageBox::Ok|QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel){
            return;
        }
    }

    qDebug() << "Redo Changesets: " << changesetNodes->size();

    auto it = changesetNodes->end();
    while (it != changesetNodes->begin()){
        it--;
        MctChangeset *changesetNode = new MctChangeset(*(*it));

        MctStaticData::instance()->getRedoGraph()->removeChangeset(*it);
        MctStaticData::instance()->getRedoGraph()->correctChangesetNode(changesetNode, MctAbstractGraph::DATE_EARLIER, false);

        MctStaticData::instance()->getUndoGraph()->correctChangesetNodeListWithDate(changesetNode, MctAbstractGraph::DATE_LATER, true);
        changesetNode->printChangeset();
        redoop->redoChangeset(changesetNode, false);

        MctStaticData::instance()->getUndoGraph()->addchangesetFromRedo(changesetNode);

        qDebug() << "Changeset " << changesetNode->getDateInString() << " redid";

        qCritical() << "Changes after redo";
        this->printChanges(MctStaticData::UNDOCHANGES);
        qCritical() << "RedoChanges after undo";
        this->printChanges(MctStaticData::REDOCHANGES);

        delete changesetNode;
    }

    qDebug() << "Exporting graphs and saving document";

    delete changesetNodes;
    changesetNodes = NULL;
    delete childListAll;
    childListAll = NULL;

    MctStaticData::instance()->getKoDocument()->save();
    MctStaticData::instance()->exportGraphs();

    undochangesetNodes = fillRevTree(MctStaticData::instance()->getUndoGraph(), ui->revTree, false);
    redochangesetNodes = fillRevTree(MctStaticData::instance()->getRedoGraph(), ui->undidRevTree, true);

    emit adjustListOfRevisions();
    adjustButtonAccess();
}

QString MctRevisionManager::getRedoChildren(QList<MctChangeset *> selection, QList<MctChangeset *> *changesetNodes, QList<ulong> *childListAll)
{
    QString additionalChangeset = "";

    int idx = selection.size() - 1;

    while (idx >= 0){
        MctChangeset* treeNode = selection.at(idx);
        idx--;
        QString nodeString = createNodeString(treeNode);
        MctChangeset* changesetNode = redochangesetNodes->value(nodeString);
        if (changesetNode == nullptr)
            continue;

        changesetNodes->append(changesetNode);
        childListAll->append(changesetNode->getId());
    }

    if (changesetNodes->size() == 0)
        return additionalChangeset;

    for (int i = 0; i < changesetNodes->size(); i++){
        QList<ulong> childlist;
        this->checkChildren(changesetNodes->at(i), &childlist, MctStaticData::instance()->getRedoGraph());

        foreach (ulong id, childlist){
            if (!childListAll->contains(id)){
                childListAll->append(id);
                QDateTime date = MctStaticData::instance()->getRedoGraph()->getDateFromId(id);
                MctChangeset* changesetNodeTemp = MctStaticData::instance()->getRedoGraph()->getChangeset(date);
                if (changesetNodeTemp == nullptr){
                    qDebug() << "Child is already redone.";
                    continue;
                }
                QString tempString = this->createNodeString(changesetNodeTemp);
                additionalChangeset += tempString + "\n";
                changesetNodes->append(changesetNodeTemp);
            }
        }
    }

    return additionalChangeset;
}

void MctRevisionManager::on_deleteButton_clicked()
{
    QList<MctChangeset*> selectionUndo;
    QModelIndexList list = ui->revTree->selectionModel()->selectedIndexes();

    int row = -1;
    foreach (QModelIndex index, list)
    {
        if (index.row()!=row && index.column()==0)
        {
            qDebug() << index.data().toString();
            row = index.row();
            selectionUndo.append(undochangesetNodes->value(index.data().toString()));
        }
    }

    QList<MctChangeset*> selectionRedo;
    list = ui->undidRevTree->selectionModel()->selectedIndexes();
    row = -1;
    foreach (QModelIndex index, list)
    {
        if (index.row()!=row && index.column()==0)
        {
            qDebug() << index.data().toString();
            row = index.row();
            selectionRedo.append(redochangesetNodes->value(index.data().toString()));
        }
    }

    if (selectionUndo.size() != 0){
        this->delUndoRev(selectionUndo);
    }
    if (selectionRedo.size() != 0){
        this->delRedoRev(selectionRedo);
    }

    qDebug() << "Exporting graphs and saving document";

    MctStaticData::instance()->getKoDocument()->save();
    MctStaticData::instance()->exportGraphs();

    undochangesetNodes = fillRevTree(MctStaticData::instance()->getUndoGraph(), ui->revTree, false);
    redochangesetNodes = fillRevTree(MctStaticData::instance()->getRedoGraph(), ui->undidRevTree, true);

    emit adjustListOfRevisions();
    MctStaticData::instance()->clearChanges();
    MctStaticData::instance()->getUndoGraph()->sortDates();

    adjustButtonAccess();
}

void MctRevisionManager::delUndoRev(QList<MctChangeset *> selection)
{
    QList<MctChangeset*>* changesetNodes = new QList<MctChangeset*>;
    QList<ulong>* childListAll = new QList<ulong>;
    QString additionalChangesets = this->getUndoChildren(selection, changesetNodes, childListAll);

    if (changesetNodes->size() == 0)
        return;

    if (changesetNodes->size() > selection.size()){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this, tr("Warning"), tr("TThe following dependent changesets need to be deleted in addition from the undorevision list: \n") + additionalChangesets, QMessageBox::Ok|QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel){
            return;
        }
    }

    qDebug() << "Delete undo Changesets: " << changesetNodes->size();

    auto it = changesetNodes->end();
    while (it != changesetNodes->begin()){
        it--;
        MctChangeset *changesetNode = *it;

        changesetNode->printChangeset();
        MctStaticData::instance()->getUndoGraph()->correctChangesetNodeListWithDate(changesetNode, MctAbstractGraph::DATE_EARLIER, false);
        MctStaticData::instance()->getUndoGraph()->removeChangeset(changesetNode);
    }

    delete changesetNodes;
    changesetNodes = NULL;
}

void MctRevisionManager::delRedoRev(QList<MctChangeset *> selection)
{
    QList<MctChangeset*>* changesetNodes = new QList<MctChangeset*>;
    QList<ulong>* childListAll = new QList<ulong>;
    QString additionalChangesets = this->getRedoChildren(selection, changesetNodes, childListAll);

    if (changesetNodes->size() == 0)
        return;

    if (additionalChangesets != ""){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this, tr("Warning"), tr("The following dependent changesets need to be deleted in addition from the undorevision list: \n") + additionalChangesets, QMessageBox::Ok|QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel){
            return;
        }
    }

    qDebug() << "Delete redo Changesets: " << changesetNodes->size();

    //reverse itaration in order to remove the childs first
    auto it = changesetNodes->end();
    while (it != changesetNodes->begin()){
        it--;
        MctChangeset *changesetNode = *it;

        changesetNode->printChangeset();
        MctStaticData::instance()->getRedoGraph()->correctChangesetNodeListWithDate(changesetNode, MctAbstractGraph::DATE_LATER, false);
        MctStaticData::instance()->getRedoGraph()->removeChangeset(changesetNode);
    }

    delete changesetNodes;
    changesetNodes = NULL;
}

void MctRevisionManager::printChanges(QString type)
{
    QMap<QDateTime, MctChangeset*>* changes = nullptr;
    if (type == MctStaticData::UNDOCHANGES)
        changes = MctStaticData::instance()->getUndoGraph()->getChangesetNodes();
    else
        changes = MctStaticData::instance()->getRedoGraph()->getChangesetNodes();

    for (auto it = changes->begin(); it != changes->end(); ++it){
        QList<MctChange*>* cs = it.value()->getChanges();
        qCritical() << "nextChange ---------------";
        qCritical() << "date: " << it.value()->getDateInString();
        foreach (MctChange *change, *cs){
            qCritical() << "start: "<< change->getPosition()->getStartPar() << "/" << change->getPosition()->getStartChar() <<
                           "end: " << change->getPosition()->getEndPar() << "/" << change->getPosition()->getEndChar() <<
                           ", type: " << change->getChangeType();
        }

    }
}

void MctRevisionManager::adjustButtonAccess()
{
    if (redochangesetNodes)
        ui->redoButton->setEnabled(redochangesetNodes->count());

    if (undochangesetNodes)
        ui->undoButton->setEnabled(undochangesetNodes->count());
}

void MctRevisionManager::on_clearButton_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("The entire revision history will be ereased!");
    msgBox.setInformativeText("Are you sure?");
    msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::No);
    int reply = msgBox.exec();
    switch (reply) {
    case QMessageBox::Yes:
        emit clearingProcess();
        break;
    default:
        return;
    }

    delete undochangesetNodes;
    undochangesetNodes = NULL;
    delete redochangesetNodes;
    redochangesetNodes = NULL;
    QStandardItemModel *model = dynamic_cast<QStandardItemModel*>(ui->revTree->model());
    model->clear();
    model->setColumnCount(1);
    model->setRowCount(0);
    model->setHeaderData(0, Qt::Horizontal, "Applied Revisions");
    ui->revTree->setModel(model);

    model = dynamic_cast<QStandardItemModel*>(ui->undidRevTree->model());
    model->clear();
    model->setColumnCount(1);
    model->setRowCount(0);
    model->setHeaderData(0, Qt::Horizontal, "Disabled Revisions");
    ui->undidRevTree->setModel(model);


}

void MctRevisionManager::on_revTree_expanded(const QModelIndex &index)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->revTree->model());
    QStandardItem *item = model->itemFromIndex(index);
    QPoint p = QCursor::pos();
    p.setX(p.x()+20);
    p.setY(p.y()+20);
    QToolTip::showText(p, item->toolTip());
}

void MctRevisionManager::on_undidRevTree_expanded(const QModelIndex &index)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->undidRevTree->model());
    QStandardItem *item = model->itemFromIndex(index);
    QPoint p = QCursor::pos();
    p.setX(p.x()+20);
    p.setY(p.y()+20);
    QToolTip::showText(p, item->toolTip());
}
