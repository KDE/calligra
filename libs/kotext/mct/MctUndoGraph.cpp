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

#include "MctUndoGraph.h"
#include "MctStaticData.h"
#include "MctChangeset.h"
#include "MctChange.h"
#include "MctUndoTags.h"
#include "MctChangeEntities.h"
#include "MctTableProperties.h"
#include "MctEmbObjProperties.h"
#include "KoDocument.h"

MctUndoGraph::MctUndoGraph(QString odt, KoTextDocument *koTextDoc) : MctUndoGraphXMLfilter(MctStaticData::UNDOCHANGES, odt, koTextDoc)
{
    fillUpGraph();
}

MctUndoGraph::~MctUndoGraph()
{

}

/**
 * @brief This converts and adds changeset from redo graph into undo graph
 * @param redochangeset ChangesetNode in the redo graph
 * @return Returns with the created changesetNode.
 */
MctChangeset* MctUndoGraph::addchangesetFromRedo(MctChangeset *redochangeset)
{
    redochangeset->clearParents();
    redochangeset->clearChilds();
    MctAuthor * author = redochangeset->getAuthor();
    QDateTime date = redochangeset->getDate();
    QString comment = redochangeset->getComment();
    QList<MctChange*> * changelist = redochangeset->getChanges();
    QList<MctChange*> * undochangelist = new QList<MctChange*>();

    foreach (MctChange *changeNode, *changelist) {
        MctPosition * pos = changeNode->getPosition();
        MctChangeTypes changeType = changeNode->getChangeType();
        MctNode* changeEntity = changeNode->getChangeEntity();
        MctChange * undoChangeNode = NULL;
        MctPosition * movedpos = NULL;

        if(changeType == MctChangeTypes::MovedString) {
            movedpos = changeNode->getMovedPosition();
            undoChangeNode = new MctChange(pos, changeType, changeEntity, movedpos);
        } else {
            undoChangeNode = new MctChange(pos, changeType, changeEntity);
        }
        undochangelist->append(undoChangeNode);
    }

    return addChangeset(undochangelist, author, date, comment);

}

/**
 * @brief This adds new change to the changeset
 * @param change The change node
 * @param changeset The parent changeset node
 */
