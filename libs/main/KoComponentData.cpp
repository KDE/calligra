/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoComponentData.h"

#include "KoComponentData_p.h"

KoComponentData::KoComponentData(const KAboutData &aboutData)
    : d(new KoComponentDataPrivate(aboutData))
{
}

KoComponentData::KoComponentData(const KoComponentData &other)

    = default;

KoComponentData::~KoComponentData() = default;

KoComponentData &KoComponentData::operator=(const KoComponentData &other) = default;

bool KoComponentData::operator==(const KoComponentData &other) const
{
    const bool equals = (d == other.d);
    return equals;
}

const KAboutData &KoComponentData::aboutData() const
{
    return d->aboutData;
}

QString KoComponentData::componentName() const
{
    return d->aboutData.componentName();
}

QString KoComponentData::componentDisplayName() const
{
    return d->aboutData.displayName();
}

const KSharedConfig::Ptr &KoComponentData::config() const
{
    if (!d->sharedConfig) {
        d->sharedConfig = KSharedConfig::openConfig(d->aboutData.componentName() + QLatin1String("rc"));
    }

    return d->sharedConfig;
}
