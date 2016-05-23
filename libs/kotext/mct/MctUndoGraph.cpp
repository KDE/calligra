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
    MctAuthor * author = redochangeset->author();
    QDateTime date = redochangeset->date();
    QString comment = redochangeset->comment();
    QList<MctChange*> * changelist = redochangeset->changes();
    QList<MctChange*> * undochangelist = new QList<MctChange*>();

    foreach (MctChange *changeNode, *changelist) {
        MctPosition * pos = changeNode->position();
        MctChangeTypes changeType = changeNode->changeType();
        MctNode* changeEntity = changeNode->changeEntity();
        MctChange * undoChangeNode = NULL;
        MctPosition * movedpos = NULL;

        if(changeType == MctChangeTypes::MovedString) {
            movedpos = changeNode->movedPosition();
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
    QDomElement parent = changeset->changeset();
    if(changeNode->changeType() == MctChangeTypes::AddedString) {
        change = m_doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        addString(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::AddedStringInTable) {
        change = m_doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        addStringInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::RemovedString) {
        change = m_doc->createElement(MctUndoTags::REMOVED);
        parent.appendChild(change);
        removeString(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::RemovedStringInTable) {
        change = m_doc->createElement(MctUndoTags::REMOVED);
        parent.appendChild(change);
        removeStringInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::MovedString) {
        change = m_doc->createElement(MctUndoTags::MOVED);
        parent.appendChild(change);
        moveString(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::MovedStringInTable) {
        change = m_doc->createElement(MctUndoTags::MOVED);
        parent.appendChild(change);
        moveStringInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::ParagraphBreak) {
        change = m_doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        addParBreak(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::ParagraphBreakInTable) {
        change = m_doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        addParBreakInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::DelParagraphBreak) {
        change = m_doc->createElement(MctUndoTags::REMOVED);
        parent.appendChild(change);
        delParBreak(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::DelParagraphBreakInTable) {
        change = m_doc->createElement(MctUndoTags::REMOVED);
        parent.appendChild(change);
        delParBreakInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::StyleChange) {
        change = m_doc->createElement(MctUndoTags::FORMATTAG);
        parent.appendChild(change);
        styleChange(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::StyleChangeInTable) {
        change = m_doc->createElement(MctUndoTags::FORMATTAG);
        parent.appendChild(change);
        styleChangeInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::AddedTextFrame) {
        change = m_doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        textFrame(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::AddedTextFrameInTable) {
    } else if (changeNode->changeType() == MctChangeTypes::RemovedTextFrame) {
    } else if (changeNode->changeType() == MctChangeTypes::RemovedTextFrameInTable) {
    } else if (changeNode->changeType() == MctChangeTypes::AddedTextGraphicObject) {
        change = m_doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        textGraphicObject(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::AddedTextGraphicObjectInTable) {
        change = m_doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        textGraphicObjectInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::RemovedTextGraphicObject) {
        change = m_doc->createElement(MctUndoTags::REMOVED);
        parent.appendChild(change);
        textGraphicObject(&change, changeNode, false);
    } else if (changeNode->changeType() == MctChangeTypes::RemovedTextGraphicObjectInTable) {
        change = m_doc->createElement(MctUndoTags::REMOVED);
        parent.appendChild(change);
        textGraphicObjectInTable(&change, changeNode, false);
    } else if (changeNode->changeType() == MctChangeTypes::AddedEmbeddedObject) {
        change = m_doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        embeddedObject(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::AddedEmbeddedObjectInTable) {
    } else if (changeNode->changeType() == MctChangeTypes::RemovedEmbeddedObject) {
    } else if (changeNode->changeType() == MctChangeTypes::RemovedEmbeddedObjectInTable) {
    } else if (changeNode->changeType() == MctChangeTypes::AddedTextTable) {
        change = m_doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        textTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::AddedTextTableInTable) {
        change = m_doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        textTableInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::RemovedTextTable) {
        change = m_doc->createElement(MctUndoTags::REMOVED);
        parent.appendChild(change);
        textTable(&change, changeNode, false);
    } else if (changeNode->changeType() == MctChangeTypes::RemovedTextTableInTable) {
        change = m_doc->createElement(MctUndoTags::REMOVED);
        parent.appendChild(change);
        textTableInTable(&change, changeNode, false);
    } else if (changeNode->changeType() == MctChangeTypes::AddedRowInTable) {
        change = m_doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        rowChangeInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::RemovedRowInTable) {
        change = m_doc->createElement(MctUndoTags::REMOVED);
        parent.appendChild(change);
        rowChangeInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::AddedColInTable) {
        change = m_doc->createElement(MctUndoTags::ADDED);
        parent.appendChild(change);
        colChangeInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::RemovedColInTable) {
        change = m_doc->createElement(MctUndoTags::REMOVED);
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
void MctUndoGraph::addString(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute("type", MctUndoTags::STRING);
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);
    MctStringChange * sc = dynamic_cast<MctStringChange*>(change->changeEntity());
    QDomText nodeText = m_doc->createTextNode(sc->getString());
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
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);
    MctStringChange * sc = dynamic_cast<MctStringChange*>(change->changeEntity());
    QDomText nodeText = m_doc->createTextNode(sc->getString());
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
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);

    MctPosition *movedpos = change->movedPosition();
    addPos2change(xmlchange, movedpos, true);

    MctStringChange * sc = dynamic_cast<MctStringChange*>(change->changeEntity());
    QDomText nodeText = m_doc->createTextNode(sc->getString());
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
    MctPosition *pos = change->position();
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
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);
}

/**
 * @brief This fills up style changes to the changeNode
 * @param xmlchange The change node in the XML
 * @param change The change node in the Graph
 */
void MctUndoGraph::styleChange(QDomElement *xmlchange, MctChange *change)
{    
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);

    MctStylePropertyChange * sc = dynamic_cast<MctStylePropertyChange*>(change->changeEntity());
    //filling up text properties
    if(sc->textPropChanges()->length() > 0) {
        QDomElement subchange = m_doc->createElement(MctUndoTags::PROPERTIESTAG);
        subchange.setAttribute("type", MctUndoTags::TEXTTAG);
        xmlchange->appendChild(subchange);
        fillPropertySubNode(&subchange, sc->textPropChanges(), MctStaticData::CharacterProperty);
    }

    //filling up parapraph properties
    if(sc->paragraphPropChanges()->length() > 0) {
        QDomElement subchange = m_doc->createElement(MctUndoTags::PROPERTIESTAG);
        subchange.setAttribute("type", MctUndoTags::PARAGRAPHTAG);
        xmlchange->appendChild(subchange);
        fillPropertySubNode(&subchange, sc->paragraphPropChanges(), MctStaticData::ParagraphProperty);
    }

    //filling up list properties
    if(sc->listPropChanges()->length() > 0) {
        QDomElement subchange = m_doc->createElement(MctUndoTags::PROPERTIESTAG);
        subchange.setAttribute("type", MctUndoTags::LIST);
        xmlchange->appendChild(subchange);
        fillPropertySubNode(&subchange, sc->listPropChanges(), MctStaticData::ListProperty);
    }

    //filling up properties for other components
    if(sc->otherPropChanges()->length() > 0) {
        QDomElement subchange = m_doc->createElement(MctUndoTags::PROPERTIESTAG);
        subchange.setAttribute("type", MctUndoTags::UNDEFINEDTAG);
        xmlchange->appendChild(subchange);
        fillPropertySubNode(&subchange, sc->otherPropChanges(), MctStaticData::UnknownProperty);
    }
}

void MctUndoGraph::fillPropertySubNode(QDomElement *subnode, ChangeEventList* propchanges, int type)
{
    foreach (ChangeEvent* change, *propchanges) {
        change->calcDiff(type);
        foreach (int key, change->changes()->keys()) {
            QPair<QString, ChangeAction> value = change->changes()->value(key);            

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
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);

    xmlchange->setAttribute("name", change->changeEntity()->name());
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
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);

    MctChangedTexObjectBase * changeEntity = dynamic_cast<MctChangedTexObjectBase*>(change->changeEntity());
    xmlchange->setAttribute("name",changeEntity->name());

    MctTableProperties * tableprops = dynamic_cast<MctTableProperties*>(changeEntity->objectProperties());
    xmlchange->setAttribute("col", tableprops->cols());
    xmlchange->setAttribute("row", tableprops->rows());

    if(added){
        return;
    }
    else {
        if(tableprops->props2Export()->size() == 0) {
            tableprops->setProps2Export(tableprops->props());
        }
    }

    QList<QString> keys = tableprops->props2Export()->keys();
    foreach (QString key, keys) {
        if( MctStaticData::TEXTTABLE_STRUCTPROPS.contains(key) ||
            MctStaticData::TEXTTABLE_ENUMPROPS.contains(key)) {
            continue;
        }
        xmlchange->setAttribute(key, tableprops->props2Export()->value(key).toString());
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
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);

    MctChangedTexObjectBase * changeEntity = dynamic_cast<MctChangedTexObjectBase*>(change->changeEntity());
    xmlchange->setAttribute("name", changeEntity->name());

    MctEmbObjProperties *embObjProps = dynamic_cast<MctEmbObjProperties*>(changeEntity->objectProperties());
    if (embObjProps->url() != ""){
        if (embObjProps->pos().x() != 0 && embObjProps->pos().y() != 0)
            embObjProps->setPositionInExport();
        xmlchange->setAttribute("URL", embObjProps->url());
    } else {
        xmlchange->setAttribute("URL", embObjProps->shape()->getFileUrl());
    }

    if(added) {
        xmlchange->setAttribute("PositionX", embObjProps->props2Export()->value("PositionX").toString());
        xmlchange->setAttribute("PositionY", embObjProps->props2Export()->value("PositionY").toString());
        xmlchange->setAttribute("Height", embObjProps->props2Export()->value("Height").toString());
        xmlchange->setAttribute("Width", embObjProps->props2Export()->value("Width").toString());
        xmlchange->setAttribute("Rotation", embObjProps->props2Export()->value("Rotation").toString());
        if (embObjProps->props2Export()->value("PrevPositionX").toString() != "")
            xmlchange->setAttribute("PrevPositionX", embObjProps->props2Export()->value("PrevPositionX").toString()),
            xmlchange->setAttribute("PrevPositionY", embObjProps->props2Export()->value("PrevPositionY").toString());

        QList<QString> keys = embObjProps->props2Export()->keys();
        foreach (QString key, keys) {
            if (key.startsWith("Stroke") || key.startsWith("Shadow") || key.startsWith("PrevSize") || key.startsWith("PrevRotation")){
                xmlchange->setAttribute(key, embObjProps->props2Export()->value(key).toString());
            }
        }

        return;
    } else {
        if(embObjProps->props2Export()->size() == 0) {
            embObjProps->setProps2Export(embObjProps->props());
        }
    }

    QList<QString> keys = embObjProps->props2Export()->keys();
    foreach (QString key, keys) {
        if(MctStaticData::TEXTGRAPHICOBJECTS_STRUCTPROPS.contains(key)
           || key == "LinkDisplayName" || key == "GraphicURL" || key == "FrameStyleName") {
            continue;
        }
        xmlchange->setAttribute(key, embObjProps->props2Export()->value(key).toString());
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
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);

    xmlchange->setAttribute("name", change->changeEntity()->name());
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
    MctPosition *pos = change->position();
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
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);
}
