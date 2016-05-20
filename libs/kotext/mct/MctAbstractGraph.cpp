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

#include "MctAbstractGraph.h"
#include "MctChange.h"
#include "MctChangeset.h"
#include "MctStaticData.h"
#include "MctPosition.h"
#include "MctEmbObjProperties.h"
#include "MctTableProperties.h"
#include "MctCell.h"
#include "MctChangeEntities.h"

#include "kzip.h"
#include "kurl.h"
#include "ExtractFile.h"

#include "KoDocument.h"
#include "KoTextDocument.h"
#include "KoCanvasController.h"
#include "KoCreateShapesTool.h"
#include "KoColorBackground.h"
#include "KoShapeFactoryBase.h"
#include "KoShapeRegistry.h"
#include "KoToolManager.h"
#include "KoShapeStrokeModel.h"
#include "KoShapeStroke.h"
#include "KoShapeShadow.h"

#include <algorithm>


const QString MctAbstractGraph::DATE_EARLIER = "earlier";
const QString MctAbstractGraph::DATE_LATER = "later";
const QString MctAbstractGraph::DATE_ALL = "all";

/**
 * @brief This initializes class abstract of Graph.
 * @param redoOrUndo "UndoChanges" for undo.xml, "RedoChanges" for redo.xml
 * @param odt file handle or name.
 */
MctAbstractGraph::MctAbstractGraph(QString redoOrUndo, QString odt, KoTextDocument *koTextDoc)
{
    _id = 1;
    m_changeNodes = new QList<MctChange*>();
    m_changesetNodes = new QMap<QDateTime, MctChangeset*>();
    m_dates = new QVector<QDateTime>();
    m_idDates = new QMap<ulong, QDateTime>();
    m_doc = 0;

    this->m_koTextDoc = koTextDoc;

    this->m_redoOrUndo = redoOrUndo;
    if(redoOrUndo == MctStaticData::UNDOCHANGES) {
        m_nodeTag = MctStaticData::UNDOTAG;
    } else {
        m_nodeTag = MctStaticData::REDOTAG;
    }

    m_odtFile = odt;
    setFilename(m_odtFile);

    KZip *zip = new KZip(odt);
    zip->open(QIODevice::ReadOnly);
    QStringList entries = zip->directory()->entries();
    bool found = false;
    foreach (QString entry, entries) {
        qDebug() << entry;
        if(entry == redoOrUndo + ".xml") {
            found = true;            
            const KArchiveEntry * kentry = zip->directory()->entry(entry);
            const KZipFileEntry * zipEntry = static_cast<const KZipFileEntry *>(kentry);
            QFileInfo fi(m_fileName);
            copyTo(fi.absolutePath(), fi.fileName(), zipEntry);
            /*QFile tmp(path + entry);
            tmp.rename(fileName);*/
        }
    }
    zip->close();
    delete zip;

    QFile file(m_fileName);
    if (!found) {
        qDebug() << "Starting new " << redoOrUndo << " graph";
        m_doc = new QDomDocument(redoOrUndo);

        file.open(QIODevice::WriteOnly);

        m_root = m_doc->createElement(redoOrUndo);
        QString datestring = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        datestring.replace(" ", "T");
        m_root.setAttribute("started",datestring);
        m_doc->appendChild(m_root);

        QTextStream out(&file);
        out << m_doc->toString();

        qDebug() << m_fileName << " graph created";

    } else {
        qDebug() << "Loading " << redoOrUndo << " graph";

        m_doc = new QDomDocument(redoOrUndo);
        file.open(QIODevice::ReadOnly);
        QByteArray xml = file.readAll();
        m_doc->setContent(xml);
    }
    file.close();
}

MctAbstractGraph::~MctAbstractGraph()
{
    delete m_changeNodes;
    delete m_changesetNodes;
    delete m_dates;
    delete m_idDates;
    delete m_doc;
}

QList<MctChange*> * MctAbstractGraph::changeNodes() const
{
    return this->m_changeNodes;
}

void MctAbstractGraph::setChangeNodes(QList<MctChange*> *changeNodes)
{
    this->m_changeNodes = changeNodes;
}

/**
 * @brief This gets list of changesetNodes
 * @return Returns with the whole < date, changeset> map.
 */
QMap<QDateTime, MctChangeset*>* MctAbstractGraph::changesetNodes() const
{
    return m_changesetNodes;
}

void MctAbstractGraph::setChangesetNodes(QMap<QDateTime, MctChangeset*> *setChangesetNodes)
{
    this->m_changesetNodes = setChangesetNodes;
}

/**
 * @brief This gets list of changeset dates
 * @return Returns with the list of dates
 */
QVector<QDateTime> * MctAbstractGraph::dates(QVector<QDateTime> *excludeDates) const
{
    if (excludeDates == NULL)
        return m_dates;

    QVector<QDateTime> *newDates = new QVector<QDateTime>(*m_dates);
    foreach (QDateTime date, *m_dates){
        qDebug() << date.toString();
        foreach (QDateTime excludeDate, *excludeDates){
            qDebug() << excludeDate.toString();
            if (date == excludeDate){
                int idx = m_dates->indexOf(date);
                m_dates->remove(idx);
                //dates->removeOne(date);
            }
        }
    }

    return newDates;
}

void MctAbstractGraph::setDates(QVector<QDateTime> *dates)
{
    this->m_dates = dates;
}

QMap<ulong, QDateTime> *MctAbstractGraph::idDates() const
{
    return this->m_idDates;
}

/**
 * @brief This gets the redoOrUndo attribute
 */
QString MctAbstractGraph::redoOrUndo() const
{
    return m_redoOrUndo;
}

void MctAbstractGraph::setRedoOrUndo(QString string)
{
    this->m_redoOrUndo = string;
}

QString MctAbstractGraph::odtFile() const
{
    return this->m_odtFile;
}

void MctAbstractGraph::setOdtFile(QString name)
{
    this->m_odtFile = name;
}

QString MctAbstractGraph::filename() const
{
    return this->m_fileName;
}

/**
 * @brief This sets the filename of the graph output
 * @param name the path of the file
 */
void MctAbstractGraph::setFilename(QString name)
{
    m_fileName = name + "_" + m_redoOrUndo + ".xml";
}

QString MctAbstractGraph::nodeTag() const
{
    return this->m_nodeTag;
}

void MctAbstractGraph::setNodeTag(QString name)
{
    this->m_nodeTag = name;
}

/**
 * @brief This gets the root of the graph
 * @return Returns with the root node.
 */
QDomElement MctAbstractGraph::root() const
{
    return m_root;
}

void MctAbstractGraph::setRoot(QDomElement  root)
{    
    this->m_root = root;
}

QDomDocument * MctAbstractGraph::doc() const
{
    return this->m_doc;
}

void MctAbstractGraph::setDoc(QDomDocument *doc)
{
    this->m_doc = doc;
}

KoTextDocument * MctAbstractGraph::koTextDoc()
{
    return m_koTextDoc;
}

/**
 * @brief This gets changeset node wth given date.
 * @param date The date corresponding to the wanted changeset
 * @return Returns with the changesetNode corresponding to the given date.
 */
MctChangeset* MctAbstractGraph::getChangeset(QDateTime date)
{
    qDebug () << "date1: " << date;
    MctChangeset* changset = NULL;
    foreach(QDateTime d, m_changesetNodes->keys())  {
        qDebug() << "date: " << d;
        if (d == date) {
            changset = m_changesetNodes->value(d);
        }
    }
    return changset;
}

/**
 * @brief This gets list of changesetNodes. If date is set, return with changesetnodes earlier than the given date.
 * @param date The date.
 * @return Returns with the list of changesets ordered by the dates.
 */
QList<MctChangeset*>* MctAbstractGraph::getChangesetList(QDateTime date)
{
    QList<MctChangeset*>* changeset = new QList<MctChangeset*>();
    const QVector<QDateTime> * datesList;
    if (date.isValid()) {
        datesList = findEarlierDates(date);
    } else {
        datesList = m_dates;
    }
    foreach (QDateTime d, *datesList) {
        MctChangeset * changesetnode = getChangeset(d);
        if(changesetnode) {
            changeset->append(changesetnode);
        }
    }
    return changeset;
}

/**
 * @brief This gets list of changesetNodes corresponding o the list of dates.
 * @param dates The list of dates.
 * @return Returns with the list of changesets ordered by the dates.
 */
