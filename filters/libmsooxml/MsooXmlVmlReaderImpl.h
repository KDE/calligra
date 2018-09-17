/*
 * This file is part of Office 2007 Filters for Calligra
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

//#define VMLREADER_DEBUG

//! Used by read_rect() to parse CSS2.
//! See ECMA-376 Part 4, 14.1.2.16, p.465.
//! @todo reuse KHTML parser?
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::parseCSS(const QString& style)
{
    m_currentVMLProperties.vmlStyle.clear();
    foreach( const QString& pair, style.split(';', QString::SkipEmptyParts)) {
        const int splitIndex = pair.indexOf(':');
        if (splitIndex < 1) {
            continue;
        }
        const QByteArray name(pair.left(splitIndex).toLatin1().trimmed());
        QString value(pair.mid(splitIndex+1).trimmed());
        if (name.isEmpty()) {
            continue;
        }
        if (value.startsWith(QLatin1Char('\'')) && value.endsWith(QLatin1Char('\''))) {
            value.remove(0, 1).chop(1);
        }
#ifdef VMLREADER_DEBUG
        debugMsooXml << "name:" << name << "value:" << value;
#endif
        m_currentVMLProperties.vmlStyle.insert(name, value);
    }
    return KoFilter::OK;
}

static void doPrependCheck(QString& checkedString) {
    if (!checkedString.isEmpty()) {
        if (checkedString.at(0) == '.') {
            checkedString.prepend("0");
        }
    }
}

static void changeToPoints(QString &value) {
    QString unit = value.right(2);
    if (unit == "pt") {
        return;
    }
    if (value == "0") {
        value = "0pt";
    }
    qreal number = value.left(value.size() - 2).toDouble();
    if (unit == "in") {
        number = number * 71;
    }
    else if (unit == "mm") {
        number = number * 56.6929130287 / 20.0;
    }
    else if (unit == "cm") {
        number = number * 566.929098146 / 20.0;
    }
    value = QString("%1pt").arg(number);
}

void MSOOXML_CURRENT_CLASS::createFrameStart(FrameStartElement startType)
{
    // Todo handle here all possible shape types
    if (startType == RectStart) {
        body->startElement("draw:rect");
    }
    else if (startType == EllipseStart) {
        body->startElement("draw:ellipse");
    }
    // Simplifying connector to be a line
    else if (startType == LineStart) {
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
    doPrependCheck(width);
    QString height(m_currentVMLProperties.vmlStyle.value("height")); // already in "...cm" format
    doPrependCheck(height);
    QString x_mar(m_currentVMLProperties.vmlStyle.value("margin-left"));
    doPrependCheck(x_mar);
    QString y_mar(m_currentVMLProperties.vmlStyle.value("margin-top"));
    doPrependCheck(y_mar);
    QString leftPos(m_currentVMLProperties.vmlStyle.value("left"));
    doPrependCheck(leftPos);
    QString topPos(m_currentVMLProperties.vmlStyle.value("top"));
    doPrependCheck(topPos);
    QString position(m_currentVMLProperties.vmlStyle.value("position"));
    QString hor_pos(m_currentVMLProperties.vmlStyle.value("mso-position-horizontal"));
    QString ver_pos(m_currentVMLProperties.vmlStyle.value("mso-position-vertical"));
    QString hor_pos_rel(m_currentVMLProperties.vmlStyle.value("mso-position-horizontal-relative"));
    QString ver_pos_rel(m_currentVMLProperties.vmlStyle.value("mso-position-vertical-relative"));
    const QString ver_align(m_currentVMLProperties.vmlStyle.value("v-text-anchor"));
    QString rotation(m_currentVMLProperties.vmlStyle.value("rotation"));
    const QString z_index(m_currentVMLProperties.vmlStyle.value("z-index"));
    qreal rotationAngle = 0;
    if (!rotation.isEmpty()) {
        if (rotation.endsWith(QLatin1String("fd"))) {
            rotationAngle = rotation.left(rotation.length()-2).toDouble() / 65536.0;
        }
        else {
            rotationAngle = rotation.toDouble();
        }
        rotationAngle = rotationAngle / 180.0 * M_PI;
    }

    qreal x_position = 0;
    QString x_pos_string, y_pos_string, widthString, heightString;
    qreal y_position = 0;
    qreal widthValue = 0;
    qreal heightValue = 0;

    /* NOTE: The default value of props. in {height, left, margin-*,
     * mso-wrp-distance-*, rotation, top, width, z-index} is 0. */
    QString *p_str = 0;

    //horizontal position
    if (m_currentVMLProperties.insideGroup) {
        if (!x_mar.isEmpty()) {
            x_position = (x_mar.toDouble() - m_currentVMLProperties.groupX) * m_currentVMLProperties.real_groupWidth /
                         m_currentVMLProperties.groupWidth + m_currentVMLProperties.groupXOffset;
            x_pos_string = QString("%1pt").arg(x_position);
        }
        else if (!leftPos.isEmpty()) {
            x_position = (leftPos.toDouble() - m_currentVMLProperties.groupX) * m_currentVMLProperties.real_groupWidth /
                         m_currentVMLProperties.groupWidth + m_currentVMLProperties.groupXOffset;
            x_pos_string = QString("%1pt").arg(x_position);
        } else {
            x_pos_string = QString("%1pt").arg(m_currentVMLProperties.groupXOffset);
        }

    } else {
        if (!x_mar.isEmpty()) {
            p_str = &x_mar;
        }
        else if (!leftPos.isEmpty()) {
            p_str = &leftPos;
        }
        //TODO: Add support for auto and percentage values.
        if (p_str && !(*p_str == "auto" || p_str->endsWith("%"))) {
            if (*p_str == "0") {
                p_str->append("in");
            }
            x_position = p_str->left(p_str->length() - 2).toDouble();
            x_pos_string = *p_str;
            p_str = 0;
        }
    }

    //vertical position
    if (m_currentVMLProperties.insideGroup) {
        if (!y_mar.isEmpty()) {
            y_position = (y_mar.toDouble() - m_currentVMLProperties.groupY) * m_currentVMLProperties.real_groupHeight /
                m_currentVMLProperties.groupHeight + m_currentVMLProperties.groupYOffset;
            y_pos_string = QString("%1pt").arg(y_position);
        }
        else if (!topPos.isEmpty()) {
            y_position = (topPos.toDouble() - m_currentVMLProperties.groupY) * m_currentVMLProperties.real_groupHeight /
                m_currentVMLProperties.groupHeight + m_currentVMLProperties.groupYOffset;
            y_pos_string = QString("%1pt").arg(y_position);
        } else {
            y_pos_string = QString("%1pt").arg(m_currentVMLProperties.groupYOffset);
        }
    } else {
        if (!y_mar.isEmpty()) {
            p_str = &y_mar;
        }
        else if (!topPos.isEmpty()) {
            p_str = &topPos;
        }
        //TODO: Add support for auto and percentage values.
        if (p_str && !(*p_str == "auto" || p_str->endsWith("%"))) {
            if (*p_str == "0") {
                p_str->append("in");
            }
            y_position = p_str->left(p_str->length() - 2).toDouble();
            y_pos_string = *p_str;
            p_str = 0;
        }
    }

    //width
    if (m_currentVMLProperties.insideGroup) {
        if (!width.isEmpty()) {
            widthValue = width.toDouble() * m_currentVMLProperties.real_groupWidth /
                         m_currentVMLProperties.groupWidth;
            widthString = QString("%1pt").arg(widthValue);
        }
    } else {
        if (width.isEmpty() || width == "0") {
            width = "0in";
        }
        //TODO: Add support for auto and percentage values.
        if (!(width == "auto" || width.endsWith('%'))) {
            widthValue = width.left(width.length() - 2).toDouble();
            widthString = width;
        }
    }

    //height
    if (m_currentVMLProperties.insideGroup) {
        if (!height.isEmpty()) {
            heightValue = height.toDouble() * m_currentVMLProperties.real_groupHeight /
                          m_currentVMLProperties.groupHeight;
            heightString = QString("%1pt").arg(heightValue);
        }
    } else {
        if (height.isEmpty() || height == "0") {
            height = "0in";
        }
        //TODO: Add support for auto and percentage values.
        if (!(height == "auto" || height.endsWith('%'))) {
            heightValue = height.left(height.length() - 2).toDouble();
            heightString = height;
        }
    }

    if (startType == LineStart) {
        QString flip(m_currentVMLProperties.vmlStyle.value("flip"));
        QString y1 = y_pos_string;
        // right(2) takes the unit
        QString x2 = QString("%1%2").arg(x_position + widthValue).arg(widthString.right(2));
        QString x1 = x_pos_string;
        QString y2 = QString("%1%2").arg(y_position + heightValue).arg(heightString.right(2));
        if (flip.contains('x')) {
            QString temp = y2;
            y2 = y1;
            y1 = temp;
        }
        if (flip.contains('y')) {
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
            if (rotationAngle != 0) {
                changeToPoints(widthString);
                changeToPoints(heightString);
                changeToPoints(x_pos_string);
                changeToPoints(y_pos_string);
                qreal width_rot = widthString.left(widthString.length()-2).toDouble();
                qreal height_rot = heightString.left(heightString.length()-2).toDouble();
                qreal left_rot = x_pos_string.left(x_pos_string.length()-2).toDouble();
                qreal top_rot = y_pos_string.left(y_pos_string.length()-2).toDouble();
                qreal xDiff = width_rot/2 - cos(rotationAngle)*width_rot/2 + sin(rotationAngle)*height_rot/2;
                qreal yDiff = height_rot/2 - sin(rotationAngle)*width_rot/2 - cos(rotationAngle)*height_rot/2;
                QString rotString = QString("rotate(%1) translate(%2pt %3pt)")
                                .arg(-rotationAngle).arg(left_rot + xDiff).arg(top_rot + yDiff);
                body->addAttribute("draw:transform", rotString);
            }
            else {
                if (!x_pos_string.isEmpty()) {
                    body->addAttribute("svg:x", x_pos_string);
                }
                if (!y_pos_string.isEmpty()) {
                    body->addAttribute("svg:y", y_pos_string);
                }
            }
            body->addAttribute("svg:width", widthString);
            body->addAttribute("svg:height", heightString);
        }
    }

    //TODO: VML allows negative numbers, ODF does NOT!  Using
    //automatic ordering in case of negative numbers temporary.
    if (!z_index.isEmpty() && z_index != "auto") {
        bool ok;
        const int n = z_index.toInt(&ok);
        if (!ok) {
            debugMsooXml << "error converting" << z_index << "to int (attribute z-index)";
        }
        else if (n >= 0) {
            body->addAttribute("draw:z-index", n);
        }
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
        else if (hor_pos_rel == "text") {
            hor_pos_rel = "paragraph";
        }
        if (!asChar) {
            m_currentDrawStyle->addProperty("style:horizontal-rel", hor_pos_rel);
        }
    }
