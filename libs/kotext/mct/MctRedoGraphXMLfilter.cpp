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
//#include "MctStaticData.h"
#include "MctChangeEntities.h"
#include "MctChange.h"
#include "MctChangeset.h"
#include "MctPosition.h"
#include "MctStaticData.h"
#include "KoDocument.h"
#include "MctEmbObjProperties.h"

MctRedoGraphXMLfilter::MctRedoGraphXMLfilter(const QString &redoOrUndo, const QString &odt, KoTextDocument *koTextDoc)
    : MctUndoGraphXMLfilter(redoOrUndo, odt, koTextDoc)
{

}

MctRedoGraphXMLfilter::~MctRedoGraphXMLfilter()
{

}

void MctRedoGraphXMLfilter::addChangeFromXML(const QDomNode &node, MctChangeset* changeset)
{
    QDomElement change = node.toElement();
    MctChange *changeNode = NULL;
    if(change.tagName() == MctStaticData::ADDED && change.attribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC)) == MctStaticData::STRING){
        changeNode = addStringFromXML(change);
    } else if (change.tagName() == MctStaticData::REMOVED && change.attribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC)) == MctStaticData::STRING) {
        changeNode = removeStringFromXML(change);
    } else if (change.tagName() == MctStaticData::MOVED && change.attribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC)) == MctStaticData::STRING) {
        changeNode = moveStringFromXML(change);
    } else if (change.tagName() == MctStaticData::ADDED && change.attribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC)) == MctStaticData::PARAGRAPH) {
        changeNode = addParBreakFromXML(change);
    } else if (change.tagName() == MctStaticData::REMOVED && change.attribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC)) == MctStaticData::PARAGRAPH) {
        changeNode = delParBreakFromXML(change);    //TODO: pythonban itt removedParBreakFromXML szerepelt
    } else if (change.tagName() == MctStaticData::FORMATTAG) {
        changeNode = styleChangeFromXML(change);
    } else if (change.tagName() == MctStaticData::ADDED && change.attribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC)) == MctStaticData::TEXTFRAME) {
        changeNode = addTextFrameFromXML(change);
    } else if (change.tagName() == MctStaticData::REMOVED && change.attribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC)) == MctStaticData::TEXTFRAME) {
        return;
    } else if (change.tagName() == MctStaticData::ADDED && change.attribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC)) == MctStaticData::TEXTGRAPHICOBJECT) {
        changeNode = addTextGraphicObjectFromXML(change);
    } else if (change.tagName() == MctStaticData::REMOVED && change.attribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC)) == MctStaticData::TEXTGRAPHICOBJECT) {
        changeNode = removeTextGraphicObjectFromXML(change);
    } else if (change.tagName() == MctStaticData::ADDED && change.attribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC)) == MctStaticData::EMBEDDEDOBJECT) {
        changeNode = addEmbeddedObjectFromXML(change);
    } else if (change.tagName() == MctStaticData::REMOVED && change.attribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC)) == MctStaticData::EMBEDDEDOBJECT) {
        return;
    } else if (change.tagName() == MctStaticData::ADDED && change.attribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC)) == MctStaticData::TABLE) {
        changeNode = addTextTableFromXML(change);
    } else if (change.tagName() == MctStaticData::REMOVED && change.attribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC)) == MctStaticData::TABLE) {
        changeNode = removeTextTableFromXML(change);
    } else if (change.attribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC)) == MctStaticData::ROWCHANGE) {
        changeNode = rowChangeFromXML(change);
    } else if (change.attribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC)) == MctStaticData::COLCHANGE) {
        changeNode = colChangeFromXML(change);
    } else {
        return;
    }

    changeNode->addElementTreeNode(change);
    changeset->addChange(changeNode);
}

