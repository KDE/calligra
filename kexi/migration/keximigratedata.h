/* This file is part of the KDE project
   Copyright (C) 2004 Adam Pigg <adam@piggz.co.uk>
   Copyright (C) 2004 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2005 Martin Ellis <kde@martinellis.co.uk>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXI_MIGRATE_DATA_H
#define KEXI_MIGRATE_DATA_H

#include "kexidb/connection.h"

class KexiProjectData;

namespace KexiMigration
{
//Use this class to store all possible options that could be used by keximigrate.
//The current members are not meant to be a definite set, for example, i envisage
//adding table/field lists if we allow only importing certain tables/fields
class KEXIMIGR_EXPORT Data
{
public:
    Data();
    ~Data();

    //! Connection data for the source database
    KexiDB::ConnectionData* source;

    //! Name of the source database
    QString sourceName;

    //! Destination project data
    KexiProjectData* destination;

//   //! Actual connection to the new database
//   KexiDB::Connection* dest;

//   //! New database name
//   QString destName;

    //! Flag to determine structure copy, or structure + data
    bool keepData;
};
}//namespace KexiMigration
#endif
