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

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "v"

//! Used by read_rect() to parse CSS2.
//! See ECMA-376 Part 4, 14.1.2.16, p.465.
//! @todo reuse KHTML parser?
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::parseCSS(const QString& style)
{
    m_vmlStyle.clear();
    foreach( const QString& pair, style.split(";", QString::SkipEmptyParts)) {
        const int splitIndex = pair.indexOf(":");
        if (splitIndex < 1)
            continue;
        const QByteArray name(pair.left(splitIndex).toLatin1().trimmed());
        QString value(pair.mid(splitIndex+1).trimmed());
        if (name.isEmpty())
            continue;
        if (value.startsWith("'") && value.endsWith("'"))
            value = value.mid(1, value.length() - 2); // strip ' '
        m_vmlStyle.insert(name, value);
        kDebug() << "name:" << name << "value:" << value;
    }
    return KoFilter::OK;
}

void MSOOXML_CURRENT_CLASS::createFrameStart()
{
    body->startElement("draw:frame");

    QString width(m_vmlStyle.value("width")); // already in "...cm" format
    QString height(m_vmlStyle.value("height")); // already in "...cm" format
    QString x_mar(m_vmlStyle.value("margin-left"));
    QString y_mar(m_vmlStyle.value("margin-top"));
    QString leftPos(m_vmlStyle.value("left"));
    QString topPos(m_vmlStyle.value("top"));
    const QString hor_pos(m_vmlStyle.value("mso-position-horizontal"));
    const QString ver_pos(m_vmlStyle.value("mso-position-vertical"));
    const QString hor_pos_rel(m_vmlStyle.value("mso-position-horizontal-relative"));
    const QString ver_pos_rel(m_vmlStyle.value("mso-position-vertical-relative"));

    if (!width.isEmpty()) {
        if (m_insideGroup) {
            width = QString("%1%2").arg(width.toInt() * m_real_groupWidth / m_groupWidth).arg(m_groupUnit);
        }
        body->addAttribute("svg:width", width);
    }
    if (!height.isEmpty()) {
        if (m_insideGroup) {
            height = QString("%1%2").arg(height.toInt() * m_real_groupHeight / m_groupHeight).arg(m_groupUnit);
        }
        body->addAttribute("svg:height", height);
    }
    if (!x_mar.isEmpty()) {
        if (m_insideGroup) {
            x_mar = QString("%1%2").arg((x_mar.toInt() - m_groupX) * m_real_groupWidth / m_groupWidth).arg(m_groupUnit);
        }
        body->addAttribute("svg:x", x_mar);
    }
    else if (!leftPos.isEmpty()) {
        if (m_insideGroup) {
            leftPos = QString("%1%2").arg((leftPos.toInt() - m_groupX) * m_real_groupWidth / m_groupWidth).arg(m_groupUnit);
        }
        body->addAttribute("svg:x", leftPos);
    }
    if (!y_mar.isEmpty()) {
        if (m_insideGroup) {
            y_mar = QString("%1%2").arg((y_mar.toInt() - m_groupY) * m_real_groupHeight / m_groupHeight).arg(m_groupUnit);
        }
        body->addAttribute("svg:y", y_mar);
    }
    else if (!topPos.isEmpty()) {
        if (m_insideGroup) {
            topPos = QString("%1%2").arg((topPos.toInt() - m_groupY) * m_real_groupHeight / m_groupHeight).arg(m_groupUnit);
        }
        body->addAttribute("svg:y", topPos);
    }
    if (!m_shapeColor.isEmpty()) {
        m_currentDrawStyle->addProperty("draw:fill", "solid");
        m_currentDrawStyle->addProperty("draw:fill-color", m_shapeColor);
    }

    if (!hor_pos.isEmpty()) {
        m_currentDrawStyle->addProperty("style:horizontal-pos", hor_pos);
    }
    if (!ver_pos.isEmpty()) {
        m_currentDrawStyle->addProperty("style:vertical-pos", ver_pos);
    }
    if (!hor_pos_rel.isEmpty()) {
        m_currentDrawStyle->addProperty("style:horizontal-rel", hor_pos_rel);
    }
    if (!ver_pos_rel.isEmpty()) {
        m_currentDrawStyle->addProperty("style:vertical-rel", ver_pos_rel);
    }

    if (!m_currentDrawStyle->isEmpty()) {
        const QString drawStyleName( mainStyles->insert(*m_currentDrawStyle, "gr") );
        if (m_moveToStylesXml) {
            mainStyles->markStyleForStylesXml(drawStyleName);
        }

        body->addAttribute("draw:style-name", drawStyleName);
    }
}

KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::createFrameEnd()
{
    if (!m_imagedataPath.isEmpty()) {
        body->startElement("draw:image");
        body->addAttribute("xlink:type", "simple");
        body->addAttribute("xlink:show", "embed");
        body->addAttribute("xlink:actuate", "onLoad");
        body->addAttribute("xlink:href", m_imagedataPath);
        body->endElement(); // draw:image
    }

    {
        {
            const QString rId(m_vmlStyle.value("v:fill@r:id"));
            if (!rId.isEmpty()) {
                body->startElement("draw:image");
                const QString sourceName(m_context->relationships->target(m_context->path, m_context->file, rId));
                kDebug() << "sourceName:" << sourceName;
                if (sourceName.isEmpty()) {
                    return KoFilter::FileNotFound;
                }

                QString destinationName = QLatin1String("Pictures/") + sourceName.mid(sourceName.lastIndexOf('/') + 1);;
                RETURN_IF_ERROR( m_context->import->copyFile(sourceName, destinationName, false ) )
                addManifestEntryForFile(destinationName);
                addManifestEntryForPicturesDir();
                body->addAttribute("xlink:href", destinationName);
                body->endElement(); //draw:image
            }
        }
    }

    body->endElement(); //draw:frame

    return KoFilter::OK;
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
 - shadow (Shadow Effect) §14.1.2.18
 - signatureline (Digital Signature Line) §14.2.2.30
 - skew (Skew Transform) §14.2.2.31
 - [done] stroke (Line Stroke Settings) §14.1.2.21
 - [done] textbox (Text Box) §14.1.2.22
 - textdata (VML Diagram Text) §14.5.2.2
 - textpath (Text Layout Path) §14.1.2.23
 - wrap (Text Wrapping) §14.3.2.6
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_rect()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(style)
    RETURN_IF_ERROR(parseCSS(style))

    TRY_READ_ATTR_WITHOUT_NS(fillcolor)
    TRY_READ_ATTR_WITHOUT_NS(strokecolor)
    TRY_READ_ATTR_WITHOUT_NS(strokeweight)

    m_strokeWidth = 1 ; // This seems to be the default

    if (!strokeweight.isEmpty()) {
        m_strokeWidth = strokeweight.left(strokeweight.length() - 2).toDouble(); // -2 removes 'pt'
    }

    m_shapeColor.clear();
    m_strokeColor.clear();

    if (!fillcolor.isEmpty()) {
        m_shapeColor = MSOOXML::Utils::rgbColor(fillcolor);
    }

    if (!strokecolor.isEmpty()) {
        m_strokeColor = MSOOXML::Utils::rgbColor(strokecolor);
    }

    MSOOXML::Utils::XmlWriteBuffer frameBuf;
    body = frameBuf.setWriter(body);

    pushCurrentDrawStyle(new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic"));

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(fill)
            ELSE_TRY_READ_IF(textbox)
            ELSE_TRY_READ_IF(stroke)
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    body = frameBuf.originalWriter();

    createFrameStart();

    (void)frameBuf.releaseWriter();

    createFrameEnd();

    popCurrentDrawStyle();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL stroke
//! Stroke style handler
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_stroke()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    m_currentPen = QPen();

    TRY_READ_ATTR_WITHOUT_NS(endcap)
    Qt::PenCapStyle penCap = m_currentPen.capStyle();
    if (endcap.isEmpty() || endcap == "sq") {
       penCap = Qt::SquareCap;
    }
    else if (endcap == "round") {
        penCap = Qt::RoundCap;
    }
    else if (endcap == "flat") {
        penCap = Qt::FlatCap;
    }
    m_currentPen.setCapStyle(penCap);
    m_currentPen.setWidthF(m_strokeWidth);
    m_currentPen.setColor(QColor(m_strokeColor));

    KoOdfGraphicStyles::saveOdfStrokeStyle(*m_currentDrawStyle, *mainStyles, m_currentPen);

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL group
//! Vml group handler
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_group()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(style)
    RETURN_IF_ERROR(parseCSS(style))

    body->startElement("draw:g");

    pushCurrentDrawStyle(new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic"));

    const QString hor_pos(m_vmlStyle.value("mso-position-horizontal"));
    const QString ver_pos(m_vmlStyle.value("mso-position-vertical"));
    const QString hor_pos_rel(m_vmlStyle.value("mso-position-horizontal-relative"));
    const QString ver_pos_rel(m_vmlStyle.value("mso-position-vertical-relative"));

    if (!hor_pos.isEmpty()) {
        m_currentDrawStyle->addProperty("style:horizontal-pos", hor_pos);
    }
    if (!ver_pos.isEmpty()) {
        m_currentDrawStyle->addProperty("style:vertical-pos", ver_pos);
    }
    if (!hor_pos_rel.isEmpty()) {
        m_currentDrawStyle->addProperty("style:horizontal-rel", hor_pos_rel);
    }
    if (!ver_pos_rel.isEmpty()) {
        m_currentDrawStyle->addProperty("style:vertical-rel", ver_pos_rel);
    }

    if (!m_currentDrawStyle->isEmpty()) {
        const QString drawStyleName( mainStyles->insert(*m_currentDrawStyle, "gr") );
        if (m_moveToStylesXml) {
            mainStyles->markStyleForStylesXml(drawStyleName);
        }

        body->addAttribute("draw:style-name", drawStyleName);
    }

    const QString width(m_vmlStyle.value("width"));
    const QString height(m_vmlStyle.value("height"));

    m_groupUnit = width.right(2); // pt, cm etc.
    m_real_groupWidth = width.left(width.length() - 2).toDouble();
    m_real_groupHeight = height.left(height.length() - 2).toDouble();

    m_groupX = 0;
    m_groupY = 0;
    m_groupWidth = 0;
    m_groupHeight = 0;

    TRY_READ_ATTR_WITHOUT_NS(coordsize)

    if (!coordsize.isEmpty()) {
        m_groupWidth = coordsize.mid(0, coordsize.indexOf(',')).toInt();
        m_groupHeight = coordsize.mid(coordsize.indexOf(',') + 1).toInt();
    }

    TRY_READ_ATTR_WITHOUT_NS(coordorigin)
    if (!coordorigin.isEmpty()) {
        m_groupX = coordorigin.mid(0, coordorigin.indexOf(',')).toInt();
        m_groupY = coordorigin.mid(coordorigin.indexOf(',') + 1).toInt();
    }

    m_insideGroup = true;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(rect)
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    body->endElement(); // draw:g

    m_insideGroup = false;

    popCurrentDrawStyle();

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
    const QXmlStreamAttributes attrs(attributes());
//! @todo support more attrs
    TRY_READ_ATTR_WITHOUT_NS(style)
    RETURN_IF_ERROR(parseCSS(style))

    pushCurrentDrawStyle(new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic"));

    MSOOXML::Utils::XmlWriteBuffer frameBuf;
    body = frameBuf.setWriter(body);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(fill)
            ELSE_TRY_READ_IF(textbox)
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    body = frameBuf.originalWriter();

    createFrameStart();

    (void)frameBuf.releaseWriter();

    createFrameEnd();

    popCurrentDrawStyle();

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
 - group (§14.1.2.7)
 - image (§14.1.2.10)
 - line (§14.1.2.12)
 - object (Part 1, §17.3.3.19)
 - oval (§14.1.2.13)
 - pict (§9.2.2.2)
 - pict (§9.5.1)
 - polyline (§14.1.2.15)
 - [done] rect (§14.1.2.16)
 - roundrect (§14.1.2.17)
 - shape (§14.1.2.19)
 - shapedefaults (§14.2.2.28)
 - shapetype (§14.1.2.20)

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
    m_vmlStyle.insert("v:fill@r:id", r_id);
    // The kind of fill. Default is solid.
    TRY_READ_ATTR_WITHOUT_NS(type)
    // frame (Stretch Image to Fit) - The image is stretched to fill the shape.
    // gradient (Linear Gradient) - The fill colors blend together in a linear gradient from bottom to top.
    // gradientRadial (Radial Gradient) - The fill colors blend together in a radial gradient.
    // pattern (Image Pattern) - The image is used to create a pattern using the fill colors.
    // tile (Tiled Image) - The fill image is tiled.
    // solid (Solid Fill) - The fill pattern is a solid color.

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
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
 - [Done]fill (Shape Fill Properties) §14.1.2.5

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
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(fill)
        }
    }
    const QString rId(m_vmlStyle.value("v:fill@r:id"));
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

#undef CURRENT_EL
#define CURRENT_EL shapetype
//! shapetype handler (Shape Template)
/*! ECMA-376 Part 4, 14.1.2.20, p.539.
 This element defines a shape template that can be used to create other shapes.
 Shapetype is identical to the shape element (§14.1.2.19) except it cannot reference another
 shapetype element.

 Parent elements:
 - background (Part 1, §17.2.1)
 - group (§14.1.2.7)
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
 - fill (Shape Fill Properties) §14.1.2.5
 - formulas (Set of Formulas) §14.1.2.6
 - handles (Set of Handles) §14.1.2.9
 - imagedata (Image Data) §14.1.2.11
 - lock (Shape Protections) §14.2.2.18
 - path (Shape Path) §14.1.2.14
 - shadow (Shadow Effect) §14.1.2.18
 - signatureline (Digital Signature Line) §14.2.2.30
 - skew (Skew Transform) §14.2.2.31
 - stroke (Line Stroke Settings) §14.1.2.21
 - textbox (Text Box) §14.1.2.22
 - textdata (VML Diagram Text) §14.5.2.2
 - textpath (Text Layout Path) §14.1.2.23
 - wrap (Text Wrapping) §14.3.2.6
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_shapetype()
{
    READ_PROLOGUE
    //const QXmlStreamAttributes attrs(attributes());
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
//            TRY_READ_IF()
        }
    }
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
 - fill (Shape Fill Properties) §14.1.2.5
 - formulas (Set of Formulas) §14.1.2.6
 - handles (Set of Handles) §14.1.2.9
 - [done] imagedata (Image Data) §14.1.2.11
 - ink (Ink) §14.2.2.15
 - iscomment (Ink Annotation Flag) §14.5.2.1
 - lock (Shape Protections) §14.2.2.18
 - path (Shape Path) §14.1.2.14
 - shadow (Shadow Effect) §14.1.2.18
 - signatureline (Digital Signature Line) §14.2.2.30
 - skew (Skew Transform) §14.2.2.31
 - [done] stroke (Line Stroke Settings) §14.1.2.21
 - [done] textbox (Text Box) §14.1.2.22
 - textdata (VML Diagram Text) §14.5.2.2
 - textpath (Text Layout Path) §14.1.2.23
 - wrap (Text Wrapping) §14.3.2.6
*/
//! @todo support all elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_shape()
{
    READ_PROLOGUE
/*    e.g. <v:shape id="_x0000_i1025" type="#_x0000_t75" style="width:166.5pt;height:124.5pt" o:ole="">
             <v:imagedata r:id="rId7" o:title=""/>
           </v:shape>*/
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(id)
    m_currentShapeId = id;

    // CSS2 styling properties of the shape, http://www.w3.org/TR/REC-CSS2
    TRY_READ_ATTR_WITHOUT_NS(style)
    RETURN_IF_ERROR(parseCSS(style))
    kDebug() << "m_vmlStyle:" << m_vmlStyle;
    // override width or height after 'object' element is possible
    const QString widthCm(MSOOXML::Utils::ST_PositiveUniversalMeasure_to_cm(m_vmlStyle.value("width")));
    if (!widthCm.isEmpty()) {
        m_currentObjectWidthCm = widthCm;
        kDebug() << "m_currentObjectWidthCm" << m_currentObjectWidthCm;
    }
    const QString heightCm(MSOOXML::Utils::ST_PositiveUniversalMeasure_to_cm(m_vmlStyle.value("height")));
    if (!heightCm.isEmpty()) {
        m_currentObjectHeightCm = heightCm;
    }
    if (!m_vmlStyle.value("margin-left").isEmpty()) {
        const QString xCmMar(MSOOXML::Utils::ST_PositiveUniversalMeasure_to_cm(m_vmlStyle.value("margin-left")));
        if (!xCmMar.isEmpty()) {
            m_currentObjectXCm = xCmMar;
        }
    }
    if (!m_vmlStyle.value("margin-top").isEmpty()) {
        const QString yCmMar(MSOOXML::Utils::ST_PositiveUniversalMeasure_to_cm(m_vmlStyle.value("margin-top")));
        if (!yCmMar.isEmpty()) {
            m_currentObjectYCm = yCmMar;
        }
    }
    // override x or y after 'object' element is possible
    const QString xCm(MSOOXML::Utils::ST_PositiveUniversalMeasure_to_cm(m_vmlStyle.value("left")));
    if (!xCm.isEmpty()) {
        m_currentObjectXCm = xCm;
    }
    const QString yCm(MSOOXML::Utils::ST_PositiveUniversalMeasure_to_cm(m_vmlStyle.value("top")));
    if (!yCm.isEmpty()) {
        m_currentObjectYCm = yCm;
    }
    //! @todo position (can be relative...)
    TRY_READ_ATTR_WITHOUT_NS_INTO(alt, m_shapeAltText)
    TRY_READ_ATTR_WITHOUT_NS_INTO(title, m_shapeTitle)
    TRY_READ_ATTR_WITHOUT_NS(fillcolor)
    TRY_READ_ATTR_WITHOUT_NS(strokecolor)
    TRY_READ_ATTR_WITHOUT_NS(strokeweight)

    m_strokeWidth = 1 ; // This seems to be the default

    if (!strokeweight.isEmpty()) {
        m_strokeWidth = strokeweight.left(strokeweight.length() - 2).toDouble(); // -2 removes 'pt'
    }

    m_shapeColor.clear();
    m_strokeColor.clear();

    if (!fillcolor.isEmpty()) {
        m_shapeColor = MSOOXML::Utils::rgbColor(fillcolor);
    }

    if (!strokecolor.isEmpty()) {
        m_strokeColor = MSOOXML::Utils::rgbColor(strokecolor);
    }

    MSOOXML::Utils::XmlWriteBuffer frameBuf;
    body = frameBuf.setWriter(body);

    pushCurrentDrawStyle(new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic"));

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(imagedata)
            ELSE_TRY_READ_IF(textbox)
            ELSE_TRY_READ_IF(stroke)
        }
    }

    body = frameBuf.originalWriter();

    if (m_outputFrames) {
        createFrameStart();
    }

    (void)frameBuf.releaseWriter();

    m_objectRectInitialized = true;

    if (m_outputFrames) {
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

        m_imagedataPath = QLatin1String("Pictures/") + imagedata.mid(imagedata.lastIndexOf('/') + 1);;
        RETURN_IF_ERROR( m_context->import->copyFile(imagedata, m_imagedataPath, false ) )
        addManifestEntryForFile(m_imagedataPath);
        m_imagedataFile = imagedata;
        addManifestEntryForPicturesDir();
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
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

    body->startElement("draw:text-box");

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
#ifdef DOCXXMLDOCREADER_CPP
            TRY_READ_IF_NS(w, txbxContent)
#endif
        }
    }

    body->endElement(); // draw-textbox

    READ_EPILOGUE
}


#endif // MSOOXMLVMLREADER_IMPL_H
