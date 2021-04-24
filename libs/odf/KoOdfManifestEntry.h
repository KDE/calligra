/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010-2011 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOODFMANIFEST_H
#define KOODFMANIFEST_H

#include "koodf_export.h"


class QString;


// A class that holds a manifest:file-entry.
class KOODF_EXPORT KoOdfManifestEntry
{
public:
    KoOdfManifestEntry(const QString &fullPath, const QString &mediatType, const QString &version);
    KoOdfManifestEntry(const KoOdfManifestEntry &other);
    ~KoOdfManifestEntry();

    KoOdfManifestEntry &operator=(const KoOdfManifestEntry &other);

    QString fullPath() const;
    void setFullPath(const QString &fullPath);

    QString mediaType() const;
    void setMediaType(const QString &mediaType);

    QString version() const;
    void setVersion(const QString &version);

private:
    class Private;
    Private * const d;
};


#endif /* KOODFMANIFEST_H */
