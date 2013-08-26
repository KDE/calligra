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



bool OdtReader::readContent(OdtReaderBackend *backend, OdfReaderContext *context)
{
    kDebug(30503) << "entering";

    m_backend = backend;
    m_context = context;

    if (m_textReader) {
        m_textReader->setContext(context);
    }

    // Extract styles, manifest, settings, etc
    if (context->analyzeOdfFile() != KoFilter::OK) {
        return false;
    }
    kDebug(30503) << "analyze ok";

    // ----------------------------------------------------------------
    // Read the body from content.xml

    KoStore *odfStore = m_context->odfStore();

    if (!odfStore->open("content.xml")) {
        kError(30503) << "Unable to open input file content.xml" << endl;
        return false;
    }
    kDebug(30503) << "open content.xml ok";

    KoXmlStreamReader reader;
    prepareForOdfInternal(reader);

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


// FIXME: Remove this function when it is exported from libs/odf/KoXmlStreamReader.cpp
//
static void prepareForOdfInternal(KoXmlStreamReader &reader)
{
    // This list of namespaces is taken from KoXmlNs.cpp
    // Maybe not all of them are expected in an ODF document?
    reader.addExpectedNamespace("office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
    reader.addExpectedNamespace("meta", "urn:oasis:names:tc:opendocument:xmlns:meta:1.0");
    reader.addExpectedNamespace("config", "urn:oasis:names:tc:opendocument:xmlns:config:1.0");
    reader.addExpectedNamespace("text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
    reader.addExpectedNamespace("table", "urn:oasis:names:tc:opendocument:xmlns:table:1.0");
    reader.addExpectedNamespace("draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
    reader.addExpectedNamespace("presentation", "urn:oasis:names:tc:opendocument:xmlns:presentation:1.0");
    reader.addExpectedNamespace("dr3d", "urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0");
    reader.addExpectedNamespace("chart", "urn:oasis:names:tc:opendocument:xmlns:chart:1.0");
    reader.addExpectedNamespace("form", "urn:oasis:names:tc:opendocument:xmlns:form:1.0");
    reader.addExpectedNamespace("script", "urn:oasis:names:tc:opendocument:xmlns:script:1.0");
    reader.addExpectedNamespace("style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
    reader.addExpectedNamespace("number", "urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0");
    reader.addExpectedNamespace("manifest", "urn:oasis:names:tc:opendocument:xmlns:manifest:1.0");
    reader.addExpectedNamespace("anim", "urn:oasis:names:tc:opendocument:xmlns:animation:1.0");

    reader.addExpectedNamespace("math", "http://www.w3.org/1998/Math/MathML");
    reader.addExpectedNamespace("svg", "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
    reader.addExpectedNamespace("fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
    reader.addExpectedNamespace("dc", "http://purl.org/dc/elements/1.1/");
    reader.addExpectedNamespace("xlink", "http://www.w3.org/1999/xlink");
    reader.addExpectedNamespace("VL", "http://openoffice.org/2001/versions-list");
    reader.addExpectedNamespace("smil", "urn:oasis:names:tc:opendocument:xmlns:smil-compatible:1.0");
    reader.addExpectedNamespace("xhtml", "http://www.w3.org/1999/xhtml");
    reader.addExpectedNamespace("xml", "http://www.w3.org/XML/1998/namespace");

    reader.addExpectedNamespace("calligra", "http://www.calligra.org/2005/");
    reader.addExpectedNamespace("officeooo", "http://openoffice.org/2009/office");
    reader.addExpectedNamespace("ooo", "http://openoffice.org/2004/office");

    reader.addExpectedNamespace("delta", "http://www.deltaxml.com/ns/track-changes/delta-namespace");
    reader.addExpectedNamespace("split", "http://www.deltaxml.com/ns/track-changes/split-namespace");
    reader.addExpectedNamespace("ac", "http://www.deltaxml.com/ns/track-changes/attribute-change-namespace");

    // This list of namespaces is taken from KoXmlReader::fixNamespace()
    // They were generated by old versions of OpenOffice.org.
    reader.addExtraNamespace("office",    "http://openoffice.org/2000/office");
    reader.addExtraNamespace("text",      "http://openoffice.org/2000/text");
    reader.addExtraNamespace("style",     "http://openoffice.org/2000/style");
    reader.addExtraNamespace("fo",        "http://www.w3.org/1999/XSL/Format");
    reader.addExtraNamespace("table",     "http://openoffice.org/2000/table");
    reader.addExtraNamespace("drawing",   "http://openoffice.org/2000/drawing");
    reader.addExtraNamespace("datastyle", "http://openoffice.org/2000/datastyle");
    reader.addExtraNamespace("svg",       "http://www.w3.org/2000/svg");
    reader.addExtraNamespace("chart",     "http://openoffice.org/2000/chart");
    reader.addExtraNamespace("dr3d",      "http://openoffice.org/2000/dr3d");
    reader.addExtraNamespace("form",      "http://openoffice.org/2000/form");
    reader.addExtraNamespace("script",    "http://openoffice.org/2000/script");
    reader.addExtraNamespace("meta",      "http://openoffice.org/2000/meta");
    reader.addExtraNamespace("config",    "http://openoffice.org/2001/config");
    reader.addExtraNamespace("pres",      "http://openoffice.org/2000/presentation");
    reader.addExtraNamespace("manifest",  "http://openoffice.org/2001/manifest");
}