QList<MctChangeset*>* MctAbstractGraph::getChangesetListByDates(QList<QDateTime> *dates)
{
    QList<MctChangeset*>* changesets = new QList<MctChangeset*>();
    foreach (QDateTime date, *dates) {
        MctChangeset* changeset = getChangeset(date);
        changesets->append(changeset);
    }
    return changesets;
}

/**
 * @brief This creates embObjProps object from properties in the graph xml
 * @param change The node representing the changeevent
 */
MctEmbObjProperties* MctAbstractGraph::createEmbObjProps(QDomElement change)
{
    QString name = change.attribute("name");
    if(name.isEmpty()) {
        return NULL;
    }

    QPointF posOrigin(change.attribute("PositionX").toDouble(), change.attribute("PositionY").toDouble());
    QPointF pos(posOrigin);
    QSizeF size(change.attribute("Width").toDouble(), change.attribute("Height").toDouble());

    if (!change.attribute("PositionX").contains("/")){
        pos.setX(pos.x() + (size.width()/2.0));
        pos.setY(pos.y() + (size.height()/2.0));
    }

    MctEmbObjProperties * cembObjProps = NULL;
//FIXME
    /*
    //KoShape *shape = MctStaticData::instance()->getKoDocument()->emitPosition(pos);
    if (shape->position().x() != posOrigin.x() && shape->position().y() != posOrigin.y()){
        KoShape *shapeNew = NULL;
        if (change.attribute("URL").contains("/")){
//FIXME
            //shapeNew = MctStaticData::instance()->getKoDocument()->emitCreateShapeFromXML(change);
        } else {
            shapeNew= createShapeFromProps(change);
        }

        if (round(shapeNew->position().x()) != round(shape->position().x()) || round(shapeNew->position().y()) != round(shape->position().y())){
            shape = shapeNew;
        }
        shape->setFileUrl(change.attribute("URL"));
        cembObjProps = new MctEmbObjProperties(name, shape, posOrigin, shape->size());
        cembObjProps->setURL(change.attribute("URL"));
    } else {
        cembObjProps = new MctEmbObjProperties(name, shape, posOrigin, size);
    }

    cembObjProps->setOdtUrl(odtFile);
    if (change.attribute("PrevPositionX") != ""){
        QPointF prevPos(change.attribute("PrevPositionX").toDouble(), change.attribute("PrevPositionY").toDouble());
        cembObjProps->setPrevPos(prevPos);
    }

    PropertyDictionary * props = new PropertyDictionary();
    QDomNamedNodeMap attribs = change.attributes();

    for(int i=0; i < attribs.length(); i++) {
        QDomNode node = attribs.item(i);
        QDomAttr attr = node.toAttr();
        QString key = attr.name();
        QString value = attr.value();

        if (key == "type") {
            continue;
        } else if ( key == "name") {
            continue;
        } else if ( key == "URL") {
            cembObjProps->setInnerUrl(value);
        } else if ( key == "id") {
            continue;
        } else if (MctStaticData::TEXTGRAPHICOBJECTS_INTPROPS.contains(key)) {
            props->insert(key, QVariant(value.toInt()));
        } else if (MctStaticData::TEXTGRAPHICOBJECTS_FLOATPROPS.contains(key)) {
            props->insert(key, QVariant(value.toFloat()));
        } else if (MctStaticData::TEXTGRAPHICOBJECTS_STRINGPROPS.contains(key)) {
            props->insert(key, QVariant(value));
        } else if (MctStaticData::TEXTGRAPHICOBJECTS_BOOLEANPROPS.contains(key)) {
            bool boolValue = false;
            if (value.compare("true", Qt::CaseInsensitive) == 0) {
                boolValue = true;
            }
            props->insert(key, QVariant(boolValue));
        }
    }
    //cembObjProps->setProps2Export(props);*/
    return cembObjProps;
}

MctEmbObjProperties *MctAbstractGraph::createEmbObjShapeProps(QDomElement change)
{
    KoShape *shape = createShapeFromProps(change);

    MctEmbObjProperties *cembObjProps = new MctEmbObjProperties(change.attribute("URL"), shape, shape->position(), shape->size());
    if (change.attribute("PrevPositionX") != ""){
        QPointF prevPos(change.attribute("PrevPositionX").toDouble(), change.attribute("PrevPositionY").toDouble());
        cembObjProps->setPrevPos(prevPos);
    }
    return cembObjProps;
}

KoShape *MctAbstractGraph::createShapeFromProps(QDomElement change)
{
    const KoProperties *properties = getShapeProperties(change.attribute("URL"));

    auto getShapeId = [] (QString val) -> QString { QStringList strList = val.split('_'); return strList[0]; };
    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(getShapeId(change.attribute("URL")));

    KoShape *shape = NULL;
    if (!properties){
        shape = factory->createDefaultShape();
    } else {
        shape = factory->createShape(properties);
    }

    setShapeStyles(shape, change);

    return shape;
}

void MctAbstractGraph::setShapeStyles(KoShape *shape, QDomElement change)
{
    QSizeF size(change.attribute("Width").toDouble(), change.attribute("Height").toDouble());
    shape->setSize(size);
    QPointF pos(change.attribute("PositionX").toDouble(), change.attribute("PositionY").toDouble());
    shape->setPosition(pos);
    if (change.attribute("BGRed") != ""){
        KoColorBackground *kcb = new KoColorBackground(QColor(change.attribute("BGRed").toInt(), change.attribute("BGGreen").toInt(), change.attribute("BGBlue").toInt()));
        shape->setBackground(QSharedPointer<KoColorBackground>(kcb));
    }

    KoShapeStroke* stroke = dynamic_cast<KoShapeStroke*>(shape->stroke());
    if (change.attribute("StrokeLineWidth") != "")
        stroke->setLineWidth(change.attribute("StrokeLineWidth").toDouble());
    if (change.attribute("StrokeColorRed") != "")
        stroke->setColor(QColor(change.attribute("StrokeColorRed").toInt(), change.attribute("StrokeColorGreen").toInt(), change.attribute("StrokeColorBlue").toInt()));
    if (change.attribute("StrokeMiterLimit") != "")
        stroke->setMiterLimit(change.attribute("StrokeMiterLimit").toDouble());
    if (change.attribute("StrokeLineStyle") != ""){
        QVector<qreal> dashes;
        if (change.attribute("StrokeLineDashes") != ""){
            QStringList dashSplit = change.attribute("StrokeLineDashes").split(",");
            foreach (QString d, dashSplit){
               dashes.push_back(d.toDouble());
            }
        }
        stroke->setLineStyle(Qt::PenStyle(change.attribute("StrokeLineStyle").toInt()),dashes);
     }
    shape->setStroke(stroke);

    KoShapeShadow* shadow = shape->shadow();
    if (change.attribute("ShadowVisible") != "")
        shadow->setVisible(change.attribute("ShadowVisible").toInt());
    if (change.attribute("ShadowColorRed") != "")
        shadow->setColor(QColor(change.attribute("ShadowColorRed").toInt(), change.attribute("ShadowColorGreen").toInt(), change.attribute("ShadowColorBlue").toInt()));
    if (change.attribute("ShadowBlur") != "")
        shadow->setBlur(change.attribute("StrokeMiterLimit").toDouble());
    if (change.attribute("ShadowOffset") != ""){
        QStringList offsetStrip = change.attribute("ShadowOffset").split(",");
        QPointF offset(offsetStrip.at(0).toDouble(), offsetStrip.at(1).toDouble());
        shadow->setOffset(offset);
     }
    shape->setShadow(shadow);

    if (change.attribute("Width") != ""){
        shape->setSize(QSizeF(change.attribute("Width").toDouble(), change.attribute("Height").toDouble()));
    }

    if (change.attribute("PrevRotation") != ""){
        shape->rotate(360 - change.attribute("Rotation").toDouble() + change.attribute("PrevRotation").toDouble());
    }
}

const KoProperties *MctAbstractGraph::getShapeProperties(QString type)
{
    foreach(const QString & id, KoShapeRegistry::instance()->keys()) {
        KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(id);
        if ( factory->hidden() ) {
            continue;
        }

        foreach(const KoShapeTemplate & shapeTemplate, factory->templates()) {

            QString id= shapeTemplate.id;
            if (!shapeTemplate.templateId.isEmpty()) {
                id += '_'+shapeTemplate.templateId;
            }
            if (id == type) return shapeTemplate.properties;

        }
    }
    return NULL;
}

/**
 * @brief This creates tableProps object from properties in the graph xml
 * @param change The node representing the changeevent
 * @return
 */
