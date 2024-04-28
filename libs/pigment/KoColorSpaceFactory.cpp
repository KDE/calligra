/*
 *  SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoColorSpaceFactory.h"

#include "DebugPigment.h"

#include <QMutexLocker>

#include "KoColorProfile.h"
#include "KoColorSpace.h"
#include "KoColorSpaceRegistry.h"

struct Q_DECL_HIDDEN KoColorSpaceFactory::Private {
    QList<KoColorProfile *> colorprofiles;
    QHash<QString, KoColorSpace *> availableColorspaces;
    QMutex mutex;
};

KoColorSpaceFactory::KoColorSpaceFactory()
    : d(new Private)
{
}

KoColorSpaceFactory::~KoColorSpaceFactory()
{
    foreach (KoColorProfile *profile, d->colorprofiles) {
        KoColorSpaceRegistry::instance()->removeProfile(profile);
        delete profile;
    }
    delete d;
}

const KoColorProfile *KoColorSpaceFactory::colorProfile(const QByteArray &rawData) const
{
    KoColorProfile *colorProfile = createColorProfile(rawData);
    if (colorProfile && colorProfile->valid()) {
        if (const KoColorProfile *existingProfile = KoColorSpaceRegistry::instance()->profileByName(colorProfile->name())) {
            delete colorProfile;
            return existingProfile;
        }
        KoColorSpaceRegistry::instance()->addProfile(colorProfile);
        d->colorprofiles.append(colorProfile);
    }
    return colorProfile;
}

const KoColorSpace *KoColorSpaceFactory::grabColorSpace(const KoColorProfile *profile)
{
    QMutexLocker l(&d->mutex);
    Q_ASSERT(profile);
    auto it = d->availableColorspaces.find(profile->name());
    KoColorSpace *cs;

    if (it == d->availableColorspaces.end()) {
        cs = createColorSpace(profile);
        if (cs) {
            d->availableColorspaces[profile->name()] = cs;
        }
    } else {
        cs = it.value();
    }

    return cs;
}
