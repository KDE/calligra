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

#include "MctMain.h"
#include "MctStaticData.h"
#include "MctUndoClass.h"
#include "MctRedoClass.h"
#include "MctUndoGraph.h"
#include "MctRedoGraph.h"
#include "MctPosition.h"
#include "MctAuthor.h"
#include "MctTableProperties.h"
#include <MctEmbObjProperties.h>

#include "KWDocument.h"
#include <frames/KWTextFrameSet.h>

#include "KoTextDocument.h"
#include "KoTextEditor.h"

#include <QMessageBox>
#include <QTextTable>

MctMain::MctMain(KWDocument *document, QString fileUrl)
    : m_doc(document),
      m_fileURL(fileUrl)
{
    m_koTextDoc = new KoTextDocument(document->mainFrameSet()->document());
    m_editor = m_koTextDoc->textEditor();

    MctStaticData::instance()->setKoDocument(m_doc);
    MctStaticData::instance()->setFileURL(m_fileURL);
    MctStaticData::instance()->setMctState(true);

    //class managing undo operations
    m_undoop = new MctUndoClass(m_koTextDoc);
    //class managing redo operations
    m_redoop = new MctRedoClass(m_koTextDoc);

    m_changebuffer.clear();

    //loading undo graph
    if(MctStaticData::instance()->getUndoGraph() == NULL) {
        qDebug() << "loading undo graph";
        MctStaticData::instance()->setUndoGraph(new MctUndoGraph(m_fileURL,m_koTextDoc));
    }
    //loading redo graph
    if(MctStaticData::instance()->getRedoGraph() == NULL) {
        qDebug() << "loading redo graph";
        MctStaticData::instance()->setRedoGraph(new MctRedoGraph(m_fileURL,m_koTextDoc));
    }
}

MctMain::~MctMain()
{
    MctStaticData::instance()->setRedoGraph(NULL);
    MctStaticData::instance()->setUndoGraph(NULL);
    MctStaticData::instance()->clearChanges();
    MctStaticData::instance()->setMctState(false);
}

MctPosition* MctMain::createPositionInTable(const QTextCursor &cursor)
{
    QTextBlock startBlock = cursor.document()->findBlock(cursor.anchor());
    QTextBlock endBlock = cursor.document()->findBlock(cursor.position());
    int selectionBegin = qMin(cursor.anchor(), cursor.position());
    int selectionEnd = qMax(cursor.anchor(), cursor.position());
    QTextTable *table = cursor.currentTable();
    if(!table) {
        qCritical() << "Cursor position is not inside a table!";
        return NULL;
    }
    QTextTableCell firstCell = table->cellAt(0,0);
    QTextTableCell cell = table->cellAt(cursor);
    MctCell *c_start = NULL;
    MctCell *c_end = NULL;
    int block1 = firstCell.firstCursorPosition().blockNumber();
    int block2 = cell.firstCursorPosition().blockNumber();
    int s = selectionBegin-startBlock.position();
    int e = selectionEnd-endBlock.position();
    //int p1 = cursor.blockNumber() - block2;
    int p1 = startBlock.blockNumber() - block2;

    int p2 = endBlock.blockNumber() - block2;
    c_start = new MctCell(cell.row(), cell.column());
    c_end = new MctCell(cell.row(), cell.column());

    MctPosition* pos = new MctPosition(p1, s, p2, e);                   //paragraph, character
    MctPosition* posParent = NULL;

    // Check if we are in a table inside a table and in that case corrigate the block positions
    QTextCursor tmpCursor(cursor);
    tmpCursor.setPosition(firstCell.firstCursorPosition().position());
    tmpCursor.movePosition(QTextCursor::PreviousBlock);
    table = tmpCursor.currentTable();
    int correction = 0;
    if(table) {
        int blockOfParentCell = table->cellAt(tmpCursor).firstCursorPosition().blockNumber();
        int firstcursorpos = table->cellAt(tmpCursor).firstCursorPosition().position();        
        // If firstcursorpos + 1 == tmpcursor, than before the table just an empyt block stays in the cell
        // Else: there is something in the previous block and need to count as a normal block
        int diff = firstcursorpos + 1 == tmpCursor.position() ? 1 : 0;
        correction = blockOfParentCell + diff;
    }

    posParent = new MctPosition(block1 - 1 - correction, 0, block1 - 1 - correction, 0, c_start, c_end);    //start block of the table, , cellinfo(row,col)

    posParent->setAnchored(pos);
    pos = posParent;
    while(table) {
        firstCell = table->cellAt(0,0);
        cell = table->cellAt(tmpCursor);
        block1 = firstCell.firstCursorPosition().blockNumber();
        block2 = cell.firstCursorPosition().blockNumber();
        c_start = new MctCell(cell.row(), cell.column());
        c_end = new MctCell(cell.row(), cell.column());

        tmpCursor.setPosition(firstCell.firstCursorPosition().position());
        tmpCursor.movePosition(QTextCursor::PreviousBlock);
        table = tmpCursor.currentTable();

        correction = 0;
        if(table) {
            int blockOfParentCell = table->cellAt(tmpCursor).firstCursorPosition().blockNumber();
            int firstcursorpos = table->cellAt(tmpCursor).firstCursorPosition().position();
            int diff = firstcursorpos + 1 == tmpCursor.position() ? 1 : 0;
            correction = blockOfParentCell + diff;         
        }
        posParent = new MctPosition(block1 - 1 - correction, 0, block1 - 1 - correction, 0, c_start, c_end);    //start block of the table, , cellinfo(row,col)

        posParent->setAnchored(pos);
        pos = posParent;
    }

    return pos;
}

