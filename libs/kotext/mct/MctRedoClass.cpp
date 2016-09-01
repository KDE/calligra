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
    m_doc = kotextdoc;
}

MctRedoClass::~MctRedoClass()
{

}

MctChangeset * MctRedoClass::addChangesetToUndo(MctChangeset *changesetNode, QList<MctChange *> *undoChangeList)
{
    changesetNode->clearParents();
    changesetNode->clearChilds();

    MctAuthor *author = changesetNode->author();
    QDateTime date = changesetNode->date();
    QString comment = changesetNode->comment();

    return MctStaticData::instance()->getUndoGraph()->addChangeset(undoChangeList, author, date, comment);
}

void MctRedoClass::redoChangeset(MctChangeset *changesetNode, bool add2Graph)
{
    QList<MctChange *> *undoChangeList = new QList<MctChange *> ();
    QList<MctChange *> *changelist = changesetNode->changes();

    foreach (MctChange * change, *changelist) {
        MctChange* undochangeNode = redoChange(change);
        undoChangeList->append(undochangeNode);
    }

    if(add2Graph) {
        addChangesetToUndo(changesetNode, undoChangeList);
    }
}

MctChange* MctRedoClass::redoChange(MctChange *changeNode)
{
    MctChange* undoChangeNode = NULL;
    if(changeNode == NULL) {
        return undoChangeNode;
    }

    //Redoing the change

    MctChangeTypes changeType = changeNode->changeType();
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

 MctChange * MctRedoClass::redoAddedString(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->position();
     MctStringChange *changeEntity = dynamic_cast<MctStringChange*>(changeNode->changeEntity());
     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->startChar());

     m_doc->textEditor()->setPosition(cursor->position());
     m_doc->textEditor()->insertText(changeEntity->getString());
     //cursor->insertText(changeEntity->getString());

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::AddedString, changeEntity);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoRemovedString(MctChange * changeNode)
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
     //cursor->removeSelectedText();
     m_doc->textEditor()->deleteChar();

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::RemovedString, changeEntity);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoMovedString(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->position();
     MctPosition *movedpos = changeNode->movedPosition();
     MctStringChange *changeEntity = dynamic_cast<MctStringChange*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->startChar());
     m_doc->textEditor()->setPosition(cursor->position());
     cursor->setPosition(blockpos + pos->endChar(), QTextCursor::KeepAnchor);
     m_doc->textEditor()->setPosition(cursor->position(), QTextCursor::KeepAnchor);
     //cursor->removeSelectedText();
     m_doc->textEditor()->deleteChar();

     cursor = createcursor(changeNode, movedpos);
     blockpos = cursor->block().position();
     cursor->setPosition(blockpos + movedpos->startChar());
     m_doc->textEditor()->setPosition(cursor->position());
     //cursor->insertText(changeEntity->getString());
     m_doc->textEditor()->insertText(changeEntity->getString());

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::MovedString, changeEntity, movedpos);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoAddedStringInTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->position();
     MctStringChangeInTable *changeEntity = dynamic_cast<MctStringChangeInTable*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);

     m_doc->textEditor()->setPosition(pos->tableCellPosition(cursor));

     m_doc->textEditor()->insertText(changeEntity->getString());

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::AddedStringInTable, changeEntity);
     return undoChangeNode;

 }

 MctChange * MctRedoClass::redoRemovedStringInTable(MctChange * changeNode)
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
         // if a nested table comes, jump +1 extra block ahead
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
     MctPosition *pos = changeNode->position();
     MctParagraphBreak *changeEntity = dynamic_cast<MctParagraphBreak*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->startChar());
     m_doc->textEditor()->setPosition(cursor->position());
     m_doc->textEditor()->newLine();

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::ParagraphBreak, changeEntity);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoParagraphBreakInTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->position();
     MctParagraphBreakInTable *changeEntity = dynamic_cast<MctParagraphBreakInTable*>(changeNode->changeEntity());
     QTextCursor *cursor = createcursor(changeNode, pos);

     m_doc->textEditor()->setPosition(pos->tableCellPosition(cursor));

     m_doc->textEditor()->newLine();

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::ParagraphBreakInTable, changeEntity);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoDelParagraphBreak(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->position();
     MctDelParagraphBreak *changeEntity = dynamic_cast<MctDelParagraphBreak*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->startChar());
     m_doc->textEditor()->setPosition(cursor->position());
     m_doc->textEditor()->deletePreviousChar();

     if(pos->startPar() > pos->endPar()) {
         ulong tmp = pos->startPar();
         pos->setStartPar(pos->endPar());
         pos->setEndPar(tmp);
         tmp = pos->startChar();
         pos->setStartChar(pos->endChar());
         pos->setEndChar(tmp);
     }

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::DelParagraphBreak, changeEntity);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoDelParagraphBreakInTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->position();
     MctDelParagraphBreakInTable *changeEntity = dynamic_cast<MctDelParagraphBreakInTable*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);

     m_doc->textEditor()->setPosition(pos->tableCellPosition(cursor));
     m_doc->textEditor()->deleteChar();

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::DelParagraphBreakInTable, changeEntity);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoStyleChange(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->position();
     MctStylePropertyChange *changeEntity = dynamic_cast<MctStylePropertyChange*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->startChar());
     m_doc->textEditor()->setPosition(cursor->position());
     cursor->setPosition(blockpos + pos->endChar(), QTextCursor::KeepAnchor);
     m_doc->textEditor()->setPosition(cursor->position(), QTextCursor::KeepAnchor);

     redoPropsChange(m_doc->textEditor()->cursor(), changeEntity->textPropChanges());
     redoPropsChange(m_doc->textEditor()->cursor(), changeEntity->paragraphPropChanges());
     redoPropsChange(m_doc->textEditor()->cursor(), changeEntity->listPropChanges());
     redoPropsChange(m_doc->textEditor()->cursor(), changeEntity->otherPropChanges());

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::StyleChange, changeEntity);
     return undoChangeNode;
 }

 void MctRedoClass::redoPropsChange(QTextCursor *cursor, ChangeEventList * propchanges)
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
                 KoListStyle *listStyle = m_doc->styleManager()->defaultListStyle()->clone();
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
                 KoList *newList = new KoList(m_doc->document(), listStyle);
                 newList->add(cursor->block(), 1);

                if(prevformat.propertyCount() == 0) {
                    prevformat = QTextListFormat();
                }
             }

             change->setNewFormat(change->oldFormat());
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
     MctAddedTextGraphicObject *changeEntity = dynamic_cast<MctAddedTextGraphicObject*>(changeNode->changeEntity());
     MctEmbObjProperties* props = dynamic_cast<MctEmbObjProperties*>(changeEntity->objectProperties());

     MctPosition *pos = changeNode->position();

     KoShape *shape = props->shape();

     if (changeEntity->name() == "changed-text-graphic-object"){
         QSizeF shapeSize = shape->size();
         if (props->props2Export()->contains("PrevSizeHeight")){
             shape->setSize(QSizeF(props->props2Export()->value("PrevSizeWidth").toDouble(), props->props2Export()->value("PrevSizeHeight").toDouble()));
         }

         double rotation = shape->rotation();
         if (props->props2Export()->contains("PrevRotation")){
             shape->rotate(360 - props->props2Export()->value("Rotation").toDouble() + props->props2Export()->value("PrevRotation").toDouble());
         }

         QPointF position(props->props2Export()->value("PositionX").toFloat(), props->props2Export()->value("PositionY").toFloat());
         QPointF prevPosition(props->props2Export()->value("PrevPositionX").toFloat(), props->props2Export()->value("PrevPositionY").toFloat());
         QPointF actPos = shape->position();
         if (prevPosition != actPos) position = prevPosition;
         shape->setPosition(position);
         MctEmbObjProperties *props2 = new MctEmbObjProperties(props->url(), shape, position, shape->size());

         if (props->props2Export()->contains("PrevSizeHeight")) {
             props2->addSizeChanged(shapeSize);
         }

         if (props->props2Export()->contains("PrevRotation")){
             props2->addRotationChanged(rotation);
         }

         props->textGraphicStyleChanges(props2, shape);

         props2->setPrevPos(actPos);
         changeEntity->setObjectProperties(props2);
     } else {
         m_doc->textEditor()->shapeOperation(shape, ADDED);
     }

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::AddedTextGraphicObject, changeEntity);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoRemovedTextGraphicObjects(MctChange * changeNode, bool withprops)
 {
     MctRemovedTextGraphicObject *changeEntity = dynamic_cast<MctRemovedTextGraphicObject*>(changeNode->changeEntity());
     MctEmbObjProperties* props = dynamic_cast<MctEmbObjProperties*>(changeEntity->objectProperties());

     MctPosition *pos = changeNode->position();

     //inserted shape, else image
     if (!props->url().contains("/")){
//FIXME
        //KoShape *shape = MctStaticData::instance()->getKoDocument()->emitPosition(props->getCenterPos());
        //doc->textEditor()->shapeOperation(shape, REMOVED);
     } else {
        KoShape *shapeImg = props->shape();
        m_doc->textEditor()->shapeOperation(shapeImg, REMOVED);
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
     MctPosition *pos = changeNode->position();
     MctAddedTextTable *changeEntity = dynamic_cast<MctAddedTextTable*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->startChar());
     m_doc->textEditor()->setPosition(cursor->position());

     MctTableProperties* props = dynamic_cast<MctTableProperties*>(changeEntity->objectProperties());
     int rows = props->rows();
     int cols = props->cols();

     m_doc->textEditor()->insertTable(rows, cols);

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::AddedTextTable, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctRedoClass::redoRemovedTextTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->position();

     qDebug() << pos->toString();

     MctRemovedTextTable *changeEntity = dynamic_cast<MctRemovedTextTable*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);
     int blockpos  = cursor->block().position();
     cursor->setPosition(blockpos + pos->startChar());

     qDebug() << "Cursor position: " << cursor->position() << " ->(1)[table]";

     m_doc->textEditor()->setPosition(cursor->position());
     m_doc->textEditor()->deleteChar();   // jump over the hidden block before the table
     qDebug() << "Cursor position: " << cursor->position() << " ->[table]";
     m_doc->textEditor()->deleteTable();

     MctChange * redoChangeNode = new MctChange(pos, MctChangeTypes::RemovedTextTable, changeEntity);
     return redoChangeNode;
 }

 MctChange * MctRedoClass::redoAddedTextTableInTable(MctChange * changeNode, bool withprops)
 {
     MctPosition *pos = changeNode->position();
     MctAddedTextTableInTable *changeEntity = dynamic_cast<MctAddedTextTableInTable*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);

     m_doc->textEditor()->setPosition(pos->tableCellPosition(cursor));

     MctTableProperties* props = dynamic_cast<MctTableProperties*>(changeEntity->objectProperties());
     int rows = props->rows();
     int cols = props->cols();

     m_doc->textEditor()->insertTable(rows, cols);

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::AddedTextTableInTable, changeEntity);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoRemovedTextTableInTable(MctChange * changeNode)
 {
     MctPosition *pos = changeNode->position();

     qDebug() << pos->toString();

     MctRemovedTextTableInTable *changeEntity = dynamic_cast<MctRemovedTextTableInTable*>(changeNode->changeEntity());

     QTextCursor *cursor = createcursor(changeNode, pos);

     m_doc->textEditor()->setPosition(pos->tableCellPosition(cursor) + 1);
     //doc->textEditor()->deleteChar();   // jump over the hidden block before the table
     qDebug() << "Cursor position: " << cursor->position() << " ->[table]";
     m_doc->textEditor()->deleteTable();

     MctChange * undoChangeNode = new MctChange(pos, MctChangeTypes::RemovedTextTableInTable, changeEntity);
     return undoChangeNode;
 }

 MctChange * MctRedoClass::redoAddedRowInTable(MctChange * changeNode)
 {
     // TODO
     return NULL;
 }

 MctChange * MctRedoClass::redoRemovedRowInTable(MctChange * changeNode)
 {
     // TODO
     return NULL;
 }

 MctChange * MctRedoClass::redoAddedColInTable(MctChange * changeNode)
 {
     // TODO
     return NULL;
 }

 MctChange * MctRedoClass::redoRemovedColInTable(MctChange * changeNode)
 {
     // TODO
     return NULL;
 }
