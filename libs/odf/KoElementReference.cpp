/*
 *  Copyright (c) 2011 Boudewijn Rempt <boud@valdyas.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KoElementReference.h"

#include "KoXmlReader.h"
#include "KoXmlWriter.h"

KoElementReference::KoElementReference()
    : d(new KoElementReferenceData())
{
}

KoElementReference::KoElementReference(const QString &xmlid)
    : d(new KoElementReferenceData)
{
    d->xmlid = xmlid;
}

KoElementReference::KoElementReference(const KoElementReference &other)
    : d(other.d)
{
}

KoElementReference &KoElementReference::operator=(const KoElementReference &rhs)
{
    if (this == &rhs) return *this;
    d = rhs.d;

    return *this;
}

bool KoElementReference::operator==(const KoElementReference &other)
{
    return d->xmlid == other.d->xmlid;
}

bool KoElementReference::operator!=(const KoElementReference &other)
{
    return !(*this == other);
}

void KoElementReference::saveOdf(KoXmlWriter *writer, SaveOptions saveOptions) const
{
    if (saveOptions & XMLID)
        writer->addAttribute("xml:id", d->xmlid);
    if (saveOptions & DRAWID)
        writer->addAttribute("draw:id", d->xmlid);
    if (saveOptions & TEXTID)
        writer->addAttribute("text:id", d->xmlid);
}

static KoElementReference loadOdf(const KoXmlElement &element)
{
    QString xmlid;

    if (element.hasAttribute("xml:id")) {
        xmlid = element.attribute("xml:id");
    }
    else if (element.hasAttribute("draw:id")) {
        xmlid = element.attribute("draw:id");
    }
    else if (element.hasAttribute("text:id")) {
        xmlid = element.attribute("text:id");
    }

    KoElementReference ref(xmlid);

    return ref;
}


