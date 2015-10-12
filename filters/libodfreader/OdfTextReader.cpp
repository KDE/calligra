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
#include "OdfTextReader.h"

// Qt
#include <QStringList>
#include <QBuffer>

// KF5
#include <klocalizedstring.h>

// Calligra, libodf{,2}
#include <KoXmlNS.h>
#include <KoXmlStreamReader.h>
#include <KoXmlUtils.h>

// Reader library
#include "OdfReader.h"
#include "OdfTextReaderBackend.h"
#include "OdfReaderContext.h"
#include "OdfDrawReader.h"
#include "OdfReaderDebug.h"


#if 1
static int debugIndent = 0;
#define DEBUGSTART() \
    ++debugIndent; \
    DEBUG_READING("entering")
#define DEBUGEND() \
    DEBUG_READING("exiting"); \
    --debugIndent
#define DEBUG_READING(param) \
    debugOdfReader << QString("%1").arg(" ", debugIndent * 2) << param << ": " \
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


OdfTextReader::OdfTextReader()
    : m_parent(0)
    , m_backend(0)
    , m_context(0)
{
}

OdfTextReader::~OdfTextReader()
{
}


// ----------------------------------------------------------------


void OdfTextReader::setParent(OdfReader *parent)
{
    m_parent = parent;
}

void OdfTextReader::setBackend(OdfTextReaderBackend *backend)
{
    m_backend = backend;
}

void OdfTextReader::setContext(OdfReaderContext *context)
{
    m_context = context;
}


// ----------------------------------------------------------------


#if 0
// This is a template function for the reader library.
// Copy this one and change the name and fill in the code.
void OdfTextReader::readElementNamespaceTagname(KoXmlStreamReader &reader)
{
   DEBUGSTART();
    m_backend->elementNamespaceTagname(reader, m_context);

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
            reader.skipCurrentElement();
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




// ----------------------------------------------------------------
//                         Text level functions


// This function is a bit special since it doesn't handle a specific
// element.  Instead it handles the common child elements between a
// number of text-level elements.
//
void OdfTextReader::readTextLevelElement(KoXmlStreamReader &reader)
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
    //          <dr3d:scene> 10.5.2
    //          <draw:a> 10.4.12
    //          <draw:caption> 10.3.11
    //          <draw:circle> 10.3.8
    //          <draw:connector> 10.3.10
    //          <draw:control> 10.3.13
    //          <draw:custom-shape> 10.6.1
    //          <draw:ellipse> 10.3.9
    //          <draw:frame> 10.4.2
    //          <draw:g> 10.3.15
    //          <draw:line> 10.3.3
    //          <draw:measure> 10.3.12
    //          <draw:page-thumbnail> 10.3.14
    //          <draw:path> 10.3.7
    //          <draw:polygon> 10.3.5
    //          <draw:polyline> 10.3.4
    //          <draw:rect> 10.3.2
    //          <draw:regular-polygon> 10.3.6
    // All of the above are sent to the draw reader.
    //
    //   [done] <table:table> 9.1.2
    //          <text:alphabetical-index> 8.8
    //          <text:bibliography> 8.9
    //          <text:change> 5.5.7.4
    //          <text:change-end> 5.5.7.3
    //          <text:change-start> 5.5.7.2
    //   [done] <text:h> 5.1.2
    //          <text:illustration-index> 8.4
    //   [done] <text:list> 5.3.1
    //          <text:numbered-paragraph> 5.3.6
    //          <text:object-index> 8.6
    //   [done] <text:p> 5.1.3
    //          <text:section> 5.4
    //   [done] <text:soft-page-break> 5.6
    //          <text:table-index> 8.5
    //          <text:table-of-content> 8.3
    //          <text:user-index> 8.7

    QString tagName = reader.qualifiedName().toString();
        
    if (reader.prefix() == "draw" || reader.prefix() == "dr3d") {
	OdfDrawReader *drawReader = m_parent->drawReader();
	if (drawReader) {
	    drawReader->readCommonGraphicsElements(reader);
	}
	else {
	    reader.skipCurrentElement();
	}
    } // draw | dr3d namespace
    else if (tagName == "text:h") {
        readElementTextH(reader);
    }
    else if (tagName == "text:p") {
        readElementTextP(reader);
    }
    else if (tagName == "text:list") {
        readElementTextList(reader);
    }
    else if (tagName == "table:table") {
        readElementTableTable(reader);
    }
    else if (tagName == "text:soft-page-break") {
        readElementTextSoftPageBreak(reader);
    }
    else {
        readUnknownElement(reader);
    }

    DEBUGEND();
}


