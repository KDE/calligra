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

#include <QHash>

class QString;

namespace KexiWebForms { // begin namespace KexiWebForms
namespace Model {        // begin namespace Model

        class Cache {
        public:
            ~Cache() {}

            static Cache* getInstance();

            bool updateCachedPkeys(const QString&);
            const QList<uint>& getCachedPkeys(const QString&);
            int getCurrentCachePosition(const QString&, uint);
        private:
            Cache() {}

            static Cache* m_instance;
            QHash< QString, QList<uint> > cachedPkeys;
        };

} // end namespace Model
} // end namespace KexiWebForms

#endif /* KEXIWEBFORMS_MODEL_CACHE_H */
