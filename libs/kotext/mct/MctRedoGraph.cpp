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
//#include "MctStaticData.h"
#include "MctChangeEntities.h"
#include "MctTableProperties.h"
#include "MctEmbObjProperties.h"
#include "KoDocument.h"

MctRedoGraph::MctRedoGraph(const QString &odt, KoTextDocument *koTextDoc)
    : MctRedoGraphXMLfilter(MctStaticData::REDOCHANGES, odt, koTextDoc)
{
    fillUpGraph();
}

MctRedoGraph::~MctRedoGraph()
{

}

MctChangeset*  MctRedoGraph::addchangesetFromUndo(MctChangeset *undochangeset)
{
    undochangeset->clearParents();
    undochangeset->clearChilds();

    MctAuthor * author = undochangeset->author();
    QDateTime date = undochangeset->date();
    QString comment = undochangeset->comment();
    QList<MctChange*> * changelist = undochangeset->changes();
    QList<MctChange*> * redochangelist = new QList<MctChange*>();

    foreach (MctChange *changeNode, *changelist) {
        MctPosition * pos = changeNode->position();
        MctChangeTypes changeType = changeNode->changeType();
        MctNode* changeEntity = changeNode->changeEntity();
        MctChange * redoChangeNode = NULL;
        MctPosition * movedpos = NULL;

        if(changeType == MctChangeTypes::MovedString) {
            movedpos = changeNode->movedPosition();
            redoChangeNode = new MctChange(pos, changeType, changeEntity, movedpos);
        } else {
            redoChangeNode = new MctChange(pos, changeType, changeEntity);
        }
        redochangelist->append(redoChangeNode);
    }

    return addChangeset(redochangelist, author, date, comment);
}

