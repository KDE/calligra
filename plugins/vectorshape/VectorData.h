/* This file is part of the KDE project
 *
 * Copyright (C) 2007, 2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2010 Inge Wallin <inge@lysator.liu.se>
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

#ifndef VECTORDATA_H
#define VECTORDATA_H

#include <QUrl>

#include <KoShapeUserData.h>

class QIODevice;
class VectorCollection;
class VectorDataPrivate;
class KoStore;
class KTemporaryFile;

/**
 * This class is meant to represent the vector data so it can be shared between vector shapes.
 * This class inherits from KoShapeUserData which means you can set it on any KoShape using
 * KoShape::setUserData() and get it using KoShape::userData().  The vectorshape plugin
 * uses this class to show its vector data.
 */
class VectorData : public KoShapeUserData
{
    Q_OBJECT
public:
    /// Various error codes representing what has gone wrong
    enum ErrorCode {
        Success,
        OpenFailed,
        StorageFailed, ///< This is set if the vector data has to be stored on disk in a temporary file, but we failed to do so
        LoadFailed
    };

    /// default constructor, creates an invalid imageData object
    VectorData();

    /**
     * copy constructor
     * @param vectorData the other one.
     */
    VectorData(const VectorData &vectorData);
    
    /// destructor
    virtual ~VectorData();

    void setExternalVector(const QUrl &location, VectorCollection *collection = 0);
    void setVector(const QString &location, KoStore *store, VectorCollection *collection = 0);

    /**
     * Save the vector data to the param device.
     * The full file is saved.
     * @param device the device that is used to get the data from.
     * @return returns true if load was successful.
     */
    bool saveData(QIODevice &device);

    QString tagForSaving(int &counter);
    
    VectorData &operator=(const VectorData &other);

    inline bool operator!=(const VectorData &other) const { return !operator==(other); }
    bool operator==(const VectorData &other) const;

    /**
     * a unique key of the vector data generated as a MD5 hash.
     */
    qint64 m_key;

    QString suffix; // the suffix of the picture e.g. png  TODO use a QByteArray ?

    /// returns if this is a valid imageData with actual vector data present on it.
    bool isValid() const;

    ErrorCode errorCode;

    QUrl playableUrl() const;

    QString m_saveName;

protected:
    friend class VectorCollection;

    /// store the suffix based on the full filename.
    void setSuffix(const QString &fileName);

    /// take the data from \a device and store it in the temporaryFile
    void copyToTemporary(QIODevice &device);

    void clear();

    static qint64 generateKey(const QByteArray &bytes);

    enum DataStoreState {
        StateEmpty,     ///< No vector data, possibly an external vector
        StateSpooled, ///< Vector data is spooled
    };

    QUrl m_vectorLocation;
    VectorCollection *m_collection;

    // vector data store.
    DataStoreState m_dataStoreState;

    KTemporaryFile *m_temporaryFile;
};

#endif
