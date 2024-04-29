/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KPRSOUNDCOLLECTION_H
#define KPRSOUNDCOLLECTION_H

#include <KoDataCenterBase.h>
#include <QObject>
#include <QStringList>
#include <QVariant>

#include "stage_export.h"

class KPrSoundData;
class KoStore;

/**
 * An collection of KPrSoundData objects to allow loading and saving them all together to the KoStore.
 */
class STAGE_EXPORT KPrSoundCollection : public QObject, public KoDataCenterBase
{
    Q_OBJECT
public:
    /// constructor
    explicit KPrSoundCollection(QObject *parent = nullptr);
    ~KPrSoundCollection() override;

    /**
     * Load all sounds from the store which have a recognized KPrSoundData::storeHref().
     * @return returns true if load was successful (no sounds failed).
     */
    bool completeLoading(KoStore *store) override;

    /**
     * Save all sounds to the store which are tagged for saving
     * and have a recognized KPrSoundData::storeHref().
     * @return returns true if save was successful (no sounds failed).
     */
    bool completeSaving(KoStore *store, KoXmlWriter *manifestWriter, KoShapeSavingContext *context) override;

    KPrSoundData *findSound(const QString &title);

    QStringList titles();

protected:
    friend class KPrSoundData;
    void addSound(KPrSoundData *image);
    void removeSound(KPrSoundData *image);

private:
    class Private;
    Private *const d;
};

Q_DECLARE_METATYPE(KPrSoundCollection *)
#endif
