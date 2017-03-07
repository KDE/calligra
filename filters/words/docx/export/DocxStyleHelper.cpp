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

#include "DocxExportDebug.h"
#include "UnitConversions.h"

// Calligra
#include <KoXmlWriter.h>
#include <KoOdfStyleProperties.h>
#include <KoOdfStyleManager.h>
#include <KoOdfStyle.h>


const qreal DefaultFontSize = 12.0;


// ----------------------------------------------------------------
//                     class DocxStyleHelper

void DocxStyleHelper::inheritTextStyles(KoOdfStyleProperties *destinationProperties,
                                        const QString &parent, KoOdfStyleManager *manager)
{
    // Inherits text styles from paragraphs
    KoOdfStyle *style = manager->style(parent, "paragraph");
    QString ancestor = style->parent();
    if (!ancestor.isEmpty()) {
        inheritTextStyles(destinationProperties, ancestor, manager);
    }
    KoOdfStyleProperties *properties = style->properties("style:text-properties");
    if (properties) {
        destinationProperties->copyPropertiesFrom(*properties);
    }
}


static qreal getHalfPoints(const QString &fontSize, qreal defaultSize)
{
    QString unit = fontSize.right(2);
    qreal sizeInHalfPoints = -1.;
    bool ok1 = true;
    bool ok2 = true;
    if (unit == "pt") {
        sizeInHalfPoints = ptToHalfPt(fontSize.left(fontSize.length() - 2).toDouble(&ok1));
    }
    else if (unit == "in") {
        sizeInHalfPoints = inToHalfPt(fontSize.left(fontSize.length() - 2).toDouble(&ok2));
    }
    else {
        // Other units not implemented yet?
        warnDocx << "Unit not implemented yet:" << unit;
        ok1 = false;
    }

    if (!ok1 || !ok2 || sizeInHalfPoints == -1.0) {
        warnDocx << "Error in fontsize";
        sizeInHalfPoints = defaultSize; // Use as default
    }
    return sizeInHalfPoints;
}

void DocxStyleHelper::handleTextStyles(KoOdfStyleProperties *properties, KoXmlWriter *writer)
{
    if (!properties) {
        return;
    }

    QString fontSize = properties->attribute("fo:font-size");
    if (!fontSize.isEmpty()) {
        qreal sizeInHalfPoints = getHalfPoints(fontSize, DefaultFontSize * 2);
        if (sizeInHalfPoints > 0) {
            writer->startElement("w:sz");
            writer->addAttribute("w:val", sizeInHalfPoints);
            writer->endElement(); // w:sz
        }
    }
    QString fontSizeC = properties->attribute("fo:font-size-complex");
    if (!fontSizeC.isEmpty()) {
        qreal sizeInHalfPoints = getHalfPoints(fontSize, DefaultFontSize * 2);
        if (sizeInHalfPoints > 0) {
            writer->startElement("w:szCs");
            writer->addAttribute("w:val", sizeInHalfPoints);
            writer->endElement(); // w:szCs
        }
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

void DocxStyleHelper::handleParagraphStyles(KoOdfStyleProperties *properties, KoXmlWriter *writer)
{
    if (!properties) {
        return;
    }

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

