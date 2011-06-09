/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef MSOOXMLVMLREADER_IMPL_H
#define MSOOXMLVMLREADER_IMPL_H

#include <math.h>

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "v"

//! Used by read_rect() to parse CSS2.
//! See ECMA-376 Part 4, 14.1.2.16, p.465.
//! @todo reuse KHTML parser?
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::parseCSS(const QString& style)
{
    m_currentVMLProperties.vmlStyle.clear();
    foreach( const QString& pair, style.split(";", QString::SkipEmptyParts)) {
        const int splitIndex = pair.indexOf(":");
        if (splitIndex < 1) {
            continue;
        }
        const QByteArray name(pair.left(splitIndex).toLatin1().trimmed());
        QString value(pair.mid(splitIndex+1).trimmed());
        if (name.isEmpty()) {
            continue;
        }
        if (value.startsWith("'") && value.endsWith("'")) {
            value = value.mid(1, value.length() - 2); // strip ' '
        }
        m_currentVMLProperties.vmlStyle.insert(name, value);
        kDebug() << "name:" << name << "value:" << value;
    }
    return KoFilter::OK;
}

void MSOOXML_CURRENT_CLASS::createFrameStart(FrameStartElement startType)
{
    // Todo handle here all possible shape types
    if (startType == RectStart) {
        body->startElement("draw:rect");
    }
    // Simplifying connector to be a line
    else if (startType == StraightConnectorStart) {
        body->startElement("draw:line");
    }
    else if (startType == GroupStart) {
        body->startElement("draw:g");
    }
    else if (startType == CustomStart) {
        body->startElement("draw:custom-shape");
    }
    else {
        body->startElement("draw:frame");
    }

    QString width(m_currentVMLProperties.vmlStyle.value("width")); // already in "...cm" format
    QString height(m_currentVMLProperties.vmlStyle.value("height")); // already in "...cm" format
    QString x_mar(m_currentVMLProperties.vmlStyle.value("margin-left"));
    QString y_mar(m_currentVMLProperties.vmlStyle.value("margin-top"));
    QString leftPos(m_currentVMLProperties.vmlStyle.value("left"));
    QString topPos(m_currentVMLProperties.vmlStyle.value("top"));
    QString position(m_currentVMLProperties.vmlStyle.value("position"));
    QString hor_pos(m_currentVMLProperties.vmlStyle.value("mso-position-horizontal"));
    QString ver_pos(m_currentVMLProperties.vmlStyle.value("mso-position-vertical"));
    QString hor_pos_rel(m_currentVMLProperties.vmlStyle.value("mso-position-horizontal-relative"));
    QString ver_pos_rel(m_currentVMLProperties.vmlStyle.value("mso-position-vertical-relative"));
    const QString ver_align(m_currentVMLProperties.vmlStyle.value("v-text-anchor"));
    const QString rotation(m_currentVMLProperties.vmlStyle.value("rotation"));
    const QString z_index(m_currentVMLProperties.vmlStyle.value("z-index"));

    qreal x_position = 0;
    QString x_pos_string, y_pos_string, widthString, heightString;
    qreal y_position = 0;
    qreal widthValue = 0;
    qreal heightValue = 0;

    if (!x_mar.isEmpty()) {
        if (m_currentVMLProperties.insideGroup) {
            x_position = (x_mar.toInt() - m_currentVMLProperties.groupX) * m_currentVMLProperties.real_groupWidth /
                m_currentVMLProperties.groupWidth + m_currentVMLProperties.groupXOffset;
            x_pos_string = QString("%1%2").arg(x_position).arg(m_currentVMLProperties.groupWidthUnit);
        } else {
            x_position = x_mar.left(x_mar.length() - 2).toDouble(); // removing the unit
            x_pos_string = x_mar;
        }
    }
    else if (!leftPos.isEmpty()) {
        if (m_currentVMLProperties.insideGroup) {
            x_position = (leftPos.toInt() - m_currentVMLProperties.groupX) * m_currentVMLProperties.real_groupWidth /
                m_currentVMLProperties.groupWidth + m_currentVMLProperties.groupXOffset;
            x_pos_string = QString("%1%2").arg(x_position).arg(m_currentVMLProperties.groupWidthUnit);
        } else {
            x_position = leftPos.left(leftPos.length() - 2).toDouble();
            x_pos_string = leftPos;
        }
    }
    else {
        if (m_currentVMLProperties.insideGroup) {
            x_pos_string = QString("%1%2").arg(m_currentVMLProperties.groupXOffset).arg(m_currentVMLProperties.groupWidthUnit);
        }
    }
    if (!y_mar.isEmpty()) {
        if (m_currentVMLProperties.insideGroup) {
            y_position = (y_mar.toInt() - m_currentVMLProperties.groupY) * m_currentVMLProperties.real_groupHeight /
                m_currentVMLProperties.groupHeight + m_currentVMLProperties.groupYOffset;
            y_pos_string = QString("%1%2").arg(y_position).arg(m_currentVMLProperties.groupHeightUnit);
        } else {
            y_position = y_mar.left(y_mar.length() -2).toDouble();
            y_pos_string = y_mar;
        }
    }
    else if (!topPos.isEmpty()) {
        if (m_currentVMLProperties.insideGroup) {
            y_position = (topPos.toInt() - m_currentVMLProperties.groupY) * m_currentVMLProperties.real_groupHeight /
                m_currentVMLProperties.groupHeight + m_currentVMLProperties.groupYOffset;
            y_pos_string = QString("%1%2").arg(y_position).arg(m_currentVMLProperties.groupHeightUnit);
        } else {
            y_position = topPos.left(topPos.length() - 2).toDouble();
            y_pos_string = topPos;
        }
    }
    else {
        if (m_currentVMLProperties.insideGroup) {
            y_pos_string = QString("%1%2").arg(m_currentVMLProperties.groupYOffset).arg(m_currentVMLProperties.groupHeightUnit);
        }
    }
    if (!width.isEmpty()) {
        if (m_currentVMLProperties.insideGroup) {
            widthValue = width.toInt() * m_currentVMLProperties.real_groupWidth / m_currentVMLProperties.groupWidth;
            widthString = QString("%1%2").arg(widthValue).arg(m_currentVMLProperties.groupWidthUnit);
        } else {
            widthValue = width.left(width.length() - 2).toDouble();
            widthString = width;
        }
    }
    if (!height.isEmpty()) {
        if (m_currentVMLProperties.insideGroup) {
            heightValue = height.toInt() * m_currentVMLProperties.real_groupHeight / m_currentVMLProperties.groupHeight;
            heightString = QString("%1%2").arg(heightValue).arg(m_currentVMLProperties.groupHeightUnit);
        }
        else {
            heightValue = height.left(height.length() - 2).toDouble();
            heightString = height;
        }
    }

    if (startType == StraightConnectorStart) {
        QString flip(m_currentVMLProperties.vmlStyle.value("flip"));
        QString y1 = y_pos_string;
        QString x2 = QString("%1%2").arg(x_position + widthValue).arg(widthString.right(2)); // right(2) takes the unit
        QString x1 = x_pos_string;
        QString y2 = QString("%1%2").arg(y_position + heightValue).arg(heightString.right(2));
        if (flip == "x") {
            QString temp = y2;
            y2 = y1;
            y1 = temp;
        }
        if (flip == "y") {
            QString temp = x2;
            x2 = x1;
            x1 = temp;
        }
        body->addAttribute("svg:x1", x1);
        body->addAttribute("svg:y1", y1);
        body->addAttribute("svg:x2", x2);
        body->addAttribute("svg:y2", y2);
    }
    else {
        if (startType != GroupStart) {
            if (!x_pos_string.isEmpty()) {
                body->addAttribute("svg:x", x_pos_string);
            }
            if (!y_pos_string.isEmpty()) {
                body->addAttribute("svg:y", y_pos_string);
            }
            body->addAttribute("svg:width", widthString);
            body->addAttribute("svg:height", heightString);
        }
    }

    if (!z_index.isEmpty()) {
        body->addAttribute("draw:z-index", z_index);
    }

    bool asChar = false;
    if (position.isEmpty() || position == "static") {
        asChar = true;
    }

    if (!hor_pos_rel.isEmpty()) {
        if (hor_pos_rel == "outer-margin-area" || hor_pos_rel == "left-margin-area") {
            hor_pos_rel = "page-start-margin";
            m_currentVMLProperties.anchorType = "paragraph"; //forced
        }
        else if (hor_pos_rel == "margin") {
            hor_pos_rel = "page-content";
            m_currentVMLProperties.anchorType = "paragraph"; //forced
        }
        else if (hor_pos_rel == "inner-margin-area" || hor_pos_rel == "right-margin-area") {
            hor_pos_rel = "page-end-margin";
            m_currentVMLProperties.anchorType = "paragraph"; //forced
        }
        if (!asChar) {
            m_currentDrawStyle->addProperty("style:horizontal-rel", hor_pos_rel);
        }
    }
    if (!ver_pos_rel.isEmpty()) {
        if (ver_pos_rel == "margin" || ver_pos_rel == "line") {
            m_currentDrawStyle->addProperty("style:vertical-rel", "page-content");
        }
        else if (ver_pos_rel == "top-margin-area" || ver_pos_rel == "inner-margin-area" || ver_pos_rel == "outer-margin-area") {
            m_currentDrawStyle->addProperty("style:vertical-rel", "page");
        }
        else if (ver_pos_rel == "bottom-margin-area") {
            m_currentDrawStyle->addProperty("style:vertical-rel", "page");
            // This effectively emulates the bottom-margin-area
            m_currentDrawStyle->addProperty("style:vertical-pos", "bottom");
        }
        else {
            m_currentDrawStyle->addProperty("style:vertical-rel", ver_pos_rel);
        }
    }

#ifdef DOCXXMLDOCREADER_H
    if (!m_currentVMLProperties.wrapRead) {
        m_currentDrawStyle->addProperty("style:wrap", "none");
        if (asChar) { // Default
            body->addAttribute("text:anchor-type", "as-char");
            if (ver_pos_rel == "line") {
                m_currentDrawStyle->addProperty("style:vertical-rel", "text");
            }
        }
        else {
            body->addAttribute("text:anchor-type", "char");
            if (m_currentDrawStyle->property("style:vertical-rel").isEmpty()) {
                m_currentDrawStyle->addProperty("style:vertical-rel", "paragraph");
            }
            if (m_currentDrawStyle->property("style:horizontal-rel").isEmpty()) {
                m_currentDrawStyle->addProperty("style:horizontal-rel", "paragraph");
            }

            if (m_currentVMLProperties.vmlStyle.contains("z-index")) {
                m_currentDrawStyle->addProperty("style:wrap", "run-through");
                if (m_currentVMLProperties.vmlStyle.value("z-index").toInt() > 0) {
                    m_currentDrawStyle->addProperty("style:run-through", "foreground");
                }
                else {
                    m_currentDrawStyle->addProperty("style:run-through", "background");
                }
            }
            else {
                m_currentDrawStyle->addProperty("style:wrap", "run-through");
                m_currentDrawStyle->addProperty("style:run-through", "foreground");
            }
        }
    }
    else {
        body->addAttribute("text:anchor-type", m_currentVMLProperties.anchorType);
    }
    if (!asChar) {
        if (hor_pos.isEmpty() || hor_pos == "absolute") {
            hor_pos = "from-left";
        }
        m_currentDrawStyle->addProperty("style:horizontal-pos", hor_pos);
    }
    if (ver_pos.isEmpty() || ver_pos == "absolute") {
        if (asChar) {
            m_currentDrawStyle->addProperty("style:vertical-pos", "bottom");
        }
        else {
            m_currentDrawStyle->addProperty("style:vertical-pos", "from-top");
        }
    }
    else {
        if (ver_pos == "center") {
            ver_pos = "middle";
        }
        m_currentDrawStyle->addProperty("style:vertical-pos", ver_pos);
    }
#endif

    m_currentDrawStyle->addProperty("svg:stroke-width", m_currentVMLProperties.strokeWidth);
    m_currentDrawStyle->addProperty("svg:stroke-color", m_currentVMLProperties.strokeColor);
    m_currentDrawStyle->addProperty("svg:stroke-linecap", m_currentVMLProperties.lineCapStyle);
    m_currentDrawStyle->addProperty("draw:stroke-linejoin", m_currentVMLProperties.joinStyle);
    if (m_currentVMLProperties.stroked) {
        if (m_currentVMLProperties.strokeStyleName.isEmpty()) {
            m_currentDrawStyle->addProperty("draw:stroke", "solid");
        }
        else {
            m_currentDrawStyle->addProperty("draw:stroke", "dash");
            m_currentDrawStyle->addProperty("draw:stroke-dash", m_currentVMLProperties.strokeStyleName);
        }
    }
    else {
        m_currentDrawStyle->addProperty("draw:stroke", "none");
    }

    m_currentDrawStyle->addProperty("draw:fill-color", m_currentVMLProperties.shapeColor);
    if (m_currentVMLProperties.filled) {
        m_currentDrawStyle->addProperty("draw:fill", "solid");
    }
    else {
        m_currentDrawStyle->addProperty("draw:fill", "none");
    }

    if (!ver_align.isEmpty()) {
        m_currentDrawStyle->addProperty("draw:textarea-vertical-align", ver_align);
    }

    if (m_currentVMLProperties.shadowed == true) {
        m_currentDrawStyle->addProperty("draw:shadow", "visible");
    }
    else {
        m_currentDrawStyle->addProperty("draw:shadow", "hidden");
    }
    m_currentDrawStyle->addProperty("draw:shadow-color", m_currentVMLProperties.shadowColor);
    m_currentDrawStyle->addProperty("draw:shadow-offset-x", m_currentVMLProperties.shadowXOffset);
    m_currentDrawStyle->addProperty("draw:shadow-offset-y", m_currentVMLProperties.shadowYOffset);
    if (m_currentVMLProperties.shadowOpacity > 0) {
        m_currentDrawStyle->addProperty("draw:shadow-opacity", QString("%1%").
            arg(m_currentVMLProperties.shadowOpacity));
    }

    if (m_currentVMLProperties.opacity > 0) {
        m_currentDrawStyle->addProperty("draw:opacity", QString("%1%").arg(m_currentVMLProperties.opacity));
    }

    if (!m_currentDrawStyle->isEmpty()) {
        const QString drawStyleName(mainStyles->insert(*m_currentDrawStyle, "gr"));
        body->addAttribute("draw:style-name", drawStyleName);
    }
}

KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::createFrameEnd()
{
    if (!m_currentVMLProperties.imagedataPath.isEmpty()) {
        body->startElement("draw:image");
        body->addAttribute("xlink:type", "simple");
        body->addAttribute("xlink:show", "embed");
        body->addAttribute("xlink:actuate", "onLoad");
        body->addAttribute("xlink:href", m_currentVMLProperties.imagedataPath);
        body->endElement(); // draw:image
    }

    {
        {
            const QString rId(m_currentVMLProperties.vmlStyle.value("v:fill@r:id"));
            if (!rId.isEmpty()) {
                body->startElement("draw:image");
                const QString sourceName(m_context->relationships->target(m_context->path, m_context->file, rId));
                kDebug() << "sourceName:" << sourceName;
                if (sourceName.isEmpty()) {
                    return KoFilter::FileNotFound;
                }

                QString destinationName = QLatin1String("Pictures/") + sourceName.mid(sourceName.lastIndexOf('/') + 1);;
                RETURN_IF_ERROR(m_context->import->copyFile(sourceName, destinationName, false ))
                addManifestEntryForFile(destinationName);
                addManifestEntryForPicturesDir();
                body->addAttribute("xlink:href", destinationName);
                body->endElement(); //draw:image
            }
        }
    }

    body->endElement(); //draw:frame or draw:rect

    return KoFilter::OK;
}

void MSOOXML_CURRENT_CLASS::takeDefaultValues()
{
    m_currentVMLProperties.strokeColor = "#000000"; // default
    m_currentVMLProperties.strokeWidth = "1pt" ; // default
    m_currentVMLProperties.shapeColor = "#ffffff"; //default
    m_currentVMLProperties.shapeSecondaryColor = "#ffffff"; //default
    m_currentVMLProperties.lineCapStyle = "square";
    m_currentVMLProperties.joinStyle = "middle";
    m_currentVMLProperties.strokeStyleName = QString();
    m_currentVMLProperties.filled = true; // default
    m_currentVMLProperties.stroked = true; // default
    m_currentVMLProperties.opacity = 0; // default
    m_currentVMLProperties.shadowed = false;
    m_currentVMLProperties.shadowOpacity = 0; // default
    m_currentVMLProperties.shadowColor = "#101010"; // default
    m_currentVMLProperties.shadowXOffset = "2pt"; // default
    m_currentVMLProperties.shadowYOffset = "2pt"; //default
}

void MSOOXML_CURRENT_CLASS::handleStrokeAndFill(const QXmlStreamAttributes& attrs)
{
    TRY_READ_ATTR_WITHOUT_NS(strokeweight)
    if (!strokeweight.isEmpty()) {
        if (strokeweight.at(0) == '.') {
            m_currentVMLProperties.strokeWidth = "0" + strokeweight;
        }
    }

    TRY_READ_ATTR_WITHOUT_NS(type)
    if (!type.isEmpty()) {
        type = type.mid(1); // removes extra # from the start
    }
    TRY_READ_ATTR_WITHOUT_NS(filled)
    if (!filled.isEmpty()) {
        if (filled == "f" || filled == "false") {
            m_currentVMLProperties.filled = false;
        }
        else {
            m_currentVMLProperties.filled = true;
        }
    }

    TRY_READ_ATTR_WITHOUT_NS(fillcolor)
    if (!fillcolor.isEmpty()) {
        m_currentVMLProperties.shapeColor = MSOOXML::Utils::rgbColor(fillcolor);
    }

    TRY_READ_ATTR_WITHOUT_NS(stroked)
    if (!stroked.isEmpty()) {
        if (stroked == "f" || stroked == "false") {
            m_currentVMLProperties.stroked = false;
        }
        else {
            m_currentVMLProperties.stroked = true;
        }
    }

    TRY_READ_ATTR_WITHOUT_NS(strokecolor)
    if (!strokecolor.isEmpty()) {
        m_currentVMLProperties.strokeColor = MSOOXML::Utils::rgbColor(strokecolor);
    }

    TRY_READ_ATTR_WITHOUT_NS(opacity)
    if (!opacity.isEmpty()) {
        if (opacity.right(1) == "f") {
            opacity = opacity.left(opacity.length()-1);
            m_currentVMLProperties.opacity = 100 * opacity.toInt() / 65536;
        }
        else {
            if (opacity.left(1) == ".") {
                opacity = "0" + opacity;
            }
            m_currentVMLProperties.opacity = 100 * opacity.toDouble();
        }
    }
}

