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
#include "OdfTextReaderDocxBackend.h"

// Qt
#include <QtGlobal>

// KDE
#include "kdebug.h"

// Calligra
#include <KoXmlWriter.h>

// This filter
#include "OdfReaderDocxContext.h"


#if 0
#define DEBUG_BACKEND() \
    kDebug(30503) << (reader.isStartElement() ? "start": (reader.isEndElement() ? "end" : "other")) \
    << reader.qualifiedName().toString()
#else
#define DEBUG_BACKEND() \
    //NOTHING
#endif


// ================================================================
//                 class OdfTextReaderDocxBackend


OdfTextReaderDocxBackend::OdfTextReaderDocxBackend()
    : OdfTextReaderBackend()
    , m_insideSpanLevel(0)
{
}

OdfTextReaderDocxBackend::~OdfTextReaderDocxBackend()
{
}


// ----------------------------------------------------------------
// Text level functions: paragraphs, headings, sections, frames, objects, etc


void OdfTextReaderDocxBackend::elementTextH(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdfTextReaderDocxBackend::elementTextP(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderDocxContext *docxContext = dynamic_cast<OdfReaderDocxContext*>(context);
    if (!docxContext) {
        return;
    }

    KoXmlWriter  *writer = docxContext->m_documentWriter;
    if (reader.isStartElement()) {
        writer->startElement("w:p");
        // FIXME: Add paragraph attributes here
        writer->startElement("w:pPr");
        // FIXME: Add paragraph properties (styling) here
        writer->endElement(); // w:pPr
    }
    else {
        writer->endElement(); // w:p
    }
}


// ----------------------------------------------------------------
// Paragraph level functions: spans, annotations, notes, text content itself, etc.


void OdfTextReaderDocxBackend::elementTextSpan(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderDocxContext *docxContext = dynamic_cast<OdfReaderDocxContext*>(context);
    if (!docxContext) {
        return;
    }

    if (reader.isStartElement()) {
        startRun(docxContext);
        ++m_insideSpanLevel;
    }
    else {
        endRun(docxContext);
        --m_insideSpanLevel;
    }
}

void OdfTextReaderDocxBackend::elementTextS(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    if (!reader.isStartElement())
        return;

    OdfReaderDocxContext *docxContext = dynamic_cast<OdfReaderDocxContext*>(context);
    if (!docxContext) {
        return;
    }

#if 0
    QString dummy = element.attribute("text:c", "1");
    bool ok;
    int  numSpaces = dummy.toUInt(&ok);
    if (!ok) 
        numSpaces = 1;

    // At the end of a paragraph, output two newlines.
    docxContext->outStream << "\n\n";
#endif
}


void OdfTextReaderDocxBackend::characterData(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderDocxContext *docxContext = dynamic_cast<OdfReaderDocxContext*>(context);
    if (!docxContext) {
        return;
    }
    //kDebug(30503) << reader.text().toString();

    // In docx, a text always has to be inside a run (w:r). This is
    // created when a text:span is encountered in odf but text nodes
    // can exist also without a text:span surrounding it.
    KoXmlWriter  *writer = docxContext->m_documentWriter;
    if (m_insideSpanLevel == 0) {
        startRun(docxContext);
    }

    writer->startElement("w:t");
    writer->addTextNode(reader.text().toString());
    writer->endElement(); // w:t

    if (m_insideSpanLevel == 0) {
        endRun(docxContext);
    }
}


// ----------------------------------------------------------------
//                         Private functions


void OdfTextReaderDocxBackend::startRun(OdfReaderDocxContext *docxContext)
{
    KoXmlWriter  *writer = docxContext->m_documentWriter;
    writer->startElement("w:r");
    writer->startElement("w:rPr");
    // FIXME: Add run properties here
    writer->endElement(); // w:rPr
}

void OdfTextReaderDocxBackend::endRun(OdfReaderDocxContext *docxContext)
{
    // FIXME: More here?
    KoXmlWriter  *writer = docxContext->m_documentWriter;
    writer->endElement(); // w:r
}
