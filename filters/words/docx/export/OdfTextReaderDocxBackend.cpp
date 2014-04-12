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
#include <KoOdfStyle.h>
#include <KoOdfStyleManager.h>
#include <KoOdfStyleProperties.h>

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
        KoXmlStreamAttributes attributes = reader.attributes();

        // FIXME: Add paragraph attributes here
        writer->startElement("w:pPr");
        QString textStyle = attributes.value("text:style-name").toString();
        KoOdfStyle *style = context->styleManager()->style(textStyle);
        writer->startElement("w:rPr");
        QString parent = style->parent();
        if (!parent.isEmpty()) {
            writer->startElement("w:rStyle");
            writer->addAttribute("w:val", parent);
            writer->endElement(); // w:rStyle
        }
        writer->endElement(); // w:rPr
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
        startRun(reader, docxContext);
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
        startRun(reader, docxContext);
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


void OdfTextReaderDocxBackend::startRun(KoXmlStreamReader &reader, OdfReaderDocxContext *docxContext)
{
    KoXmlWriter *writer = docxContext->m_documentWriter;
    writer->startElement("w:r");
    KoXmlStreamAttributes attributes = reader.attributes();
    writer->startElement("w:rPr");
    QString textStyle = attributes.value("text:style-name").toString();
    if (!textStyle.isEmpty()) {
        KoOdfStyle *style = docxContext->styleManager()->style(textStyle);
        KoOdfStyleProperties *properties = style->properties("style:text-properties");
        QString parent = style->parent();
        if (!parent.isEmpty()) {
            writer->startElement("w:rStyle");
            writer->addAttribute("w:val", parent);
            writer->endElement(); // w:rStyle
        }
        if (properties != 0) {
            QString fontWeight = properties->attribute("fo:font-weight");
            if (fontWeight == "bold") {
                writer->startElement("w:b");
                writer->addAttribute("w:val", "1");
                writer->endElement(); // w:b
                writer->startElement("w:bCs");
                writer->addAttribute("w:val", "1");
                writer->endElement(); // w:bCs
            }
            QString underlineStyle = properties->attribute("style:text-underline-style");
            if (!underlineStyle.isEmpty()) {
                if (underlineStyle == "solid") {
                    writer->startElement("w:u");
                    writer->addAttribute("w:val", "single");
                    writer->endElement(); //:u
                }
            }
        }
    }
    // FIXME: Add more run properties here
    writer->endElement(); // w:rPr
}

void OdfTextReaderDocxBackend::endRun(OdfReaderDocxContext *docxContext)
{
    // FIXME: More here?
    KoXmlWriter  *writer = docxContext->m_documentWriter;
    writer->endElement(); // w:r
}