void OdfTextReader::readElementTextH(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTextH(reader, m_context);

    // The function readParagraphContents() expects to have the reader
    // point to the contents of the paragraph so we have to read past
    // the text:h start tag here.
    reader.readNext();
    m_context->setIsInsideParagraph(true);
    readParagraphContents(reader);
    m_context->setIsInsideParagraph(false);

    m_backend->elementTextH(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementTextP(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTextP(reader, m_context);

    // The function readParagraphContents() expects to have the reader
    // point to the contents of the paragraph so we have to read past
    // the text:p start tag here.
    reader.readNext();
    m_context->setIsInsideParagraph(true);
    readParagraphContents(reader);
    m_context->setIsInsideParagraph(false);

    m_backend->elementTextP(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementTextList(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTextList(reader, m_context);

    // <text:list> has the following children in ODF 1.2:
    //   [done] <text:list-header> 5.3.3
    //   [done] <text:list-item> 5.3.4

    m_context->setIsInsideParagraph(true);
    while (reader.readNextStartElement()) {
        DEBUG_READING("loop-start");

        QString tagName = reader.qualifiedName().toString();
        //debugOdfReader << "list child:" << tagName;
        if (tagName == "text:list-item") {
            readElementTextListItem(reader);
        }
        else if (tagName == "text:list-header") {
            readElementTextListHeader(reader);
        }
        else {
            readUnknownElement(reader);
        }
        DEBUG_READING("loop-end");
    }
    m_context->setIsInsideParagraph(false);

    m_backend->elementTextList(reader, m_context);
    DEBUGEND();
}

// ----------------------------------------------------------------
//                             Tables


void OdfTextReader::readElementTableTable(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTableTable(reader, m_context);

    // <table:table> has the following children in ODF 1.2:
    //          <office:dde-source> 14.6.5
    //          <office:forms> 13.2
    //   [done] <table:desc> 9.1.14
    //          <table:named-expressions> 9.4.11
    //          <table:scenario> 9.2.7
    //          <table:shapes> 9.2.8
    //   [done] <table:table-column> 9.1.6
    //   [done] <table:table-column-group> 9.1.10
    //   [done] <table:table-columns> 9.1.12
    //   [done] <table:table-header-columns> 9.1.11
    //   [done] <table:table-header-rows> 9.1.7
    //   [done] <table:table-row> 9.1.3
    //   [done] <table:table-row-group> 9.1.9
    //   [done] <table:table-rows> 9.1.8
    //          <table:table-source> 9.2.6
    //   [done] <table:title> 9.1.13
    //   [done] <text:soft-page-break> 5.6
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
        if (tagName == "table:table-column") {
            readElementTableTableColumn(reader);
        }
        else if (tagName == "table:table-column-group") {
            readElementTableTableColumnGroup(reader);
        }
        else if (tagName == "table:table-columns") {
            readElementTableTableColumns(reader);
        }
        else if (tagName == "table:table-header-columns") {
            readElementTableTableHeaderColumns(reader);
        }
        else if (tagName == "table:table-header-rows") {
            readElementTableTableHeaderRows(reader);
        }
        else if (tagName == "table:table-row") {
            readElementTableTableRow(reader);
        }
        else if (tagName == "table:table-row-group") {
            readElementTableTableRowGroup(reader);
        }
        else if (tagName == "table:table-rows") {
            readElementTableTableRows(reader);
        }
        else if (tagName == "table:title") {
	    QString value;
	    readCharacterData(reader, value);
	    m_backend->textVariable(tagName, value);
        }
        else if (tagName == "table:desc") {
	    QString value;
	    readCharacterData(reader, value);
	    m_backend->textVariable(tagName, value);
        }
        else if (tagName == "text:soft-page-break") {
            readElementTextSoftPageBreak(reader);
        }
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementTableTable(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementTableTableColumnGroup(KoXmlStreamReader &reader)
{
   DEBUGSTART();
    m_backend->elementTableTableColumnGroup(reader, m_context);

    // <table:table-column-group> has the following children in ODF 1.2:
    //          <table:table-header-columns> 9.1.11
    //          <table:table-column> 9.1.6
    //          <table:table-column-group> 9.1.10
    //          <table:table-columns> 9.1.12
    //
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
	if (tagName == "table:table-header-columns") {
            readElementTableTableHeaderColumns(reader);
        }
        else if (tagName == "table:table-column") {
            readElementTableTableColumn(reader);
        }
        else if (tagName == "table:table-column-group") {
            readElementTableTableColumnGroup(reader);
        }
        else if (tagName == "table:table-columns") {
            readElementTableTableColumns(reader);
        }
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementTableTableColumnGroup(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementTableTableColumns(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTableTableColumns(reader, m_context);

    // <table:table-columns> has the following children in ODF 1.2:
    //   [done] <table:table-column> 9.1.6
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
        if (tagName == "table:table-column") {
            readElementTableTableColumn(reader);
        }
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementTableTableColumns(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementTableTableHeaderColumns(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTableTableHeaderColumns(reader, m_context);

    // <table:table-header-columns> has the following children in ODF 1.2:
    //   [done] <table:table-column> 9.1.6
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
        if (tagName == "table:table-column") {
            readElementTableTableColumn(reader);
        }
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementTableTableHeaderColumns(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementTableTableHeaderRows(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTableTableHeaderRows(reader, m_context);

    // <table:table-header-rows> has the following children in ODF 1.2:
    //   [done] <table:table-row> 9.1.3
    //   [done] <text:soft-page-break> 5.6.
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
        if (tagName == "table:table-row") {
            readElementTableTableRow(reader);
        }
        else if (tagName == "text:soft-page-break") {
            readElementTextSoftPageBreak(reader);
        }
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementTableTableHeaderRows(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementTableTableColumn(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTableTableColumn(reader, m_context);

    // <table:table-column> has no children in ODF 1.2
    reader.skipCurrentElement();

    m_backend->elementTableTableColumn(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementTableTableRowGroup(KoXmlStreamReader &reader)
{
   DEBUGSTART();
    m_backend->elementTableTableRowGroup(reader, m_context);

    // <table:table-row-group> has the following children in ODF 1.2:
    //          <table:table-header-rows> 9.1.7
    //          <table:table-row> 9.1.3
    //          <table:table-row-group> 9.1.9
    //          <table:table-rows> 9.1.8
    //          <text:soft-page-break> 5.6
   //
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
	if (tagName == "table:table-header-rows") {
            readElementTableTableHeaderRows(reader);
        }
        else if (tagName == "table:table-row") {
            readElementTableTableRow(reader);
        }
        else if (tagName == "table:table-row-group") {
            readElementTableTableRowGroup(reader);
        }
        else if (tagName == "table:table-rows") {
            readElementTableTableRows(reader);
        }
        else if (tagName == "text:soft-page-break") {
            readElementTextSoftPageBreak(reader);
        }
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementTableTableRowGroup(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementTableTableRow(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTableTableRow(reader, m_context);

    // <table:table-row> has the following children in ODF 1.2:
    //   [done] <table:covered-table-cell> 9.1.5
    //   [done] <table:table-cell> 9.1.4.
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
        if (tagName == "table:covered-table-cell") {
            readElementTableCoveredTableCell(reader);
        }
        else if (tagName == "table:table-cell") {
            readElementTableTableCell(reader);
        }
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementTableTableRow(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementTableTableRows(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTableTableRows(reader, m_context);

    // <table:table-header-rows> has the following children in ODF 1.2:
    //   [done] <table:table-row> 9.1.3
    //   [done] <text:soft-page-break> 5.6.
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
        if (tagName == "table:table-row") {
            readElementTableTableRow(reader);
        }
        else if (tagName == "text:soft-page-break") {
            readElementTextSoftPageBreak(reader);
        }
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementTableTableRows(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementTableTableCell(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTableTableCell(reader, m_context);

    // <table:table-cell> has the following children in ODF 1.2:
    //
    // In addition to the text level tags like <text:p> etc that can
    // be found in any textbox, table cell or similar, it has the
    // following text document children:
    //
    //   [done] <office:annotation> 14.1
    //          <table:cell-range-source> 9.3.1
    //          <table:detective> 9.3.2

    while (reader.readNextStartElement()) {
        DEBUG_READING("loop-start");
        
        QString tagName = reader.qualifiedName().toString();
        if (tagName == "office:annotation") {
	    readElementOfficeAnnotation(reader);
        }
        else if (tagName == "table:cell-range-source") {
            // FIXME: NYI
            reader.skipCurrentElement();
        }
        else if (tagName == "table:detective") {
            // FIXME: NYI
            reader.skipCurrentElement();
        }
        else {
            readTextLevelElement(reader);
        }
        DEBUG_READING("loop-end");
    }

    m_backend->elementTableTableCell(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementTableCoveredTableCell(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTableCoveredTableCell(reader, m_context);

    // <table:covered-table-cell> has the following children in ODF 1.2:
    //
    // In addition to the text level tags like <text:p> etc that can
    // be found in any textbox, table cell or similar, it has the
    // following text document children:
    //
    //   [done] <office:annotation> 14.1
    //          <table:cell-range-source> 9.3.1
    //          <table:detective> 9.3.2

    while (reader.readNextStartElement()) {
        DEBUG_READING("loop-start");
        
        QString tagName = reader.qualifiedName().toString();
        if (tagName == "office:annotation") {
	    readElementOfficeAnnotation(reader);
	}
        else if (tagName == "table:cell-range-source") {
            // FIXME: NYI
            reader.skipCurrentElement();
        }
        else if (tagName == "table:detective") {
            // FIXME: NYI
            reader.skipCurrentElement();
        }
        else {
            readTextLevelElement(reader);
        }
        DEBUG_READING("loop-end");
    }

    m_backend->elementTableCoveredTableCell(reader, m_context);
    DEBUGEND();
}


// ----------------------------------------------------------------
//                     Paragraph level functions


// This function is a bit special since it doesn't handle a specific
// element.  Instead it handles the common child elements between a
// number of paragraph-level elements.
//
void OdfTextReader::readParagraphContents(KoXmlStreamReader &reader)
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
            //debugOdfReader << "Found character data";
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
        //          <dr3d:scene> 10.5.2
        //          <draw:a> 10.4.12
        //          <draw:caption> 10.3.11
        //          <draw:circle> 10.3.8
        //          <draw:connector> 10.3.10
        //          <draw:control> 10.3.13
        //          <draw:custom-shape> 10.6.1
        //          <draw:ellipse> 10.3.9
        //   [done] <draw:frame> 10.4.2
        //          <draw:g> 10.3.15
        //          <draw:line> 10.3.3
        //          <draw:measure> 10.3.12
        //          <draw:page-thumbnail> 10.3.14
        //          <draw:path> 10.3.7
        //          <draw:polygon> 10.3.5
        //          <draw:polyline> 10.3.4
        //          <draw:rect> 10.3.2
        //          <draw:regular-polygon> 10.3.6
	// All of the above are sent to the draw reader.
	//
        //   [done] <office:annotation> 14.1
        //   [done] <office:annotation-end> 14.2
        //          <presentation:date-time> 10.9.3.5
        //          <presentation:footer> 10.9.3.3
        //          <presentation:header> 10.9.3.1
        //   [done] <text:a> 6.1.8
        //          <text:alphabetical-index-mark> 8.1.10
        //          <text:alphabetical-index-mark-end> 8.1.9
        //          <text:alphabetical-index-mark-start> 8.1.8
        //          <text:author-initials> 7.3.7.2
        //          <text:author-name> 7.3.7.1
        //          <text:bibliography-mark> 8.1.11
        //          <text:bookmark> 6.2.1.2
        //          <text:bookmark-end> 6.2.1.4
        //          <text:bookmark-ref> 7.7.6
        //          <text:bookmark-start> 6.2.1.3
        //          <text:change> 5.5.7.4
        //          <text:change-end> 5.5.7.3
        //          <text:change-start> 5.5.7.2
        //          <text:chapter> 7.3.8
        //          <text:character-count> 7.5.18.5
        //          <text:conditional-text> 7.7.3
        //          <text:creation-date> 7.5.3
        //          <text:creation-time> 7.5.4
        //          <text:creator> 7.5.17
        //          <text:database-display> 7.6.3
        //          <text:database-name> 7.6.7
        //          <text:database-next> 7.6.4
        //          <text:database-row-number> 7.6.6
        //          <text:database-row-select> 7.6.5
        //          <text:date> 7.3.2
        //          <text:dde-connection> 7.7.12
        //          <text:description> 7.5.5
        //          <text:editing-cycles> 7.5.13
        //          <text:editing-duration> 7.5.14
        //          <text:execute-macro> 7.7.10
        //          <text:expression> 7.4.14
        //          <text:file-name> 7.3.9
        //          <text:hidden-paragraph> 7.7.11
        //          <text:hidden-text> 7.7.4
        //          <text:image-count> 7.5.18.7
        //          <text:initial-creator> 7.5.2
        //          <text:keywords> 7.5.12
        //   [done] <text:line-break> 6.1.5
        //          <text:measure> 7.7.13
        //          <text:meta> 6.1.9
        //          <text:meta-field> 7.5.19
        //          <text:modification-date> 7.5.16
        //          <text:modification-time> 7.5.15
        //          <text:note> 6.3.2
        //          <text:note-ref> 7.7.7
        //          <text:object-count> 7.5.18.8
        //          <text:page-continuation> 7.3.5
        //          <text:page-count> 7.5.18.2
        //          <text:page-number> 7.3.4
        //          <text:page-variable-get> 7.7.1.3
        //          <text:page-variable-set> 7.7.1.2
        //          <text:paragraph-count> 7.5.18.3
        //          <text:placeholder> 7.7.2
        //          <text:print-date> 7.5.8
        //          <text:printed-by> 7.5.9
        //          <text:print-time> 7.5.7
        //          <text:reference-mark> 6.2.2.2
        //          <text:reference-mark-end> 6.2.2.4
        //          <text:reference-mark-start> 6.2.2.3
        //          <text:reference-ref> 7.7.5
        //          <text:ruby> 6.4
        //   [done] <text:s> 6.1.3
        //          <text:script> 7.7.9
        //          <text:sender-city> 7.3.6.13
        //          <text:sender-company> 7.3.6.10
        //          <text:sender-country> 7.3.6.15
        //          <text:sender-email> 7.3.6.7
        //          <text:sender-fax> 7.3.6.9
        //          <text:sender-firstname> 7.3.6.2
        //          <text:sender-initials> 7.3.6.4
        //          <text:sender-lastname> 7.3.6.3
        //          <text:sender-phone-private> 7.3.6.8
        //          <text:sender-phone-work> 7.3.6.11
        //          <text:sender-position> 7.3.6.6
        //          <text:sender-postal-code> 7.3.6.14
        //          <text:sender-state-or-province> 7.3.6.16
        //          <text:sender-street> 7.3.6.12
        //          <text:sender-title> 7.3.6.5
        //          <text:sequence> 7.4.13
        //          <text:sequence-ref> 7.7.8
        //          <text:sheet-name> 7.3.11
        //   [done] <text:soft-page-break> 5.6
        //   [done] <text:span> 6.1.7
        //          <text:subject> 7.5.11
        //          <text:tab> 6.1.4
        //          <text:table-count> 7.5.18.6
        //          <text:table-formula> 7.7.14
        //          <text:template-name> 7.3.10
        //          <text:text-input> 7.4.15
        //          <text:time> 7.3.3
        //          <text:title> 7.5.10
        //          <text:toc-mark> 8.1.4
        //          <text:toc-mark-end> 8.1.3
        //          <text:toc-mark-start> 8.1.2
        //          <text:user-defined> 7.5.6
        //          <text:user-field-get> 7.4.9
        //          <text:user-field-input> 7.4.10
        //          <text:user-index-mark> 8.1.7
        //          <text:user-index-mark-end> 8.1.6
        //          <text:user-index-mark-start> 8.1.5
        //          <text:variable-get> 7.4.5
        //          <text:variable-input> 7.4.6
        //          <text:variable-set> 7.4.4
        //          <text:word-count> 7.5.18.4.
        //        
        // FIXME: Only very few tags are handled right now.

        QString tagName = reader.qualifiedName().toString();

	if (reader.prefix() == "draw" || reader.prefix() == "dr3d") {
	    OdfDrawReader *drawReader = m_parent->drawReader();
	    if (drawReader) {
		drawReader->readCommonGraphicsElements(reader);
	    }
	    else {
		reader.skipCurrentElement();
	    }
        } // draw | dr3d namespace
        else if (reader.prefix() == "office") {
            if (tagName == "office:annotation") {
                readElementOfficeAnnotation(reader);
            }
            else if (tagName == "office:annotation-end") {
                readElementOfficeAnnotationEnd(reader);
            }
            else {
                // Unknown office: element
                readUnknownElement(reader);
            }
        } // office namespace
        else if (reader.prefix() == "text") {

            if (tagName == "text:a") {
                readElementTextA(reader);
            }
            else if (tagName == "text:line-break") {
                readElementTextLineBreak(reader);
            }
            else if (tagName == "text:span") {
                readElementTextSpan(reader);
            }
            else if (tagName == "text:s") {
                readElementTextS(reader);
            }
            else if (tagName == "text:soft-page-break") {
                readElementTextSoftPageBreak(reader);
            }
            else {
                // Unknown text: element
                readUnknownElement(reader);
            }
        } // text namespace
        else {
            // Unknown namespace
            readUnknownElement(reader);
        }

        // Read past the end tag of the just parsed element.
        reader.readNext();
        DEBUG_READING("loop-end");
    }

    DEBUGEND();
}

void OdfTextReader::readElementOfficeAnnotation(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementOfficeAnnotation(reader, m_context);

    // <office:annotation> has the following children in ODF 1.2:
    //   [done] <dc:creator> 4.3.2.7
    //   [done] <dc:date> 4.3.2.10
    //   [done] <meta:date-string> 14.3
    //   [done] <text:list> 5.3.1
    //   [done] <text:p> 5.1.3
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();

        if (tagName == "dc:creator") {
            readElementDcCreator(reader);
        }
        else if (tagName == "dc:date") {
            readElementDcDate(reader);
        }
        else if (tagName == "meta:date-string") {
	    QString value;
	    readCharacterData(reader, value);
	    m_backend->textVariable(tagName, value);
        }
        else if (tagName == "text:list") {
            readElementTextList(reader);
        }
        else if (tagName == "text:p") {
            readElementTextP(reader);
        }
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementOfficeAnnotation(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementOfficeAnnotationEnd(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementOfficeAnnotationEnd(reader, m_context);

    // <office:annotation-end> has no children in ODF 1.2:
    // FIXME: Skip current element or call parseUnknownElement?
    reader.skipCurrentElement();

    m_backend->elementOfficeAnnotationEnd(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementDcCreator(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementDcCreator(reader, m_context);

    reader.readNext();
    readParagraphContents(reader);

    m_backend->elementDcCreator(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementDcDate(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementDcDate(reader, m_context);

    reader.readNext();
    readParagraphContents(reader);

    m_backend->elementDcDate(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementTextA(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTextA(reader, m_context);

    // readParagraphContents expect to have the reader point to the
    // contents of the paragraph so we have to read past the text:a
    // start tag here.
    reader.readNext();
    readParagraphContents(reader);

    m_backend->elementTextA(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementTextLineBreak(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTextLineBreak(reader, m_context);

    // This element has no child elements in ODF 1.2.
    reader.skipCurrentElement();

    m_backend->elementTextLineBreak(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementTextS(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTextS(reader, m_context);

    // This element has no child elements in ODF 1.2.
    reader.skipCurrentElement();

    m_backend->elementTextS(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementTextSpan(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTextSpan(reader, m_context);

    reader.readNext();
    readParagraphContents(reader);

    m_backend->elementTextSpan(reader, m_context);
    DEBUGEND();
}


// ----------------------------------------------------------------
//                        List level functions


void OdfTextReader::readElementTextListHeader(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTextListHeader(reader, m_context);

    // <text:list-header> has the following children in ODF 1.2:
    //   [done] <text:h> 5.1.2
    //   [done] <text:p> 5.1.3
    //   [done] <text:list> 5.3.1
    //   [done] <text:soft-page-break> 5.6
    //          <text:number> 6.1.10
    while(reader.readNextStartElement()) {
	DEBUG_READING("loop-start");

	QString tagName = reader.qualifiedName().toString();
	if (tagName == "text:h") {
	    readElementTextH(reader);
	}
	else if (tagName == "text:p") {
	    readElementTextP(reader);
	}
	else if (tagName == "text:list") {
	    readElementTextList(reader);
	}
        else if (tagName == "text:soft-page-break") {
	    readElementTextSoftPageBreak(reader);
        }
        else if (tagName == "text:number") {
            // FIXME: NYI
            reader.skipCurrentElement();
        }
	else {
	    readUnknownElement(reader);
	}

	DEBUG_READING("loop-end");
    }

    m_backend->elementTextListHeader(reader, m_context);
    DEBUGEND();
}

void OdfTextReader::readElementTextListItem(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTextListItem(reader, m_context);

    // <text:list-item> has the following children in ODF 1.2:
    //   [done] <text:h> 5.1.2
    //   [done] <text:p> 5.1.3
    //   [done] <text:list> 5.3.1
    //   [done] <text:soft-page-break> 5.6
    //          <text:number> 6.1.10
    while(reader.readNextStartElement()) {
        DEBUG_READING("loop-start");

        QString tagName = reader.qualifiedName().toString();
        debugOdfReader <<tagName;
	if (tagName == "text:h") {
	    readElementTextH(reader);
	}
	else if (tagName == "text:p") {
	    readElementTextP(reader);
	}
        else if (tagName == "text:list") {
            readElementTextList(reader);
        }
        else if (tagName == "text:soft-page-break") {
	    readElementTextSoftPageBreak(reader);
        }
        else if (tagName == "text:number") {
            //FIXME
            reader.skipCurrentElement();
        }
        else {
            readUnknownElement(reader);
        }

	DEBUG_READING("loop-end");
    }

    m_backend->elementTextListItem(reader, m_context);
    DEBUGEND();
}

// ----------------------------------------------------------------
//                             Other functions


void OdfTextReader::readElementTextSoftPageBreak(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementTextSoftPageBreak(reader, m_context);

    // <text:soft-page-break> has no children in ODF 1.2
    reader.skipCurrentElement();

    m_backend->elementTextSoftPageBreak(reader, m_context);
    DEBUGEND();
}


void OdfTextReader::readUnknownElement(KoXmlStreamReader &reader)
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

