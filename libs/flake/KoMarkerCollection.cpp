/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoMarkerCollection.h"

#include "KoMarker.h"
#include "KoMarkerSharedLoadingData.h"
#include <FlakeDebug.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfReadStore.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlReader.h>
#include <QStandardPaths>

#include <QFile>

class Q_DECL_HIDDEN KoMarkerCollection::Private
{
public:
    ~Private() = default;

    QList<QExplicitlySharedDataPointer<KoMarker>> markers;
};

KoMarkerCollection::KoMarkerCollection(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
    // Add no marker so the user can remove a marker from the line.
    d->markers.append(QExplicitlySharedDataPointer<KoMarker>(nullptr));
    // Add default markers
    loadDefaultMarkers();
}

KoMarkerCollection::~KoMarkerCollection()
{
    delete d;
}

bool KoMarkerCollection::loadOdf(KoShapeLoadingContext &context)
{
    debugFlake;
    QHash<QString, KoMarker *> lookupTable;

    const QHash<QString, KoXmlElement *> markers = context.odfLoadingContext().stylesReader().drawStyles("marker");
    loadOdfMarkers(markers, context, lookupTable);

    KoMarkerSharedLoadingData *sharedMarkerData = new KoMarkerSharedLoadingData(lookupTable);
    context.addSharedData(MARKER_SHARED_LOADING_ID, sharedMarkerData);

    return true;
}

void KoMarkerCollection::loadDefaultMarkers()
{
    // use the same mechanism for loading the markers that are available
    // per default as when loading the normal markers.
    KoOdfStylesReader markerReader;
    KoOdfLoadingContext odfContext(markerReader, nullptr);
    KoShapeLoadingContext shapeContext(odfContext, nullptr);
    KoXmlDocument doc;
    QString filePath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "calligra/styles/markers.xml");

    if (!filePath.isEmpty()) {
        QFile file(filePath);
        QString errorMessage;
        if (KoOdfReadStore::loadAndParse(&file, doc, errorMessage, filePath)) {
            markerReader.createStyleMap(doc, true);

            QHash<QString, KoMarker *> lookupTable;
            const QHash<QString, KoXmlElement *> defaultMarkers = markerReader.drawStyles("marker");
            loadOdfMarkers(defaultMarkers, shapeContext, lookupTable);
        } else {
            warnFlake << "reading of" << filePath << "failed:" << errorMessage;
        }
    } else {
        debugFlake << "markers.xml not found";
    }
}

void KoMarkerCollection::loadOdfMarkers(const QHash<QString, KoXmlElement *> &markers, KoShapeLoadingContext &context, QHash<QString, KoMarker *> &lookupTable)
{
    QHash<QString, KoXmlElement *>::const_iterator it(markers.constBegin());
    for (; it != markers.constEnd(); ++it) {
        KoMarker *marker = new KoMarker();
        if (marker->loadOdf(*(it.value()), context)) {
            KoMarker *m = addMarker(marker);
            lookupTable.insert(it.key(), m);
            debugFlake << "loaded marker" << it.key() << marker << m;
            if (m != marker) {
                delete marker;
            }
        } else {
            delete marker;
        }
    }
}

QList<KoMarker *> KoMarkerCollection::markers() const
{
    QMap<QString, KoMarker *> markerMap;
    foreach (const QExplicitlySharedDataPointer<KoMarker> &m, d->markers) {
        const auto name = m ? m->name() : QString();
        markerMap[name] = m.data();
    }
    return markerMap.values();
}

KoMarker *KoMarkerCollection::addMarker(KoMarker *marker)
{
    foreach (const QExplicitlySharedDataPointer<KoMarker> &m, d->markers) {
        if (marker == m.data()) {
            return marker;
        }
        if (m && *marker == *m) {
            debugFlake << "marker is the same as other";
            return m.data();
        }
    }
    d->markers.append(QExplicitlySharedDataPointer<KoMarker>(marker));
    return marker;
}
