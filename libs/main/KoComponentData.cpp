/* This file is part of the KDE project
   Copyright (C) 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "KoComponentData.h"

#include "KoComponentData_p.h"

KoComponentData::KoComponentData(const KAboutData &aboutData)
    : d(new KoComponentDataPrivate(aboutData))
{
}

KoComponentData::KoComponentData(const KoComponentData &other)
    : d(other.d)
{
}

KoComponentData::~KoComponentData()
{
}

KoComponentData& KoComponentData::operator=(const KoComponentData &other)
{
    d = other.d;
    return *this;
}

bool KoComponentData::operator==(const KoComponentData &other) const
{
    const bool equals = (d == other.d);
    return equals;
}

const KAboutData& KoComponentData::aboutData() const
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


const KSharedConfig::Ptr& KoComponentData::config() const
{
    if (!d->sharedConfig) {
        d->sharedConfig = KSharedConfig::openConfig(d->aboutData.componentName() + QLatin1String("rc"));
    }

    return d->sharedConfig;
}
