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
#include "OdfReaderInternals.h"


class QSizeF;

class OdfReader;
class OdfDrawReaderBackend;
class OdfReaderContext;


/** @brief Read the XML tree of the content of an ODT file.
 *
 * The OdfDrawReader is used to traverse (read) the draw contents of
 * an ODF file using an XML stream reader.  For every XML element that
 * the reading process comes across it will call a specific function
 * in a backend class: @see OdfDrawReaderBackend.  The OdfDrawReader
 * is used as a common way to read draw content and is called from all
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

    DECLARE_READER_FUNCTION(Dr3dScene);   // ODF 1.2  10.5.2
    DECLARE_READER_FUNCTION(Dr3dLight);   // ODF 1.2  10.5.3
    DECLARE_READER_FUNCTION(Dr3dCube);    // ODF 1.2  10.5.4
    DECLARE_READER_FUNCTION(Dr3dSphere);  // ODF 1.2  10.5.5
    DECLARE_READER_FUNCTION(Dr3dExtrude); // ODF 1.2  10.5.6
    DECLARE_READER_FUNCTION(Dr3dRotate);  // ODF 1.2  10.5.7

    // ----------------------------------------------------------------
    // Draw elements

    // Read all common draw level elements like draw:p, draw:h, draw:frame, etc.
    // This is the main entry point for draw reading.
    void readCommonGraphicsElements(KoXmlStreamReader &reader);

 protected:
    // ----------------------------------------------------------------
    // Dr3d elements


    // ----------------------------------------------------------------
    // Draw functions: circle, rectangle, etc

    DECLARE_READER_FUNCTION(DrawA);

    DECLARE_READER_FUNCTION(DrawRect);            // ODF 1.2  10.3.2
    DECLARE_READER_FUNCTION(DrawLine);            // ODF 1.2  10.3.3
    DECLARE_READER_FUNCTION(DrawPolyline);        // ODF 1.2  10.3.4
    DECLARE_READER_FUNCTION(DrawPolygon);         // ODF 1.2  10.3.5
    DECLARE_READER_FUNCTION(DrawRegularPolygon);  // ODF 1.2  10.3.6
    DECLARE_READER_FUNCTION(DrawPath);            // ODF 1.2  10.3.7
    DECLARE_READER_FUNCTION(DrawCircle);          // ODF 1.2  10.3.8
    DECLARE_READER_FUNCTION(DrawEllipse);         // ODF 1.2  10.3.9
    DECLARE_READER_FUNCTION(DrawConnector);       // ODF 1.2  10.3.10
    DECLARE_READER_FUNCTION(DrawCaption);         // ODF 1.2  10.3.11
    DECLARE_READER_FUNCTION(DrawMeasure);         // ODF 1.2  10.3.12

    void readGraphicsObjectChildren(KoXmlStreamReader &reader);

    // ----------------------------------------------------------------
    // Frames

    DECLARE_READER_FUNCTION(DrawFrame);
    DECLARE_READER_FUNCTION(DrawObject);
    DECLARE_READER_FUNCTION(DrawObjectOle);

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