MctTableProperties * MctAbstractGraph::createTableProps(QDomElement change)
{
    QString name = change.attribute("name");
    if(name.isEmpty()) {
        return NULL;
    }

    MctTableProperties * ctableProps = new MctTableProperties(name);

    PropertyDictionary * props = new PropertyDictionary();
    QDomNamedNodeMap attribs = change.attributes();

    for(uint i=0; i < attribs.length(); i++) {
        QDomNode node = attribs.item(i);
        QDomAttr attr = node.toAttr();
        QString key = attr.name();
        QString value = attr.value();
        if (key == "type") {
            continue;
        } else if ( key == "name") {
            continue;
        } else if ( key == "URL") {
            ctableProps->setInnerUrl(value);    //FIXME: buggy in the original python code, check if really neccessary to setup
        } else if ( key == "id") {
            continue;
        } else if ( key == "row" || key == "col") {
            props->insert(key, QVariant(value.toInt()));
            if (key == "row")
                ctableProps->setRows(value.toInt());
            else
                ctableProps->setCols(value.toInt());
        }
    }

    ctableProps->setProps2Export(props);

    return ctableProps;
}

/**
 * @brief This fills the graph with the content of the file "undo.xml"/"redo.xml"
 */
void MctAbstractGraph::fillUpGraph()
{
    _id = 0;
    if(!m_doc) {
        m_doc = new QDomDocument(m_redoOrUndo);
        _id = 1;
        return;
    }
    qDebug() << "fill up the graph";

    QMap<ulong, ulong> *indexes = nullptr;
    if (m_koTextDoc) indexes = MctStaticData::instance()->getFrameIndexes(m_koTextDoc);

    m_root = m_doc->firstChildElement();
    QDomNodeList childs = m_root.childNodes();
    for(uint i=0; i < childs.length(); i++) {
        QDomNode node = childs.at(i);
        if(node.isElement()) {
            QDomElement elem = node.toElement();
            if (elem.tagName() == m_nodeTag) {
                MctChangeset * changeset = new MctChangeset(elem);
                int parentidx = 1;
                while (true) {
                    QString parentID = elem.attribute("parent"+ QString::number(parentidx));
                    if(parentID.isEmpty()) {
                        break;
                    }
                    changeset->addParentId(parentID.toInt());
                    ++parentidx;
                }
                QDateTime date = changeset->getDate();
                m_dates->append(date);
                m_changesetNodes->insert(date, changeset);
                m_idDates->insert(changeset->getId(), date);

                QDomNodeList changeList = elem.childNodes();
                for(uint j = 0; j < changeList.length(); j++) {
                    QDomNode node = changeList.at(j);

                    //if (koTextDoc) correctBlockPosition(&node, indexes);

                    addChangeFromXML(node, changeset);
                }
            }
            QString idLocal = elem.attribute("id");
            if(idLocal.toULong() > _id) {
                _id = idLocal.toULong();
            }
        }
    }

    //fill up changesetNodes with children Id's
    foreach (MctChangeset * changeset, m_changesetNodes->values()) {
        QList<ulong>* parents = changeset->getParents();
        foreach (ulong idnum, *parents) {
            QDateTime date = m_idDates->value(idnum);
            if(date.isNull()) {
                continue;
            }
            MctChangeset* changesetNode_tmp = m_changesetNodes->value(date);
            if(changesetNode_tmp != NULL) {
                changesetNode_tmp->addChild(changeset->getId());
            }
        }
    }

    ++_id;
    sortDates();
}

/**
 * @brief MctAbstractGraph::correctBlockPosition
 * @param node xml node
 */
void MctAbstractGraph::correctBlockPosition(QDomNode *node, QMap<ulong, ulong> *indexes, bool import)
{
    QDomElement change = node->toElement();
    QString start;
    QString end;
    ulong startpar;
    ulong endpar;

    start = change.attribute(MctStaticData::POSSTART);
    end = change.attribute(MctStaticData::POSEND);

    int idx1 = start.indexOf(MctStaticData::POSSEPARATOR, 1);
    startpar = start.mid(1, idx1 - 1).toULong();
    int idx2 = end.indexOf(MctStaticData::POSSEPARATOR, 1);
    endpar = end.mid(1, idx2 - 1).toULong();

    QMap<ulong,ulong>::iterator it = indexes->lowerBound(startpar);
    if(it != indexes->begin()) {
        --it;
    }
    int d = import ? 0: 1;
    if(startpar > it.key() + d) {
        if(import) {
            if(it != indexes->begin()) {
                start.replace(1, idx1 - 1, QString::number(startpar + it.value()));
                end.replace(1, idx2 - 1, QString::number(endpar + it.value()));
            }
        } else {
            start.replace(1, idx1 - 1, QString::number(startpar - it.value()));
            end.replace(1, idx2 - 1, QString::number(endpar - it.value()));
        }
    }
    change.setAttribute(MctStaticData::POSSTART, start);
    change.setAttribute(MctStaticData::POSEND, end);

    //Moved position
    QString mstart = change.attribute(MctStaticData::MPOSSTART);
    if(!mstart.isEmpty()) {
        QString mend = change.attribute(MctStaticData::MPOSEND);

        idx1 = mstart.indexOf(MctStaticData::POSSEPARATOR, 1);
        startpar = mstart.mid(1, idx1 - 1).toULong();
        idx2 = mend.indexOf(MctStaticData::POSSEPARATOR, 1);
        endpar = mend.mid(1, idx2 - 1).toULong();

        it = indexes->lowerBound(startpar);
        if(it != indexes->begin()) {
            --it;
        }
        if(startpar > it.key() + 1) {
            if(import) {
                mstart.replace(1, idx1 - 1, QString::number(startpar + it.value()));
                mend.replace(1, idx2 - 1, QString::number(endpar + it.value()));
            } else {
                mstart.replace(1, idx1 - 1, QString::number(startpar - it.value()));
                mend.replace(1, idx2 - 1, QString::number(endpar - it.value()));
            }
        }
        change.setAttribute(MctStaticData::MPOSSTART, mstart);
        change.setAttribute(MctStaticData::MPOSEND, mend);
    }
//todo
}

/**
 * @brief During export the paragraph numbers should be written in the xml,
 * but the blocknumber gives a false value if the document contains tables.
 * The correction is applied right before exporting to xml.
 * @param document The document within the positions need to be corrected
 * @param indexes The frame index, it is the base of the correction
 */
void MctAbstractGraph::correctBlockPositionForExport(QDomDocument *document, QMap<ulong, ulong> *indexes)
{
    QDomNodeList childs = document->firstChildElement().childNodes();
    for(uint i=0; i < childs.length(); i++) {
        QDomNode node = childs.at(i);
        if(node.isElement()) {
            QDomElement elem = node.toElement();
            if (elem.tagName() == m_nodeTag) {
                QDomNodeList changeList = elem.childNodes();
                for(uint j = 0; j < changeList.length(); j++) {
                    QDomNode node = changeList.at(j);
                    correctBlockPosition(&node, indexes, false);
                }
            }
        }
    }
}

/**
 * @brief This gets the date of changeset by its Id
 * @param id The Id number
 * @return Returns with the date
 */
QDateTime MctAbstractGraph::getDateFromId(ulong id)
{
    return m_idDates->value(id);
}

/**
 * @brief This finds a changesetnode corresponding to given revision. The revision number is actually the index of the revision in the data structure.
 * @param revNum The number of the revision
 * @return Returns with the changeset node, or NULL if revision does not exist.
 */
MctChangeset* MctAbstractGraph::findChangeset(ulong revNum)
{
    MctChangeset* changesetNode = NULL;
    if(revNum > m_dates->size()) {
        qDebug() << "Revisions " << revNum << " not found in the list of changeset nodes!";
        return NULL;
    }
    QDateTime date = m_dates->at(revNum - 1);
    if(m_changesetNodes->contains(date)) {
        changesetNode = m_changesetNodes->value(date);
    } else {
        qDebug() << "Revisions " << revNum << " not found";
    }
    return changesetNode;
}

/**
 * @brief This sorts the list of dates from the most older (first index) to the youngest one (last index).
 */
void MctAbstractGraph::sortDates()
{
    qSort(m_dates->begin(), m_dates->end());
}

/**
 * @brief This adds new changeset to the end of the graph without correction of position attribute
 * @param changes The list of changes
 * @param author The author of the revision
 * @param date The date of the revision
 * @param comment comment of the revision
 * @param parentId The parent changeset id
 * @param merge True if the adding changeset is being merged from another graph, False otherwise.
 * @return Returns with the created changesetNode
 */
