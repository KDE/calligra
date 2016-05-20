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

#include "MctUndoGraphXMLfilter.h"
#include "MctStaticData.h"
#include "MctUndoTags.h"
#include "MctChange.h"
#include "MctChangeset.h"
#include "MctPosition.h"
#include "MctCell.h"
#include "MctChangeEntities.h"
#include "MctEmbObjProperties.h"
#include "KoDocument.h"

MctUndoGraphXMLfilter::MctUndoGraphXMLfilter(QString redoOrUndo, QString odt, KoTextDocument *koTextDoc): MctAbstractGraph(redoOrUndo, odt, koTextDoc)
{

}

MctUndoGraphXMLfilter::~MctUndoGraphXMLfilter()
{

}

/**
 * @brief This adds new change to the changeset node from the loaded XML tree.
 * @param change The change node in the XML
 * @param changeset The parent changeset node
 */
void MctUndoGraphXMLfilter::addChangeFromXML(QDomNode node, MctChangeset* changeset) {
    QDomElement change = node.toElement();
    MctChange *changeNode = NULL;
    if(change.tagName() == MctUndoTags::ADDED && change.attribute("type") == MctUndoTags::STRING){
        changeNode = addStringFromXML(change);
    } else if (change.tagName() == MctUndoTags::REMOVED && change.attribute("type") == MctUndoTags::STRING) {
        changeNode = removeStringFromXML(change);
    } else if (change.tagName() == MctUndoTags::MOVED && change.attribute("type") == MctUndoTags::STRING) {
        changeNode = moveStringFromXML(change);
    } else if (change.tagName() == MctUndoTags::ADDED && change.attribute("type") == MctUndoTags::PARAGRAPH) {
        changeNode = addParBreakFromXML(change);
    } else if (change.tagName() == MctUndoTags::REMOVED && change.attribute("type") == MctUndoTags::PARAGRAPH) {
        changeNode = delParBreakFromXML(change);    //TODO: pythonban itt removedParBreakFromXML szerepelt
    } else if (change.tagName() == MctUndoTags::FORMATTAG) {
        changeNode = styleChangeFromXML(change);
    } else if (change.tagName() == MctUndoTags::ADDED && change.attribute("type") == MctUndoTags::TEXTFRAME) {
        changeNode = addTextFrameFromXML(change);
    } else if (change.tagName() == MctUndoTags::REMOVED && change.attribute("type") == MctUndoTags::TEXTFRAME) {
        return;
    } else if (change.tagName() == MctUndoTags::ADDED && change.attribute("type") == MctUndoTags::TEXTGRAPHICOBJECT) {
        changeNode = addTextGraphicObjectFromXML(change);
    } else if (change.tagName() == MctUndoTags::REMOVED && change.attribute("type") == MctUndoTags::TEXTGRAPHICOBJECT) {
        changeNode = removeTextGraphicObjectFromXML(change);
    } else if (change.tagName() == MctUndoTags::ADDED && change.attribute("type") == MctUndoTags::EMBEDDEDOBJECT) {
        changeNode = addEmbeddedObjectFromXML(change);
    } else if (change.tagName() == MctUndoTags::REMOVED && change.attribute("type") == MctUndoTags::EMBEDDEDOBJECT) {
        return;
    } else if (change.tagName() == MctUndoTags::ADDED && change.attribute("type") == MctUndoTags::TABLE) {
        changeNode = addTextTableFromXML(change);
    } else if (change.tagName() == MctUndoTags::REMOVED && change.attribute("type") == MctUndoTags::TABLE) {
        changeNode = removeTextTableFromXML(change);
    } else if (change.attribute("type") == MctUndoTags::ROWCHANGE) {
        changeNode = rowChangeFromXML(change);
    } else if (change.attribute("type") == MctUndoTags::COLCHANGE) {
        changeNode = colChangeFromXML(change);
    } else {
        return;
    }

    changeNode->addElementTreeNode(change);
    changeset->addChange(changeNode);
}

/**
 * @brief This adds addedstring/addedstringInTable change to the changeset node from the loaded XML tree.
 * @param change The change node in the XML
 * @return Returns with the created change node.
 */
