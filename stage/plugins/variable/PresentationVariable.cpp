/*
* This file is part of the KDE project
*
* SPDX-FileCopyrightText: 2009 Nokia Corporation and /or its subsidiary(-ies).
*
* Contact: Amit Aggarwal <amit.5.aggarwal@nokia.com>
*
* SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>
*
SPDX-License-Identifier: LGPL-2.1-or-later
*
*/

#include "PresentationVariable.h"

#include <KPrPage.h>
#include <KoPATextPage.h>
#include <KoProperties.h>
#include <KoShape.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutRootArea.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

PresentationVariable::PresentationVariable()
    : KoVariable(true)
    , m_type(KPrDeclarations::Footer)
{
}

void PresentationVariable::setProperties(const KoProperties *props)
{
    switch (props->intProperty("vartype")) {
    case 1:
        m_type = KPrDeclarations::Header;
        break;
    case 2:
        m_type = KPrDeclarations::Footer;
        break;
    case 3:
        m_type = KPrDeclarations::DateTime;
        break;
    default:
        Q_ASSERT(false);
        break;
    }
}

void PresentationVariable::resize(const QTextDocument *document, QTextInlineObject &object, int posInDocument, const QTextCharFormat &format, QPaintDevice *pd)
{
    KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout *>(document->documentLayout());
    if (lay) {
        KoTextLayoutRootArea *rootArea = lay->rootAreaForPosition(posInDocument);
        if (rootArea) {
            if (KoPATextPage *textPage = dynamic_cast<KoPATextPage *>(rootArea->page())) {
                if (KPrPage *page = dynamic_cast<KPrPage *>(textPage->page())) {
                    setValue(page->declaration(m_type));
                }
            }
        }
    }
    KoVariable::resize(document, object, posInDocument, format, pd);
}

void PresentationVariable::saveOdf(KoShapeSavingContext &context)
{
    KoXmlWriter *writer = &context.xmlWriter();
    const char *type = "";
    switch (m_type) {
    case KPrDeclarations::Footer:
        type = "presentation:footer";
        break;
    case KPrDeclarations::Header:
        type = "presentation:header";
        break;
    case KPrDeclarations::DateTime:
        type = "presentation:date-time";
        break;
    }
    writer->startElement(type);
    writer->endElement();
}

bool PresentationVariable::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    Q_UNUSED(context);
    const QString localName(element.localName());

    if (localName == "footer") {
        m_type = KPrDeclarations::Footer;
    } else if (localName == "header") {
        m_type = KPrDeclarations::Header;
    } else if (localName == "date-time") {
        m_type = KPrDeclarations::DateTime;
    }
    return true;
}
