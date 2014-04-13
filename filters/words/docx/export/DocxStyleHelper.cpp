/* This file is part of the KDE project

   Copyright (C) 2014 Lassi Nieminen <lassniem@gmail.com>

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
   Boston, MA 02110-1301, USA.
*/

// Own
#include "DocxStyleHelper.h"

// Calligra
#include <KoXmlWriter.h>
#include <KoOdfStyleProperties.h>

// ----------------------------------------------------------------
//                     class DocxStyleHelper

void DocxStyleHelper::handleTextStyles(KoOdfStyleProperties *properties, KoXmlWriter *writer)
{
    if (properties != 0) {
        QString fontWeight = properties->attribute("fo:font-weight");
        if (fontWeight == "bold") {
            writer->startElement("w:b");
            writer->addAttribute("w:val", "1");
            writer->endElement(); // w:b
        }
        QString fontWeightC = properties->attribute("style:font-weight-complex");
        if (fontWeightC == "bold") {
            writer->startElement("w:bCs");
            writer->addAttribute("w:val", "1");
            writer->endElement(); // w:bCs
        }
        QString fontStyle = properties->attribute("fo:font-style");
        if (fontStyle == "italic") {
           writer->startElement("w:i");
           writer->addAttribute("w:val", "1");
           writer->endElement(); // w:i
        }
        QString fontStyleC = properties->attribute("style:font-style-complex");
        if (fontStyleC == "italic") {
           writer->startElement("w:iCs");
           writer->addAttribute("w:val", "1");
           writer->endElement(); // w:iCs
        }
        QString underlineStyle = properties->attribute("style:text-underline-style");
        if (!underlineStyle.isEmpty()) {
            if (underlineStyle == "solid") {
                writer->startElement("w:u");
                writer->addAttribute("w:val", "single");
                writer->endElement(); //:u
            }
        }
    }

}

void DocxStyleHelper::handleParagraphStyles(KoOdfStyleProperties *properties, KoXmlWriter *writer)
{
    if (properties != 0) {
    }
}

