/*
 * This file is part of the KDE project
 *
 * (C) Copyright 2008 by Lorenzo Villani <lvillani@binaryhelix.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KEXIWEBFORMS_MODEL_CACHE_H
#define KEXIWEBFORMS_MODEL_CACHE_H

#include <QList>
#include <QHash>

class QString;

namespace KexiWebForms { // begin namespace KexiWebForms
namespace Model {        // begin namespace Model

/*!
 * @brief A simple cache for table's primary keys
 *
 * Primary key cache (singleton)
 *
 * @note the name of this class is meaningless, expect a rename
 */
class Cache {
public:
    ~Cache() {}

    static Cache* getInstance();

    /*!
     * Update the primary key values cache for the specified table name
     * @param QString& the table name
     * @return boolean true if operation succeeded
     */
    bool updateCachedPkeys(const QString&);

    /*!
     * Get the cached primary key QList
     * @param QString& the table name
     * @return a const reference to QList<uint> which is, in fact, the cached primary key list
     */
    const QList<uint>& getCachedPkeys(const QString&);

    /*!
     * Get current position in cache
     * @param QString& the table name
     * @param uint current primary key value
     * @return an unsinged int with the current position of that primary key value in the cache
     * @note design of this function can be improved
     */
    uint getCurrentCachePosition(const QString&, uint);
private:
    Cache() {}

    static Cache* m_instance;
    QHash< QString, QList<uint> > cachedPkeys;
};

} // end namespace Model
} // end namespace KexiWebForms

#endif /* KEXIWEBFORMS_MODEL_CACHE_H */
