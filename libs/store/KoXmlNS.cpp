/* This file is part of the KDE project
   Copyright (C) 2004 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KoXmlNS.h"

#include <string.h>

const QString KoXmlNS::office(QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:office:1.0"));
const QString KoXmlNS::meta(QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:meta:1.0"));
const QString KoXmlNS::config(QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:config:1.0"));
const QString KoXmlNS::text(QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:text:1.0"));
const QString KoXmlNS::table(QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:table:1.0"));
const QString KoXmlNS::draw(QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:drawing:1.0"));
const QString KoXmlNS::presentation(QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:presentation:1.0"));
const QString KoXmlNS::dr3d(QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0"));
const QString KoXmlNS::chart(QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:chart:1.0"));
const QString KoXmlNS::form(QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:form:1.0"));
const QString KoXmlNS::script(QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:script:1.0"));
const QString KoXmlNS::style(QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:style:1.0"));
const QString KoXmlNS::number(QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0"));
const QString KoXmlNS::manifest(QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:manifest:1.0"));
const QString KoXmlNS::anim(QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:animation:1.0"));

const QString KoXmlNS::math(QStringLiteral("http://www.w3.org/1998/Math/MathML"));
const QString KoXmlNS::svg(QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0"));
const QString KoXmlNS::fo(QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0"));
const QString KoXmlNS::dc(QStringLiteral("http://purl.org/dc/elements/1.1/"));
const QString KoXmlNS::xlink(QStringLiteral("http://www.w3.org/1999/xlink"));
const QString KoXmlNS::VL(QStringLiteral("http://openoffice.org/2001/versions-list"));
const QString KoXmlNS::smil(QStringLiteral("urn:oasis:names:tc:opendocument:xmlns:smil-compatible:1.0"));
const QString KoXmlNS::xhtml(QStringLiteral("http://www.w3.org/1999/xhtml"));
const QString KoXmlNS::xml(QStringLiteral("http://www.w3.org/XML/1998/namespace"));

const QString KoXmlNS::calligra = QStringLiteral("http://www.calligra.org/2005/");
const QString KoXmlNS::officeooo = QStringLiteral("http://openoffice.org/2009/office");
const QString KoXmlNS::ooo = QStringLiteral("http://openoffice.org/2004/office");

const QString KoXmlNS::delta(QStringLiteral("http://www.deltaxml.com/ns/track-changes/delta-namespace"));
const QString KoXmlNS::split(QStringLiteral("http://www.deltaxml.com/ns/track-changes/split-namespace"));
const QString KoXmlNS::ac(QStringLiteral("http://www.deltaxml.com/ns/track-changes/attribute-change-namespace"));

const char* KoXmlNS::nsURI2NS(const QString &nsURI)
{
    if (nsURI == KoXmlNS::office)
        return "office";
    else if (nsURI == KoXmlNS::meta)
        return "meta";
    else if (nsURI == KoXmlNS::config)
        return "config";
    else if (nsURI == KoXmlNS::text)
        return "text";
    else if (nsURI == KoXmlNS::table)
        return "table";
    else if (nsURI == KoXmlNS::draw)
        return "draw";
    else if (nsURI == KoXmlNS::presentation)
        return "presentation";
    else if (nsURI == KoXmlNS::dr3d)
        return "dr3d";
    else if (nsURI == KoXmlNS::chart)
        return "chart";
    else if (nsURI == KoXmlNS::form)
        return "form";
    else if (nsURI == KoXmlNS::script)
        return "script";
    else if (nsURI == KoXmlNS::style)
        return "style";
    else if (nsURI == KoXmlNS::number)
        return "number";
    else if (nsURI == KoXmlNS::manifest)
        return "manifest";
    else if (nsURI == KoXmlNS::anim)
        return "anim";
    else if (nsURI == KoXmlNS::math)
        return "math";
    else if (nsURI == KoXmlNS::svg)
        return "svg";
    else if (nsURI == KoXmlNS::fo)
        return "fo";
    else if (nsURI == KoXmlNS::dc)
        return "dc";
    else if (nsURI == KoXmlNS::xlink)
        return "xlink";
    else if (nsURI == KoXmlNS::VL)
        return "VL";
    else if (nsURI == KoXmlNS::smil)
        return "smil";
    else if (nsURI == KoXmlNS::xhtml)
        return "xhtml";
    else if (nsURI == KoXmlNS::calligra)
        return "calligra";
    else if (nsURI == KoXmlNS::officeooo)
        return "officeooo";
    else if (nsURI == KoXmlNS::xml)
        return "xml";

    // Shouldn't happen.
    return "";
}

