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

// Calligra
#include <KoStore.h>
#include <KoXmlStreamReader.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>  // For copyXmlElement
#include <KoOdfReadStore.h>

// Reader library
#include "OdfReader.h"
#include "OdfDrawReaderBackend.h"
#include "OdfReaderContext.h"
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


OdfDrawReader::OdfDrawReader()
    : m_parent(0)
    , m_backend(0)
    , m_context(0)
{
}

OdfDrawReader::~OdfDrawReader()
{
}


// ----------------------------------------------------------------
//                             setters


void OdfDrawReader::setParent(OdfReader *parent)
{
    m_parent = parent;
}

void OdfDrawReader::setBackend(OdfDrawReaderBackend *backend)
{
    m_backend = backend;
}

void OdfDrawReader::setContext(OdfReaderContext *context)
{
    m_context = context;
}


// ----------------------------------------------------------------
//                         namespace dr3d


void OdfDrawReader::readElementDr3dScene(KoXmlStreamReader &reader)
{
   DEBUGSTART();
    m_backend->elementDr3dScene(reader, m_context);

    // <dr3d:scene> has the following children in ODF 1.2:
    //   [done] <dr3d:cube> 10.5.4
    //   [done] <dr3d:extrude> 10.5.6
    //   [done] <dr3d:light> 10.5.3
    //   [done] <dr3d:rotate> 10.5.7
    //   [done] <dr3d:scene> 10.5.2
    //   [done] <dr3d:sphere> 10.5.5

    //          <draw:glue-point> 10.3.16
    //          <svg:desc> 10.3.18
    //          <svg:title> 10.3.17.
    //
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
        if (tagName == "dr3d:cube") {
	    readElementDr3dCube(reader);
        }
        else if (tagName == "dr3d:extrude") {
	    readElementDr3dExtrude(reader);
        }
        else if (tagName == "dr3d:light") {
	    readElementDr3dLight(reader);
        }
        else if (tagName == "dr3d:rotate") {
	    readElementDr3dRotate(reader);
        }
        else if (tagName == "dr3d:scene") {
	    readElementDr3dScene(reader);
        }
        else if (tagName == "dr3d:sphere") {
	    readElementDr3dSphere(reader);
        }
        //...  MORE else if () HERE
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementDr3dScene(reader, m_context);
    DEBUGEND();
}

IMPLEMENT_READER_FUNCTION_NO_CHILDREN(OdfDrawReader, Dr3dLight)   // ODF 1.2  10.5.3
IMPLEMENT_READER_FUNCTION_NO_CHILDREN(OdfDrawReader, Dr3dCube)    // ODF 1.2  10.5.4
IMPLEMENT_READER_FUNCTION_NO_CHILDREN(OdfDrawReader, Dr3dSphere)  // ODF 1.2  10.5.5
IMPLEMENT_READER_FUNCTION_NO_CHILDREN(OdfDrawReader, Dr3dExtrude) // ODF 1.2  10.5.6
IMPLEMENT_READER_FUNCTION_NO_CHILDREN(OdfDrawReader, Dr3dRotate)  // ODF 1.2  10.5.7


// ================================================================
//                         namespace draw


// ----------------------------------------------------------------


#if 0
// This is a template function for the reader library.
// Copy this one and change the name and fill in the code.
void OdfDrawReader::readElementNamespaceTagname(KoXmlStreamReader &reader)
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


