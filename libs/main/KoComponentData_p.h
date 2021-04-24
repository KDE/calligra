/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOCOMPONENTDATA_P_H
#define KOCOMPONENTDATA_P_H

#include <KSharedConfig>
#include <KAboutData>

#include <QSharedData>

class KoComponentDataPrivate : public QSharedData
{
public:
    explicit KoComponentDataPrivate(const KAboutData &_aboutData)
        : aboutData(_aboutData)
        {}

    KAboutData aboutData;
    KSharedConfig::Ptr sharedConfig;
};

#endif
