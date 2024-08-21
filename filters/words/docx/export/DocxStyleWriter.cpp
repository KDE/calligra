/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2014 Lassi Nieminen <lassniem@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "DocxStyleWriter.h"

// Qt
#include <QBuffer>
#include <QByteArray>

// Calligra
#include <KoOdfStyle.h>
#include <KoOdfStyleManager.h>
#include <KoOdfStyleProperties.h>
#include <KoXmlWriter.h>
#include <OdfReaderContext.h>

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

QByteArray DocxStyleWriter::documentContent() const
{
    return m_documentContent;
}

void DocxStyleWriter::read()
{
    KoOdfStyleManager *manager = m_readerContext->styleManager();
    m_documentWriter->startElement("w:styles");
    m_documentWriter->addAttribute("xmlns:r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships");
    m_documentWriter->addAttribute("xmlns:w", "http://schemas.openxmlformats.org/wordprocessingml/2006/main");

    QList<KoOdfStyle *> defaultStyles = manager->defaultStyles();
    m_documentWriter->startElement("w:docDefaults");
    foreach (KoOdfStyle *style, defaultStyles) {
        if (!style->isFromStylesXml()) {
            continue;
        }
        QString family = style->family();
        if (family == "paragraph") {
            m_documentWriter->startElement("w:pPrDefault");
            m_documentWriter->startElement("w:pPr");
            KoOdfStyleProperties *paragraphProperties = style->properties("style:paragraph-properties");
            DocxStyleHelper::handleParagraphStyles(paragraphProperties, m_documentWriter);
            KoOdfStyleProperties *textProperties = style->properties("style:text-properties");
            m_documentWriter->startElement("w:rPr");
            DocxStyleHelper::handleTextStyles(textProperties, m_documentWriter);
            m_documentWriter->endElement(); // w:rPr
            m_documentWriter->endElement(); // w:pPr
            m_documentWriter->endElement(); // w:pPrDefault
        } else if (family == "text") {
            m_documentWriter->startElement("w:rPrDefault");
            m_documentWriter->startElement("w:rPr");
            KoOdfStyleProperties *textProperties = style->properties("style:text-properties");
            DocxStyleHelper::handleTextStyles(textProperties, m_documentWriter);
            m_documentWriter->endElement(); // w:rPr
            m_documentWriter->endElement(); // w:rPrDefault
        }
    }
    m_documentWriter->endElement(); // w:docDefaults

    QList<KoOdfStyle *> styles = manager->styles();
    foreach (KoOdfStyle *style, styles) {
        if (!style->isFromStylesXml()) {
            continue;
        }
        QString family = style->family();
        QString parent = style->parent();

        if (family == "paragraph") {
            m_documentWriter->startElement("w:style");
            m_documentWriter->addAttribute("w:type", "paragraph");
            m_documentWriter->addAttribute("w:styleId", style->name());
            m_documentWriter->startElement("w:name");
            QString displayName = style->displayName();
            if (displayName.isEmpty()) {
                displayName = style->name();
            }
            m_documentWriter->addAttribute("w:val", displayName);
            m_documentWriter->endElement(); // w:name
            if (!parent.isEmpty()) {
                m_documentWriter->startElement("w:basedOn");
                m_documentWriter->addAttribute("w:val", parent);
                m_documentWriter->endElement(); // w:basedOn
            }
            m_documentWriter->startElement("w:pPr");
            KoOdfStyleProperties *paragraphProperties = style->properties("style:paragraph-properties");
            DocxStyleHelper::handleParagraphStyles(paragraphProperties, m_documentWriter);
            m_documentWriter->endElement(); // w:pPr
            KoOdfStyleProperties *textProperties = style->properties("style:text-properties");
            m_documentWriter->startElement("w:rPr");
            KoOdfStyleProperties properties;
            if (!parent.isEmpty()) {
                DocxStyleHelper::inheritTextStyles(&properties, parent, manager);
            }
            if (textProperties != nullptr) {
                properties.copyPropertiesFrom(*textProperties);
            }
            DocxStyleHelper::handleTextStyles(&properties, m_documentWriter);
            m_documentWriter->endElement(); // w:rPr
            m_documentWriter->endElement(); // w:style
        } else if (family == "text") {
            m_documentWriter->startElement("w:style");
            m_documentWriter->addAttribute("w:type", "character");
            m_documentWriter->addAttribute("w:styleId", style->name());
            m_documentWriter->startElement("w:name");
            QString displayName = style->displayName();
            if (displayName.isEmpty()) {
                displayName = style->name();
            }
            m_documentWriter->addAttribute("w:val", displayName);
            m_documentWriter->endElement(); // w:name
            if (!parent.isEmpty()) {
                m_documentWriter->startElement("w:basedOn");
                m_documentWriter->addAttribute("w:val", parent);
                m_documentWriter->endElement(); // w:basedOn
            }
            KoOdfStyleProperties *textProperties = style->properties("style:text-properties");
            m_documentWriter->startElement("w:rPr");
            DocxStyleHelper::handleTextStyles(textProperties, m_documentWriter);
            m_documentWriter->endElement(); // w:rPr
            m_documentWriter->endElement(); // w:style
        }
    }

    m_documentWriter->endElement(); // w:styles
}