MctUndoClass *MctMain::undoop()
{
    return m_undoop;
}

MctRedoClass *MctMain::redoop()
{
    return m_redoop;
}

void MctMain::createMctChange(QTextCursor &selection, MctChangeTypes changeType, const KUndo2MagicString title, QTextFormat format, QTextFormat prevFormat)
{
    Q_UNUSED(title);
    QTextBlock startBlock = selection.document()->findBlock(selection.selectionStart());
    QTextBlock endBlock = selection.document()->findBlock(selection.selectionEnd());

    QTextCursor checker = QTextCursor(selection);
    int selectionBegin = qMin(checker.anchor(), checker.position());
    int selectionEnd = qMax(checker.anchor(), checker.position());

    QTextTable *table1 = selection.currentTable();
    QTextList *isList = selection.currentList();

    int blockpos = startBlock.position();
    int endblockpos = endBlock.position();
    int s = selectionBegin-blockpos;
    int e = selectionEnd-blockpos;    
    int p1 = startBlock.blockNumber();
    int p2 = endBlock.blockNumber();    

    MctNode *changeEntity;
    //MctPosition *pos_tmp = NULL;
    MctPosition *pos = NULL;

    switch(changeType) {
    case MctChangeTypes::AddedString:
    {
        if(!table1) {
            changeEntity = new MctStringChange(checker.selectedText());
        } else {
            changeEntity = new MctStringChangeInTable(checker.selectedText());
            changeType = MctChangeTypes::AddedStringInTable;
            pos = createPositionInTable(checker);            
        }
        break;
    }
    case MctChangeTypes::StyleChange:
    {
        if (format == prevFormat) qWarning() << "New and old style format are the same. Unhealty behavior.";

        qDebug() << "current format";
        ChangeEvent::printProperties(format);
        qDebug() << "previous format";
        ChangeEvent::printProperties(prevFormat);

        prevFormat = ChangeEvent::getOldValuesForFormat(format, prevFormat);
        ChangeEvent::printProperties(prevFormat);

        const int noChange = 0;
        if (prevFormat.properties().size() == noChange){
            qDebug() << "Skip empty formatting MctChange entry";
            return;
        }

        ChangeEventList *formatChanges = new QList<ChangeEvent*>();
        ChangeEvent * change = new ChangeEvent(prevFormat, format);

        formatChanges->append(change);
        changeEntity = new MctStylePropertyChange(formatChanges);
        break;
    }
    case MctChangeTypes::RemovedString:
    {
        if(!table1) {
            changeEntity = new MctStringChange(checker.selectedText());
        } else {
            changeEntity = new MctStringChangeInTable(checker.selectedText());
            changeType = MctChangeTypes::RemovedStringInTable;
            pos = createPositionInTable(checker);
        }
        break;
    }
    case MctChangeTypes::ParagraphBreak:
    {
        if (table1) {
            changeType = MctChangeTypes::ParagraphBreakInTable;
            changeEntity = new MctParagraphBreakInTable();
            pos = createPositionInTable(checker);
            e = 0;
            /*QTextTableCell firstCell = table1->cellAt(0,0);
            QTextTableCell cell = table1->cellAt(checker);
            MctCell *c_start = NULL;
            MctCell *c_end = NULL;
            int block1 = firstCell.firstCursorPosition().blockNumber();
            int block2 = cell.firstCursorPosition().blockNumber();
            p1 = checker.blockNumber() - block2 ;
            p2 = endBlock.blockNumber() - block2;
            c_start = new MctCell(cell.row(), cell.column());
            c_end = new MctCell(cell.row(), cell.column());

            pos = new MctPosition(p1, s, p2, e, c_start, c_end);        //paragraph, character, cellinfo(row,col)
            pos_tmp = new MctPosition(block1 - 1, 0, block1 - 1, 0);    //start block of the table
            pos->setAnchored(pos_tmp);*/
        } else if (isList){
            // Todo: maybe...
        } else {
            changeEntity = new MctParagraphBreak();
            e = 0;
        }
        break;
    }
    case MctChangeTypes::DelParagraphBreak:
    {
        if(!table1) {
            if(s < 0) {
                int tmp = p1;
                p1 = p2;
                p2 = tmp;
                e = 0;
                s = selectionBegin - endblockpos;
            }
            changeEntity = new MctDelParagraphBreak();
        } else {
            changeType = MctChangeTypes::DelParagraphBreakInTable;
            changeEntity = new MctDelParagraphBreakInTable();
            pos = createPositionInTable(checker);
        }
        break;
    }
    case MctChangeTypes::AddedTextTable:
    {
        qDebug() << "AddedTextTable";
        QTextTable *table = selection.currentTable();
        if (table != nullptr) {
            changeEntity = new MctAddedTextTable("added-table-without-prop", new MctTableProperties("", table->rows(), table->columns()));
            p1--; /// !!! KoTextEditor's table: {hidden block}[table] but Mct's table is a logical table, thus contains the hidden (technical) block right before the table.
            p2 = p1;
            e = s;
        } else {
            qFatal("No table at cursor!");
        }

        break;
    }
    case MctChangeTypes::RemovedTextTable:
    {
        qDebug() << "RemovedTextTable";
        QTextTable *table = selection.currentTable();
        if (table != nullptr) {
            changeEntity = new MctRemovedTextTable("removed-table-without-prop", new MctTableProperties("", table->rows(), table->columns()));
            p1--; /// !!! KoTextEditor's table: {hidden block}[table] but Mct's table is a logical table, thus contains the hidden (technical) block right before the table.
            p2 = p1;
            e = s;
        } else {
            qFatal("No table at cursor!");
        }

        break;
    }
    case MctChangeTypes::RemovedTextTableInTable:
    {
        qDebug() << "RemovedTextTableInTable";
        QTextTable *table = selection.currentTable();
        if (table != nullptr) {
            QTextCursor outerTableCursor = QTextCursor(selection);
            outerTableCursor.setPosition(blockpos - 1);
            pos = createPositionInTable(outerTableCursor);
            pos->startCellInfo()->convertCellPos2CellName();
            changeEntity = new MctRemovedTextTableInTable("removed-table-in-table", new MctTableProperties("", table->rows(), table->columns()), pos->startCellInfo()->cellName(), "", pos->startCellInfo());
            p1--; /// !!! KoTextEditor's table: {hidden block}[table] but Mct's table is a logical table, thus contains the hidden (technical) block right before the table.
        } else {
            qFatal("No table at cursor!");
        }

        break;
    }
    case MctChangeTypes::AddedTextTableInTable:
    {
        qDebug() << "AddedTextTableInTable";
        QTextTable *table = selection.currentTable();           
        if (table != nullptr) {
            QTextCursor outerTableCursor = QTextCursor(selection);
            outerTableCursor.setPosition(blockpos-1);
            pos = createPositionInTable(outerTableCursor);
            pos->startCellInfo()->convertCellPos2CellName();
            changeEntity = new MctAddedTextTableInTable("added-table-in-table", new MctTableProperties("", table->rows(), table->columns()), pos->startCellInfo()->cellName(), "", pos->startCellInfo());
            p1--; /// !!! KoTextEditor's table: {hidden block}[table] but Mct's table is a logical table, thus contains the hidden (technical) block right before the table.
        } else {
            qFatal("No table at cursor!");
        }

        break;
    }
    default:
        qWarning() << "This change type " << changeType << " is not handled yet. Returning now.";
        return;
        break;
    }

    if(pos == NULL) {
        qDebug() << "pos: s=/" << QString::number(p1) << "/" << QString::number(s) << " e=/" << QString::number(p2) << "/" << QString::number(e);
        pos =new MctPosition(p1, s, p2, e);
    }
    QString text = checker.selectedText();
    qDebug() << "text: " << text;

    MctChange *change = new MctChange(pos, changeType, changeEntity);

    if(m_changebuffer.isEmpty()) {
        m_changebuffer.append(change);
    } else {
        MctChange *lastchange = m_changebuffer.last();
        // Is this change about following characters? Merge them to a word into only one MctChange
        if(changeType == MctChangeTypes::AddedString) {
            if(lastchange->position()->endPar() == change->position()->startPar()
               && lastchange->position()->endChar() == change->position()->startChar()
               && changeType == lastchange->changeType()) {
                m_changebuffer.append(change);
            } else {
                normailizeChangebuffer();

                m_changebuffer.clear();
                m_changebuffer.append(change);
            }
        } else if (changeType == MctChangeTypes::RemovedString) {
            if(lastchange->position()->startPar() == change->position()->endPar()
               && lastchange->position()->startChar() == change->position()->endChar()
               && changeType == lastchange->changeType()) {
                m_changebuffer.append(change);
            } else {
                normailizeChangebuffer();

                m_changebuffer.clear();
                m_changebuffer.append(change);
            }

        } else if (changeType == MctChangeTypes::AddedStringInTable) {
            if(changeType == lastchange->changeType()
               && lastchange->position()->endPar() == change->position()->startPar()
               && lastchange->position()->endChar() == change->position()->startChar()
               && posCheckInTable(lastchange->position(), change->position(), changeType)) {
                m_changebuffer.append(change);
            } else {
                normailizeChangebuffer();

                m_changebuffer.clear();
                m_changebuffer.append(change);
            }
        } else if (changeType == MctChangeTypes::RemovedStringInTable) {
            if(changeType == lastchange->changeType()
               && lastchange->position()->startPar() == change->position()->endPar()
               && lastchange->position()->startChar() == change->position()->endChar()
               && posCheckInTable(lastchange->position(), change->position(), changeType)) {
                m_changebuffer.append(change);
            } else {
                normailizeChangebuffer();

                m_changebuffer.clear();
                m_changebuffer.append(change);
            }

        } else {
            normailizeChangebuffer();
            MctStaticData::instance()->getChanges()->append(change);
        }
    }

    //MctStaticData::instance()->getChanges()->append(change);
}

