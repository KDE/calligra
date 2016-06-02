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

class MctUndoGraphXMLfilter : public MctAbstractGraph
{
public:
    MctUndoGraphXMLfilter(const QString &m_redoOrUndo, const QString &odt, KoTextDocument *m_koTextDoc);
    virtual ~MctUndoGraphXMLfilter();

    virtual void addChangeFromXML(const QDomNode &node, MctChangeset* changeset);
    MctChange* addStringFromXML(const QDomElement &change);
    MctChange* removeStringFromXML(const QDomElement &change);
    MctChange* moveStringFromXML(const QDomElement &change);
    MctChange* addParBreakFromXML(const QDomElement &change);
    MctChange* delParBreakFromXML(const QDomElement &change);
    MctChange* styleChangeFromXML(const QDomElement &change);

    virtual void particularStyleChangeFromXML(const QDomNode &node, ChangeEventList * propchanges);    //!!!
    void listChangesFromXML(const QDomNamedNodeMap &attribs, ChangeEventList * propchanges);

    virtual MctChange* addTextFrameFromXML(const QDomElement &change);
    virtual MctChange* addTextGraphicObjectFromXML(const QDomElement &change);
    virtual MctChange* removeTextGraphicObjectFromXML(const QDomElement &change);
    virtual MctChange* addEmbeddedObjectFromXML(const QDomElement &change);
    MctChange* addTextTableFromXML(const QDomElement &change);
    MctChange* removeTextTableFromXML(const QDomElement &change);
    virtual MctChange* rowChangeFromXML(const QDomElement &change);
    virtual MctChange* colChangeFromXML(const QDomElement &change);
};

#endif // MCTUNDOGRAPHXMLFILTER_H
