/* This file is part of the KDE project
 *
 * Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>
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


#include "KoOdfStyle.h"

#include <QString>

#include "KoXmlReader.h"
#include "KoXmlNS.h"
#include "KoOdfStyleProperties.h"


// ================================================================
//                         class KoOdfStyle


class KoOdfStyle::Private
{
public:
    Private();
    ~Private();

    QString name;
    QString family;
    QString parent;
    QString displayName;
    bool    isDefaultStyle;

    bool    inUse;

    QHash<QString, KoOdfStyleProperties*> properties;  // e.g. "text-properties", 
};

KoOdfStyle::Private::Private()
    : isDefaultStyle(false)
    , inUse(false)
{
}

KoOdfStyle::Private::~Private()
{
    qDeleteAll(properties);
}


// ----------------------------------------------------------------


KoOdfStyle::KoOdfStyle()
    : d(new KoOdfStyle::Private())
{
}

KoOdfStyle::~KoOdfStyle()
{
    delete d;
}


QString KoOdfStyle::name() const
{
    return d->name;
}

void KoOdfStyle::setName(QString &name)
{
    d->name = name;
}

QString KoOdfStyle::family() const
{
    return d->family;
}

void KoOdfStyle::setFamily(QString &family)
{
    d->family = family;
}

QString KoOdfStyle::parent() const
{
    return d->parent;
}

void KoOdfStyle::setParent(QString &parent)
{
    d->parent = parent;
}

QString KoOdfStyle::displayName() const
{
    return d->displayName;
}

void KoOdfStyle::setDisplayName(QString &name)
{
    d->displayName = name;
}


bool KoOdfStyle::isDefaultStyle() const
{
    return d->isDefaultStyle;
}

void KoOdfStyle::setIsDefaultStyle(bool isDefaultStyle)
{
    d->isDefaultStyle = isDefaultStyle;
}



bool KoOdfStyle::inUse() const
{
    return d->inUse;
}

void KoOdfStyle::setInUse(bool inUse)
{
    d->inUse = inUse;
}


QHash<QString, KoOdfStyleProperties*> KoOdfStyle::properties()
{
    return d->properties;
}

KoOdfStyleProperties *KoOdfStyle::properties(QString &name) const
{
    return d->properties.value(name, 0);
}

QString KoOdfStyle::property(QString &propertySet, QString &property) const
{
    KoOdfStyleProperties *props = d->properties.value(propertySet, 0);
    if (props)
        return props->value(property);
    else
        return QString();
}

void KoOdfStyle::setProperty(QString &propertySet, QString &property, QString &value)
{
    KoOdfStyleProperties *props = d->properties.value(propertySet);
    if (!props)
        props = new KoOdfStyleProperties();
    props->setValue(property, value);
}


bool KoOdfStyle::loadOdf(KoXmlElement &element)
{
    // Load style attributes.
    QString dummy;              // Because the set*() methods take a QString &,
    dummy = element.attribute("family");
    setFamily(dummy);
    dummy = element.attribute("name", QString());
    setName(dummy);
    dummy = element.attribute("parent-style-name", QString());
    setParent(dummy);
    dummy = element.attribute("display-name", QString());
    setDisplayName(dummy);

    // Load child elements: property sets and other children.
    KoXmlElement elem;
    forEachElement(elem, element) {
        if (!(elem.namespaceURI() == KoXmlNS::style)) {
            continue;
        }

        // So far we only have support for text-, paragaph- and graphic-properties
        QString propertiesType = elem.localName();
        if (propertiesType == "text-properties"
            || propertiesType == "paragraph-properties"
            || propertiesType == "graphic-properties")
        {
            // FIXME: In the future, create per type.
            KoOdfStyleProperties *properties = new KoOdfStyleProperties();
            if (!properties->readOdf(elem)) {
                return false;
            }
            d->properties[propertiesType] = properties;
        }
    }

    return true;
}

