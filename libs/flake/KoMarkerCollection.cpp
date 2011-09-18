/* This file is part of the KDE project
   Copyright (C) 2011 Thorsten Zachmann <zachmann@kde.org>

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

#include "KoMarkerCollection.h"

#include "KoMarker.h"
#include "KoMarkerSharedLoadingData.h"
#include "KoResourceManager.h"
#include <KoXmlReader.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>

class KoMarkerCollection::Private
{
public:
    QList<KoMarker *> markers;
};

KoMarkerCollection::KoMarkerCollection(QObject *parent)
: QObject(parent)
, d(new Private)
{
}

KoMarkerCollection::~KoMarkerCollection()
{
    delete d;
}

bool KoMarkerCollection::loadOdf(KoShapeLoadingContext &context)
{
    QHash<QString, KoMarker*> lookupTable;

    QHash<QString, KoXmlElement*> markers = context.odfLoadingContext().stylesReader().drawStyles("marker");
    QHash<QString, KoXmlElement*>::const_iterator it(markers.constBegin());
    for (; it != markers.constEnd(); ++it) {
        KoMarker *marker = new KoMarker();
        if (marker->loadOdf(*(it.value()), context)) {
            d->markers.append(marker);
            lookupTable.insert(it.key(), marker);
        }
        else {
            delete marker;
        }
    }

    KoMarkerSharedLoadingData * sharedMarkerData = new KoMarkerSharedLoadingData(lookupTable);
    context.addSharedData(MARKER_SHARED_LOADING_ID, sharedMarkerData);

    return true;
}

QList<KoMarker*> KoMarkerCollection::markers() const
{
    return d->markers;
}

void KoMarkerCollection::addMarker(KoMarker *marker)
{
    if (d->markers.contains(marker)) {
        return;
    }

    // TODO check that the same marker (content) is not yet in the list
    d->markers.append(marker);
}
