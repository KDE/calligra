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
   Boston, MA 02110-1301, USA.
*/

#ifndef ODFREADERCONTEXT_H
#define ODFREADERCONTEXT_H

// Qt
#include <QHash>

// Calligra
#include <KoFilter.h>

// this library
#include "koodfreader_export.h"


class QSizeF;
class KoStore;
class KoOdfStyleManager;


/** @brief The OdfReaderContext contains data that is usable while reading an ODF file.
 *
 * In the process of reading content.xml of an ODF file, it is often
 * necessary to access other parts of the ODF files.  In particular,
 * the styles (both named and automatic), the manifest and the
 * metadata are such resources. The OdfReaderContext provides a
 * storage for this type of data.
 *
 * At the beginning of the traverse process, the Reader class is
 * supposed to call analyzeOdfFile() in this class. This initializes
 * the style, metadata and manifest store of the context. This data is
 * not changed during the traverse and can be considered const.
 *
 * Another use of the OdfReaderContext class is to collect data
 * during the traversal that can be used afterwards. One such example
 * is that a list of all images that are accessed in the content is
 * created. This list can be accessed afterwards by calling images().
 *
 * When you create a file format filter using a reader
 * (e.g. OdtReader or OdsReader), you should inherit this class
 * to create your own context class and extend it with storage of all
 * data that is relevant for you during the conversion. Such data
 * could be data about internal links, current list level, etc.
 *
 * @Note: At the time of writing there is only an OdtReader, no other readers.
 *
 * @see OdtReader
 * @see OdtReaderBackend
 */

class KOODFREADER_EXPORT OdfReaderContext
{
 public:
    explicit OdfReaderContext(KoStore *store);
    virtual ~OdfReaderContext();

    /** Analyze (parse) the data in an ODF file other than the content
     *  and store it internally.  This data can later be accessed by
     *  the getters such as metadata() and manifest().
     */
    KoFilter::ConversionStatus analyzeOdfFile();

    /** Return the store that is used during the parsing.
     */
    KoStore *odfStore() const;

    /** Return the styles of the ODF file.
      */
    KoOdfStyleManager *styleManager() const;

    /** Return the metadata of an ODF file.
     *
     * Format is QHash<name, value>
     * where
     *   name  is the name of the metadata tag
     *   value is its value
     *
     * This data is created before the traversal starts and can be
     * accessed during the traversal.
     */
    QHash<QString, QString> metadata() const;

    /*** Return the manifest of an ODF file.
     *
     * Format is QHash<path, type>
     * where
     *   path  is the full path of the file stored in the manifest
     *   type  is the mimetype of the file.
     *
     * This data is created before the traversal starts and can be
     * accessed during the traversal.
     */
    QHash<QString, QString> manifest() const;

    // This data changes while the parsing proceeds.
    bool isInsideParagraph() const;
    void setIsInsideParagraph(bool isInside);


    // This data below is created during the traversal and can be
    // accessed after the traversal is finished.

    /** Return a list of images and their sizes. This list is
     * collected during the traversal of an ODF file.
     *
     * The format is QHash<name, size>
     * where
     *    name   is the name of the picture inside the ODT file
     *    size   is the size in points.
     */
    QHash<QString, QSizeF>   images() const;

    //QHash<QString, QString>  mediaFiles() const;

 private:
    class Private;
    Private * const d;
};


#endif // ODFREADERCONTEXT_H
