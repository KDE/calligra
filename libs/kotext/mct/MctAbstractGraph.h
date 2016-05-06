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

#ifndef MCTABSTRACTGRAPH_H
#define MCTABSTRACTGRAPH_H

#include <QList>
#include <QMap>
#include <QDateTime>

#include <QDomDocument>

#include "MctChangeTypes.h"
#include "KoShape.h"
#include "KoProperties.h"

class MctChange;
class MctChangeset;
class MctPosition;
class MctEmbObjProperties;
class MctTableProperties;
class MctAuthor;

class KoTextDocument;

class MctAbstractGraph
{
public:
    MctAbstractGraph(QString redoOrUndo, QString odt, KoTextDocument *koTextDoc);
    virtual ~MctAbstractGraph();

    // Getter-Setterek
    QList<MctChange*> * getChangeNodes() const;
    void setChangeNodes(QList<MctChange*> *changeNodes);

    QMap<QDateTime, MctChangeset*> * getChangesetNodes() const;
    void setChangesetNodes(QMap<QDateTime, MctChangeset*> *changesetNodes);

    QVector<QDateTime> * getDates(QVector<QDateTime> *excludeDates = NULL) const;
    void setDates(QVector<QDateTime> *dates);

    QMap<ulong, QDateTime> * getIdDates() const;
    void setIdDates(QMap<ulong, QDateTime> *idDates);

    QString getRedoOrUndo() const;
    void setRedoOrUndo(QString string);

    QString getOdtFile() const;
    void setOdtFile(QString name);

    QString getFilename() const;
    void setFilename(QString name);

    QString getNodeTag() const;
    void setNodeTag(QString name);

    QDomElement getRoot() const;
    void setRoot(QDomElement root);

    QDomDocument * getDoc() const;
    void setDoc(QDomDocument *doc);

    KoTextDocument * getKoTextDoc();


    MctChangeset * getChangeset(QDateTime date);
    QList<MctChangeset*> * getChangesetList(QDateTime date = QDateTime());
    QList<MctChangeset*> * getChangesetListByDates(QList<QDateTime> *dates);

    void fillUpGraph();

    void correctBlockPosition(QDomNode *node, QMap<ulong, ulong> *indexes, bool import = true);
    void correctBlockPositionForExport(QDomDocument *document, QMap<ulong, ulong> *indexes);

    MctEmbObjProperties * createEmbObjProps(QDomElement change);
    MctEmbObjProperties * createEmbObjShapeProps(QDomElement change);
    void setShapeStyles(KoShape* shape, QDomElement change);
    KoShape * createShapeFromProps(QDomElement change);
    const KoProperties * getShapeProperties(QString type);
    MctTableProperties * createTableProps(QDomElement change);

    QDateTime getDateFromId(ulong id);
    MctChangeset * findChangeset(ulong revNum);

    void sortDates();

    MctChangeset * addChangeset(QList<MctChange*> *changes, MctAuthor* author, QDateTime date, QString comment, uint parentId = 0, bool merge = false);

    int dateComapre(QDateTime d1, QDateTime d2) ;

    void findParents(MctChangeset *changsetNode, bool merge = false);
    void correctParentsAndChildren(MctChangeset *changsetNode, bool merge=false);

    bool areChangesOverlapping2(MctChange* change1, MctChange* change2);
    bool arePositionsOverlapping(MctPosition *pos1, MctPosition *pos2);
    int comparePositions(MctPosition *pos1, MctPosition *pos2);

    bool areChangesOverlapping(MctChange *change, MctPosition *pos_orig, MctPosition *movedpos_orig);

    void addChangesetNodeWithCorr(MctChangeset *changesetNode);

    QVector<QDateTime> * findLaterDates(QDateTime date);
    QVector<QDateTime> * findEarlierDates(QDateTime date);

    void correctChangesetNodeList(MctChangeset *changesetNode);
    void correctChangesetNodeListWithDate(MctChangeset*changesetNode, QString withdates = DATE_LATER, bool foradd = true);
    int correctChangesetNode(MctChangeset*changesetNode, QString withdates = DATE_LATER, bool foradd = true);
    int correctChangesetNode2(MctChangeset *changesetNode, QList<MctChangeset*> *changesetnodes, bool foradd = true);

    MctChange* correctChangeNode(MctChange* changeNode, MctChange* prevChangeNode, bool foradd=true);
    void correctParInPosition(MctPosition *pos, MctPosition *prevpos, MctChangeTypes prevchangetype);
    MctPosition * correctPosition(MctPosition *pos, MctPosition *prevpos, MctChangeTypes prevchangetype, MctPosition *pos_orig = NULL, bool foradd=true);

    void removeChangeset(MctChangeset *changesetnode, bool clearchanges=true);
    MctChangeset * popChangeset(QDateTime date);

    virtual void addChange(MctChange* change, MctChangeset* changeset) = 0 ;
    virtual void addChangeFromXML(QDomNode change, MctChangeset* changeset) = 0;

    void exportGraph(QDomDocument *document);
    void regManifest(QString filename);

    ulong getCurrentRevision();
    MctPosition * getPosFromXML(QDomElement change, bool moved=false);

    void addTableDataToPos(QDomElement change, MctChange *changeNode, bool moved=false);
    void addPos2change(QDomElement *change, MctPosition *pos, bool moved=false);

    template <typename T> void reverseList(QList<T> **list);

    QList<MctChange *> *  findMovedChanges(QList<MctChange *> * changes);
    bool removeDir(const QString & dirName);


    static const QString DATE_EARLIER;
    static const QString DATE_LATER;
    static const QString DATE_ALL;        

protected:
    ulong _id;  //static id number which is given to the next node

    QList<MctChange*> *changeNodes;
    QMap<QDateTime, MctChangeset*> *changesetNodes;
    QVector<QDateTime> *dates;
    QMap<ulong, QDateTime> *idDates;
    QString redoOrUndo;
    QString odtFile;        //odt file name
    QString fileName;       //filename containing the XML graph
    QString nodeTag;        //Node tag (undo or redo graph)

    QDomElement root;
    QDomDocument *doc;    
    KoTextDocument *koTextDoc;
};

#endif // MCTABSTRACTGRAPH_H
