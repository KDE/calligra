/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "OdtReaderAsciiBackend.h"

// Calligra
#include <KoXmlReader.h>

// This filter
#include "AsciiExportDebug.h"
#include "OdfReaderAsciiContext.h"

#if 0
#define DEBUG_BACKEND() debugAsciiExport << (reader.isStartElement() ? "start" : (reader.isEndElement() ? "end" : "other")) << reader.qualifiedName().toString()
#else
#define DEBUG_BACKEND() // NOTHING
#endif

// ================================================================
//                 class OdtReaderAsciiBackend

OdtReaderAsciiBackend::OdtReaderAsciiBackend()
    : OdfTextReaderBackend()
{
}

OdtReaderAsciiBackend::~OdtReaderAsciiBackend() = default;

// ----------------------------------------------------------------
// Text level functions: paragraphs, headings, sections, frames, objects, etc

void OdtReaderAsciiBackend::elementTextH(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    if (!reader.isEndElement())
        return;

    OdfReaderAsciiContext *asciiContext = dynamic_cast<OdfReaderAsciiContext *>(context);
    Q_ASSERT(asciiContext);

    // At the end of a paragraph (headers also), output two newlines.
    asciiContext->outStream << "\n\n";
}

void OdtReaderAsciiBackend::elementTextP(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    if (!reader.isEndElement())
        return;

    OdfReaderAsciiContext *asciiContext = dynamic_cast<OdfReaderAsciiContext *>(context);
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

    OdfReaderAsciiContext *asciiContext = dynamic_cast<OdfReaderAsciiContext *>(context);
    Q_ASSERT(asciiContext);

    asciiContext->outStream << '\n';
}

void OdtReaderAsciiBackend::elementTextS(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    if (!reader.isStartElement())
        return;

    OdfReaderAsciiContext *asciiContext = dynamic_cast<OdfReaderAsciiContext *>(context);
    Q_ASSERT(asciiContext);

    QString dummy = reader.attributes().value("text:c").toString();
    bool ok;
    quint32 numSpaces = dummy.toUInt(&ok);
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
    OdfReaderAsciiContext *asciiContext = dynamic_cast<OdfReaderAsciiContext *>(context);
    Q_ASSERT(asciiContext);

    asciiContext->outStream << reader.text().toString();
}
