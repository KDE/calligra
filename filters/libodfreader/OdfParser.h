/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ODFPARSER_H
#define ODFPARSER_H

// Qt
#include <QHash>
#include <QString>

// Calligra
#include <KoFilter.h> // For the return values.
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
