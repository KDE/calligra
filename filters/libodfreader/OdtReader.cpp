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
{
}

OdtReader::~OdtReader()
{
}


bool OdtReader::readContent(OdtReaderBackend *backend, OdfReaderContext *context)
{
    kDebug(30503) << "entering";

    m_backend = backend;
    m_context = context;

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
    //   <office:automatic-styles> 3.15.3
    //   <office:body> 3.3
    //   <office:font-face-decls> 3.14
    //   <office:scripts> 3.12.
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
    //   <office:automatic-styles> 3.15.3
    //   <office:body> 3.3
    //   <office:font-face-decls> 3.14
    //   <office:scripts> 3.12.
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
    //   <office:chart> 3.8,
    //   <office:database> 12.1
    //   <office:drawing> 3.5
    //   <office:image> 3.9
    //   <office:presentation> 3.6
    //   <office:spreadsheet> 3.7
    //   <office:text> 3.4
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
    //   <office:forms> 13.2
    //   <table:calculation-settings> 9.4.1
    //   <table:consolidation> 9.7
    //   <table:content-validations> 9.4.4
    //   <table:database-ranges> 9.4.14
    //   <table:data-pilot-tables> 9.6.2
    //   <table:dde-links> 9.8
    //   <table:label-ranges> 9.4.10
    //   <table:named-expressions> 9.4.11
    //   <text:alphabetical-index-auto-mark-file> 8.8.3
    //   <text:dde-connection-decls> 14.6.2
    //   <text:page-sequence> 5.2
    //   <text:sequence-decls> 7.4.11
    //   <text:tracked-changes> 5.5.1
    //   <text:user-field-decls> 7.4.7
    //   <text:variable-decls> 7.4.2
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
            readTextLevelElement(reader);
        }
        DEBUG_READING("loop-end");
    }

    m_backend->elementOfficeText(reader, m_context);
    DEBUGEND();
}


// ----------------------------------------------------------------
//                         Text level functions


// This function is a bit special since it doesn't handle a specific
// element.  Instead it handles the common child elements between a
// number of text-level elements.
//
void OdtReader::readTextLevelElement(KoXmlStreamReader &reader)
{
    DEBUGSTART();

    // We should not call any backend functions here.  That is already
    // done in the functions that call this one.

    // We define the common elements on the text level as the
    // following list.  They are the basic text level contents that
    // can be found in a text box (<draw:text-box>) but also in many
    // other places like <table:table-cell>, <text:section>,
    // <office:text>, etc.
    //
    // The ones that are not text boxes can also have other children
    // but these are the ones we have found to be the common ones.
    //
    //   <dr3d:scene> 10.5.2
    //   <draw:a> 10.4.12
    //   <draw:caption> 10.3.11
    //   <draw:circle> 10.3.8
    //   <draw:connector> 10.3.10
    //   <draw:control> 10.3.13
    //   <draw:custom-shape> 10.6.1
    //   <draw:ellipse> 10.3.9
    //   <draw:frame> 10.4.2
    //   <draw:g> 10.3.15
    //   <draw:line> 10.3.3
    //   <draw:measure> 10.3.12
    //   <draw:page-thumbnail> 10.3.14
    //   <draw:path> 10.3.7
    //   <draw:polygon> 10.3.5
    //   <draw:polyline> 10.3.4
    //   <draw:rect> 10.3.2
    //   <draw:regular-polygon> 10.3.6
    //   <table:table> 9.1.2
    //   <text:alphabetical-index> 8.8
    //   <text:bibliography> 8.9
    //   <text:change> 5.5.7.4
    //   <text:change-end> 5.5.7.3
    //   <text:change-start> 5.5.7.2
    //   <text:h> 5.1.2
    //   <text:illustration-index> 8.4
    //   <text:list> 5.3.1
    //   <text:numbered-paragraph> 5.3.6
    //   <text:object-index> 8.6
    //   <text:p> 5.1.3
    //   <text:section> 5.4
    //   <text:soft-page-break> 5.6
    //   <text:table-index> 8.5
    //   <text:table-of-content> 8.3
    //   <text:user-index> 8.7

    QString tagName = reader.qualifiedName().toString();
        
    // FIXME: Only paragraphs are handled right now.
    if (tagName == "text:h") {
        readElementTextH(reader);
    }
    else if (tagName == "text:p") {
        readElementTextP(reader);
    }
    else {
        readUnknownElement(reader);
    }

    DEBUGEND();
}


