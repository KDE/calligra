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

class MctRedoGraphXMLfilter : public MctUndoGraphXMLfilter
{
public:
    MctRedoGraphXMLfilter(QString redoOrUndo, QString odt, KoTextDocument *koTextDoc);
    virtual ~MctRedoGraphXMLfilter();

    void addChangeFromXML(QDomNode change, MctChangeset* changeset);

    void particularStyleChangeFromXML(QDomNode node, ChangeEventList * propchanges);    //!!!
    MctChange* addTextFrameFromXML(QDomElement change) ;
    MctChange* addTextGraphicObjectFromXML(QDomElement change);
    MctChange* removeTextGraphicObjectFromXML(QDomElement change);
    MctChange* addEmbeddedObjectFromXML(QDomElement change);

    MctChange* rowChangeFromXML(QDomElement change);
    MctChange* colChangeFromXML(QDomElement change);
};

#endif // MCTREDOGRAPHXMLFILTER_H