void MctRedoGraph::addChange(MctChange* changeNode, MctChangeset* changeset)
{
    if(changeNode == NULL) {
        qCritical() << "MctChange NULL";
        return;
    }
    QDomElement change;
    QDomElement parent = changeset->changeset();
    if(changeNode->changeType() == MctChangeTypes::AddedString) {
        change = m_doc->createElement(MctStaticData::ADDED);
        parent.appendChild(change);
        addString(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::AddedStringInTable) {
        change = m_doc->createElement(MctStaticData::ADDED);
        parent.appendChild(change);
        addStringInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::RemovedString) {
        change = m_doc->createElement(MctStaticData::REMOVED);
        parent.appendChild(change);
        removeString(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::RemovedStringInTable) {
        change = m_doc->createElement(MctStaticData::REMOVED);
        parent.appendChild(change);
        removeStringInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::MovedString) {
        change = m_doc->createElement(MctStaticData::MOVED);
        parent.appendChild(change);
        moveString(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::MovedStringInTable) {
        change = m_doc->createElement(MctStaticData::MOVED);
        parent.appendChild(change);
        moveStringInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::ParagraphBreak) {
        change = m_doc->createElement(MctStaticData::ADDED);
        parent.appendChild(change);
        addParBreak(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::ParagraphBreakInTable) {
        change = m_doc->createElement(MctStaticData::ADDED);
        parent.appendChild(change);
        addParBreakInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::DelParagraphBreak) {
        change = m_doc->createElement(MctStaticData::REMOVED);
        parent.appendChild(change);
        delParBreak(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::DelParagraphBreakInTable) {
        change = m_doc->createElement(MctStaticData::REMOVED);
        parent.appendChild(change);
        delParBreakInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::StyleChange) {
        change = m_doc->createElement(MctStaticData::FORMATTAG);
        parent.appendChild(change);
        styleChange(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::StyleChangeInTable) {
        change = m_doc->createElement(MctStaticData::FORMATTAG);
        parent.appendChild(change);
        styleChangeInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::AddedTextFrame) {
        change = m_doc->createElement(MctStaticData::ADDED);
        parent.appendChild(change);
        textFrame(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::AddedTextFrameInTable) {
    } else if (changeNode->changeType() == MctChangeTypes::RemovedTextFrame) {
    } else if (changeNode->changeType() == MctChangeTypes::RemovedTextFrameInTable) {
    } else if (changeNode->changeType() == MctChangeTypes::AddedTextGraphicObject) {
        change = m_doc->createElement(MctStaticData::ADDED);
        parent.appendChild(change);
        textGraphicObject(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::AddedTextGraphicObjectInTable) {
        change = m_doc->createElement(MctStaticData::ADDED);
        parent.appendChild(change);
        textGraphicObjectInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::RemovedTextGraphicObject) {
        change = m_doc->createElement(MctStaticData::REMOVED);
        parent.appendChild(change);
        textGraphicObject(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::RemovedTextGraphicObjectInTable) {
        change = m_doc->createElement(MctStaticData::REMOVED);
        parent.appendChild(change);
        textGraphicObjectInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::AddedEmbeddedObject) {
        change = m_doc->createElement(MctStaticData::ADDED);
        parent.appendChild(change);
        embeddedObject(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::AddedEmbeddedObjectInTable) {
    } else if (changeNode->changeType() == MctChangeTypes::RemovedEmbeddedObject) {
    } else if (changeNode->changeType() == MctChangeTypes::RemovedEmbeddedObjectInTable) {
    } else if (changeNode->changeType() == MctChangeTypes::AddedTextTable) {
        change = m_doc->createElement(MctStaticData::ADDED);
        parent.appendChild(change);
        textTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::AddedTextTableInTable) {
        change = m_doc->createElement(MctStaticData::ADDED);
        parent.appendChild(change);
        textTableInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::RemovedTextTable) {
        change = m_doc->createElement(MctStaticData::REMOVED);
        parent.appendChild(change);
        textTable(&change, changeNode, false);
    } else if (changeNode->changeType() == MctChangeTypes::RemovedTextTableInTable) {
        change = m_doc->createElement(MctStaticData::REMOVED);
        parent.appendChild(change);
        textTableInTable(&change, changeNode, false);
    } else if (changeNode->changeType() == MctChangeTypes::AddedRowInTable) {
        change = m_doc->createElement(MctStaticData::ADDED);
        parent.appendChild(change);
        rowChangeInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::RemovedRowInTable) {
        change = m_doc->createElement(MctStaticData::REMOVED);
        parent.appendChild(change);
        rowChangeInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::AddedColInTable) {
        change = m_doc->createElement(MctStaticData::ADDED);
        parent.appendChild(change);
        colChangeInTable(&change, changeNode);
    } else if (changeNode->changeType() == MctChangeTypes::RemovedColInTable) {
        change = m_doc->createElement(MctStaticData::REMOVED);
        parent.appendChild(change);
        colChangeInTable(&change, changeNode);
    } else {
        change = m_doc->createElement("unknown");
        parent.appendChild(change);
    }

    if(!change.isNull()) {
        changeNode->addElementTreeNode(change);
        changeset->addChange(changeNode);

        change.setAttribute(MctStaticData::attributeNS(MctStaticData::ID, MctStaticData::NS_C), QString::number(this->_id));

        ++_id;

    } else {
        qDebug () << "Change is NULL, not added to the graph";
    }
}

void MctRedoGraph::addString(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC), MctStaticData::STRING);
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);
    MctStringChange * sc = dynamic_cast<MctStringChange*>(change->changeEntity());
    QDomText nodeText = m_doc->createTextNode(sc->getString());
    xmlchange->appendChild(nodeText);
}

void MctRedoGraph::removeString(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC), MctStaticData::STRING);
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);
    MctStringChange * sc = dynamic_cast<MctStringChange*>(change->changeEntity());
    QDomText nodeText = m_doc->createTextNode(sc->getString());
    xmlchange->appendChild(nodeText);
}

void MctRedoGraph::moveString(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC), MctStaticData::STRING);
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);

    MctPosition *movedpos = change->movedPosition();
    addPos2change(xmlchange, movedpos, true);

    MctStringChange * sc = dynamic_cast<MctStringChange*>(change->changeEntity());
    QDomText nodeText = m_doc->createTextNode(sc->getString());
    xmlchange->appendChild(nodeText);
}

void MctRedoGraph::addParBreak(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC), MctStaticData::PARAGRAPH);
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);
    xmlchange->removeAttribute(MctStaticData::attributeNS(MctStaticData::POSEND, MctStaticData::NS_C)); // remove redundant ending position from xml
}

