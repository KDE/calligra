/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>
   SPDX-FileCopyrightText: 2011 Ben Martin <monkeyiq@users.sourceforge.net> hacking for fun!

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoRdfLocationEditWidget.h"
#include "KoDocumentRdf.h"
// marble for geolocation
#ifdef CAN_USE_MARBLE
#include <marble/LatLonEdit.h>
#include <marble/MarbleWidget.h>
#endif
// KF5
#include <kdebug.h>

class KoRdfLocationEditWidgetPrivate
{
public:
#ifdef CAN_USE_MARBLE
    Marble::LatLonEdit *xlat;
    Marble::LatLonEdit *xlong;
    Marble::MarbleWidget *map;
#endif
};

KoRdfLocationEditWidget::KoRdfLocationEditWidget(QWidget *parent, Ui::KoRdfLocationEditWidget *ew)
    : QWidget(parent)
    , d(new KoRdfLocationEditWidgetPrivate())
{
    Q_UNUSED(ew);
}

KoRdfLocationEditWidget::~KoRdfLocationEditWidget()
{
    delete d;
}

#ifdef CAN_USE_MARBLE
void KoRdfLocationEditWidget::mouseMoveGeoPosition()
{
    kDebug(30015) << "KoRdfLocationEditWidget::mouseMoveGeoPosition()";
    if (d->map) {
        kDebug(30015) << "lat:" << d->map->centerLatitude() << " long:" << d->map->centerLongitude();

        d->xlat->setValue(d->map->centerLatitude());
        d->xlong->setValue(d->map->centerLongitude());
    }
}

void KoRdfLocationEditWidget::setupMap(Marble::MarbleWidget *_map, Marble::LatLonEdit *_xlat, Marble::LatLonEdit *_xlong)
{
    d->map = _map;
    d->xlat = _xlat;
    d->xlong = _xlong;
    kDebug(30015) << " map:" << d->map;

    connect(d->map, SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)), this, SLOT(mouseMoveGeoPosition()));
}
#endif
