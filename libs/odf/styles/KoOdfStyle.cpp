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
#include <QMap>


// ================================================================
//                 class KoOdfStylePrpoerties


class KoOdfStyleProperties::Private
{
public:
    Private() {};

    // We could use a QHash instead but I like that the values are saved in order.
    QMap<QString, QString> attributes;  // name, value
};


// ----------------------------------------------------------------


KoOdfStyleProperties::KoOdfStyleProperties()
    : d(new KoOdfStyleProperties::Private())
{
}

KoOdfStyleProperties::~KoOdfStyleProperties()
{
    delete d;
}


QString KoOdfStyleProperties::value(QString &property) const
{
    return d->attributes.value(property, QString());
}

void KoOdfStyleProperties::setValue(QString &property, QString &value)
{
    d->attributes[property] = value;
}


void KoOdfStyleProperties::clear()
{
    d->attributes.clear();
}


// ================================================================
//                         class KoOdfStyle


class KoOdfStyle::Private
{
public:
    Private() {};
    ~Private();

    QString name;
    QString family;
    QString parent;
    bool    isDefaultStyle;

    QMap<QString, KoOdfStyleProperties*> properties;  // e.g. "text-properties", 
};

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


bool KoOdfStyle::isDefaultStyle() const
{
    return d->isDefaultStyle;
}

void KoOdfStyle::setIsDefaultStyle(bool isDefaultStyle)
{
    d->isDefaultStyle = isDefaultStyle;
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