void MctMain::addGraphicMctChange(KoShape &selection, MctChangeTypes changeType, const KUndo2MagicString title, const QString &fileUrl, ChangeAction action)
{
    qDebug() << "selection: " << selection.position();
    MctNode *changeEntity;
    MctPosition *pos = NULL;

    if(pos == NULL) {
        //TODO check position determination possibility
        qDebug() << "pos set: /0/0 /0/0";
        pos = new MctPosition(0, 0, 0, 0);
    }
    MctEmbObjProperties *props = new MctEmbObjProperties(title.toString(), &selection);
    props->setURL(fileUrl);

    if (action == REMOVED){
        changeEntity = new MctRemovedTextGraphicObject("removed-text-graphic-object", props);
    } else if (action == ADDED){
        changeEntity = new MctAddedTextGraphicObject("added-text-graphic-object", props);
    } else if (action == CHANGED){
        changeEntity = new MctAddedTextGraphicObject("changed-text-graphic-object", props);
    }
    MctChange *change = new MctChange(pos, changeType, changeEntity);
    MctStaticData::instance()->getChanges()->append(change);
}

void MctMain::shapeOperationSlot(KoShape *shape, ChangeAction action)
{
    if (action == REMOVED){
        m_doc->removeShape(shape);
    } else if (action == ADDED){
        m_doc->addShape(shape);
    }
}