MctChange* MctUndoGraphXMLfilter::addStringFromXML(QDomElement change)
{
    //determine whether the string change was in table
    MctPosition* pos = getPosFromXML(change);
    QString tableName;
    QString cellName;
    MctChangeTypes changeType;
    MctNode* changeEntity;

    MctCell *cellinfo = pos->getCellInfo();
    if(cellinfo != NULL) {
        cellinfo->convertCellPos2CellName();
        tableName = cellinfo->tableName();
        cellName = cellinfo->cellName();
    }

    if(tableName.isEmpty()) {
        changeType = MctChangeTypes::AddedString;
        changeEntity = new MctStringChange(change.text());
    } else {
        changeType = MctChangeTypes::AddedStringInTable;
        changeEntity = new MctStringChangeInTable(change.text(), cellName, tableName);
    }

    MctChange* changeNode = new MctChange(pos, changeType, changeEntity);
    return changeNode;
}

/**
 * @brief This adds removedstring/removedstringInTable change to the changeset node from the loaded XML tree.
 * @param change The change node in the XML
 * @return Returns with the created change node.
 */
MctChange* MctUndoGraphXMLfilter::removeStringFromXML(QDomElement change)
{
    //determine whether the string change was in table
    MctPosition* pos = getPosFromXML(change);
    QString tableName;
    QString cellName;
    MctChangeTypes changeType;
    MctNode* changeEntity;

    MctCell *cellinfo = pos->getCellInfo();
    if(cellinfo != NULL) {
        cellinfo->convertCellPos2CellName();
        tableName = cellinfo->tableName();
        cellName = cellinfo->cellName();
    }

    if(tableName.isEmpty()) {
        changeType = MctChangeTypes::RemovedString;
        changeEntity = new MctStringChange(change.text());
    } else {
        changeType = MctChangeTypes::RemovedStringInTable;
        changeEntity = new MctStringChangeInTable(change.text(), cellName, tableName);
    }

    MctChange* changeNode = new MctChange(pos, changeType, changeEntity);
    return changeNode;
}

/**
 * @brief This adds movedstring/movedstringInTable change to the changeset node from the loaded XML tree.
 * @param change The change node in the XML
 * @return Returns with the created change node.
 */
MctChange* MctUndoGraphXMLfilter::moveStringFromXML(QDomElement change)
{
    //determine whether the string change was in table
    MctPosition* pos = getPosFromXML(change);
    QString tableName;
    QString cellName;
    MctChangeTypes changeType;
    MctNode* changeEntity;

    MctCell *cellinfo = pos->getCellInfo();
    if(cellinfo != NULL) {
        cellinfo->convertCellPos2CellName();
        tableName = cellinfo->tableName();
        cellName = cellinfo->cellName();
    }

    if(tableName.isEmpty()) {
        changeType = MctChangeTypes::MovedString;
        changeEntity = new MctStringChange(change.text());
    } else {
        changeType = MctChangeTypes::MovedStringInTable;
        changeEntity = new MctStringChangeInTable(change.text(), cellName, tableName);
    }

    MctPosition * movedpos = getPosFromXML(change, true);
    MctChange* changeNode = new MctChange(pos, changeType, changeEntity, movedpos);
    return changeNode;
}

/**
 * @brief This adds addedParagraphbreak/addedParagraphbreakInTable change to the changeset node from the loaded XML tree.
 * @param change The change node in the XML
 * @return Returns with the created change node.
 */
MctChange* MctUndoGraphXMLfilter::addParBreakFromXML(QDomElement change)
{
    //determine whether the string change was in table
    MctPosition* pos = getPosFromXML(change);
    QString tableName;
    QString cellName;
    MctChangeTypes changeType;
    MctNode* changeEntity;

    MctCell *cellinfo = pos->getCellInfo();
    if(cellinfo != NULL) {
        cellinfo->convertCellPos2CellName();
        tableName = cellinfo->tableName();
        cellName = cellinfo->cellName();
    }

    if(tableName.isEmpty()) {
        changeType = MctChangeTypes::ParagraphBreak;        
        changeEntity = new MctParagraphBreak();
    } else {
        changeType = MctChangeTypes::ParagraphBreakInTable;
        changeEntity = new MctParagraphBreakInTable(cellName, tableName);
    }

    MctChange* changeNode = new MctChange(pos, changeType, changeEntity);
    return changeNode;
}

/**
 * @brief This adds removedParagraphbreak/removedParagraphbreakInTable change to the changeset node from the loaded XML tree.
 * @param change The change node in the XML
 * @return Returns with the created change node.
 */
