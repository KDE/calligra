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


// Own
#include "OdtReaderAsciiBackend.h"

// KDE
#include "kdebug.h"

// Calligra
#include <KoXmlReader.h>

// This filter
#include "OdfReaderAsciiContext.h"


#define DEBUG_BACKEND() \
    kDebug(30503) << (reader.isStartElement() ? "start": (reader.isEndElement() ? "end" : "other")) \
    << reader.qualifiedName().toString()

// ================================================================
//                 class OdtReaderAsciiBackend


OdtReaderAsciiBackend::OdtReaderAsciiBackend(OdfReaderContext *context)
    : OdtReaderBackend(context)
{
}

OdtReaderAsciiBackend::~OdtReaderAsciiBackend()
{
}


// ----------------------------------------------------------------
// ODT document level functions



void OdtReaderAsciiBackend::elementOfficeText(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    Q_UNUSED(reader);
    Q_UNUSED(context);
}


// ----------------------------------------------------------------
// Text level functions: paragraphs, headings, sections, frames, objects, etc

void OdtReaderAsciiBackend::elementTextH(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdtReaderAsciiBackend::elementTextP(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    if (!reader.isEndElement())
        return;

    OdfReaderAsciiContext *asciiContext = dynamic_cast<OdfReaderAsciiContext*>(context);
    if (!asciiContext) {
        return;
    }

    // At the end of a paragraph, output two newlines.
    asciiContext->outStream << "\n\n";
}


// ----------------------------------------------------------------
// Paragraph level functions: spans, annotations, notes, text content itself, etc.

void OdtReaderAsciiBackend::elementTextSpan(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdtReaderAsciiBackend::elementTextS(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    if (!reader.isStartElement())
        return;

    OdfReaderAsciiContext *asciiContext = dynamic_cast<OdfReaderAsciiContext*>(context);
    if (!asciiContext) {
        return;
    }

#if 0
    QString dummy = element.attribute("text:c", "1");
    bool ok;
    int  numSpaces = dummy.toUInt(&ok);
    if (!ok) 
        numSpaces = 1;

    // At the end of a paragraph, output two newlines.
    asciiContext->outStream << "\n\n";
#endif
}


void OdtReaderAsciiBackend::characterData(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderAsciiContext *asciiContext = dynamic_cast<OdfReaderAsciiContext*>(context);
    if (!asciiContext) {
        return;
    }
    kDebug(30503) << reader.text().toString();

    asciiContext->outStream << reader.text().toString();
}
