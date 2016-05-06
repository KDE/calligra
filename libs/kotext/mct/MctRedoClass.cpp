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

#include "MctRedoClass.h"
#include "MctUndoGraph.h"
#include "MctChangeEntities.h"
#include "MctPosition.h"
#include "MctTableProperties.h"
#include "MctEmbObjProperties.h"

#include <QTextBlock>
#include <QTextTable>
#include <KoTextDocument.h>
#include <KoTextEditor.h>
#include <KoList.h>
#include <KoStyleManager.h>
#include <KoListLevelProperties.h>
#include <KoParagraphStyle.h>
#include <KoDocument.h>

MctRedoClass::MctRedoClass(KoTextDocument *kotextdoc)
{
    doc = kotextdoc;
}

MctRedoClass::~MctRedoClass()
{

}

/**
 * @brief This converts and adds changeset from redo graph into undo graph
 * @param changesetNode ChangesetNode in the undo graph
 * @param undoChangeList The list of corresponding undo change nodes.
 * @return Returns with the created changesetNode.
 */
MctChangeset * MctRedoClass::addChangesetToUndo(MctChangeset *changesetNode, QList<MctChange *> *undoChangeList)
{
    changesetNode->clearParents();
    changesetNode->clearChilds();

    MctAuthor *author = changesetNode->getAuthor();
    QDateTime date = changesetNode->getDate();
    QString comment = changesetNode->getComment();

    return MctStaticData::instance()->getUndoGraph()->addChangeset(undoChangeList, author, date, comment);
}

/**
 * @brief This redos one changeset in the document
 * @param changesetNode The changeset node in the redograph.
 * @param add2Graph
 */
void MctRedoClass::redoChangeset(MctChangeset *changesetNode, bool add2Graph)
{
    QList<MctChange *> *undoChangeList = new QList<MctChange *> ();
    QList<MctChange *> *changelist = changesetNode->getChanges();

    foreach (MctChange * change, *changelist) {
        MctChange* undochangeNode = redoChange(change);
        undoChangeList->append(undochangeNode);
    }

    if(add2Graph) {
        addChangesetToUndo(changesetNode, undoChangeList);
    }
}

/**
 * @brief This redo one change in the document
 * @param changeNode The change node in the redograph.
 * @return
 */
MctChange* MctRedoClass::redoChange(MctChange *changeNode)
{
    MctChange* undoChangeNode = NULL;
    if(changeNode == NULL) {
        return undoChangeNode;
    }

    //Redoing the change

    MctChangeTypes changeType = changeNode->getChangeType();
    if(changeType == MctChangeTypes::AddedString) {
        undoChangeNode = redoAddedString(changeNode);
    } else if (changeType == MctChangeTypes::RemovedString) {
        undoChangeNode = redoRemovedString(changeNode);
    } else if (changeType == MctChangeTypes::MovedString) {
        undoChangeNode = redoMovedString(changeNode);
    } else if( changeType == MctChangeTypes::ParagraphBreak ){
        undoChangeNode = redoParagraphBreak( changeNode );
    } else if ( changeType == MctChangeTypes::DelParagraphBreak ){
        undoChangeNode = redoDelParagraphBreak( changeNode );
    } else if ( changeType == MctChangeTypes::StyleChange ){
        undoChangeNode = redoStyleChange( changeNode );
    } else if ( changeType == MctChangeTypes::StyleChangeInTable ){
        undoChangeNode = redoStyleChangeInTable( changeNode );
    } else if ( changeType == MctChangeTypes::AddedTextFrame ) {
    } else if ( changeType == MctChangeTypes::RemovedTextFrame  ) {
    } else if ( changeType == MctChangeTypes::AddedTextGraphicObject  ) {
        undoChangeNode = redoAddedTextGraphicObjects( changeNode );
    } else if ( changeType == MctChangeTypes::AddedTextGraphicObjectInTable  ) {
        undoChangeNode = redoAddedTextGraphicObjectsInTable( changeNode );
    } else if ( changeType == MctChangeTypes::RemovedTextGraphicObject ) {
        undoChangeNode = redoRemovedTextGraphicObjects( changeNode );
    } else if ( changeType == MctChangeTypes::RemovedTextGraphicObjectInTable ) {
        undoChangeNode = redoRemovedTextGraphicObjectsInTable( changeNode );
    } else if ( changeType == MctChangeTypes::AddedEmbeddedObject  ) {
    } else if ( changeType == MctChangeTypes::RemovedEmbeddedObject  ) {
    } else if ( changeType == MctChangeTypes::AddedStringInTable ){
        undoChangeNode = redoAddedStringInTable( changeNode );
    } else if ( changeType == MctChangeTypes::RemovedStringInTable ){
        undoChangeNode = redoRemovedStringInTable( changeNode );
    } else if ( changeType == MctChangeTypes::MovedStringInTable ){
        undoChangeNode = redoMovedStringInTable( changeNode );
    } else if ( changeType == MctChangeTypes::ParagraphBreakInTable ){
        undoChangeNode = redoParagraphBreakInTable( changeNode );
    } else if ( changeType == MctChangeTypes::DelParagraphBreakInTable ){
        undoChangeNode = redoDelParagraphBreakInTable( changeNode );
    } else if ( changeType == MctChangeTypes::AddedTextTable  ) {
        undoChangeNode = redoAddedTextTable( changeNode );
    } else if ( changeType == MctChangeTypes::AddedTextTableInTable  ) {
        undoChangeNode = redoAddedTextTableInTable( changeNode );
    } else if ( changeType == MctChangeTypes::RemovedTextTable  ) {
        undoChangeNode = redoRemovedTextTable( changeNode );
    } else if ( changeType == MctChangeTypes::RemovedTextTableInTable  ) {
        undoChangeNode = redoRemovedTextTableInTable( changeNode );
    } else if ( changeType == MctChangeTypes::AddedRowInTable  ) {
        undoChangeNode = redoAddedRowInTable( changeNode );
    } else if ( changeType == MctChangeTypes::RemovedRowInTable  ) {
        undoChangeNode = redoRemovedRowInTable( changeNode );
    } else if ( changeType == MctChangeTypes::AddedColInTable  ) {
        undoChangeNode = redoAddedColInTable( changeNode );
    } else if ( changeType == MctChangeTypes::RemovedColInTable  ) {
        undoChangeNode = redoRemovedColInTable( changeNode );
    } else{
        undoChangeNode = NULL;
    }

    return undoChangeNode;
}

