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

/**
 * Represents a DOM document in the memory
 *
 * The MctAbstractGraph contains all the changes of the actual document.
 * Each document must have two abstract graph (undo and redo).
 */
class MctAbstractGraph
{
public:
    /**
     * constructor
     *
     * @param redoOrUndo "UndoChanges" for undo.xml, "RedoChanges" for redo.xml
     * @param odt file handle or name.
     * @param koTextDoc
     */
    MctAbstractGraph(const QString &redoOrUndo, const QString &odt, KoTextDocument *koTextDoc);
    virtual ~MctAbstractGraph();

    QList<MctChange*> *changeNodes() const;
    void setChangeNodes(QList<MctChange*> *changeNodes);

    QMap<QDateTime, MctChangeset*> *changesetNodes() const;
    void setChangesetNodes(QMap<QDateTime, MctChangeset*> *setChangesetNodes);

    QVector<QDateTime> * dates(QVector<QDateTime> *excludeDates = NULL) const;
    void setDates(QVector<QDateTime> *m_dates);

    QMap<ulong, QDateTime> * idDates() const;
    void setIdDates(QMap<ulong, QDateTime> *m_idDates);

    /// return whether the graph is redo or undo
    QString redoOrUndo() const;

    void setRedoOrUndo(const QString &string);

    /// return the filename of the odt file
    QString odtFile() const;
    void setOdtFile(const QString &name);

    /// return the filename of the graph output
    QString filename() const;
    void setFilename(const QString &name);

    /// return the value of the root xml tag
    /// @deprecated
    QString nodeTag() const;
    void setNodeTag(const QString &name);

    /// return the root of the graph
    QDomElement root() const;
    void setRoot(const QDomElement &m_root);

    QDomDocument * doc() const;
    void setDoc(QDomDocument *m_doc);

    KoTextDocument * koTextDoc();

    /**
     * return the changeset node with given date.
     *
     * @param date the date corresponding to the wanted changeset
     * @return the changesetNode corresponding to the given date.
     */
    MctChangeset * getChangeset(const QDateTime &date);

    /**
     * return the list of changesetNodes.
     *
     * If date is set, return with changesetnodes earlier than the given date.
     * @param date the date of the wanted change
     * @return the list of changesets ordered by the dates.
     */
    QList<MctChangeset*> * getChangesetList(const QDateTime &date = QDateTime());

    /**
     * return the list of changesetNodes corresponding to the list of dates.
     * @param dates list of dates.
     * @return the list of changesets ordered by the dates.
     */
    QList<MctChangeset*> * getChangesetListByDates(QList<QDateTime> *dates);

    /**
     * create embObjProps object from properties in the graph xml
     *
     * @param change the node representing the change
     * @return an object with the properties of the embedded object
     */
    MctEmbObjProperties * createEmbObjProps(const QDomElement &change);
    MctEmbObjProperties * createEmbObjShapeProps(const QDomElement &change);
    void setShapeStyles(KoShape* shape, const QDomElement &change);
    KoShape * createShapeFromProps(const QDomElement &change);
    const KoProperties * getShapeProperties(const QString &type);

    /**
     * create tableProps object from properties in the graph xml
     * @param change the node representing the change
     * @return an object with the properties of the table
     */
    MctTableProperties * createTableProps(const QDomElement &change);

    /**
     * fill the graph from input xml
     *
     * Opening a document, or starting an MCT session will call this with method.
     * The content of the related "undo.xml"/"redo.xml" files will be read in the memory.
     */
    void fillUpGraph();

    /**
     * correct the block positions of a change
     *
     * During export the paragraph numbers should be written in the xml,
     * but the blocknumber gives a false value if the document contains tables.
     * The correction is applied right before exporting to xml.
     * @param node actual change node in the xml
     * @param indexes
     * @param import
     */
    void correctBlockPosition(QDomNode *node, QMap<ulong, ulong> *indexes, bool import = true);

    /**
     * @brief wrapper function to apply correction for all nodes
     * @param document pointer to the Dom document
     * @param indexes the frame index, it is the base of the correction
     */
    void correctBlockPositionForExport(QDomDocument *document, QMap<ulong, ulong> *indexes);

    /// get the date of changeset by its Id
    QDateTime getDateFromId(ulong id);

    /**
     * @brief find a changesetnode corresponding to given revision.
     *
     * The revision number is actually the index of the revision in the data structure.
     * @param revNum the number of the revision
     * @return the changeset node, or NULL if revision does not exist.
     */
    MctChangeset * findChangeset(ulong revNum);

    /**
     * sort the list of dates
     *
     * The sorting goes from the most older (first index) to the youngest one (last index).
     */
    void sortDates();

