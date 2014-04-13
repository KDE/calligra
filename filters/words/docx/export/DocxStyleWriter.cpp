/* This file is part of the KDE project

   Copyright (C) 2014 Lassi Nieminen <lassniem@gmail.com>

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
#include "DocxStyleWriter.h"

// Qt
#include <QByteArray>
#include <QBuffer>

// Calligra
#include <KoXmlWriter.h>
#include <OdfReaderContext.h>
#include <KoOdfStyleManager.h>
#include <KoOdfStyle.h>

// This filter
#include "DocxStyleHelper.h"

// ----------------------------------------------------------------
//                     class DocxStyleWriter

DocxStyleWriter::DocxStyleWriter(OdfReaderContext *context)
    : m_readerContext(context)
    , m_documentContent()
    , m_documentIO(&m_documentContent)
{
    m_documentWriter = new KoXmlWriter(&m_documentIO);
}

DocxStyleWriter::~DocxStyleWriter()
{
    delete m_documentWriter;
}

void DocxStyleWriter::read()
{
    KoOdfStyleManager *manager = m_readerContext->styleManager();
    m_documentWriter->startElement("w:styles");
    m_documentWriter->addAttribute("xmlns:r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships");
    m_documentWriter->addAttribute("xmlns:w", "http://schemas.openxmlformats.org/wordprocessingml/2006/main");

    QList<KoOdfStyle*> styles = manager->styles();
    foreach (KoOdfStyle* style, styles) {
        if (!style->isFromStylesXml()) {
            continue;
        }
        QString family = style->family();
        if (family == "paragraph") {
            m_documentWriter->startElement("w:style");
            m_documentWriter->addAttribute("w:type", "paragraph");
            m_documentWriter->addAttribute("w:styleId", style->name());
            m_documentWriter->startElement("w:name");
            QString displayName = style->displayName();
            if (displayName.isEmpty())
            {
                displayName = style->name();
            }
            m_documentWriter->addAttribute("w:val", displayName);
            m_documentWriter->endElement(); // w:name
            m_documentWriter->startElement("w:pPr");
            KoOdfStyleProperties *paragraphProperties = style->properties("style:paragraph-properties");
            DocxStyleHelper::handleParagraphStyles(paragraphProperties, m_documentWriter);
            KoOdfStyleProperties *textProperties = style->properties("style:text-properties");
            m_documentWriter->startElement("w:rPr");
            DocxStyleHelper::handleTextStyles(textProperties, m_documentWriter);
            m_documentWriter->endElement(); // w:rPr
            m_documentWriter->endElement(); // w:pPr
            m_documentWriter->endElement(); // w:style
        }
        else if (family == "text") {
            m_documentWriter->startElement("w:style");
            m_documentWriter->addAttribute("w:type", "text");
            m_documentWriter->addAttribute("w:styleId", style->name());
            m_documentWriter->startElement("w:name");
            QString displayName = style->displayName();
            if (displayName.isEmpty())
            {
                displayName = style->name();
            }
            m_documentWriter->addAttribute("w:val", displayName);
            m_documentWriter->endElement(); // w:name
            KoOdfStyleProperties *textProperties = style->properties("style:text-properties");
            m_documentWriter->startElement("w:rPr");
            DocxStyleHelper::handleTextStyles(textProperties, m_documentWriter);
            m_documentWriter->endElement(); // w:rPr
            m_documentWriter->endElement(); // w:style
        }
    }

    m_documentWriter->endElement(); // w:styles
}
