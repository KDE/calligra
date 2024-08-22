/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "KoOdfStyle.h"

// Qt
#include <QString>

// Odflib
#include "KoOdfGraphicProperties.h"
#include "KoOdfParagraphProperties.h"
#include "KoOdfStyleProperties.h"
#include "KoOdfTextProperties.h"
#include "KoXmlStreamReader.h"
#include "KoXmlWriter.h"

#include "Odf2Debug.h"

// ================================================================
//                         class KoOdfStyle

class Q_DECL_HIDDEN KoOdfStyle::Private
{
public:
    Private();
    ~Private();

    QString family;
    QString parent;

    QHash<QString, KoOdfStyleProperties *> properties; // e.g. "text-properties",
};

KoOdfStyle::Private::Private() = default;

KoOdfStyle::Private::~Private()
{
    qDeleteAll(properties);
}

// ----------------------------------------------------------------

KoOdfStyle::KoOdfStyle()
    : KoOdfStyleBase(StyleStyle)
    , d(new KoOdfStyle::Private())
{
}

KoOdfStyle::~KoOdfStyle()
{
    delete d;
}

QString KoOdfStyle::family() const
{
    return d->family;
}

void KoOdfStyle::setFamily(const QString &family)
{
    d->family = family;
}

QString KoOdfStyle::parent() const
{
    return d->parent;
}

void KoOdfStyle::setParent(const QString &parent)
{
    d->parent = parent;
}

QHash<QString, KoOdfStyleProperties *> KoOdfStyle::properties() const
{
    return d->properties;
}

KoOdfStyleProperties *KoOdfStyle::properties(const QString &name) const
{
    return d->properties.value(name, 0);
}

QString KoOdfStyle::property(const QString &propertySet, const QString &property) const
{
    KoOdfStyleProperties *props = d->properties.value(propertySet, 0);
    if (props)
        return props->attribute(property);
    else
        return QString();
}

void KoOdfStyle::setProperty(const QString &propertySet, const QString &property, const QString &value)
{
    KoOdfStyleProperties *props = d->properties.value(propertySet);
    if (!props)
        props = new KoOdfStyleProperties();
    props->setAttribute(property, value);
}

bool KoOdfStyle::readOdf(KoXmlStreamReader &reader)
{
    // Load style attributes.
    KoXmlStreamAttributes attrs = reader.attributes();

    setName(attrs.value("style:name").toString());
    setDisplayName(attrs.value("style:display-name").toString());
    setFamily(attrs.value("style:family").toString());
    setParent(attrs.value("style:parent-style-name").toString());

    debugOdf2 << "Style:" << name() << family() << parent() << displayName();

    // Load child elements: property sets and other children.
    while (reader.readNextStartElement()) {
        // So far we only have support for text-, paragraph- and graphic-properties
        const QString propertiesType = reader.qualifiedName().toString();
        // Create a new propertyset variable depending on the type of properties.
        KoOdfStyleProperties *properties = nullptr;
        if (propertiesType == "style:text-properties") {
            properties = new KoOdfTextProperties();
        } else if (propertiesType == "style:paragraph-properties") {
            properties = new KoOdfParagraphProperties();
        } else if (propertiesType == "style:graphic-properties") {
            properties = new KoOdfGraphicProperties();
        }

        if (properties) {
            if (!properties->readOdf(reader)) {
                delete properties;
                return false;
            }
            d->properties[propertiesType] = properties;
        }
    }

    return true;
}

bool KoOdfStyle::saveOdf(KoXmlWriter *writer)
{
    if (isDefaultStyle()) {
        writer->startElement("style:default-style");
    } else {
        writer->startElement("style:style");
        writer->addAttribute("style:name", name());
    }

    // Write style attributes
    writer->addAttribute("style:family", family());
    if (!d->parent.isEmpty()) {
        writer->addAttribute("style:parent-style-name", d->parent);
    }
    if (!displayName().isEmpty()) {
        writer->addAttribute("style:display-name", displayName());
    }

    // Write properties
    foreach (const QString &propertySet, d->properties.keys()) {
        d->properties.value(propertySet)->saveOdf(propertySet, writer);
    }

    writer->endElement(); // style:{default-,}style
    return true;
}
