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
#include "OdtReaderDocxBackend.h"

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
//                 class OdtReaderDocxBackend


OdtReaderDocxBackend::OdtReaderDocxBackend(OdfReaderContext *context)
    : OdtReaderBackend(context)
{
}

OdtReaderDocxBackend::~OdtReaderDocxBackend()
{
}


// ----------------------------------------------------------------
// ODT document level functions



void OdtReaderDocxBackend::elementOfficeText(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderDocxContext *docxContext = dynamic_cast<OdfReaderDocxContext*>(context);
    if (!docxContext) {
        return;
    }

    KoXmlWriter  *writer = docxContext->m_documentWriter;
    if (reader.isStartElement()) {
        writer->startDocument(0);
        
        // Start the document and add all necessary namespaces to it.
        writer->startElement("w:document");
        writer->addAttribute("xmlns:wpc", "http://schemas.microsoft.com/office/word/2010/wordprocessingCanvas");
        writer->addAttribute("xmlns:mc", "http://schemas.openxmlformats.org/markup-compatibility/2006");
        writer->addAttribute("xmlns:o", "urn:schemas-microsoft-com:office:office");
        writer->addAttribute("xmlns:r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships");
        writer->addAttribute("xmlns:m", "http://schemas.openxmlformats.org/officeDocument/2006/math");
        writer->addAttribute("xmlns:v", "urn:schemas-microsoft-com:vml");
        writer->addAttribute("xmlns:wp14", "http://schemas.microsoft.com/office/word/2010/wordprocessingDrawing");
        writer->addAttribute("xmlns:wp", "http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing");
        writer->addAttribute("xmlns:w10", "urn:schemas-microsoft-com:office:word");
        writer->addAttribute("xmlns:w", "http://schemas.openxmlformats.org/wordprocessingml/2006/main");
        writer->addAttribute("xmlns:w14", "http://schemas.microsoft.com/office/word/2010/wordml");
        writer->addAttribute("xmlns:w15", "http://schemas.microsoft.com/office/word/2012/wordml");
        writer->addAttribute("xmlns:wpg", "http://schemas.microsoft.com/office/word/2010/wordprocessingGroup");
        writer->addAttribute("xmlns:wpi", "http://schemas.microsoft.com/office/word/2010/wordprocessingInk");
        writer->addAttribute("xmlns:wne", "http://schemas.microsoft.com/office/word/2006/wordml");
        writer->addAttribute("xmlns:wps", "http://schemas.microsoft.com/office/word/2010/wordprocessingShape");
        writer->addAttribute("mc:Ignorable", "w14 w15 wp14");

        writer->startElement("w:body");
    }
    else {
        // FIXME: Do we have to add w:sectPr here always or only sometimes?

        writer->endElement(); // w:body
        writer->endElement(); // w:document
        writer->endDocument();
    }
}


// ----------------------------------------------------------------
// Text level functions: paragraphs, headings, sections, frames, objects, etc

void OdtReaderDocxBackend::elementTextH(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    Q_UNUSED(reader);
    Q_UNUSED(context);
}

void OdtReaderDocxBackend::elementTextP(KoXmlStreamReader &reader, OdfReaderContext *context)
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

void OdtReaderDocxBackend::elementTextSpan(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderDocxContext *docxContext = dynamic_cast<OdfReaderDocxContext*>(context);
    if (!docxContext) {
        return;
    }

    KoXmlWriter  *writer = docxContext->m_documentWriter;
    if (reader.isStartElement()) {
        startRun(writer, context);
        // FIXME: This is wrong.  text:span can be inside each other so we need to keep track of level.
        m_isInsideSpan = true;
    }
    else {
        endRun(writer, context);
        m_isInsideSpan = false;
    }
}

void OdtReaderDocxBackend::elementTextS(KoXmlStreamReader &reader, OdfReaderContext *context)
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


void OdtReaderDocxBackend::characterData(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    Q_UNUSED(reader);

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
    if (!m_isInsideSpan) {
        startRun(writer, context);
    }

    writer->startElement("w:t");
    writer->addTextNode(reader.text().toString());
    writer->endElement(); // w:t

    if (!m_isInsideSpan) {
        endRun(writer, context);
    }
}


// ----------------------------------------------------------------
//                         Private functions


void OdtReaderDocxBackend::startRun(KoXmlWriter *writer, OdfReaderContext *context)
{
    Q_UNUSED(context);

    writer->startElement("w:r");
    writer->startElement("w:rPr");
    // FIXME: Add run properties here
    writer->endElement(); // w:rPr
}

void OdtReaderDocxBackend::endRun(KoXmlWriter *writer, OdfReaderContext *context)
{
    Q_UNUSED(context);

    // FIXME: More here?
    writer->endElement(); // w:r
}
