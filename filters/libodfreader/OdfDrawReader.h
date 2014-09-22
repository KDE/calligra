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

#ifndef ODFDRAWREADER_H
#define ODFDRAWREADER_H

// Qt
#include <QHash>
#include <QString>

// Calligra
#include <KoXmlStreamReader.h>

// this library
#include "koodfreader_export.h"


class QSizeF;

class KoXmlWriter;
class KoStore;

class OdfReader;
class OdfDrawReaderBackend;
class OdfReaderContext;


/** @brief Read the XML tree of the content of an ODT file.
 *
 * The OdfDrawReader is used to traverse (read) the draw contents of
 * an ODF file using an XML stream reader.  For every XML element that
 * the reading process comes across it will call a specific function
 * in a backend class: @see OdfDrawReaderBackend.  The OdfDrawReader
 * is used as a common way to reat draw content and is called from all
 * readers for different ODF formats.  @see OdtReader, @see OdsReader,
 * @see OdpReader.
 */
class KOODFREADER_EXPORT OdfDrawReader
{
 public:
    OdfDrawReader();
    ~OdfDrawReader();

    void setParent(OdfReader *parent);
    void setBackend(OdfDrawReaderBackend *backend);
    void setContext(OdfReaderContext *context);

    // ----------------------------------------------------------------
    // Dr3d elements
    void readElementDr3dScene(KoXmlStreamReader &reader);

    // ----------------------------------------------------------------
    // Draw elements

    // Read all common draw level elements like draw:p, draw:h, draw:frame, etc.
    // This is the main entry point for draw reading.
    void readDrawElement(KoXmlStreamReader &reader);


 protected:
    // All readElement*() are named after the full qualifiedName of
    // the element in ODF that they handle.

    // ----------------------------------------------------------------
    // Draw functions: circle, rectangle, etc

#if 0
    void readElementOfficeAnnotation(KoXmlStreamReader &reader);
    void readElementOfficeAnnotationEnd(KoXmlStreamReader &reader);

    void readElementDcCreator(KoXmlStreamReader &reader);
    void readElementDcDate(KoXmlStreamReader &reader);

    void readElementDrawH(KoXmlStreamReader &reader);
    void readElementDrawP(KoXmlStreamReader &reader);
    void readElementDrawList(KoXmlStreamReader &reader);
    void readElementDrawA(KoXmlStreamReader &reader);

    void readElementTableTableColumn(KoXmlStreamReader &reader);
    void readElementTableTableHeaderRows(KoXmlStreamReader &reader);
    void readElementTableTableRow(KoXmlStreamReader &reader);
    void readElementTableTableCell(KoXmlStreamReader &reader);
    void readElementTableCoveredTableCell(KoXmlStreamReader &reader);
#endif

    // ----------------------------------------------------------------
    // Other functions

    // FIXME: Move this to a common file (OdfReaderUtils?)
    void readUnknownElement(KoXmlStreamReader &reader);


 private:
    OdfReader             *m_parent;  // The OdfReader controlling this one.

    OdfDrawReaderBackend  *m_backend;
    OdfReaderContext      *m_context;
};

#endif // ODFDRAWREADER_H
