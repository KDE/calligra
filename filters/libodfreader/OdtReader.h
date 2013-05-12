/* This file is part of the KDE project

   Copyright (C) 2012-2013 Inge Wallin            <inge@lysator.liu.se>

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

#ifndef ODTREADER_H
#define ODTREADER_H

// Qt
#include <QHash>
#include <QString>

// Calligra
#include <KoXmlStreamReader.h>

// this library
#include "odfreader_export.h"


class QSizeF;

class KoXmlWriter;
class KoStore;

class OdtReaderBackend;
class OdfReaderContext;


/** @brief Read the XML tree of the content of an ODT file.
 *
 * The OdtReader is used to traverse (read) the contents of an ODT
 * file using an XML stream reader.  For every XML element that the
 * reading process comes across it will call a specific function in a
 * backend class: @see OdtReaderBackend.
 *
 * Before the reading process is started the ODT file will be
 * analyzed to collect some data that may be needed during the
 * read: metadata, manifest and styles are examples of this. This
 * data is stored in the so called reading context, which is kept in
 * an instance of the OdfReaderContext class.
 *
 * The context will be passed around to the backend in every call to a
 * backend callback function.
 *
 * In addition to the pre-analyzed data from the ODT file, the context
 * can be used to keep track of data that is used in the backend
 * processing such as internal links, lists of embedded data such as
 * pictures.
 */
class ODFREADER_EXPORT OdtReader
{
 public:
    OdtReader();
    ~OdtReader();

    bool readContent(OdtReaderBackend *backend, OdfReaderContext *context);

 protected:
    // All readElement*() are named after the full qualifiedName of
    // the element in ODF that they handle.

    // ODT document level functions
    void readElementOfficeBody(KoXmlStreamReader &reader);
    void readElementOfficeText(KoXmlStreamReader &reader);

    // ----------------------------------------------------------------
    // Text level functions: paragraphs, headings, sections, frames, objects, etc

    // Read all common text level elements like text:p, text:h, draw:frame, etc.
    void readTextLevelElements(KoXmlStreamReader &reader);

    void readElementTextH(KoXmlStreamReader &reader);
    void readElementTextP(KoXmlStreamReader &reader);

    // ----------------------------------------------------------------
    // Paragraph level functions: spans, annotations, notes, text content itself, etc.

    void readParagraphLevelElements(KoXmlStreamReader &reader);

    void readElementTextSpan(KoXmlStreamReader &reader);
    void readElementTextS(KoXmlStreamReader &reader);

    // ----------------------------------------------------------------
    // Other functions

    void readUnknownElement(KoXmlStreamReader &reader);


 private:
    OdtReaderBackend  *m_backend;
    OdfReaderContext  *m_context;
};

#endif // ODTREADER_H