void MctMain::createShapeMctChange(const QString &type, const QPointF &pos, KoShape &shape, ChangeAction action, QPointF *prevPos)
{
    MctNode *changeEntity;
    MctPosition *position = NULL;
    MctChangeTypes changeType;

    if(position == NULL) {
        //TODO check position determination possibility
        qDebug() << "pos set: /0/0 /0/0";
        position = new MctPosition(0, 0, 0, 0);
    }

    if (action != REMOVED) shape.setFileUrl(type);

    MctEmbObjProperties *props = new MctEmbObjProperties(type, &shape, pos, shape.size());
    MctStaticData::instance()->setAddedShapeType(type);

    if (action == REMOVED){
        KoShape *shape1 = MctStaticData::instance()->getKoDocument()->emitPosition(props->centerPos());
        props->setURL(shape1->getFileUrl());

        changeType = MctChangeTypes::RemovedTextGraphicObject;
        changeEntity = new MctRemovedTextGraphicObject("removed-text-graphic-object", props);
    } else if (action == ADDED) {
        props->setURL(type);
        changeType = MctChangeTypes::AddedTextGraphicObject;
        changeEntity = new MctAddedTextGraphicObject("added-text-graphic-object", props);
    } else if (action == CHANGED){
        qDebug() << "Shape changed";
        props->setURL(type);
        QPointF prev(*prevPos);
        props->setPrevPos(prev);
        changeType = MctChangeTypes::AddedTextGraphicObject;
        changeEntity = new MctAddedTextGraphicObject("changed-text-graphic-object", props);
    }

    MctChange *change = new MctChange(position, changeType, changeEntity);
    MctStaticData::instance()->getChanges()->append(change);
}

void MctMain::createShapePositionChanged(KoShape *selectedShape, const QPointF &point, QPointF *prevPos)
{
    qDebug() << "Shape type: " << selectedShape->getFileUrl() << ", position: " << point;
    KoShape *shape1 = MctStaticData::instance()->getKoDocument()->emitPosition(point);

    if (shape1 && shape1->getFileUrl() == selectedShape->getFileUrl() && selectedShape->getFileUrl() != ""){
        createShapeMctChange(selectedShape->getFileUrl(), selectedShape->position(), *selectedShape, CHANGED, prevPos);
    } else {
        qDebug() << "No position change";
    }
}

