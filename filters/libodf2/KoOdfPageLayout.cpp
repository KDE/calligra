/* This file is part of the KDE project
 *
 * Copyright (C) 2014 Inge Wallin <inge@lysator.liu.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */


// Own
#include "KoOdfPageLayout.h"

// Qt
#include <QString>

// Odflib
#include "KoXmlStreamReader.h"
#include "KoXmlWriter.h"
#include "KoOdfStyleProperties.h"
#include "KoOdfPageLayoutProperties.h"
#include "KoOdfHeaderFooterProperties.h"

#include "Odf2Debug.h"

// ================================================================
//                         class KoOdfPageLayout


class Q_DECL_HIDDEN KoOdfPageLayout::Private
{
public:
    Private();
    ~Private();

    QString pageUsage;

    KoOdfPageLayoutProperties   *pageLayoutProperties;
    KoOdfHeaderFooterProperties *headerProperties;
    KoOdfHeaderFooterProperties *footerProperties;
};

KoOdfPageLayout::Private::Private()
    : pageLayoutProperties(0)
    , headerProperties(0)
    , footerProperties(0)
{
}

KoOdfPageLayout::Private::~Private()
{
    delete pageLayoutProperties;
    delete headerProperties;
    delete footerProperties;
}


// ----------------------------------------------------------------


KoOdfPageLayout::KoOdfPageLayout()
    : KoOdfStyleBase(PageLayout)
    , d(new KoOdfPageLayout::Private())
{
}

KoOdfPageLayout::~KoOdfPageLayout()
{
    delete d;
}


QString KoOdfPageLayout::pageUsage() const
{
    return d->pageUsage;
}

void KoOdfPageLayout::setPageUsage(const QString &pageUsage)
{
    d->pageUsage = pageUsage;
}


KoOdfPageLayoutProperties *KoOdfPageLayout::pageLayoutProperties() const
{
    if (!d->pageLayoutProperties) {
        d->pageLayoutProperties = new KoOdfPageLayoutProperties();
    }

    return d->pageLayoutProperties;
}

KoOdfHeaderFooterProperties *KoOdfPageLayout::headerProperties() const
{
    if (!d->headerProperties) {
        d->headerProperties = new KoOdfHeaderFooterProperties();
    }

    return d->headerProperties;
}

KoOdfHeaderFooterProperties *KoOdfPageLayout::footerProperties() const
{
    if (!d->footerProperties) {
        d->footerProperties = new KoOdfHeaderFooterProperties();
    }

    return d->footerProperties;
}


bool KoOdfPageLayout::readOdf(KoXmlStreamReader &reader)
{
    bool retval = true;

    // Load style attributes.
    KoXmlStreamAttributes  attrs = reader.attributes();
    setName(attrs.value("style:name").toString());
    setDisplayName(attrs.value("style:display-name").toString());
    setPageUsage(attrs.value("style:page-usage").toString());

    debugOdf2 << "PageLayout:" << name() << displayName() << pageUsage();

    // Load child elements: property sets and other children.
    while (reader.readNextStartElement()) {

        // Create a new propertyset variable depending on the type of properties.
        const QString propertiesType = reader.qualifiedName().toString();

        KoOdfStyleProperties *properties = 0;
        if (propertiesType == "style:page-layout-properties") {
            properties = new KoOdfPageLayoutProperties();
            if (!properties->readOdf(reader)) {
                delete properties;
                retval = false;
                break;
            }
            d->pageLayoutProperties = dynamic_cast<KoOdfPageLayoutProperties*>(properties);
        }

        else if (propertiesType == "style:header-style") {
            // The header/footer properties are contained inside a
            // style element so we need to read past that.
            reader.readNextStartElement();
            if (reader.qualifiedName() != "style:header-footer-properties") {
                reader.skipCurrentElement();
                return false;
            }
            properties = new KoOdfHeaderFooterProperties();

            if (!properties->readOdf(reader)) {
                delete properties;
                retval = false;
                break;
            }
            d->headerProperties = dynamic_cast<KoOdfHeaderFooterProperties*>(properties);

            // Read past the end element for the header style;
            reader.skipCurrentElement();
        }

        else if (propertiesType == "style:footer-style") {
            // The header/footer properties are contained inside a
            // style element so we need to read past that.
            reader.readNextStartElement();
            if (reader.qualifiedName() != "style:header-footer-properties") {
                reader.skipCurrentElement();
                return false;
            }
            properties = new KoOdfHeaderFooterProperties();

            if (!properties->readOdf(reader)) {
                delete properties;
                return false;
            }
            d->footerProperties = dynamic_cast<KoOdfHeaderFooterProperties*>(properties);

            // Read past the end element for the footer style;
            reader.skipCurrentElement();
        }
    }

    // Skip rest of each element including children that are not read yet (shouldn't be any).
    reader.skipCurrentElement();

    return retval;
}

bool KoOdfPageLayout::saveOdf(KoXmlWriter *writer)
{
    if (isDefaultStyle()) {
        writer->startElement("style:default-page-layout");
    }
    else {
        writer->startElement("style:page-layout");
        writer->addAttribute("style:name", name());
    }

    // Write style attributes
    writer->addAttribute("style:page-usage", pageUsage());

    // Write properties
    if (d->pageLayoutProperties) {
        d->pageLayoutProperties->saveOdf("", writer);
    }
    if (d->headerProperties) {
        writer->startElement("style:header-style");
        d->headerProperties->saveOdf("", writer);
        writer->endElement();  // style:header-style
    }
    if (d->footerProperties) {
        writer->startElement("style:footer-style");
        d->footerProperties->saveOdf("", writer);
        writer->endElement();  // style:footer-style
    }

    writer->endElement();  // style:{default-,}page-layout
    return true;
}