MctChange* MctUndoGraphXMLfilter::delParBreakFromXML(QDomElement change)
{
    //determine whether the string change was in table
    MctPosition* pos = getPosFromXML(change);
    QString tableName;
    QString cellName;
    MctChangeTypes changeType;
    MctNode* changeEntity;

    MctCell *cellinfo = pos->getCellInfo();
    if(cellinfo != NULL) {
        cellinfo->convertCellPos2CellName();
        tableName = cellinfo->tableName();
        cellName = cellinfo->cellName();
    }

    if(tableName.isEmpty()) {
        changeType = MctChangeTypes::DelParagraphBreak;
        changeEntity = new MctDelParagraphBreak();
    } else {
        changeType = MctChangeTypes::DelParagraphBreakInTable;
        changeEntity = new MctDelParagraphBreakInTable(cellName, tableName);
    }

    MctChange* changeNode = new MctChange(pos, changeType, changeEntity);
    return changeNode;
}

/**
 * @brief This adds styleChange/styleChangeInTable change to the changeset node from the loaded XML tree.
 * @param change The change node in the XML
 * @return Returns with the created change node.
 */
MctChange* MctUndoGraphXMLfilter::styleChangeFromXML(QDomElement change)
{
    //determine whether the string change was in table
    MctPosition* pos = getPosFromXML(change);
    QString tableName;
    QString cellName;
    MctChangeTypes changeType;
    MctNode* changeEntity;

    MctCell *cellinfo = pos->getCellInfo();
    if(cellinfo != NULL) {
        cellinfo->convertCellPos2CellName();
        tableName = cellinfo->tableName();
        cellName = cellinfo->cellName();
    }

    ChangeEventList * propchanges = new ChangeEventList();

    QDomNodeList subnodes = change.childNodes();
    for(uint j = 0; j < subnodes.length(); j++) {
        particularStyleChangeFromXML(subnodes.at(j), propchanges);
    }



    if(tableName.isEmpty()) {
        changeType = MctChangeTypes::StyleChange;
        changeEntity = new MctStylePropertyChange(propchanges);
    } else {
        changeType = MctChangeTypes::StyleChangeInTable;
        changeEntity = new MctStylePropertyChangeInTable(propchanges, cellName, tableName);
    }

    MctChange* changeNode = new MctChange(pos, changeType, changeEntity);
    return changeNode;
}

void MctUndoGraphXMLfilter::particularStyleChangeFromXML(QDomNode node, ChangeEventList * propchanges){
    QDomNamedNodeMap attribs = node.attributes();
    QString strtype = attribs.namedItem("type").toAttr().value();
    int type = QTextFormat::InvalidFormat;
    if(strtype == MctUndoTags::TEXTTAG) {
        type = QTextFormat::CharFormat;
    } else if(strtype == MctUndoTags::PARAGRAPHTAG) {
        type = QTextFormat::BlockFormat;
    } else if(strtype == MctUndoTags::LIST) {
        type = QTextFormat::ListFormat;
    }
    if(strtype == MctUndoTags::LIST) {
        listChangesFromXML(attribs, propchanges);
        return;
    }
    QTextFormat oldformat(type);
    QTextFormat newformat(type);

    for(uint i = 0 ; i < attribs.length(); i++) {
        QDomAttr attr = attribs.item(i).toAttr();
        if(!attr.isNull()) {
            if(attr.name() == "id" || attr.name() == "type" ||
                    attr.name() == "e" || attr.name() == "s") {
                continue;
            }
            QVariant v;
            QString attrName = attr.name();
            int propkey = MctStaticData::getTextPropAsInt(attr.name());
            if (MctStaticData::TEXT_PROPS_INT.contains(attrName)) {
                v = attr.value().toInt();
            } else if (MctStaticData::TEXT_PROPS_DOUBLE.contains(attrName)) {
                v = attr.value().toDouble();
            } else if (attrName == "ForegroundBrush" || attrName == "BackgroundBrush") {
                QBrush brush(QColor(attr.value()));
                if(attrName == "BackgroundBrush" && attr.value() == "transparent") {
                    brush.setColor(QColor("#000000"));
                    brush.setStyle(Qt::NoBrush);
                }
                v = brush;
            } else if (attrName == "TextUnderlineColor"|| attrName == "StrikeOutColor") {
                v = QColor(attr.value());
            } else {
                v = attr.value();
            }
            oldformat.setProperty(propkey, v);
        }
    }

    ChangeEvent *changeEvent = new ChangeEvent(oldformat, newformat);
    propchanges->append(changeEvent);
}