void MctMain::createShapeStyleChanged(const QString &type, const QPointF &pos, KoShape &shape, KoShapeStroke *newStroke, KoShapeShadow *newShadow, QPointF *prevPos, const QSizeF &prevSize, double rotation)
{
    MctNode *changeEntity;
    MctPosition *position = NULL;
    MctChangeTypes changeType;

    if(position == NULL) {
        //TODO check position determination possibility
        qDebug() << "pos set: /0/0 /0/0";
        position = new MctPosition(0, 0, 0, 0);
    }

    shape.setFileUrl(type);

    MctEmbObjProperties *props = new MctEmbObjProperties(type, &shape, pos, shape.size());
    MctStaticData::instance()->setAddedShapeType(type);
    qDebug() << "Shape changed";
    props->setURL(type);
    QPointF prev(*prevPos);
    props->setPrevPos(prev);
    changeType = MctChangeTypes::AddedTextGraphicObject;
    changeEntity = new MctAddedTextGraphicObject("changed-text-graphic-object", props);

    if (newStroke != nullptr)
        props->addStrokeStyleChanges(newStroke);
    if (newShadow != nullptr)
        props->addShadowStyleChanges(newShadow);
    QSizeF nullSize = QSizeF(0,0);
    if (prevSize != nullSize){
        props->addSizeChanged(prevSize);
    }

    if (rotation <= 360){
        props->addRotationChanged(rotation);
    }

    MctChange *change = new MctChange(position, changeType, changeEntity);
    MctStaticData::instance()->getChanges()->append(change);
}

void MctMain::createRevision(const QString &author, const QString &comment)
{
#if QT_VERSION < 0x050000
    bool oldState = m_editor->blockSignals(true);
#else
    QObject::disconnect(editorConnection);
#endif
    normailizeChangebuffer();
    // already saved, no need to do it again
    if(! (author == "System" && comment == "revision on save")) {
        m_doc->save();
    }
    MctStaticData::instance()->getUndoGraph()->addChangeset(MctStaticData::instance()->getChanges(), new MctAuthor(author), QDateTime::currentDateTime(), comment);    
    MctStaticData::instance()->exportGraphs();
    emit adjustListOfRevisions();
    MctStaticData::instance()->clearChanges();
    qDebug() << "revision created";    
#if QT_VERSION < 0x050000
    m_editor->blockSignals(oldState);
#else
    editorConnection = connect(editor, &KoTextEditor::createMctChange, this, &MctMain::createMctChange);
#endif
}

/**
 * @brief MctMain::restoreRevision is restoring the selected revision
 * @param target is the id of the revision
 */
void MctMain::restoreRevision(QString &target)
{
#if QT_VERSION < 0x050000
    bool oldState = m_editor->blockSignals(true);
#else
    QObject::disconnect(editorConnection);
#endif
    int rev;    
    if(target.startsWith(MctStaticData::REDOCHAR)) {
        target.replace(MctStaticData::REDOCHAR, "");
        rev = target.toInt();
        updateRedoRevision(rev);
    } else {
        rev = target.toInt();
        restoreUndoRevision(rev);
    }
    m_doc->save();
    emit adjustListOfRevisions();    
    MctStaticData::instance()->exportGraphs();
    MctStaticData::instance()->clearChanges();
#if QT_VERSION < 0x050000
    m_editor->blockSignals(oldState);
#else
    editorConnection = connect(editor, &KoTextEditor::createMctChange, this, &MctMain::createMctChange);
#endif
}

/**
 * @brief MctMain::updateRedoRevision is applies the selected @b REDO revision. If the selected is not the oldest, restores all previous redo revison.
 * @param targetRevision is the seqeuential number (index) of the selected REDO revision.
 * @note Indexing starts from 1!
 */
void MctMain::updateRedoRevision(int targetRevision)
{
    int currentRev = 1;
    while (currentRev <= targetRevision) {
        MctChangeset *changesetNode = MctStaticData::instance()->getRedoGraph()->findChangeset(currentRev);
        MctStaticData::instance()->getRedoGraph()->removeChangeset(changesetNode, false);
        MctStaticData::instance()->getUndoGraph()->correctChangesetNodeListWithDate(changesetNode, MctAbstractGraph::DATE_LATER, true);
        m_redoop->redoChangeset(changesetNode);
        --targetRevision;
    }
}

/**
 * @brief MctMain::restoreUndoRevision is redoing the selected @ UNDO revision.
 * The current state - marked green - can not be selected. If there are revisions between selected and current those will be also redone.
 * @param targetRevision is the seqeuential number (index) of the selected UNDO revision.
 * @note Indexing starts from 0 but that is the initial state, can not move to REDO. #1 is the first useable revision.
 */
void MctMain::restoreUndoRevision(int targetRevision)
{
    int currentRev = MctStaticData::instance()->getUndoGraph()->getCurrentRevision();
    while(currentRev > targetRevision) {
        MctChangeset *changesetNode = MctStaticData::instance()->getUndoGraph()->findChangeset(currentRev);
        MctStaticData::instance()->getUndoGraph()->removeChangeset(changesetNode, false);
        m_undoop->undoChangeset(changesetNode);
        --currentRev;
    }
}

/**
 * @brief MctMain::getUndoRevCount
 * @return the index of the last (= newest) UNDO revision.
 */
int MctMain::undoRevCount()
{
    return MctStaticData::instance()->getUndoGraph()->getCurrentRevision();
}

/**
 * @brief MctMain::getRedoRevCount
 * @return the index of the last (= newest) REDO revision.
 */
int MctMain::redoRevCount()
{
    return MctStaticData::instance()->getRedoGraph()->getCurrentRevision();
}