    /**
     * add new changeset
     *
     * This will add new changeset to the end of the graph
     * without correcting the position attribute.
     * @param changes list of changes
     * @param author author of the revision
     * @param date date of the revision
     * @param comment comment of the revision
     * @param parentId parent changeset id
     * @param merge True if the adding changeset is being merged from another graph, False otherwise.
     * @return Returns with the created changesetNode
     */
    MctChangeset * addChangeset(QList<MctChange*> *changes, MctAuthor* author, const QDateTime &date, const QString &comment, uint parentId = 0, bool merge = false);

    /**
    * compare dates
    *
    * @param d1 first date
    * @param d2 second date
    * @return @c -1 if d1 earlier,  @c 0 if equal,  @c 1 if d1 later than d2
    */
    int dateComapre(const QDateTime &d1, const QDateTime &d2) ;

    /**
     * find the parents of the changesetNode.
     *
     * @param changsetnode changeset
     * @param merge True if the adding changeset is being merged from another graph, False otherwise.
     */
    void findParents(MctChangeset *changsetNode, bool merge = false);

    /**
     * corrects parent-child correspondance, when new changeset is merged into the graph
     *
     * @param changsetnode changeset node that was merged
     * @param merge True if the adding changeset is being merged from another graph, False otherwise.
     */
    void correctParentsAndChildren(MctChangeset *changsetNode, bool merge=false);

    /**
     * detect overlapping changes
     *
     * This method decid whether change1 and change2 related to embedded objects are overlaping or not.
     * @param change1 ChangeNode
     * @param change2 ChangeNode
     * @return @c true if changes are related to the same embedded object, @c false otherwise.
     */
    bool areChangesOverlapping2(MctChange* change1, MctChange* change2);

    /**
     * detect overlapping positions
     *
     * Recursively decides, whether pos1 and pos2 are overlappong or not.
     * @param pos1 position object
     * @param pos2 position object
     * @return @c true if positions are overlapping in paragraph, @c false otherwise.
     */
    bool arePositionsOverlapping(MctPosition *pos1, MctPosition *pos2);

    /**
     * compare positions
     *
     * Recursively decides, whether pos1 is before pos2 or not.
     * @param pos1 position object
     * @param pos2 position object
     * @return @c 0 if pos1 and pos2 are not in the same textcontent, @c 1 if pos1 is before pos2, @c -1 otherwise.
     */
    int comparePositions(MctPosition *pos1, MctPosition *pos2);

    /**
     * detect overlapping changes
     *
     *
     * This decides, whether change1 and change2 are overlaping or not.
     * @param change ChangeNode
     * @param pos_orig original position
     * @param movedpos_orig moved position of change
     * @return @c true if they are overlapping in paragraph, @c false otherwise.
     * @warning there is a similar function above FIXME please
     */
    bool areChangesOverlapping(MctChange *change, MctPosition *pos_orig, MctPosition *movedpos_orig);

    /**
     * add changesetNode into the graph.
     *
     * The changesetNode is corrected due to the earlier changesetNodes and the later ChangesetNodes in the graph are corrected as well.
     * @param changesetnode the new changesetNode
     */
    void addChangesetNodeWithCorr(MctChangeset *changesetNode);

    /**
     * find date keys of changesets with later date, than the given one
     * @param date date
     * @return list of later dates
     */
    QVector<QDateTime> * findLaterDates(const QDateTime &date);

    /**
     * find date keys of changesets with earlier or equal date, than the given one
     * @param date date
     * @return list of earlier or equal dates
     */
    QVector<QDateTime> * findEarlierDates(const QDateTime &date);

    /**
     * correct the position attributes of changes
     *
     * The correction is applied to all changes in the set, according to the given changeset.
     * @param changesetNode changeset
     */
    void correctChangesetNodeList(MctChangeset *changesetNode);

    /**
     * correct the position attributes of changes
     *
     * This correct the position attributes of changes in all changesets of the graph with later/earlier/all date than the given changeset according to the given changeset.
     * @param changesetNode changesetNode
     * @param withdates string: 'earlier', 'later', 'all'. Used to select dates to correct.
     * @param foradd True if the changesetNode is intended to be added.
     */
    void correctChangesetNodeListWithDate(MctChangeset*changesetNode, const QString &withdates = DATE_LATER, bool foradd = true);

    /**
     * correct the position attributes of changes
     *
     * This corrects the position attributes of changes in changeset according to previous/later/all changes in the graph.
     * @param changesetnode changesetNode
     * @param withdates string: 'earlier', 'later', 'all'. Used to select dates to correct.
     * @param foradd True if the changesetNode is intended to be added.
     * @return parent ID
     */
    int correctChangesetNode(MctChangeset*changesetNode, const QString &withdates = DATE_LATER, bool foradd = true);

    /**
     * correct the position attributes of changes
     *
     * This corrects the position attributes of changes in changeset according to other changes
     * @param changesetNode changeset node to correct.
     * @param changesetnodes list of changeset nodes using to correct the position attributes.
     * @param foradd True if the changesetNode is intended to be added.
     * @return parent ID
     */
    int correctChangesetNode2(MctChangeset *changesetNode, QList<MctChangeset*> *changesetnodes, bool foradd = true);