void OdtReader::readElementTextH(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTextH(reader, m_context);

    // readParagraphContents expect to have the reader point to the
    // contents of the paragraph so we have to read past the text:h
    // start tag here.
    reader.readNext();
    m_context->setIsInsideParagraph(true);
    readParagraphContents(reader);
    m_context->setIsInsideParagraph(false);

    m_backend->elementTextH(reader, m_context);
    DEBUGEND();
}

void OdtReader::readElementTextP(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTextP(reader, m_context);

    // readParagraphContents expect to have the reader point to the
    // contents of the paragraph so we have to read past the text:p
    // start tag here.
    reader.readNext();
    m_context->setIsInsideParagraph(true);
    readParagraphContents(reader);
    m_context->setIsInsideParagraph(false);

    m_backend->elementTextP(reader, m_context);
    DEBUGEND();
}


// ----------------------------------------------------------------
//                     Paragraph level functions


// This function is a bit special since it doesn't handle a specific
// element.  Instead it handles the common child elements between a
// number of paragraph-level elements.
//
void OdtReader::readParagraphContents(KoXmlStreamReader &reader)
{
    DEBUGSTART();

    // We enter this function with the reader pointing to the first
    // element *inside* the paragraph.
    //
    // We should not call any backend functions here.  That is already
    // done in the functions that call this one.

    while (!reader.atEnd() && !reader.isEndElement()) {
        DEBUG_READING("loop-start");

        if (reader.isCharacters()) {
            kDebug(30503) << "Found character data";
            m_backend->characterData(reader, m_context);
            reader.readNext();
            continue;
        }

        if (!reader.isStartElement())
            continue;

        // We define the common elements on the paragraph level as the
        // following list.  They are the basic paragraph level contents that
        // can be found in a paragraph (text:p), heading (text:h), etc
        //
        // The common paragraph level elements are the following in ODF 1.2:
        //
        //  <dr3d:scene> 10.5.2
        //  <draw:a> 10.4.12
        //  <draw:caption> 10.3.11
        //  <draw:circle> 10.3.8
        //  <draw:connector> 10.3.10
        //  <draw:control> 10.3.13
        //  <draw:custom-shape> 10.6.1
        //  <draw:ellipse> 10.3.9
        //  <draw:frame> 10.4.2
        //  <draw:g> 10.3.15
        //  <draw:line> 10.3.3
        //  <draw:measure> 10.3.12
        //  <draw:page-thumbnail> 10.3.14
        //  <draw:path> 10.3.7
        //  <draw:polygon> 10.3.5
        //  <draw:polyline> 10.3.4
        //  <draw:rect> 10.3.2
        //  <draw:regular-polygon> 10.3.6
        //  <office:annotation> 14.1
        //  <office:annotation-end> 14.2
        //  <presentation:date-time> 10.9.3.5
        //  <presentation:footer> 10.9.3.3
        //  <presentation:header> 10.9.3.1
        //  <text:a> 6.1.8
        //  <text:alphabetical-index-mark> 8.1.10
        //  <text:alphabetical-index-mark-end> 8.1.9
        //  <text:alphabetical-index-mark-start> 8.1.8
        //  <text:author-initials> 7.3.7.2
        //  <text:author-name> 7.3.7.1
        //  <text:bibliography-mark> 8.1.11
        //  <text:bookmark> 6.2.1.2
        //  <text:bookmark-end> 6.2.1.4
        //  <text:bookmark-ref> 7.7.6
        //  <text:bookmark-start> 6.2.1.3
        //  <text:change> 5.5.7.4
        //  <text:change-end> 5.5.7.3
        //  <text:change-start> 5.5.7.2
        //  <text:chapter> 7.3.8
        //  <text:character-count> 7.5.18.5
        //  <text:conditional-text> 7.7.3
        //  <text:creation-date> 7.5.3
        //  <text:creation-time> 7.5.4
        //  <text:creator> 7.5.17
        //  <text:database-display> 7.6.3
        //  <text:database-name> 7.6.7
        //  <text:database-next> 7.6.4
        //  <text:database-row-number> 7.6.6
        //  <text:database-row-select> 7.6.5
        //  <text:date> 7.3.2
        //  <text:dde-connection> 7.7.12
        //  <text:description> 7.5.5
        //  <text:editing-cycles> 7.5.13
        //  <text:editing-duration> 7.5.14
        //  <text:execute-macro> 7.7.10
        //  <text:expression> 7.4.14
        //  <text:file-name> 7.3.9
        //  <text:hidden-paragraph> 7.7.11
        //  <text:hidden-text> 7.7.4
        //  <text:image-count> 7.5.18.7
        //  <text:initial-creator> 7.5.2
        //  <text:keywords> 7.5.12
        //  <text:line-break> 6.1.5
        //  <text:measure> 7.7.13
        //  <text:meta> 6.1.9
        //  <text:meta-field> 7.5.19
        //  <text:modification-date> 7.5.16
        //  <text:modification-time> 7.5.15
        //  <text:note> 6.3.2
        //  <text:note-ref> 7.7.7
        //  <text:object-count> 7.5.18.8
        //  <text:page-continuation> 7.3.5
        //  <text:page-count> 7.5.18.2
        //  <text:page-number> 7.3.4
        //  <text:page-variable-get> 7.7.1.3
        //  <text:page-variable-set> 7.7.1.2
        //  <text:paragraph-count> 7.5.18.3
        //  <text:placeholder> 7.7.2
        //  <text:print-date> 7.5.8
        //  <text:printed-by> 7.5.9
        //  <text:print-time> 7.5.7
        //  <text:reference-mark> 6.2.2.2
        //  <text:reference-mark-end> 6.2.2.4
        //  <text:reference-mark-start> 6.2.2.3
        //  <text:reference-ref> 7.7.5
        //  <text:ruby> 6.4
        //  <text:s> 6.1.3
        //  <text:script> 7.7.9
        //  <text:sender-city> 7.3.6.13
        //  <text:sender-company> 7.3.6.10
        //  <text:sender-country> 7.3.6.15
        //  <text:sender-email> 7.3.6.7
        //  <text:sender-fax> 7.3.6.9
        //  <text:sender-firstname> 7.3.6.2
        //  <text:sender-initials> 7.3.6.4
        //  <text:sender-lastname> 7.3.6.3
        //  <text:sender-phone-private> 7.3.6.8
        //  <text:sender-phone-work> 7.3.6.11
        //  <text:sender-position> 7.3.6.6
        //  <text:sender-postal-code> 7.3.6.14
        //  <text:sender-state-or-province> 7.3.6.16
        //  <text:sender-street> 7.3.6.12
        //  <text:sender-title> 7.3.6.5
        //  <text:sequence> 7.4.13
        //  <text:sequence-ref> 7.7.8
        //  <text:sheet-name> 7.3.11
        //  <text:soft-page-break> 5.6
        //  <text:span> 6.1.7
        //  <text:subject> 7.5.11
        //  <text:tab> 6.1.4
        //  <text:table-count> 7.5.18.6
        //  <text:table-formula> 7.7.14
        //  <text:template-name> 7.3.10
        //  <text:text-input> 7.4.15
        //  <text:time> 7.3.3
        //  <text:title> 7.5.10
        //  <text:toc-mark> 8.1.4
        //  <text:toc-mark-end> 8.1.3
        //  <text:toc-mark-start> 8.1.2
        //  <text:user-defined> 7.5.6
        //  <text:user-field-get> 7.4.9
        //  <text:user-field-input> 7.4.10
        //  <text:user-index-mark> 8.1.7
        //  <text:user-index-mark-end> 8.1.6
        //  <text:user-index-mark-start> 8.1.5
        //  <text:variable-get> 7.4.5
        //  <text:variable-input> 7.4.6
        //  <text:variable-set> 7.4.4
        //  <text:word-count> 7.5.18.4.
        //        
        // FIXME: Only very few tags are handled right now.
        QString tagName = reader.qualifiedName().toString();
        if (tagName == "text:a") {
            readElementTextA(reader);
        }
        else if (tagName == "text:span") {
            readElementTextSpan(reader);
        }
        else if (tagName == "text:s") {
            readElementTextS(reader);
        }
        else {
            readUnknownElement(reader);
        }

        // Read past the end tag of the just parsed element.
        reader.readNext();
        DEBUG_READING("loop-end");
    }

    DEBUGEND();
}


void OdtReader::readElementTextA(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTextA(reader, m_context);

    // readParagraphContents expect to have the reader point to the
    // contents of the paragraph so we have to read past the text:p
    // start tag here.
    reader.readNext();
    readParagraphContents(reader);

    m_backend->elementTextA(reader, m_context);
    DEBUGEND();
}

void OdtReader::readElementTextS(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTextS(reader, m_context);

    // This element has no child elements.
    reader.skipCurrentElement();
    m_backend->elementTextS(reader, m_context);
    DEBUGEND();
}

void OdtReader::readElementTextSpan(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTextSpan(reader, m_context);

    reader.readNext();
    readParagraphContents(reader);

    m_backend->elementTextSpan(reader, m_context);
    DEBUGEND();
}


// ----------------------------------------------------------------
//                             Other functions


void OdtReader::readUnknownElement(KoXmlStreamReader &reader)
{
    DEBUGSTART();

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
