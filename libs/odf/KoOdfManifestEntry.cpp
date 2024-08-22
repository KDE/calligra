/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "KoOdfManifestEntry.h"

#include <QString>

class Q_DECL_HIDDEN KoOdfManifestEntry::Private
{
public:
    Private() = default;

    QString fullPath; // manifest:full-path
    QString mediaType; // manifest:media-type
    QString version; // manifest:version  (isNull==true if not present)
};

// ----------------------------------------------------------------

KoOdfManifestEntry::KoOdfManifestEntry(const QString &fullPath, const QString &mediaType, const QString &version)
    : d(new Private())
{
    d->fullPath = fullPath;
    d->mediaType = mediaType;
    d->version = version;
}

KoOdfManifestEntry::KoOdfManifestEntry(const KoOdfManifestEntry &other)
    : d(new Private())
{
    d->fullPath = other.d->fullPath;
    d->mediaType = other.d->mediaType;
    d->version = other.d->version;
}

KoOdfManifestEntry::~KoOdfManifestEntry()
{
    delete d;
}

KoOdfManifestEntry &KoOdfManifestEntry::operator=(const KoOdfManifestEntry &other)
{
    d->fullPath = other.d->fullPath;
    d->mediaType = other.d->mediaType;
    d->version = other.d->version;

    return *this;
}

QString KoOdfManifestEntry::fullPath() const
{
    return d->fullPath;
}

void KoOdfManifestEntry::setFullPath(const QString &fullPath)
{
    d->fullPath = fullPath;
}

QString KoOdfManifestEntry::mediaType() const
{
    return d->mediaType;
}

void KoOdfManifestEntry::setMediaType(const QString &mediaType)
{
    d->mediaType = mediaType;
}

QString KoOdfManifestEntry::version() const
{
    return d->version;
}

void KoOdfManifestEntry::setVersion(const QString &version)
{
    d->version = version;
}
