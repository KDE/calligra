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

#include "mctmergemanagerdialog.h"
#include "ui_mctmergemanagerdialog.h"

#include <QtCore/qmath.h>
#include <QFileDialog>
#include <QDesktopWidget>
#include <MctStaticData.h>
#include <MctChangeset.h>
#include <MctChange.h>
#include <MctPosition.h>
#include <QMessageBox>
#include <KoTextDocument.h>
#include <KoDocument.h>
#include "MctRevisionManager.h"

MctMergeManagerDialog::MctMergeManagerDialog(QWidget *parent, MctUndoClass *undoop, MctRedoClass *redoop) :
    QDialog(parent),
    ui(new Ui::MctMergeManagerDialog),
    undoGraphOpened(nullptr),
    redoGraphOpened(nullptr),
    undoop(undoop),
    redoop(redoop),
    doc2merge(nullptr)
{
    ui->setupUi(this);

    loadRevisionTrees(ui->treeUndoCurrent, ui->treeRedoCurrent);
}

MctMergeManagerDialog::~MctMergeManagerDialog()
{
    delete ui;
    delete undoGraphOpened;
    delete redoGraphOpened;
}

void MctMergeManagerDialog::on_mergeRevButton_clicked()
{
    if (ui->tabOpened->currentIndex() == 0){
        qCritical() << "UNDO";
        mergingGraphs(ChangeType::UNDOCHANGE, ui->treeUndoOpened, ui->treeRedoOpened, undoGraphOpened, redoGraphOpened);
    } else {
        qCritical() << "REDO";
        mergingGraphs(ChangeType::REDOCHANGE, ui->treeUndoOpened, ui->treeRedoOpened, undoGraphOpened, redoGraphOpened);
    }
    QMessageBox::StandardButton reply;
    reply = QMessageBox::information(this, tr("Merging finished"), tr("Merging succesfully finished."), QMessageBox::Ok);
}

void MctMergeManagerDialog::on_browseButton_clicked()
{
    char * s=getenv("EXTERNAL_STORAGE");
    QString path = QString(s);

    QDesktopWidget w;
    QRect r = w.screenGeometry();

    QFileDialog *fdia = new QFileDialog(this, "Open", path, "Odt files (*.odt)");
    fdia->setFileMode(QFileDialog::ExistingFile);
    fdia->setGeometry(r);
    int result = fdia->exec();
    if(result == QDialog::Accepted) {
        QString filename = fdia->selectedFiles().first();
        fdia->close();
        delete fdia;
        ui->filePathText->setText(filename);
        this->show();
        this->loadGraph();
    } else {
        fdia->close();
        delete fdia;
        this->show();
    }
}

void MctMergeManagerDialog::on_tabCurrent_currentChanged(int index)
{
    ui->tabOpened->setCurrentIndex(index);
}

void MctMergeManagerDialog::on_tabOpened_currentChanged(int index)
{
    ui->tabCurrent->setCurrentIndex(index);
    if (index == 0) {
        checkMergeButtonState(*ui->treeUndoOpened, *ui->treeUndoCurrent);
    } else {
        checkMergeButtonState(*ui->treeRedoOpened, *ui->treeRedoCurrent);
    }
}

void MctMergeManagerDialog::loadTreesOfOpened()
{
    loadRevisionTrees(ui->treeUndoOpened, ui->treeRedoOpened);
}

void MctMergeManagerDialog::loadGraph()
{
    doc2merge = new KoTextDocument(new QTextDocument);
    ui->treeUndoOpened->clear();
    ui->treeRedoOpened->clear();

    undoGraphOpened = new MctUndoGraph(ui->filePathText->text(), doc2merge);
    if(undoGraphOpened->getChangesetNodes()->size() > 0)
        addRevision2Tree(ui->treeUndoOpened, undoGraphOpened->getChangesetNodes()->begin(), 0);

    redoGraphOpened = new MctRedoGraph(ui->filePathText->text(), doc2merge);
    if(redoGraphOpened->getChangesetNodes()->size() > 0)
        addRevision2Tree(ui->treeRedoOpened, redoGraphOpened->getChangesetNodes()->begin(), 0);

    QString undoOpenedCheckNum = undoGraphOpened->getRoot().attributeNode(MctStaticData::STARTINGSTRING).value();
    QString redoOpenedCheckNum = redoGraphOpened->getRoot().attributeNode(MctStaticData::STARTINGSTRING).value();

    QString undoCurrentCheckNum = MctStaticData::instance()->getUndoGraph()->getRoot().attributeNode(MctStaticData::STARTINGSTRING).value();
    QString redoCurrentCheckNum = MctStaticData::instance()->getRedoGraph()->getRoot().attributeNode(MctStaticData::STARTINGSTRING).value();

    if ((undoCurrentCheckNum != undoOpenedCheckNum) || (redoOpenedCheckNum != redoCurrentCheckNum)){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this, tr("Warning"), tr("The selected file is not from the same root. \n Would you like to continue?"), QMessageBox::No|QMessageBox::Yes);
        if (reply != QMessageBox::Yes){
            undoGraphOpened = nullptr;
            redoGraphOpened = nullptr;
            ui->mergeRevButton->setEnabled(false);
            return;
        }
    }
    loadRevisionTrees(ui->treeUndoOpened, ui->treeRedoOpened, undoGraphOpened->getChangesetNodes(), redoGraphOpened->getChangesetNodes());
    //ui->mergeRevButton->setEnabled(true);
}

