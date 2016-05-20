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

#include "MctRedoGraph.h"
#include "MctStaticData.h"
#include "MctChangeset.h"
#include "MctChange.h"
#include "MctRedoTags.h"
#include "MctChangeEntities.h"
#include "MctTableProperties.h"
#include "MctEmbObjProperties.h"
#include "KoDocument.h"

MctRedoGraph::MctRedoGraph(QString odt, KoTextDocument *koTextDoc) : MctRedoGraphXMLfilter(MctStaticData::REDOCHANGES, odt, koTextDoc)
{
    fillUpGraph();
}

MctRedoGraph::~MctRedoGraph()
{

}

/**
 * @brief This converts and adds changeset from undo graph into redo graph
 * @param undochangeset ChangesetNode in the undo graph
 * @return Returns with the created changesetNode.
 */
MctChangeset*  MctRedoGraph::addchangesetFromUndo(MctChangeset *undochangeset)
{
    undochangeset->clearParents();
    undochangeset->clearChilds();

    MctAuthor * author = undochangeset->getAuthor();
    QDateTime date = undochangeset->getDate();
    QString comment = undochangeset->getComment();
    QList<MctChange*> * changelist = undochangeset->getChanges();
    QList<MctChange*> * redochangelist = new QList<MctChange*>();

    foreach (MctChange *changeNode, *changelist) {
        MctPosition * pos = changeNode->getPosition();
        MctChangeTypes changeType = changeNode->getChangeType();
        MctNode* changeEntity = changeNode->getChangeEntity();
        MctChange * redoChangeNode = NULL;
        MctPosition * movedpos = NULL;

        if(changeType == MctChangeTypes::MovedString) {
            movedpos = changeNode->getMovedPosition();
            redoChangeNode = new MctChange(pos, changeType, changeEntity, movedpos);
        } else {
            redoChangeNode = new MctChange(pos, changeType, changeEntity);
        }
        redochangelist->append(redoChangeNode);
    }

    return addChangeset(redochangelist, author, date, comment);
}

/**
 * @brief This adds new change to the changeset
 * @param change The change node
 * @param changeset The parent changeset node
 */
