/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoMarkerSharedLoadingData.h"

#include <QString>

class KoMarkerSharedLoadingData::Private
{
public:
    QHash<QString, KoMarker *> lookupTable;
};

KoMarkerSharedLoadingData::KoMarkerSharedLoadingData(const QHash<QString, KoMarker *> &lookupTable)
    : d(new Private())
{
    d->lookupTable = lookupTable;
}

KoMarkerSharedLoadingData::~KoMarkerSharedLoadingData()
{
    delete d;
}

KoMarker *KoMarkerSharedLoadingData::marker(const QString &name) const
{
    return d->lookupTable.value(name, 0);
}
