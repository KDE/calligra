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

#ifndef ODFTEXTREADERBACKEND_H
#define ODFTEXTREADERBACKEND_H

// Calligra
#include <KoXmlStreamReader.h>
#include <KoFilter.h>

// this library
#include "koodfreader_export.h"
#include "OdfTextReader.h"


class OdfReaderContext;


/** @brief A default backend for the OdfTextReader class.
 *
 * This class defines an interface and the default behaviour for the
 * backend to the ODF text reader (@see OdfTextReader). When the
 * reader is called upon to traverse a certain XML tree, there will
 * be two parameters to the root traverse function: a pointer to a
 * backend object and a pointer to a context object.
 *
 * The reader will traverse (read) the XML tree and for every element
 * it comes across it will call a specific function in the backend and
 * every call will pass the pointer to the context object.
 *
 * Each supported XML tag has a corresponding callback function. This
 * callback function will be called twice: once when the tag is first
 * encountered anc once when the tag is closed.  This means that an
 * element with no child elements will be called twice in succession.
 */
class KOODFREADER_EXPORT OdfTextReaderBackend
{
 public:
    explicit OdfTextReaderBackend();
    virtual ~OdfTextReaderBackend();

    // ----------------------------------------------------------------
    // Text level functions: paragraphs, headings, sections, frames, objects, etc

    DECLARE_BACKEND_FUNCTION(OfficeAnnotation);
    DECLARE_BACKEND_FUNCTION(OfficeAnnotationEnd);
    DECLARE_BACKEND_FUNCTION(DcCreator);
    DECLARE_BACKEND_FUNCTION(DcDate);

    DECLARE_BACKEND_FUNCTION(TextH);
    DECLARE_BACKEND_FUNCTION(TextP);
    DECLARE_BACKEND_FUNCTION(TextList);

    DECLARE_BACKEND_FUNCTION(TableTable);
    DECLARE_BACKEND_FUNCTION(TableTableColumnGroup);
    DECLARE_BACKEND_FUNCTION(TableTableColumn);
    DECLARE_BACKEND_FUNCTION(TableTableColumns);
    DECLARE_BACKEND_FUNCTION(TableTableHeaderColumns);
    DECLARE_BACKEND_FUNCTION(TableTableHeaderRows);
    DECLARE_BACKEND_FUNCTION(TableTableRowGroup);
    DECLARE_BACKEND_FUNCTION(TableTableRow);
    DECLARE_BACKEND_FUNCTION(TableTableRows);
    DECLARE_BACKEND_FUNCTION(TableTableCell);
    DECLARE_BACKEND_FUNCTION(TableCoveredTableCell);

    // ----------------------------------------------------------------
    // Paragraph level functions: spans, annotations, notes, etc.
    // This includes text content itself.

    DECLARE_BACKEND_FUNCTION(TextA);
    DECLARE_BACKEND_FUNCTION(TextLineBreak);
    DECLARE_BACKEND_FUNCTION(TextSpan);
    DECLARE_BACKEND_FUNCTION(TextS);

    // ----------------------------------------------------------------
    // List level functions: list-header and list-item.

    DECLARE_BACKEND_FUNCTION(TextListHeader);
    DECLARE_BACKEND_FUNCTION(TextListItem);
    DECLARE_BACKEND_FUNCTION(TextSoftPageBreak);

    // ----------------------------------------------------------------
    // Some special functions
    virtual void characterData(KoXmlStreamReader &reader, OdfReaderContext *context);
    virtual void textVariable(const QString &name, const QString &value);

 private:
    class Private;
    Private * const d;
};


#endif // ODFTEXTREADERBACKEND_H
