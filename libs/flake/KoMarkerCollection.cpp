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
#include <KoXmlReader.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>
#include <kdebug.h>

class KoMarkerCollection::Private
{
public:
    QList<KoMarker *> markers;
};

KoMarkerCollection::KoMarkerCollection(QObject *parent)
: QObject(parent)
, d(new Private)
{
    // Add no marker so the user can remove a marker from the line.
    d->markers.append(0);
}

KoMarkerCollection::~KoMarkerCollection()
{
    delete d;
}

bool KoMarkerCollection::loadOdf(KoShapeLoadingContext &context)
{
    kDebug(30006);
    QHash<QString, KoMarker*> lookupTable;

    QHash<QString, KoXmlElement*> markers = context.odfLoadingContext().stylesReader().drawStyles("marker");
    QHash<QString, KoXmlElement*>::const_iterator it(markers.constBegin());
    for (; it != markers.constEnd(); ++it) {
        KoMarker *marker = new KoMarker();
        if (marker->loadOdf(*(it.value()), context)) {
            KoMarker *m = addMarker(marker);
            lookupTable.insert(it.key(), m);
            kDebug(30006) << "loaded marker" << it.key() << marker << m;
            if (m != marker) {
                delete marker;
            }
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

KoMarker * KoMarkerCollection::addMarker(KoMarker *marker)
{
    foreach (KoMarker *m, d->markers) {
        if (marker == m) {
            return marker;
        }
        if (m && *marker == *m) {
            kDebug(30006) << "marker is the same as other";
            return m;
        }
    }
    d->markers.append(marker);
    return marker;
}