void MctRedoGraphXMLfilter::particularStyleChangeFromXML(const QDomNode &node, ChangeEventList * propchanges)
{
    QDomNamedNodeMap attribs = node.attributes();
    QString strtype = attribs.namedItem(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC)).toAttr().value();
    int type = QTextFormat::InvalidFormat;
    if(strtype == MctStaticData::TEXTTAG) {
        type = QTextFormat::CharFormat;
    } else if(strtype == MctStaticData::PARAGRAPHTAG) {
        type = QTextFormat::BlockFormat;
    } else if(strtype == MctStaticData::LIST) {
        type = QTextFormat::ListFormat;
    }
    if(strtype == MctStaticData::LIST) {
        listChangesFromXML(attribs, propchanges);
        return;
    }
    QTextFormat oldformat(type);
    QTextFormat newformat(type);

    for(uint i = 0 ; i < attribs.length(); i++) {
        QDomAttr attr = attribs.item(i).toAttr();
        if(!attr.isNull()) {
            if(attr.name() == MctStaticData::attributeNS(MctStaticData::ID, MctStaticData::NS_C)
                    || attr.name() == MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC)
                    || attr.name() == MctStaticData::attributeNS(MctStaticData::POSEND, MctStaticData::NS_C)
                    || attr.name() == MctStaticData::attributeNS(MctStaticData::POSSTART, MctStaticData::NS_DC)) {
                continue;
            }
            QVariant v;
            QString attrName = attr.name(); // FIXME: probaly XMLNS extension is required later
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

MctChange* MctRedoGraphXMLfilter::addTextFrameFromXML(const QDomElement &change)
{
    QString name = change.attribute("name");
    MctChangeTypes changeType = MctChangeTypes::AddedTextFrame;
    MctNode* changeEntity = new MctAddedTextFrame();
    MctPosition* pos = getPosFromXML(change);

    MctChange* changeNode = new MctChange(pos, changeType, changeEntity);
    return changeNode;
}

MctChange* MctRedoGraphXMLfilter::addTextGraphicObjectFromXML(const QDomElement &change)
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

MctChange* MctRedoGraphXMLfilter::removeTextGraphicObjectFromXML(const QDomElement &change)
{
    QString name = change.attribute("name");
    MctEmbObjProperties* cembObjProps = createEmbObjProps(change);

    MctChangeTypes changeType = MctChangeTypes::RemovedTextGraphicObject;
    MctNode* changeEntity = new MctRemovedTextGraphicObject(name, cembObjProps);
    MctPosition* pos = getPosFromXML(change);

    MctChange* changeNode = new MctChange(pos, changeType, changeEntity);
    return changeNode;
}

MctChange* MctRedoGraphXMLfilter::addEmbeddedObjectFromXML(const QDomElement &change)
{
    QString name = change.attribute("name");
    MctChangeTypes changeType = MctChangeTypes::AddedEmbeddedObject;
    MctNode* changeEntity = new MctAddedEmbeddedObject(name);
    MctPosition* pos = getPosFromXML(change);

    MctChange* changeNode = new MctChange(pos, changeType, changeEntity);
    return changeNode;
}

MctChange* MctRedoGraphXMLfilter::rowChangeFromXML(const QDomElement &change)
{
    //determine whether the string change was in table
    MctPosition* pos = getPosFromXML(change);
    MctChangeTypes changeType;
    MctNode* changeEntity;

    QString tableName = pos->startCellInfo()->tableName();
    int startrow = pos->startCellInfo()->row();
    int endrow = pos->endCellInfoEnd()->row();
    int rownum = endrow - startrow + 1;

    if(change.tagName() == MctStaticData::ADDED) {
        changeType = MctChangeTypes::AddedRowInTable;
        changeEntity = new MctAddedRowInTable(startrow, rownum, tableName);
    } else {
        changeType = MctChangeTypes::RemovedRowInTable;
        changeEntity = new MctRemovedRowInTable(startrow, rownum, tableName);
    }

    MctChange* changeNode = new MctChange(NULL, changeType, changeEntity);
    return changeNode;
}

MctChange* MctRedoGraphXMLfilter::colChangeFromXML(const QDomElement &change)
{
    //determine whether the string change was in table
    MctPosition* pos = getPosFromXML(change);
    MctChangeTypes changeType;
    MctNode* changeEntity;

    QString tableName = pos->startCellInfo()->tableName();
    int startcol = pos->startCellInfo()->col();
    int endcol = pos->endCellInfoEnd()->col();
    int colnum = endcol - startcol + 1;

    if(change.tagName() == MctStaticData::ADDED) {
        changeType = MctChangeTypes::AddedColInTable;
        changeEntity = new MctAddedColInTable(startcol, colnum, tableName);
    } else {
        changeType = MctChangeTypes::RemovedColInTable;
        changeEntity = new MctRemovedColInTable(startcol, colnum, tableName);
    }

    MctChange* changeNode = new MctChange(NULL, changeType, changeEntity);
    return changeNode;
}
