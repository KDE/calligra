/*  This file is part of the KDE libraries
 *  Copyright (C) 1999 Torben Weis <weis@kde.org>
 *  Copyright (C) 2006 David Faure <faure@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation;
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#ifndef KSERVICETYPEPROFILE_P_H
#define KSERVICETYPEPROFILE_P_H

#include <QMap>

/**
 * @internal
 */
class KServiceTypeProfileEntry
{
public:
    explicit KServiceTypeProfileEntry() {}

    /**
     * Add a service to this profile.
     * @param _service the name of the service
     * @param _preference the user's preference value, must be positive,
     *              bigger is better
     * @param _allow_as_default true if the service should be used as
     *                 default
     */
    void addService( const QString& service, int preference = 1 ) {
        m_mapServices.insert( service, preference );
    }

    /**
     * Map of all services for which we have assessments.
     * Key: service ID
     * Value: preference
     */
    QMap<QString,int> m_mapServices;
};

#endif /* KSERVICETYPEPROFILE_P_H */

