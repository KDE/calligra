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

#include "UnitConversions.h"

// Calligra
#include <KoXmlWriter.h>
#include <KoOdfStyleProperties.h>

// ----------------------------------------------------------------
//                     class DocxStyleHelper

void DocxStyleHelper::handleTextStyles(KoOdfStyleProperties *properties, KoXmlWriter *writer)
{
    if (properties != 0) {
        QString fontSize = properties->attribute("fo:font-size");
        if (!fontSize.isEmpty()) {
            writer->startElement("w:sz");
            int sizeInHalfPoints = 0;
            QString unit = fontSize.right(2);
            if (unit == "pt") {
                sizeInHalfPoints = ptToHalfPt(fontSize.left(fontSize.length() - 2).toDouble());
            }
            else if (unit == "in") {
                sizeInHalfPoints = inToHalfPt(fontSize.left(fontSize.length() - 2).toDouble());
            }
            if (sizeInHalfPoints > 0) {
                writer->addAttribute("w:val", sizeInHalfPoints);
            }
            writer->endElement(); // w:sz
        }
        QString fontSizeC = properties->attribute("fo:font-size-complex");
        if (!fontSizeC.isEmpty()) {
            writer->startElement("w:szCs");
            int sizeInHalfPoints = 0;
            QString unit = fontSize.right(2);
            if (unit == "pt") {
                sizeInHalfPoints = ptToHalfPt(fontSizeC.left(fontSizeC.length() - 2).toDouble());
            }
            else if (unit == "in") {
                sizeInHalfPoints = inToHalfPt(fontSizeC.left(fontSizeC.length() - 2).toDouble());
            }
            if (sizeInHalfPoints > 0) {
                writer->addAttribute("w:val", sizeInHalfPoints);
            }
            writer->endElement(); // w:szCs
        }
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
        QString fontName = properties->attribute("style:font-name");
        if (!fontName.isEmpty()) {
            // todo
        }
        QString textPosition = properties->attribute("style:text-position");
        if (!textPosition.isEmpty()) {
           writer->startElement("w:vertAlign");
           if (textPosition == "super") {
               writer->addAttribute("w:val", "superscript");
           }
           else if (textPosition == "sub") {
               writer->addAttribute("w:val", "subscript");
           }
           writer->endElement(); // w:vertAlign
        }
        QString textColor = properties->attribute("fo:color");
        if (!textColor.isEmpty()) {
            writer->startElement("w:color");
            writer->addAttribute("w:val", textColor.mid(1));
            writer->endElement(); // w:color
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
        QString tabStop = properties->attribute("style:tab-stop-distance");
        if (!tabStop.isEmpty()) {
            // todo
        }
        QString lineHeight = properties->attribute("fo:line-height");
        if (!lineHeight.isEmpty()) {
            writer->startElement("w:spacing");
            writer->addAttribute("w:lineRule", "auto");
            int percentage = lineHeight.left(lineHeight.length() - 1).toDouble() * 2.4;
            writer->addAttribute("w:line", percentage);
            writer->endElement(); // w:spacing
        }
        QString textAlign = properties->attribute("fo:text-align");
        if (!textAlign.isEmpty()) {
            writer->startElement("w:jc");
            if (textAlign == "center") {
                writer->addAttribute("w:val", "center");
            }
            else if (textAlign == "start") {
                writer->addAttribute("w:val", "left");
            }
            else if (textAlign == "right") {
                writer->addAttribute("w:val", "right");
            }
            else if (textAlign == "justify") {
                writer->addAttribute("w:val", "both");
            }
            writer->endElement(); // w:jc
        }
    }
}