void MctMergeManagerDialog::mergingGraphs(ChangeType type, QTreeWidget *undoTreeWidget, QTreeWidget *redoTreeWidget, MctUndoGraph *undoGraph, MctRedoGraph *redoGraph)
{
    qDebug() << type;
    QTreeWidget *revs = (type == UNDOCHANGE ? undoTreeWidget : redoTreeWidget);
    MctAbstractGraph *graph = (type == UNDOCHANGE ? static_cast<MctAbstractGraph*>(MctStaticData::instance()->getUndoGraph()) : static_cast<MctAbstractGraph*>(MctStaticData::instance()->getRedoGraph()));
    MctAbstractGraph *graph2merge = (type == UNDOCHANGE ? static_cast<MctAbstractGraph*>(undoGraphOpened) : static_cast<MctAbstractGraph*>(redoGraphOpened));

    qDebug() << "Merging graphs started";

    QList<QTreeWidgetItem*> selection = revs->selectedItems();
    int selectionCount = selection.size();

    QList<MctChangeset*> *changesetNodes = new QList<MctChangeset*>();
    QList<ulong> *childListAll = new QList<ulong>();

    QString additionalChangesets = "";
    if (type == UNDOCHANGE){
        additionalChangesets = getParents(selection, changesetNodes, childListAll, type, undoGraph);
    } else {
        additionalChangesets = getChildren(selection, changesetNodes, childListAll, type, redoGraph);
    }

    qDebug() << "selectionCount: " << selectionCount;

    this->checkChangesetNodesInGraph(changesetNodes, type);

    qDebug() << "parent/child changesetnodes found";

    if (changesetNodes->size() == 0)
        return;

    if (additionalChangesets != ""){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this, tr("Warning"), tr("The following dependent changesets need to be merged in addition: \n") + additionalChangesets, QMessageBox::Ok|QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel){
            return;
        }
    }

    qDebug() << "merging graphs in process: parents obtained";
    qDebug() << "loading document to merge";
    //FIXME
    //doc2merge = MctStaticData::instance()->getKoDocument()->loadFileInMemory(ui->filePathText->text());

    if (type != UNDOCHANGE){
        if (graph2merge == nullptr){
            qDebug() << "graph2merge is None when attempting to merge!";
        } else {
            QVector<QDateTime>* excludeDates = graph2merge->getDates();
            QVector<QDateTime>* dates2CorrectWith = MctStaticData::instance()->getUndoGraph()->getDates(excludeDates);
            QList<QDateTime> dates2CorrectWithList = dates2CorrectWith->toList();
            QList<MctChangeset*>* changesetNodes2CorrectWith = MctStaticData::instance()->getUndoGraph()->getChangesetListByDates(&dates2CorrectWithList);
            qDebug() << "Changesets to use to correct redo changesets:";
            foreach (MctChangeset *ch, *changesetNodes2CorrectWith){
                ch->printChangeset();
            }
        }
    } 

    //TODO order changesetNodes by date

    for (auto it = changesetNodes->begin(); it != changesetNodes->end(); it++){
        MctChangeset* changesetNode = *it;
        qCritical() << "ChangesetNode date: " << changesetNode->getDateInString();
        if (type == UNDOCHANGE){
            QMap<ulong, MctChange*> redoChangeNodes;
            QMap<ulong, MctPosition*> origPositions;
            QList<MctChange*>* changeNodes = changesetNode->getChanges();
            //loaded changeNode ids have no value
            ulong changeNodeCount = 0;
            auto changeIt = changeNodes->end();
            do {
                changeIt--;
                MctChange* changeNode = *changeIt;
                changeNode->setId(changeNodeCount++);
                MctChange* redoChangeNode = MctStaticData::instance()->createRedoChangeNode(doc2merge, changeNode);
                redoChangeNodes[changeNode->getId()] = redoChangeNode;
                origPositions[changeNode->getId()] = changeNode->getPosition();
                redoop->createcursor(changeNode, changeNode->getPosition());
                graph->correctChangesetNode(changesetNode, MctAbstractGraph::DATE_LATER, true);
                redoop->createcursor(changeNode, changeNode->getPosition());
            } while (changeIt != changeNodes->begin());

            changeIt = changeNodes->end();
            do {
                changeIt--;
                MctChange* changeNode = *changeIt;
                MctChange* redoChangeNode = redoChangeNodes[changeNode->getId()];
                if (redoChangeNode != nullptr){
                    redoChangeNode->setPosition(changeNode->getPosition());
                    redoop->redoChange(redoChangeNode);
                    changeNode->setPosition(origPositions[changeNode->getId()]);
                }
            } while (changeIt != changeNodes->begin());

            graph->correctChangesetNodeListWithDate(changesetNode, MctAbstractGraph::DATE_LATER, true);
        } else {
            QVector<QDateTime>* excludeDates = graph2merge->getDates();
            QVector<QDateTime>* dates2CorrectWith = MctStaticData::instance()->getUndoGraph()->getDates(excludeDates);
            QList<QDateTime> dates2CorrectWithList = dates2CorrectWith->toList();
            QList<MctChangeset*>* changesetNodes2CorrectWith = MctStaticData::instance()->getUndoGraph()->getChangesetListByDates(&dates2CorrectWithList);
            //TODO changesetNodes2correctWith value?
            MctStaticData::instance()->getUndoGraph()->correctChangesetNode2(changesetNode, changesetNodes2CorrectWith, true);
        }

        qDebug() << "Changeset to be merge:";
        changesetNode->printChangeset();
        graph->addChangeset(changesetNode->getChanges(), changesetNode->getAuthor(), changesetNode->getDate(), changesetNode->getComment(), 0, true);
    }

    if (type == UNDOCHANGE){
        loadRevisionTrees(ui->treeUndoCurrent, ui->treeRedoCurrent);
    } else {
        loadRevisionTrees(ui->treeUndoCurrent, ui->treeRedoCurrent);
    }

    emit adjustListOfRevisions();

    delete changesetNodes;
    changesetNodes = nullptr;
    delete childListAll;
    childListAll = nullptr;

    MctStaticData::instance()->getKoDocument()->save();
    MctStaticData::instance()->exportGraphs();
    emit normalizeBuffer();
    MctStaticData::instance()->clearChanges();
}

