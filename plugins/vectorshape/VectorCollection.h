/* This file is part of the KDE project
 * Copyright (C) 2007, 2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2010       Inge Wallin <inge@lysator.liu.se>
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
#ifndef VECTORCOLLECTION_H
#define VECTORCOLLECTION_H

#include <KoDataCenterBase.h>
#include <QObject>

class QUrl;
class KoStore;
class VectorData;

/**
 * A collection of VectorData objects to allow loading and saving them
 * all together to the KoStore.  It also makes sure that if the same
 * image is added to the collection that they share the internal data
 * structure.
 */
class VectorCollection : public QObject, public KoDataCenterBase
{
    Q_OBJECT
public:
    enum ResouceManager {
        //FIXME: Should probably be renumbered.  But to what?
        ResourceId = 75208282
    };
    /// constructor
    VectorCollection(QObject *parent = 0);
    virtual ~VectorCollection();

    /// reimplemented
    bool completeLoading(KoStore *store);

    /**
     * Save all vectors to the store which are in the context
    * @return returns true if save was successful (no vector files failed).
     */
    bool completeSaving(KoStore *store, KoXmlWriter *manifestWriter, KoShapeSavingContext *context);

    /**
     * Create a data object for the vector data.
     * The collection will create a vector data in a way that if there is an
     * existing data object with the same vector file the returned VectorData will
     * share its data.
     * @param url a valid, local url to point to a vector file on the filesystem.
     * @see VectorData::isValid()
     */
    VectorData *createExternalVectorData(const QUrl &url);

    /**
     * Create a data object for the vector data.
     * The collection will create an image data in a way that if there is an
     * existing data object with the same vector the returned VectorData will
     * share its data.
     * @param href the name of the vector inside the store.
     * @param store the KoStore object.
     * @see VectorData::isValid()
     */
    VectorData *createVectorData(const QString &href, KoStore *store);

    void add(const VectorData &data);
    void remove(const VectorData &data);
    void removeOnKey(qint64 vectorDataKey);

    /**
     * Get the number of vectors inside the collection
     */
    int size() const;
    /**
     * Get the number of vectors inside the collection
     */
    int count() const;
    
    // FIXME: This is never used.  What is it for?  Ask Casper since
    //        this is copied from the video shape.
    int saveCounter;

private:
    VectorData *cacheVector(VectorData *data);

    class Private;
    Private * const d;
};

#endif // VECTORCOLLECTION_H
