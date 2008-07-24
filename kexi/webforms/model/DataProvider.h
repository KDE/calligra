/* This file is part of the KDE project

   (C) Copyright 2008 by Lorenzo Villani <lvillani@binaryhelix.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KEXIWEBFORMS_DATAPROVIDER_H
#define KEXIWEBFORMS_DATAPROVIDER_H

#include <kexidb/connection.h>
#include <core/kexiprojectdata.h>
#include <core/kexiproject.h>

class QString;

namespace KexiWebForms {
    /*!
     * Global connection object
     *
     * @fixme That's a bit awful, isn't it?
     */
    extern KexiDB::Connection* gConnection;
    extern KexiProjectData* gProjectData;
    extern KexiProject* gProject;

    /*!
     * Essentially, initialize the KexiDB::Connection object
     *
     * @param const QString& Path to a KexiDB file, connection file
     * shortcut file
     *
     * @return boolean false when error occurs, true if everything went well
     */
    bool initDatabase(const QString& fileName);
}

#endif /* KEXIWEBFORMS_DATAPROVIDER_H */
