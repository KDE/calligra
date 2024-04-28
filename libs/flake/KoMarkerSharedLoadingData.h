/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOMARKERSHAREDLOADINGDATA_H
#define KOMARKERSHAREDLOADINGDATA_H

#include "KoSharedLoadingData.h"

#include <QHash>

#define MARKER_SHARED_LOADING_ID "KoMarkerShareadLoadingId"

class KoMarker;
class QString;

class KoMarkerSharedLoadingData : public KoSharedLoadingData
{
public:
    KoMarkerSharedLoadingData(const QHash<QString, KoMarker *> &lookupTable);
    ~KoMarkerSharedLoadingData() override;

    KoMarker *marker(const QString &name) const;

private:
    class Private;
    Private *const d;
};

#endif /* KOMARKERSHAREDLOADINGDATA_H */
