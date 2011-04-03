/* This file is part of the KDE project
 * Copyright (C) 2011 Thorsten Zachmann <zachmann@kde.org>
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

#include "KoMarkerSelector.h"

#include "KoMarker.h"
#include "KoMarkerModel.h"
#include "KoMarkerItemDelegate.h"

class KoMarkerSelector::Private
{
public:
    Private(QWidget *parent)
    : model(new KoMarkerModel(QList<KoMarker*>(), parent))
    {}

    KoMarkerModel *model;
};

KoMarkerSelector::KoMarkerSelector(QWidget *parent)
: QComboBox(parent)
, d(new Private(this))
{
    setModel(d->model);
    setItemDelegate(new KoMarkerItemDelegate(this));
}

KoMarkerSelector::~KoMarkerSelector()
{
    delete d;
}

void KoMarkerSelector::setMarker(KoMarker *marker)
{
    int index = d->model->markerIndex(marker);
    if (index >= 0) {
        setCurrentIndex(index);
    }
}

KoMarker *KoMarkerSelector::marker() const
{
    return itemData(currentIndex(), Qt::DecorationRole).value<KoMarker*>();
}

void KoMarkerSelector::updateMarkers(const QList<KoMarker*> markers)
{
    KoMarkerModel *model = new KoMarkerModel(markers, this);
    d->model = model;
    // this deletes the old model
    setModel(model);
}