#ifdef DOCXXMLDOCUMENTREADER_H
    if (!ver_pos_rel.isEmpty()) {
        if (ver_pos_rel == "margin" || ver_pos_rel == "line") {
            if (m_headerActive || m_footerActive) {
                m_currentDrawStyle->addProperty("style:vertical-rel", "frame-content");
            } else {
                m_currentDrawStyle->addProperty("style:vertical-rel", "page-content");
            }
        }
        else if (ver_pos_rel == "top-margin-area" || ver_pos_rel == "inner-margin-area" ||
                 ver_pos_rel == "outer-margin-area") {
            if (m_headerActive || m_footerActive) {
                m_currentDrawStyle->addProperty("style:vertical-rel", "frame");
            } else {
                m_currentDrawStyle->addProperty("style:vertical-rel", "page");
            }
        }
        else if (ver_pos_rel == "bottom-margin-area") {
            if (m_headerActive || m_footerActive) {
                m_currentDrawStyle->addProperty("style:vertical-rel", "frame");
            } else {
                m_currentDrawStyle->addProperty("style:vertical-rel", "page");
            }
            // This effectively emulates the bottom-margin-area
            m_currentDrawStyle->addProperty("style:vertical-pos", "bottom");
        }
        else {
            m_currentDrawStyle->addProperty("style:vertical-rel", ver_pos_rel);
        }
    }

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
        body->addAttribute("text:anchor-type", m_currentVMLProperties.anchorType.isEmpty() ? "char": m_currentVMLProperties.anchorType);
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

    if (m_currentVMLProperties.filled) {
        if (m_currentVMLProperties.fillType == "solid") {
            m_currentDrawStyle->addProperty("draw:fill", "solid");
            m_currentDrawStyle->addProperty("draw:fill-color", m_currentVMLProperties.shapeColor);
        }
        else if (m_currentVMLProperties.fillType == "gradient" || m_currentVMLProperties.fillType == "gradientRadial") {
            m_currentDrawStyle->addProperty("draw:fill", "gradient");
            m_currentDrawStyle->addProperty("draw:fill-gradient-name", m_currentVMLProperties.gradientStyle);
        }
        else if (m_currentVMLProperties.fillType == "picture" || m_currentVMLProperties.fillType == "pattern") {
            KoGenStyle fillStyle = KoGenStyle(KoGenStyle::FillImageStyle);
            fillStyle.addProperty("xlink:href", m_currentVMLProperties.imagedataPath);
            fillStyle.addProperty("xlink:type", "simple");
            fillStyle.addProperty("xlink:actuate", "onLoad");
            const QString imageName = mainStyles->insert(fillStyle);
            m_currentDrawStyle->addProperty("draw:fill", "bitmap");
            m_currentDrawStyle->addProperty("draw:fill-image-name", imageName);
            if (m_currentVMLProperties.fillType == "picture") {
                m_currentDrawStyle->addProperty("style:repeat", "stretch");
            }
            else {
                m_currentDrawStyle->addProperty("style:repeat", "repeat");
            }
        }
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

    // ------------------------------
    // shadow offset
    // ------------------------------
    QString offset = m_currentVMLProperties.shadowXOffset;
    if (offset.endsWith(QLatin1Char('%'))) {
        offset.chop(1);
        bool ok;
        int p = offset.toInt(&ok);
        if (!ok) {
            debugMsooXml << "error converting" << offset << "to int (shadow x-offset)";
        } else {
            offset = QString::number(p * widthValue / 100.0,'f').append(widthString.right(2));
        }
    }
    m_currentDrawStyle->addProperty("draw:shadow-offset-x", offset);

    offset = m_currentVMLProperties.shadowYOffset;
    if (offset.endsWith(QLatin1Char('%'))) {
        offset.chop(1);
        bool ok;
        int p = offset.toInt(&ok);
        if (!ok) {
            debugMsooXml << "error converting" << offset << "to int (shadow y-offset)";
        } else {
            offset = QString::number(p * heightValue / 100.0,'f').append(heightString.right(2));
        }
    }
    m_currentDrawStyle->addProperty("draw:shadow-offset-y", offset);
    // ------------------------------

    if (m_currentVMLProperties.shadowOpacity > 0) {
        m_currentDrawStyle->addProperty("draw:shadow-opacity", QString("%1%").
            arg(m_currentVMLProperties.shadowOpacity));
    }

    if (m_currentVMLProperties.opacity > 0) {
        m_currentDrawStyle->addProperty("draw:opacity", QString("%1%").arg(m_currentVMLProperties.opacity));
    }

    // FIXME: The draw:fit-to-size attribute specifies whether to stretch the
    // text content of a drawing object to fill an entire object.  The
    // style:shrink-to-fit attribute specifies whether content is reduced in
    // size to fit within a cell or drawing object.  Shrinking means that the
    // font size of the content is decreased to fit the content into a cell or
    // drawing object.  That's needed to be compatible with MS PowerPoint.  Any
    // margin, padding or indent MUST be retained.
    if (m_currentVMLProperties.fitTextToShape) {
        m_currentDrawStyle->addProperty("draw:fit-to-size", "true");
    }

    if (m_currentVMLProperties.fitShapeToText) {
        m_currentDrawStyle->addProperty("draw:auto-grow-height", "true");
        m_currentDrawStyle->addProperty("draw:auto-grow-width", "true");
    }

    // --------------------
    // padding
    // --------------------
#ifdef VMLREADER_DEBUG
    debugMsooXml << this << "padding-left:" << m_currentVMLProperties.internalMarginLeft;
    debugMsooXml << this << "padding-top:" << m_currentVMLProperties.internalMarginTop;
    debugMsooXml << this << "padding-right:" << m_currentVMLProperties.internalMarginRight;
    debugMsooXml << this << "padding-bottom:" << m_currentVMLProperties.internalMarginBottom;
#endif
    m_currentDrawStyle->addProperty("fo:padding-left", m_currentVMLProperties.internalMarginLeft);
    m_currentDrawStyle->addProperty("fo:padding-right", m_currentVMLProperties.internalMarginRight);
    m_currentDrawStyle->addProperty("fo:padding-top", m_currentVMLProperties.internalMarginTop);
    m_currentDrawStyle->addProperty("fo:padding-bottom", m_currentVMLProperties.internalMarginBottom);

    // --------------------
    // margins
    // --------------------
    QString *p_margin = &m_currentVMLProperties.marginLeft;
    if (m_currentVMLProperties.vmlStyle.contains("mso-wrap-distance-left")) {
        *p_margin = m_currentVMLProperties.vmlStyle.value("mso-wrap-distance-left");
        doPrependCheck(*p_margin);
        changeToPoints(*p_margin);
    }
    p_margin = &m_currentVMLProperties.marginTop;
    if (m_currentVMLProperties.vmlStyle.contains("mso-wrap-distance-top")) {
        *p_margin = m_currentVMLProperties.vmlStyle.value("mso-wrap-distance-top");
        doPrependCheck(*p_margin);
        changeToPoints(*p_margin);
    }
    p_margin = &m_currentVMLProperties.marginRight;
    if (m_currentVMLProperties.vmlStyle.contains("mso-wrap-distance-right")) {
        *p_margin = m_currentVMLProperties.vmlStyle.value("mso-wrap-distance-right");
        doPrependCheck(*p_margin);
        changeToPoints(*p_margin);
    }
    p_margin = &m_currentVMLProperties.marginBottom;
    if (m_currentVMLProperties.vmlStyle.contains("mso-wrap-distance-bottom")) {
        *p_margin = m_currentVMLProperties.vmlStyle.value("mso-wrap-distance-bottom");
        doPrependCheck(*p_margin);
        changeToPoints(*p_margin);
    }
    m_currentDrawStyle->addProperty("fo:margin-left", m_currentVMLProperties.marginLeft);
    m_currentDrawStyle->addProperty("fo:margin-top", m_currentVMLProperties.marginTop);
    m_currentDrawStyle->addProperty("fo:margin-right", m_currentVMLProperties.marginRight);
    m_currentDrawStyle->addProperty("fo:margin-bottom", m_currentVMLProperties.marginBottom);

    if (!m_currentDrawStyle->isEmpty()) {
        const QString drawStyleName(mainStyles->insert(*m_currentDrawStyle, "gr"));
        body->addAttribute("draw:style-name", drawStyleName);
    }
}

