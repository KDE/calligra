/* This file is part of the KDE project

   Copyright (C) 2013 Inge Wallin            <inge@lysator.liu.se>

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

#ifndef ODFTRAVERSERCONTEXT_H
#define ODFTRAVERSERCONTEXT_H

// Qt
#include <QHash>

// Calligra
#include <KoFilter.h>

// this library
#include "odftraverse_export.h"


class QByteArray;
class QSizeF;
class QStringList;
class KoStore;


/** @brief The OdfTraverserContext contains data that is usable while parsing an ODF file.
 *
 * In the process of parsing content.xml of an ODF file, it is often
 * necessary to access other parts of the ODF files.  In particular,
 * the styles (both named and automatic), the manifest and the
 * metadata are such resources. The OdfTraverserContext provides a
 * storage for this type of data.
 *
 * At the beginning of the traverse process, the Traverser class is
 * supposed to call analyzeOdfFile() in this class. This initializes
 * the style, metadata and manifest store of the context. This data is
 * not changed during the traverse and can be considered const.
 *
 * Another use of the OdfTraverserContext class is to collect data
 * during the traversal that can be used afterwards. One such example
 * is that a list of all images that are accessed in the content is
 * created. This list can be accessed afterwards by calling images().
 *
 * When you create a file format filter using a traverser
 * (e.g. OdtTraverser or OdsTraverser), you should inherit this class
 * in your own context class and extend it with storage of all data
 * that is relevant during the conversion. Such data could be data
 * about internal links, current list level, etc.
 *
 * Note: at the time of writing, styles are not parsed and stored, and
 *       there is only an OdtTraverser, no other traversers.
 *
 * @see OdtTraverser
 * @see OdtTraverserBackend
 */

class ODFTRAVERSE_EXPORT OdfTraverserContext 
{
 public: 
    OdfTraverserContext(KoStore *store);
    virtual ~OdfTraverserContext();

    KoFilter::ConversionStatus analyzeOdfFile();

    KoStore *odfStore() const;

    // This data is created before the traversal starts and can be
    // accessed during the traversal.
    // NOTE: QHash is implicitly shared and making a copy is cheap.
    QHash<QString, QString>    metadata() const;
    QHash<QString, QString>    manifest() const;

    // This data is created during the traversal and can be accessed
    // after the traversal is finished.

    // A list of images and their sizes. This list is collected during
    // the conversion and returned from traverseContent() using an
    // outparameter.
    //
    // The format is QHash<name, size>
    // where
    //    name   is the name of the picture inside the ODT file
    //    size   is the size in points.
    //
    QHash<QString, QSizeF>   images() const;
    QHash<QString, QString>  mediaFiles() const;

 private:
    class Private;
    Private * const d;
};


#endif // ODFTRAVERSERCONTEXT_H