void MctUndoGraph::addChange(MctChange* changeNode, MctChangeset* changeset)
{
    QDomElement change;
    QDomElement parent = changeset->getChangeSetNode();
    if(changeNode->getChangeType() == MctChangeTypes::AddedString) {
        change = doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        addString(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedStringInTable) {
        change = doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        addStringInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedString) {
        change = doc->createElement(MctUndoTags::REMOVED);
        parent.appendChild(change);
        removeString(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedStringInTable) {
        change = doc->createElement(MctUndoTags::REMOVED);
        parent.appendChild(change);
        removeStringInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::MovedString) {
        change = doc->createElement(MctUndoTags::MOVED);
        parent.appendChild(change);
        moveString(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::MovedStringInTable) {
        change = doc->createElement(MctUndoTags::MOVED);
        parent.appendChild(change);
        moveStringInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::ParagraphBreak) {
        change = doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        addParBreak(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::ParagraphBreakInTable) {
        change = doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        addParBreakInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::DelParagraphBreak) {
        change = doc->createElement(MctUndoTags::REMOVED);
        parent.appendChild(change);
        delParBreak(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::DelParagraphBreakInTable) {
        change = doc->createElement(MctUndoTags::REMOVED);
        parent.appendChild(change);
        delParBreakInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::StyleChange) {
        change = doc->createElement(MctUndoTags::FORMATTAG);
        parent.appendChild(change);
        styleChange(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::StyleChangeInTable) {
        change = doc->createElement(MctUndoTags::FORMATTAG);
        parent.appendChild(change);
        styleChangeInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedTextFrame) {
        change = doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        textFrame(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedTextFrameInTable) {
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedTextFrame) {
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedTextFrameInTable) {
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedTextGraphicObject) {
        change = doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        textGraphicObject(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedTextGraphicObjectInTable) {
        change = doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        textGraphicObjectInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedTextGraphicObject) {
        change = doc->createElement(MctUndoTags::REMOVED);
        parent.appendChild(change);
        textGraphicObject(&change, changeNode, false);
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedTextGraphicObjectInTable) {
        change = doc->createElement(MctUndoTags::REMOVED);
        parent.appendChild(change);
        textGraphicObjectInTable(&change, changeNode, false);
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedEmbeddedObject) {
        change = doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        embeddedObject(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedEmbeddedObjectInTable) {
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedEmbeddedObject) {
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedEmbeddedObjectInTable) {
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedTextTable) {
        change = doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        textTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedTextTableInTable) {
        change = doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        textTableInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedTextTable) {
        change = doc->createElement(MctUndoTags::REMOVED);
        parent.appendChild(change);
        textTable(&change, changeNode, false);
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedTextTableInTable) {
        change = doc->createElement(MctUndoTags::REMOVED);
        parent.appendChild(change);
        textTableInTable(&change, changeNode, false);
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedRowInTable) {
        change = doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        rowChangeInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedRowInTable) {
        change = doc->createElement(MctUndoTags::REMOVED);
        parent.appendChild(change);
        rowChangeInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedColInTable) {
        change = doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        colChangeInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedColInTable) {
        change = doc->createElement(MctUndoTags::REMOVED);
        parent.appendChild(change);
        colChangeInTable(&change, changeNode);
    } else {
        change = doc->createElement("unknown");
        parent.appendChild(change);
    }

    if(!change.isNull()) {
        changeNode->addElementTreeNode(change);
        changeset->addChange(changeNode);

        change.setAttribute("id", QString::number(this->_id));

        ++_id;

    } else {
        qDebug () << "Change is NULL, not added to the graph";
    }
}

/**
 * @brief This fills up added string changes to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctUndoGraph::addString(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute("type", MctUndoTags::STRING);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);
    MctStringChange * sc = dynamic_cast<MctStringChange*>(change->getChangeEntity());
    QDomText nodeText = doc->createTextNode(sc->getString());
    xmlchange->appendChild(nodeText);
}

/**
 * @brief This fills up removed string changes to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctUndoGraph::removeString(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute("type", MctUndoTags::STRING);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);
    MctStringChange * sc = dynamic_cast<MctStringChange*>(change->getChangeEntity());
    QDomText nodeText = doc->createTextNode(sc->getString());
    xmlchange->appendChild(nodeText);
}

/**
 * @brief This fills up moved string changes to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctUndoGraph::moveString(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute("type", MctUndoTags::STRING);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);

    MctPosition *movedpos = change->getMovedPosition();
    addPos2change(xmlchange, movedpos, true);

    MctStringChange * sc = dynamic_cast<MctStringChange*>(change->getChangeEntity());
    QDomText nodeText = doc->createTextNode(sc->getString());
    xmlchange->appendChild(nodeText);
}

/**
 * @brief This fills up added paragraph break changes to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctUndoGraph::addParBreak(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute("type", MctUndoTags::PARAGRAPH);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);
}

/**
 * @brief This fills up deleted paragraph break changes to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctUndoGraph::delParBreak(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute("type", MctUndoTags::PARAGRAPH);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);
}

/**
 * @brief This fills up style changes to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctUndoGraph::styleChange(QDomElement *xmlchange, MctChange *change)
{    
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);

    MctStylePropertyChange * sc = dynamic_cast<MctStylePropertyChange*>(change->getChangeEntity());
    //filling up text properties
    if(sc->getTextPropChanges()->length() > 0) {
        QDomElement subchange = doc->createElement(MctUndoTags::PROPERTIESTAG);
        subchange.setAttribute("type", MctUndoTags::TEXTTAG);
        xmlchange->appendChild(subchange);
        fillPropertySubNode(&subchange, sc->getTextPropChanges(), MctStaticData::CharacterProperty);
    }

    //filling up parapraph properties
    if(sc->getParagraphPropChanges()->length() > 0) {
        QDomElement subchange = doc->createElement(MctUndoTags::PROPERTIESTAG);
        subchange.setAttribute("type", MctUndoTags::PARAGRAPHTAG);
        xmlchange->appendChild(subchange);
        fillPropertySubNode(&subchange, sc->getParagraphPropChanges(), MctStaticData::ParagraphProperty);
    }

    //filling up list properties
    if(sc->getListPropChanges()->length() > 0) {
        QDomElement subchange = doc->createElement(MctUndoTags::PROPERTIESTAG);
        subchange.setAttribute("type", MctUndoTags::LIST);
        xmlchange->appendChild(subchange);
        fillPropertySubNode(&subchange, sc->getListPropChanges(), MctStaticData::ListProperty);
    }

    //filling up properties for other components
    if(sc->getOtherPropChanges()->length() > 0) {
        QDomElement subchange = doc->createElement(MctUndoTags::PROPERTIESTAG);
        subchange.setAttribute("type", MctUndoTags::UNDEFINEDTAG);
        xmlchange->appendChild(subchange);
        fillPropertySubNode(&subchange, sc->getOtherPropChanges(), MctStaticData::UnknownProperty);
    }
}

void MctUndoGraph::fillPropertySubNode(QDomElement *subnode, ChangeEventList* propchanges, int type)
{
    foreach (ChangeEvent* change, *propchanges) {
        change->calcDiff(type);
        foreach (int key, change->getChanges()->keys()) {
            QPair<QString, ChangeAction> value = change->getChanges()->value(key);            

            QString propName = MctStaticData::getTextPropetyString(key, type);

            qDebug() << propName << "(" << key << ") : "  << value.first;
            if(propName != "") {
                subnode->setAttribute(propName, value.first);
            } else {
                qDebug() << "Not mapped property: " << QString::number(key);
            }
        }
    }
}

/**
 * @brief This fills up added/removed TextFrame change to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctUndoGraph::textFrame(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute("type", MctUndoTags::TEXTFRAME);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);

    xmlchange->setAttribute("name", change->getChangeEntity()->getName());
}

/**
 * @brief This fills up added/removed TextTable change to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 * @param added If False other style data are also exported.
 */
void MctUndoGraph::textTable(QDomElement *xmlchange, MctChange *change, bool added)
{
    xmlchange->setAttribute("type", MctUndoTags::TABLE);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);

    MctChangedTexObjectBase * changeEntity = dynamic_cast<MctChangedTexObjectBase*>(change->getChangeEntity());
    xmlchange->setAttribute("name",changeEntity->getName());

    MctTableProperties * tableprops = dynamic_cast<MctTableProperties*>(changeEntity->getObjectProperties());
    xmlchange->setAttribute("col", tableprops->getCols());
    xmlchange->setAttribute("row", tableprops->getRows());

    if(added){
        return;
    }
    else {
        if(tableprops->getProps2Export()->size() == 0) {
            tableprops->setProps2Export(tableprops->getProps());
        }
    }

    QList<QString> keys = tableprops->getProps2Export()->keys();
    foreach (QString key, keys) {
        if( MctStaticData::TEXTTABLE_STRUCTPROPS.contains(key) ||
            MctStaticData::TEXTTABLE_ENUMPROPS.contains(key)) {
            continue;
        }
        xmlchange->setAttribute(key, tableprops->getProps2Export()->value(key).toString());
    }
}

/**
 * @brief This fills up added/removed TextGraphicObject change to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 * @param added If False other style data are also exported.
 */
void MctUndoGraph::textGraphicObject(QDomElement *xmlchange, MctChange *change, bool added)
{
    xmlchange->setAttribute("type", MctUndoTags::TEXTGRAPHICOBJECT);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);

    MctChangedTexObjectBase * changeEntity = dynamic_cast<MctChangedTexObjectBase*>(change->getChangeEntity());
    xmlchange->setAttribute("name", changeEntity->getName());

    MctEmbObjProperties *embObjProps = dynamic_cast<MctEmbObjProperties*>(changeEntity->getObjectProperties());
    if (embObjProps->getURL() != ""){
        if (embObjProps->getPos().x() != 0 && embObjProps->getPos().y() != 0)
            embObjProps->setPositionInExport();
        xmlchange->setAttribute("URL", embObjProps->getURL());
    } else {
        xmlchange->setAttribute("URL", embObjProps->getShape()->getFileUrl());
    }

    if(added) {
        xmlchange->setAttribute("PositionX", embObjProps->getProps2Export()->value("PositionX").toString());
        xmlchange->setAttribute("PositionY", embObjProps->getProps2Export()->value("PositionY").toString());
        xmlchange->setAttribute("Height", embObjProps->getProps2Export()->value("Height").toString());
        xmlchange->setAttribute("Width", embObjProps->getProps2Export()->value("Width").toString());
        xmlchange->setAttribute("Rotation", embObjProps->getProps2Export()->value("Rotation").toString());
        if (embObjProps->getProps2Export()->value("PrevPositionX").toString() != "")
            xmlchange->setAttribute("PrevPositionX", embObjProps->getProps2Export()->value("PrevPositionX").toString()),
            xmlchange->setAttribute("PrevPositionY", embObjProps->getProps2Export()->value("PrevPositionY").toString());

        QList<QString> keys = embObjProps->getProps2Export()->keys();
        foreach (QString key, keys) {
            if (key.startsWith("Stroke") || key.startsWith("Shadow") || key.startsWith("PrevSize") || key.startsWith("PrevRotation")){
                xmlchange->setAttribute(key, embObjProps->getProps2Export()->value(key).toString());
            }
        }

        return;
    } else {
        if(embObjProps->getProps2Export()->size() == 0) {
            embObjProps->setProps2Export(embObjProps->getProps());
        }
    }

    QList<QString> keys = embObjProps->getProps2Export()->keys();
    foreach (QString key, keys) {
        if(MctStaticData::TEXTGRAPHICOBJECTS_STRUCTPROPS.contains(key)
           || key == "LinkDisplayName" || key == "GraphicURL" || key == "FrameStyleName") {
            continue;
        }
        xmlchange->setAttribute(key, embObjProps->getProps2Export()->value(key).toString());
    }
}

/**
 * @brief This fills up added/removed EmbeddedObject change to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctUndoGraph::embeddedObject(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute("type", MctUndoTags::EMBEDDEDOBJECT);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);

    xmlchange->setAttribute("name", change->getChangeEntity()->getName());
}

/**
 * @brief This fills up added string in table to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctUndoGraph::addStringInTable(QDomElement *xmlchange, MctChange *change)
{
    addString(xmlchange, change);
}

/**
 * @brief This fills up removed string in table to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctUndoGraph::removeStringInTable(QDomElement *xmlchange, MctChange *change)
{
    removeString(xmlchange, change);
}

/**
 * @brief This fills up moved string in table to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctUndoGraph::moveStringInTable(QDomElement *xmlchange, MctChange *change)
{
    moveString(xmlchange, change);
}

/**
 * @brief This fills up added paragraph break in table to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctUndoGraph::addParBreakInTable(QDomElement *xmlchange, MctChange *change)
{
    addParBreak(xmlchange, change);
}

/**
 * @brief This fills up deleted paragraph break in table to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctUndoGraph::delParBreakInTable(QDomElement *xmlchange, MctChange *change)
{
    delParBreak(xmlchange, change);
}

/**
 * @brief This fills up style changes in table to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctUndoGraph::styleChangeInTable(QDomElement *xmlchange, MctChange *change)
{
    styleChange(xmlchange, change);
}

/**
 * @brief This fills up added/removed TextGraphicObject change in table to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 * @param added If False other style data are also exported.
 */
void MctUndoGraph::textGraphicObjectInTable(QDomElement *xmlchange, MctChange *change, bool added)
{
    textGraphicObject(xmlchange, change, added);
}

/**
 * @brief This fills up added/removed TextTable change in table to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 * @param added If False other style data are also exported.
 */
void MctUndoGraph::textTableInTable(QDomElement *xmlchange, MctChange *change, bool added)
{
    textTable(xmlchange, change, added);
}

/**
 * @brief This fills up added/removed row change in table into the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctUndoGraph::rowChangeInTable(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute("type", MctUndoTags::ROWCHANGE);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);
}

/**
 * @brief This fills up added/removed col change in table into the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctUndoGraph::colChangeInTable(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute("type", MctUndoTags::COLCHANGE);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);
}