void MctUndoGraphXMLfilter::listChangesFromXML(QDomNamedNodeMap attribs, ChangeEventList * propchanges)
{
    int type = QTextFormat::ListFormat;
    QTextFormat oldformat(type);
    QTextFormat newformat(type);
    for(uint i = 0 ; i < attribs.length(); i++) {
        QDomAttr attr = attribs.item(i).toAttr();
        if(!attr.isNull()) {
            if(attr.name() == "id" || attr.name() == "type" || attr.name() == "e" || attr.name() == "s") {
                continue;
            }

            QVariant v(attr.value());
            int propkey = MctStaticData::getTextPropAsInt(attr.name());
            oldformat.setProperty(propkey, v);
        }
    }
    ChangeEvent *changeEvent = new ChangeEvent(oldformat, newformat);
    propchanges->append(changeEvent);
}

/**
 * @brief This adds addedTextFrame/addedTextFrameInTable change to the changeset node from the loaded XML tree.
 * @param change The change node in the XML
 * @return Returns with the created change node.
 */
MctChange* MctUndoGraphXMLfilter::addTextFrameFromXML(QDomElement change)
{
    QString name = change.attribute("name");
    MctChangeTypes changeType = MctChangeTypes::AddedTextFrame;
    MctNode* changeEntity = new MctAddedTextFrame();

    MctChange* changeNode = new MctChange(NULL, changeType, changeEntity);
    return changeNode;
}

/**
 * @brief This adds addedTextGraphicObject/addedTextGraphicObjectInTable change to the changeset node from the loaded XML tree.
 * @param change The change node in the XML
 * @return Returns with the created change node.
 */
MctChange* MctUndoGraphXMLfilter::addTextGraphicObjectFromXML(QDomElement change)
{
    QString name = change.attribute("name");
    MctEmbObjProperties* cembObjProps = createEmbObjProps(change);
    //ensure that the embedded file is backed up
    if(!cembObjProps->isBackedup()) {
        cembObjProps->backupFileFromOdt(cembObjProps->getInnerUrl(), m_odtFile);
    }

    MctChangeTypes changeType = MctChangeTypes::AddedTextGraphicObject;
    MctNode* changeEntity = new MctAddedTextGraphicObject(name, cembObjProps);

    MctPosition* pos = getPosFromXML(change);

    MctChange* changeNode = new MctChange(pos, changeType, changeEntity);
    return changeNode;
}

/**
 * @brief This adds removedTextGraphicObject/removedTextGraphicObjectInTable change to the changeset node from the loaded XML tree.
 * @param change The change node in the XML
 * @return Returns with the created change node.
 */
MctChange* MctUndoGraphXMLfilter::removeTextGraphicObjectFromXML(QDomElement change)
{
    QString name = change.attribute("name");
    MctEmbObjProperties* cembObjProps = NULL;

    if (change.attribute("URL").contains("/")){
        QSizeF size(change.attribute("Width").toDouble(), change.attribute("Height").toDouble());
        QPointF pos(change.attribute("PositionX").toDouble(), change.attribute("PositionY").toDouble());
//FIXME
        //cembObjProps = new MctEmbObjProperties(name, MctStaticData::instance()->getKoDocument()->emitCreateShapeFromXML(change), pos, size);
        //cembObjProps->setURL(change.attribute("URL"));
    } else {
        cembObjProps = createEmbObjShapeProps(change);
    }
    //ensure that the embedded file is backed up
    if(!cembObjProps->isBackedup()) {
        cembObjProps->backupFileFromOdt(cembObjProps->getInnerUrl(), m_odtFile);
    }

    MctChangeTypes changeType = MctChangeTypes::RemovedTextGraphicObject;
    MctNode* changeEntity = new MctRemovedTextGraphicObject(name, cembObjProps);
    MctPosition* pos = getPosFromXML(change);

    MctChange* changeNode = new MctChange(pos, changeType, changeEntity);
    return changeNode;
}

/**
 * @brief This adds addedEmbeddedObject/addedEmbeddedObjectInTable change to the changeset node from the loaded XML tree.
 * @param change The change node in the XML
 * @return Returns with the created change node.
 */
MctChange* MctUndoGraphXMLfilter::addEmbeddedObjectFromXML(QDomElement change)
{
    QString name = change.attribute("name");
    MctChangeTypes changeType = MctChangeTypes::AddedEmbeddedObject;
    MctNode* changeEntity = new MctAddedEmbeddedObject(name);

    MctChange* changeNode = new MctChange(NULL, changeType, changeEntity);
    return changeNode;
}

/**
 * @brief This adds addedTextTable/addedTextTableInTable change to the changeset node from the loaded XML tree.
 * @param change The change node in the XML
 * @return Returns with the created change node.
 */
