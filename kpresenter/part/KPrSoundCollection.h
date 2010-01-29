/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
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
#ifndef KPRSOUNDCOLLECTION_H
#define KPRSOUNDCOLLECTION_H

#include <KoDataCenterBase.h>
#include <QStringList>
#include <QObject>
#include <QVariant>

#include "kpresenter_export.h"

class KPrSoundData;
class KoStore;

/**
 * An collection of KPrSoundData objects to allow loading and saving them all together to the KoStore.
 */
class KPRESENTER_EXPORT KPrSoundCollection : public QObject, public KoDataCenterBase {
public:
    /// constructor
    KPrSoundCollection(QObject *parent = 0);
    ~KPrSoundCollection();

    /**
     * Load all sounds from the store which have a recognized KPrSoundData::storeHref().
     * @return returns true if load was successful (no sounds failed).
     */
    bool completeLoading(KoStore *store);

    /**
     * Save all sounds to the store which are tagged for saving
     * and have a recognized KPrSoundData::storeHref().
     * @return returns true if save was successful (no sounds failed).
     */
    bool completeSaving(KoStore *store, KoXmlWriter * manifestWriter, KoShapeSavingContext * context );

    KPrSoundData *findSound(QString title);

    QStringList titles();

protected:
    friend class KPrSoundData;
    void addSound(KPrSoundData *image);
    void removeSound(KPrSoundData *image);


private:
    class Private;
    Private * const d;
};

Q_DECLARE_METATYPE(KPrSoundCollection*)
#endif
