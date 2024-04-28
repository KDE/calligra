/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>
 * SPDX-FileCopyrightText: 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "KoOdfListStyle.h"

// Qt
#include <QString>

// Odflib
#include "KoOdfListLevelProperties.h"
#include "KoOdfStyleProperties.h"
#include "KoOdfTextProperties.h"
#include "KoXmlStreamReader.h"
#include "KoXmlWriter.h"

#include "Odf2Debug.h"

// ================================================================
//                         class KoOdfListStyle

class Q_DECL_HIDDEN KoOdfListStyle::Private
{
public:
    Private();
    ~Private();

    QString name;
    QString displayName;
    QString listLevelType;
    bool isDefaultStyle;
    bool inUse;
    //
    QHash<QString, KoOdfStyleProperties *> properties; // e.g. "text-properties",
};

KoOdfListStyle::Private::Private()
    : isDefaultStyle(false)
    , inUse(false)
{
}

KoOdfListStyle::Private::~Private()
{
    qDeleteAll(properties);
}

KoOdfListStyle::KoOdfListStyle()
    : d(new KoOdfListStyle::Private())
{
}

KoOdfListStyle::~KoOdfListStyle()
{
    delete d;
}

QString KoOdfListStyle::name() const
{
    return d->name;
}

void KoOdfListStyle::setName(QString &name)
{
    d->name = name;
}

QString KoOdfListStyle::displayName() const
{
    return d->displayName;
}

void KoOdfListStyle::setDisplayName(QString &name)
{
    d->displayName = name;
}

QString KoOdfListStyle::listLevelStyleType() const
{
    return d->listLevelType;
}

void KoOdfListStyle::setListLevelStyleType(QString &name)
{
    d->listLevelType = name;
}

bool KoOdfListStyle::inUse() const
{
    return d->inUse;
}

void KoOdfListStyle::setInUse(bool inUse)
{
    d->inUse = inUse;
}

QHash<QString, KoOdfStyleProperties *> KoOdfListStyle::properties()
{
    return d->properties;
}

KoOdfStyleProperties *KoOdfListStyle::properties(QString &name) const
{
    return d->properties.value(name, 0);
}

QString KoOdfListStyle::property(QString &propertySet, QString &property) const
{
    KoOdfStyleProperties *props = d->properties.value(propertySet, 0);
    if (props)
        return props->attribute(property);
    else
        return QString();
}

void KoOdfListStyle::setProperty(QString &propertySet, QString &property, QString &value)
{
    KoOdfStyleProperties *props = d->properties.value(propertySet);
    if (!props)
        props = new KoOdfStyleProperties();
    props->setAttribute(property, value);
}

bool KoOdfListStyle::readProperties(KoXmlStreamReader &reader)
{
    // Load child elements: property sets and other children.
    while (reader.readNextStartElement()) {
        // So far we only have support for text-properties and list-level-properties.
        QString propertiesType = reader.qualifiedName().toString();
        debugOdf2 << "properties type: " << propertiesType;

        // Create a new propertyset variable depending on the type of properties.
        KoOdfStyleProperties *properties;
        if (propertiesType == "style:text-properties") {
            properties = new KoOdfTextProperties();
        } else if (propertiesType == "style:list-level-properties") {
            properties = new KoOdfListLevelProperties();
        } else {
            // FIXME: support office:binary-data
            // debugOdf2 << "Unsupported property type: " << propertiesType;
            reader.skipCurrentElement();
            continue;
        }

        if (!properties->readOdf(reader)) {
            return false;
        }
        d->properties[propertiesType] = properties;
    }

    return true;
}

bool KoOdfListStyle::readOdf(KoXmlStreamReader &reader)
{
    // Load style attributes.
    KoXmlStreamAttributes attrs = reader.attributes();
    QString dummy; // Because the set*() methods take a QString &,

    dummy = attrs.value("style:name").toString();
    setName(dummy);
    dummy = attrs.value("style:display-name").toString();
    setDisplayName(dummy);

    debugOdf2 << "Style:" << name() << displayName();

    // Load child elements: list-level-style-bullet, text:list-level-style-number, text:list-level-style-image
    while (reader.readNextStartElement()) {
        // So far we only have support for text-, paragraph- and graphic-properties
        // And list-level-style-bullet, list-level-style-image, list-level-style-number for List-style.
        QString listLevelType = reader.qualifiedName().toString();
        setListLevelStyleType(listLevelType);
        if (listLevelType == "text:list-level-style-bullet" || listLevelType == "text:list-level-style-number"
            || listLevelType == "text:list-level-style-image") {
            debugOdf2 << "List Level style type" << listLevelType;
            if (!readProperties(reader)) {
                return false;
            }
        }
    }

    return true;
}

bool KoOdfListStyle::saveOdf(KoXmlWriter *writer)
{
    writer->startElement("text:list-style");
    // Write style attributes
    if (!d->displayName.isEmpty()) {
        writer->addAttribute("style:display-name", d->displayName);
    }

    // Write child element
    writer->startElement(listLevelStyleType().toUtf8());
    // Write properties
    foreach (const QString &propertySet, d->properties.keys()) {
        d->properties.value(propertySet)->saveOdf(propertySet, writer);
    }
    writer->endElement();

    writer->endElement();
    return true;
}