void MSOOXML_CURRENT_CLASS::takeDefaultValues()
{
    m_currentVMLProperties.modifiers.clear();
    m_currentVMLProperties.viewBox.clear();
    m_currentVMLProperties.shapePath.clear();
    m_currentVMLProperties.strokeColor = "#000000"; // default
    m_currentVMLProperties.strokeWidth = "1pt" ; // default
    m_currentVMLProperties.shapeColor = "#ffffff"; //default
    m_currentVMLProperties.fillType = "solid"; //default
    m_currentVMLProperties.shapeSecondaryColor = "#ffffff"; //default
    m_currentVMLProperties.lineCapStyle = "square";
    m_currentVMLProperties.joinStyle = "middle";
    m_currentVMLProperties.strokeStyleName.clear();
    m_currentVMLProperties.filled = true; // default
    m_currentVMLProperties.stroked = true; // default
    m_currentVMLProperties.opacity = 0; // default
    m_currentVMLProperties.shadowed = false;
    m_currentVMLProperties.shadowOpacity = 0; // default
    m_currentVMLProperties.shadowColor = "#101010"; // default
    m_currentVMLProperties.shadowXOffset = "2pt"; // default
    m_currentVMLProperties.shadowYOffset = "2pt"; //default
    m_currentVMLProperties.imagedataPath.clear();
    // default internal margins
    m_currentVMLProperties.internalMarginLeft = "0.1in";
    m_currentVMLProperties.internalMarginRight = "0.1in";
    m_currentVMLProperties.internalMarginTop = "0.05in";
    m_currentVMLProperties.internalMarginBottom = "0.05in";
    // default margins (according to MS Word UI NOT MS-ODRAW defaults)
    m_currentVMLProperties.marginLeft = "0.13in";
    m_currentVMLProperties.marginRight = "0.13in";
    m_currentVMLProperties.marginTop = "0in";
    m_currentVMLProperties.marginBottom = "0in";
    m_currentVMLProperties.fitTextToShape = false;
    m_currentVMLProperties.fitShapeToText = false;
}