MctChangeset* MctAbstractGraph::addChangeset(QList<MctChange *> *changes, MctAuthor *author, QDateTime date, QString comment, uint parentId, bool merge)
{
    qDebug() << "adding changeset to graph";
    merge = true; //FIXME!

    QDomElement changeset = m_doc->createElement(m_nodeTag);
    m_root.appendChild(changeset);

    if((parentId == 0) && (m_dates->size() > 0)) {
        QDateTime lastDate = m_dates->last();
        MctChangeset* parentNode = m_changesetNodes->value(lastDate);
        parentId = parentNode->getId();
    }

    MctChangeset*changesetNode = new MctChangeset(changeset, parentId, author, date, comment, this->_id++);

    QList<MctChange *> * tmpchanges = new QList<MctChange*> (*changes);

    if(m_redoOrUndo == MctStaticData::UNDOCHANGES) {
        tmpchanges = findMovedChanges(tmpchanges);       // TODO: that was different in the python code
    }

    reverseList<MctChange *>(&tmpchanges);
    foreach (MctChange* change, *tmpchanges) {
        addChange(change, changesetNode);
    }

    findParents(changesetNode, merge);
    qDebug() << date;
    m_changesetNodes->insert(date, changesetNode);
    m_dates->append(date);
    qDebug() << date;
    m_idDates->insert(changesetNode->getId(), date);
    sortDates();
    qDebug() << date;
//FIXME:
    if(merge) {
        correctParentsAndChildren(changesetNode, merge);
    }

    qDebug() << "changeset added to graph";
    return changesetNode;
}

/**
 * @brief Compare two date
 * @param d1 The date class.
 * @param d2 The date class.
 * @return Returns with -1 if d1 earlier, return 0 if equal, return 1 if d1 later than d2
 */
int MctAbstractGraph::dateComapre(QDateTime d1, QDateTime d2) {
    if(d1 < d2) return -1;
    else if(d1 == d2) return 0;
    else return 1;
}

/**
 * @brief This find the parents of the changesetNode.
 * @param changsetnode The changeset Node.
 * @param merge True if the adding changeset is being merged from another graph, False otherwise.
 */
void MctAbstractGraph::findParents(MctChangeset *changesetNode, bool merge)
{
    qDebug() << "searching for parents";
    QList<MctChange*> *changes = changesetNode->getChanges();
    QList<MctPosition*> orig_pos;
    QList<MctPosition*> orig_movedpos;
    QDateTime date = changesetNode->getDate();

    int sgn = m_redoOrUndo == MctStaticData::UNDOCHANGES ? -1 : 1;

    //store original positions
    for (int i = 0; i< changes->length(); i++) {
        MctChange * change = changes->at(i);
        orig_pos.append(change->getPosition());
        orig_movedpos.append(change->getMovedPosition());
    }

    for(int i = m_dates->size() - 1; i >= 0; i--) {
        MctChangeset* changesetNode_temp;
        if(m_redoOrUndo == MctStaticData::UNDOCHANGES){
            changesetNode_temp = m_changesetNodes->value(m_dates->at(i));
        } else {
            changesetNode_temp = m_changesetNodes->value(m_dates->at(m_dates->size()- i - 1));
        }
        QList<MctChange *> *changes_temp = changesetNode_temp->getChanges();
        QDateTime date_temp = changesetNode_temp->getDate();

        foreach (MctChange* change_temp, *changes_temp) {
            for(int j =0; j < changes->length(); j++) {
                MctChange* change = changes->at(j);
                MctPosition* pos_orig = orig_pos.at(j);
                MctPosition* movedpos_orig = orig_movedpos.at(j);

                if( (areChangesOverlapping(change_temp, pos_orig, movedpos_orig)) &&
                  (dateComapre(date_temp, date) == sgn || !merge) ) {
                    changesetNode->addParentId(changesetNode_temp->getId());
                    changesetNode_temp->addChild(changesetNode->getId());
                } else if ( (areChangesOverlapping2(change, change_temp)) &&
                            (dateComapre(date_temp, date) == sgn || !merge) ) {
                    changesetNode->addParentId(changesetNode_temp->getId());
                    changesetNode_temp->addChild(changesetNode->getId());
                }
            }
        }
    }

    qDebug() << "searching for parents finished";
}

/**
 * @brief This corrects parent-child correspondance, when new changeset is merged into the graph
 * @param changsetnode The changeset node that was merged
 * @param merge True if the adding changeset is being merged from another graph, False otherwise.
 */
void MctAbstractGraph::correctParentsAndChildren(MctChangeset *changsetNode, bool merge)
{
    QList<ulong> *parentIds = changsetNode->getParents();

    foreach (ulong id, *parentIds) {
        QDateTime date = getDateFromId(id);
        MctChangeset * changesetNodeParent = getChangeset(date);
        QList<ulong> *childrenIds = changesetNodeParent->getChilds();
        foreach (ulong idChild, *childrenIds) {
            if(idChild == changsetNode->getId()) {
                continue;
            }
            changesetNodeParent->removeChild(idChild);
            QDateTime dateChild = getDateFromId(idChild);
            MctChangeset* changesetNodeChild = getChangeset(dateChild);
            if (changesetNodeChild == NULL)
                continue;
            changesetNodeChild->clearParents();
            findParents(changesetNodeChild,merge);
        }
    }
}

/**
 * @brief This decides, whether change1 and change2 related to embedded objects are overlaping or not.
 * @param change1 ChangeNode
 * @param change2 ChangeNode
 * @return Returns with true if changes are related to the same embedded object, false otherwise.
 */
