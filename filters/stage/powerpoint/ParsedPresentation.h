/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
    const MSO::DocumentContainer *documentContainer;
    const MSO::NotesContainer *notesMaster;
    const MSO::HandoutContainer *handoutMaster;
    QVector<const MSO::MasterOrSlideContainer *> masters;
    QVector<const MSO::SlideContainer *> slides;
    QVector<const MSO::NotesContainer *> notes;

    ParsedPresentation()
    {
        documentContainer = nullptr;
        notesMaster = nullptr;
        handoutMaster = nullptr;
    }

    const MSO::MasterOrSlideContainer *getMaster(const MSO::SlideContainer *slide) const;
    bool parse(POLE::Storage &storage);
};

#endif // PARSEDPRESENTATION_H
