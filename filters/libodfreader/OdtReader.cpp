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


// Own
#include "OdtReader.h"

// Qt
#include <QStringList>
#include <QBuffer>

// KDE
#include <kdebug.h>
#include <klocalizedstring.h>

// Calligra
#include <KoStore.h>
#include <KoXmlStreamReader.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>  // For copyXmlElement
#include <KoOdfReadStore.h>

// Reader library
#include "OdtReaderBackend.h"
#include "OdfReaderContext.h"
#include "OdfTextReader.h"


static void prepareForOdfInternal(KoXmlStreamReader &reader);


#if 0
static int debugIndent = 0;
#define DEBUGSTART() \
    ++debugIndent; \
    DEBUG_READING("entering")
#define DEBUGEND() \
    DEBUG_READING("exiting"); \
    --debugIndent
#define DEBUG_READING(param) \
    kDebug(30503) << QString("%1").arg(" ", debugIndent * 2) << param << ": " \
    << (reader.isStartElement() ? "start": (reader.isEndElement() ? "end" : "other")) \
    << reader.qualifiedName().toString()
#else
#define DEBUGSTART() \
    // NOTHING
#define DEBUGEND() \
    // NOTHING
#define DEBUG_READING(param) \
    // NOTHING
#endif


OdtReader::OdtReader()
    : m_backend(0)
    , m_context(0)
    , m_textReader(0)
{
}

OdtReader::~OdtReader()
{
}


void OdtReader::setTextReader(OdfTextReader *textReader)
{
    m_textReader = textReader;
}

bool OdtReader::analyzeContent(OdfReaderContext *context)
{
    // Extract styles, manifest, settings, etc
    if (context->analyzeOdfFile() != KoFilter::OK) {
        return false;
    }
    kDebug(30503) << "analyze ok";
    return true;
}

bool OdtReader::readContent(OdtReaderBackend *backend, OdfReaderContext *context)
{
    kDebug(30503) << "entering";

    m_backend = backend;
    m_context = context;

    if (m_textReader) {
        m_textReader->setContext(context);
    }

    // ----------------------------------------------------------------
    // Read the body from content.xml

    KoStore *odfStore = m_context->odfStore();

    if (!odfStore->open("content.xml")) {
        kError(30503) << "Unable to open input file content.xml" << endl;
        return false;
    }
    kDebug(30503) << "open content.xml ok";

    KoXmlStreamReader reader;
    prepareForOdf(reader);

    reader.setDevice(odfStore->device());
    bool  foundContent = false;
    while (!reader.atEnd()) {
        reader.readNext();

        if (reader.isStartElement() && reader.qualifiedName() == "office:document-content") {
            foundContent = true;
            break;
        }
    }
    if (!foundContent) {
        kError(30503) << "Couldn't find the content in content.xml" << endl;
    }

    m_backend->elementOfficeDocumentcontent(reader, m_context);

    // <office:document-content> has the following children in ODF 1.2:
    //          <office:automatic-styles> 3.15.3
    //   [done] <office:body> 3.3
    //          <office:font-face-decls> 3.14
    //          <office:scripts> 3.12.
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();

        if (tagName == "office:automatic-styles") {
            // We already have the styles in the context.  No need to read them again.
            reader.skipCurrentElement();
        }
        else if (tagName == "office:font-face-decls") {
            // FIXME: Not yet implemented
            reader.skipCurrentElement();
        }
        else if (tagName == "office:scripts") {
            // FIXME: Not yet implemented
            reader.skipCurrentElement();
        }
        else if (tagName == "office:body") {
            // This is the big one.
            readElementOfficeBody(reader);
        }
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementOfficeDocumentcontent(reader, m_context);
    odfStore->close();

    return true;
}


#if 0
// This is a template function for the reader library.
// Copy this one and change the name and fill in the code.
void OdtReader::readElementNamespaceTagname(KoXmlStreamReader &reader)
{
   DEBUGSTART();

    // <namespace:tagname> has the following children in ODF 1.2:
    //   FILL IN THE CHILDREN LIKE THIS EXAMPLE (taken from office:document-content):
    //          <office:automatic-styles> 3.15.3
    //          <office:body> 3.3
    //          <office:font-face-decls> 3.14
    //          <office:scripts> 3.12.
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();

        if (tagName == "office:automatic-styles") {
            // FIXME: NYI
        }
        else if (tagName == "office:body") {
            readElementOfficeBody(reader);
        }
        ...  MORE else if () HERE
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementNamespaceTagname(reader, m_context);
    DEBUGEND();
}
#endif


