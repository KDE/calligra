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
    m_doc = kotextdoc;
}

MctUndoClass::~MctUndoClass()
{

}

MctChangeset * MctUndoClass::addChangesetToRedo(MctChangeset *changesetNode, QList<MctChange *> *redoChangeList)
{
    changesetNode->clearParents();
    changesetNode->clearChilds();

    MctAuthor *author = changesetNode->author();
    QDateTime date = changesetNode->date();
    QString comment = changesetNode->comment();

    return MctStaticData::instance()->getRedoGraph()->addChangeset(redoChangeList, author, date, comment);
}

void MctUndoClass::undoChangeset(MctChangeset *changesetNode, bool add2Graph)
{
    QList<MctChange *> *redochangelist = new QList<MctChange *> ();
    QList<MctChange *> *changelist = changesetNode->changes();

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

    MctChangeTypes changeType = changeNode->changeType();
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

 MctChange * MctUndoClass::undoAddedString(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->position();
     MctStringChange *changeEntity = dynamic_cast<MctStringChange*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->startChar());
     m_doc->textEditor()->setPosition(cursor->position());

     cursor->setPosition(blockpos + pos->endChar(), QTextCursor::KeepAnchor);
     m_doc->textEditor()->setPosition(cursor->position(), QTextCursor::KeepAnchor);

     QString string = cursor->selectedText();
     changeEntity->setString(string);
     m_doc->textEditor()->deleteChar();
//     cursor->removeSelectedText();

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::AddedString, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoRemovedString(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->position();
     MctStringChange *changeEntity = dynamic_cast<MctStringChange*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->startChar());

     m_doc->textEditor()->setPosition(cursor->position());
     m_doc->textEditor()->insertText(changeEntity->getString());
     //cursor->insertText(changeEntity->getString());

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::RemovedString, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoMovedString(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->position();
     MctPosition *movedpos = changeNode->movedPosition();
     MctStringChange *changeEntity = dynamic_cast<MctStringChange*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, movedpos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + movedpos->startChar());
     m_doc->textEditor()->setPosition(cursor->position());
     cursor->setPosition(blockpos + movedpos->endChar(), QTextCursor::KeepAnchor);
     m_doc->textEditor()->setPosition(cursor->position(), QTextCursor::KeepAnchor);
     //cursor->removeSelectedText();
     m_doc->textEditor()->deleteChar();

     cursor = createcursor(changeNode, pos);
     blockpos = cursor->block().position();
     cursor->setPosition(blockpos + pos->startChar());
     m_doc->textEditor()->setPosition(cursor->position());
     //cursor->insertText(changeEntity->getString());
     m_doc->textEditor()->insertText(changeEntity->getString());

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::MovedString, changeEntity, movedpos);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoAddedStringInTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->position();
     MctStringChangeInTable *changeEntity = dynamic_cast<MctStringChangeInTable*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     //táblázat kezdő blokja
     int blockpos  = cursor->block().position();
     MctPosition *tmp = pos;
     QTextCursor tmpcursor(*cursor);
     QTextTable * table;
     while(tmp->anchoredPos()) {
         blockpos  = tmpcursor.block().position();
         bool hiddenTableHandling = tmpcursor.blockFormat().hasProperty(KoParagraphStyle::HiddenByTable);
         table = tmpcursor.currentTable();
         if(hiddenTableHandling) {
             tmpcursor.movePosition(QTextCursor::NextCharacter);
             table = tmpcursor.currentTable();
         }
         QTextTableCell cell = table->cellAt(tmp->startCellInfo()->row(), tmp->startCellInfo()->col());
         tmpcursor = cell.firstCursorPosition();
         tmp = tmp->anchoredPos();
         // if nested table comes, jump +1 extra block ahead
         int k = tmp->anchoredPos() ? 1 : 0;
         tmpcursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, k + tmp->startPar());
         tmpcursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, tmp->startChar());
     }

     m_doc->textEditor()->setPosition(tmpcursor.position());

     blockpos  = tmpcursor.block().position();

     tmpcursor.setPosition(blockpos + tmp->endChar(), QTextCursor::KeepAnchor);
     m_doc->textEditor()->setPosition(tmpcursor.position(), QTextCursor::KeepAnchor);

     QString string = tmpcursor.selectedText();
     changeEntity->setString(string);

     m_doc->textEditor()->deleteChar();

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::AddedStringInTable, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoRemovedStringInTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->position();
     MctStringChangeInTable *changeEntity = dynamic_cast<MctStringChangeInTable*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);

     m_doc->textEditor()->setPosition(pos->tableCellPosition(cursor));

     m_doc->textEditor()->insertText(changeEntity->getString());

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
     MctPosition *pos = changeNode->position();
     MctParagraphBreak *changeEntity = dynamic_cast<MctParagraphBreak*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->startChar());
     m_doc->textEditor()->setPosition(cursor->position());
     m_doc->textEditor()->deleteChar();

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::ParagraphBreak, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoParagraphBreakInTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->position();
     MctParagraphBreakInTable *changeEntity = dynamic_cast<MctParagraphBreakInTable*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);

     m_doc->textEditor()->setPosition(pos->tableCellPosition(cursor));
     m_doc->textEditor()->deleteChar();

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::ParagraphBreakInTable, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoDelParagraphBreak(MctChange * changeNode)
{
     MctPosition *pos = changeNode->position();
     MctDelParagraphBreak *changeEntity = dynamic_cast<MctDelParagraphBreak*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->startChar());
     m_doc->textEditor()->setPosition(cursor->position());
     m_doc->textEditor()->newLine();

     if(pos->startPar() < pos->endPar()) {
         ulong tmp = pos->startPar();
         pos->setStartPar(pos->endPar());
         pos->setEndPar(tmp);
         tmp = pos->startChar();
         pos->setStartChar(pos->endChar());
         pos->setEndChar(tmp);
     }

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::DelParagraphBreak, changeEntity);
     return redoChangeNode;
}

 MctChange * MctUndoClass::undoDelParagraphBreakInTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->position();
     MctDelParagraphBreakInTable *changeEntity = dynamic_cast<MctDelParagraphBreakInTable*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);

     m_doc->textEditor()->setPosition(pos->tableCellPosition(cursor));

     m_doc->textEditor()->newLine();

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::DelParagraphBreakInTable, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoStyleChange(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->position();
     MctStylePropertyChange *changeEntity = dynamic_cast<MctStylePropertyChange*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->startChar());
     m_doc->textEditor()->setPosition(cursor->position());
     cursor->setPosition(blockpos + pos->endChar(), QTextCursor::KeepAnchor);
     m_doc->textEditor()->setPosition(cursor->position(), QTextCursor::KeepAnchor);

     undoPropsChange(m_doc->textEditor()->cursor(), changeEntity->textPropChanges());
     undoPropsChange(m_doc->textEditor()->cursor(), changeEntity->paragraphPropChanges());
     undoPropsChange(m_doc->textEditor()->cursor(), changeEntity->listPropChanges());
     undoPropsChange(m_doc->textEditor()->cursor(), changeEntity->otherPropChanges());

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::StyleChange, changeEntity);
     return redoChangeNode;
 }

 void MctUndoClass::undoPropsChange(QTextCursor *cursor, ChangeEventList * propchanges)
 {
     foreach (ChangeEvent *change, *propchanges) {
         QTextFormat format = change->oldFormat();
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
             m_doc->textEditor()->registerTrackedChange(*cursor, KoGenChange::FormatChange, kundo2_i18n("Formatting"), format, currentformat, false);

         } else if (format.isBlockFormat()) {
             QTextBlockFormat currentformat = cursor->blockFormat();
             ChangeEvent::printProperties(format);
             ChangeEvent::printProperties(currentformat);
             QTextFormat prevformat = ChangeEvent::getNewValuesForFormat(currentformat, format);
             ChangeEvent::printProperties(prevformat);

             cursor->mergeBlockFormat(format.toBlockFormat());
             ChangeEvent::printProperties(cursor->blockFormat());

             change->setNewFormat(change->oldFormat());
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

                change->setNewFormat(change->oldFormat());
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
     MctAddedTextGraphicObject *changeEntity = dynamic_cast<MctAddedTextGraphicObject*>(changeNode->changeEntity());
     MctEmbObjProperties* props = dynamic_cast<MctEmbObjProperties*>(changeEntity->objectProperties());

     MctPosition *pos = changeNode->position();

     if (changeEntity->name() == "changed-text-graphic-object"){
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
         if (!props->url().contains("/")){
//FIXME
            /*QPointF shapePos = props->getCenterPos();
            KoShape *shape = MctStaticData::instance()->getKoDocument()->emitPosition(shapePos);
            doc->textEditor()->shapeOperation(shape, REMOVED);
            QString shapeType = props->getURL();
            MctEmbObjProperties *props2 = new MctEmbObjProperties(shapeType, shape, props->getPos(), shape->size());
            props2->setURL(shapeType);
            changeEntity->setObjectProperties(props2);*/
         } else {
            KoShape *shapeImg = props->shape();
            m_doc->textEditor()->shapeOperation(shapeImg, REMOVED);
         }
     }

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::AddedTextGraphicObject, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoRemovedTextGraphicObjects(MctChange * changeNode, bool withprops)
 {
     MctRemovedTextGraphicObject *changeEntity = dynamic_cast<MctRemovedTextGraphicObject*>(changeNode->changeEntity());
     MctEmbObjProperties* props = dynamic_cast<MctEmbObjProperties*>(changeEntity->objectProperties());

     MctPosition *pos = changeNode->position();

     KoShape *shape = props->shape();

     m_doc->textEditor()->shapeOperation(shape, ADDED);

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

     MctPosition *pos = changeNode->position();

     qDebug() << pos->toString();

     MctAddedTextTable *changeEntity = dynamic_cast<MctAddedTextTable*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->startChar());

     qDebug() << "Cursor position: " << cursor->position() << " ->(1)[table]";

     m_doc->textEditor()->setPosition(cursor->position());
     m_doc->textEditor()->deleteChar();   // jump over the hidden block before the table
     qDebug() << "Cursor position: " << cursor->position() << " ->[table]";
     m_doc->textEditor()->deleteTable();

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::AddedTextTable, changeEntity);
     return redoChangeNode;

 }

 MctChange * MctUndoClass::undoRemovedTextTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->position();
     MctRemovedTextTable *changeEntity = dynamic_cast<MctRemovedTextTable*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->startChar());
     m_doc->textEditor()->setPosition(cursor->position());

     MctTableProperties* props = dynamic_cast<MctTableProperties*>(changeEntity->objectProperties());
     int rows = props->rows();
     int cols = props->cols();

     m_doc->textEditor()->insertTable(rows, cols);

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::RemovedTextTable, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoAddedTextTableInTable(MctChange * changeNode, bool withprops)
 {
     MctPosition *pos = changeNode->position();

     qDebug() << pos->toString();

     MctAddedTextTableInTable *changeEntity = dynamic_cast<MctAddedTextTableInTable*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);

     m_doc->textEditor()->setPosition(pos->tableCellPosition(cursor) + 1);
     //doc->textEditor()->deleteChar();   // jump over the hidden block before the table
     qDebug() << "Cursor position: " << cursor->position() << " ->[table]";
     m_doc->textEditor()->deleteTable();

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::AddedTextTableInTable, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoRemovedTextTableInTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->position();
     MctRemovedTextTableInTable *changeEntity = dynamic_cast<MctRemovedTextTableInTable*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);

     m_doc->textEditor()->setPosition(pos->tableCellPosition(cursor));

     MctTableProperties* props = dynamic_cast<MctTableProperties*>(changeEntity->objectProperties());
     int rows = props->rows();
     int cols = props->cols();

     m_doc->textEditor()->insertTable(rows, cols);

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::RemovedTextTableInTable, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctUndoClass::undoAddedRowInTable(MctChange * changeNode)
 {
     // TODO
     return NULL;
 }

 MctChange * MctUndoClass::undoRemovedRowInTable(MctChange * changeNode)
 {
     // TODO
     return NULL;
 }

 MctChange * MctUndoClass::undoAddedColInTable(MctChange * changeNode)
 {
     // TODO
     return NULL;
 }

 MctChange * MctUndoClass::undoRemovedColInTable(MctChange * changeNode)
 {
     // TODO
     return NULL;
 }
