/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <ben.martin@kogmbh.com>
   Copyright (C) 2013 Friedrich W. H. Kossebau <kossebau@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KORDFSEMANTICITEMREADER_H
#define KORDFSEMANTICITEMREADER_H

// lib
#include "KoRdfSemanticItem.h"

class QMimeData;

/**
 */
class KoRdfSemanticItemReader
{
public:
    KoRdfSemanticItemReader();
    virtual ~KoRdfSemanticItemReader();

public: // API to be implemented
    virtual QString className() const = 0;
    virtual QString classDisplayName() const = 0;

    virtual void updateSemanticItems(QList<hKoRdfSemanticItem> &semanticItems, KoDocumentRdf *rdf, QSharedPointer<Soprano::Model> m) = 0;
    virtual hKoRdfSemanticItem createSemanticItem(const KoDocumentRdf *rdf, QObject *parent) = 0;
    virtual bool acceptsMimeData(const QMimeData *mimeData) const = 0;
    virtual hKoRdfSemanticItem createSemanticItemFromMimeData(const QMimeData* mimeData, KoCanvasBase *host,
                                                              const KoDocumentRdf *rdf, QObject *parent = 0) const = 0;
};

#endif
