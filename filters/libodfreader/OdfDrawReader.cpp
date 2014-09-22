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
#include "OdfReader.h"
#include "OdfDrawReaderBackend.h"
#include "OdfReaderContext.h"


#if 1
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




// ----------------------------------------------------------------
//                         namespace dr3d


void OdfDrawReader::readElementDr3dScene(KoXmlStreamReader &reader)
{
   DEBUGSTART();
    m_backend->elementDr3dScene(reader, m_context);

    // <dr3d:scene> has the following children in ODF 1.2:
    //          <dr3d:cube> 10.5.4
    //          <dr3d:extrude> 10.5.6
    //          <dr3d:light> 10.5.3
    //          <dr3d:rotate> 10.5.7
    //          <dr3d:scene> 10.5.2
    //          <dr3d:sphere> 10.5.5
    //          <draw:glue-point> 10.3.16
    //          <svg:desc> 10.3.18
    //          <svg:title> 10.3.17.
    //
    while (reader.readNextStartElement()) {
        QString tagName = reader.qualifiedName().toString();
        
        if (tagName == "dr3d:cube") {
            // FIXME: NYI
            reader.skipCurrentElement();
        }
        else if (tagName == "dr3d:...") {
            // FIXME: NYI
            reader.skipCurrentElement();
        }
        //...  MORE else if () HERE
        else {
            reader.skipCurrentElement();
        }
    }

    m_backend->elementDr3dScene(reader, m_context);
    DEBUGEND();
}


// ----------------------------------------------------------------
//                         namespace draw


void OdfDrawReader::readDrawElement(KoXmlStreamReader &reader)
{
    DEBUGSTART();

    // This is a function common to all draw elements so no backend function
    // should be called here.

    // The common draw elements are:
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

    QString tagName = reader.qualifiedName().toString();
    //kDebug() << "list child:" << tagName;
    if (tagName == "draw:a") {
	//readElementDrawA(reader);
	reader.skipCurrentElement();

    }
    else if (tagName == "draw:frame") {
	//readElementDrawFrame(reader);
	reader.skipCurrentElement();
    }
    else {
	// FIXME: Should this perhaps be skipCurrentElement()?
	readUnknownElement(reader);
    }

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
