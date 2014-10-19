/* This file is part of the KDE project

   Copyright (C) 2013-2014 Inge Wallin       <inge@lysator.liu.se>

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

#ifndef ODFDRAWREADERBACKEND_H
#define ODFDRAWREADERBACKEND_H

// Calligra
#include <KoXmlStreamReader.h>
#include <KoFilter.h>

// this library
#include "koodfreader_export.h"
#include "OdfDrawReader.h"


class QByteArray;
class QSizeF;
class QStringList;
class KoStore;
class OdfReaderContext;


/** @brief A default backend for the OdfDrawReader class.
 *
 * This class defines an interface and the default behaviour for the
 * backend to the ODF draw reader (@see OdfDrawReader). When the
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
class KOODFREADER_EXPORT OdfDrawReaderBackend
{
 public:
    explicit OdfDrawReaderBackend();
    virtual ~OdfDrawReaderBackend();

    // ----------------------------------------------------------------
    // Dr3d functions

    DECLARE_BACKEND_FUNCTION(Dr3dScene);   // ODF 1.2  10.5.2
    DECLARE_BACKEND_FUNCTION(Dr3dLight);   // ODF 1.2  10.5.3
    DECLARE_BACKEND_FUNCTION(Dr3dCube);    // ODF 1.2  10.5.4
    DECLARE_BACKEND_FUNCTION(Dr3dSphere);  // ODF 1.2  10.5.5
    DECLARE_BACKEND_FUNCTION(Dr3dExtrude); // ODF 1.2  10.5.6
    DECLARE_BACKEND_FUNCTION(Dr3dRotate);  // ODF 1.2  10.5.7

    // ----------------------------------------------------------------
    // Draw functions

    DECLARE_BACKEND_FUNCTION(DrawA);

    DECLARE_BACKEND_FUNCTION(DrawRect);            // ODF 1.2  10.3.2
    DECLARE_BACKEND_FUNCTION(DrawLine);            // ODF 1.2  10.3.3
    DECLARE_BACKEND_FUNCTION(DrawPolyline);        // ODF 1.2  10.3.4
    DECLARE_BACKEND_FUNCTION(DrawPolygon);         // ODF 1.2  10.3.5
    DECLARE_BACKEND_FUNCTION(DrawRegularPolygon);  // ODF 1.2  10.3.6
    DECLARE_BACKEND_FUNCTION(DrawPath);            // ODF 1.2  10.3.7
    DECLARE_BACKEND_FUNCTION(DrawCircle);          // ODF 1.2  10.3.8
    DECLARE_BACKEND_FUNCTION(DrawEllipse);         // ODF 1.2  10.3.9
    DECLARE_BACKEND_FUNCTION(DrawConnector);       // ODF 1.2  10.3.10
    DECLARE_BACKEND_FUNCTION(DrawCaption);         // ODF 1.2  10.3.11
    DECLARE_BACKEND_FUNCTION(DrawMeasure);         // ODF 1.2  10.3.12

    // ----------------------------------------------------------------
    // Frames

    DECLARE_BACKEND_FUNCTION(DrawFrame);
    DECLARE_BACKEND_FUNCTION(DrawObject);
    DECLARE_BACKEND_FUNCTION(DrawObjectOle);

 private:
    class Private;
    Private * const d;
};


#endif // ODFDRAWREADERBACKEND_H