void OdfDrawReader::readCommonGraphicsElements(KoXmlStreamReader &reader)
{
    DEBUGSTART();

    // This is a function common to all draw elements so no backend function
    // should be called here.

    // The common graphics elements are:
    //   [done] <dr3d:scene> 10.5.2
    //   [done] <draw:a> 10.4.12
    //   [done] <draw:caption> 10.3.11
    //   [done] <draw:circle> 10.3.8
    //   [done] <draw:connector> 10.3.10
    //          <draw:control> 10.3.13
    //          <draw:custom-shape> 10.6.1
    //   [done] <draw:ellipse> 10.3.9
    //   [done] <draw:frame> 10.4.2
    //          <draw:g> 10.3.15
    //   [done] <draw:line> 10.3.3
    //   [done] <draw:measure> 10.3.12
    //          <draw:page-thumbnail> 10.3.14
    //   [path] <draw:path> 10.3.7
    //   [done] <draw:polygon> 10.3.5
    //   [done] <draw:polyline> 10.3.4
    //   [done] <draw:rect> 10.3.2
    //   [done] <draw:regular-polygon> 10.3.6

    QString tagName = reader.qualifiedName().toString();
    //debugOdfReader << "list child:" << tagName;
    if (tagName == "dr3d:scene") {
	readElementDr3dScene(reader);
    }
    else if (tagName == "draw:a") {
	readElementDrawA(reader);
    }
    else if (tagName == "draw:caption") {
	readElementDrawCaption(reader);
    }
    else if (tagName == "draw:circle") {
	readElementDrawCircle(reader);
    }
    else if (tagName == "draw:connector") {
	readElementDrawConnector(reader);
    }
    else if (tagName == "draw:ellipse") {
	readElementDrawEllipse(reader);
    }
    else if (tagName == "draw:frame") {
	readElementDrawFrame(reader);
    }
    else if (tagName == "draw:line") {
	readElementDrawLine(reader);
    }
    else if (tagName == "draw:measure") {
	readElementDrawMeasure(reader);
    }
    else if (tagName == "draw:path") {
	readElementDrawPath(reader);
    }
    else if (tagName == "draw:polygon") {
	readElementDrawPolygon(reader);
    }
    else if (tagName == "draw:polyline") {
	readElementDrawPolyline(reader);
    }
    else if (tagName == "draw:rect") {
	readElementDrawRect(reader);
    }
    else if (tagName == "draw:regular-polygon") {
	readElementDrawRegularPolygon(reader);
    }
    else {
	// FIXME: Should this perhaps be skipCurrentElement()?
	readUnknownElement(reader);
    }

    DEBUGEND();
}

void OdfDrawReader::readElementDrawA(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementDrawA(reader, m_context);

    // <draw:a> has all the normal drawing children.
    readCommonGraphicsElements(reader);

    m_backend->elementDrawA(reader, m_context);
    DEBUGEND();
}


#define IMPLEMENT_GRAPHIC_OBJECT(object)                                \
void OdfDrawReader::readElementDraw##object(KoXmlStreamReader &reader)  \
{                                                                       \
    DEBUGSTART();                                                       \
    m_backend->elementDraw##object(reader, m_context);                  \
                                                                        \
    readGraphicsObjectChildren(reader);                                 \
                                                                        \
    m_backend->elementDraw##object(reader, m_context);                  \
    DEBUGEND();                                                         \
}

IMPLEMENT_GRAPHIC_OBJECT(Rect)            // ODF 1.2  10.3.2
IMPLEMENT_GRAPHIC_OBJECT(Line)		  // ODF 1.2  10.3.3
IMPLEMENT_GRAPHIC_OBJECT(Polyline)	  // ODF 1.2  10.3.4
IMPLEMENT_GRAPHIC_OBJECT(Polygon)	  // ODF 1.2  10.3.5
IMPLEMENT_GRAPHIC_OBJECT(RegularPolygon)  // ODF 1.2  10.3.6
IMPLEMENT_GRAPHIC_OBJECT(Path)		  // ODF 1.2  10.3.7
IMPLEMENT_GRAPHIC_OBJECT(Circle)	  // ODF 1.2  10.3.8
IMPLEMENT_GRAPHIC_OBJECT(Ellipse)	  // ODF 1.2  10.3.9
IMPLEMENT_GRAPHIC_OBJECT(Connector)	  // ODF 1.2  10.3.10
IMPLEMENT_GRAPHIC_OBJECT(Caption)	  // ODF 1.2  10.3.11
IMPLEMENT_GRAPHIC_OBJECT(Measure)	  // ODF 1.2  10.3.12


void OdfDrawReader::readGraphicsObjectChildren(KoXmlStreamReader &reader)
{
    // No backend calls in this function

    // <draw:circle>, <draw:rect>, etc have the following children in ODF 1.2:
    //          <draw:glue-point> 10.3.16
    //          <office:event-listeners> 10.3.19
    //          <svg:desc> 10.3.18
    //          <svg:title> 10.3.17
    //   [done] <text:list> 5.3.1
    //   [done] <text:p> 5.1.3.
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
        if (tagName == "draw:glue-point") {
            // FIXME: NYI
            reader.skipCurrentElement();
            //readElementOfficeDrawGluePoint(reader);
        }
        else if (tagName == "office:event-listeners") {
            // FIXME: NYI
            reader.skipCurrentElement();
            //readElementOfficeEventListeners(reader);
        }
        else if (reader.prefix() == "svg") {
	    if (tagName == "svg:desc") {
		// FIXME: NYI
		reader.skipCurrentElement();
		//readElementSvgDesc(reader);
	    }
	    else if (tagName == "svg:title") {
		// FIXME: NYI
		reader.skipCurrentElement();
		//readElementSvgTitle(reader);
	    }
	    else {
		reader.skipCurrentElement();
	    }
        } // namespace svg
        else if (reader.prefix() == "text") {
	    OdfTextReader *textReader = m_parent->textReader();
	    if (!textReader) {
		reader.skipCurrentElement();
	    }
	    else if (tagName == "text:list") {
		textReader->readElementTextList(reader);
	    }
	    else if (tagName == "text:p") {
		textReader->readElementTextP(reader);
	    }
	    else {
		reader.skipCurrentElement();
	    }
        } // namespace text
        else {
            reader.skipCurrentElement();
        }
    }
}


