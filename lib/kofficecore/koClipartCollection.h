/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (c) 2001 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef koClipartCollection_h
#define koClipartCollection_h

#include <qmap.h>
#include "koClipart.h"
class KoStore;

/**
 * A collection of cliparts
 */
class KoClipartCollection : public QMap<KoClipartKey, KoClipart>
{
public:
    /**
     * Looks for a clipart in the collection, returns 0L if not found.
     */
    KoClipart findClipart( const KoClipartKey &key ) const;

    /**
     * Inserts a clipart into the collection, if not already there
     */
    KoClipart insertClipart( const KoClipartKey &key, const QPicture &picture );

    /**
     * Load a clipart from a file (and insert into the collection).
     * The modification date of the file is checked, to create the key
     * for this clipart. If this key maps to an existing clipart in the
     * collection, then this picture is returned, otherwise the file is loaded.
     */
    KoClipart loadClipart( const QString &fileName );

    /**
     * Save the used cliparts from the collection into the store
     * Usually called from completeSaving().
     *
     * @param store the store in which to save the cliparts
     * @param keys the list of keys corresponding to the cliparts to save
     * @param prefix the prefix to use when saving the cliparts in the store
     * (usually: isStoredExtern() ? QString::null : url().url() + "/",
     * where 'this' is the document)
     */
    void saveToStore( KoStore * store, QValueList<KoClipartKey> keys, const QString & prefix );

    /**
     * Generate the <CLIPARTS> tag, that saves the key and the related
     * relative path in the store (e.g. pictures/picture1.wmf) for each clipart.
     */
    QDomElement KoClipartCollection::saveXML( QDomDocument &doc, QValueList<KoClipartKey> keys, const QString & prefix );

    typedef QMap<KoClipartKey, QString> StoreMap;
    /**
     * Read the <CLIPARTS> tag, and save the result (key<->store-filename associations)
     * into the QMap. You may want to 'new' a QMap in loadXML, and to use and then delete
     * it in completeLoading (to save memory).
     */
    StoreMap readXML( QDomElement &pixmapsElem, const QDateTime & defaultDateTime );
    /**
     * Read all pictures from the store, into this collection
     * The map comes from readXML above, and is used to find which pictures
     * to load, and which key to associate them.
     * @param prefix same as above (only used for old format support)
     */
    void readFromStore( KoStore * store, const StoreMap & storeMap, const QString &prefix );
};

#endif