/**
 * @brief MctMain::removeRevision delete the selected revision from the graph
 * @param target is the id of the selected revision
 * @note Removing a revision from the structure does not affect the state of the document. The changes of the removed revision will be permanent.
 *
 * @warning It should remove child (or depending) revision, but this part is not working yet!
 */
void MctMain::removeRevision(QString &target)
{
// FIXME: works differently than the legacy python code

    // Inicializalas
    int revNum;
    bool isUndo;
    QString list;
    MctAbstractGraph* graph;
    QList<ulong>* childrenIDList = new QList<ulong>();  // IDs of  the depending, child revisions
    QStack<ulong> revisionStack;                        // Stack, to store the order of revisions to be deleted

    // Muveleti graf kivalasztasa
    if (target.startsWith(MctStaticData::REDOCHAR)) {
        target.replace(MctStaticData::REDOCHAR, "");
        revNum = target.toInt();
        graph = MctStaticData::instance()->getRedoGraph();        
        isUndo = false;
    } else {
        revNum = target.toInt();
        graph = MctStaticData::instance()->getUndoGraph();
        isUndo = true;
    }

    // Revision doesn't exist in the graph...
    MctChangeset* targetRevision = graph->findChangeset(revNum);
    if (targetRevision == nullptr) {
        qDebug() << "Revision not found to this id!";
        return;
    }

    // Last delete: the selected revision
    childrenIDList->append(revNum);

    // Recursive search for other depending revisions (this fills up childrenList)
    collectChildren(targetRevision, childrenIDList,  graph);

    // Reverse the order of depending revs: put them into the stack
    foreach (ulong cid, *childrenIDList) {
        revisionStack.push(cid);
        list.append(QString::number(cid) + "\n");
    }
    qDebug() << "### Remove the following revisions before the selected one: " << list;

    // Alert if depending revision found
    if (childrenIDList->size() > 1) {
        QMessageBox msgBox;
        msgBox.setText("The following dependent revisions need to be deleted: \n" + list);
        msgBox.setInformativeText("Are you sure?");
        msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
        msgBox.setDefaultButton(QMessageBox::No);

        if (msgBox.exec() == QMessageBox::No) return;
    }

    // Id -> Obj, delete and correct
    for (int i = 0; i < revisionStack.size(); ++i) {
        ulong id = revisionStack.pop();
        MctChangeset* revision = graph->findChangeset(id);
        if (revision != nullptr) {
            graph->removeChangeset(revision, false);
            graph->correctChangesetNodeListWithDate(revision, (isUndo) ? (MctAbstractGraph::DATE_EARLIER) : (MctAbstractGraph::DATE_LATER), true);
        } else {
            continue; // assert?
            qCritical() << "Should not happen! (Invalid object to a valid child id)";
        }
    }

    // Update files
    MctStaticData::instance()->exportGraphs();

    // Update UI
    emit adjustListOfRevisions();

    delete childrenIDList;
}

/**
 * @brief MctMain::collectChildren recursive function to collect all depending revision in the given graph
 * @param change The concrete changeset
 * @param childrenIDs is an @b IN/OUT parameter, a list where the children's id is collected. During the recursive calls will be this list filled as well.
 * @param graph Pointer to a change graph. (Undo or Redo)
 */
void MctMain::collectChildren(MctChangeset* change, QList<ulong>* childrenIDs, MctAbstractGraph* graph)
{
    QList<ulong> *children = change->childs();

    if (children->length() == 0) return;

    // Add children to the list
    foreach (ulong cid, *children) {
        if (!childrenIDs->contains(cid))
            childrenIDs->append(cid);
    }

    // Iterate the children and collect their children
    foreach (ulong cid, *children) {
            MctChangeset* child = graph->findChangeset(cid);
            if (child != nullptr)
                collectChildren(child, childrenIDs, graph); // Go deeper
            else
                continue; // no child with this id -> assert
    }
}

/**
 * @brief Clear the entire change-tracking history. The state of the last revision will be the new initial status.
 * @note This will not restore the default initial state!
 */
void MctMain::clearRevisionHistory()
{
    QList<MctChangeset*> * revisions = MctStaticData::instance()->getUndoGraph()->getChangesetList();
    for (auto it = revisions->begin(); it != revisions->end(); ++it) {
        MctStaticData::instance()->getUndoGraph()->removeChangeset(*it, true);
    }

    revisions = MctStaticData::instance()->getRedoGraph()->getChangesetList();
    for (auto it = revisions->begin(); it != revisions->end(); ++it) {
        MctStaticData::instance()->getRedoGraph()->removeChangeset(*it, true);
    }

    emit adjustListOfRevisions();
}

/**
 * @brief Merge the changes in the changebuffer, and add the merged changes into MctStaticData.changes
 */
