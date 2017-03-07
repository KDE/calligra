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

#ifndef MCTUNDOGRAPHXMLFILTER_H
#define MCTUNDOGRAPHXMLFILTER_H
#include "MctAbstractGraph.h"
#include "MctChangeEntities.h"

/**
 * Filter class for undo graph
 *
 * This filter is responsible for the connection between abstract graph and xml file.
 * XML read/write operations are implemented here.
 *
 * @todo refactoring and merge with MctRedoGraphXMLfilter is recommended
 */
class MctUndoGraphXMLfilter : public MctAbstractGraph
{
public:
    /// constructor
    MctUndoGraphXMLfilter(const QString &m_redoOrUndo, const QString &odt, KoTextDocument *m_koTextDoc);
    virtual ~MctUndoGraphXMLfilter();

    /**
     * import single change from XML to abstract changeset representation (wrapper function)
     *
     * @param change node in the XML
     * @param changeset parent changeset node
     */
    virtual void addChangeFromXML(const QDomNode &node, MctChangeset* changeset);

    /**
     * import added string change from XML
     *
     * @param change node in the XML
     * @return created abstract change node
     */
    MctChange* addStringFromXML(const QDomElement &change);

    /**
     * import removed string change from XML
     *
     * @param change node in the XML
     * @return created abstract change node
     */
    MctChange* removeStringFromXML(const QDomElement &change);

    /**
     * import moved string change from XML
     *
     * @param change node in the XML
     * @return created abstract change node
     */
    MctChange* moveStringFromXML(const QDomElement &change);

    /**
     * import added paragraph change from XML
     *
     * @param change node in the XML
     * @return created abstract change node
     */
    MctChange* addParBreakFromXML(const QDomElement &change);

    /**
     * import removed paragraph change from XML
     *
     * @param change node in the XML
     * @return created abstract change node
     */
    MctChange* delParBreakFromXML(const QDomElement &change);

    /**
     * import addedTextFrame/addedTextFrameInTable change from XML
     *
     * @param change node in the XML
     * @return created abstract change node
     */
    MctChange* styleChangeFromXML(const QDomElement &change);

    /**
     * import stlye change from XML to style change list
     *
     * @param node node in XML
     * @param propchanges list of style changes
     */
    virtual void particularStyleChangeFromXML(const QDomNode &node, ChangeEventList * propchanges);    //!!!

    /**
     * import list related stlye change from XML to style change list
     *
     * @param attribs list attributes from XML
     * @param propchanges list of style changes
     */
    void listChangesFromXML(const QDomNamedNodeMap &attribs, ChangeEventList * propchanges);

    /**
     * import addedTextFrame/addedTextFrameInTable change from XML
     *
     * @param change node in the XML
     * @return created abstract change node
     */
    virtual MctChange* addTextFrameFromXML(const QDomElement &change);

    /**
     * import addedTextGraphicObject/addedTextGraphicObjectInTable change from XML
     *
     * @param node in the XML
     * @return created abstract change node.
     */
    virtual MctChange* addTextGraphicObjectFromXML(const QDomElement &change);

    /**
     * import removedTextGraphicObject/removedTextGraphicObjectInTable change from XML
     *
     * @param node in the XML
     * @return created abstract change node.
     */
    virtual MctChange* removeTextGraphicObjectFromXML(const QDomElement &change);

    /**
     * import adds addedEmbeddedObject/addedEmbeddedObjectInTable change from XML
     *
     * @param node in the XML
     * @return created abstract change node.
     */
    virtual MctChange* addEmbeddedObjectFromXML(const QDomElement &change);

    /**
     * import addTextTable change from XML
     *
     * @param node in the XML
     * @return created abstract change node.
     */
    MctChange* addTextTableFromXML(const QDomElement &change);

    /**
     * import removeTextTable change from XML
     *
     * @param node in the XML
     * @return created abstract change node.
     */
    MctChange* removeTextTableFromXML(const QDomElement &change);

    /**
     * import table row change from XML
     *
     * @param node in the XML
     * @return created abstract change node.
     */
    virtual MctChange* rowChangeFromXML(const QDomElement &change);

    /**
     * import table col change from XML
     *
     * @param node in the XML
     * @return created abstract change node.
     */
    virtual MctChange* colChangeFromXML(const QDomElement &change);

private:
    QString ADDED;      // to store undo logic locally
    QString REMOVED;    // to store undo logic locally
};

#endif // MCTUNDOGRAPHXMLFILTER_H