bool MctAbstractGraph::areChangesOverlapping2(MctChange *change1, MctChange *change2)
{
    if(! (change1->getChangeType() == MctChangeTypes::AddedTextFrame || change1->getChangeType() == MctChangeTypes::RemovedTextFrame ||
          change1->getChangeType() == MctChangeTypes::AddedTextGraphicObject || change1->getChangeType() == MctChangeTypes::RemovedTextGraphicObject ||
          change1->getChangeType() == MctChangeTypes::AddedEmbeddedObject || change1->getChangeType() == MctChangeTypes::RemovedEmbeddedObject )) {
        return false;
    }
    if(! (change2->getChangeType() == MctChangeTypes::AddedTextFrame || change2->getChangeType() == MctChangeTypes::RemovedTextFrame ||
          change2->getChangeType() == MctChangeTypes::AddedTextGraphicObject || change2->getChangeType() == MctChangeTypes::RemovedTextGraphicObject ||
          change2->getChangeType() == MctChangeTypes::AddedEmbeddedObject || change2->getChangeType() == MctChangeTypes::RemovedEmbeddedObject )) {
        return false;
    }

    MctNode* changeEntity1 = change1->getChangeEntity();
    MctNode* changeEntity2 = change2->getChangeEntity();

    if(changeEntity1->getName() == changeEntity2->getName()) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief Recursively decides, whether pos1 and pos2 are overlappong or not.
 * @param pos1 Instance of Position class.
 * @param pos2 Instance of Position class.
 * @return Returns with true if positions are overlapping in paragraph, false otherwise.
 */
bool MctAbstractGraph::arePositionsOverlapping(MctPosition *pos1, MctPosition *pos2)
{
    if (pos1 == NULL || pos2 == NULL) {
        return false;
    }

    bool change_overlapping = false;
    bool paragraph_overlapping = false;
    bool character_overlapping = false;
    QList<MctPosition *> *poslist1 = new QList<MctPosition *>();
    QList<MctPosition *> *poslist2 = new QList<MctPosition *>();

    MctPosition * pos_tmp = pos1;
    while(true) {
        poslist1->append(pos_tmp);
        MctPosition* parentPos = pos_tmp->getAnchoredPos();
        if (parentPos == NULL) {
            break;
        } else {
            pos_tmp = parentPos;
        }
    }

    pos_tmp = pos2;
    while (true) {
        poslist2->append(pos_tmp);
        MctPosition* parentPos = pos_tmp->getAnchoredPos();
        if(parentPos == NULL) {
            break;
        } else {
            pos_tmp = parentPos;
        }
    }

//    reverseList<MctPosition *>(&poslist1);
//    reverseList<MctPosition *>(&poslist2);

    if(abs(poslist1->length() - poslist2->length()) > 1) {
        return change_overlapping;
    }

    int length = qMin(poslist1->length(), poslist2->length());
    for (int i = 0; i< length; i++) {
        MctPosition* pos1_tmp = poslist1->at(i);
        MctPosition* pos2_tmp = poslist2->at(i);

        //determine wheter overlapping paragraphs of changes
        if((pos1_tmp->getEndPar() >= pos2_tmp->getStartPar()) && (pos1_tmp->getStartPar() <= pos2_tmp->getEndPar())){
            paragraph_overlapping = true;
        } else {
            paragraph_overlapping = false;
            return paragraph_overlapping;
        }

        //determine wheter overlapping characters of changes
        if((pos1_tmp->getEndChar() >= pos2_tmp->getStartChar()) && (pos1_tmp->getStartChar() <= pos2_tmp->getEndChar())) {
            character_overlapping = true;
        } else {
            character_overlapping = false;
            return character_overlapping;
        }

        if(paragraph_overlapping && character_overlapping) {
            change_overlapping = true;
        } else {
            change_overlapping = false;
        }

        if (pos1_tmp->getStartPar() == pos2_tmp->getEndPar() && pos1_tmp->getStartChar() == pos2_tmp->getEndChar()){
            change_overlapping = false;
        }

        //check AddedTable
        if (pos2_tmp->getStartPar() == pos2_tmp->getEndPar() && pos2_tmp->getStartChar() == pos2_tmp->getEndChar()){
            if (pos1_tmp->getStartPar() == pos2_tmp->getStartPar() && pos1_tmp->getStartChar() == pos2_tmp->getStartChar())
                change_overlapping = true;
        }

        MctCell* cellinf1 = pos1_tmp->getCellInfo();
        MctCell* cellinf2 = pos2_tmp->getCellInfo();
        if(cellinf1 == NULL && cellinf2 == NULL) {
            //change_overlapping = false;
            return change_overlapping;
        }
        if(cellinf1 != NULL && cellinf2 == NULL) {
            //change_overlapping = false;
            return change_overlapping;
        }
        if(cellinf1 != NULL && cellinf2 != NULL) {
            cellinf1->convertCellPos2CellName();
            cellinf2->convertCellPos2CellName();
            if( cellinf1->cellName() != cellinf2->cellName()) {
                change_overlapping = false;
                return change_overlapping;
            }
        }
    }

    delete poslist1;
    delete poslist2;
    return change_overlapping;
}

/**
 * @brief Recursively decides, whether pos1 is before pos2 or not.
 * @param pos1 Instance of Position class.
 * @param pos2 Instance of Position class.
 * @return Returns 0 if pos1 and pos2 are not in the same textcontent, returns 1 if pos1 is before pos2, -1 otherwise.
 */
int MctAbstractGraph::comparePositions(MctPosition *pos1, MctPosition *pos2)
{
    if(pos1 == NULL || pos2 == NULL) {
        return 0;
    }

    bool paragraph_preceeding = false;
    bool character_preceeding = false;

    QList<MctPosition *> poslist1;
    QList<MctPosition *> poslist2;

    MctPosition * pos_tmp = pos1;
    while (true) {
        poslist1.append(pos_tmp);
        MctPosition* parent_pos = pos_tmp->getAnchoredPos();
        if(parent_pos == NULL) {
            break;
        } else {
            pos_tmp = parent_pos;
        }
    }

    pos_tmp = pos2;
    while (true) {
        poslist2.append(pos_tmp);
        MctPosition* parent_pos = pos_tmp->getAnchoredPos();
        if(parent_pos == NULL) {
            break;
        } else {
            pos_tmp = parent_pos;
        }
    }

    if(poslist1.length() != poslist2.length()) {
        return 0;
    }

    for(int i = 0; i< poslist1.length(); i++) {
        MctPosition* pos1_tmp = poslist1.at(i);
        MctPosition* pos2_tmp = poslist2.at(i);

        MctCell* cellinf1 = pos1_tmp->getCellInfo();
        MctCell* cellinf2 = pos2_tmp->getCellInfo();

        /*if(cellinf1 == NULL && cellinf2 == NULL) {
            return 0;
        }*/
        if(cellinf1 != NULL && cellinf2 == NULL) {
            return 0;
        }
        if(cellinf1 != NULL && cellinf2 != NULL) {
            cellinf1->convertCellPos2CellName();
            cellinf2->convertCellPos2CellName();
            if(cellinf1->cellName() != cellinf2->cellName()) {
                return 0;
            }
        }

        //determine wheter overlapping paragraphs of changes
        if(pos1_tmp->getEndPar() < pos2_tmp->getStartPar() || (pos1_tmp->getEndPar() == pos2_tmp->getStartPar() && pos2_tmp->getStartChar() >= pos1_tmp->getEndChar())) {
            paragraph_preceeding = true;
        } else {
            return -1;
        }

        //determine wheter overlapping characters of changes ???
        /*if(pos1_tmp->getEndChar() < pos2_tmp->getStartChar()) {
            character_preceeding = true;
        } else {
            return -1;
        }*/

        character_preceeding = true;

        if(paragraph_preceeding && character_preceeding) {
            return 1;
        } else {
            return -1;
        }
    }
    return 0;
}

/**
 * @brief This decides, whether change1 and change2 are overlaping or not.
 * @param change ChangeNode
 * @param pos_orig Position of change
 * @param movedpos_orig Moved position of change
 * @return Returns with true if they are overlapping in paragraph, false otherwise.
 */
bool MctAbstractGraph::areChangesOverlapping(MctChange *change, MctPosition *pos_orig, MctPosition *movedpos_orig)
{
    MctPosition * pos1 = pos_orig;
    MctPosition * pos2 = change->getPosition();

    MctPosition * movedpos1 = movedpos_orig;
    MctPosition * movedpos2 = change->getMovedPosition();

    bool change_overlapping = false;

    if (pos1 == NULL || pos2 == NULL) {
        return change_overlapping;
    }

    //determine wheter overlapping changes
    change_overlapping = arePositionsOverlapping(pos1, pos2);
    if(change_overlapping) {
        return change_overlapping;
    } else {
        correctPosition(pos1, pos2, change->getChangeType());
    }

    change_overlapping = arePositionsOverlapping(movedpos1, pos2);
    if(change_overlapping) {
        return change_overlapping;
    } else {
        correctPosition(movedpos1, pos2, change->getChangeType());
    }

    change_overlapping = arePositionsOverlapping(pos1, movedpos2);
    if(change_overlapping) {
        return change_overlapping;
    } else {
        correctPosition(pos1, movedpos2, change->getChangeType());
    }

    change_overlapping = arePositionsOverlapping(movedpos1, movedpos2);
    if(change_overlapping) {
        return change_overlapping;
    } else {
        correctPosition(movedpos1, movedpos2, change->getChangeType());
    }

    return change_overlapping;
}

/**
 * @brief This adds changesetNode into the graph.
 * @detailed The changesetNode is corrected due to the earlier changesetNodes and the later ChangesetNodes in the graph are corrected as well.
 * @param changesetnode The new changesetNode
 */
void MctAbstractGraph::addChangesetNodeWithCorr(MctChangeset *changesetNode)
{
    //correct changesetNode according to earlier ones
    correctChangesetNode(changesetNode, DATE_EARLIER);

    //correct later changesetNodes in the graph
    QVector<QDateTime> * laterDates = findLaterDates(changesetNode->getDate());
    QList<MctChangeset*> list_temp;
    list_temp.append(changesetNode);

    foreach (QDateTime date_temp, *laterDates) {
        MctChangeset* changesetNode_temp = m_changesetNodes->value(date_temp);
        correctChangesetNode2(changesetNode_temp, &list_temp);
    }

    //add changeset to the graph
    changesetNode->setId(_id);

    QList<MctChange*> *changes = changesetNode->getChanges();
    MctAuthor *author = changesetNode->getAuthor();
    QDateTime date = changesetNode->getDate();
    QString comment = changesetNode->getComment();

    reverseList<MctChange*>(&changes);
    addChangeset(changes, author, date, comment);
    sortDates();
}

/**
 * @brief Finds date keys of changesets with later date, than the given one
 * @param date The date class.
 * @return Returns with the list of later dates
 */
QVector<QDateTime> * MctAbstractGraph::findLaterDates(QDateTime date)
{
    QVector<QDateTime> * retDates = new QVector<QDateTime>();
    sortDates();
    foreach (QDateTime date_temp, *m_dates) {
        if(dateComapre(date_temp, date) == 1) { //date_temp > date
            retDates->append(date_temp);
        }
    }
    return retDates;
}

/**
 * @brief Finds date keys of changesets with earlier or equal date, than the given one
 * @param date The date
 * @return Returns with the list of earlier or equal dates
 */
QVector<QDateTime> * MctAbstractGraph::findEarlierDates(QDateTime date)
{
    QVector<QDateTime> * retDates = new QVector<QDateTime>();
    sortDates();

    foreach (QDateTime date_temp, *m_dates) {
        int result = dateComapre(date_temp, date);
        if(result <= 0 ) { // date_temp < date , date_temp == date
            retDates->append(date_temp);
        }
    }
    return retDates;
}

/**
 * @brief This corrects the position attributes of changes in all changesets of the graph, according to the given changeset.
 * @param changesetNode The changesetNode
 */
void MctAbstractGraph::correctChangesetNodeList(MctChangeset *changesetNode)
{
    QList<MctChangeset*> *changesetList = new QList<MctChangeset*>();
    changesetList->append(changesetNode);

    foreach (QDateTime date, *m_dates) {
        MctChangeset * changesetNode_temp = getChangeset(date);
        correctChangesetNode2(changesetNode_temp, changesetList);
    }
}

/**
 * @brief This corrects the position attributes of changes in all changesets of the graph with later/earlier/all date than the given changeset according to the given changeset.
 * @param changesetNode The changesetNode
 * @param withdates string: 'earlier', 'later', 'all'. Used to select dates to correct.
 * @param foradd True if the changesetNode is intended to be added.
 */
void MctAbstractGraph::correctChangesetNodeListWithDate(MctChangeset *changesetNode, QString withdates, bool foradd)
{
    QDateTime date = changesetNode->getDate();
    QVector<QDateTime> *datesList;
    if(withdates == DATE_EARLIER) {
        datesList =findEarlierDates(date);
    } else if (withdates == DATE_LATER) {
        datesList = findLaterDates(date);
    } else {
        datesList = m_dates;
    }

    QList<MctChangeset*> *changesetNodesList = new QList<MctChangeset*>();
    changesetNodesList->append(changesetNode);

    foreach (QDateTime date, *datesList) {
        MctChangeset* changesetNode_temp = getChangeset(date);
        correctChangesetNode2(changesetNode_temp, changesetNodesList, foradd);
    }
}

/**
 * @brief This corrects the position attributes of changes in changeset according to previous/later/all changes in the graph.
 * @param changesetnode The new changesetNode
 * @param withdates string: 'earlier', 'later', 'all'. Used to select dates to correct.
 * @param foradd True if the changesetNode is intended to be added.
 * @return Returns with the parent ID
 */
int MctAbstractGraph::correctChangesetNode(MctChangeset *changesetNode, QString withdates, bool foradd)
{
    QDateTime date = changesetNode->getDate();
    QVector<QDateTime> *datesList;
    if(withdates == DATE_EARLIER) {
        datesList =findEarlierDates(date);
    } else if (withdates == DATE_LATER) {
        datesList = findLaterDates(date);
    } else {
        datesList = m_dates;
    }

    QList<MctChangeset*> *changesetNodesList = new QList<MctChangeset*>();
    MctChangeset *changesetNode_temp = NULL;

    foreach (QDateTime date, *datesList) {
        if(m_changesetNodes->contains(date)) {
            changesetNode_temp = m_changesetNodes->value(date);
        }
        changesetNodesList->append(changesetNode_temp);
    }

    return correctChangesetNode2(changesetNode, changesetNodesList, foradd);
}

/**
 * @brief This corrects the position attributes of changes in changeset according to other changes
 * @param changesetNode The changeset node to correct.
 * @param changesetnodes The list of changeset nodes using to correct the position attributes.
 * @param foradd True if the changesetNode is intended to be added.
 * @return Returns with the parent ID
 */
int MctAbstractGraph::correctChangesetNode2(MctChangeset *changesetNode, QList<MctChangeset *> *changesetnodes, bool foradd)
{
    QList<MctChange*> *changes = changesetNode->getChanges();
    int parentId = 0;
    foreach (MctChangeset* node, *changesetnodes) {
        if(node == NULL) {
            continue;
        }

        QList<MctChange*> *changestemp = node->getChanges();
        parentId = node->getParentId();

        foreach (MctChange *changeNode, *changes) {
            foreach (MctChange *changetemp, *changestemp) {
                correctChangeNode(changeNode, changetemp, foradd);
            }
        }
    }
    changesetNode->setCorrected(true);
    return parentId;
}

/**
 * @brief This corrects the position attributes of change node according to previous changes.
 * @param changeNode The change node to correct.
 * @param prevChangeNode The changeNode in order to correct the change node.
 * @param foradd True if the changesetNode is intended to be added.
 * @return Returns with the corrected change node.
 */
MctChange* MctAbstractGraph::correctChangeNode(MctChange *changeNode, MctChange *prevChangeNode, bool foradd)
{
    MctPosition * pos = changeNode->getPosition();
    MctPosition * apos = pos;
// TODO: what? check python code...
//    while (true) {
//        apos = apos->getAnchoredPos();
//        if(apos == NULL) {
//            break;
//        }
//    }

    MctPosition * prevpos = prevChangeNode->getPosition();
    MctPosition * movedpos = NULL;
    MctPosition * prevmovedpos = NULL;

    MctPosition * pos_orig = new MctPosition(*pos);
    MctPosition * movedpos_orig = NULL;

    if(changeNode->getChangeType() == MctChangeTypes::MovedString || changeNode->getChangeType() == MctChangeTypes::MovedStringInTable) {
        movedpos = changeNode->getMovedPosition();
        movedpos_orig = new MctPosition(*movedpos);
    }

    if(prevChangeNode->getChangeType() == MctChangeTypes::MovedString || prevChangeNode->getChangeType() == MctChangeTypes::MovedStringInTable) {
        prevmovedpos = prevChangeNode->getMovedPosition();
    }

    pos = correctPosition(pos, prevpos, prevChangeNode->getChangeType(), pos_orig, foradd);

    if( prevmovedpos != NULL) {
        if(pos != NULL) {
            pos = correctPosition(pos, prevmovedpos, MctChangeTypes::AddedString, pos_orig, foradd);    //first remove than add string in case MovedString
        } else {
            return NULL;
        }
    }

    if( movedpos != NULL) {
        movedpos = correctPosition(movedpos, prevpos, prevChangeNode->getChangeType(), movedpos_orig, foradd);
    }
    if(prevmovedpos != NULL && movedpos != NULL) {
        movedpos = correctPosition(movedpos, prevmovedpos, MctChangeTypes::AddedString, movedpos_orig, foradd); //first remove than add string in case MovedString
    }

    //if changes are overlapping, return empty change
    if (pos == NULL) {
        return NULL;
    }

    changeNode->setPosition(pos);

    if(movedpos != NULL) {
        changeNode->setMovedPosition(movedpos);
    } else {
        return NULL;
    }

    return changeNode;
}

/**
 * @brief This corrects the paragraph attributes of position structure, when the order of two changes are exchanged.
 * @param pos The position class to be corrected.
 * @param prevpos The reference position class used in the exchange with the other position class pos.
 * @param prevchangetype The change type of the reference position class.
 */
void MctAbstractGraph::correctParInPosition(MctPosition *pos, MctPosition *prevpos, MctChangeTypes prevchangetype)
{
    if(pos == NULL or prevpos == NULL) {
        return;
    }

    bool preceeding_paragraph = false;

    //determine whether prev pos is connected to precceding paragraph than pos_orig
    if(pos->getStartPar() >= prevpos->getEndPar()) {
        preceeding_paragraph = true;
    }

    //adjusting paragraph numbers
    if(preceeding_paragraph) {
        ulong diff = prevpos->getEndPar() - prevpos->getStartPar();
        if(prevchangetype == MctChangeTypes::AddedString) {
            pos->setStartPar(pos->getStartPar() + diff);
            pos->setEndPar(pos->getEndPar() + diff);
        } else if (prevchangetype == MctChangeTypes::RemovedString) {
            pos->setStartPar(pos->getStartPar() - diff);
            pos->setEndPar(pos->getEndPar() - diff);
        } else if (prevchangetype == MctChangeTypes::MovedString) {
            pos->setStartPar(pos->getStartPar() - diff);
            pos->setEndPar(pos->getEndPar() - diff);
        } else if (prevchangetype == MctChangeTypes::ParagraphBreak) {
            pos->setStartPar(pos->getStartPar() + 1);
            pos->setEndPar(pos->getEndPar() + 1);
        } else if (prevchangetype == MctChangeTypes::DelParagraphBreak) {
            pos->setStartPar(pos->getStartPar() - 1);
            pos->setEndPar(pos->getEndPar() - 1);
        }
    }
}

/**
 * @brief This corrects the position structure.
 * @param pos The position class to correct.
 * @param prevpos The position class used to correct the pos position class.
 * @param prevchangetype The change type of the previous change.
 * @param pos_orig The original position to correct.
 * @param foradd True if the changesetNode related to pos is intended to be added.
 * @return Returns the corrected position
 */
MctPosition * MctAbstractGraph::correctPosition(MctPosition *pos, MctPosition *prevpos, MctChangeTypes prevchangetype, MctPosition *pos_orig, bool foradd)
{
    if (pos == NULL) {
        return NULL;
    }

    if(pos_orig == NULL) {
        pos_orig = new MctPosition(*pos);
    }

    int sgn = foradd ? 1 : -1;

    //determine wheter overlapping changes
    /*bool paragraph_overlapping = arePositionsOverlapping(prevpos, pos_orig);
    bool preceeding_paragraph = false;
    //determine whether prev pos is before pos_orig or not
    if( ! paragraph_overlapping) {
        if(comparePositions(prevpos, pos_orig) == 1) {
            preceeding_paragraph = true;
        } else {
            return pos;
        }
    } else {
        return pos;
    }*/

    if((comparePositions(prevpos, pos_orig) != 1 && !foradd) || prevpos == NULL) {
        return pos;
    }

    //adjusting paragraph and character numbers    
    long diff = sgn*(prevpos->getEndPar() - prevpos->getStartPar());
    bool samePosition = prevpos->getStartChar() == pos_orig->getStartChar() && prevpos->getStartPar() == pos_orig->getStartPar();
    bool foraddChar = (foradd && samePosition);
    if(prevchangetype == MctChangeTypes::AddedString || prevchangetype == MctChangeTypes::AddedStringInTable) {
        pos->setStartPar(pos->getStartPar() + diff);
        pos->setEndPar(pos->getEndPar() + diff);
        bool sameParPrevOrEqChar = prevpos->getEndPar() == pos_orig->getStartPar() && prevpos->getEndChar() <= pos_orig->getStartChar();
        bool notEmptyPar = pos_orig->getEndChar() != 0;
        if(sameParPrevOrEqChar && notEmptyPar || foraddChar) {
            long chardiff = sgn*(prevpos->getEndChar() - prevpos->getStartChar());
            if (!foradd){
                pos->setStartChar(pos->getStartChar() + chardiff);
                bool onePar = pos_orig->getStartPar() == pos_orig->getEndPar();
                if(onePar) {
                    pos->setEndChar(pos->getEndChar() + chardiff);
                }
            }
        }
    } else if(prevchangetype == MctChangeTypes::RemovedString || prevchangetype == MctChangeTypes::RemovedStringInTable ||
              prevchangetype == MctChangeTypes::MovedString || prevchangetype == MctChangeTypes::MovedStringInTable) {
        pos->setStartPar(pos->getStartPar() - diff);
        pos->setEndPar(pos->getEndPar() - diff);
        if(prevpos->getEndPar() == pos_orig->getStartPar() && prevpos->getEndChar() < pos_orig->getStartChar()) {
            long chardiff = sgn*(prevpos->getEndChar() - prevpos->getStartChar());
            pos->setStartChar(pos->getStartChar() - chardiff);
            if(pos_orig->getStartPar() == pos_orig->getEndPar()) {
                pos->setEndChar(pos->getEndChar() - chardiff);
            }
        }
        //TODO: a pythonban itt AddedTable volt AddedTextTable helyett
    } else if(prevchangetype == MctChangeTypes::AddedTextTable || prevchangetype == MctChangeTypes::AddedTextTableInTable ||
              prevchangetype == MctChangeTypes::ParagraphBreak || prevchangetype == MctChangeTypes::ParagraphBreakInTable) {
        pos->setStartPar(pos->getStartPar() + sgn);
        pos->setEndPar(pos->getEndPar() + sgn);
        bool sameParPrevOrEqChar = prevpos->getEndPar() == pos_orig->getStartPar() && prevpos->getEndChar() <= pos_orig->getStartChar();
        bool notEmptyPar = (prevpos->getEndChar() != 0) || (foradd && pos->getEndChar() != 0);
        if((sameParPrevOrEqChar && notEmptyPar) || foraddChar) {
            long chardiff = sgn*(prevpos->getStartChar());
            bool diffIsPositive = pos->getStartChar() - chardiff <= pos->getStartChar() + chardiff;
                if (diffIsPositive && !foradd){
		            pos->setStartChar(pos->getStartChar() - chardiff);
		            if(pos_orig->getStartPar() == pos_orig->getEndPar()) {
		                pos->setEndChar(pos->getEndChar() - chardiff);
                }
            }
        }
        //TODO: in the python code there was RemovedTable instead of RemovedTextTable
    } else if(prevchangetype == MctChangeTypes::RemovedTextTable || prevchangetype == MctChangeTypes::RemovedTextTableInTable ||
              prevchangetype == MctChangeTypes::DelParagraphBreak || prevchangetype == MctChangeTypes::DelParagraphBreakInTable) {
        pos->setStartPar(pos->getStartPar() - sgn);
        pos->setEndPar(pos->getEndPar() - sgn);
        if(prevpos->getEndPar() == pos_orig->getStartPar() && prevpos->getEndChar() < pos_orig->getStartChar()) {
            long chardiff = sgn*(prevpos->getStartChar());
            pos->setStartChar(pos->getStartChar() + chardiff);
            if(pos_orig->getStartPar() == pos_orig->getEndPar()) {
                pos->setEndChar(pos->getEndChar() + chardiff);
            }
        }
    }

    return pos;
}

/**
 * @brief This removes changeset from the graph without position correction of the other changesets
 * @param changesetnode The changeset node to remove.
 * @param clearchanges  Whether clear the list of changes or not.
 */
void MctAbstractGraph::removeChangeset(MctChangeset *changesetnode, bool clearchanges)
{
    qDebug() << "removing changeset";
    QDateTime date = changesetnode->getDate();
    QDateTime givendate = m_dates->last();

    int idx = m_dates->size() - 1;
    for (int i=0; i < m_dates->size(); i++) {
        QDateTime idate = m_dates->at(i);
        if(dateComapre(date, idate) == 0) {
            givendate = idate;
            idx = i;
        }
    }

    // Removing children changes
    if(clearchanges) {
        changesetnode->clearChangeset();
    }

    // Updating parents of changesetNode
    QList<ulong> *parents = changesetnode->getParents();
    foreach (ulong parent, *parents) {
        QDateTime date_tmp = getDateFromId(parent);
        MctChangeset *changesetnode_parent = getChangeset(date_tmp);
        if (changesetnode_parent != NULL)
            changesetnode_parent->removeChildId(changesetnode->getId());
    }

    // Removing changesetNode
    m_dates->remove(idx);
    m_root.removeChild(changesetnode->getChangeSetNode());
    m_changesetNodes->remove(givendate);
}

/**
 * @brief This removes changeset from the graph without position correction of the other changesets
 * @param date The date of the wanted changesetNode.
 * @return Returns with the removed changesetNode.
 */
MctChangeset * MctAbstractGraph::popChangeset(QDateTime date)
{
    int idx = -1;
    QDateTime givendate;
    for(int i=0; i<m_dates->size(); i++){
        if(dateComapre(date, m_dates->at(i)) == 0) {
            idx = i;
            givendate = m_dates->at(i);
            break;
        }
    }

    m_dates->remove(idx);

    //removing changeset Node
/// FIXME: in the python code there was a delete, but the changesetNode wasn't defined
qCritical() << "This function is suspicious";
///    root.removeChild(changesetNode.getChangeSet());
    MctChangeset* tmp = m_changesetNodes->value(givendate);

    return tmp;
}

bool MctAbstractGraph::removeDir(const QString & dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

/**
 * @brief This exports the graph to undo.xml/redo.xml
 */
void MctAbstractGraph::exportGraph(QDomDocument *document)
{
    qDebug() << "temporary exporting graph to: " << m_fileName;
    QFile file(m_fileName);
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    out << document->toString();
    qDebug() << document->toString();
    file.close();

    qDebug() << "exporting graph to backuped file: " << m_odtFile;

    QString outfilename = MctStaticData::instance()->tempMctFile(m_odtFile);
    qDebug() << "exporting graph to: " << outfilename;

    KZip *zfile;

    QFileInfo fi(outfilename);
    if(fi.exists()) {
        zfile = new KZip(outfilename);
    } else {
        zfile = new KZip(m_odtFile);
    }

    zfile->open(QIODevice::ReadOnly);
    QString outfilename_temp = fi.path() + QDir::separator() + "temp_mct_" + fi.fileName();

    KZip *zfileout = new KZip(outfilename_temp);
    zfileout->open(QIODevice::WriteOnly);

    bool found = false;
    QStringList entries = zfile->directory()->entries();
    foreach (QString item, entries) {
        if (item == m_redoOrUndo + ".xml") {
            zfileout->addLocalFile(m_fileName, m_redoOrUndo + ".xml");
            found = true;
        } else {
            const KArchiveEntry * kentry = zfile->directory()->entry(item);
            const KZipFileEntry * zipEntry = static_cast<const KZipFileEntry *>(kentry);
            if(zipEntry->isFile()) {
                qDebug() << "copy item: " << item;
                zfileout->writeFile(item, QString(""), QString(""), zipEntry->data().constData(), zipEntry->data().size());
            } else if(zipEntry->isDirectory()) {
                const KArchiveDirectory *currentDir = dynamic_cast<const KArchiveDirectory*>(kentry);
                QString tmp_dir = "mct_tmp" + QString::number(QDateTime::currentMSecsSinceEpoch());
                currentDir->copyTo(tmp_dir);
                QDir dir(tmp_dir);
                if(item == "META-INF") {
                    regManifest(tmp_dir + "/manifest.xml");
                }
                zfileout->addLocalDirectory(tmp_dir,kentry->name());
                removeDir(tmp_dir);
            }
        }
    }

    if(!found) {
         zfileout->addLocalFile(m_fileName, m_redoOrUndo + ".xml");
    }

    zfile->close();
    zfileout->close();

    QFile f(m_odtFile);
    f.remove();

    QFile::rename(outfilename_temp, m_odtFile);

    qDebug() << m_redoOrUndo << " graph exported";
}

/**
 * @brief This register undo.xml/redo.xml files in the manifest.xml
 * @param filename The name of the copied manifest file
 */
void MctAbstractGraph::regManifest(QString filename)
{
    qDebug() << "registering manifest";
    QDomDocument *manifest_tree = new QDomDocument(filename);

    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QByteArray xml = file.readAll();
    manifest_tree->setContent(xml);
    file.close();

    QDomElement rootNode = manifest_tree->firstChildElement();
    QStringList tmplist = rootNode.tagName().split(":");
    QString manifest = tmplist.first() + ":";

    QDomNodeList childs = rootNode.childNodes();

    bool found = false;
    for(uint i = 0; i < childs.length(); i++) {
        QDomNode node = childs.at(i);
        if(node.isElement()) {
            QDomElement elem = node.toElement();
            QString full_path = elem.attribute(manifest+"full-path");
            if(full_path == m_redoOrUndo+".xml") {
                found = true;
                break;
            }
        }
    }

    if(!found) {
        QString tag = manifest + "file-entry";
        QDomElement elem = manifest_tree->createElement(tag);
        elem.setAttribute(manifest+"media-type", "text/xml");
        elem.setAttribute(manifest+"full-path", m_redoOrUndo+".xml");
        rootNode.appendChild(elem);

        file.open(QIODevice::WriteOnly);
        QTextStream out(&file);
        out << manifest_tree->toString();
        file.close();
    }
}

/**
 * @brief This gives the current number of revisions
 * @return The current number of revisions
 */
ulong MctAbstractGraph::getCurrentRevision()
{
    return m_dates->size();
}

/**
 * @brief This function makes position class from string extracted from the XML node.
 * @param change The XML node.
 * @param moved True if moved position is wanted, False otherwise
 * @return Returns with the created position class.
 */
MctPosition* MctAbstractGraph::getPosFromXML(QDomElement change, bool moved)
{
    return MctStaticData::instance()->getPosFromElementreeNode(change, moved);
}

void MctAbstractGraph::addTableDataToPos(QDomElement change, MctChange *changeNode, bool moved)
{
    MctNode * changeEntity = changeNode->getChangeEntity();
    MctTable * tableChangeEntity = dynamic_cast<MctTable*> (changeEntity);
    if(tableChangeEntity == NULL) {
        qCritical() << "changeEntity in not a derived class of MctTable";
        return;
    }
    tableChangeEntity->convertCellName2CellPos();
    if(tableChangeEntity->tableName().isNull()) {
        tableChangeEntity->setTableName("");
    }
    if(tableChangeEntity->cellName().isNull()) {
        tableChangeEntity->setCellName("");
    }

    QString starttag;
    QString endtag;

    if(moved) {
        starttag = MctStaticData::MPOSSTART;
        endtag = MctStaticData::MPOSEND;
    } else {
        starttag = MctStaticData::POSSTART;
        endtag = MctStaticData::POSEND;
    }

    QString posstringstart = change.attribute(starttag);
    posstringstart = MctStaticData::POSSEPARATOR + tableChangeEntity->tableName() + MctStaticData::POSSEPARATOR
            + QString::number(tableChangeEntity->row()) + MctStaticData::POSSEPARATOR + QString::number(tableChangeEntity->col()) + posstringstart;

    change.setAttribute(starttag, posstringstart);

    QString posstringend = change.attribute(endtag);
    posstringend = MctStaticData::POSSEPARATOR + tableChangeEntity->tableName() + MctStaticData::POSSEPARATOR
            + QString::number(tableChangeEntity->row()) + MctStaticData::POSSEPARATOR + QString::number(tableChangeEntity->col()) + posstringend;
    change.setAttribute(endtag, posstringend);
}

/**
 * @brief This recursively adds position attribute to the node of the graph
 * @param change The change node in the ElementTree
 * @param pos
 * @param moved If Ture, moved position is affected.
 */
void MctAbstractGraph::addPos2change(QDomElement *change, MctPosition *pos, bool moved)
{
    MctStaticData::instance()->addPos2change(change, pos, moved);
}

template <typename T> void MctAbstractGraph::reverseList(QList<T> **list)
{
    QList<T> *result = new QList<T>;
    result->reserve((*list)->size());
    std::reverse_copy((*list)->begin(), (*list)->end(), std::back_inserter(*result));
    delete *list;
    *list = result;
}

QList<MctChange *>* MctAbstractGraph::findMovedChanges(QList<MctChange *> *changes)
{
    MctChange * change = NULL;
    MctChange * nextChange = NULL;
    MctChange * movedChange = NULL;
    QList<MctChange *> *tmpchanges = new QList<MctChange *>();
    int i = 0;
    bool moved = false;
    while(i < changes->length() - 1) {
        moved = false;
        change = changes->at(i);
        nextChange = changes->at(i+1);
        /*if(change->getChangeType() == MctChangeTypes::AddedString && nextChange->getChangeType() == MctChangeTypes::RemovedString) {
            MctStringChange *changeEntity = dynamic_cast<MctStringChange*>(change->getChangeEntity());
            MctStringChange *nextChangeEntity = dynamic_cast<MctStringChange*>(nextChange->getChangeEntity());
            //ugyanaz a szöveg lett beszúrva ami előtt törölve lett -> át lett mozgatva a szöveg
            if(changeEntity->getString() == nextChangeEntity->getString()) {
                movedChange = new MctChange(change->getPosition(), MctChangeTypes::MovedString, changeEntity, nextChange->getPosition());
                tmpchanges->append(movedChange);
                moved = true;
            }
            i += 2;
        } else */
        if (change->getChangeType() == MctChangeTypes::RemovedString && nextChange->getChangeType() == MctChangeTypes::AddedString) {
            MctStringChange *changeEntity = dynamic_cast<MctStringChange*>(change->getChangeEntity());
            MctStringChange *nextChangeEntity = dynamic_cast<MctStringChange*>(nextChange->getChangeEntity());
            //ugyanaz a szöveg lett beszúrva ami előtt törölve lett -> át lett mozgatva a szöveg
            if(changeEntity->getString() == nextChangeEntity->getString()) {
                movedChange = new MctChange(change->getPosition(), MctChangeTypes::MovedString, changeEntity, nextChange->getPosition());
                tmpchanges->append(movedChange);
                moved = true;
                i += 2;
            } else {
                ++i;
            }
        } else {
            ++i;
        }
        if(!moved) {
            tmpchanges->append(change);            
        }
    }

    if(i == changes->length() - 1) {    // there is an extra element at the end to add
        tmpchanges->append(changes->at(i));
    }

    return tmpchanges;
}