#undef CURRENT_EL
#define CURRENT_EL rect
//! rect handler (Rectangle)
/*! ECMA-376 Part 4, 14.1.2.16, p.449.
 This element is used to draw a simple rectangle.
 The CSS2 style content width and height define the width and height of the rectangle.

 Parent elements:
 - background (Part 1, §17.2.1)
 - group (§14.1.2.7)
 - object (Part 1, §17.3.3.19)
 - [done] pict (§9.2.2.2)
 - pict (§9.5.1)

 Child elements:
 - anchorlock (Anchor Location Is Locked) §14.3.2.1
 - borderbottom (Bottom Border) §14.3.2.2
 - borderleft (Left Border) §14.3.2.3
 - borderright (Right Border) §14.3.2.4
 - bordertop (Top Border) §14.3.2.5
 - callout (Callout) §14.2.2.2
 - ClientData (Attached Object Data) §14.4.2.12
 - clippath (Shape Clipping Path) §14.2.2.3
 - extrusion (3D Extrusion) §14.2.2.11
 - [done] fill (Shape Fill Properties) §14.1.2.5
 - formulas (Set of Formulas) §14.1.2.6
 - handles (Set of Handles) §14.1.2.9
 - imagedata (Image Data) §14.1.2.11
 - lock (Shape Protections) §14.2.2.18
 - path (Shape Path) §14.1.2.14
 - [done] shadow (Shadow Effect) §14.1.2.18
 - signatureline (Digital Signature Line) §14.2.2.30
 - skew (Skew Transform) §14.2.2.31
 - [done] stroke (Line Stroke Settings) §14.1.2.21
 - [done] textbox (Text Box) §14.1.2.22
 - textdata (VML Diagram Text) §14.5.2.2
 - textpath (Text Layout Path) §14.1.2.23
 - [done] wrap (Text Wrapping) §14.3.2.6
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_rect()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(style)
    RETURN_IF_ERROR(parseCSS(style))

    takeDefaultValues();
    handleStrokeAndFill(attrs);

    MSOOXML::Utils::XmlWriteBuffer frameBuf;
    body = frameBuf.setWriter(body);

    pushCurrentDrawStyle(new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic"));
    if (m_moveToStylesXml) {
        m_currentDrawStyle->setAutoStyleInStylesDotXml(true);
    }

    m_currentVMLProperties.wrapRead = false;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(fill)
            ELSE_TRY_READ_IF(textbox)
            ELSE_TRY_READ_IF(stroke)
            ELSE_TRY_READ_IF(shadow)
            else if (qualifiedName() == "w10:wrap") {
                m_currentVMLProperties.wrapRead = true;
                TRY_READ(wrap)
            }
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    body = frameBuf.originalWriter();

    createFrameStart(RectStart);

    (void)frameBuf.releaseWriter();

    createFrameEnd();

    popCurrentDrawStyle();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL shadow
//! Shadow handler
/*
 Parent elements:
 - arc (§14.1.2.1);
 - background (Part 1, §17.2.1);
 - curve (§14.1.2.3);
 - [done] group (§14.1.2.7);
 - image (§14.1.2.10);
 - line (§14.1.2.12);
 - object (Part 1, §17.3.3.19);
 - [done] oval (§14.1.2.13);
 - pict (§9.2.2.2);
 - pict (§9.5.1);
 - polyline (§14.1.2.15);
 - [done] rect (§14.1.2.16);
 - [done] roundrect (§14.1.2.17);
 - [done] shape (§14.1.2.19);
 - shapedefaults (§14.2.2.28);
 - [done] shapetype (§14.1.2.20)

 Child elements:
 - none
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_shadow()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    m_currentVMLProperties.shadowed = true; // default in this function

    TRY_READ_ATTR_WITHOUT_NS(on)
    if (on == "f" || on == "false") {
        m_currentVMLProperties.shadowed = false;
    }

    TRY_READ_ATTR_WITHOUT_NS(color)
    if (!color.isEmpty()) {
        m_currentVMLProperties.shadowColor = MSOOXML::Utils::rgbColor(color);
    }

    TRY_READ_ATTR_WITHOUT_NS(offset)
    int index = offset.indexOf(',');
    if (index > 0) {
        m_currentVMLProperties.shadowXOffset = offset.left(index);
        m_currentVMLProperties.shadowYOffset = offset.mid(index + 1);
    }

    TRY_READ_ATTR_WITHOUT_NS(opacity)
    if (!opacity.isEmpty()) {
        if (opacity.right(1) == "f") {
            opacity = opacity.left(opacity.length()-1);
            m_currentVMLProperties.shadowOpacity = 100 * opacity.toInt() / 65536;
        }
        else {
            if (opacity.left(1) == ".") {
                opacity = "0" + opacity;
            }
            m_currentVMLProperties.shadowOpacity = 100 * opacity.toDouble();
        }
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL stroke
//! Stroke style handler
/*
 Parent elements:
 - arc (§14.1.2.1);
 - background (Part 1, §17.2.1);
 - curve (§14.1.2.3);
 - [done] group (§14.1.2.7);
 - image (§14.1.2.10);
 - line (§14.1.2.12);
 - object (Part 1, §17.3.3.19);
 - [done] oval (§14.1.2.13);
 - pict (§9.2.2.2);
 - pict (§9.5.1);
 - polyline (§14.1.2.15);
 - [done] rect (§14.1.2.16);
 - [done] roundrect (§14.1.2.17);
 - [done] shape (§14.1.2.19);
 - shapedefaults (§14.2.2.28);
 - [done] shapetype (§14.1.2.20)

 Child elements:
 - bottom (Text Box Bottom Stroke) §14.2.2.1
 - column (Text Box Interior Stroke) §14.2.2.6
 - left (Text Box Left Stroke) §14.2.2.16
 - right (Text Box Right Stroke) §14.2.2.26
 - top (Text Box Top Stroke) §14.2.2.32
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_stroke()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    // Spec says that this should overwrite the shape's value, but apparently
    // a non specified value does not overwrite it
    //m_currentVMLProperties.stroked = true; // default in this function

    TRY_READ_ATTR_WITHOUT_NS(on)
    if (on == "f" || on == "false") {
        m_currentVMLProperties.stroked = false;
    }

    TRY_READ_ATTR_WITHOUT_NS(endcap)
    if (endcap.isEmpty() || endcap == "sq") {
       m_currentVMLProperties.lineCapStyle = "square";
    }
    else if (endcap == "round") {
         m_currentVMLProperties.lineCapStyle = "round";
    }
    else if (endcap == "flat") {
        m_currentVMLProperties.lineCapStyle = "flat";
    }

    TRY_READ_ATTR_WITHOUT_NS(joinstyle)
    if (!joinstyle.isEmpty()) {
        m_currentVMLProperties.joinStyle = joinstyle;
    }

    TRY_READ_ATTR_WITHOUT_NS(dashstyle)
    if (!dashstyle.isEmpty()) {
        // This is over simplification
        QPen pen;
        pen.setWidthF(2);
        pen.setStyle(Qt::DashLine);
        m_currentDrawStyle->addProperty("draw:stroke", "dash");
        KoGenStyle dashStyle(KoGenStyle::StrokeDashStyle);
        dashStyle.addAttribute("draw:style", "rect");
        QVector<qreal> dashes = pen.dashPattern();
        dashStyle.addAttribute("draw:dots1", static_cast<int>(1));
        dashStyle.addAttributePt("draw:dots1-length", dashes[0]*pen.widthF());
        dashStyle.addAttributePt("draw:distance", dashes[1]*pen.widthF());
        if (dashes.size() > 2) {
            dashStyle.addAttribute("draw:dots2", static_cast<int>(1));
            dashStyle.addAttributePt("draw:dots2-length", dashes[2]*pen.widthF());
        }
        m_currentVMLProperties.strokeStyleName = mainStyles->insert(dashStyle, "dash");
        /* TODO : implement in reality
        if (dashStyle == "dashDot") {

        }
        else if (dashStyle == "longDash") {

        }
        else if (dashStyle == "1 1") {

        }
        else if (dashStyle == "3 1") {

        }
        else if (dashStyle == "dash") {

        }*/
    // TODO
    }

    while (!atEnd()) {
        BREAK_IF_END_OF(CURRENT_EL)
        readNext();
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL group
//! Vml group handler
/*
 Parent elements:
 - background (Part 1, §17.2.1);
 - group (§14.1.2.7);
 - object (Part 1, §17.3.3.19);
 - [done] pict (§9.2.2.2);
 - [done] pict (§9.5.1)

 Child elements:
 - anchorlock (Anchor Location Is Locked) §14.3.2.1
 - arc (Arc Segment) §14.1.2.1
 - borderbottom (Bottom Border) §14.3.2.2
 - borderleft (Left Border) §14.3.2.3
 - borderright (Right Border) §14.3.2.4
 - bordertop (Top Border) §14.3.2.5
 - callout (Callout) §14.2.2.2
 - ClientData (Attached Object Data) §14.4.2.12
 - clippath (Shape Clipping Path) §14.2.2.3
 - curve (Bezier Curve) §14.1.2.3
 - diagram (VML Diagram) §14.2.2.8
 - extrusion (3D Extrusion) §14.2.2.11
 - [done] fill (Shape Fill Properties) §14.1.2.5
 - formulas (Set of Formulas) §14.1.2.6
 - [done] group (Shape Group) §14.1.2.7
 - handles (Set of Handles) §14.1.2.9
 - image (Image File) §14.1.2.10
 - imagedata (Image Data) §14.1.2.11
 - line (Line) §14.1.2.12
 - lock (Shape Protections) §14.2.2.18
 - [done] oval (Oval) §14.1.2.13
 - path (Shape Path) §14.1.2.14
 - polyline (Multiple Path Line) §14.1.2.15
 - [done] rect (Rectangle) §14.1.2.16
 - [done] roundrect (Rounded Rectangle) §14.1.2.17
 - [done] shadow (Shadow Effect) §14.1.2.18
 - [done] shape (Shape Definition) §14.1.2.19
 - [done] shapetype (Shape Template) §14.1.2.20
 - signatureline (Digital Signature Line) §14.2.2.30
 - skew (Skew Transform) §14.2.2.31
 - [done] stroke (Line Stroke Settings) §14.1.2.21
 - textbox (Text Box) §14.1.2.22
 - textdata (VML Diagram Text) §14.5.2.2
 - textpath (Text Layout Path) §14.1.2.23
 - [done] wrap (Text Wrapping) §14.3.2.6
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_group()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(style)
    RETURN_IF_ERROR(parseCSS(style))

    pushCurrentDrawStyle(new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic"));
    if (m_moveToStylesXml) {
        m_currentDrawStyle->setAutoStyleInStylesDotXml(true);
    }

    if (!m_currentVMLProperties.insideGroup) {

        const QString width(m_currentVMLProperties.vmlStyle.value("width"));
        const QString height(m_currentVMLProperties.vmlStyle.value("height"));

        // These potentially cause an offset to all shapes in the group
        // Unhandled case: theoretically x_mar could be in different units
        // than width, in this case they should be added somehow intelligently
        const QString x_mar(m_currentVMLProperties.vmlStyle.value("margin-left"));
        const QString y_mar(m_currentVMLProperties.vmlStyle.value("margin-top"));

        m_currentVMLProperties.groupWidthUnit = width.right(2); // pt, cm etc.
        m_currentVMLProperties.groupHeightUnit = height.right(2);
        m_currentVMLProperties.real_groupWidth = width.left(width.length() - 2).toDouble();
        m_currentVMLProperties.real_groupHeight = height.left(height.length() - 2).toDouble();

        m_currentVMLProperties.groupX = 0;
        m_currentVMLProperties.groupY = 0;
        m_currentVMLProperties.groupWidth = 0;
        m_currentVMLProperties.groupHeight = 0;
        m_currentVMLProperties.groupXOffset = 0;
        m_currentVMLProperties.groupYOffset = 0;
        // Assuming that two last chars are unit (cm, pt)
        m_currentVMLProperties.groupXOffset = x_mar.left(x_mar.length() - 2).toDouble();
        m_currentVMLProperties.groupYOffset = y_mar.left(y_mar.length() - 2).toDouble();

        TRY_READ_ATTR_WITHOUT_NS(coordsize)

        if (!coordsize.isEmpty()) {
            m_currentVMLProperties.groupWidth = coordsize.mid(0, coordsize.indexOf(',')).toInt();
            m_currentVMLProperties.groupHeight = coordsize.mid(coordsize.indexOf(',') + 1).toInt();
        }

        TRY_READ_ATTR_WITHOUT_NS(coordorigin)
        if (!coordorigin.isEmpty()) {
            m_currentVMLProperties.groupX = coordorigin.mid(0, coordorigin.indexOf(',')).toInt();
            m_currentVMLProperties.groupY = coordorigin.mid(coordorigin.indexOf(',') + 1).toInt();
        }
    }

    MSOOXML::Utils::XmlWriteBuffer frameBuf;
    body = frameBuf.setWriter(body);

    m_currentVMLProperties.wrapRead = false;

    takeDefaultValues();
    handleStrokeAndFill(attrs);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (name() == "shapetype") {
                m_VMLShapeStack.push(m_currentVMLProperties);
                // Template by default should not have any group info
                TRY_READ(shapetype)
                m_currentVMLProperties = m_VMLShapeStack.pop();
            }
            else if (name() == "rect") {
                m_VMLShapeStack.push(m_currentVMLProperties);
                m_currentVMLProperties.insideGroup = true;
                TRY_READ(rect)
                m_currentVMLProperties = m_VMLShapeStack.pop();
            }
            else if (name() == "roundrect") {
                m_VMLShapeStack.push(m_currentVMLProperties);
                m_currentVMLProperties.insideGroup = true;
                TRY_READ(roundrect)
                m_currentVMLProperties = m_VMLShapeStack.pop();
            }
            else if (name() == "oval") {
                m_VMLShapeStack.push(m_currentVMLProperties);
                m_currentVMLProperties.insideGroup = true;
                TRY_READ(oval)
                m_currentVMLProperties = m_VMLShapeStack.pop();
            }
            else if (name() == "shape") {
                m_VMLShapeStack.push(m_currentVMLProperties);
                m_currentVMLProperties.insideGroup = true;
                TRY_READ(shape)
                m_currentVMLProperties = m_VMLShapeStack.pop();
            }
            else if (name() == "group") {
                m_VMLShapeStack.push(m_currentVMLProperties);
                m_currentVMLProperties.insideGroup = true;
                TRY_READ(group)
                m_currentVMLProperties = m_VMLShapeStack.pop();
            }
            ELSE_TRY_READ_IF(fill)
            ELSE_TRY_READ_IF(stroke)
            ELSE_TRY_READ_IF(shadow)
            else if (qualifiedName() == "w10:wrap") {
                m_currentVMLProperties.wrapRead = true;
                TRY_READ(wrap)
            }
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    body = frameBuf.originalWriter();

    createFrameStart(GroupStart);

    (void)frameBuf.releaseWriter();

    body->endElement(); // draw:g

    popCurrentDrawStyle();

    READ_EPILOGUE
}

// Generic helper which approximates all figures to be rectangles
// use until better implementation is done
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::genericReader()
{
    const QXmlStreamAttributes attrs(attributes());
//! @todo support more attrs
    TRY_READ_ATTR_WITHOUT_NS(style)
    RETURN_IF_ERROR(parseCSS(style))

    pushCurrentDrawStyle(new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic"));
    if (m_moveToStylesXml) {
        m_currentDrawStyle->setAutoStyleInStylesDotXml(true);
    }

    takeDefaultValues();
    handleStrokeAndFill(attrs);

    MSOOXML::Utils::XmlWriteBuffer frameBuf;
    body = frameBuf.setWriter(body);

    m_currentVMLProperties.wrapRead = false;

    while (!atEnd()) {
        readNext();
        if (isEndElement() && qualifiedName() == m_currentVMLProperties.currentEl) {
            break;
        }
        else if (isStartElement()) {
            TRY_READ_IF(fill)
            ELSE_TRY_READ_IF(textbox)
            ELSE_TRY_READ_IF(stroke)
            ELSE_TRY_READ_IF(shadow)
            else if (qualifiedName() == "w10:wrap") {
                m_currentVMLProperties.wrapRead = true;
                TRY_READ(wrap)
            }
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    body = frameBuf.originalWriter();

    createFrameStart(RectStart);

    (void)frameBuf.releaseWriter();

    createFrameEnd();

    popCurrentDrawStyle();

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL oval
//! oval handler (Oval)
// For parents, children, look from rect
// Note: this is atm. simplified, should in reality make an oval
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_oval()
{
    READ_PROLOGUE

    m_currentVMLProperties.currentEl = "v:oval";
    KoFilter::ConversionStatus status = genericReader();
    if (status != KoFilter::OK) {
        return status;
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL roundrect
//! roundrect handler (Rouned rectangle)
// For parents, children, look from rect
// Note: this is atm. simplified, should in reality make a round rectangle
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_roundrect()
{
    READ_PROLOGUE

    m_currentVMLProperties.currentEl = "v:roundrect";
    KoFilter::ConversionStatus status = genericReader();
    if (status != KoFilter::OK) {
        return status;
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL fill
//! fill handler (Shape Fill Properties)
/*! ECMA-376 Part 4, 14.1.2.16, p.280.
 This element specifies how the path should be filled if something beyond
 a solid color fill is desired.

 Parent elements:
 - arc (§14.1.2.1)
 - background (Part 1, §17.2.1)
 - background (§14.1.2.2)
 - curve (§14.1.2.3)
 - [done] group (§14.1.2.7)
 - image (§14.1.2.10)
 - line (§14.1.2.12)
 - object (Part 1, §17.3.3.19)
 - [done] oval (§14.1.2.13)
 - pict (§9.2.2.2)
 - pict (§9.5.1)
 - polyline (§14.1.2.15)
 - [done] rect (§14.1.2.16)
 - [done] roundrect (§14.1.2.17)
 - [done] shape (§14.1.2.19)
 - shapedefaults (§14.2.2.28)
 - [done] shapetype (§14.1.2.20)

 Child elements:
 - fill (Shape Fill Extended Properties) §14.2.2.13
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_fill()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
//! @todo support more attrs
    // Relationship ID of the relationship to the image used for this fill.
    TRY_READ_ATTR_WITH_NS(r, id)
    m_currentVMLProperties.vmlStyle.insert("v:fill@r:id", r_id);
    // The kind of fill. Default is solid.
    TRY_READ_ATTR_WITHOUT_NS(type)
    // frame (Stretch Image to Fit) - The image is stretched to fill the shape.
    // gradient (Linear Gradient) - The fill colors blend together in a linear gradient from bottom to top.
    // gradientRadial (Radial Gradient) - The fill colors blend together in a radial gradient.
    // pattern (Image Pattern) - The image is used to create a pattern using the fill colors.
    // tile (Tiled Image) - The fill image is tiled.
    // solid (Solid Fill) - The fill pattern is a solid color.

    // Spec says that this should overwrite the shape's value, but apparently
    // a non specified value does not overwrite it
    //m_currentVMLProperties.filled = true; // default in this function

    TRY_READ_ATTR_WITHOUT_NS(on)
    if (on == "f" || on == "false") {
        m_currentVMLProperties.filled = false;
    }

    // Note this is only like this for solidfill, for others do something...
    TRY_READ_ATTR_WITHOUT_NS(color)
    if (!color.isEmpty()) {
        m_currentVMLProperties.shapeColor = MSOOXML::Utils::rgbColor(color);
    }

    TRY_READ_ATTR_WITHOUT_NS(color2)
    if (!color2.isEmpty()) {
        m_currentVMLProperties.shapeSecondaryColor = MSOOXML::Utils::rgbColor(color2, m_currentVMLProperties.shapeColor);
    }
    TRY_READ_ATTR_WITHOUT_NS(angle)

    TRY_READ_ATTR_WITHOUT_NS(opacity)
    if (!opacity.isEmpty()) {
        if (opacity.right(1) == "f") {
            opacity = opacity.left(opacity.length()-1);
            m_currentVMLProperties.opacity = 100 * opacity.toInt() / 65536;
        }
        else {
            if (opacity.left(1) == ".") {
                opacity = "0" + opacity;
            }
            m_currentVMLProperties.opacity = 100 * opacity.toDouble();
        }
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
//            TRY_READ_IF(fill)
//! @todo add ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL background

//! fill handler (Document Background)
/*! ECMA-376 Part 4, 14.1.2.2, p.235.
 This element describes the fill of the background of a page using vector graphics fills.

 Parent elements:
 - background (Part 1, §17.2.1)
 - object (Part 1, §17.3.3.19)
 - pict (§9.2.2.2)
 - pict (§9.5.1)

 Child elements:
 - [Done] fill (Shape Fill Properties) §14.1.2.5

 Attributes:
 - bwmode (Blackand- White Mode)
 - bwnormal (Normal Black-and-White Mode)
 - bwpure (Pure Black-and-White Mode)
 - fillcolor (Fill Color)
 - filled (Shape Fill Toggle)
 - id (Unique Identifier)
 - targetscreensize (Target Screen Size)
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_VML_background()
{
    READ_PROLOGUE2(VML_background)
    //const QXmlStreamAttributes attrs(attributes());
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(fill)
            ELSE_WRONG_FORMAT
        }
    }
    const QString rId(m_currentVMLProperties.vmlStyle.value("v:fill@r:id"));
    if (!rId.isEmpty()) {
        const QString sourceName(m_context->relationships->target(m_context->path, m_context->file, rId));
        kDebug() << "sourceName:" << sourceName;
        if (sourceName.isEmpty()) {
            return KoFilter::FileNotFound;
        }
        QString destinationName = QLatin1String("Pictures/") + sourceName.mid(sourceName.lastIndexOf('/') + 1);;
        RETURN_IF_ERROR( m_context->import->copyFile(sourceName, destinationName, false ) )
        addManifestEntryForFile(destinationName);
        addManifestEntryForPicturesDir();
        if (m_pDocBkgImageWriter) {
            delete m_pDocBkgImageWriter->device();
            delete m_pDocBkgImageWriter;
            m_pDocBkgImageWriter = NULL;
        }
        QBuffer* buffer = new QBuffer();
        m_pDocBkgImageWriter = new KoXmlWriter(buffer);

        m_pDocBkgImageWriter->startElement("style:background-image");
        m_pDocBkgImageWriter->addAttribute("xlink:href", destinationName);
        m_pDocBkgImageWriter->addAttribute("xlink:type", "simple");
        m_pDocBkgImageWriter->addAttribute("xlink:actuate", "onLoad");
        m_pDocBkgImageWriter->endElement(); //style:background-image
    }
    READ_EPILOGUE
}

static QString getNumber(QString& source)
{
    QString number;
    int index = 0;
    bool numberOk = true;
    while (true) {
        QString(source.at(index)).toInt(&numberOk);
        if (numberOk) {
            number = number + source.at(index);
            ++index;
        }
        else {
            break;
        }
    }
    source = source.mid(index);
    return number;
}

static QString getArgument(QString& source, bool commaMeansZero, bool& wasCommand)
{
    wasCommand = false;
    if (source.at(0) == ',') {
        source = source.mid(1);
        if (commaMeansZero) {
            return "0";
        }
    }
    bool isNumber;
    QString(source.at(0)).toInt(&isNumber);
    if (isNumber) {
        return getNumber(source);
    }
    if (source.at(0) == '-') { //negative number
        source = source.mid(1);
        return QString("-%1").arg(getNumber(source));
    }
    if (source.at(0) == ',') { // case of 1,,2
        return "0";
    }
    if (source.at(0) == '#') {
        source = source.mid(1);
        return QString("$%1").arg(getNumber(source));
    }
    if (source.at(0) == '@') {
        source = source.mid(1);
        return QString("?f%1").arg(getNumber(source));
    }

    wasCommand = true;
    return "0"; // this means case 1,e
}

static QString convertToEnhancedPath(const QString& source, QString& extraShapeFormulas)
{
    enum ConversionState {CommandExpected, ArgumentExpected};
    int extraFormulaIndex = 1;
    QString parsedString = source;
    QString returnedString;
    ConversionState state = CommandExpected;
    enum CommandType {MoveCommand, LineCommand, RelativeLineCommand, QuadEllipXCommand, QuadEllipYCommand,
                      CurveCommand, RelativeCurveCommand, ArcCommand, ArcToCommand, ArcCommandClock, ArcToCommandClock,
                      AngleEllipseToCommand, AngleEllipseCommand};
    CommandType lastCommand = MoveCommand;
    QString firstMoveX, firstMoveY, currentX, currentY;
    bool argumentMove;
    QChar command;
    QString first, second, third, fourth, fifth, sixth, seventh, eighth;

    while (true) {
        if (parsedString.length() == 0) {
            break;
        }
        while (parsedString.at(0) == ' ') {
            parsedString = parsedString.trimmed();
        }
        switch (state) {
        case CommandExpected:
            command = parsedString.at(0);
            parsedString = parsedString.mid(1);
            state = ArgumentExpected;
            if (command == 'm') {
                lastCommand = MoveCommand;
            }
            else if (command == 'l') {
                lastCommand = LineCommand;
            }
            else if (command == 'r') {
                lastCommand = RelativeLineCommand;
            }
            else if (command == 'x') {
                state = CommandExpected;
                returnedString += " Z";
            }
            else if (command == 'e') {
                returnedString += " N";
                state = CommandExpected;
            }
            else if (command == 'c') {
                lastCommand = CurveCommand;
            }
            else if (command == 'v') {
                lastCommand = RelativeCurveCommand;
            }
            else if (command == 'q') {
                QChar subcommand = parsedString.at(0);
                parsedString = parsedString.mid(1);
                if (subcommand == 'x') {
                    lastCommand = QuadEllipXCommand;
                    returnedString += " X";
                }
                else {
                    lastCommand = QuadEllipYCommand;
                    returnedString += " Y";
                }
            }
            else if (command == 'a') {
                QChar subcommand = parsedString.at(0);
                parsedString = parsedString.mid(1);
                if (subcommand == 'r') {
                    lastCommand = ArcCommand;
                    returnedString += " B";
                }
                else if (subcommand == 'e') {
                    lastCommand = AngleEllipseToCommand;
                    returnedString += " T";
                }
                else if (subcommand == 'l') {
                    lastCommand = AngleEllipseCommand;
                    returnedString += " U";
                }
                else {
                    lastCommand = ArcToCommand;
                    returnedString += " A";
                }
            }
            else if (command == 'w') {
                QChar subcommand = parsedString.at(0);
                parsedString = parsedString.mid(1);
                if (subcommand == 'r') {
                    lastCommand = ArcCommandClock;
                    returnedString += " V";
                }
                else {
                    lastCommand = ArcToCommandClock;
                    returnedString += " W";
                }
            }
            else if (command == 'n') {
                QChar subcommand = parsedString.at(0);
                parsedString = parsedString.mid(1);
                if (subcommand == 'f') {
                    returnedString += " F";
                }
                else {
                    returnedString += " S";
                }
                state = CommandExpected;
            }
            break;
        case ArgumentExpected:
            switch (lastCommand) {
            case MoveCommand:
                first = getArgument(parsedString, true, argumentMove);
                second = getArgument(parsedString, false, argumentMove);
                firstMoveX = first;
                firstMoveY = second;
                currentX = first;
                currentY = second;
                returnedString += QString(" M %1 %2").arg(first).arg(second);
                state = CommandExpected;
                break;
            case RelativeCurveCommand:
                first = getArgument(parsedString, true, argumentMove);
                second = getArgument(parsedString, false, argumentMove);
                third = getArgument(parsedString, false, argumentMove);
                fourth = getArgument(parsedString, false, argumentMove);
                fifth = getArgument(parsedString, false, argumentMove);
                sixth = getArgument(parsedString, false, argumentMove);
                first = QString("%1").arg(first.toInt() + currentX.toInt());
                second = QString("%1").arg(second.toInt() + currentY.toInt());
                third = QString("%1").arg(third.toInt() + currentX.toInt());
                fourth = QString("%1").arg(fourth.toInt() + currentY.toInt());
                currentX = QString("%1").arg(fifth.toInt() + currentX.toInt());
                currentY = QString("%1").arg(sixth.toInt() + currentY.toInt());
                returnedString += QString(" C %1 %2 %3 %4 %5 %6").arg(first).arg(second).arg(third).
                                  arg(fourth).arg(currentX).arg(currentY);
                while (true) {
                    first = getArgument(parsedString, false, argumentMove);
                    if (argumentMove) {
                        state = CommandExpected;
                        break;
                    }
                    second = getArgument(parsedString, false, argumentMove);
                    third = getArgument(parsedString, false, argumentMove);
                    fourth = getArgument(parsedString, false, argumentMove);
                    fifth = getArgument(parsedString, false, argumentMove);
                    sixth = getArgument(parsedString, false, argumentMove);
                    first = QString("%1").arg(first.toInt() + currentX.toInt());
                    second = QString("%1").arg(second.toInt() + currentY.toInt());
                    third = QString("%1").arg(third.toInt() + currentX.toInt());
                    fourth = QString("%1").arg(fourth.toInt() + currentY.toInt());
                    currentX = QString("%1").arg(fifth.toInt() + currentX.toInt());
                    currentY = QString("%1").arg(sixth.toInt() + currentY.toInt());
                    returnedString += QString(" %1 %2 %3 %4 %5 %6").arg(first).arg(second).arg(third).
                                      arg(fourth).arg(currentX).arg(currentY);
                }
                break;
            case CurveCommand:
                first = getArgument(parsedString, true, argumentMove);
                second = getArgument(parsedString, false, argumentMove);
                third = getArgument(parsedString, false, argumentMove);
                fourth = getArgument(parsedString, false, argumentMove);
                fifth = getArgument(parsedString, false, argumentMove);
                sixth = getArgument(parsedString, false, argumentMove);
                returnedString += QString(" C %1 %2 %3 %4 %5 %6").arg(first).arg(second).arg(third).
                                  arg(fourth).arg(fifth).arg(sixth);
                currentX = fifth;
                currentY = sixth;
                while (true) {
                    first = getArgument(parsedString, false, argumentMove);
                    if (argumentMove) {
                        state = CommandExpected;
                        break;
                    }
                    second = getArgument(parsedString, false, argumentMove);
                    third = getArgument(parsedString, false, argumentMove);
                    fourth = getArgument(parsedString, false, argumentMove);
                    fifth = getArgument(parsedString, false, argumentMove);
                    sixth = getArgument(parsedString, false, argumentMove);
                    returnedString += QString(" %1 %2 %3 %4 %5 %6").arg(first).arg(second).arg(third).
                                      arg(fourth).arg(fifth).arg(sixth);
                    currentX = fifth;
                    currentY = sixth;
                }
                break;
            case LineCommand:
                first = getArgument(parsedString, true, argumentMove);
                second = getArgument(parsedString, false, argumentMove);
                returnedString += QString(" L %1 %2").arg(first).arg(second);
                currentX = first;
                currentY = second;
                while (true) {
                    first = getArgument(parsedString, false, argumentMove);
                    if (argumentMove) {
                        state = CommandExpected;
                        break;
                    }
                    second = getArgument(parsedString, false, argumentMove);
                    currentX = first;
                    currentY = second;
                    returnedString += QString(" %1 %2").arg(first).arg(second);
                }
                break;
            case RelativeLineCommand:
                first = getArgument(parsedString, true, argumentMove);
                second = getArgument(parsedString, false, argumentMove);
                currentX = QString("%1").arg(first.toInt() + currentX.toInt());
                currentY = QString("%1").arg(second.toInt() + currentY.toInt());
                returnedString += QString(" L %1 %2").arg(currentX).arg(currentY);
                while (true) {
                    first = getArgument(parsedString, false, argumentMove);
                    if (argumentMove) {
                        state = CommandExpected;
                        break;
                    }
                    second = getArgument(parsedString, false, argumentMove);
                    currentX = QString("%1").arg(first.toInt() + currentX.toInt());
                    currentY = QString("%1").arg(second.toInt() + currentY.toInt());
                    returnedString += QString(" %1 %2").arg(currentX).arg(currentY);
                }
                break;
            case QuadEllipXCommand:
            case QuadEllipYCommand:
                first = getArgument(parsedString, true, argumentMove);
                second = getArgument(parsedString, false, argumentMove);
                returnedString += QString(" %1 %2").arg(first).arg(second);
                currentX = first;
                currentY = second;
                while (true) {
                    first = getArgument(parsedString, false, argumentMove);
                    if (argumentMove) {
                        state = CommandExpected;
                        break;
                    }
                    second = getArgument(parsedString, false, argumentMove);
                    currentX = first;
                    currentY = second;
                    returnedString += QString(" %1 %2").arg(first).arg(second);
                }
                break;
            case ArcCommand:
            case ArcToCommand:
            case ArcCommandClock:
            case ArcToCommandClock:
                first = getArgument(parsedString, true, argumentMove);
                second = getArgument(parsedString, false, argumentMove);
                third = getArgument(parsedString, false, argumentMove);
                fourth = getArgument(parsedString, false, argumentMove);
                fifth = getArgument(parsedString, false, argumentMove);
                sixth = getArgument(parsedString, false, argumentMove);
                seventh = getArgument(parsedString, false, argumentMove);
                eighth = getArgument(parsedString, false, argumentMove);
                currentX = seventh;
                currentY = eighth;
                returnedString += QString(" %1 %2 %3 %4 %5 %6 %7 %8").arg(first).arg(second).arg(third).arg(fourth).
                    arg(fifth).arg(sixth).arg(seventh).arg(eighth);
                while (true) {
                    first = getArgument(parsedString, false, argumentMove);
                    if (argumentMove) {
                        state = CommandExpected;
                        break;
                    }
                    second = getArgument(parsedString, false, argumentMove);
                    third = getArgument(parsedString, false, argumentMove);
                    fourth = getArgument(parsedString, false, argumentMove);
                    fifth = getArgument(parsedString, false, argumentMove);
                    sixth = getArgument(parsedString, false, argumentMove);
                    seventh = getArgument(parsedString, false, argumentMove);
                    eighth = getArgument(parsedString, false, argumentMove);
                    currentX = seventh;
                    currentY = eighth;
                    returnedString += QString(" %1 %2 %3 %4 %5 %6 %7 %8").arg(first).arg(second).arg(third).arg(fourth).
                        arg(fifth).arg(sixth).arg(seventh).arg(eighth);
                }
                break;
            case AngleEllipseToCommand:
            case AngleEllipseCommand:
                first = getArgument(parsedString, true, argumentMove);
                second = getArgument(parsedString, false, argumentMove);
                third = getArgument(parsedString, false, argumentMove);
                fourth = getArgument(parsedString, false, argumentMove);
                fifth = getArgument(parsedString, false, argumentMove);
                sixth = getArgument(parsedString, false, argumentMove);
                seventh = QString("?extraFormula%1").arg(extraFormulaIndex);
                extraShapeFormulas += "\n<draw:equation ";
                extraShapeFormulas += QString("draw:name=\"extraFormula%1\" draw:formula=\"").arg(extraFormulaIndex);
                extraShapeFormulas += QString("%1 / 65536").arg(fifth);
                extraShapeFormulas += "\" ";
                extraShapeFormulas += "/>";
                ++extraFormulaIndex;
                eighth = QString("?extraFormula%1").arg(extraFormulaIndex);
                extraShapeFormulas += "\n<draw:equation ";
                extraShapeFormulas += QString("draw:name=\"extraFormula%1\" draw:formula=\"").arg(extraFormulaIndex);
                extraShapeFormulas += QString("%1 / 65536").arg(sixth);
                extraShapeFormulas += "\" ";
                extraShapeFormulas += "/>";
                ++extraFormulaIndex;
                currentX = QString("%1").arg(first.toInt() + cos(sixth.toDouble()));
                currentY = QString("%1").arg(first.toInt() + sin(sixth.toDouble()));
                returnedString += QString(" %1 %2 %3 %4 %5 %6").arg(first).arg(second).arg(third).arg(fourth).
                    arg(seventh).arg(eighth);
                while (true) {
                    first = getArgument(parsedString, false, argumentMove);
                    if (argumentMove) {
                        state = CommandExpected;
                        break;
                    }
                    second = getArgument(parsedString, false, argumentMove);
                    third = getArgument(parsedString, false, argumentMove);
                    fourth = getArgument(parsedString, false, argumentMove);
                    fifth = getArgument(parsedString, false, argumentMove);
                    sixth = getArgument(parsedString, false, argumentMove);
                    seventh = QString("?extraFormula%1").arg(extraFormulaIndex);
                    extraShapeFormulas += "\n<draw:equation ";
                    extraShapeFormulas += QString("draw:name=\"extraFormula%1\" draw:formula=\"").arg(extraFormulaIndex);
                    extraShapeFormulas += QString("%1 / 65536").arg(fifth);
                    extraShapeFormulas += "\" ";
                    extraShapeFormulas += "/>";
                    ++extraFormulaIndex;
                    eighth = QString("?extraFormula%1").arg(extraFormulaIndex);
                    extraShapeFormulas += "\n<draw:equation ";
                    extraShapeFormulas += QString("draw:name=\"extraFormula%1\" draw:formula=\"").arg(extraFormulaIndex);
                    extraShapeFormulas += QString("%1 / 65536").arg(sixth);
                    extraShapeFormulas += "\" ";
                    extraShapeFormulas += "/>";
                    ++extraFormulaIndex;
                    currentX = QString("%1").arg(first.toInt() + cos(sixth.toDouble()));
                    currentY = QString("%1").arg(first.toInt() + sin(sixth.toDouble()));
                    returnedString += QString(" %1 %2 %3 %4 %5 %6").arg(first).arg(second).arg(third).arg(fourth).
                    arg(seventh).arg(eighth);
                }
                break;
            }
        }
    }

    return returnedString;
}

#undef CURRENT_EL
#define CURRENT_EL shapetype
//! shapetype handler (Shape Template)
/*! ECMA-376 Part 4, 14.1.2.20, p.539.
 This element defines a shape template that can be used to create other shapes.
 Shapetype is identical to the shape element (§14.1.2.19) except it cannot reference another
 shapetype element.

 Parent elements:
 - background (Part 1, §17.2.1)
 - [done] group (§14.1.2.7)
 - [done] object (Part 1, §17.3.3.19)
 - pict (§9.2.2.2)
 - pict (§9.5.1)

 Child elements:
 - bordertop (Top Border) §14.3.2.5
 - callout (Callout) §14.2.2.2
 - ClientData (Attached Object Data) §14.4.2.12
 - clippath (Shape Clipping Path) §14.2.2.3
 - complex (Complex) §14.2.2.7
 - extrusion (3D Extrusion) §14.2.2.11
 - [done] fill (Shape Fill Properties) §14.1.2.5
 - [done] formulas (Set of Formulas) §14.1.2.6
 - handles (Set of Handles) §14.1.2.9
 - imagedata (Image Data) §14.1.2.11
 - lock (Shape Protections) §14.2.2.18
 - path (Shape Path) §14.1.2.14
 - [done shadow (Shadow Effect) §14.1.2.18
 - signatureline (Digital Signature Line) §14.2.2.30
 - skew (Skew Transform) §14.2.2.31
 - [done] stroke (Line Stroke Settings) §14.1.2.21
 - textbox (Text Box) §14.1.2.22
 - textdata (VML Diagram Text) §14.5.2.2
 - textpath (Text Layout Path) §14.1.2.23
 - wrap (Text Wrapping) §14.3.2.6
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_shapetype()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(adj)
    TRY_READ_ATTR_WITHOUT_NS(coordsize)
    TRY_READ_ATTR_WITHOUT_NS(id)
    TRY_READ_ATTR_WITHOUT_NS(path)

    // Using KoXmlWriters is another opinion, but this one is used to avoid
    // hassle with deletions (koxmlwriter not deleting its device etc)
    m_currentVMLProperties.shapeTypeString = "<draw:enhanced-geometry ";

    if (!adj.isEmpty()) {
        QString tempModifiers = adj;
        tempModifiers.replace(",,", ",0,");
        tempModifiers.replace(',', " ");
        m_currentVMLProperties.shapeTypeString += QString("draw:modifiers=\"%1\" ").arg(tempModifiers);
    }
    if (!coordsize.isEmpty()) {
        QString tempViewBox = "0 0 " + coordsize;
        tempViewBox.replace(',', " ");
        m_currentVMLProperties.shapeTypeString += QString("svg:viewBox=\"%1\" ").arg(tempViewBox);
    }
    m_currentVMLProperties.extraShapeFormulas = QString();
    if (!path.isEmpty()) {
        QString enPath = convertToEnhancedPath(path, m_currentVMLProperties.extraShapeFormulas);
        m_currentVMLProperties.shapeTypeString += QString("draw:enhanced-path=\"%1\" ").arg(enPath);
    }

    takeDefaultValues();
    handleStrokeAndFill(attrs);

    m_currentVMLProperties.shapeTypeString += ">";

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(formulas)
            ELSE_TRY_READ_IF(shadow)
            ELSE_TRY_READ_IF(fill)
            ELSE_TRY_READ_IF(stroke)
            SKIP_UNKNOWN
        }
    }

    m_currentVMLProperties.shapeTypeString += m_currentVMLProperties.extraShapeFormulas;
    m_currentVMLProperties.shapeTypeString += "</draw:enhanced-geometry>";

    m_definedShapeTypes[id] = m_currentVMLProperties;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL formulas
/* formulas handlers (Set of Formulas)

 Parent elements:
 - arc (§14.1.2.1);
 - background (Part 1, §17.2.1);
 - curve (§14.1.2.3);
 - group (§14.1.2.7);
 - image (§14.1.2.10);
 - line (§14.1.2.12);
 - object (Part 1, §17.3.3.19);
 - oval (§14.1.2.13);
 - pict (§9.2.2.2);
 - pict (§9.5.1);
 - polyline (§14.1.2.15);
 - rect (§14.1.2.16);
 - roundrect (§14.1.2.17);
 - shape (§14.1.2.19);
 - [done] shapetype (§14.1.2.20)

 Child elements:
 -  [done] f (Single Formula) §14.1.2.4
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_formulas()
{
    READ_PROLOGUE

    m_currentVMLProperties.formulaIndex = 0;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(f)
            ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL f
/*! f handler (Shape Definition)
 Parent elements:
 - [done] formulas (§14.1.2.6)

 Child elements:
 - none
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_f()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(eqn)
    m_currentVMLProperties.shapeTypeString += "\n<draw:equation ";
    m_currentVMLProperties.shapeTypeString += QString("draw:name=\"f%1\" draw:formula=\"").arg(m_currentVMLProperties.formulaIndex);

    if (!eqn.isEmpty()) {
        eqn = eqn.trimmed();
        eqn.replace('#', '$'); // value reference
        eqn.replace('@', "?f"); // function reference
        eqn.replace("pixelWidth", "width");
        eqn.replace("pixelHeight", "height");
        eqn.replace("emuWidth", "width");
        eqn.replace("emuHeight", "height");
        eqn.replace("emuWidth2", "(width/2)");
        eqn.replace("emuHeight2", "(height/2)");
        eqn.replace("lineDrawn", "hasstroke");
        eqn.replace("pixelLineWidth", "1");
        eqn.replace("xcenter", "(width/2)");
        eqn.replace("ycenter", "(height/2)");
        int commandIndex = eqn.indexOf(' ');
        QString command = eqn.left(commandIndex);
        eqn = eqn.mid(commandIndex + 1);
        QList<QString> parameters;
        while (true) {
            commandIndex = eqn.indexOf(' ');
            if (commandIndex < 0) {
                parameters.append(eqn);
                break;
            }
            parameters.append(eqn.left(commandIndex));
            eqn = eqn.mid(commandIndex + 1);
        }
        if (command == "val") {
            m_currentVMLProperties.shapeTypeString += parameters.at(0);
        }
        else if (command == "sum") {
            m_currentVMLProperties.shapeTypeString += parameters.at(0) + "+" + parameters.at(1) + "-" + parameters.at(2);
        }
        else if (command == "prod") {
            m_currentVMLProperties.shapeTypeString += parameters.at(0) + "*" + parameters.at(1) + "/" + parameters.at(2);
        }
        else if (command == "abs") {
            m_currentVMLProperties.shapeTypeString += QString("abs(%1)").arg(parameters.at(0));
        }
        else if (command == "min") {
            m_currentVMLProperties.shapeTypeString += QString("min(%1,%2)").arg(parameters.at(0)).arg(parameters.at(1));
        }
        else if (command == "max") {
            m_currentVMLProperties.shapeTypeString += QString("max(%1,%2)").arg(parameters.at(0)).arg(parameters.at(1));
        }
        else if (command == "if") {
            m_currentVMLProperties.shapeTypeString += QString("if(%1,%2,%3)").arg(parameters.at(0)).arg(parameters.at(1)).arg(parameters.at(2));
        }
        else if (command == "sqrt") {
            m_currentVMLProperties.shapeTypeString += QString("sqrt(%1)").arg(parameters.at(0));
        }
        else if (command == "mid") {
            m_currentVMLProperties.shapeTypeString += QString("(%1+%2)/2").arg(parameters.at(0)).arg(parameters.at(1));
        }
        else if (command == "mod") {
            m_currentVMLProperties.shapeTypeString += QString("sqrt(%1*%1+%2*%2+%3*%3)").arg(parameters.at(0)).arg(parameters.at(1)).arg(parameters.at(2));
        }
        else if (command == "ellipse") {
            m_currentVMLProperties.shapeTypeString += QString("%3-sqrt(1-(%1/%2)*(%1/%2))").arg(parameters.at(0)).arg(parameters.at(1)).arg(parameters.at(2));
        }
        else if (command == "atan2") { // converting to fd unit (degrees * 65536)
            m_currentVMLProperties.shapeTypeString += QString("3754936*atan2(%2,%1)").arg(parameters.at(0)).arg(parameters.at(1));
        }
        else if (command == "cosatan2") {
            m_currentVMLProperties.shapeTypeString += QString("%1*cos(atan2(%3,%2))").arg(parameters.at(0)).arg(parameters.at(1)).arg(parameters.at(2));
        }
        else if (command == "sinatan2") {
            m_currentVMLProperties.shapeTypeString += QString("%1*sin(atan2(%3,%2))").arg(parameters.at(0)).arg(parameters.at(1)).arg(parameters.at(2));
        }
        else if (command == "sumangle") {
            m_currentVMLProperties.shapeTypeString += QString("%1+%2*65536-%3*65536").arg(parameters.at(0)).arg(parameters.at(1)).arg(parameters.at(2));
        }
        else if (command == "sin") { // converting fd unit to radians
            m_currentVMLProperties.shapeTypeString += QString("%1*sin(%2 * 0.000000266)").arg(parameters.at(0)).arg(parameters.at(1));
        }
        else if (command == "cos") {
            m_currentVMLProperties.shapeTypeString += QString("%1*cos(%2 * 0.000000266)").arg(parameters.at(0)).arg(parameters.at(1));
        }
        else if (command == "tan") {
            m_currentVMLProperties.shapeTypeString += QString("%1*tan(%2 * 0.000000266)").arg(parameters.at(0)).arg(parameters.at(1));
        }
    }

    m_currentVMLProperties.shapeTypeString += "\" ";
    m_currentVMLProperties.shapeTypeString += "/>";

    ++m_currentVMLProperties.formulaIndex;
    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL shape
/*! shape handler (Shape Definition)
 ECMA-376 Part 4, 14.1.2.19, p.509.

 Parent elements:
 - background (Part 1, §17.2.1)
 - group (§14.1.2.7)
 - [done] object (Part 1, §17.3.3.19)
 - [done] pict (§9.2.2.2); pict (§9.5.1)

 Child elements:
 - anchorlock (Anchor Location Is Locked) §14.3.2.1
 - borderbottom (Bottom Border) §14.3.2.2
 - borderleft (Left Border) §14.3.2.3
 - borderright (Right Border) §14.3.2.4
 - bordertop (Top Border) §14.3.2.5
 - callout (Callout) §14.2.2.2
 - ClientData (Attached Object Data) §14.4.2.12
 - clippath (Shape Clipping Path) §14.2.2.3
 - equationxml (Storage for Alternate Math Content) §14.2.2.10
 - extrusion (3D Extrusion) §14.2.2.11
 - [done] fill (Shape Fill Properties) §14.1.2.5
 - [done] formulas (Set of Formulas) §14.1.2.6
 - handles (Set of Handles) §14.1.2.9
 - [done] imagedata (Image Data) §14.1.2.11
 - ink (Ink) §14.2.2.15
 - iscomment (Ink Annotation Flag) §14.5.2.1
 - lock (Shape Protections) §14.2.2.18
 - path (Shape Path) §14.1.2.14
 - [done] shadow (Shadow Effect) §14.1.2.18
 - signatureline (Digital Signature Line) §14.2.2.30
 - skew (Skew Transform) §14.2.2.31
 - [done] stroke (Line Stroke Settings) §14.1.2.21
 - [done] textbox (Text Box) §14.1.2.22
 - textdata (VML Diagram Text) §14.5.2.2
 - textpath (Text Layout Path) §14.1.2.23
 - [done] wrap (Text Wrapping) §14.3.2.6
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_shape()
{
    READ_PROLOGUE
/*    e.g. <v:shape id="_x0000_i1025" type="#_x0000_t75" style="width:166.5pt;height:124.5pt" o:ole="">
             <v:imagedata r:id="rId7" o:title=""/>
           </v:shape>*/
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(type)
    if (!type.isEmpty()) {
        type = type.mid(1); // removes extra # from the start
        // Inheriting all values from the template shape, except for group values
        // since it is possible that the template was declared outside the group
        bool _insideGroup = m_currentVMLProperties.insideGroup;
        int _groupWidth = m_currentVMLProperties.groupWidth;
        int _groupHeight = m_currentVMLProperties.groupHeight;
        int _groupX = m_currentVMLProperties.groupX;
        int _groupY = m_currentVMLProperties.groupY;
        qreal _groupXOffset = m_currentVMLProperties.groupXOffset;
        qreal _groupYOffset = m_currentVMLProperties.groupYOffset;
        QString _groupWidthUnit = m_currentVMLProperties.groupWidthUnit;
        QString _groupHeightUnit = m_currentVMLProperties.groupHeightUnit;
        qreal _real_groupWidth = m_currentVMLProperties.real_groupWidth;
        qreal _real_groupHeight = m_currentVMLProperties.real_groupHeight;

        m_currentVMLProperties = m_definedShapeTypes.value(type);

        if (_insideGroup) {
            m_currentVMLProperties.insideGroup = _insideGroup;
            m_currentVMLProperties.groupWidth = _groupWidth;
            m_currentVMLProperties.groupHeight = _groupHeight;
            m_currentVMLProperties.groupX = _groupX;
            m_currentVMLProperties.groupY = _groupY;
            m_currentVMLProperties.groupXOffset = _groupXOffset;
            m_currentVMLProperties.groupYOffset = _groupYOffset;
            m_currentVMLProperties.groupWidthUnit = _groupWidthUnit;
            m_currentVMLProperties.groupHeightUnit = _groupHeightUnit;
            m_currentVMLProperties.real_groupWidth = _real_groupWidth;
            m_currentVMLProperties.real_groupHeight = _real_groupHeight;
        }
    }
    else {
        takeDefaultValues();
    }

    handleStrokeAndFill(attrs);

    TRY_READ_ATTR_WITHOUT_NS(id)
    m_currentVMLProperties.currentShapeId = id;

    // For some shapes, it seems that this is the correct id.
    TRY_READ_ATTR_WITH_NS(o, spid)
    if (!o_spid.isEmpty()) {
        m_currentVMLProperties.currentShapeId = o_spid;
    }

    TRY_READ_ATTR_WITH_NS(o, connectortype)

    // CSS2 styling properties of the shape, http://www.w3.org/TR/REC-CSS2
    TRY_READ_ATTR_WITHOUT_NS(style)
    RETURN_IF_ERROR(parseCSS(style))
    kDebug() << "m_vmlStyle:" << m_currentVMLProperties.vmlStyle;

    //! @todo position (can be relative...)
    TRY_READ_ATTR_WITHOUT_NS_INTO(alt, m_currentVMLProperties.shapeAltText)
    TRY_READ_ATTR_WITHOUT_NS_INTO(title, m_currentVMLProperties.shapeTitle)
    TRY_READ_ATTR_WITHOUT_NS(path)
    TRY_READ_ATTR_WITHOUT_NS(adj)
    TRY_READ_ATTR_WITHOUT_NS(coordsize)
    if (!path.isEmpty()) {
        m_currentVMLProperties.shapeTypeString = "<draw:enhanced-geometry ";

        if (!adj.isEmpty()) {
            QString tempModifiers = adj;
            tempModifiers.replace(",,", ",0,");
            tempModifiers.replace(',', " ");
            m_currentVMLProperties.shapeTypeString += QString("draw:modifiers=\"%1\" ").arg(tempModifiers);
        }
        if (!coordsize.isEmpty()) {
            QString tempViewBox = "0 0 " + coordsize;
            tempViewBox.replace(',', " ");
            m_currentVMLProperties.shapeTypeString += QString("svg:viewBox=\"%1\" ").arg(tempViewBox);
        }
        m_currentVMLProperties.extraShapeFormulas = QString();

        m_currentVMLProperties.shapeTypeString += QString("draw:enhanced-path=\"%1\" ").
            arg(convertToEnhancedPath(path, m_currentVMLProperties.extraShapeFormulas));
        m_currentVMLProperties.shapeTypeString += ">";
    }

    MSOOXML::Utils::XmlWriteBuffer frameBuf;
    body = frameBuf.setWriter(body);

    pushCurrentDrawStyle(new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic"));
    if (m_moveToStylesXml) {
        m_currentDrawStyle->setAutoStyleInStylesDotXml(true);
    }

    m_currentVMLProperties.wrapRead = false;

    bool isCustomShape = true;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (qualifiedName() == "v:imagedata") {
                isCustomShape = false;
                TRY_READ(imagedata)
            }
            else if (qualifiedName() == "v:textbox") {
                isCustomShape = false;
                body->startElement("draw:text-box");
                TRY_READ(textbox)
                body->endElement(); // draw:text-box
            }
            ELSE_TRY_READ_IF(stroke)
            ELSE_TRY_READ_IF(fill)
            ELSE_TRY_READ_IF(shadow)
            else if (qualifiedName() == "w10:wrap") {
                m_currentVMLProperties.wrapRead = true;
                TRY_READ(wrap)
            }
            ELSE_TRY_READ_IF(formulas)
            SKIP_UNKNOWN
        }
    }

    // Case of a custom shape which is not defined in a shapetype
    if (!path.isEmpty()) {
        m_currentVMLProperties.shapeTypeString += m_currentVMLProperties.extraShapeFormulas;
        m_currentVMLProperties.shapeTypeString += "</draw:enhanced-geometry>";
    }

    body = frameBuf.originalWriter();

    if (m_outputFrames) {
        if (o_connectortype.isEmpty()) {
            if (!isCustomShape) {
                createFrameStart();
            }
            else {
                createFrameStart(CustomStart);
            }
        }
        else {
            createFrameStart(StraightConnectorStart);
        }
    }

    (void)frameBuf.releaseWriter();

    if (m_outputFrames) {
        if (isCustomShape && o_connectortype.isEmpty()) {
            body->addCompleteElement(m_currentVMLProperties.shapeTypeString.toUtf8());
        }
        createFrameEnd();
    }

    popCurrentDrawStyle();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL imagedata