void MctRedoGraph::delParBreak(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC), MctStaticData::PARAGRAPH);
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);
    xmlchange->removeAttribute(MctStaticData::attributeNS(MctStaticData::POSEND, MctStaticData::NS_C)); // remove redundant ending position from xml
}

void MctRedoGraph::styleChange(QDomElement *xmlchange, MctChange *change)
{
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);

    MctStylePropertyChange * sc = dynamic_cast<MctStylePropertyChange*>(change->changeEntity());
    //filling up text properties
    if(sc->textPropChanges()->length() > 0) {
        QDomElement subchange = m_doc->createElement(MctStaticData::PROPERTIESTAG);
        subchange.setAttribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC), MctStaticData::TEXTTAG);
        xmlchange->appendChild(subchange);
        fillPropertySubNode(&subchange, sc->textPropChanges(), MctStaticData::CharacterProperty);
    }

    //filling up parapraph properties
    if(sc->paragraphPropChanges()->length() > 0) {
        QDomElement subchange = m_doc->createElement(MctStaticData::PROPERTIESTAG);
        subchange.setAttribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC), MctStaticData::PARAGRAPHTAG);
        xmlchange->appendChild(subchange);
        fillPropertySubNode(&subchange, sc->paragraphPropChanges(), MctStaticData::ParagraphProperty);
    }

    //filling up list properties
    if(sc->listPropChanges()->length() > 0) {
        QDomElement subchange = m_doc->createElement(MctStaticData::PROPERTIESTAG);
        subchange.setAttribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC), MctStaticData::LIST);
        xmlchange->appendChild(subchange);
        fillPropertySubNode(&subchange, sc->listPropChanges(), MctStaticData::ListProperty);
    }

    //filling up properties for other components
    if(sc->otherPropChanges()->length() > 0) {
        QDomElement subchange = m_doc->createElement(MctStaticData::PROPERTIESTAG);
        subchange.setAttribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC), MctStaticData::UNDEFINEDTAG);
        xmlchange->appendChild(subchange);
        fillPropertySubNode(&subchange, sc->otherPropChanges(), MctStaticData::UnknownProperty);
    }
}

void MctRedoGraph::fillPropertySubNode(QDomElement *subnode, ChangeEventList *propchanges, int type)
{
    foreach (ChangeEvent* change, *propchanges) {
        change->calcDiff(type);
        foreach (int key, change->changes()->keys()) {
            QPair<QString, ChangeAction> value = change->changes()->value(key);

            QString propName = MctStaticData::getTextPropetyString(key, type);
            qDebug() << propName << "(" << key << ") : "  << value.first;

            if(propName != "") {
                subnode->setAttribute(MctStaticData::attributeNS(propName, MctStaticData::NS_C), value.first);
            } else {
                qDebug() << "Not mappped property: " << QString::number(key);
            }
        }
    }    
}

void MctRedoGraph::textFrame(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC), MctStaticData::TEXTFRAME);
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);

    xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::NAME, MctStaticData::NS_DC), change->changeEntity()->name());
}

void MctRedoGraph::textTable(QDomElement *xmlchange, MctChange *change, bool added)
{
    xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC), MctStaticData::TABLE);
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);

    MctChangedTexObjectBase * changeEntity = dynamic_cast<MctChangedTexObjectBase*>(change->changeEntity());
    xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::NAME, MctStaticData::NS_C),changeEntity->name());

    MctTableProperties * tableprops = dynamic_cast<MctTableProperties*>(changeEntity->objectProperties());
    xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::COL, MctStaticData::NS_C), tableprops->cols());
    xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::ROW, MctStaticData::NS_C), tableprops->rows());
    if( ! added){
        return;
    }
    else if (tableprops) {
        if(tableprops->props2Export()->size() == 0) {
            tableprops->setProps2Export(tableprops->props());
        }
    }

    if (tableprops) {
        QList<QString> keys = tableprops->props2Export()->keys();
        if (!keys.isEmpty()) {
            foreach (QString key, keys) {
                if( MctStaticData::TEXTTABLE_STRUCTPROPS.contains(key) ||
                    MctStaticData::TEXTTABLE_ENUMPROPS.contains(key)) {
                    continue;
                }
                xmlchange->setAttribute(MctStaticData::attributeNS(key, MctStaticData::NS_C), tableprops->props2Export()->value(key).toString());
            }
        }
    }
}