void MctMergeManagerDialog::checkChangesetNodesInGraph(QList<MctChangeset *> *changesetNodes, ChangeType type)
{
    QString additionalChangesets = "";
    if (type == UNDOCHANGE){
        additionalChangesets = "The following changesets needs to be redone in the document: \n";
    } else {
        additionalChangesets = "The following changesets needs to be undone in the document: \n";
    }

    QList<MctChangeset*> changesetNodes2done;

    for (auto it = changesetNodes->begin(); it != changesetNodes->end(); ++it) {
        QDateTime date = (*it)->getDate();

        MctChangeset *changesetNodeInGraph = nullptr;
        if (type == UNDOCHANGE){
            changesetNodeInGraph = MctStaticData::instance()->getRedoGraph()->getChangeset(date);
        } else {
            changesetNodeInGraph = MctStaticData::instance()->getUndoGraph()->getChangeset(date);
        }

        if (changesetNodeInGraph == nullptr)
            continue;
        else {
            changesetNodes->removeOne(*it);
            changesetNodes2done.append(*it);
            QString tempString = this->createNodeString(*it);
            additionalChangesets += tempString + "\n";
        }
    }

    if (changesetNodes2done.size() == 0)
        return;
    else {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this, tr("Warning"), additionalChangesets, QMessageBox::Ok|QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel){
            return;
        }
    }

    MctRevisionManager revManagerDialog;
    revManagerDialog.show();

    QList<QString> displayValues;

    for (auto changesetNode : changesetNodes2done){
        displayValues.append(createNodeString(changesetNode));
    }

    /*QList<QDomNode> selection;

    if (type == UNDOCHANGE){
        selection = getTreeNodes(displayValues, redoGraphOpened);
        revManagerDialog.ui->treeRedoRevisions = selection;
    } else {
        selection = getTreeNodes(displayValues, undoGraphOpened);
        revManagerDialog.ui->treeUndoRevisions = selection;
    }*/

    if (changesetNodes->length() == 0)
        loadRevisionTrees(ui->treeUndoCurrent, ui->treeRedoCurrent);

}

void MctMergeManagerDialog::checkMergeButtonState(const QTreeWidget& treeOpenedWid, const QTreeWidget& treeCurrentWid)
{
    if (treeOpenedWid.selectedItems().size() > 0 &&
        treeCurrentWid.selectedItems().size() == 1){
        ui->mergeRevButton->setEnabled(true);
    } else {
        ui->mergeRevButton->setEnabled(false);
    }
}

void MctMergeManagerDialog::on_treeUndoOpened_itemSelectionChanged()
{
    checkMergeButtonState(*ui->treeUndoOpened, *ui->treeUndoCurrent);
}

void MctMergeManagerDialog::on_treeRedoOpened_itemSelectionChanged()
{
    checkMergeButtonState(*ui->treeRedoOpened, *ui->treeRedoCurrent);
}