/*! imagedata handler (Image Data)
 ECMA-376 Part 4, 14.1.2.11, p.351.

 Parent elements:
 - arc (§14.1.2.1)
 - background (Part 1, §17.2.1)
 - curve (§14.1.2.3)
 - group (§14.1.2.7)
 - image (§14.1.2.10)
 - line (§14.1.2.12)
 - object (Part 1, §17.3.3.19)
 - oval (§14.1.2.13)
 - pict (§9.2.2.2)
 - pict (§9.5.1)
 - polyline (§14.1.2.15)
 - rect (§14.1.2.16)
 - roundrect (§14.1.2.17)
 - [done] shape (§14.1.2.19)
 - shapetype (§14.1.2.20)

 Child elements:
 - none

*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_imagedata()
{
    READ_PROLOGUE

/*    e.g. <v:imagedata r:id="rId7" o:title="..."/> */
    const QXmlStreamAttributes attrs(attributes());
    QString imagedata;
    TRY_READ_ATTR_WITH_NS(r, id)
    if (!r_id.isEmpty()) {
        imagedata = m_context->relationships->target(m_context->path, m_context->file, r_id);
    }
    else {
        TRY_READ_ATTR_WITH_NS(o, relid)
        if (!o_relid.isEmpty()) {
            imagedata = m_context->relationships->target(m_context->path, m_context->file, o_relid);
        }
    }

    kDebug() << "imagedata:" << imagedata;
    if (!imagedata.isEmpty()) {
//! @todo ooo saves binaries to the root dir; should we?
        m_context->import->imageSize(imagedata, m_imageSize);

        m_currentVMLProperties.imagedataPath = QLatin1String("Pictures/") + imagedata.mid(imagedata.lastIndexOf('/') + 1);;
        RETURN_IF_ERROR( m_context->import->copyFile(imagedata, m_currentVMLProperties.imagedataPath, false ) )
        addManifestEntryForFile(m_currentVMLProperties.imagedataPath);
        m_currentVMLProperties.imagedataFile = imagedata;
        addManifestEntryForPicturesDir();
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL textbox
/*! text box handler (Text Box)

 Parent elements:
 - [done] shape (§14.1.2.19)
 - more...

*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_textbox()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
#ifdef DOCXXMLDOCREADER_CPP
            TRY_READ_IF_NS(w, txbxContent)
#endif
        }
    }

    READ_EPILOGUE
}

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "w10"

#undef CURRENT_EL
#define CURRENT_EL wrap
// Wrap handler, todo fully..
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_wrap()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(type)
    TRY_READ_ATTR_WITHOUT_NS(side)

    if (type.isEmpty()) {
        if (m_currentVMLProperties.vmlStyle.contains("z-index")) {
            m_currentDrawStyle->addProperty("style:wrap", "run-through");
            if (m_currentVMLProperties.vmlStyle.value("z-index").toInt() > 0) {
                m_currentDrawStyle->addProperty("style:run-through", "foreground");
            }
            else {
                m_currentDrawStyle->addProperty("style:run-through", "background");
            }
        }
        else {
            m_currentDrawStyle->addProperty("style:wrap", "run-through");
            m_currentDrawStyle->addProperty("style:run-through", "foreground");
        }
    }
    else if (type == "through" || type == "square" || type == "tight") {
        if (side.isEmpty()) {
            m_currentDrawStyle->addProperty("style:wrap", "parallel");
        }
        else if (side == "left") {
            m_currentDrawStyle->addProperty("style:wrap", "left");
        }
        else if (side == "largest") {
            m_currentDrawStyle->addProperty("style:wrap", "biggest");
        }
        else if (side == "right") {
            m_currentDrawStyle->addProperty("style:wrap", "right");
        }
        else if (side == "both") {
            m_currentDrawStyle->addProperty("style:wrap", "parallel");
        }
    }
    else if (type == "topAndBottom") {
        m_currentDrawStyle->addProperty("style:wrap", "none");
    }
    else {
        if (side.isEmpty()) { // Note doc doesn't say which one is default
            m_currentDrawStyle->addProperty("style:wrap", "biggest");
        }
        else if (side == "left") {
            m_currentDrawStyle->addProperty("style:wrap", "left");
        }
        else if (side == "largest") {
            m_currentDrawStyle->addProperty("style:wrap", "biggest");
        }
        else if (side == "right") {
            m_currentDrawStyle->addProperty("style:wrap", "right");
        }
        else if (side == "both") {
            m_currentDrawStyle->addProperty("style:wrap", "parallel");
        }
    }

    TRY_READ_ATTR_WITHOUT_NS(anchorx)
    TRY_READ_ATTR_WITHOUT_NS(anchory)

    // Documentation says default to be 'page', however because these are always in a paragraph
    // in a text run, a better default for odf purposes seems to be 'paragraph'

    if (anchory == "page") {
        m_currentDrawStyle->addProperty("style:vertical-rel", "page");
        m_currentVMLProperties.anchorType = "page";
    }
    else if (anchory == "text") {
        m_currentVMLProperties.anchorType = "as-char";
        m_currentDrawStyle->addProperty("style:vertical-rel", "text");
    }
    else if (anchory == "line") {
        m_currentVMLProperties.anchorType = "as-char";
        m_currentDrawStyle->addProperty("style:vertical-rel", "line");
    }
    else { // margin
        m_currentVMLProperties.anchorType = "paragraph";
        m_currentDrawStyle->addProperty("style:vertical-rel", "paragraph");
    }

    if (anchorx == "page") {
        m_currentDrawStyle->addProperty("style:horizontal-rel", "page");
    }
    else if (anchorx == "margin") {
        m_currentDrawStyle->addProperty("style:horizontal-rel", "page-start-margin");
    }
    else if (anchorx == "text") {
        // Empty, horizontal-rel cannot be anything
    }
    else {
        m_currentDrawStyle->addProperty("style:horizontal-rel", "paragraph");
    }

    readNext();
    READ_EPILOGUE
}

#endif // MSOOXMLVMLREADER_IMPL_H
