/* This file is part of the KDE project

   Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef OPCCONTENTTYPES_H
#define OPCCONTENTTYPES_H

#include <QHash>

#include <KoFilter.h>


class QString;

class KoStore;


/**
 * @brief this class manages the content types in a file in the OPC format
 */
class OpcContentTypes
{
public:
    OpcContentTypes();
    ~OpcContentTypes();

    // FIXME: So far we only design this for writing.  In the general
    //        case we should also support lookup and reading.

    void addDefault(const QString &extension, const QString &contentType);
    void addFile(const QString &partName, const QString &contentType);

    KoFilter::ConversionStatus writeToStore(KoStore *opcStore);

private:
    // Privat functions

private:
    // data
    QHash<QString, QString>  defaults;  // Extension, ContentType
    QHash<QString, QString>  parts;  // Partname, ContentType
};

#endif // OPCCONTENTTYPES_H
