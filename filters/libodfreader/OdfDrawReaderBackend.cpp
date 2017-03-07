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


// Own
#include "OdfDrawReaderBackend.h"

// Calligra
#include <KoXmlReader.h>

// Odftraverse library
#include "OdfParser.h"


// ================================================================
//             class OdfDrawReaderBackend::Private


class OdfDrawReaderBackend::Private
{
 public:
    Private();
    ~Private();

    bool dummy;                 // We don't have any actual content in
                                // this class yet but it's still
                                // needed for forward binary compatibility.
};

OdfDrawReaderBackend::Private::Private()
{
}

OdfDrawReaderBackend::Private::~Private()
{
}


// ================================================================
//                 class OdfDrawReaderBackend


OdfDrawReaderBackend::OdfDrawReaderBackend()
    : d(new OdfDrawReaderBackend::Private)
{
}

OdfDrawReaderBackend::~OdfDrawReaderBackend()
{
    delete d;
}


// ----------------------------------------------------------------
//                         Dr3d functions


IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, Dr3dScene)    // ODF 1.2  10.5.2
IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, Dr3dLight);   // ODF 1.2  10.5.3
IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, Dr3dCube);    // ODF 1.2  10.5.4
IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, Dr3dSphere);  // ODF 1.2  10.5.5
IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, Dr3dExtrude); // ODF 1.2  10.5.6
IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, Dr3dRotate);  // ODF 1.2  10.5.7


// ----------------------------------------------------------------
//                         Draw functions


IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, DrawA)

IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, DrawRect);            // ODF 1.2  10.3.2
IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, DrawLine);            // ODF 1.2  10.3.3
IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, DrawPolyline);        // ODF 1.2  10.3.4
IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, DrawPolygon);         // ODF 1.2  10.3.5
IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, DrawRegularPolygon);  // ODF 1.2  10.3.6
IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, DrawPath);            // ODF 1.2  10.3.7
IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, DrawCircle);          // ODF 1.2  10.3.8
IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, DrawEllipse);         // ODF 1.2  10.3.9
IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, DrawConnector);       // ODF 1.2  10.3.10
IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, DrawCaption);         // ODF 1.2  10.3.11
IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, DrawMeasure);         // ODF 1.2  10.3.12


// ----------------------------------------------------------------
//                             Frames


IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, DrawFrame)
IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, DrawObject)
IMPLEMENT_BACKEND_FUNCTION(OdfDrawReader, DrawObjectOle)
