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

#ifndef MCTREDOGRAPH_H
#define MCTREDOGRAPH_H

#include "MctRedoGraphXMLfilter.h"

class ChangeEvent;
using ChangeEventList = QList<ChangeEvent*>;

/**
 * Export changes from abstract redo graph to XML
 */
class MctRedoGraph : public MctRedoGraphXMLfilter
{
public:
    MctRedoGraph(const QString &odt, KoTextDocument *m_koTextDoc);
    virtual ~MctRedoGraph();

    /**
     * convert and add changeset from undo graph into redo graph
     *
     * @param redochangeset node in the redo graph
     * @return created changeset node.
     */
    MctChangeset* addchangesetFromUndo(MctChangeset* undochangeset);

    /**
     * add a single change to changeset and XML (wrapper function)
     *
     * @param change change to be added
     * @param changeset parent changeset
     */
    void addChange(MctChange* changeNode, MctChangeset* changeset);

    /**
     * add addString to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     */
    void addString(QDomElement *xmlchange, MctChange* change);

    /**
     * add removeString to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     */
    void removeString(QDomElement *xmlchange, MctChange* change);

    /**
     * add moveString to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     */
    void moveString(QDomElement *xmlchange, MctChange* change);

    /**
     * add addParBreak to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     */
    void addParBreak(QDomElement *xmlchange, MctChange* change);

    /**
     * add delParBreak to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     */
    void delParBreak(QDomElement *xmlchange, MctChange* change);

    /**
     * add styleChange to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     */
    void styleChange(QDomElement *xmlchange, MctChange* change);

    /**
     * add styling change as subnode
     *
     * @param subnode node to insert change
     * @param propchanges style changes in a list
     * @param type style type flag @todo replace with enum
     */
    void fillPropertySubNode(QDomElement *subnode, ChangeEventList *propchanges, int type); //UNO

    /**
     * add textFrame to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     */
    void textFrame(QDomElement *xmlchange, MctChange* change);

    /**
     * add textTable to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     * @param added
     */
    void textTable(QDomElement *xmlchange, MctChange* change, bool added=true);

    /**
     * add textGraphicObject to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     * @param added
     */
    void textGraphicObject(QDomElement *xmlchange, MctChange* change, bool added=true);

    /**
     * add embeddedObject to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     */
    void embeddedObject(QDomElement *xmlchange, MctChange* change);

    /**
     * add addStringInTable to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     */
    void addStringInTable(QDomElement *xmlchange, MctChange* change);

    /**
     * add removeStringInTable to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     */
    void removeStringInTable(QDomElement *xmlchange, MctChange* change);

    /**
     * add moveStringInTable to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     */
    void moveStringInTable(QDomElement *xmlchange, MctChange* change);

    /**
     * add addParBreakInTable to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     */
    void addParBreakInTable(QDomElement *xmlchange, MctChange* change);

    /**
     * add delParBreakInTable to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     */
    void delParBreakInTable(QDomElement *xmlchange, MctChange* change);

    /**
     * add styleChangeInTable to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     */
    void styleChangeInTable(QDomElement *xmlchange, MctChange* change);

    /**
     * add textGraphicObjectInTable to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     * @param added if False other style data are also exported.
     */
    void textGraphicObjectInTable(QDomElement *xmlchange, MctChange* change, bool added=true);

    /**
     * add textTableInTable to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     * @param added if False other style data are also exported.
     */
    void textTableInTable(QDomElement *xmlchange, MctChange* change, bool added=true);

    /**
     * add rowChangeInTable to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     */
    void rowChangeInTable(QDomElement *xmlchange, MctChange* change);

    /**
     * add colChangeInTable to changeset and XML
     *
     * @param xmlchange node to insert change
     * @param change change to inserted
     */
    void colChangeInTable(QDomElement *xmlchange, MctChange* change);
};

#endif // MCTREDOGRAPH_H
