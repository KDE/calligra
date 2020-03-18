/* This file is part of the KDE project

   Copyright (C) 2012-2014 Inge Wallin            <inge@lysator.liu.se>

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

#ifndef ODSREADER_H
#define ODSREADER_H

// Qt
#include <QHash>
#include <QString>

// Calligra
#include <KoXmlStreamReader.h>

// this library
#include "koodfreader_export.h"
#include "OdfReader.h"
#include "OdfReaderInternals.h"


class QSizeF;

class KoXmlWriter;
class KoStore;

class OdsReaderBackend;
class OdfReaderContext;

class OdfTextReader;


/** @brief Read the XML tree of the content of an ODS file.
 *
 * The OdsReader is used to traverse (read) the contents of an ODS
 * file using an XML stream reader.  For every XML element that the
 * reading process comes across it will call a specific function in a
 * backend class: @see OdsReaderBackend.
 *
 * Before the reading process is started the ODS file will be
 * analyzed to collect some data that may be needed during the
 * read: metadata, manifest and styles are examples of this. This
 * data is stored in the so called reading context, which is kept in
 * an instance of the OdfReaderContext class.
 *
 * The context will be passed around to the backend in every call to a
 * backend callback function.
 *
 * In addition to the pre-analyzed data from the ODS file, the context
 * can be used to keep track of data that is used in the backend
 * processing such as internal links, lists of embedded data such as
 * pictures.
 */
class KOODFREADER_EXPORT OdsReader : public OdfReader
{
 public:
    OdsReader();
    ~OdsReader() override;

 protected:
    // All readElement*() are named after the full qualifiedName of
    // the element in ODF that they handle.

    // ODS document level functions
    DECLARE_READER_FUNCTION(OfficeSpreadsheet) override;

 private:
    // Not much here. Most are already in OdfReader.
};

#endif // ODSREADER_H
