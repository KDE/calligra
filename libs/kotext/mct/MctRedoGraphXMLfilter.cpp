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

#include "MctRedoGraphXMLfilter.h"
#include "MctRedoTags.h"
#include "MctChangeEntities.h"
#include "MctChange.h"
#include "MctChangeset.h"
#include "MctPosition.h"
#include "MctStaticData.h"
#include "KoDocument.h"
#include "MctEmbObjProperties.h"

MctRedoGraphXMLfilter::MctRedoGraphXMLfilter(QString redoOrUndo, QString odt, KoTextDocument *koTextDoc) : MctUndoGraphXMLfilter(redoOrUndo, odt, koTextDoc)
{

}

MctRedoGraphXMLfilter::~MctRedoGraphXMLfilter()
{

}

/**
 * @brief This adds new change to the changeset change node from the loaded XML tree to the changeset
 * @param change The change node in the XML
 * @param changeset The parent changeset node
 */
void MctRedoGraphXMLfilter::addChangeFromXML(QDomNode node, MctChangeset* changeset)
{
    QDomElement change = node.toElement();
    MctChange *changeNode = NULL;
    if(change.tagName() == MctRedoTags::ADDED && change.attribute("type") == MctRedoTags::STRING){
        changeNode = addStringFromXML(change);
    } else if (change.tagName() == MctRedoTags::REMOVED && change.attribute("type") == MctRedoTags::STRING) {
        changeNode = removeStringFromXML(change);
    } else if (change.tagName() == MctRedoTags::MOVED && change.attribute("type") == MctRedoTags::STRING) {
        changeNode = moveStringFromXML(change);
    } else if (change.tagName() == MctRedoTags::ADDED && change.attribute("type") == MctRedoTags::PARAGRAPH) {
        changeNode = addParBreakFromXML(change);
    } else if (change.tagName() == MctRedoTags::REMOVED && change.attribute("type") == MctRedoTags::PARAGRAPH) {
        changeNode = delParBreakFromXML(change);    //TODO: pythonban itt removedParBreakFromXML szerepelt
    } else if (change.tagName() == MctRedoTags::FORMATTAG) {
        changeNode = styleChangeFromXML(change);
    } else if (change.tagName() == MctRedoTags::ADDED && change.attribute("type") == MctRedoTags::TEXTFRAME) {
        changeNode = addTextFrameFromXML(change);
    } else if (change.tagName() == MctRedoTags::REMOVED && change.attribute("type") == MctRedoTags::TEXTFRAME) {
        return;
    } else if (change.tagName() == MctRedoTags::ADDED && change.attribute("type") == MctRedoTags::TEXTGRAPHICOBJECT) {
        changeNode = addTextGraphicObjectFromXML(change);
    } else if (change.tagName() == MctRedoTags::REMOVED && change.attribute("type") == MctRedoTags::TEXTGRAPHICOBJECT) {
        changeNode = removeTextGraphicObjectFromXML(change);
    } else if (change.tagName() == MctRedoTags::ADDED && change.attribute("type") == MctRedoTags::EMBEDDEDOBJECT) {
        changeNode = addEmbeddedObjectFromXML(change);
    } else if (change.tagName() == MctRedoTags::REMOVED && change.attribute("type") == MctRedoTags::EMBEDDEDOBJECT) {
        return;
    } else if (change.tagName() == MctRedoTags::ADDED && change.attribute("type") == MctRedoTags::TABLE) {
        changeNode = addTextTableFromXML(change);
    } else if (change.tagName() == MctRedoTags::REMOVED && change.attribute("type") == MctRedoTags::TABLE) {
        changeNode = removeTextTableFromXML(change);
    } else if (change.attribute("type") == MctRedoTags::ROWCHANGE) {
        changeNode = rowChangeFromXML(change);
    } else if (change.attribute("type") == MctRedoTags::COLCHANGE) {
        changeNode = colChangeFromXML(change);
    } else {
        return;
    }

    changeNode->addElementTreeNode(change);
    changeset->addChange(changeNode);
}

