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

#ifndef ODFTEXTREADER_H
#define ODFTEXTREADER_H

// Qt
#include <QHash>
#include <QString>

// Calligra
#include <KoXmlStreamReader.h>

// this library
#include "koodfreader_export.h"
#include "OdfReaderInternals.h"



class OdfReader;
class OdfReaderContext;
class OdfTextReaderBackend;


/** @brief Read the XML tree of the content of an ODT file.
 *
 * The OdfTextReader is used to traverse (read) the text contents of
 * an ODF file using an XML stream reader.  For every XML element that
 * the reading process comes across it will call a specific function
 * in a backend class: @see OdfTextReaderBackend.  The OdfTextReader
 * is used as a common way to reat text content and is called from all
 * readers for different ODF formats.  @see OdtReader, @see OdsReader,
 * @see OdpReader.
 */
class KOODFREADER_EXPORT OdfTextReader
{
 public:
    OdfTextReader();
    ~OdfTextReader();

    // Read all common text level elements like text:p, text:h, draw:frame, etc.
    // This is the main entry point for text reading.
    void readTextLevelElement(KoXmlStreamReader &reader);
    void readElementTableTable(KoXmlStreamReader &reader);

    void setParent(OdfReader *parent);
    void setBackend(OdfTextReaderBackend *backend);
    void setContext(OdfReaderContext *context);

    // All readElement*() are named after the full qualifiedName of
    // the element in ODF that they handle.

    // ----------------------------------------------------------------
    // Text level functions: paragraphs, headings, sections, frames, objects, etc

    DECLARE_READER_FUNCTION(OfficeAnnotation);
    DECLARE_READER_FUNCTION(OfficeAnnotationEnd);

    DECLARE_READER_FUNCTION(DcCreator);
    DECLARE_READER_FUNCTION(DcDate);

    DECLARE_READER_FUNCTION(TextH);
    DECLARE_READER_FUNCTION(TextP);
    DECLARE_READER_FUNCTION(TextList);
    DECLARE_READER_FUNCTION(TextA);

    DECLARE_READER_FUNCTION(TableTableColumnGroup);
    DECLARE_READER_FUNCTION(TableTableColumn);
    DECLARE_READER_FUNCTION(TableTableColumns);
    DECLARE_READER_FUNCTION(TableTableHeaderColumns);
    DECLARE_READER_FUNCTION(TableTableHeaderRows);
    DECLARE_READER_FUNCTION(TableTableRowGroup);
    DECLARE_READER_FUNCTION(TableTableRow);
    DECLARE_READER_FUNCTION(TableTableRows);
    DECLARE_READER_FUNCTION(TableTableCell);
    DECLARE_READER_FUNCTION(TableCoveredTableCell);

    // ----------------------------------------------------------------
    // Paragraph level functions: spans, annotations, notes, text content itself, etc.

    void readParagraphContents(KoXmlStreamReader &reader);

    DECLARE_READER_FUNCTION(TextLineBreak);
    DECLARE_READER_FUNCTION(TextS);
    DECLARE_READER_FUNCTION(TextSpan);

    // ----------------------------------------------------------------
    // List level functions: list-item and list header.

    DECLARE_READER_FUNCTION(TextListItem);
    DECLARE_READER_FUNCTION(TextListHeader);

    // ----------------------------------------------------------------
    // Other functions

    DECLARE_READER_FUNCTION(TextSoftPageBreak);

    void readUnknownElement(KoXmlStreamReader &reader);


 private:
    OdfReader             *m_parent;  // The OdfReader controlling this one.

    OdfTextReaderBackend  *m_backend;
    OdfReaderContext      *m_context;
};

#endif // ODFTEXTREADER_H
