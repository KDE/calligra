/* This file is part of the KDE project
 *
 * Copyright (C) 2013-2014 Inge Wallin <inge@lysator.liu.se>
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
#include "KoOdfStyleBase.h"

// Qt
#include <QString>

// Odflib
#include "KoXmlStreamReader.h"
//#include "KoOdfStyleProperties.h"
//#include "KoOdfTextProperties.h"
//#include "KoOdfParagraphProperties.h"
//#include "KoOdfGraphicProperties.h"

#include "Odf2Debug.h"

// ================================================================
//                         class KoOdfStyleBase


class Q_DECL_HIDDEN KoOdfStyleBase::Private
{
public:
    Private(KoOdfStyleBase::StyleType type);
    ~Private();

    KoOdfStyleBase::StyleType  type;

    QString name;
    QString displayName;

    bool    isDefaultStyle;
    bool    inUse;
    bool    isFromStylesXml;
};

KoOdfStyleBase::Private::Private(KoOdfStyleBase::StyleType type)
    : type(type)
    , isDefaultStyle(false)
    , inUse(false)
    , isFromStylesXml(false)
{
}

KoOdfStyleBase::Private::~Private()
{
}


// ----------------------------------------------------------------


KoOdfStyleBase::KoOdfStyleBase(KoOdfStyleBase::StyleType type)
    : d(new KoOdfStyleBase::Private(type))
{
}

KoOdfStyleBase::~KoOdfStyleBase()
{
    delete d;
}


KoOdfStyleBase::StyleType KoOdfStyleBase::type() const
{
    return d->type;
}


QString KoOdfStyleBase::name() const
{
    return d->name;
}

void KoOdfStyleBase::setName(const QString &name)
{
    d->name = name;
}

QString KoOdfStyleBase::displayName() const
{
    return d->displayName;
}

void KoOdfStyleBase::setDisplayName(const QString &name)
{
    d->displayName = name;
}


bool KoOdfStyleBase::isDefaultStyle() const
{
    return d->isDefaultStyle;
}

void KoOdfStyleBase::setIsDefaultStyle(bool isDefaultStyle)
{
    d->isDefaultStyle = isDefaultStyle;
}



bool KoOdfStyleBase::inUse() const
{
    return d->inUse;
}

void KoOdfStyleBase::setInUse(bool inUse)
{
    d->inUse = inUse;
}

bool KoOdfStyleBase::isFromStylesXml() const
{
    return d->isFromStylesXml;
}

void KoOdfStyleBase::setIsFromStylesXml(bool isFromStylesXml)
{
    d->isFromStylesXml = isFromStylesXml;
}

