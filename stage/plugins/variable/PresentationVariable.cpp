/*
* This file is part of the KDE project
*
* Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
*
* Contact: Amit Aggarwal <amit.5.aggarwal@nokia.com>
*
* Copyright (C) 2010 Thorsten Zachmann <zachmann@kde.org>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
* 02110-1301 USA
*
*/

#include "PresentationVariable.h"

#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoProperties.h>
#include <KoShape.h>
#include <KoShapeSavingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutRootArea.h>
#include <KoXmlNS.h>
#include <KoPATextPage.h>
#include <KPrPage.h>

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
    KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout*>(document->documentLayout());
    if (lay) {
        KoTextLayoutRootArea *rootArea = lay->rootAreaForPosition(posInDocument);
        if (rootArea) {
            if (KoPATextPage *textPage = dynamic_cast<KoPATextPage*>(rootArea->page())) {
                if (KPrPage *page = dynamic_cast<KPrPage*>(textPage->page())) {
                    setValue(page->declaration(m_type));
                }
            }
        }
    }
    KoVariable::resize(document, object, posInDocument, format, pd);
}

void PresentationVariable::saveOdf(KoShapeSavingContext & context)
{
    KoXmlWriter *writer = &context.xmlWriter();
    const char * type = "";
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

bool PresentationVariable::loadOdf(const KoXmlElement & element, KoShapeLoadingContext & context)
{
    Q_UNUSED(context);
    const QString localName(element.localName());

    if (localName == "footer") {
        m_type = KPrDeclarations::Footer;
    }
    else if (localName == "header") {
        m_type = KPrDeclarations::Header;
    }
    else if (localName == "date-time") {
        m_type = KPrDeclarations::DateTime;
    }
    return true;
}