void MctMain::normailizeChangebuffer()
{
    if(m_changebuffer.isEmpty()) {
        return;
    }
    //TODO: AddedString/RemovedString, AddedStringInTable/RemovedStringInTable works only
    int i = 0;
    MctChange *tmpchange = m_changebuffer.at(i);
    int p1 = tmpchange->position()->startPar();
    int s = tmpchange->position()->startChar();
    int p2 = tmpchange->position()->endPar();
    int e = tmpchange->position()->endChar();
    MctCell *startCell;
    MctCell *endCell;
    MctPosition *anchor;

    if(tmpchange->changeType() == MctChangeTypes::AddedStringInTable || tmpchange->changeType() == MctChangeTypes::RemovedStringInTable){
        startCell = tmpchange->position()->startCellInfo();
        endCell = tmpchange->position()->endCellInfoEnd();
        anchor = tmpchange->position()->anchoredPos();
    }
    QString str;
    if(tmpchange->changeType() == MctChangeTypes::AddedString || tmpchange->changeType() == MctChangeTypes::RemovedString
       || tmpchange->changeType() == MctChangeTypes::AddedStringInTable
       || tmpchange->changeType() == MctChangeTypes::RemovedStringInTable) {
        str = dynamic_cast<MctStringChange*>(tmpchange->changeEntity())->getString();
    } else {
        MctStaticData::instance()->getChanges()->append(tmpchange);
        m_changebuffer.clear();
        return;
    }
    while(++i < m_changebuffer.length()) {
        tmpchange = m_changebuffer.at(i);
        if(tmpchange->changeType() == MctChangeTypes::AddedString) {
            p2 = tmpchange->position()->endPar();
            e = tmpchange->position()->endChar();
            str.append(dynamic_cast<MctStringChange*>(tmpchange->changeEntity())->getString());
        } else if(tmpchange->changeType() == MctChangeTypes::RemovedString) {
            p1 = tmpchange->position()->startPar();
            s =  tmpchange->position()->startChar();
            str.prepend(dynamic_cast<MctStringChange*>(tmpchange->changeEntity())->getString());
        } else if(tmpchange->changeType() == MctChangeTypes::AddedStringInTable) {
            p2 = tmpchange->position()->endPar();
            e = tmpchange->position()->endChar();
            endCell = tmpchange->position()->endCellInfoEnd();
            setAnchorPosition(anchor, tmpchange->position()->anchoredPos(), tmpchange->changeType());
            str.append(dynamic_cast<MctStringChangeInTable*>(tmpchange->changeEntity())->getString());
        } else if(tmpchange->changeType() == MctChangeTypes::RemovedStringInTable) {
            p1 = tmpchange->position()->startPar();
            s =  tmpchange->position()->startChar();
            startCell = tmpchange->position()->startCellInfo();
            setAnchorPosition(anchor, tmpchange->position()->anchoredPos(), tmpchange->changeType());
            str.prepend(dynamic_cast<MctStringChangeInTable*>(tmpchange->changeEntity())->getString());
        }
    }

    MctPosition *pos = new MctPosition(p1, s, p2, e);
    MctNode *changeEntity;
    if(tmpchange->changeType() == MctChangeTypes::AddedStringInTable || tmpchange->changeType() == MctChangeTypes::RemovedStringInTable){
        pos->setStartCellInfo(startCell);
        pos->setEndCellInfo(endCell);
        pos->setAnchored(anchor);
        changeEntity = new MctStringChangeInTable(str);
    } else {
        changeEntity = new MctStringChange(str);
    }
    MctChange *change = new MctChange(pos, tmpchange->changeType(), changeEntity);

    MctStaticData::instance()->getChanges()->append(change);
    m_changebuffer.clear();
}

QString MctMain::fileUrl() const
{
    return m_fileURL;
}

void MctMain::documentSavedAs(const QString &fileUrl)
{
    MctStaticData::instance()->setFileURL(fileUrl);
    MctStaticData::instance()->getUndoGraph()->setFilename(fileUrl);
    MctStaticData::instance()->getRedoGraph()->setFilename(fileUrl);
    MctStaticData::instance()->getUndoGraph()->setOdtFile(fileUrl);
    MctStaticData::instance()->getRedoGraph()->setOdtFile(fileUrl);
}

void MctMain::connectSignals()
{
    #if QT_VERSION < 0x050000
        connect(m_editor, SIGNAL(shapeOperationSignal(KoShape*,ChangeAction)), this, SLOT(shapeOperationSlot(KoShape*,ChangeAction)));
        connect(m_doc, SIGNAL(createShapeMctChange(QString,QPointF,KoShape&,ChangeAction,QPointF*)), this, SLOT(createShapeMctChange(QString,QPointF,KoShape&,ChangeAction,QPointF*)));
        connect(m_doc, SIGNAL(createShapeStyleChanged(QString,QPointF,KoShape&,KoShapeStroke*,KoShapeShadow*,QPointF*,QSizeF,double)), this, SLOT(createShapeStyleChanged(QString,QPointF,KoShape&,KoShapeStroke*,KoShapeShadow*,QPointF*,QSizeF,double)));
        connect(m_doc, SIGNAL(shapePositionChanged(KoShape*,QPointF,QPointF*)), this, SLOT(createShapePositionChanged(KoShape*,QPointF,QPointF*)));
        connect(m_editor, SIGNAL(createMctChange(QTextCursor&,MctChangeTypes,KUndo2MagicString,QTextFormat,QTextFormat)), this, SLOT(createMctChange(QTextCursor&,MctChangeTypes,KUndo2MagicString,QTextFormat,QTextFormat)));
    #else
        connect(editor, &KoTextEditor::shapeOperationSignal, this, &MctMain::shapeOperationSlot);
        connect(doc, &KoDocument::createShapeMctChange, this, &MctMain::createShapeMctChange);
        connect(doc, &KoDocument::createShapeStyleChanged, this, &MctMain::createShapeStyleChanged);
        connect(doc, &KoDocument::shapePositionChanged, this, &MctMain::createShapePositionChanged);
        editorConnection = connect(editor, &KoTextEditor::createMctChange, this, &MctMain::createMctChange);
    #endif
}