void MctRedoGraph::addChange(MctChange* changeNode, MctChangeset* changeset)
{
    if(changeNode == NULL) {
        qCritical() << "MctChange NULL";
        return;
    }
    QDomElement change;
    QDomElement parent = changeset->getChangeSetNode();
    if(changeNode->getChangeType() == MctChangeTypes::AddedString) {
        change = m_doc->createElement(MctRedoTags::ADDED);
        parent.appendChild(change);
        addString(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedStringInTable) {
        change = m_doc->createElement(MctRedoTags::ADDED);
        parent.appendChild(change);
        addStringInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedString) {
        change = m_doc->createElement(MctRedoTags::REMOVED);
        parent.appendChild(change);
        removeString(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedStringInTable) {
        change = m_doc->createElement(MctRedoTags::REMOVED);
        parent.appendChild(change);
        removeStringInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::MovedString) {
        change = m_doc->createElement(MctRedoTags::MOVED);
        parent.appendChild(change);
        moveString(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::MovedStringInTable) {
        change = m_doc->createElement(MctRedoTags::MOVED);
        parent.appendChild(change);
        moveStringInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::ParagraphBreak) {
        change = m_doc->createElement(MctRedoTags::ADDED);
        parent.appendChild(change);
        addParBreak(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::ParagraphBreakInTable) {
        change = m_doc->createElement(MctRedoTags::ADDED);
        parent.appendChild(change);
        addParBreakInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::DelParagraphBreak) {
        change = m_doc->createElement(MctRedoTags::REMOVED);
        parent.appendChild(change);
        delParBreak(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::DelParagraphBreakInTable) {
        change = m_doc->createElement(MctRedoTags::REMOVED);
        parent.appendChild(change);
        delParBreakInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::StyleChange) {
        change = m_doc->createElement(MctRedoTags::FORMATTAG);
        parent.appendChild(change);
        styleChange(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::StyleChangeInTable) {
        change = m_doc->createElement(MctRedoTags::FORMATTAG);
        parent.appendChild(change);
        styleChangeInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedTextFrame) {
        change = m_doc->createElement(MctRedoTags::ADDED);
        parent.appendChild(change);
        textFrame(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedTextFrameInTable) {
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedTextFrame) {
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedTextFrameInTable) {
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedTextGraphicObject) {
        change = m_doc->createElement(MctRedoTags::ADDED);
        parent.appendChild(change);
        textGraphicObject(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedTextGraphicObjectInTable) {
        change = m_doc->createElement(MctRedoTags::ADDED);
        parent.appendChild(change);
        textGraphicObjectInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedTextGraphicObject) {
        change = m_doc->createElement(MctRedoTags::REMOVED);
        parent.appendChild(change);
        textGraphicObject(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedTextGraphicObjectInTable) {
        change = m_doc->createElement(MctRedoTags::REMOVED);
        parent.appendChild(change);
        textGraphicObjectInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedEmbeddedObject) {
        change = m_doc->createElement(MctRedoTags::ADDED);
        parent.appendChild(change);
        embeddedObject(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedEmbeddedObjectInTable) {
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedEmbeddedObject) {
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedEmbeddedObjectInTable) {
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedTextTable) {
        change = m_doc->createElement(MctRedoTags::ADDED);
        parent.appendChild(change);
        textTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedTextTableInTable) {
        change = m_doc->createElement(MctRedoTags::ADDED);
        parent.appendChild(change);
        textTableInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedTextTable) {
        change = m_doc->createElement(MctRedoTags::REMOVED);
        parent.appendChild(change);
        textTable(&change, changeNode, false);
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedTextTableInTable) {
        change = m_doc->createElement(MctRedoTags::REMOVED);
        parent.appendChild(change);
        textTableInTable(&change, changeNode, false);
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedRowInTable) {
        change = m_doc->createElement(MctRedoTags::ADDED);
        parent.appendChild(change);
        rowChangeInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedRowInTable) {
        change = m_doc->createElement(MctRedoTags::REMOVED);
        parent.appendChild(change);
        rowChangeInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::AddedColInTable) {
        change = m_doc->createElement(MctRedoTags::ADDED);
        parent.appendChild(change);
        colChangeInTable(&change, changeNode);
    } else if (changeNode->getChangeType() == MctChangeTypes::RemovedColInTable) {
        change = m_doc->createElement(MctRedoTags::REMOVED);
        parent.appendChild(change);
        colChangeInTable(&change, changeNode);
    } else {
        change = m_doc->createElement("unknown");
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
void MctRedoGraph::addString(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute("type", MctRedoTags::STRING);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);
    MctStringChange * sc = dynamic_cast<MctStringChange*>(change->getChangeEntity());
    QDomText nodeText = m_doc->createTextNode(sc->getString());
    xmlchange->appendChild(nodeText);
}

/**
 * @brief This fills up removed string changes to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctRedoGraph::removeString(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute("type", MctRedoTags::STRING);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);
    MctStringChange * sc = dynamic_cast<MctStringChange*>(change->getChangeEntity());
    QDomText nodeText = m_doc->createTextNode(sc->getString());
    xmlchange->appendChild(nodeText);
}

/**
 * @brief This fills up moved string changes to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctRedoGraph::moveString(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute("type", MctRedoTags::STRING);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);

    MctPosition *movedpos = change->getMovedPosition();
    addPos2change(xmlchange, movedpos, true);

    MctStringChange * sc = dynamic_cast<MctStringChange*>(change->getChangeEntity());
    QDomText nodeText = m_doc->createTextNode(sc->getString());
    xmlchange->appendChild(nodeText);
}

/**
 * @brief This fills up added paragraph break changes to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctRedoGraph::addParBreak(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute("type", MctRedoTags::PARAGRAPH);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);
}

/**
 * @brief This fills up deleted paragraph break changes to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctRedoGraph::delParBreak(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute("type", MctRedoTags::PARAGRAPH);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);
}

/**
 * @brief This fills up style changes to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctRedoGraph::styleChange(QDomElement *xmlchange, MctChange *change)
{
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);

    MctStylePropertyChange * sc = dynamic_cast<MctStylePropertyChange*>(change->getChangeEntity());
    //filling up text properties
    if(sc->getTextPropChanges()->length() > 0) {
        QDomElement subchange = m_doc->createElement(MctRedoTags::PROPERTIESTAG);
        subchange.setAttribute("type", MctRedoTags::TEXTTAG);
        xmlchange->appendChild(subchange);
        fillPropertySubNode(&subchange, sc->getTextPropChanges(), MctStaticData::CharacterProperty);
    }

    //filling up parapraph properties
    if(sc->getParagraphPropChanges()->length() > 0) {
        QDomElement subchange = m_doc->createElement(MctRedoTags::PROPERTIESTAG);
        subchange.setAttribute("type", MctRedoTags::PARAGRAPHTAG);
        xmlchange->appendChild(subchange);
        fillPropertySubNode(&subchange, sc->getParagraphPropChanges(), MctStaticData::ParagraphProperty);
    }

    //filling up list properties
    if(sc->getListPropChanges()->length() > 0) {
        QDomElement subchange = m_doc->createElement(MctRedoTags::PROPERTIESTAG);
        subchange.setAttribute("type", MctRedoTags::LIST);
        xmlchange->appendChild(subchange);
        fillPropertySubNode(&subchange, sc->getListPropChanges(), MctStaticData::ListProperty);
    }

    //filling up properties for other components
    if(sc->getOtherPropChanges()->length() > 0) {
        QDomElement subchange = m_doc->createElement(MctRedoTags::PROPERTIESTAG);
        subchange.setAttribute("type", MctRedoTags::UNDEFINEDTAG);
        xmlchange->appendChild(subchange);
        fillPropertySubNode(&subchange, sc->getOtherPropChanges(), MctStaticData::UnknownProperty);
    }
}

void MctRedoGraph::fillPropertySubNode(QDomElement *subnode, ChangeEventList *propchanges, int type)
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
                qDebug() << "Not mappped property: " << QString::number(key);
            }
        }
    }    
}

/**
 * @brief This fills up added/removed TextFrame change to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctRedoGraph::textFrame(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute("type", MctRedoTags::TEXTFRAME);
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
void MctRedoGraph::textTable(QDomElement *xmlchange, MctChange *change, bool added)
{
    xmlchange->setAttribute("type", MctRedoTags::TABLE);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);

    MctChangedTexObjectBase * changeEntity = dynamic_cast<MctChangedTexObjectBase*>(change->getChangeEntity());
    xmlchange->setAttribute("name",changeEntity->getName());

    MctTableProperties * tableprops = dynamic_cast<MctTableProperties*>(changeEntity->getObjectProperties());
    xmlchange->setAttribute("col", tableprops->getCols());
    xmlchange->setAttribute("row", tableprops->getRows());
    if( ! added){
        return;
    }
    else if (tableprops) {
        if(tableprops->getProps2Export()->size() == 0) {
            tableprops->setProps2Export(tableprops->getProps());
        }
    }

    if (tableprops) {
        QList<QString> keys = tableprops->getProps2Export()->keys();
        if (!keys.isEmpty()) {
            foreach (QString key, keys) {
                if( MctStaticData::TEXTTABLE_STRUCTPROPS.contains(key) ||
                    MctStaticData::TEXTTABLE_ENUMPROPS.contains(key)) {
                    continue;
                }
                xmlchange->setAttribute(key, tableprops->getProps2Export()->value(key).toString());
            }
        }
    }
}

/**
 * @brief This fills up added/removed TextGraphicObject change to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 * @param added If False other style data are also exported.
 */
void MctRedoGraph::textGraphicObject(QDomElement *xmlchange, MctChange *change, bool added)
{
    xmlchange->setAttribute("type", MctRedoTags::TEXTGRAPHICOBJECT);
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

    if( ! added) {
        xmlchange->setAttribute("PositionX", embObjProps->getProps2Export()->value("PositionX").toString());
        xmlchange->setAttribute("PositionY", embObjProps->getProps2Export()->value("PositionY").toString());
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
    }

    QList<QString> keys = embObjProps->getProps2Export()->keys();
    foreach (QString key, keys) {
        if(MctStaticData::TEXTGRAPHICOBJECTS_STRUCTPROPS.contains(key)
           || key == "LinkDisplayName") {
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
void MctRedoGraph::embeddedObject(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute("type", MctRedoTags::EMBEDDEDOBJECT);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);

    xmlchange->setAttribute("name", change->getChangeEntity()->getName());
}

/**
 * @brief This fills up added string in table to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctRedoGraph::addStringInTable(QDomElement *xmlchange, MctChange *change)
{
    addString(xmlchange, change);
}

/**
 * @brief This fills up removed string in table to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctRedoGraph::removeStringInTable(QDomElement *xmlchange, MctChange *change)
{
    removeString(xmlchange, change);
}

/**
 * @brief This fills up moved string in table to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctRedoGraph::moveStringInTable(QDomElement *xmlchange, MctChange *change)
{
    moveString(xmlchange, change);
}

/**
 * @brief This fills up added paragraph break in table to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctRedoGraph::addParBreakInTable(QDomElement *xmlchange, MctChange *change)
{
    addParBreak(xmlchange, change);
}

/**
 * @brief This fills up deleted paragraph break in table to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctRedoGraph::delParBreakInTable(QDomElement *xmlchange, MctChange *change)
{
    delParBreak(xmlchange, change);
}

/**
 * @brief This fills up style changes in table to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctRedoGraph::styleChangeInTable(QDomElement *xmlchange, MctChange *change)
{
    styleChange(xmlchange, change);
}

/**
 * @brief This fills up added/removed TextGraphicObject change in table to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 * @param added If False other style data are also exported.
 */
void MctRedoGraph::textGraphicObjectInTable(QDomElement *xmlchange, MctChange *change, bool added)
{
    textGraphicObject(xmlchange, change, added);
}

/**
 * @brief This fills up added/removed TextTable change in table to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 * @param added If False other style data are also exported.
 */
void MctRedoGraph::textTableInTable(QDomElement *xmlchange, MctChange *change, bool added)
{
    textTable(xmlchange, change);
}

/**
 * @brief This fills up added/removed row change in table into the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctRedoGraph::rowChangeInTable(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute("type", MctRedoTags::ROWCHANGE);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);
}

/**
 * @brief This fills up added/removed col change in table into the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctRedoGraph::colChangeInTable(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute("type", MctRedoTags::COLCHANGE);
    MctPosition *pos = change->getPosition();
    addPos2change(xmlchange, pos);
}
