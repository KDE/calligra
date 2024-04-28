/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2012-2014 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ODFREADER_H
#define ODFREADER_H

// Qt
#include <QHash>
#include <QString>

// Calligra
#include <KoXmlStreamReader.h>

// this library
#include "OdfReaderInternals.h"
#include "koodfreader_export.h"

class QSizeF;

class KoXmlWriter;
class KoStore;

class OdfReaderBackend;
class OdfReaderContext;

class OdfTextReader;
class OdfDrawReader;

/** @brief Read the XML tree of the content of an ODF file.
 *
 * The OdfReader is used to traverse (read) the contents of an ODF file using
 * an XML stream reader.  For reading a specific type of ODF, e.g. a text
 * document, you should create a new class, OdtReader that inherits this
 * class.
 *
 * For every XML element that the reading process comes across it will call a
 * specific function in a backend class: @see OdfReaderBackend.
 *
 * Before the reading process is started the ODF file will be
 * analyzed to collect some data that may be needed during the
 * read: metadata, manifest and styles are examples of this. This
 * data is stored in the so called reading context, which is kept in
 * an instance of the OdfReaderContext class.
 *
 * The context will be passed around to the backend in every call to a
 * backend callback function.
 *
 * In addition to the pre-analyzed data from the ODF file, the context
 * can be used to keep track of data that is used in the backend
 * processing such as internal links, lists of embedded data such as
 * pictures.
 */
class KOODFREADER_EXPORT OdfReader
{
public:
    OdfReader();
    virtual ~OdfReader();

    OdfTextReader *textReader() const;
    void setTextReader(OdfTextReader *textReader);

    OdfDrawReader *drawReader() const;
    void setDrawReader(OdfDrawReader *drawReader);

    bool analyzeContent(OdfReaderContext *context);

    bool readContent(OdfReaderBackend *backend, OdfReaderContext *context);

protected:
    // All readElement*() are named after the full qualifiedName of
    // the element in ODF that they handle.

    // ODF document level functions
    DECLARE_READER_FUNCTION(OfficeBody);

    // ONE of these should be reimplemented by each subclass, respectively.
    virtual DECLARE_READER_FUNCTION(OfficeText);
    virtual DECLARE_READER_FUNCTION(OfficeSpreadsheet);
    virtual DECLARE_READER_FUNCTION(OfficePresentation);

    // ----------------------------------------------------------------
    // Other functions

    void readUnknownElement(KoXmlStreamReader &reader);

protected:
    OdfReaderBackend *m_backend;
    OdfReaderContext *m_context;

    // Helper readers
    OdfTextReader *m_textReader;
    OdfDrawReader *m_drawReader;
};

#endif // ODFREADER_H
