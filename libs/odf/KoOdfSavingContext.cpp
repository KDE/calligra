/* This file is part of the KDE project
   Copyright (C) 2004-2006 David Faure <faure@kde.org>
   Copyright (C) 2007-2009, 2011 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>
   Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>

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

// Own
#include "KoOdfSavingContext.h"

// Qt
#include <QMap>
#include <QUuid>

// KDE
#include <kmimetype.h>
#include <kdebug.h>

// Calligra odf library
#include <KoXmlWriter.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoElementReference.h>


class KoOdfSavingContextPrivate {
public:
    KoOdfSavingContextPrivate(KoXmlWriter&, KoGenStyles&, KoEmbeddedDocumentSaver&);
    ~KoOdfSavingContextPrivate();

    KoXmlWriter *xmlWriter;
    KoGenStyles& mainStyles;
    KoEmbeddedDocumentSaver& embeddedSaver;

    QMap<const void*, KoElementReference> references;
    QMap<QString, int> referenceCounters;
    QMap<QString, QList<const void*> > prefixedReferences;
};


KoOdfSavingContextPrivate::KoOdfSavingContextPrivate(KoXmlWriter &w,
                                                     KoGenStyles &s, KoEmbeddedDocumentSaver &e)
        : xmlWriter(&w)
        , mainStyles(s)
        , embeddedSaver(e)
{
}

KoOdfSavingContextPrivate::~KoOdfSavingContextPrivate()
{
}


// ----------------------------------------------------------------


KoOdfSavingContext::KoOdfSavingContext(KoXmlWriter &xmlWriter, KoGenStyles &mainStyles,
                                       KoEmbeddedDocumentSaver &embeddedSaver)
    : d(new KoOdfSavingContextPrivate(xmlWriter, mainStyles, embeddedSaver))
{
}

KoOdfSavingContext::~KoOdfSavingContext()
{
    delete d;
}

KoXmlWriter & KoOdfSavingContext::xmlWriter()
{
    return *d->xmlWriter;
}

void KoOdfSavingContext::setXmlWriter(KoXmlWriter &xmlWriter)
{
    d->xmlWriter = &xmlWriter;
}

KoGenStyles & KoOdfSavingContext::mainStyles()
{
    return d->mainStyles;
}

KoEmbeddedDocumentSaver &KoOdfSavingContext::embeddedSaver()
{
    return d->embeddedSaver;
}


KoElementReference KoOdfSavingContext::xmlid(const void *referent, const QString& prefix,
                                             KoElementReference::GenerationOption counter)
{
    Q_ASSERT(counter == KoElementReference::UUID || (counter == KoElementReference::Counter && !prefix.isEmpty()));

    if (d->references.contains(referent)) {
        return d->references[referent];
    }

    KoElementReference ref;

    if (counter == KoElementReference::Counter) {
        int referenceCounter = d->referenceCounters[prefix];
        referenceCounter++;
        ref = KoElementReference(prefix, referenceCounter);
        d->references.insert(referent, ref);
        d->referenceCounters[prefix] = referenceCounter;
    }
    else {
        if (!prefix.isEmpty()) {
            ref = KoElementReference(prefix);
            d->references.insert(referent, ref);
        }
        else {
            d->references.insert(referent, ref);
        }
    }

    if (!prefix.isNull()) {
        d->prefixedReferences[prefix].append(referent);
    }
    return ref;
}

KoElementReference KoOdfSavingContext::existingXmlid(const void *referent)
{
    if (d->references.contains(referent)) {
        return d->references[referent];
    }
    else {
        KoElementReference ref;
        ref.invalidate();
        return ref;
    }
}

void KoOdfSavingContext::clearXmlIds(const QString &prefix)
{

    if (d->prefixedReferences.contains(prefix)) {
        foreach(const void* ptr, d->prefixedReferences[prefix]) {
            d->references.remove(ptr);
        }
        d->prefixedReferences.remove(prefix);
    }

    if (d->referenceCounters.contains(prefix)) {
        d->referenceCounters[prefix] = 0;
    }
}