void MctMain::disconnectSignals()
{
    this->disconnect();
    this->m_editor->disconnect();
    this->m_doc->disconnect();
}

void MctMain::setAnchorPosition(MctPosition *anchor, MctPosition *InnerAnchor, MctChangeTypes changeType)
{
    if (changeType == MctChangeTypes::AddedStringInTable) {
        anchor->setEndCellInfo(InnerAnchor->endCellInfoEnd());
        anchor->setEndChar(InnerAnchor->endChar());
        anchor->setEndPar(InnerAnchor->endPar());
    } else if (changeType == MctChangeTypes::RemovedStringInTable) {
        anchor->setStartCellInfo(InnerAnchor->startCellInfo());
        anchor->setStartChar(InnerAnchor->startChar());
        anchor->setStartPar(InnerAnchor->startPar());
    }
    if (InnerAnchor->anchoredPos()) {
        setAnchorPosition(anchor->anchoredPos(), InnerAnchor->anchoredPos(), changeType);
    }
}

bool MctMain::posCheckInTable(MctPosition *lastpos, MctPosition *pos, MctChangeTypes changeType)
{
    if (changeType == MctChangeTypes::AddedStringInTable) {
        if (lastpos->startCellInfo()->col() == pos->startCellInfo()->col()
           && lastpos->startCellInfo()->row() == pos->startCellInfo()->row()
           && lastpos->anchoredPos()->endChar() == pos->anchoredPos()->startChar()
           && lastpos->anchoredPos()->endPar() == pos->anchoredPos()->startPar()) {
            bool isPosRight = true;

            if (lastpos->anchoredPos()->startCellInfo() && pos->anchoredPos()->startCellInfo()) {
                isPosRight = posCheckInTable(lastpos->anchoredPos(), pos->anchoredPos(), changeType);
            } else if (lastpos->anchoredPos()->startCellInfo()  || pos->anchoredPos()->startCellInfo() ) {
                isPosRight = false;
            }

            return isPosRight;
        }
    } else if (changeType == MctChangeTypes::RemovedStringInTable) {
        if (lastpos->startCellInfo()->col() == pos->startCellInfo()->col()
           && lastpos->startCellInfo()->row() == pos->startCellInfo()->row()
           && lastpos->anchoredPos()->startChar() == pos->anchoredPos()->endChar()
           && lastpos->anchoredPos()->startPar() == pos->anchoredPos()->endPar()) {
            bool isPosRight = true;

            if (lastpos->anchoredPos()->startCellInfo() && pos->anchoredPos()->startCellInfo()) {
                isPosRight = posCheckInTable(lastpos->anchoredPos(), pos->anchoredPos(), changeType);
            } else if (lastpos->anchoredPos()->startCellInfo()  || pos->anchoredPos()->startCellInfo() ) {
                isPosRight = false;
            }

            return isPosRight;
        }
    }
    return false;
}


//void MctMain::correctBlockPositions()
//{
//    QMap<ulong, ulong> *indexes = MctStaticData::instance()->getFrameIndexes(koTextDoc);

//    QList<MctChange *> *changes = MctStaticData::instance()->getChanges();
//    foreach (MctChange* change, *changes) {
//        MctPosition *pos = change->getPosition();
//        ulong startidx = pos->getStartPar();
//        QMap<ulong,ulong>::iterator it = indexes->lowerBound(startidx);
//        if(it != indexes->begin()) {
//            --it;
//        }
//        if(startidx > it.key()) {
//            pos->setStartPar(startidx - it.value());
//            pos->setEndPar(pos->getEndPar() - it.value());
//            change->setPosition(pos);
//        }

//        MctPosition *pos_moved = change->getMovedPosition();
//        if(pos_moved) {
//            startidx = pos_moved->getStartPar();
//            it = indexes->lowerBound(startidx);
//            if(it != indexes->begin()) {
//                --it;
//            }
//            if(startidx >= it.key()) {
//                pos_moved->setStartPar(startidx - it.value());
//                pos_moved->setEndPar(pos_moved->getEndPar() - it.value());
//                change->setMovedPosition(pos_moved);
//            }
//        }
//    }
//}
//
