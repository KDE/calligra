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

#ifndef MCTREDOGRAPHXMLFILTER_H
#define MCTREDOGRAPHXMLFILTER_H
#include "MctUndoGraphXMLfilter.h"

/**
 * Filter class for redo graph
 *
 * This filter is responsible for the connection between abstract graph and xml file.
 * XML read/write operations are implemented here.
 *
 * @todo refactoring and merge with MctUndoGraphXMLfilter is recommended
 */
class MctRedoGraphXMLfilter : public MctUndoGraphXMLfilter
{
public:
    /// constructor
    MctRedoGraphXMLfilter(const QString &m_redoOrUndo, const QString &odt, KoTextDocument *m_koTextDoc);
    virtual ~MctRedoGraphXMLfilter();

    /**
     * import single change from XML to abstract changeset representation (wrapper function)
     *
     * @param change node in the XML
     * @param changeset parent changeset node
     */
    void addChangeFromXML(const QDomNode &change, MctChangeset* changeset);

    /**
     * import stlye change from XML to style change list
     *
     * @param node node in XML
     * @param propchanges list of style changes
     */
    void particularStyleChangeFromXML(const QDomNode &node, ChangeEventList * propchanges);    //!!!

    /**
     * import addedTextFrame/addedTextFrameInTable change from XML
     *
     * @param change node in the XML
     * @return created abstract change node
     */
    MctChange* addTextFrameFromXML(const QDomElement &change) ;

    /**
     * import addedTextGraphicObject/addedTextGraphicObjectInTable change from XML
     *
     * @param node in the XML
     * @return created abstract change node.
     */
    MctChange* addTextGraphicObjectFromXML(const QDomElement &change);

    /**
     * import removedTextGraphicObject/removedTextGraphicObjectInTable change from XML
     *
     * @param node in the XML
     * @return created abstract change node.
     */
    MctChange* removeTextGraphicObjectFromXML(const QDomElement &change);

    /**
     * import adds addedEmbeddedObject/addedEmbeddedObjectInTable change from XML
     *
     * @param node in the XML
     * @return created abstract change node.
     */
    MctChange* addEmbeddedObjectFromXML(const QDomElement &change);

    /**
     * import addedRowInTable/removedRowInTable change from XML
     *
     * @param node in the XML
     * @return created abstract change node.
     */
    MctChange* rowChangeFromXML(const QDomElement &change);


    /**
     * import addedRowInTable/removedRowInTable change from XML
     *
     * @param node in the XML
     * @return created abstract change node.
     */
    MctChange* colChangeFromXML(const QDomElement &change);
};

#endif // MCTREDOGRAPHXMLFILTER_H
