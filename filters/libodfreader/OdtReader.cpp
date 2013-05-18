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


OdtReader::OdtReader()
{
}

OdtReader::~OdtReader()
{
}


bool OdtReader::readContent(OdtReaderBackend *backend, OdfReaderContext *context)
{
    m_backend = backend;
    m_context = context;

    if (context->analyzeOdfFile() != KoFilter::OK) {
        return false;
    }

    // ----------------------------------------------------------------
    // Read the body from content.xml

    KoStore *odfStore = m_context->odfStore();

    if (!odfStore->open("content.xml")) {
        kError(30503) << "Unable to open input file content.xml" << endl;
        return false;
    }

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

    // FIXME: Call backend function for starting the document here.

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

    // FIXME: Call backend function for ending the document here.
    odfStore->close();

    return true;
}


#if 0
// This is a template function for the reader library.
// Copy this one and change the name and fill in the code.
void OdtReader::readElementNamespaceTagname(KoXmlStreamReader &reader)
{
    m_backend->elementNamespaceTagname(reader, m_context);

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
}
#endif


void OdtReader::readElementOfficeBody(KoXmlStreamReader &reader)
{
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
}

void OdtReader::readElementOfficeText(KoXmlStreamReader &reader)
{
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
        QString tagName = reader.qualifiedName().toString();
        
        if (tagName == "office:forms") {
            // FIXME: NYI
            reader.skipCurrentElement();
        }
        else if (tagName == "...") {
            // HANDLE CONTENTS HERE
        }
        else {
            readTextLevelElements(reader);
        }
    }

    m_backend->elementOfficeText(reader, m_context);
}


// ----------------------------------------------------------------
//                         Text level functions


// This function is a bit special since it doesn't handle a specific
// element.  Instead it handles the common child elements between a
// number of text-level elements.
//
void OdtReader::readTextLevelElements(KoXmlStreamReader &reader)
{
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
}


void OdtReader::readElementTextH(KoXmlStreamReader &reader)
{
    m_backend->elementTextH(reader, m_context);

    m_context->setIsInsideParagraph(true);
    readParagraphLevelElements(reader);
    m_context->setIsInsideParagraph(false);

    m_backend->elementTextH(reader, m_context);
}

void OdtReader::readElementTextP(KoXmlStreamReader &reader)
{
    m_backend->elementTextP(reader, m_context);

    m_context->setIsInsideParagraph(true);
    readParagraphLevelElements(reader);
    m_context->setIsInsideParagraph(false);

    m_backend->elementTextP(reader, m_context);
}


// ----------------------------------------------------------------
//                     Paragraph level functions


// This function is a bit special since it doesn't handle a specific
// element.  Instead it handles the common child elements between a
// number of paragraph-level elements.
//
void OdtReader::readParagraphLevelElements(KoXmlStreamReader &reader)
{
    // We should not call any backend functions here.  That is already
    // done in the functions that call this one.

    if (reader.isCharacters()) {
        m_backend->characterData(reader, m_context);
    }

    // We define the common elements on the paragraph level as the
    // following list.  They are the basic paragraph level contents that
    // can be found in a paragraph (text:p), heading (text:h), etc
    //

    QString tagName = reader.qualifiedName().toString();
        
    // FIXME: Add a list of paragraph level elements here
    // FIXME: Only very few tags are handled right now.
    if (tagName == "text:span") {
        readElementTextSpan(reader);
    }
    else if (tagName == "text:s") {
        readElementTextS(reader);
    }
    else {
        readUnknownElement(reader);
    }
}


void OdtReader::readElementTextS(KoXmlStreamReader &reader)
{
    m_backend->elementTextS(reader, m_context);

    // FIXME: NYI
    reader.skipCurrentElement();

    m_backend->elementTextS(reader, m_context);
}

void OdtReader::readElementTextSpan(KoXmlStreamReader &reader)
{
    m_backend->elementTextSpan(reader, m_context);

    // FIXME: NYI
    reader.skipCurrentElement();

    m_backend->elementTextSpan(reader, m_context);
}


// ----------------------------------------------------------------
//                             Other functions


void OdtReader::readUnknownElement(KoXmlStreamReader &reader)
{
    while (reader.readNextStartElement()) {
        if (m_context->isInsideParagraph()) {
            readParagraphLevelElements(reader);
        }
        else {
            readTextLevelElements(reader);
        }
    }
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
