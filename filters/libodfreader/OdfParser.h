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

#ifndef ODFPARSER_H
#define ODFPARSER_H

// Qt
#include <QString>
#include <QHash>

// Calligra
#include <KoFilter.h>           // For the return values.
                                // If we ever move this out of filters/ we should move the
                                // filterstatus return values to bools.

class KoStore;


/** @brief Provide a parser for some parts of an ODF file.
 *
 * The purpose of this class is to provide a parser for the
 * information in an ODF file outside of the actual content and
 * provide easy access to it.  This includes the manifest, the
 * metadata, settings and styles although not all of the above is
 * implemented yet.
 */
class OdfParser
{
public:
    OdfParser();
    virtual ~OdfParser();

    /** Parse the metadata.
     *
     * Format is QHash<name, value>
     * where
     *   name  is the name of the metadata tag
     *   value is its value
     *
     * @param odfStore The store where the information is fetched
     * @param metadata The result
     * @return returns KoFilter::OK if everything went well.
     * @return returns an error status otherwise.
     */
    KoFilter::ConversionStatus parseMetadata(KoStore &odfStore,
                                             // Out parameter:
                                             QHash<QString, QString> *metadata);

    /*** Parse manifest
     *
     * Format is QHash<path, type>
     * where
     *   path  is the full path of the file stored in the manifest
     *   type  is the mimetype of the file.
     *
     * @param odfStore The store where the information is fetched
     * @param metadata The result
     * @return returns KoFilter::OK if everything went well.
     * @return returns an error status otherwise.
     */
    KoFilter::ConversionStatus parseManifest(KoStore &odfStore,
                                             // Out parameter:
                                             QHash<QString, QString> *manifest);

private:
};

#endif // ODFPARSER_H
