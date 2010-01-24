/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef PARSEDPRESENTATION_H
#define PARSEDPRESENTATION_H

#include "mso/simpleParser.h"
#include "pole.h"

class ParsedPresentation
{
public:
    PPT::CurrentUserStream currentUserStream;
    PPT::PowerPointStructs presentation;
    PPT::PicturesStream pictures;
    // map persistObjectIds to stream offsets
    QMap<quint32, quint32> persistDirectory;
    const PPT::DocumentContainer* documentContainer;
    const PPT::NotesContainer* notesMaster;
    const PPT::HandoutContainer* handoutMaster;
    QVector<const PPT::MasterOrSlideContainer*> masters;
    QVector<const PPT::SlideContainer*> slides;
    QVector<const PPT::NotesContainer*> notes;

    ParsedPresentation() {
        documentContainer = 0;
        notesMaster = 0;
        handoutMaster = 0;
    }

    const PPT::MasterOrSlideContainer* getMaster(const PPT::SlideContainer* slide) const;
    bool parse(POLE::Storage& storage);
};

#endif // PARSEDPRESENTATION_H