QString MSOOXML_CURRENT_CLASS::rgbColor(QString color)
{
    QString extraArgument, argumentValue;
    int colorSeparator = color.indexOf(' ');
    if (colorSeparator > 0) {
        extraArgument = color.mid(colorSeparator + 1);
        color = color.left(colorSeparator);
        int startIndex = extraArgument.indexOf('(');
        if (startIndex > 0) {
            argumentValue = extraArgument.mid(startIndex + 1);
            argumentValue = argumentValue.left(argumentValue.length() - 1);
            extraArgument = extraArgument.left(startIndex);
        }
    }

    QString newColor;
    if (color.startsWith('#')) {
        QColor c(color); // use QColor parser to validate and/or correct color
        newColor = c.name();
    }
    else if (color == "red") {
        newColor = "#ff0000";
    }
    else if (color == "green") {
        newColor = "#008000";
    }
    else if (color == "blue") {
        newColor = "#0000ff";
    }
    else if (color == "yellow") {
        newColor = "#ffff00";
    }
    else if (color == "window") {
        newColor = "#ffffff"; // should ask from system
    }
    else if (color == "white") {
        newColor = "#ffffff";
    }
    else if (color == "black") {
        newColor = "#000000";
    }
    else if (color == "silver") {
        newColor = "#c0c0c0";
    }
    else if (color == "gray") {
        newColor = "#808080";
    }
    else if (color == "maroon") {
        newColor = "#800000";
    }
    else if (color == "purple") {
        newColor = "#800080";
    }
    else if (color == "fuchsia") {
        newColor = "#ff00ff";
    }
    else if (color == "lime") {
        newColor = "#00ff00";
    }
    else if (color == "olive") {
        newColor = "#808000";
    }
    else if (color == "navy") {
        newColor = "#000080";
    }
    else if (color == "teal") {
        newColor = "#008080";
    }
    else if (color == "aqua") {
        newColor = "#00ffff";
    }
    else if (color == "windowText") {
        newColor = "#000000"; // should ask from system
    }
    else if (color == "buttonFace") {
        newColor = "#808080"; // should ask from system
    }
    else if (color == "fill") { // referencing the other color
        newColor = m_currentVMLProperties.shapeColor;
    }
    else if (color == "line") {
        newColor = m_currentVMLProperties.strokeColor;
    }
    else if (color == "shadow") {
        newColor = m_currentVMLProperties.shadowColor;
    }
    else {
        // unhandled situation, means missing implementation
        newColor = color;
    }

    if (!argumentValue.isEmpty()) {
        int argument = argumentValue.toInt();
        QColor temp = newColor;
        int red = temp.red();
        int green = temp.green();
        int blue = temp.blue();
        if (extraArgument == "darken") {
            red = red * argument / 255;
            green = green * argument / 255;
            blue = blue * argument / 255;
        }
        else if (extraArgument == "lighten") {
            red = 255 - (255 - red) * argument / 255;
            green = 255 - (255 - green) * argument / 255;
            blue = 255 - (255 - blue) * argument / 255;
        }
        if (red > 255) {
            red = 255;
        }
        else if (red < 0) {
            red = 0;
        }
        if (green > 255) {
            green = 255;
        }
        else if (green < 0) {
            green = 0;
        }
        if (blue > 255) {
            blue = 255;
        }
        else if (blue < 0) {
            blue = 0;
        }
        newColor = QColor(red, green, blue).name();
    }

    return newColor;
}

void MSOOXML_CURRENT_CLASS::handleStrokeAndFill(const QXmlStreamAttributes& attrs)
{
    TRY_READ_ATTR_WITHOUT_NS(strokeweight)
    doPrependCheck(strokeweight);
    if (!strokeweight.isEmpty()) {
        m_currentVMLProperties.strokeWidth = strokeweight;
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
        m_currentVMLProperties.shapeColor = rgbColor(fillcolor);
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
        m_currentVMLProperties.strokeColor = rgbColor(strokecolor);
    }

    TRY_READ_ATTR_WITHOUT_NS(opacity)
    if (!opacity.isEmpty()) {
        if (opacity.right(1) == "f") {
            opacity = opacity.left(opacity.length() - 1);
            m_currentVMLProperties.opacity = 100.0 * opacity.toDouble() / 65536.0;
        }
        else {
            doPrependCheck(opacity);
            m_currentVMLProperties.opacity = 100.0 * opacity.toDouble();
        }
    }
}

