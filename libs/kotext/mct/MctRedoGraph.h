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

class MctRedoGraph : public MctRedoGraphXMLfilter
{
public:
    MctRedoGraph(QString odt, KoTextDocument *koTextDoc);
    virtual ~MctRedoGraph();

    MctChangeset* addchangesetFromUndo(MctChangeset* undochangeset);
    void addChange(MctChange* changeNode, MctChangeset* changeset);
    void addString(QDomElement *xmlchange, MctChange* change);
    void removeString(QDomElement *xmlchange, MctChange* change);
    void moveString(QDomElement *xmlchange, MctChange* change);
    void addParBreak(QDomElement *xmlchange, MctChange* change);
    void delParBreak(QDomElement *xmlchange, MctChange* change);
    void styleChange(QDomElement *xmlchange, MctChange* change);
    void fillPropertySubNode(QDomElement *subnode, ChangeEventList *propchanges, int type); //UNO
    void textFrame(QDomElement *xmlchange, MctChange* change);
    void textTable(QDomElement *xmlchange, MctChange* change, bool added=true);
    void textGraphicObject(QDomElement *xmlchange, MctChange* change, bool added=true);
    void embeddedObject(QDomElement *xmlchange, MctChange* change);
    void addStringInTable(QDomElement *xmlchange, MctChange* change);
    void removeStringInTable(QDomElement *xmlchange, MctChange* change);
    void moveStringInTable(QDomElement *xmlchange, MctChange* change);
    void addParBreakInTable(QDomElement *xmlchange, MctChange* change);
    void delParBreakInTable(QDomElement *xmlchange, MctChange* change);
    void styleChangeInTable(QDomElement *xmlchange, MctChange* change);
    void textGraphicObjectInTable(QDomElement *xmlchange, MctChange* change, bool added=true);
    void textTableInTable(QDomElement *xmlchange, MctChange* change, bool added=true);
    void rowChangeInTable(QDomElement *xmlchange, MctChange* change);
    void colChangeInTable(QDomElement *xmlchange, MctChange* change);
};

#endif // MCTREDOGRAPH_H