MctChange* MctUndoGraphXMLfilter::addTextTableFromXML(QDomElement change)
{
    QString name = change.attribute("name");
    MctTableProperties* ctableProps = createTableProps(change);

    //determine whether the string change was in table
    MctPosition* pos = getPosFromXML(change);
    QString tableName;
    QString cellName;
    MctChangeTypes changeType;
    MctNode* changeEntity;

    MctCell *cellinfo = pos->getCellInfo();
    if(cellinfo != NULL) {
        cellinfo->convertCellPos2CellName();
        tableName = cellinfo->tableName();
        cellName = cellinfo->cellName();
    }

    if(tableName.isEmpty()) {
        changeType = MctChangeTypes::AddedTextTable;
        changeEntity = new MctAddedTextTable(name, ctableProps);
    } else {
        changeType = MctChangeTypes::AddedTextTableInTable;
        changeEntity = new MctAddedTextTableInTable(name, ctableProps, cellName, tableName);
    }

    MctChange* changeNode = new MctChange(pos, changeType, changeEntity);
    return changeNode;
}

/**
 * @brief This adds removedTextTable/removedTextTableInTable change to the changeset node from the loaded XML tree.
 * @param change The change node in the XML
 * @return Returns with the created change node.
 */
MctChange* MctUndoGraphXMLfilter::removeTextTableFromXML(QDomElement change)
{
    QString name = change.attribute("name");
    MctTableProperties* ctableProps = createTableProps(change);

    //determine whether the string change was in table
    MctPosition* pos = getPosFromXML(change);
    QString tableName;
    QString cellName;
    MctChangeTypes changeType;
    MctNode* changeEntity;

    MctCell *cellinfo = pos->getCellInfo();
    if(cellinfo != NULL) {
        cellinfo->convertCellPos2CellName();
        tableName = cellinfo->tableName();
        cellName = cellinfo->cellName();
    }

    if(tableName.isEmpty()) {
        changeType = MctChangeTypes::RemovedTextTable;
        changeEntity = new MctRemovedTextTable(name, ctableProps);
    } else {
        changeType = MctChangeTypes::RemovedTextTableInTable;
        changeEntity = new MctRemovedTextTableInTable(name, ctableProps, cellName, tableName);
    }

    MctChange* changeNode = new MctChange(pos, changeType, changeEntity);
    return changeNode;
}

/**
 * @brief This adds addedRowInTable/removedRowInTable change to the changeset node from the loaded XML tree.
 * @param change The change node in the XML
 * @return Returns with the created change node.
 */
MctChange* MctUndoGraphXMLfilter::rowChangeFromXML(QDomElement change)
{
    //determine whether the string change was in table
    MctPosition* pos = getPosFromXML(change);
    MctChangeTypes changeType;
    MctNode* changeEntity;

    QString tableName = pos->getCellInfo()->tableName();
    int startrow = pos->getCellInfo()->row();
    int endrow = pos->getCellInfoEnd()->row();
    int rownum = endrow - startrow + 1;

    if(change.tagName() == MctUndoTags::ADDED) {
        changeType = MctChangeTypes::AddedRowInTable;
        changeEntity = new MctAddedRowInTable(startrow, rownum, tableName);
    } else {
        changeType = MctChangeTypes::RemovedRowInTable;
        changeEntity = new MctRemovedRowInTable(startrow, rownum, tableName);
    }

    MctChange* changeNode = new MctChange(NULL, changeType, changeEntity);
    return changeNode;
}

/**
 * @brief This adds addedRowInTable/removedRowInTable change to the changeset node from the loaded XML tree.
 * @param change The change node in the XML
 * @return Returns with the created change node.
 */
MctChange* MctUndoGraphXMLfilter::colChangeFromXML(QDomElement change)
{
    //determine whether the string change was in table
    MctPosition* pos = getPosFromXML(change);
    MctChangeTypes changeType;
    MctNode* changeEntity;

    QString tableName = pos->getCellInfo()->tableName();
    int startcol = pos->getCellInfo()->col();
    int endcol = pos->getCellInfoEnd()->col();
    int colnum = endcol - startcol + 1;

    if(change.tagName() == MctUndoTags::ADDED) {
        changeType = MctChangeTypes::AddedColInTable;
        changeEntity = new MctAddedColInTable(startcol, colnum, tableName);
    } else {
        changeType = MctChangeTypes::RemovedColInTable;
        changeEntity = new MctRemovedColInTable(startcol, colnum, tableName);
    }

    MctChange* changeNode = new MctChange(NULL, changeType, changeEntity);
    return changeNode;
}
