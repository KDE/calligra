/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "OdtReaderDocxBackend.h"

// Qt
#include <QtGlobal>

// Calligra
#include <KoXmlWriter.h>

// This filter
#include "DocxExportDebug.h"
#include "OdfReaderDocxContext.h"

#if 0
#define DEBUG_BACKEND() debugDocx << (reader.isStartElement() ? "start" : (reader.isEndElement() ? "end" : "other")) << reader.qualifiedName().toString()
#else
#define DEBUG_BACKEND() // NOTHING
#endif

// ================================================================
//                 class OdtReaderDocxBackend

OdtReaderDocxBackend::OdtReaderDocxBackend()
    : OdtReaderBackend()
{
}

OdtReaderDocxBackend::~OdtReaderDocxBackend() = default;

// ----------------------------------------------------------------
// ODT document level functions

void OdtReaderDocxBackend::elementOfficeBody(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderDocxContext *docxContext = dynamic_cast<OdfReaderDocxContext *>(context);
    if (!docxContext) {
        return;
    }

    KoXmlWriter *writer = docxContext->m_documentWriter;
    if (reader.isStartElement()) {
        writer->startDocument(nullptr);

        // Start the document and add all necessary namespaces to it.
        writer->startElement("w:document");
        writer->addAttribute("xmlns:r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships");
        writer->addAttribute("xmlns:w", "http://schemas.openxmlformats.org/wordprocessingml/2006/main");

        writer->startElement("w:body");
    } else {
        // FIXME: Do we have to add w:sectPr here always or only sometimes?

        writer->endElement(); // w:body
        writer->endElement(); // w:document
        writer->endDocument();
    }
}