void MctRedoGraph::textGraphicObject(QDomElement *xmlchange, MctChange *change, bool added)
{
    xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC), MctStaticData::TEXTGRAPHICOBJECT);
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);

    MctChangedTexObjectBase * changeEntity = dynamic_cast<MctChangedTexObjectBase*>(change->changeEntity());
    xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::NAME, MctStaticData::NS_C), changeEntity->name());

    MctEmbObjProperties *embObjProps = dynamic_cast<MctEmbObjProperties*>(changeEntity->objectProperties());
    if (embObjProps->url() != ""){
        if (embObjProps->pos().x() != 0 && embObjProps->pos().y() != 0)
            embObjProps->setPositionInExport();
        xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::URL, MctStaticData::NS_C), embObjProps->url());
    } else {
        xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::URL, MctStaticData::NS_C), embObjProps->shape()->getFileUrl());
    }

    if( ! added) {
        xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::POSX, MctStaticData::NS_C), embObjProps->props2Export()->value("PositionX").toString());
        xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::POSY, MctStaticData::NS_C), embObjProps->props2Export()->value("PositionY").toString());
        xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::HEIGHT, MctStaticData::NS_C), embObjProps->props2Export()->value("Rotation").toString());
        if (embObjProps->props2Export()->value("PrevPositionX").toString() != "")
            xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::PREVPOSX, MctStaticData::NS_C), embObjProps->props2Export()->value("PrevPositionX").toString()),
            xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::PREVPOSY, MctStaticData::NS_C), embObjProps->props2Export()->value("PrevPositionY").toString());

        QList<QString> keys = embObjProps->props2Export()->keys();
        foreach (QString key, keys) {
            if (key.startsWith("Stroke") || key.startsWith("Shadow") || key.startsWith("PrevSize") || key.startsWith("PrevRotation")){
                xmlchange->setAttribute(MctStaticData::attributeNS(key, MctStaticData::NS_C), embObjProps->props2Export()->value(key).toString());
            }
        }
        return;
    }

    QList<QString> keys = embObjProps->props2Export()->keys();
    foreach (QString key, keys) {
        if(MctStaticData::TEXTGRAPHICOBJECTS_STRUCTPROPS.contains(key)
           || key == "LinkDisplayName") {
            continue;
        }
        xmlchange->setAttribute(MctStaticData::attributeNS(key, MctStaticData::NS_C), embObjProps->props2Export()->value(key).toString());
    }
}

void MctRedoGraph::embeddedObject(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC), MctStaticData::EMBEDDEDOBJECT);
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);

    xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::NAME, MctStaticData::NS_DC), change->changeEntity()->name());
}

void MctRedoGraph::addStringInTable(QDomElement *xmlchange, MctChange *change)
{
    addString(xmlchange, change);
}

void MctRedoGraph::removeStringInTable(QDomElement *xmlchange, MctChange *change)
{
    removeString(xmlchange, change);
}

void MctRedoGraph::moveStringInTable(QDomElement *xmlchange, MctChange *change)
{
    moveString(xmlchange, change);
}

void MctRedoGraph::addParBreakInTable(QDomElement *xmlchange, MctChange *change)
{
    addParBreak(xmlchange, change);
}

void MctRedoGraph::delParBreakInTable(QDomElement *xmlchange, MctChange *change)
{
    delParBreak(xmlchange, change);
}

void MctRedoGraph::styleChangeInTable(QDomElement *xmlchange, MctChange *change)
{
    styleChange(xmlchange, change);
}

void MctRedoGraph::textGraphicObjectInTable(QDomElement *xmlchange, MctChange *change, bool added)
{
    textGraphicObject(xmlchange, change, added);
}

void MctRedoGraph::textTableInTable(QDomElement *xmlchange, MctChange *change, bool added)
{
    textTable(xmlchange, change);
}

void MctRedoGraph::rowChangeInTable(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC), MctStaticData::ROWCHANGE);
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);
}

void MctRedoGraph::colChangeInTable(QDomElement *xmlchange, MctChange *change)
{
    xmlchange->setAttribute(MctStaticData::attributeNS(MctStaticData::TYPE, MctStaticData::NS_DC), MctStaticData::COLCHANGE);
    MctPosition *pos = change->position();
    addPos2change(xmlchange, pos);
}