void MctRedoGraphXMLfilter::particularStyleChangeFromXML(QDomNode node, ChangeEventList * propchanges)
{
    QDomNamedNodeMap attribs = node.attributes();
    QString strtype = attribs.namedItem("type").toAttr().value();
    int type = QTextFormat::InvalidFormat;
    if(strtype == MctRedoTags::TEXTTAG) {
        type = QTextFormat::CharFormat;
    } else if(strtype == MctRedoTags::PARAGRAPHTAG) {
        type = QTextFormat::BlockFormat;
    } else if(strtype == MctRedoTags::LIST) {
        type = QTextFormat::ListFormat;
    }
    if(strtype == MctRedoTags::LIST) {
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

/**
 * @brief This adds addedTextFrame/addedTextFrameInTable change to the changeset node from the loaded XML tree.
 * @param change The change node in the XML
 * @return Returns with the created change node.
 */
MctChange* MctRedoGraphXMLfilter::addTextFrameFromXML(QDomElement change)
{
    QString name = change.attribute("name");
    MctChangeTypes changeType = MctChangeTypes::AddedTextFrame;
    MctNode* changeEntity = new MctAddedTextFrame();
    MctPosition* pos = getPosFromXML(change);

    MctChange* changeNode = new MctChange(pos, changeType, changeEntity);
    return changeNode;
}

/**
 * @brief This adds addedTextGraphicObject/addedTextGraphicObjectInTable change to the changeset node from the loaded XML tree.
 * @param change The change node in the XML
 * @return Returns with the created change node.
 */
MctChange* MctRedoGraphXMLfilter::addTextGraphicObjectFromXML(QDomElement change)
{
    QString name = change.attribute("name");
    MctEmbObjProperties* cembObjProps = NULL;

    if (name == "changed-text-graphic-object"){
            QSizeF size(change.attribute("Width").toDouble(), change.attribute("Height").toDouble());
            QPointF pos(change.attribute("PrevPositionX").toDouble(), change.attribute("PrevPositionY").toDouble());
            QPointF prevpos(change.attribute("PositionX").toDouble(), change.attribute("PositionY").toDouble());
            QPointF posCenter(pos.x() + (size.width()/2), pos.y() + (size.height()/2));
//FIXME
            //KoShape *shape = MctStaticData::instance()->getKoDocument()->emitPosition(posCenter);
            //cembObjProps = new MctEmbObjProperties(change.attribute("URL"), shape, pos, size);
            //cembObjProps->setPrevPos(prevpos);
    } else {
        if (change.attribute("URL").contains("/")){
            QSizeF size(change.attribute("Width").toDouble(), change.attribute("Height").toDouble());
            QPointF pos(change.attribute("PositionX").toDouble(), change.attribute("PositionY").toDouble());
//FIXME
            /*cembObjProps = new MctEmbObjProperties(name, MctStaticData::instance()->getKoDocument()->emitCreateShapeFromXML(change), pos, size);
            if (change.attribute("PrevPositionX") != ""){
                QPointF prevPos(change.attribute("PrevPositionX").toDouble(), change.attribute("PrevPositionY").toDouble());
                cembObjProps->setPrevPos(prevPos);
            }
            cembObjProps->setURL(change.attribute("URL"));*/
        } else {
            cembObjProps = createEmbObjShapeProps(change);
        }
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
MctChange* MctRedoGraphXMLfilter::removeTextGraphicObjectFromXML(QDomElement change)
{
    QString name = change.attribute("name");
    MctEmbObjProperties* cembObjProps = createEmbObjProps(change);

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
MctChange* MctRedoGraphXMLfilter::addEmbeddedObjectFromXML(QDomElement change)
{
    QString name = change.attribute("name");
    MctChangeTypes changeType = MctChangeTypes::AddedEmbeddedObject;
    MctNode* changeEntity = new MctAddedEmbeddedObject(name);
    MctPosition* pos = getPosFromXML(change);

    MctChange* changeNode = new MctChange(pos, changeType, changeEntity);
    return changeNode;
}

/**
 * @brief This adds addedRowInTable/removedRowInTable change to the changeset node from the loaded XML tree.
 * @param change The change node in the XML
 * @return Returns with the created change node.
 */
MctChange* MctRedoGraphXMLfilter::rowChangeFromXML(QDomElement change)
{
    //determine whether the string change was in table
    MctPosition* pos = getPosFromXML(change);
    MctChangeTypes changeType;
    MctNode* changeEntity;

    QString tableName = pos->startCellInfo()->tableName();
    int startrow = pos->startCellInfo()->row();
    int endrow = pos->endCellInfoEnd()->row();
    int rownum = endrow - startrow + 1;

    if(change.tagName() == MctRedoTags::ADDED) {
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
MctChange* MctRedoGraphXMLfilter::colChangeFromXML(QDomElement change)
{
    //determine whether the string change was in table
    MctPosition* pos = getPosFromXML(change);
    MctChangeTypes changeType;
    MctNode* changeEntity;

    QString tableName = pos->startCellInfo()->tableName();
    int startcol = pos->startCellInfo()->col();
    int endcol = pos->endCellInfoEnd()->col();
    int colnum = endcol - startcol + 1;

    if(change.tagName() == MctRedoTags::ADDED) {
        changeType = MctChangeTypes::AddedColInTable;
        changeEntity = new MctAddedColInTable(startcol, colnum, tableName);
    } else {
        changeType = MctChangeTypes::RemovedColInTable;
        changeEntity = new MctRemovedColInTable(startcol, colnum, tableName);
    }

    MctChange* changeNode = new MctChange(NULL, changeType, changeEntity);
    return changeNode;
}