#undef CURRENT_EL
#define CURRENT_EL line
//! line handler (Line)
/*
 Parent elements:
 - background (Part 1, §17.2.1)
 - [done] group (§14.1.2.7)
 - object (Part 1, §17.3.3.19)
 - [done] pict (§9.2.2.2)
 - [done] pict (§9.5.1)

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
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_line()
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

    READ_ATTR_WITHOUT_NS(from)
    READ_ATTR_WITHOUT_NS(to)

    // The transformations below are done in order to utilize group transformations
    // in createFrameStart function
    int index = from.indexOf(',');
    QString temp = from.left(index);
    doPrependCheck(temp);
    if (temp == "0") {
        temp = "0pt";
    }
    qreal fromX = temp.left(temp.size() - 2).toDouble();
    m_currentVMLProperties.vmlStyle["left"] = temp;
    temp = from.mid(index + 1);
    doPrependCheck(temp);
    if (temp == "0") {
        temp = "0pt";
    }
    qreal fromY = temp.left(temp.size() - 2).toDouble();
    m_currentVMLProperties.vmlStyle["top"] = temp;
    index = to.indexOf(',');
    temp = to.left(index);
    doPrependCheck(temp);
    if (temp == "0") {
        temp = "0pt";
    }
    QString unit = temp.right(2);
    qreal toX = temp.left(temp.size() - 2).toDouble() - fromX;
    m_currentVMLProperties.vmlStyle["width"] = QString("%1%2").arg(toX).arg(unit);
    temp = to.mid(index + 1);
    doPrependCheck(temp);
    if (temp == "0") {
        temp = "0pt";
    }
    unit = temp.right(2);
    qreal toY = temp.left(temp.size() - 2).toDouble() - fromY;
    m_currentVMLProperties.vmlStyle["height"] = QString("%1%2").arg(toY).arg(unit);

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

    createFrameStart(LineStart);

    (void)frameBuf.releaseWriter();

    body->endElement(); //draw:frame or draw:rect

    popCurrentDrawStyle();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL rect
//! rect handler (Rectangle)
/*! ECMA-376 Part 4, 14.1.2.16, p.449.
 This element is used to draw a simple rectangle.
 The CSS2 style content width and height define the width and height of the rectangle.

 Parent elements:
 - background (Part 1, §17.2.1)
 - [done] group (§14.1.2.7)
 - object (Part 1, §17.3.3.19)
 - [done] pict (§9.2.2.2)
 - [done] pict (§9.5.1)

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
 - [done] imagedata (Image Data) §14.1.2.11
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
            ELSE_TRY_READ_IF(imagedata)
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

    body->endElement(); //draw:frame or draw:rect

    popCurrentDrawStyle();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL shadow
//! Shadow handler
/*! ECMA-376 Part 4, 19.1.2.18, p.587.

 Parent elements:
 - arc (§14.1.2.1);
 - background (Part 1, §17.2.1);
 - curve (§14.1.2.3);
 - [done] group (§14.1.2.7);
 - image (§14.1.2.10);
 - [done] line (§14.1.2.12);
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
        m_currentVMLProperties.shadowColor = rgbColor(color);
    }

    TRY_READ_ATTR_WITHOUT_NS(offset)
    int index = offset.indexOf(',');
    if (index > 0) {
        if (offset.left(index) != "0") {
            m_currentVMLProperties.shadowXOffset = offset.left(index);
        }
        if (offset.mid(index + 1) != "0") {
            m_currentVMLProperties.shadowYOffset = offset.mid(index + 1);
        }
    }
    else if (offset == "0") {
        m_currentVMLProperties.shadowed = false;
    }

    TRY_READ_ATTR_WITHOUT_NS(opacity)
    if (!opacity.isEmpty()) {
        if (opacity.right(1) == "f") {
            opacity = opacity.left(opacity.length() - 1);
            m_currentVMLProperties.shadowOpacity = 100.0 * opacity.toDouble() / 65536.0;
        }
        else {
            doPrependCheck(opacity);
            m_currentVMLProperties.shadowOpacity = 100.0 * opacity.toDouble();
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
 - [done] line (§14.1.2.12);
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

    TRY_READ_ATTR_WITHOUT_NS(weight)
    doPrependCheck(weight);
    if (!weight.isEmpty()) {
        m_currentVMLProperties.strokeWidth = weight;
    }

    TRY_READ_ATTR_WITHOUT_NS(on)
    if (on == "f" || on == "false") {
        m_currentVMLProperties.stroked = false;
    }

    TRY_READ_ATTR_WITHOUT_NS(color)
    if (!color.isEmpty()) {
        m_currentVMLProperties.strokeColor = rgbColor(color);
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
 - [done] imagedata (Image Data) §14.1.2.11
 - [done] line (Line) §14.1.2.12
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
        QString width(m_currentVMLProperties.vmlStyle.value("width"));
        doPrependCheck(width);
        changeToPoints(width);
        QString height(m_currentVMLProperties.vmlStyle.value("height"));
        doPrependCheck(height);
        changeToPoints(height);

        // These potentially cause an offset to all shapes in the group
        // Unhandled case: theoretically x_mar could be in different units
        // than width, in this case they should be added somehow intelligently
        QString x_mar(m_currentVMLProperties.vmlStyle.value("margin-left"));
        doPrependCheck(x_mar);
        changeToPoints(x_mar);
        QString y_mar(m_currentVMLProperties.vmlStyle.value("margin-top"));
        doPrependCheck(y_mar);
        changeToPoints(y_mar);

        m_currentVMLProperties.real_groupWidth = width.left(width.length() - 2).toDouble();
        m_currentVMLProperties.real_groupHeight = height.left(height.length() - 2).toDouble();
        m_currentVMLProperties.groupXOffset = x_mar.left(x_mar.length() - 2).toDouble();
        m_currentVMLProperties.groupYOffset = y_mar.left(y_mar.length() - 2).toDouble();
    }
    else { // We are already in a group, this is a sub group, we're calculating new relative values for its children to use
        QString width(m_currentVMLProperties.vmlStyle.value("width"));
        QString height(m_currentVMLProperties.vmlStyle.value("height"));
        QString x_mar(m_currentVMLProperties.vmlStyle.value("left"));
        QString y_mar(m_currentVMLProperties.vmlStyle.value("top"));

        m_currentVMLProperties.groupXOffset = (x_mar.toDouble() - m_currentVMLProperties.groupX) * m_currentVMLProperties.real_groupWidth /
            m_currentVMLProperties.groupWidth + m_currentVMLProperties.groupXOffset;
        m_currentVMLProperties.groupYOffset = (y_mar.toDouble() - m_currentVMLProperties.groupY) * m_currentVMLProperties.real_groupHeight /
            m_currentVMLProperties.groupHeight + m_currentVMLProperties.groupYOffset;
        m_currentVMLProperties.real_groupWidth = width.toDouble() * m_currentVMLProperties.real_groupWidth / m_currentVMLProperties.groupWidth;
        m_currentVMLProperties.real_groupHeight = height.toDouble() * m_currentVMLProperties.real_groupHeight / m_currentVMLProperties.groupHeight;
    }

    m_currentVMLProperties.groupX = 0;
    m_currentVMLProperties.groupY = 0;
    m_currentVMLProperties.groupWidth = 1000; // default
    m_currentVMLProperties.groupHeight = 1000; //default

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
            else if (name() == "line") {
                m_VMLShapeStack.push(m_currentVMLProperties);
                m_currentVMLProperties.insideGroup = true;
                TRY_READ(line)
                m_currentVMLProperties = m_VMLShapeStack.pop();
            }
            ELSE_TRY_READ_IF(fill)
            ELSE_TRY_READ_IF(stroke)
            ELSE_TRY_READ_IF(shadow)
            ELSE_TRY_READ_IF(imagedata)
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

// Generic helper to help with draw:xxx shapes
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::genericReader(FrameStartElement startType)
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
            ELSE_TRY_READ_IF(imagedata)
            else if (qualifiedName() == "w10:wrap") {
                m_currentVMLProperties.wrapRead = true;
                TRY_READ(wrap)
            }
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    body = frameBuf.originalWriter();

    createFrameStart(startType);

    (void)frameBuf.releaseWriter();

    body->endElement(); //draw:frame or draw:rect

    popCurrentDrawStyle();

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL oval
//! oval handler (Oval)
// For parents, children, look from rect
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_oval()
{
    READ_PROLOGUE

    m_currentVMLProperties.currentEl = "v:oval";
    KoFilter::ConversionStatus status = genericReader(EllipseStart);
    if (status != KoFilter::OK) {
        return status;
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL roundrect
//! roundrect handler (Rounded rectangle)
// For parents, children, look from rect
// Note: this is atm. simplified, should in reality make a round rectangle
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_roundrect()
{
    READ_PROLOGUE

    m_currentVMLProperties.currentEl = "v:roundrect";
    KoFilter::ConversionStatus status = genericReader(RectStart);
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
 - [done] line (§14.1.2.12)
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
        m_currentVMLProperties.shapeColor = rgbColor(color);
    }

    TRY_READ_ATTR_WITHOUT_NS(color2)
    if (!color2.isEmpty()) {
        m_currentVMLProperties.shapeSecondaryColor = rgbColor(color2);
    }
    TRY_READ_ATTR_WITHOUT_NS(angle)
    TRY_READ_ATTR_WITHOUT_NS(colors)

    TRY_READ_ATTR_WITHOUT_NS(opacity)
    if (!opacity.isEmpty()) {
        if (opacity.right(1) == "f") {
            opacity = opacity.left(opacity.length() - 1);
            m_currentVMLProperties.opacity = 100.0 * opacity.toDouble() / 65536.0;
        }
        else {
            doPrependCheck(opacity);
            m_currentVMLProperties.opacity = 100.0 * opacity.toDouble();
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

    TRY_READ_ATTR_WITHOUT_NS(focusposition)

    TRY_READ_ATTR_WITHOUT_NS(type)
    if (!type.isEmpty()) {
        m_currentVMLProperties.fillType = type;
        if (type == "gradient") {
            m_currentGradientStyle = KoGenStyle(KoGenStyle::LinearGradientStyle);
            if (angle.isEmpty()) { // default
                m_currentGradientStyle.addAttribute("svg:x1", "50%");
                m_currentGradientStyle.addAttribute("svg:y1", "0%");
                m_currentGradientStyle.addAttribute("svg:x2", "50%");
                m_currentGradientStyle.addAttribute("svg:y2", "100%");
            }
            else {
                qreal angleReal = (90.0 + angle.toDouble()) * M_PI / 180.0;
                m_currentGradientStyle.addAttribute("svg:x1", QString("%1%").arg(50 - 50 * cos(angleReal)));
                m_currentGradientStyle.addAttribute("svg:y1", QString("%1%").arg(50 + 50 * sin(angleReal)));
                m_currentGradientStyle.addAttribute("svg:x2", QString("%1%").arg(50 + 50 * cos(angleReal)));
                m_currentGradientStyle.addAttribute("svg:y2", QString("%1%").arg(50 - 50 * sin(angleReal)));
            }
        }
        else if (type == "gradientRadial") {
            m_currentGradientStyle = KoGenStyle(KoGenStyle::RadialGradientStyle);
            if (focusposition.isEmpty()) {
                m_currentGradientStyle.addAttribute("svg:fx", QString("%1%").arg(0)); // default
                m_currentGradientStyle.addAttribute("svg:fy", QString("%1%").arg(0)); // default
            }
            else {
                int index = focusposition.indexOf(',');
                if (index > 0) {
                    QString first = focusposition.left(index);
                    doPrependCheck(first);
                    focusposition.remove(0, index + 1);
                    doPrependCheck(focusposition);
                    qreal fx = first.toDouble() * 100;
                    qreal fy = focusposition.toDouble() * 100;
                    m_currentGradientStyle.addAttribute("svg:fx", QString("%1%").arg(fx));
                    m_currentGradientStyle.addAttribute("svg:fy", QString("%1%").arg(fy));
                }
            }
            // defaulting so that gradient is always towards the center
            m_currentGradientStyle.addAttribute("svg:cx", QString("%1%").arg(50));
            m_currentGradientStyle.addAttribute("svg:cy", QString("%1%").arg(50));
            m_currentGradientStyle.addAttribute("svg:r", "80%"); ; // fix me if possible
        }
        else if (type == "frame") {
            m_currentVMLProperties.fillType = "picture";
        }
        else if (type == "tile" || type == "pattern") {
            m_currentVMLProperties.fillType = "pattern";
        }
        else {
            m_currentVMLProperties.fillType = "solid"; // defaulting
        }
        if (type == "gradientRadial" || type == "gradient") {
            if (colors.isEmpty()) {
                QString contents = QString("<svg:stop svg:offset=\"%1\" svg:stop-color=\"%2\" svg:stop-opacity=\"1\"/>").
                    arg(0).arg(m_currentVMLProperties.shapeColor);
                QString name = QString("%1").arg(1);
                m_currentGradientStyle.addChildElement(name, contents);
                contents = QString("<svg:stop svg:offset=\"%1\" svg:stop-color=\"%2\" svg:stop-opacity=\"1\"/>").
                    arg(1.0).arg(m_currentVMLProperties.shapeSecondaryColor);
                name = QString("%1").arg(2);
                m_currentGradientStyle.addChildElement(name, contents);
            }
            else {
                QList<QString> gradientColors;
                QList<qreal> gradientPositions;
                int index = colors.indexOf(';');
                bool lastRound = false;
                while (index > 0 || lastRound) {
                    QString gradientString = colors.left(index);
                    colors.remove(0, index + 1);
                    int spaceLocation = gradientString.indexOf(' ');
                    QString pos = gradientString.left(spaceLocation);
                    if (pos.right(1) == "f") {
                        pos = pos.left(pos.length() - 1);
                        gradientPositions.push_back(pos.toDouble() / 65536.0);
                    }
                    else {
                        doPrependCheck(pos);
                        gradientPositions.push_back(pos.toDouble());
                    }
                    gradientColors.push_back(rgbColor(gradientString.mid(spaceLocation + 1)));
                    if (lastRound) {
                        break;
                    }
                    index = colors.indexOf(';');
                    if (index < 0 ) {
                        lastRound = true;
                    }
                }
                index = 0;
                while (index < gradientPositions.size()) {
                    QString contents = QString("<svg:stop svg:offset=\"%1\" svg:stop-color=\"%2\" svg:stop-opacity=\"1\"/>").
                         arg(gradientPositions.at(index)).arg(gradientColors.at(index));
                    QString name = QString("%1").arg(index);
                    m_currentGradientStyle.addChildElement(name, contents);
                    ++index;
                }
            }
            m_currentVMLProperties.gradientStyle = mainStyles->insert(m_currentGradientStyle);
        }
    }

    TRY_READ_ATTR_WITH_NS(r, id)
    if (!r_id.isEmpty()) {
        const QString sourceName(m_context->relationships->target(m_context->path, m_context->file, r_id));
        m_currentVMLProperties.imagedataPath = QLatin1String("Pictures/") + sourceName.mid(sourceName.lastIndexOf('/') + 1);
        KoFilter::ConversionStatus status = m_context->import->copyFile(sourceName, m_currentVMLProperties.imagedataPath, false);
        if (status == KoFilter::OK) {
            addManifestEntryForFile(m_currentVMLProperties.imagedataPath);
            addManifestEntryForPicturesDir();
        }
        else {
            m_currentVMLProperties.fillType = "solid"; // defaulting
        }
    }

    // frame (Stretch Image to Fit) - The image is stretched to fill the shape.
    // gradient (Linear Gradient) - The fill colors blend together in a linear gradient from bottom to top.
    // gradientRadial (Radial Gradient) - The fill colors blend together in a radial gradient.
    // pattern (Image Pattern) - The image is used to create a pattern using the fill colors.
    // tile (Tiled Image) - The fill image is tiled.
    // solid (Solid Fill) - The fill pattern is a solid color.

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
        debugMsooXml << "sourceName:" << sourceName;
        if (sourceName.isEmpty()) {
            return KoFilter::FileNotFound;
        }
        QString destinationName = QLatin1String("Pictures/") + sourceName.mid(sourceName.lastIndexOf('/') + 1);
        RETURN_IF_ERROR( m_context->import->copyFile(sourceName, destinationName, false ) )
        addManifestEntryForFile(destinationName);
        addManifestEntryForPicturesDir();
        if (m_pDocBkgImageWriter) {
            delete m_pDocBkgImageWriter->device();
            delete m_pDocBkgImageWriter;
            m_pDocBkgImageWriter = nullptr;
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
    source.remove(0, index);
    return number;
}

static QString getArgument(QString& source, bool commaMeansZero, bool& wasCommand)
{
    wasCommand = false;
    if (source.at(0) == ',') {
        source.remove(0, 1);
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
        source.remove(0, 1);
        return QString("-%1").arg(getNumber(source));
    }
    if (source.at(0) == ',') { // case of 1,,2
        return "0";
    }
    if (source.at(0) == '#') {
        source.remove(0, 1);
        return QString("$%1").arg(getNumber(source));
    }
    if (source.at(0) == '@') {
        source.remove(0, 1);
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
            parsedString.remove(0, 1);
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
                parsedString.remove(0, 1);
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
                parsedString.remove(0, 1);
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
                parsedString.remove(0, 1);
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
                parsedString.remove(0, 1);
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
                extraShapeFormulas += "\n<draw:equation " +
                                      QString("draw:name=\"extraFormula%1\" draw:formula=\"").arg(extraFormulaIndex) +
                                      QString("%1 / 65536").arg(fifth) +
                                      "\" "
                                      "/>";
                ++extraFormulaIndex;
                eighth = QString("?extraFormula%1").arg(extraFormulaIndex);
                extraShapeFormulas += "\n<draw:equation " +
                                      QString("draw:name=\"extraFormula%1\" draw:formula=\"").arg(extraFormulaIndex) +
                                      QString("%1 / 65536").arg(sixth) +
                                      "\" "
                                      "/>";
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
                    extraShapeFormulas += "\n<draw:equation " +
                                          QString("draw:name=\"extraFormula%1\" draw:formula=\"").arg(extraFormulaIndex) +
                                          QString("%1 / 65536").arg(fifth) +
                                          "\" "
                                          "/>";
                    ++extraFormulaIndex;
                    eighth = QString("?extraFormula%1").arg(extraFormulaIndex);
                    extraShapeFormulas += "\n<draw:equation " +
                                          QString("draw:name=\"extraFormula%1\" draw:formula=\"").arg(extraFormulaIndex) +
                                          QString("%1 / 65536").arg(sixth) +
                                          "\" "
                                          "/>";
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
 - [done] imagedata (Image Data) §14.1.2.11
 - lock (Shape Protections) §14.2.2.18
 - [done] path (Shape Path) §14.1.2.14
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

    TRY_READ_ATTR_WITHOUT_NS(id)

    takeDefaultValues();
    handleStrokeAndFill(attrs);
    handlePathValues(attrs);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(formulas)
            ELSE_TRY_READ_IF(path)
            ELSE_TRY_READ_IF(shadow)
            ELSE_TRY_READ_IF(fill)
            ELSE_TRY_READ_IF(stroke)
            ELSE_TRY_READ_IF(imagedata)
            SKIP_UNKNOWN
        }
    }

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
    m_currentVMLProperties.normalFormulas.clear();

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
    m_currentVMLProperties.normalFormulas += "\n<draw:equation " +
                                             QString("draw:name=\"f%1\" draw:formula=\"").arg(m_currentVMLProperties.formulaIndex);

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
        eqn.remove(0, commandIndex + 1);
        QList<QString> parameters;
        while (true) {
            commandIndex = eqn.indexOf(' ');
            if (commandIndex < 0) {
                parameters.append(eqn);
                break;
            }
            parameters.append(eqn.left(commandIndex));
            eqn.remove(0, commandIndex + 1);
        }
        if (command == "val") {
            m_currentVMLProperties.normalFormulas += parameters.at(0);
        }
        else if (command == "sum") {
            m_currentVMLProperties.normalFormulas += parameters.at(0) + "+" + parameters.at(1) + "-" + parameters.at(2);
        }
        else if (command == "prod") {
            m_currentVMLProperties.normalFormulas += parameters.at(0) + "*" + parameters.at(1) + "/" + parameters.at(2);
        }
        else if (command == "abs") {
            m_currentVMLProperties.normalFormulas += QString("abs(%1)").arg(parameters.at(0));
        }
        else if (command == "min") {
            m_currentVMLProperties.normalFormulas += QString("min(%1,%2)").arg(parameters.at(0)).arg(parameters.at(1));
        }
        else if (command == "max") {
            m_currentVMLProperties.normalFormulas += QString("max(%1,%2)").arg(parameters.at(0)).arg(parameters.at(1));
        }
        else if (command == "if") {
            m_currentVMLProperties.normalFormulas += QString("if(%1,%2,%3)").arg(parameters.at(0)).arg(parameters.at(1)).arg(parameters.at(2));
        }
        else if (command == "sqrt") {
            m_currentVMLProperties.normalFormulas += QString("sqrt(%1)").arg(parameters.at(0));
        }
        else if (command == "mid") {
            m_currentVMLProperties.normalFormulas += QString("(%1+%2)/2").arg(parameters.at(0)).arg(parameters.at(1));
        }
        else if (command == "mod") {
            m_currentVMLProperties.normalFormulas += QString("sqrt(%1*%1+%2*%2+%3*%3)").arg(parameters.at(0)).arg(parameters.at(1)).arg(parameters.at(2));
        }
        else if (command == "ellipse") {
            m_currentVMLProperties.normalFormulas += QString("%3-sqrt(1-(%1/%2)*(%1/%2))").arg(parameters.at(0)).arg(parameters.at(1)).arg(parameters.at(2));
        }
        else if (command == "atan2") { // converting to fd unit (degrees * 65536)
            m_currentVMLProperties.normalFormulas += QString("3754936*atan2(%2,%1)").arg(parameters.at(0)).arg(parameters.at(1));
        }
        else if (command == "cosatan2") {
            m_currentVMLProperties.normalFormulas += QString("%1*cos(atan2(%3,%2))").arg(parameters.at(0)).arg(parameters.at(1)).arg(parameters.at(2));
        }
        else if (command == "sinatan2") {
            m_currentVMLProperties.normalFormulas += QString("%1*sin(atan2(%3,%2))").arg(parameters.at(0)).arg(parameters.at(1)).arg(parameters.at(2));
        }
        else if (command == "sumangle") {
            m_currentVMLProperties.normalFormulas += QString("%1+%2*65536-%3*65536").arg(parameters.at(0)).arg(parameters.at(1)).arg(parameters.at(2));
        }
        else if (command == "sin") { // converting fd unit to radians
            m_currentVMLProperties.normalFormulas += QString("%1*sin(%2 * 0.000000266)").arg(parameters.at(0)).arg(parameters.at(1));
        }
        else if (command == "cos") {
            m_currentVMLProperties.normalFormulas += QString("%1*cos(%2 * 0.000000266)").arg(parameters.at(0)).arg(parameters.at(1));
        }
        else if (command == "tan") {
            m_currentVMLProperties.normalFormulas += QString("%1*tan(%2 * 0.000000266)").arg(parameters.at(0)).arg(parameters.at(1));
        }
    }

    m_currentVMLProperties.normalFormulas += "\" "
                                             "/>";

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
 - [done] path (Shape Path) §14.1.2.14
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
        type.remove(0, 1); // removes extra # from the start
        // Inheriting all values from the template shape, except for group values
        // since it is possible that the template was declared outside the group
        bool _insideGroup = m_currentVMLProperties.insideGroup;
        int _groupWidth = m_currentVMLProperties.groupWidth;
        int _groupHeight = m_currentVMLProperties.groupHeight;
        int _groupX = m_currentVMLProperties.groupX;
        int _groupY = m_currentVMLProperties.groupY;
        qreal _groupXOffset = m_currentVMLProperties.groupXOffset;
        qreal _groupYOffset = m_currentVMLProperties.groupYOffset;
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
            m_currentVMLProperties.real_groupWidth = _real_groupWidth;
            m_currentVMLProperties.real_groupHeight = _real_groupHeight;
        }
    } else {
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
    debugMsooXml << "m_vmlStyle:" << m_currentVMLProperties.vmlStyle;

    TRY_READ_ATTR_WITHOUT_NS_INTO(alt, m_currentVMLProperties.shapeAltText)
    TRY_READ_ATTR_WITHOUT_NS_INTO(title, m_currentVMLProperties.shapeTitle)

    handlePathValues(attrs);

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
            TRY_READ_IF(imagedata)
            //TODO: represent as draw:frame/draw:text-box
            ELSE_TRY_READ_IF(textbox)
            ELSE_TRY_READ_IF(stroke)
            ELSE_TRY_READ_IF(fill)
            ELSE_TRY_READ_IF(shadow)
            else if (qualifiedName() == "w10:wrap") {
                m_currentVMLProperties.wrapRead = true;
                TRY_READ(wrap)
            }
            ELSE_TRY_READ_IF(formulas)
            ELSE_TRY_READ_IF(path)
            SKIP_UNKNOWN
        }
    }

    body = frameBuf.originalWriter();
    bool makeFrameInstead = false;

    // Checking for a special case where there is a picture and the shape is a rectangle
    // In this case we draw a simple frame, this because calligra atm. does not support
    // wmf/emf pictures are the background fill of an element
    if (!m_currentVMLProperties.imagedataPath.isEmpty() &&
        m_currentVMLProperties.shapePath == " M ?f4 ?f5 L ?f4 ?f11 ?f9 ?f11 ?f9 ?f5 Z N") {
        m_currentVMLProperties.filled = false;
        makeFrameInstead = true;
    }

    if (m_outputFrames) {
        if (makeFrameInstead) {
            createFrameStart();
        }
        else {
            createFrameStart(CustomStart);
        }
    }

    (void)frameBuf.releaseWriter();

    if (m_outputFrames) {
        if (makeFrameInstead) {
            body->startElement("draw:image");
            body->addAttribute("xlink:type", "simple");
            body->addAttribute("xlink:show", "embed");
            body->addAttribute("xlink:actuate", "onLoad");
            body->addAttribute("xlink:href", m_currentVMLProperties.imagedataPath);
            body->endElement(); // draw:image
        }
        else {
            m_currentVMLProperties.shapeTypeString = "<draw:enhanced-geometry ";

            QString flip(m_currentVMLProperties.vmlStyle.value("flip"));
            if (flip.contains('x')) {
                m_currentVMLProperties.shapeTypeString += "draw:mirror-vertical=\"true\" ";
            }
            if (flip.contains('y')) {
                m_currentVMLProperties.shapeTypeString += "draw:mirror-horizontal=\"true\" ";
            }
            m_currentVMLProperties.shapeTypeString +=
                QString("draw:modifiers=\"%1\" ").arg(m_currentVMLProperties.modifiers) +
                QString("svg:viewBox=\"%1\" ").arg(m_currentVMLProperties.viewBox) +
                QString("draw:enhanced-path=\"%1\" ").arg(m_currentVMLProperties.shapePath) +
                QLatin1Char('>') +
                m_currentVMLProperties.extraShapeFormulas +
                m_currentVMLProperties.normalFormulas +
                "</draw:enhanced-geometry>";

            body->addCompleteElement(m_currentVMLProperties.shapeTypeString.toUtf8());
        }
        body->endElement(); //draw:frame or draw:custom-shape
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
 - [done] group (§14.1.2.7)
 - image (§14.1.2.10)
 - line (§14.1.2.12)
 - object (Part 1, §17.3.3.19)
 - oval (§14.1.2.13)
 - pict (§9.2.2.2)
 - pict (§9.5.1)
 - polyline (§14.1.2.15)
 - [done] rect (§14.1.2.16)
 - [done] roundrect (§14.1.2.17)
 - [done] shape (§14.1.2.19)
 - [done] shapetype (§14.1.2.20)

 Child elements:
 - none

*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_imagedata()
{
    READ_PROLOGUE

    m_currentVMLProperties.filled = true;

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

    debugMsooXml << "imagedata:" << imagedata;
    if (!imagedata.isEmpty()) {
        m_currentVMLProperties.imagedataPath = QLatin1String("Pictures/") + imagedata.mid(imagedata.lastIndexOf('/') + 1);
        KoFilter::ConversionStatus status = m_context->import->copyFile(imagedata, m_currentVMLProperties.imagedataPath, false);
        if (status == KoFilter::OK) {
            addManifestEntryForFile(m_currentVMLProperties.imagedataPath);
            addManifestEntryForPicturesDir();
            m_currentVMLProperties.fillType = "picture";
        }
        else {
            m_currentVMLProperties.fillType = "solid"; // defaulting
        }
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
    VMLShapeProperties oldProperties = m_currentVMLProperties;
    TRY_READ_ATTR_WITHOUT_NS(style)
    RETURN_IF_ERROR(parseCSS(style))

    if (m_currentVMLProperties.vmlStyle.value("mso-fit-shape-to-text") == "t") {
        oldProperties.fitShapeToText = true;
    }
    if (m_currentVMLProperties.vmlStyle.value("mso-fit-text-to-shape") == "t") {
        oldProperties.fitTextToShape = true;
    }

    // In below code, the else clauses are needed for cases that not
    // all insets are defined
    TRY_READ_ATTR_WITHOUT_NS(inset)
    if (!inset.isEmpty()) {
        doPrependCheck(inset);
        inset.replace(",,", ",d,"); //Default
        int index = inset.indexOf(',');
        if (index > 0) {
            QString str = inset.left(index);
            if (str != "d") {
                if (str == "0") {
                    str.append("in");
                }
                oldProperties.internalMarginLeft = str;
            }
            inset.remove(0, index + 1);
            doPrependCheck(inset);
            index = inset.indexOf(',');
            if (index > 0) {
                str = inset.left(index);
                if (str != "d") {
                    if (str == "0") {
                        str.append("in");
                    }
                    oldProperties.internalMarginTop = str;
                }
                inset.remove(0, index + 1);
                doPrependCheck(inset);
                index = inset.indexOf(',');
                if (index > 0) {
                    str = inset.left(index);
                    if (str != "d") {
                        if (str == "0") {
                            str.append("in");
                        }
                        oldProperties.internalMarginRight = str;
                    }
                    str = inset.mid(index + 1);
                    if (str != "d") {
                        if (str == "0") {
                            str.append("in");
                        }
                        oldProperties.internalMarginBottom = str;
                        doPrependCheck(oldProperties.internalMarginBottom);
                    }
                } else {
                    str = inset.left(index);
                    if (str != "d") {
                        if (str == "0") {
                            str.append("in");
                        }
                        oldProperties.internalMarginRight = str;
                    }
                }
            } else {
                str = inset.left(index);
                if (str != "d") {
                    if (str == "0") {
                        str.append("in");
                    }
                    oldProperties.internalMarginTop = str;
                }
            }
        }
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
#ifdef DOCXXMLDOCREADER_CPP
            TRY_READ_IF_NS(w, txbxContent)
#endif
        }
    }

    m_currentVMLProperties = oldProperties;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL path
/*! path handler (Shape path)

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
 - [done] shape (§14.1.2.19);
 - [done] shapetype (§14.1.2.20)

 Child elements:
 - none
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_path()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(shadowok)
    if (shadowok == "f" || shadowok == "false") {
        m_currentVMLProperties.shadowed = false;
    }

    TRY_READ_ATTR_WITHOUT_NS(fillok)
    if (fillok == "f" || fillok == "false") {
        m_currentVMLProperties.filled = false;
    }

    TRY_READ_ATTR_WITHOUT_NS(strokeok)
    if (strokeok == "f" || strokeok == "false") {
        m_currentVMLProperties.stroked = false;
    }

    TRY_READ_ATTR_WITHOUT_NS(v)
    if (!v.isEmpty()) {
        m_currentVMLProperties.extraShapeFormulas.clear();
        m_currentVMLProperties.shapePath = convertToEnhancedPath(v, m_currentVMLProperties.extraShapeFormulas);
    }

    readNext();

    READ_EPILOGUE
}

void MSOOXML_CURRENT_CLASS::handlePathValues(const QXmlStreamAttributes& attrs)
{
    TRY_READ_ATTR_WITHOUT_NS(adj)
    if (!adj.isEmpty()) {
        QString tempModifiers = adj;
        doPrependCheck(tempModifiers);
        tempModifiers.replace(",,", ",0,");
        tempModifiers.replace(',', ' ');
        m_currentVMLProperties.modifiers = tempModifiers;
    }

    TRY_READ_ATTR_WITHOUT_NS(coordsize)
    if (!coordsize.isEmpty()) {
        QString tempViewBox = "0 0 " + coordsize;
        tempViewBox.replace(',', ' ');
        m_currentVMLProperties.viewBox = tempViewBox;
    }

    TRY_READ_ATTR_WITHOUT_NS(path)
    if (!path.isEmpty()) {
        m_currentVMLProperties.extraShapeFormulas.clear();
        m_currentVMLProperties.shapePath = convertToEnhancedPath(path, m_currentVMLProperties.extraShapeFormulas);
    }
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
        if (type == "square" || type == "tight") {
            m_currentDrawStyle->addProperty("style:wrap-contour-mode", "outside");
            m_currentDrawStyle->addProperty("style:wrap-contour", "false");
        }
        else {
            m_currentDrawStyle->addProperty("style:wrap-contour-mode", "full");
            m_currentDrawStyle->addProperty("style:wrap-contour", "true");
        }
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

#ifdef DOCXMLDOCREADER_H
    if (anchory == "page") {
        if (m_headerActive || m_footerActive) {
            m_currentDrawStyle->addProperty("style:vertical-rel", "frame");
            m_currentVMLProperties.anchorType = "frame";
        } else {
            m_currentDrawStyle->addProperty("style:vertical-rel", "page");
            m_currentVMLProperties.anchorType = "page";
        }
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
#endif

    readNext();
    READ_EPILOGUE
}

#endif // MSOOXMLVMLREADER_IMPL_H
