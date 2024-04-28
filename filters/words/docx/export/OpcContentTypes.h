/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
    QHash<QString, QString> defaults; // Extension, ContentType
    QHash<QString, QString> parts; // Partname, ContentType
};

#endif // OPCCONTENTTYPES_H
