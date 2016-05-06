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

#include "MctUndoClass.h"
#include "MctChangeEntities.h"
#include "MctPosition.h"
#include "MctTableProperties.h"
#include "MctEmbObjProperties.h"

#include <QTextBlock>
#include <KoTextDocument.h>
#include <KoTextEditor.h>
#include <KoList.h>
#include <QTextList>
#include <QTextTable>
#include <KoParagraphStyle.h>
#include <KoDocument.h>

MctUndoClass::MctUndoClass(KoTextDocument *kotextdoc)
{
    doc = kotextdoc;
}

MctUndoClass::~MctUndoClass()
{

}

MctChangeset * MctUndoClass::addChangesetToRedo(MctChangeset *changesetNode, QList<MctChange *> *redoChangeList)
{
    changesetNode->clearParents();
    changesetNode->clearChilds();

    MctAuthor *author = changesetNode->getAuthor();
    QDateTime date = changesetNode->getDate();
    QString comment = changesetNode->getComment();

    return MctStaticData::instance()->getRedoGraph()->addChangeset(redoChangeList, author, date, comment);
}

void MctUndoClass::undoChangeset(MctChangeset *changesetNode, bool add2Graph)
{
    QList<MctChange *> *redochangelist = new QList<MctChange *> ();
    QList<MctChange *> *changelist = changesetNode->getChanges();

    foreach (MctChange * change, *changelist) {
        MctChange* redochangeNode = undoChange(change);
        redochangelist->append(redochangeNode);
    }

    if(add2Graph) {
        addChangesetToRedo(changesetNode, redochangelist);
    }
}

MctChange* MctUndoClass::undoChange(MctChange *changeNode)
{
    MctChange* redoChangeNode = NULL;
    if(changeNode == NULL) {
        return redoChangeNode;
    }

    //Redoing the change

    MctChangeTypes changeType = changeNode->getChangeType();
    if(changeType == MctChangeTypes::AddedString) {
        redoChangeNode = undoAddedString(changeNode);
    } else if (changeType == MctChangeTypes::RemovedString) {
        redoChangeNode = undoRemovedString(changeNode);
    } else if (changeType == MctChangeTypes::MovedString) {
        redoChangeNode = undoMovedString(changeNode);
    } else if( changeType == MctChangeTypes::ParagraphBreak ){
        redoChangeNode = undoParagraphBreak( changeNode );
    } else if ( changeType == MctChangeTypes::DelParagraphBreak ){
        redoChangeNode = undoDelParagraphBreak( changeNode );
    } else if ( changeType == MctChangeTypes::StyleChange ){
        redoChangeNode = undoStyleChange( changeNode );
    } else if ( changeType == MctChangeTypes::StyleChangeInTable ){
        redoChangeNode = undoStyleChangeInTable( changeNode );
    } else if ( changeType == MctChangeTypes::AddedTextFrame ) {
    } else if ( changeType == MctChangeTypes::RemovedTextFrame  ) {
    } else if ( changeType == MctChangeTypes::AddedTextGraphicObject  ) {
        redoChangeNode = undoAddedTextGraphicObjects( changeNode );
    } else if ( changeType == MctChangeTypes::AddedTextGraphicObjectInTable  ) {
        redoChangeNode = undoAddedTextGraphicObjectsInTable( changeNode );
    } else if ( changeType == MctChangeTypes::RemovedTextGraphicObject ) {
        redoChangeNode = undoRemovedTextGraphicObjects( changeNode );
    } else if ( changeType == MctChangeTypes::RemovedTextGraphicObjectInTable ) {
        redoChangeNode = undoRemovedTextGraphicObjectsInTable( changeNode );
    } else if ( changeType == MctChangeTypes::AddedEmbeddedObject  ) {
    } else if ( changeType == MctChangeTypes::RemovedEmbeddedObject  ) {
    } else if ( changeType == MctChangeTypes::AddedStringInTable ){
        redoChangeNode = undoAddedStringInTable( changeNode );
    } else if ( changeType == MctChangeTypes::RemovedStringInTable ){
        redoChangeNode = undoRemovedStringInTable( changeNode );
    } else if ( changeType == MctChangeTypes::MovedStringInTable ){
        redoChangeNode = undoMovedStringInTable( changeNode );
    } else if ( changeType == MctChangeTypes::ParagraphBreakInTable ){
        redoChangeNode = undoParagraphBreakInTable( changeNode );
    } else if ( changeType == MctChangeTypes::DelParagraphBreakInTable ){
        redoChangeNode = undoDelParagraphBreakInTable( changeNode );
    } else if ( changeType == MctChangeTypes::AddedTextTable  ) {
        redoChangeNode = undoAddedTextTable( changeNode );
    } else if ( changeType == MctChangeTypes::AddedTextTableInTable  ) {
        redoChangeNode = undoAddedTextTableInTable( changeNode );
    } else if ( changeType == MctChangeTypes::RemovedTextTable  ) {
        redoChangeNode = undoRemovedTextTable( changeNode );
    } else if ( changeType == MctChangeTypes::RemovedTextTableInTable  ) {
        redoChangeNode = undoRemovedTextTableInTable( changeNode );
    } else if ( changeType == MctChangeTypes::AddedRowInTable  ) {
        redoChangeNode = undoAddedRowInTable( changeNode );
    } else if ( changeType == MctChangeTypes::RemovedRowInTable  ) {
        redoChangeNode = undoRemovedRowInTable( changeNode );
    } else if ( changeType == MctChangeTypes::AddedColInTable  ) {
        redoChangeNode = undoAddedColInTable( changeNode );
    } else if ( changeType == MctChangeTypes::RemovedColInTable  ) {
        redoChangeNode = undoRemovedColInTable( changeNode );
    } else{
        redoChangeNode = NULL;
    }

    return redoChangeNode;
}