void OdtReader::readElementOfficeBody(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementOfficeBody(reader, m_context);

    // <office:body> has the following children in ODF 1.2:
    //          <office:chart> 3.8,
    //          <office:database> 12.1
    //          <office:drawing> 3.5
    //          <office:image> 3.9
    //          <office:presentation> 3.6
    //          <office:spreadsheet> 3.7
    //   [done] <office:text> 3.4
    //
    // Of those only <office:text> is present in a text document (odt).
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();

        if (tagName == "office:text") {
            readElementOfficeText(reader);
        }
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementOfficeBody(reader, m_context);
    DEBUGEND();
}

void OdtReader::readElementOfficeText(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementOfficeText(reader, m_context);

    // <office:text> has the following children in ODF 1.2:
    //
    // In addition to the text level tags like <text:p> etc that can
    // be found in any textbox, table cell or similar, it has the
    // following text document children:
    //
    //          <office:forms> 13.2
    //          <table:calculation-settings> 9.4.1
    //          <table:consolidation> 9.7
    //          <table:content-validations> 9.4.4
    //          <table:database-ranges> 9.4.14
    //          <table:data-pilot-tables> 9.6.2
    //          <table:dde-links> 9.8
    //          <table:label-ranges> 9.4.10
    //          <table:named-expressions> 9.4.11
    //          <text:alphabetical-index-auto-mark-file> 8.8.3
    //          <text:dde-connection-decls> 14.6.2
    //          <text:page-sequence> 5.2
    //          <text:sequence-decls> 7.4.11
    //          <text:tracked-changes> 5.5.1
    //          <text:user-field-decls> 7.4.7
    //          <text:variable-decls> 7.4.2
    //
    // FIXME: For now, none of these are handled
    while (reader.readNextStartElement()) {
        DEBUG_READING("loop-start");

        QString tagName = reader.qualifiedName().toString();
        if (tagName == "office:forms") {
            // FIXME: NYI
            reader.skipCurrentElement();
        }
        else if (tagName == "table:calculation-settings") {
            reader.skipCurrentElement();
        }
        else if (tagName == "table:consolidation") {
            reader.skipCurrentElement();
        }
        else if (tagName == "table:content-validation") {
            reader.skipCurrentElement();
        }
        else if (tagName == "table:database-ranges") {
            reader.skipCurrentElement();
        }
        else if (tagName == "table:data-pilot-tables") {
            reader.skipCurrentElement();
        }
        else if (tagName == "table:dde-links") {
            reader.skipCurrentElement();
        }
        else if (tagName == "table:label-ranges") {
            reader.skipCurrentElement();
        }
        else if (tagName == "table:named-expressions") {
            reader.skipCurrentElement();
        }
        else if (tagName == "text:alphabetical-index-auto-mark-file") {
            reader.skipCurrentElement();
        }
        else if (tagName == "text:dde-connection-decls") {
            reader.skipCurrentElement();
        }
        else if (tagName == "text:page-sequence") {
            reader.skipCurrentElement();
        }
        else if (tagName == "text:sequence-decls") {
            reader.skipCurrentElement();
        }
        else if (tagName == "text:tracked-changes") {
            reader.skipCurrentElement();
        }
        else if (tagName == "text:user-field-decls") {
            reader.skipCurrentElement();
        }
        else if (tagName == "text:variable-decls") {
            reader.skipCurrentElement();
        }
        else {
            if (m_textReader) {
                m_textReader->readTextLevelElement(reader);
            }
            else {
                reader.skipCurrentElement();
            }
        }
        DEBUG_READING("loop-end");
    }

    m_backend->elementOfficeText(reader, m_context);
    DEBUGEND();
}


// ----------------------------------------------------------------
//                             Other functions


void OdtReader::readUnknownElement(KoXmlStreamReader &reader)
{
    DEBUGSTART();

#if 1
    // FIXME: We need to handle this.
    reader.skipCurrentElement();
#else
    if (m_context->isInsideParagraph()) {
        // readParagraphContents expect to have the reader point to the
        // contents of the paragraph so we have to read past the text:p
        // start tag here.
        reader.readNext();
        readParagraphContents(reader);
    }
    else {
        while (reader.readNextStartElement()) {
            readTextLevelElement(reader);
        }
    }
#endif

    DEBUGEND();
}