// ----------------------------------------------------------------
//                                 Frames


void OdfDrawReader::readElementDrawFrame(KoXmlStreamReader &reader)
{
    DEBUGSTART();
    m_backend->elementDrawFrame(reader, m_context);

    // <draw:frame> has the following children in ODF 1.2:
    //          <draw:applet> 10.4.7
    //          <draw:contour-path> 10.4.11.3
    //          <draw:contour-polygon> 10.4.11.2
    //          <draw:floating-frame> 10.4.10
    //          <draw:glue-point> 10.3.16
    //          <draw:image> 10.4.4
    //          <draw:image-map> 10.4.13.2
    //   [done] <draw:object> 10.4.6.2
    //   [done] <draw:object-ole> 10.4.6.3
    //          <draw:plugin> 10.4.8
    //          <draw:text-box> 10.4.3
    //          <office:event-listeners> 10.3.19
    //          <svg:desc> 10.3.18
    //          <svg:title> 10.3.17
    //   [done] <table:table> 9.1.2
    //
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
        if (tagName == "draw:image") {
            // FIXME: NYI
            reader.skipCurrentElement();
        }
        else if (tagName == "draw:object") {
	    readElementDrawObject(reader);
        }
        else if (tagName == "draw:object-ole") {
	    readElementDrawObjectOle(reader);
        }
        //...  MORE else if () HERE
        else if (tagName == "table:table") {
	    OdfTextReader *textReader = m_parent->textReader();
	    if (textReader) {
		textReader->readElementTableTable(reader);
	    }
	    else {
		reader.skipCurrentElement();
	    }
        }
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementDrawFrame(reader, m_context);
    DEBUGEND();
}

void OdfDrawReader::readElementDrawObject(KoXmlStreamReader &reader)
{
   DEBUGSTART();
    m_backend->elementDrawObject(reader, m_context);

    // <draw:object> has the following children in ODF 1.2:
    //          <math:math> 14.5
    //          <office:document> 3.1.2
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
        if (tagName == "math:math") {
            // FIXME: NYI
            reader.skipCurrentElement();
        }
        else if (tagName == "office:document") {
            // FIXME: NYI
            reader.skipCurrentElement();
        }
        else {
	    // Shouldn't happen.
            reader.skipCurrentElement();
        }
    }

    m_backend->elementDrawObject(reader, m_context);
    DEBUGEND();
}

void OdfDrawReader::readElementDrawObjectOle(KoXmlStreamReader &reader)
{
   DEBUGSTART();
    m_backend->elementDrawObjectOle(reader, m_context);

    // <draw:object-ole> has the following children in ODF 1.2:
    //          <office:binary-data> 10.4.5
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
	if (tagName == "office:binary-data") {
            // FIXME: NYI
            reader.skipCurrentElement();
        }
        else {
	    // Shouldn't happen.
            reader.skipCurrentElement();
        }
    }

    m_backend->elementDrawObjectOle(reader, m_context);
    DEBUGEND();
}


// ----------------------------------------------------------------
//                             Other functions


void OdfDrawReader::readUnknownElement(KoXmlStreamReader &reader)
{
    DEBUGSTART();

#if 0  // FIXME: Fix this
    if (m_context->isInsideParagraph()) {
        // readParagraphContents expect to have the reader point to the
        // contents of the paragraph so we have to read past the draw:p
        // start tag here.
        reader.readNext();
        readParagraphContents(reader);
    }
    else {
        while (reader.readNextStartElement()) {
            readTextLevelElement(reader);
        }
    }
#else
    reader.skipCurrentElement();
#endif

    DEBUGEND();
}
