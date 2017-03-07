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

// Calligra
#include <KoXmlWriter.h>

// This filter
#include "OdfReaderDocxContext.h"
#include "DocxExportDebug.h"


#if 0
#define DEBUG_BACKEND() \
    debugDocx << (reader.isStartElement() ? "start": (reader.isEndElement() ? "end" : "other")) \
    << reader.qualifiedName().toString()
#else
#define DEBUG_BACKEND() \
    //NOTHING
#endif


// ================================================================
//                 class OdtReaderDocxBackend


OdtReaderDocxBackend::OdtReaderDocxBackend()
    : OdtReaderBackend()
{
}

OdtReaderDocxBackend::~OdtReaderDocxBackend()
{
}


// ----------------------------------------------------------------
// ODT document level functions


void OdtReaderDocxBackend::elementOfficeBody(KoXmlStreamReader &reader, OdfReaderContext *context)
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
        writer->addAttribute("xmlns:r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships");
        writer->addAttribute("xmlns:w", "http://schemas.openxmlformats.org/wordprocessingml/2006/main");

        writer->startElement("w:body");
    }
    else {
        // FIXME: Do we have to add w:sectPr here always or only sometimes?

        writer->endElement(); // w:body
        writer->endElement(); // w:document
        writer->endDocument();
    }
}