/**
  * @brief This Redoes an AddedString change node
  * @param changeNode The change node to Redone
  * @return Returns with the cretaed redochangeNode
  */
 MctChange * MctRedoClass::redoAddedString(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();
     MctStringChange *changeEntity = dynamic_cast<MctStringChange*>(changeNode->getChangeEntity());
     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->getStartChar());

     doc->textEditor()->setPosition(cursor->position());
     doc->textEditor()->insertText(changeEntity->getString());
     //cursor->insertText(changeEntity->getString());

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::AddedString, changeEntity);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoRemovedString(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();
     MctStringChange *changeEntity = dynamic_cast<MctStringChange*>(changeNode->getChangeEntity());
     QTextCursor *cursor = createcursor(changeNode, pos);

     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->getStartChar());
     doc->textEditor()->setPosition(cursor->position());
     cursor->setPosition(blockpos + pos->getEndChar(), QTextCursor::KeepAnchor);
     doc->textEditor()->setPosition(cursor->position(), QTextCursor::KeepAnchor);
     QString string = cursor->selectedText();
     changeEntity->setString(string);
     //cursor->removeSelectedText();
     doc->textEditor()->deleteChar();

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::RemovedString, changeEntity);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoMovedString(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();
     MctPosition *movedpos = changeNode->getMovedPosition();
     MctStringChange *changeEntity = dynamic_cast<MctStringChange*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->getStartChar());
     doc->textEditor()->setPosition(cursor->position());
     cursor->setPosition(blockpos + pos->getEndChar(), QTextCursor::KeepAnchor);
     doc->textEditor()->setPosition(cursor->position(), QTextCursor::KeepAnchor);
     //cursor->removeSelectedText();
     doc->textEditor()->deleteChar();

     cursor = createcursor(changeNode, movedpos);
     blockpos = cursor->block().position();
     cursor->setPosition(blockpos + movedpos->getStartChar());
     doc->textEditor()->setPosition(cursor->position());
     //cursor->insertText(changeEntity->getString());
     doc->textEditor()->insertText(changeEntity->getString());

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::MovedString, changeEntity, movedpos);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoAddedStringInTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();
     MctStringChangeInTable *changeEntity = dynamic_cast<MctStringChangeInTable*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);

     doc->textEditor()->setPosition(pos->getTableCellPosition(cursor));

     doc->textEditor()->insertText(changeEntity->getString());

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::AddedStringInTable, changeEntity);
     return undoChangeNode;

 }

 MctChange * MctRedoClass::redoRemovedStringInTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();
     MctStringChangeInTable *changeEntity = dynamic_cast<MctStringChangeInTable*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     //táblázat kezdő blokja
     int blockpos  = cursor->block().position();
     MctPosition *tmp = pos;
     QTextCursor tmpcursor(*cursor);
     QTextTable * table;
     while(tmp->getAnchoredPos()) {
         blockpos  = tmpcursor.block().position();
         bool hiddenTableHandling = tmpcursor.blockFormat().hasProperty(KoParagraphStyle::HiddenByTable);
         table = tmpcursor.currentTable();
         if(hiddenTableHandling) {
             tmpcursor.movePosition(QTextCursor::NextCharacter);
             table = tmpcursor.currentTable();
         }
         QTextTableCell cell = table->cellAt(tmp->getCellInfo()->getRow(), tmp->getCellInfo()->getCol());
         tmpcursor = cell.firstCursorPosition();
         tmp = tmp->getAnchoredPos();
         // if a nested table comes, jump +1 extra block ahead
         int k = tmp->getAnchoredPos() ? 1 : 0;
         tmpcursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, k + tmp->getStartPar());
         tmpcursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, tmp->getStartChar());
     }

     doc->textEditor()->setPosition(tmpcursor.position());
     blockpos  = tmpcursor.block().position();

     tmpcursor.setPosition(blockpos + tmp->getEndChar(), QTextCursor::KeepAnchor);
     doc->textEditor()->setPosition(tmpcursor.position(), QTextCursor::KeepAnchor);

     QString string = tmpcursor.selectedText();
     changeEntity->setString(string);

     doc->textEditor()->deleteChar();

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::RemovedStringInTable, changeEntity);
     return undoChangeNode;

 }

 MctChange * MctRedoClass::redoMovedStringInTable(MctChange * changeNode)
 {
     //todo
     return NULL;
 }

 MctChange * MctRedoClass::redoParagraphBreak(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();
     MctParagraphBreak *changeEntity = dynamic_cast<MctParagraphBreak*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->getStartChar());
     doc->textEditor()->setPosition(cursor->position());
     doc->textEditor()->newLine();

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::ParagraphBreak, changeEntity);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoParagraphBreakInTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();
     MctParagraphBreakInTable *changeEntity = dynamic_cast<MctParagraphBreakInTable*>(changeNode->getChangeEntity());
     QTextCursor *cursor = createcursor(changeNode, pos);

     doc->textEditor()->setPosition(pos->getTableCellPosition(cursor));

     doc->textEditor()->newLine();

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::ParagraphBreakInTable, changeEntity);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoDelParagraphBreak(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();
     MctDelParagraphBreak *changeEntity = dynamic_cast<MctDelParagraphBreak*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->getStartChar());
     doc->textEditor()->setPosition(cursor->position());
     doc->textEditor()->deletePreviousChar();

     if(pos->getStartPar() > pos->getEndPar()) {
         ulong tmp = pos->getStartPar();
         pos->setStartPar(pos->getEndPar());
         pos->setEndPar(tmp);
         tmp = pos->getStartChar();
         pos->setStartChar(pos->getEndChar());
         pos->setEndChar(tmp);
     }

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::DelParagraphBreak, changeEntity);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoDelParagraphBreakInTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();
     MctDelParagraphBreakInTable *changeEntity = dynamic_cast<MctDelParagraphBreakInTable*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);

     doc->textEditor()->setPosition(pos->getTableCellPosition(cursor));
     doc->textEditor()->deleteChar();

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::DelParagraphBreakInTable, changeEntity);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoStyleChange(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();
     MctStylePropertyChange *changeEntity = dynamic_cast<MctStylePropertyChange*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->getStartChar());
     doc->textEditor()->setPosition(cursor->position());
     cursor->setPosition(blockpos + pos->getEndChar(), QTextCursor::KeepAnchor);
     doc->textEditor()->setPosition(cursor->position(), QTextCursor::KeepAnchor);

     redoPropsChange(doc->textEditor()->cursor(), changeEntity->getTextPropChanges());
     redoPropsChange(doc->textEditor()->cursor(), changeEntity->getParagraphPropChanges());
     redoPropsChange(doc->textEditor()->cursor(), changeEntity->getListPropChanges());
     redoPropsChange(doc->textEditor()->cursor(), changeEntity->getOtherPropChanges());

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::StyleChange, changeEntity);
     return undoChangeNode;
 }

 void MctRedoClass::redoPropsChange(QTextCursor *cursor, ChangeEventList * propchanges)
 {
     foreach (ChangeEvent *change, *propchanges) {
         QTextFormat format = change->getOldFormat();
         if(format.isCharFormat()) {
             QTextFormat currentformat = cursor->charFormat();
             ChangeEvent::printProperties(format);
             ChangeEvent::printProperties(currentformat);
             QTextFormat prevformat = ChangeEvent::getNewValuesForFormat(currentformat, format);
             ChangeEvent::printProperties(prevformat);

             cursor->mergeCharFormat(format.toCharFormat());
             ChangeEvent::printProperties(cursor->charFormat());

             change->setNewFormat(format);
             change->setOldFormat(prevformat);
             doc->textEditor()->registerTrackedChange(*cursor, KoGenChange::FormatChange, kundo2_i18n("Formatting"), format, currentformat, false);

         } else if (format.isBlockFormat()) {
             QTextBlockFormat currentformat = cursor->blockFormat();
             ChangeEvent::printProperties(format);
             ChangeEvent::printProperties(currentformat);
             QTextFormat prevformat = ChangeEvent::getNewValuesForFormat(currentformat, format);
             ChangeEvent::printProperties(prevformat);

             cursor->mergeBlockFormat(format.toBlockFormat());
             ChangeEvent::printProperties(cursor->blockFormat());

             change->setNewFormat(change->getOldFormat());
             change->setOldFormat(prevformat);

         } else if(format.isListFormat()) {
             QTextList *textList = cursor->currentList();
             QTextListFormat listformat = format.toListFormat();
             QTextFormat prevformat;
             ChangeEvent::printProperties(format);
             ChangeEvent::printProperties(prevformat);
             if(textList) {
                 QTextListFormat currentformat = textList->format();
                 prevformat = ChangeEvent::getNewValuesForFormat(currentformat, format);

                 if(prevformat.properties().empty()) {
                     textList->setFormat(listformat);
                     prevformat = currentformat;
                 } else {
                     textList->setFormat(listformat);
                 }
             } else { // TODO: in depth compre with ChangeListCommand constructor (should be similar)

                 QTextListFormat currentTextListFormat = format.toListFormat();
                 KoListStyle *listStyle = doc->styleManager()->defaultListStyle()->clone();
                 KoListLevelProperties llp = listStyle->levelProperties(1);
                 llp.setStyle(static_cast<KoListStyle::Style>(currentTextListFormat.style()));
                 KoListStyle::Style currentStyle = static_cast<KoListStyle::Style>(currentTextListFormat.style());
                 if (KoListStyle::isNumberingStyle(currentStyle)) {
                     llp.setStartValue(1);
                     llp.setListItemSuffix(".");
                 } else {
                     llp.setStartValue(0);
                     llp.setListItemSuffix("");
                 }
                 llp.setLevel(currentTextListFormat.indent());
                 listStyle->setLevelProperties(llp);
                 KoList *newList = new KoList(doc->document(), listStyle);                 
                 newList->add(cursor->block(), 1);

                if(prevformat.propertyCount() == 0) {
                    prevformat = QTextListFormat();
                }
             }

             change->setNewFormat(change->getOldFormat());
             change->setOldFormat(prevformat);
         }

     }
 }

 MctChange * MctRedoClass::redoStyleChangeInTable(MctChange * changeNode)
 {
     //todo
     return NULL;
 }

 MctChange * MctRedoClass::redoAddedTextGraphicObjects(MctChange * changeNode, bool withprops)
 {
     MctAddedTextGraphicObject *changeEntity = dynamic_cast<MctAddedTextGraphicObject*>(changeNode->getChangeEntity());
     MctEmbObjProperties* props = dynamic_cast<MctEmbObjProperties*>(changeEntity->getObjectProperties());

     MctPosition *pos = changeNode->getPosition();

     KoShape *shape = props->getShape();

     if (changeEntity->getName() == "changed-text-graphic-object"){
         QSizeF shapeSize = shape->size();
         if (props->getProps2Export()->contains("PrevSizeHeight")){
             shape->setSize(QSizeF(props->getProps2Export()->value("PrevSizeWidth").toDouble(), props->getProps2Export()->value("PrevSizeHeight").toDouble()));
         }

         double rotation = shape->rotation();
         if (props->getProps2Export()->contains("PrevRotation")){
             shape->rotate(360 - props->getProps2Export()->value("Rotation").toDouble() + props->getProps2Export()->value("PrevRotation").toDouble());
         }

         QPointF position(props->getProps2Export()->value("PositionX").toFloat(), props->getProps2Export()->value("PositionY").toFloat());
         QPointF prevPosition(props->getProps2Export()->value("PrevPositionX").toFloat(), props->getProps2Export()->value("PrevPositionY").toFloat());
         QPointF actPos = shape->position();
         if (prevPosition != actPos) position = prevPosition;
         shape->setPosition(position);
         MctEmbObjProperties *props2 = new MctEmbObjProperties(props->getURL(), shape, position, shape->size());

         if (props->getProps2Export()->contains("PrevSizeHeight")) {
             props2->addSizeChanged(shapeSize);
         }

         if (props->getProps2Export()->contains("PrevRotation")){
             props2->addRotationChanged(rotation);
         }

         props->textGraphicStyleChanges(props2, shape);

         props2->setPrevPos(actPos);
         changeEntity->setObjectProperties(props2);
     } else {
         doc->textEditor()->shapeOperation(shape, ADDED);
     }

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::AddedTextGraphicObject, changeEntity);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoRemovedTextGraphicObjects(MctChange * changeNode, bool withprops)
 {
     MctRemovedTextGraphicObject *changeEntity = dynamic_cast<MctRemovedTextGraphicObject*>(changeNode->getChangeEntity());
     MctEmbObjProperties* props = dynamic_cast<MctEmbObjProperties*>(changeEntity->getObjectProperties());

     MctPosition *pos = changeNode->getPosition();

     //inserted shape, else image
     if (!props->getURL().contains("/")){
//FIXME
        //KoShape *shape = MctStaticData::instance()->getKoDocument()->emitPosition(props->getCenterPos());
        //doc->textEditor()->shapeOperation(shape, REMOVED);
     } else {
        KoShape *shapeImg = props->getShape();
        doc->textEditor()->shapeOperation(shapeImg, REMOVED);
     }

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::RemovedTextGraphicObject, changeEntity);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoAddedTextGraphicObjectsInTable(MctChange * changeNode, bool withprops)
 {
     //todo
     return NULL;
 }

 MctChange * MctRedoClass::redoRemovedTextGraphicObjectsInTable(MctChange * changeNode, bool withprops)
 {
     //todo
     return NULL;
 }

 MctChange * MctRedoClass::redoAddedTextTable(MctChange * changeNode, bool withprops)
 {
     MctPosition *pos = changeNode->getPosition();
     MctAddedTextTable *changeEntity = dynamic_cast<MctAddedTextTable*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->getStartChar());
     doc->textEditor()->setPosition(cursor->position());

     MctTableProperties* props = dynamic_cast<MctTableProperties*>(changeEntity->getObjectProperties());
     int rows = props->getRows();
     int cols = props->getCols();

     doc->textEditor()->insertTable(rows, cols);

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::AddedTextTable, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctRedoClass::redoRemovedTextTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();

     qDebug() << pos->toString();

     MctRemovedTextTable *changeEntity = dynamic_cast<MctRemovedTextTable*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->getStartChar());

     qDebug() << "Cursor position: " << cursor->position() << " ->(1)[table]";

     doc->textEditor()->setPosition(cursor->position());
     doc->textEditor()->deleteChar();   // jump over the hidden block before the table
     qDebug() << "Cursor position: " << cursor->position() << " ->[table]";
     doc->textEditor()->deleteTable();

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::RemovedTextTable, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctRedoClass::redoAddedTextTableInTable(MctChange * changeNode, bool withprops)
 {
     MctPosition *pos = changeNode->getPosition();
     MctAddedTextTableInTable *changeEntity = dynamic_cast<MctAddedTextTableInTable*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);

     doc->textEditor()->setPosition(pos->getTableCellPosition(cursor));

     MctTableProperties* props = dynamic_cast<MctTableProperties*>(changeEntity->getObjectProperties());
     int rows = props->getRows();
     int cols = props->getCols();

     doc->textEditor()->insertTable(rows, cols);

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::AddedTextTableInTable, changeEntity);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoRemovedTextTableInTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();

     qDebug() << pos->toString();

     MctRemovedTextTableInTable *changeEntity = dynamic_cast<MctRemovedTextTableInTable*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);

     doc->textEditor()->setPosition(pos->getTableCellPosition(cursor) + 1);
     //doc->textEditor()->deleteChar();   // jump over the hidden block before the table
     qDebug() << "Cursor position: " << cursor->position() << " ->[table]";
     doc->textEditor()->deleteTable();

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::RemovedTextTableInTable, changeEntity);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoAddedRowInTable(MctChange * changeNode)
 {
     //todo
     return NULL;
 }

 MctChange * MctRedoClass::redoRemovedRowInTable(MctChange * changeNode)
 {
     //todo
     return NULL;
 }

 MctChange * MctRedoClass::redoAddedColInTable(MctChange * changeNode)
 {
     //todo
     return NULL;
 }

 MctChange * MctRedoClass::redoRemovedColInTable(MctChange * changeNode)
 {
     //todo
     return NULL;
 }