    /**
     * correct the position attributes of changes
     *
     * This corrects the position attributes of change node according to previous changes.
     * @param changeNode change node to correct.
     * @param prevChangeNode changeNode in order to correct the change node.
     * @param foradd True if the changesetNode is intended to be added.
     * @return corrected change node.
     */
    MctChange* correctChangeNode(MctChange* changeNode, MctChange* prevChangeNode, bool foradd=true);

    /**
     * correct the paragraph position attribute of change
     *
     * This corrects the paragraph attributes of position structure, when the order of two changes are exchanged.
     * @param pos position class to be corrected.
     * @param prevpos reference position class used in the exchange with the other position class pos.
     * @param prevchangetype change type of the reference position class.
     */
    void correctParInPosition(MctPosition *pos, MctPosition *prevpos, MctChangeTypes prevchangetype);

    /**
     * correct position structure
     *
     * @param pos position class to correct.
     * @param prevpos position class used to correct the pos position class.
     * @param prevchangetype change type of the previous change.
     * @param pos_orig original position to correct.
     * @param foradd True if the changesetNode related to pos is intended to be added.
     * @return corrected position
     */
    MctPosition * correctPosition(MctPosition *pos, MctPosition *prevpos, MctChangeTypes prevchangetype, MctPosition *pos_orig = NULL, bool foradd=true);

    /**
     * remove changeset from the graph
     *
     * This removes changeset from the graph without position correction of the other changesets
     * @param changesetnode changeset node to remove.
     * @param clearchanges  whether clear the list of changes or not.
     */
    void removeChangeset(MctChangeset *changesetnode, bool clearchanges=true);

    /**
     * pop changeset from the graph
     *
     * This removes changeset from the graph without position correction of the other changesets
     * @param date date of the wanted changesetNode.
     * @return the removed changesetNode.
     */
    MctChangeset * popChangeset(const QDateTime &date);

    virtual void addChange(MctChange* change, MctChangeset* changeset) = 0 ;
    virtual void addChangeFromXML(const QDomNode &change, MctChangeset* changeset) = 0;

    /**
     * export the graph to xml
     *
     * @param document Dom document to export into undo/redo.xml
     */
    void exportGraph(QDomDocument *document);

    /**
     * register undo.xml/redo.xml files in the manifest.xml
     *
     * @param filename name of the copied manifest file
     */
    void regManifest(const QString &filename);

    /// return the current number of revisions
    ulong getCurrentRevision();

    /**
     * export position information from XML to Position object
     *
     * @param change XML node.
     * @param moved True if moved position is wanted, False otherwise
     * @return exported position class.
     */
    MctPosition * getPosFromXML(const QDomElement &change, bool moved=false);

    /**
     * get postion of a table related change
     *
     * @param change Dom element
     * @param changeNode change representation
     * @param moved if true, moved position is affected
     */
    void addTableDataToPos(QDomElement &change, MctChange *changeNode, bool moved=false);

    /**
     * add position attribute to dom tree
     *
     * This recursively adds position attribute to the node of the graph
     * @param change change node in the ElementTree
     * @param pos position object
     * @param moved if true, moved position is affected.
     */
    void addPos2change(QDomElement *change, MctPosition *pos, bool moved=false);

    /**
     * reverse a list
     *
     * @param list list to reverse
     */
    template <typename T> void reverseList(QList<T> **list);

    /**
     * look for moved changes
     *
     * @param changes list of changes
     * @return changes which
     */
    QList<MctChange *> *  findMovedChanges(QList<MctChange *> * changes);

    /**
     * delete directory (used for remove temp dirs)
     *
     * @param dirName path to directory
     * @return result of the delete operation
     */
    bool removeDir(const QString &dirName);

    static const QString DATE_EARLIER;
    static const QString DATE_LATER;
    static const QString DATE_ALL;        

protected:
    ulong _id;  ///< static id number which is given to the next node

    QList<MctChange*> *m_changeNodes; ///< list of changes
    QMap<QDateTime, MctChangeset*> *m_changesetNodes;  ///< map of revisions
    QVector<QDateTime> *m_dates;    ///< array of revision dates
    QMap<ulong, QDateTime> *m_idDates;  ///< map of
    QString m_redoOrUndo;
    QString m_odtFile;  ///< odt file name
    QString m_fileName; ///< filename containing the XML graph
    QString m_nodeTag;  ///< root node tag (string of xml tag)

    QDomElement m_root;     ///< root Dom element
    QDomDocument *m_doc;    ///< Dom document
    KoTextDocument *m_koTextDoc;    ///< legacy document
};

#endif // MCTABSTRACTGRAPH_H
