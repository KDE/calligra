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

#include "generated/simpleParser.h"
#include "pole.h"
#include <QMap>

class ParsedPresentation
{
public:
    MSO::CurrentUserStream currentUserStream;
    MSO::PowerPointStructs presentation;
    MSO::PicturesStream pictures;
    MSO::SummaryInformationPropertySetStream summaryInfo;
    // map persistObjectIds to stream offsets
    QMap<quint32, quint32> persistDirectory;
    const MSO::DocumentContainer* documentContainer;
    const MSO::NotesContainer* notesMaster;
    const MSO::HandoutContainer* handoutMaster;
    QVector<const MSO::MasterOrSlideContainer*> masters;
    QVector<const MSO::SlideContainer*> slides;
    QVector<const MSO::NotesContainer*> notes;

    ParsedPresentation() {
        documentContainer = 0;
        notesMaster = 0;
        handoutMaster = 0;
    }

    const MSO::MasterOrSlideContainer* getMaster(const MSO::SlideContainer* slide) const;
    bool parse(POLE::Storage& storage);
};

#endif // PARSEDPRESENTATION_H
