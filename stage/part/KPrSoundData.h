/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2008 C. Boemann <cbo@boemann.dk>
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
#ifndef KPRSOUNDDATA_H
#define KPRSOUNDDATA_H

#include "stage_export.h"

#include <QString>

class KPrSoundCollection;
class QIODevice;

/**
 * Class meant to hold sound data so it can be shared between shapes.
 * In Stage shapes can have click actions attached to them. One such action is playing sound.
 * The binary data for those sounds are saved in this class.
 */

/* 
 * TODO needs a file for playing, store it as a tmp file
 */
class STAGE_EXPORT KPrSoundData {
public:
    /**
     * The storage location
     */
    enum StorageLocation {
        SaveRelativeUrl,        ///< in the odf use a relative (to document) xlink:href, if possible
        SaveAbsoluteUrl,        ///< in the odf use a fully specified xlink:href
        SaveInStore            ///< Save the sound data in the ODF store
    };

    /**
     * constructor
     * @param collection the sound collection which will do the loading of the sound data for us.
     * @param href the url of the sound in the store.
     */
    explicit KPrSoundData(KPrSoundCollection *collection, const QString &href = QString());

    /**
     * copy constructor using ref-counting.
     * @param soundData the other one.
     */
    KPrSoundData(const KPrSoundData &soundData);
    /// destructor
    ~KPrSoundData();

    /**
     * Tags this sound to be saved and returns the href for reference in the xml.
     * @return returns the url-like location this sound will be saved to.
     */
    QString tagForSaving();

    /// returns the url-like location
    QString storeHref() const;

    /// returns the url-like location of the tmp file
    QString nameOfTempFile() const;

    /// returns the title of the sound (for now its the basename part of the filename
    QString title() const;

    /**
     * Load the sound data from the param device.
     * Note that it will copy the data to a temp-file and postpone loading it until the phonon plays it.
     * @para device the device that is used to get the data from.
     * @return returns true if load was successful.
     */
    bool loadFromFile(QIODevice *device);

    /**
     * Save the sound data to the param device.
     * The full file is saved.
     * @para device the device that is used to get the data from.
     * @return returns true if load was successful.
     */
    bool saveToFile(QIODevice *device);

    /**
     * Return whether this sound have been tagged for saving.
     * @return returns true if this sound should be saved.
     */
    bool isTaggedForSaving();


    bool operator==(const KPrSoundData &other) const;

    /**
     * Get the collection used
     */
    KPrSoundCollection * soundCollection();

private:
    class Private;
    Private * const d;
};

#endif

