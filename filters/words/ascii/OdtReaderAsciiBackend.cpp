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

// Calligra
#include <KoXmlReader.h>

// This filter
#include "OdfReaderAsciiContext.h"
#include "AsciiExportDebug.h"


#if 0
#define DEBUG_BACKEND() \
    debugAsciiExport << (reader.isStartElement() ? "start": (reader.isEndElement() ? "end" : "other")) \
    << reader.qualifiedName().toString()
#else
#define DEBUG_BACKEND() \
    //NOTHING
#endif


// ================================================================
//                 class OdtReaderAsciiBackend


OdtReaderAsciiBackend::OdtReaderAsciiBackend()
    : OdfTextReaderBackend()
{
}

OdtReaderAsciiBackend::~OdtReaderAsciiBackend()
{
}


// ----------------------------------------------------------------
// Text level functions: paragraphs, headings, sections, frames, objects, etc

void OdtReaderAsciiBackend::elementTextH(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    if (!reader.isEndElement())
        return;

    OdfReaderAsciiContext *asciiContext = dynamic_cast<OdfReaderAsciiContext*>(context);
    Q_ASSERT(asciiContext);

    // At the end of a paragraph (headers also), output two newlines.
    asciiContext->outStream << "\n\n";
}

void OdtReaderAsciiBackend::elementTextP(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    if (!reader.isEndElement())
        return;

    OdfReaderAsciiContext *asciiContext = dynamic_cast<OdfReaderAsciiContext*>(context);
    Q_ASSERT(asciiContext);

    // At the end of a paragraph, output two newlines.
    asciiContext->outStream << "\n\n";
}


// ----------------------------------------------------------------
// Paragraph level functions: spans, annotations, notes, text content itself, etc.


void OdtReaderAsciiBackend::elementTextLineBreak(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    if (!reader.isStartElement())
        return;

    OdfReaderAsciiContext *asciiContext = dynamic_cast<OdfReaderAsciiContext*>(context);
    Q_ASSERT(asciiContext);

    asciiContext->outStream << '\n';
}

void OdtReaderAsciiBackend::elementTextS(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    if (!reader.isStartElement())
        return;

    OdfReaderAsciiContext *asciiContext = dynamic_cast<OdfReaderAsciiContext*>(context);
    Q_ASSERT(asciiContext);

    QString dummy = reader.attributes().value("text:c").toString();
    bool ok;
    quint32  numSpaces = dummy.toUInt(&ok);
    if (!ok) 
        numSpaces = 1;

    // Output the required number of spaces.
    for (quint32 i = 0; i < numSpaces; ++i) {
        asciiContext->outStream << ' ';
    }
}

void OdtReaderAsciiBackend::elementTextSpan(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    Q_UNUSED(reader);
    Q_UNUSED(context);
}


void OdtReaderAsciiBackend::characterData(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderAsciiContext *asciiContext = dynamic_cast<OdfReaderAsciiContext*>(context);
    Q_ASSERT(asciiContext);

    asciiContext->outStream << reader.text().toString();
}