/**
  * @brief This undoes an AddedString change node
  * @param changeNode The change node to undone
  * @return Returns with the cretaed redochangeNode
  */
 MctChange * MctUndoClass::undoAddedString(MctChange * changeNode)
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
     doc->textEditor()->deleteChar();
//     cursor->removeSelectedText();

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::AddedString, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoRemovedString(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();
     MctStringChange *changeEntity = dynamic_cast<MctStringChange*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->getStartChar());

     doc->textEditor()->setPosition(cursor->position());
     doc->textEditor()->insertText(changeEntity->getString());
     //cursor->insertText(changeEntity->getString());

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::RemovedString, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoMovedString(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();
     MctPosition *movedpos = changeNode->getMovedPosition();
     MctStringChange *changeEntity = dynamic_cast<MctStringChange*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, movedpos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + movedpos->getStartChar());
     doc->textEditor()->setPosition(cursor->position());
     cursor->setPosition(blockpos + movedpos->getEndChar(), QTextCursor::KeepAnchor);
     doc->textEditor()->setPosition(cursor->position(), QTextCursor::KeepAnchor);
     //cursor->removeSelectedText();
     doc->textEditor()->deleteChar();

     cursor = createcursor(changeNode, pos);
     blockpos = cursor->block().position();
     cursor->setPosition(blockpos + pos->getStartChar());
     doc->textEditor()->setPosition(cursor->position());
     //cursor->insertText(changeEntity->getString());
     doc->textEditor()->insertText(changeEntity->getString());

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::MovedString, changeEntity, movedpos);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoAddedStringInTable(MctChange * changeNode)
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
         // if nested table comes, jump +1 extra block ahead
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

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::AddedStringInTable, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoRemovedStringInTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();
     MctStringChangeInTable *changeEntity = dynamic_cast<MctStringChangeInTable*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);

     doc->textEditor()->setPosition(pos->getTableCellPosition(cursor));

     doc->textEditor()->insertText(changeEntity->getString());

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::RemovedStringInTable, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoMovedStringInTable(MctChange * changeNode)
 {
     //todo
     return NULL;
 }

 MctChange * MctUndoClass::undoParagraphBreak(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();
     MctParagraphBreak *changeEntity = dynamic_cast<MctParagraphBreak*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->getStartChar());
     doc->textEditor()->setPosition(cursor->position());
     doc->textEditor()->deleteChar();

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::ParagraphBreak, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoParagraphBreakInTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();
     MctParagraphBreakInTable *changeEntity = dynamic_cast<MctParagraphBreakInTable*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);

     doc->textEditor()->setPosition(pos->getTableCellPosition(cursor));
     doc->textEditor()->deleteChar();

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::ParagraphBreakInTable, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoDelParagraphBreak(MctChange * changeNode)
{
     MctPosition *pos = changeNode->getPosition();
     MctDelParagraphBreak *changeEntity = dynamic_cast<MctDelParagraphBreak*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->getStartChar());
     doc->textEditor()->setPosition(cursor->position());
     doc->textEditor()->newLine();

     if(pos->getStartPar() < pos->getEndPar()) {
         ulong tmp = pos->getStartPar();
         pos->setStartPar(pos->getEndPar());
         pos->setEndPar(tmp);
         tmp = pos->getStartChar();
         pos->setStartChar(pos->getEndChar());
         pos->setEndChar(tmp);
     }

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::DelParagraphBreak, changeEntity);
     return redoChangeNode;
}

 MctChange * MctUndoClass::undoDelParagraphBreakInTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();
     MctDelParagraphBreakInTable *changeEntity = dynamic_cast<MctDelParagraphBreakInTable*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);

     doc->textEditor()->setPosition(pos->getTableCellPosition(cursor));

     doc->textEditor()->newLine();

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::DelParagraphBreakInTable, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoStyleChange(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();
     MctStylePropertyChange *changeEntity = dynamic_cast<MctStylePropertyChange*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->getStartChar());
     doc->textEditor()->setPosition(cursor->position());
     cursor->setPosition(blockpos + pos->getEndChar(), QTextCursor::KeepAnchor);
     doc->textEditor()->setPosition(cursor->position(), QTextCursor::KeepAnchor);

     undoPropsChange(doc->textEditor()->cursor(), changeEntity->getTextPropChanges());
     undoPropsChange(doc->textEditor()->cursor(), changeEntity->getParagraphPropChanges());
     undoPropsChange(doc->textEditor()->cursor(), changeEntity->getListPropChanges());
     undoPropsChange(doc->textEditor()->cursor(), changeEntity->getOtherPropChanges());

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::StyleChange, changeEntity);
     return redoChangeNode;
 }

 void MctUndoClass::undoPropsChange(QTextCursor *cursor, ChangeEventList * propchanges)
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
            if(textList) {
                QTextListFormat currentformat = textList->format();
                QTextFormat prevformat = ChangeEvent::getNewValuesForFormat(currentformat, format);
                ChangeEvent::printProperties(currentformat);
                ChangeEvent::printProperties(prevformat);
                ChangeEvent::printProperties(listformat);

                if(prevformat.properties().empty()) {
                // only ListIndent prop contained
                    if(listformat.propertyCount() == 1 && listformat.hasProperty(QTextFormat::ListIndent)) {
                        KoList::remove(cursor->block());
                        prevformat = currentformat;                        
                    }
                } else if(listformat.hasProperty(QTextFormat::ListStyle) && listformat.property(QTextFormat::ListStyle).toString() == ""){
                    KoList::remove(cursor->block());
                    prevformat = currentformat;
                    change->setOldFormat(QTextListFormat());
                } else {
                    textList->setFormat(listformat);
                }

                change->setNewFormat(change->getOldFormat());
                change->setOldFormat(prevformat);
            } else {
                //TODO
            }
         }
     }
 }

 MctChange * MctUndoClass::undoStyleChangeInTable(MctChange * changeNode)
 {
     //todo
     return NULL;
 }

 MctChange * MctUndoClass::undoAddedTextGraphicObjects(MctChange * changeNode, bool withprops)
 {
     MctAddedTextGraphicObject *changeEntity = dynamic_cast<MctAddedTextGraphicObject*>(changeNode->getChangeEntity());
     MctEmbObjProperties* props = dynamic_cast<MctEmbObjProperties*>(changeEntity->getObjectProperties());

     MctPosition *pos = changeNode->getPosition();

     if (changeEntity->getName() == "changed-text-graphic-object"){
//FIXME
         /*KoShape *shape = MctStaticData::instance()->getKoDocument()->emitPosition(props->getCenterPos());
         QSizeF shapeSize = shape->size();
         if (props->getProps2Export()->contains("PrevSizeHeight")){
             shape->setSize(QSizeF(props->getProps2Export()->value("PrevSizeWidth").toDouble(), props->getProps2Export()->value("PrevSizeHeight").toDouble()));
         }

         double rotation = shape->rotation();
         if (props->getProps2Export()->contains("PrevRotation")){
             shape->rotate(360 - props->getProps2Export()->value("Rotation").toDouble() + props->getProps2Export()->value("PrevRotation").toDouble());
         }

         MctEmbObjProperties* props2 = new MctEmbObjProperties(props->getURL(), shape, shape->position(), shape->size());
         if (props->getProps2Export()->contains("PrevSizeHeight")){
             props2->addSizeChanged(shapeSize);
         }

         if (props->getProps2Export()->contains("PrevRotation")){
             props2->addRotationChanged(rotation);
         }

         QPointF position(props->getProps2Export()->value("PrevPositionX").toFloat(), props->getProps2Export()->value("PrevPositionY").toFloat());

         props->textGraphicStyleChanges(props2, shape);

         shape->setPosition(position);
         props2->setPrevPos(position);
         changeEntity->setObjectProperties(props2);*/
     } else {
         //inserted shape, else image
         if (!props->getURL().contains("/")){
//FIXME
            /*QPointF shapePos = props->getCenterPos();
            KoShape *shape = MctStaticData::instance()->getKoDocument()->emitPosition(shapePos);
            doc->textEditor()->shapeOperation(shape, REMOVED);
            QString shapeType = props->getURL();
            MctEmbObjProperties *props2 = new MctEmbObjProperties(shapeType, shape, props->getPos(), shape->size());
            props2->setURL(shapeType);
            changeEntity->setObjectProperties(props2);*/
         } else {
            KoShape *shapeImg = props->getShape();
            doc->textEditor()->shapeOperation(shapeImg, REMOVED);
         }
     }

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::AddedTextGraphicObject, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoRemovedTextGraphicObjects(MctChange * changeNode, bool withprops)
 {
     MctRemovedTextGraphicObject *changeEntity = dynamic_cast<MctRemovedTextGraphicObject*>(changeNode->getChangeEntity());
     MctEmbObjProperties* props = dynamic_cast<MctEmbObjProperties*>(changeEntity->getObjectProperties());

     MctPosition *pos = changeNode->getPosition();

     KoShape *shape = props->getShape();

     doc->textEditor()->shapeOperation(shape, ADDED);

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::RemovedTextGraphicObject, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoAddedTextGraphicObjectsInTable(MctChange * changeNode, bool withprops)
 {
     //todo
     return NULL;
 }

 MctChange * MctUndoClass::undoRemovedTextGraphicObjectsInTable(MctChange * changeNode, bool withprops)
 {
     //todo
     return NULL;
 }

 MctChange * MctUndoClass::undoAddedTextTable(MctChange * changeNode, bool withprops)
 {// TODO: block emitting around deleteTable() !!! + check redo!
     //QSignalBlocker blocker(doc->textEditor());

     MctPosition *pos = changeNode->getPosition();

     qDebug() << pos->toString();

     MctAddedTextTable *changeEntity = dynamic_cast<MctAddedTextTable*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->getStartChar());

     qDebug() << "Cursor position: " << cursor->position() << " ->(1)[table]";

     doc->textEditor()->setPosition(cursor->position());
     doc->textEditor()->deleteChar();   // jump over the hidden block before the table
     qDebug() << "Cursor position: " << cursor->position() << " ->[table]";
     doc->textEditor()->deleteTable();

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::AddedTextTable, changeEntity);
     return redoChangeNode;

 }

 MctChange * MctUndoClass::undoRemovedTextTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();
     MctRemovedTextTable *changeEntity = dynamic_cast<MctRemovedTextTable*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->getStartChar());
     doc->textEditor()->setPosition(cursor->position());

     MctTableProperties* props = dynamic_cast<MctTableProperties*>(changeEntity->getObjectProperties());
     int rows = props->getRows();
     int cols = props->getCols();

     doc->textEditor()->insertTable(rows, cols);

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::RemovedTextTable, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoAddedTextTableInTable(MctChange * changeNode, bool withprops)
 {
     MctPosition *pos = changeNode->getPosition();

     qDebug() << pos->toString();

     MctAddedTextTableInTable *changeEntity = dynamic_cast<MctAddedTextTableInTable*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);

     doc->textEditor()->setPosition(pos->getTableCellPosition(cursor) + 1);
     //doc->textEditor()->deleteChar();   // jump over the hidden block before the table
     qDebug() << "Cursor position: " << cursor->position() << " ->[table]";
     doc->textEditor()->deleteTable();

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::AddedTextTableInTable, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoRemovedTextTableInTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->getPosition();
     MctRemovedTextTableInTable *changeEntity = dynamic_cast<MctRemovedTextTableInTable*>(changeNode->getChangeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);

     doc->textEditor()->setPosition(pos->getTableCellPosition(cursor));

     MctTableProperties* props = dynamic_cast<MctTableProperties*>(changeEntity->getObjectProperties());
     int rows = props->getRows();
     int cols = props->getCols();

     doc->textEditor()->insertTable(rows, cols);

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::RemovedTextTableInTable, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoAddedRowInTable(MctChange * changeNode)
 {
     //todo
     return NULL;
 }

 MctChange * MctUndoClass::undoRemovedRowInTable(MctChange * changeNode)
 {
     //todo
     return NULL;
 }

 MctChange * MctUndoClass::undoAddedColInTable(MctChange * changeNode)
 {
     //todo
     return NULL;
 }

 MctChange * MctUndoClass::undoRemovedColInTable(MctChange * changeNode)
 {
     //todo
     return NULL;
 }
