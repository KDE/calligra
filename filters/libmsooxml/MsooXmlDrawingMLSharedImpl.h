/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * Copyright (C) 2012 Matus Uzak (matus.uzak@gmail.com).
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

#ifndef MSOOXML_DRAWINGML_SHARED_IMPL_H
#define MSOOXML_DRAWINGML_SHARED_IMPL_H

#undef CURRENT_EL
#define CURRENT_EL extLst
//! extLst (Extension List)
//! DrawingML ECMA-376 20.1.2.2.15, p.3034
/*!
  This element specifies the extension list within which all future
  extensions of element type ext is defined. The extension list along
  with corresponding future extensions is used to extend the storage
  capabilities of the DrawingML framework. This allows for various new
  types of data to be stored natively within the framework.
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_extLst()
{
    READ_PROLOGUE
    // NO support atm, skipping
    skipCurrentElement();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lnL
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_Table_lnL()
{
    READ_PROLOGUE2(Table_lnL)

    return read_Table_generic("lnL");

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lnR
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_Table_lnR()
{
    READ_PROLOGUE2(Table_lnR)

    return read_Table_generic("lnR");

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lnT
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_Table_lnT()
{
    READ_PROLOGUE2(Table_lnT)

    return read_Table_generic("lnT");

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lnB
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_Table_lnB()
{
    READ_PROLOGUE2(Table_lnB)

    return read_Table_generic("lnB");

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ln
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_Table_ln()
{
    READ_PROLOGUE2(Table_ln)

    return read_Table_generic("ln");

    READ_EPILOGUE
}

KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_Table_generic(const QString& endElement)
{
    QXmlStreamAttributes attrs = attributes();

    m_currentBorder = KoBorder::BorderData();

    //compound line type
    TRY_READ_ATTR_WITHOUT_NS(cmpd)
    //double lines
    if( cmpd.isEmpty() || cmpd == "sng" ) {
        m_currentBorder.style = KoBorder::BorderSolid;
    }
    //single line
    else if (cmpd == "dbl") {
        m_currentBorder.style = KoBorder::BorderDouble;
    }
    //thick thin double lines
    else if (cmpd == "thickThin") {
        //FIXME it seem we don't support this properly. Use solid for now.
        m_currentBorder.style = KoBorder::BorderDouble;
    }
    //thin thick double lines
    else if (cmpd == "thinThick") {
        //FIXME it doesn't seem we support this properly.
        m_currentBorder.style = KoBorder::BorderDouble;
    }
    //thin thick thin triple lines
    else if (cmpd == "tri") {
        //NOTE: There is not triple in ODF
        m_currentBorder.style = KoBorder::BorderSolid;
    }

    TRY_READ_ATTR_WITHOUT_NS(w) //width
    m_currentBorder.outerPen.setWidthF(EMU_TO_POINT(w.toDouble()));

    while (!atEnd()) {
        readNext();
        if (isEndElement() && name() == endElement) {
            break;
        }
        if(isStartElement()) {
            if(QUALIFIED_NAME_IS(solidFill)) {
                TRY_READ(solidFill);
                m_currentBorder.style = KoBorder::BorderSolid;
                m_currentBorder.innerPen.setColor(m_currentColor);
                m_currentBorder.outerPen.setColor(m_currentColor);
            }
            else if (QUALIFIED_NAME_IS(prstDash)) {
                attrs = attributes();
                //TODO find out how other colors are handled
                m_currentBorder.innerPen.setColor(Qt::black);
                m_currentBorder.outerPen.setColor(Qt::black);
                TRY_READ_ATTR_WITHOUT_NS(val)
                //TODO support other dash types. Make it its own function.
                if (val == "dash") {
                    m_currentBorder.style = KoBorder::BorderDashed;
                }
                else if(val == "dashDot") {
                    m_currentBorder.style = KoBorder::BorderDashDot;
                }
                else if(val == "dot") {
                    m_currentBorder.style = KoBorder::BorderDotted;
                }
            }
            SKIP_UNKNOWN
//             ELSE_WRONG_FORMAT
        }
    }
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL ln
//! ln (Outline)
//! DrawingML ECMA-376, 20.1.2.2.24, p. 3048.
/*!
 This element specifies an outline style that can be applied to a
 number of different objects such as shapes and text. The line allows
 for the specifying of many different types of outlines including
 even line dashes and bevels.

 Parent Elements:

 Child Elements:
 - [done] bevel (Line Join Bevel) §20.1.8.9
 - custDash (Custom Dash) §20.1.8.21
 - extLst (Extension List) §20.1.2.2.15
 - gradFill (Gradient Fill) §20.1.8.33
 - [done] headEnd (Line Head/End Style) §20.1.8.38
 - [done] miter (Miter Line Join) §20.1.8.43
 - [done] noFill (No Fill) §20.1.8.44
 - pattFill (Pattern Fill) §20.1.8.47
 - [done] prstDash (Preset Dash) §20.1.8.48
 - [done] round (Round Line Join) §20.1.8.52
 - [done] solidFill (Solid Fill) §20.1.8.54
 - [done] tailEnd (Tail line end style) §20.1.8.57

 Attributes:
 - algn (Stroke Alignment)
 - [done] cap (Line Ending Cap Style)
 - cmpd (Compound Line Type)
 - [done] w (Line Width)
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_ln()
{
    READ_PROLOGUE
    QXmlStreamAttributes attrs(attributes());

    //TODO: align
    TRY_READ_ATTR_WITHOUT_NS(algn)
    //center
    if (algn.isEmpty() || algn == "ctr") {
    }
    //inset
    else if (algn == "in") {
    }

    //line ending cap
    TRY_READ_ATTR_WITHOUT_NS(cap)
    if (cap.isEmpty() || cap == "sq") {
       m_currentDrawStyle->addProperty("svg:stroke-linecap", "square");
    }
    else if (cap == "rnd") {
        m_currentDrawStyle->addProperty("svg:stroke-linecap", "round");
    }
    else if (cap == "flat") {
        m_currentDrawStyle->addProperty("svg:stroke-linecap", "butt");
    }

    //TODO: compound line type
    TRY_READ_ATTR_WITHOUT_NS(cmpd)

    //single line
    if( cmpd.isEmpty() || cmpd == "sng" ) {
    }
    //double lines
    else if (cmpd == "dbl") {
    }
    //thick thin double lines
    else if (cmpd == "thickThin") {
    }
    //thin thick double lines
    else if (cmpd == "thinThick") {
    }
    //thin thick thin triple lines
    else if (cmpd == "tri") {
    }

    TRY_READ_ATTR_WITHOUT_NS(w)

    if (!w.isEmpty()) {
        m_currentLineWidth = EMU_TO_POINT(w.toDouble());
        m_currentDrawStyle->addPropertyPt("svg:stroke-width", m_currentLineWidth);
        m_currentDrawStyle->addProperty("draw:stroke", "solid");
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if ( isStartElement() ) {

            // line head/tail end
            TRY_READ_IF(headEnd)
            ELSE_TRY_READ_IF(tailEnd)

            // linejoin
            else if (qualifiedName() == QLatin1String("a:bevel")) {
                m_currentDrawStyle->addProperty("draw:stroke-linejoin", "bevel");
            }
            else if (qualifiedName() == QLatin1String("a:miter")) {
                m_currentDrawStyle->addProperty("draw:stroke-linejoin", "miter");
            }
            else if (qualifiedName() == QLatin1String("a:round")) {
                m_currentDrawStyle->addProperty("draw:stroke-linejoin", "round");
            }

            // //custom dash
            // else if(qualifiedName() == QLatin1String("a:custDash")) {
            // }
            // //extension list
            // else if(qualifiedName() == QLatin1String("a:extLst")) {
            // }
            // //gradient fill
            // else if(qualifiedName() == QLatin1String("a:gradFill")) {
            // }

            // //pattern fill
            // else if(qualifiedName() == QLatin1String("a:pattFill")) {
            // }

            //solid fill
            else if (qualifiedName() == QLatin1String("a:solidFill")) {
                TRY_READ(solidFill)
                m_currentDrawStyle->addProperty("svg:stroke-color", m_currentColor.name());

                // Opacity is disabled because there's a bug somewhere
                // which makes even 1% opacity hide lines.

                // if (m_currentAlpha > 0) {
                //     m_currentDrawStyle->addProperty("svg:stroke-opacity",
                //     QString("%1%").arg(m_currentAlpha/100.0));
                // }
            }
            else if (qualifiedName() == QLatin1String("a:noFill")) {
                m_currentDrawStyle->addProperty("draw:stroke", "none");
            }
            else if (qualifiedName() == QLatin1String("a:prstDash")) {
                attrs = attributes();
                TRY_READ_ATTR_WITHOUT_NS(val)
                QPen pen;
                pen.setWidthF(m_currentLineWidth);
                if (val == "dash") {
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
                    QString dashStyleName = mainStyles->insert(dashStyle, "dash");
                    m_currentDrawStyle->addProperty("draw:stroke-dash", dashStyleName);
                    // NOTE: A "dash" looks wrong in Calligra/LO when
                    // svg:stroke-linecap is applied.
                    m_currentDrawStyle->removeProperty("svg:stroke-linecap");
                }
            }
            SKIP_UNKNOWN
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL solidFill
//! solidFill - Solid Fill
/*! DrawingML ECMA-376 20.1.8.54, p. 3234.

  This element specifies a solid color fill.  The shape is filled entirely with
  the specified color.

 Parents:
    - bg (§21.4.3.1)
    - bgFillStyleLst (§20.1.4.1.7)
    - bgPr (§19.3.1.2)
    - defRPr (§21.1.2.3.2)
    - endParaRPr (§21.1.2.2.3)
    - fill (§20.1.8.28)
    - fill (§20.1.4.2.9)
    - fillOverlay (§20.1.8.29)
    - fillStyleLst (§20.1.4.1.13)
    - grpSpPr (§21.3.2.14)
    - grpSpPr (§20.1.2.2.22)
    - grpSpPr (§20.5.2.18)
    - grpSpPr (§19.3.1.23)
    - ln (§20.1.2.2.24)
    - lnB (§21.1.3.5)
    - lnBlToTr (§21.1.3.6)
    - lnL (§21.1.3.7)
    - lnR (§21.1.3.8)
    - lnT (§21.1.3.9)
    - lnTlToBr (§21.1.3.10)
    - [done] rPr (§21.1.2.3.9)
    - spPr (§21.2.2.197)
    - spPr (§21.3.2.23)
    - spPr (§21.4.3.7)
    - [done] spPr (§20.1.2.2.35)
    - spPr (§20.2.2.6)
    - spPr (§20.5.2.30)
    - [done] spPr (§19.3.1.44)
    - tblPr (§21.1.3.15)
    - tcPr (§21.1.3.17)
    - uFill (§21.1.2.3.12)
    - uLn (§21.1.2.3.14)

 Child elements:
    - [done] hslClr (Hue, Saturation, Luminance Color Model) §20.1.2.3.13
    - [done] prstClr (Preset Color) §20.1.2.3.22
    - [done] schemeClr (Scheme Color) §20.1.2.3.29
    - [done] scrgbClr (RGB Color Model - Percentage Variant) §20.1.2.3.30
    - [done] srgbClr (RGB Color Model - Hex Variant) §20.1.2.3.32
    - [done] sysClr (System Color) §20.1.2.3.33

 Attributes:
    None.
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_solidFill()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        debugMsooXml << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(scrgbClr)
            ELSE_TRY_READ_IF(schemeClr)
            ELSE_TRY_READ_IF(srgbClr)
            ELSE_TRY_READ_IF(sysClr)
            ELSE_TRY_READ_IF(prstClr)
            ELSE_TRY_READ_IF(hslClr)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

//! fillREf handler (Fill reference)
/*
 Parent elements:
 - [done] style (§21.3.2.24);
 - [done] style (§21.4.2.28);
 - [done] style (§20.1.2.2.37);
 - [done] style (§20.5.2.31);
 - [done] style (§19.3.1.46);
 - [done] tblBg (§20.1.4.2.25);
 - [done] tcStyle (§20.1.4.2.29)

 Child elements:
 - [done] hslClr (Hue, Saturation, Luminance Color Model) §20.1.2.3.13
 - [done] prstClr (Preset Color) §20.1.2.3.22
 - [done] schemeClr (Scheme Color) §20.1.2.3.29
 - [done] scrgbClr (RGB Color Model - Percentage Variant) §20.1.2.3.30
 - [done] srgbClr (RGB Color Model - Hex Variant) §20.1.2.3.32
 - [done] sysClr (System Color) §20.1.2.3.33

*/
#if !defined MSOOXML_THEMESREADER_CPP
#undef CURRENT_EL
#define CURRENT_EL fillRef
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_fillRef()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(idx)
    int index = idx.toInt();

    // If it has draw:fill it means that the style has already been defined
    if (!m_currentDrawStyle->property("draw:fill").isEmpty()) {
        skipCurrentElement();
        READ_EPILOGUE
    }

    while (!atEnd()) {
        readNext();
        debugMsooXml << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(schemeClr)
            ELSE_TRY_READ_IF(scrgbClr)
            ELSE_TRY_READ_IF(sysClr)
            ELSE_TRY_READ_IF(srgbClr)
            ELSE_TRY_READ_IF(prstClr)
            ELSE_TRY_READ_IF(hslClr)
            ELSE_WRONG_FORMAT
        }
    }

    MSOOXML::DrawingMLFillBase *fillBase = m_context->themes->formatScheme.fillStyles.value(index);
    if (fillBase) {
        fillBase->writeStyles(*mainStyles, m_currentDrawStyle, m_currentColor);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL fontRef
//! fontRef handler (Font reference)
/*
 Parent elements:
 - [done] style (§21.3.2.24);
 - [done] style (§21.4.2.28);
 - [done] style (§20.1.2.2.37);
 - [done] style (§20.5.2.31);
 - [done] style (§19.3.1.46);
 - [done] tcTxStyle (§20.1.4.2.30)

 Child elements:
 - [done] hslClr (Hue, Saturation, Luminance Color Model) §20.1.2.3.13
 - [done] prstClr (Preset Color) §20.1.2.3.22
 - [done] schemeClr (Scheme Color) §20.1.2.3.29
 - [done] scrgbClr (RGB Color Model - Percentage Variant) §20.1.2.3.30
 - [done] srgbClr (RGB Color Model - Hex Variant) §20.1.2.3.32
 - [done] sysClr (System Color) §20.1.2.3.33
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_fontRef()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(idx)

    if (!idx.isEmpty()) {
        if (idx.startsWith("major")) {
            m_referredFontName = m_context->themes->fontScheme.majorFonts.latinTypeface;
        }
        else if (idx.startsWith("minor")) {
            m_referredFontName = m_context->themes->fontScheme.minorFonts.latinTypeface;
        }
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(schemeClr)
            ELSE_TRY_READ_IF(srgbClr)
            ELSE_TRY_READ_IF(sysClr)
            ELSE_TRY_READ_IF(scrgbClr)
            ELSE_TRY_READ_IF(prstClr)
            ELSE_TRY_READ_IF(hslClr)
            ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}
#endif //!defined MSOOXML_THEMESREADER_CPP

#undef CURRENT_EL
#define CURRENT_EL hslClr
//! hslClr (hue saturation luminance color)
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_hslClr()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    READ_ATTR_WITHOUT_NS(hue)
    READ_ATTR_WITHOUT_NS(sat)
    READ_ATTR_WITHOUT_NS(lum)

    qreal trueHue = hue.toDouble() / 6000.0 / 360;
    qreal trueSat = sat.left(sat.size() - 1).toDouble() / 100.0;
    qreal trueLum = lum.left(lum.size() - 1).toDouble() / 100.0;

    m_currentColor.setHslF(trueHue, trueSat, trueLum);

    //TODO: all the color transformations
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(tint)
            ELSE_TRY_READ_IF(shade)
            ELSE_TRY_READ_IF(satMod)
            ELSE_TRY_READ_IF(alpha)
            SKIP_UNKNOWN
        }
    }

    MSOOXML::Utils::modifyColor(m_currentColor, m_currentTint, m_currentShadeLevel, m_currentSatMod);

    READ_EPILOGUE
}


#undef CURRENT_EL
#define CURRENT_EL prstClr
//! prstClr (preset color)
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_prstClr()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(val)

    // TODO: support all of them..
    if (!val.isEmpty()) {
        if (val == "aliceBlue") {
            m_currentColor = QColor(240, 248, 255);
        }
        else if (val == "antiqueWhite") {
            m_currentColor = QColor(250, 235, 215);
        }
        else if (val == "aqua") {
            m_currentColor = QColor(0, 255, 255);
        }
        else if (val == "aquamarine") {
            m_currentColor = QColor(127, 255, 212);
        }
        else if (val == "azure") {
            m_currentColor = QColor(240, 255, 255);
        }
        else if (val == "beige") {
            m_currentColor = QColor(245, 245, 220);
        }
        else if (val == "bisque") {
            m_currentColor = QColor(255, 228, 196);
        }
        else if (val == "black") {
            m_currentColor = QColor(0, 0, 0);
        }
        else if (val == "blue") {
            m_currentColor = QColor(0, 0, 215);
        }
        else if (val == "green") {
            m_currentColor = QColor(0, 255, 0);
        }
        else if (val == "red") {
            m_currentColor = QColor(255, 0, 0);
        }
        else if (val == "violet") {
            m_currentColor = QColor(238, 130, 238);
        }
        else if (val == "wheat") {
            m_currentColor = QColor(245, 222, 179);
        }
        else if (val == "white") {
            m_currentColor = QColor(255, 255, 255);
        }
        else if (val == "whiteSmoke") {
            m_currentColor = QColor(245, 245, 245);
        }
        else if (val == "yellow") {
            m_currentColor = QColor(255, 255, 0);
        }
        else if (val == "yellowGreen") {
            m_currentColor = QColor(154, 205, 50);
        }
    }

    m_currentTint = 0;
    m_currentShadeLevel = 0;
    m_currentSatMod = 0;
    m_currentAlpha = 0;

    //TODO: all the color transformations
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(tint)
            ELSE_TRY_READ_IF(shade)
            ELSE_TRY_READ_IF(satMod)
            ELSE_TRY_READ_IF(alpha)
            SKIP_UNKNOWN
        }
    }

    MSOOXML::Utils::modifyColor(m_currentColor, m_currentTint, m_currentShadeLevel, m_currentSatMod);

    READ_EPILOGUE
}

/*
This element specifies a color bound to a user's theme. As with all
elements which define a color, it is possible to apply a list of color
transforms to the base color defined.

Parent Elements
  - accent1 (§20.1.4.1.1)
  - accent2 (§20.1.4.1.2)
  - accent3 (§20.1.4.1.3)
  - accent4 (§20.1.4.1.4)
  - accent5 (§20.1.4.1.5)
  - accent6 (§20.1.4.1.6)
  - alphaInv (§20.1.8.4)
  - bgClr (§20.1.8.10)
  - bgRef (§19.3.1.3)
  - buClr (§21.1.2.4.4)
  - clrFrom (§20.1.8.17)
  - clrMru (§19.2.1.4)
  - clrRepl (§20.1.8.18)
  - clrTo (§20.1.8.19)
  - clrVal (§19.5.27)
  - contourClr (§20.1.5.6)
  - custClr (§20.1.4.1.8)
  - dk1 (§20.1.4.1.9)
  - dk2 (§20.1.4.1.10)
  - duotone (§20.1.8.23)
  - effectClrLst (§21.4.4.7)
  - effectRef (§20.1.4.2.8)
  - extrusionClr (§20.1.5.7)
  - fgClr (§20.1.8.27)
  - fillClrLst (§21.4.4.8)
  - fillRef (§20.1.4.2.10)
  - folHlink (§20.1.4.1.15)
  - fontRef (§20.1.4.1.17)
  - from (§19.5.43)
  - glow (§20.1.8.32)
  - gs (§20.1.8.36)
  - highlight (§21.1.2.3.4)
  - hlink (§20.1.4.1.19)
  - innerShdw (§20.1.8.40)
  - linClrLst (§21.4.4.9)
  - lnRef (§20.1.4.2.19)
  - lt1 (§20.1.4.1.22)
  - lt2 (§20.1.4.1.23)
  - outerShdw (§20.1.8.45)
  - penClr (§19.2.1.23)
  - prstShdw (§20.1.8.49)
  - [done] solidFill (§20.1.8.54)
  - tcTxStyle (§20.1.4.2.30)
  - to (§19.5.90)
  - txEffectClrLst (§21.4.4.12)
  - txFillClrLst (§21.4.4.13)
  - txLinClrLst (§21.4.4.14)

Child elements:
  - [done] alpha (Alpha) §20.1.2.3.1
  - alphaMod (Alpha Modulation) §20.1.2.3.2
  - alphaOff (Alpha Offset) §20.1.2.3.3
  - blue (Blue) §20.1.2.3.4
  - blueMod (Blue Modification) §20.1.2.3.5
  - blueOff (Blue Offset) §20.1.2.3.6
  - comp (Complement) §20.1.2.3.7
  - gamma (Gamma) §20.1.2.3.8
  - gray (Gray) §20.1.2.3.9
  - green (Green) §20.1.2.3.10
  - greenMod (Green Modification) §20.1.2.3.11
  - greenOff (Green Offset) §20.1.2.3.12
  - hue (Hue) §20.1.2.3.14
  - hueMod (Hue Modulate) §20.1.2.3.15
  - hueOff (Hue Offset) §20.1.2.3.16
  - inv (Inverse) §20.1.2.3.17
  - invGamma (Inverse Gamma) §20.1.2.3.18
  - lum (Luminance) §20.1.2.3.19
  - [done] lumMod (Luminance Modulation) §20.1.2.3.20
  - [done] lumOff (Luminance Offset) §20.1.2.3.21
  - red (Red) §20.1.2.3.23
  - redMod (Red Modulation) §20.1.2.3.24
  - redOff (Red Offset) §20.1.2.3.25
  - sat (Saturation) §20.1.2.3.26
  - [done] satMod (Saturation Modulation) §20.1.2.3.27
  - satOff (Saturation Offset) §20.1.2.3.28
  - shade (Shade) §20.1.2.3.31
  - [done] tint (Tint) §20.1.2.3.34

Attributes
  - val (Value)    Specifies the desired scheme.
 */
#undef CURRENT_EL
#define CURRENT_EL schemeClr
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_schemeClr()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS(val)

#ifdef PPTXXMLDOCUMENTREADER_CPP
    // Skip the rest of the code, the color scheme map (clrMap) is unknown at
    // time of reading.
    if (m_colorState == PptxXmlDocumentReader::defRPrState) {
        defaultTextColors[defaultTextColors.size() - 1] = val;
    }
    else {
        defaultBulletColors[defaultBulletColors.size() - 1] = val;
    }
    skipCurrentElement();
    READ_EPILOGUE
#endif

    m_currentTint = 0;
    m_currentShadeLevel = 0;
    m_currentSatMod = 0;
    m_currentAlpha = 0;

    MSOOXML::DrawingMLColorSchemeItemBase *colorItem = 0;

#if !defined MSOOXML_THEMESREADER_CPP
    QString valTransformed = m_context->colorMap.value(val);
    if (valTransformed.isEmpty()) {
        // In some cases, such as fontRef, mapping is bypassed
        colorItem = m_context->themes->colorScheme.value(val);
    } else {
        colorItem = m_context->themes->colorScheme.value(valTransformed);
    }
#endif

    // Parse the child elements
    MSOOXML::Utils::DoubleModifier lumMod;
    MSOOXML::Utils::DoubleModifier lumOff;
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            // @todo: Hmm, are these color modifications only available for pptx?
            if (QUALIFIED_NAME_IS(lumMod)) {
                m_currentDoubleValue = &lumMod.value;
                TRY_READ(lumMod)
                lumMod.valid = true;
            } else if (QUALIFIED_NAME_IS(lumOff)) {
                m_currentDoubleValue = &lumOff.value;
                TRY_READ(lumOff)
                lumOff.valid = true;
            }
            ELSE_TRY_READ_IF(shade)
            ELSE_TRY_READ_IF(tint)
            ELSE_TRY_READ_IF(satMod)
            ELSE_TRY_READ_IF(alpha)
            SKIP_UNKNOWN
        }
    }

    QColor col = Qt::white;
    if (colorItem) {
        col = colorItem->value();
    }

    col = MSOOXML::Utils::colorForLuminance(col, lumMod, lumOff);
    m_currentColor = col;

    MSOOXML::Utils::modifyColor(m_currentColor, m_currentTint, m_currentShadeLevel, m_currentSatMod);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sysClr
//! sysClr handler
// SysClr is bit controversial, it is supposed to use
// color defined by the system at the moment, the document is read
// however, it often means that when reading the document, it is not
// using the same colors, the creator wished.
// Sometimes sysclr saves attribute lastClr which tells which color
// the creator was using, the current implementation uses that
// and ignores real system colors.
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_sysClr()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    m_currentTint = 0;
    m_currentShadeLevel = 0;
    m_currentSatMod = 0;
    m_currentAlpha = 0;

    TRY_READ_ATTR_WITHOUT_NS(lastClr)

    if (!lastClr.isEmpty()) {
        m_currentColor = QColor( QLatin1Char('#') + lastClr );
    }

    //TODO: all the color transformations
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(tint)
            ELSE_TRY_READ_IF(shade)
            ELSE_TRY_READ_IF(satMod)
            ELSE_TRY_READ_IF(alpha)
            SKIP_UNKNOWN
        }
    }

    MSOOXML::Utils::modifyColor(m_currentColor, m_currentTint, m_currentShadeLevel, m_currentSatMod);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL scrgbClr
//! RGB Color Model - Percentage Variant
//! DrawingML ECMA-376 20.1.2.3.30, p. 3074.
/*!
This element specifies a solid color fill.

 Child elements:
    - [done] alpha (Alpha) §20.1.2.3.1
    - alphaMod (Alpha Modulation) §20.1.2.3.2
    - alphaOff (Alpha Offset) §20.1.2.3.3
    - blue (Blue) §20.1.2.3.4
    - blueMod (Blue Modification) §20.1.2.3.5
    - blueOff (Blue Offset) §20.1.2.3.6
    - comp (Complement) §20.1.2.3.7
    - gamma (Gamma) §20.1.2.3.8
    - gray (Gray) §20.1.2.3.9
    - green (Green) §20.1.2.3.10
    - greenMod (Green Modification) §20.1.2.3.11
    - greenOff (Green Offset) §20.1.2.3.12
    - hue (Hue) §20.1.2.3.14
    - hueMod (Hue Modulate) §20.1.2.3.15
    - hueOff (Hue Offset) §20.1.2.3.16
    - [done] inv (Inverse) §20.1.2.3.17
    - invGamma (Inverse Gamma) §20.1.2.3.18
    - lum (Luminance) §20.1.2.3.19
    - lumMod (Luminance Modulation) §20.1.2.3.20
    - lumOff (Luminance Offset) §20.1.2.3.21
    - red (Red) §20.1.2.3.23
    - redMod (Red Modulation) §20.1.2.3.24
    - redOff (Red Offset) §20.1.2.3.25

 Attributes:
    - [done] b (blue)
    - [done] g (green)
    - [done] r (red)
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_scrgbClr()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    m_currentTint = 0;
    m_currentShadeLevel = 0;
    m_currentSatMod = 0;
    m_currentAlpha = 0;

    READ_ATTR_WITHOUT_NS(r)
    READ_ATTR_WITHOUT_NS(g)
    READ_ATTR_WITHOUT_NS(b)

    bool okR, okG, okB;

    m_currentColor = QColor::fromRgbF(qreal(MSOOXML::Utils::ST_Percentage_to_double(r, okR)),
                                      qreal(MSOOXML::Utils::ST_Percentage_to_double(g, okG)),
                                      qreal(MSOOXML::Utils::ST_Percentage_to_double(b, okB)));

    //TODO: all the color transformations
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(tint)
            ELSE_TRY_READ_IF(alpha)
            SKIP_UNKNOWN
        }
    }

    MSOOXML::Utils::modifyColor(m_currentColor, m_currentTint, m_currentShadeLevel, m_currentSatMod);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL srgbClr
//! srgbClr (RGB Color Model - Hex Variant)
//! DrawingML ECMA-376 20.1.2.3.32, p. 3085.
/*!
This element specifies a color in RGB notation.

 Child elements:
    - alpha (Alpha) §20.1.2.3.1
    - alphaMod (Alpha Modulation) §20.1.2.3.2
    - alphaOff (Alpha Offset) §20.1.2.3.3
    - blue (Blue) §20.1.2.3.4
    - blueMod (Blue Modification) §20.1.2.3.5
    - blueOff (Blue Offset) §20.1.2.3.6
    - comp (Complement) §20.1.2.3.7
    - gamma (Gamma) §20.1.2.3.8
    - gray (Gray) §20.1.2.3.9
    - green (Green) §20.1.2.3.10
    - greenMod (Green Modification) §20.1.2.3.11
    - greenOff (Green Offset) §20.1.2.3.12
    - hue (Hue) §20.1.2.3.14
    - hueMod (Hue Modulate) §20.1.2.3.15
    - hueOff (Hue Offset) §20.1.2.3.16
    - inv (Inverse) §20.1.2.3.17
    - invGamma (Inverse Gamma) §20.1.2.3.18
    - lum (Luminance) §20.1.2.3.19
    - lumMod (Luminance Modulation) §20.1.2.3.20
    - lumOff (Luminance Offset) §20.1.2.3.21
    - red (Red) §20.1.2.3.23
    - redMod (Red Modulation) §20.1.2.3.24
    - redOff (Red Offset) §20.1.2.3.25
    - sat (Saturation) §20.1.2.3.26
    - [done] satMod (Saturation Modulation) §20.1.2.3.27
    - satOff (Saturation Offset) §20.1.2.3.28
    - [done] shade (Shade) §20.1.2.3.31
    - [done] tint (Tint) §20.1.2.3.34

 Attributes:
    - [done] val ("RRGGBB" hex digits)
*/
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_srgbClr()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    m_currentTint = 0;
    m_currentShadeLevel = 0;
    m_currentSatMod = 0;
    m_currentAlpha = 0;

    READ_ATTR_WITHOUT_NS(val)

    m_currentColor = QColor( QLatin1Char('#') + val );

    //TODO: all the color transformations
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(tint)
            ELSE_TRY_READ_IF(shade)
            ELSE_TRY_READ_IF(satMod)
            ELSE_TRY_READ_IF(alpha)
            SKIP_UNKNOWN
        }
    }

    MSOOXML::Utils::modifyColor(m_currentColor, m_currentTint, m_currentShadeLevel, m_currentSatMod);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL shade
//! shade (shade value)
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_shade()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)

    if (!val.isEmpty()) {
        bool ok = false;
        int value = val.toInt(&ok);
        if (!ok) {
            value = 0;
        }
        m_currentShadeLevel = value/100000.0; // To get percentage (form 0.x)
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tint
//! tint (tint value)
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_tint()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)

    if (!val.isEmpty()) {
        bool ok = false;
        int value = val.toInt(&ok);
        if (!ok) {
            value = 0;
        }
        m_currentTint = value/100000.0; // To get percentage (form 0.x)
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL alpha
//! alpha (alpha value)
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_alpha()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)

    if (!val.isEmpty()) {
        bool ok = false;
        int value = val.toInt(&ok);
        if (!ok) {
            value = 0;
        }
        m_currentAlpha = value/1000; // To get percentage
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lumMod
//! 20.1.2.3.20 lumMod (Luminance Modulation)
//! This element specifies the input color with its luminance modulated by the given percentage.
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lumMod()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS(val)

    bool ok;
    Q_ASSERT(m_currentDoubleValue);
    *m_currentDoubleValue = MSOOXML::Utils::ST_Percentage_withMsooxmlFix_to_double(val, ok);
    if (!ok)
        return KoFilter::WrongFormat;

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL satMod
//! satMod (Saturation modulation value)
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_satMod()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)

    if (!val.isEmpty()) {
        bool ok = false;
        int value = val.toInt(&ok);
        if (!ok) {
            value = 0;
        }
        m_currentSatMod = value/100000.0; // To get percentage in from 0.x
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lumOff
//! 20.1.2.3.21 lumOff (Luminance Offset)
//! This element specifies the input color with its luminance shifted, but with its hue and saturation unchanged.
//! @todo support all child elements
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_lumOff()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS(val)

    bool ok;
    Q_ASSERT(m_currentDoubleValue);
    *m_currentDoubleValue = MSOOXML::Utils::ST_Percentage_withMsooxmlFix_to_double(val, ok);
    if (!ok)
        return KoFilter::WrongFormat;

    readNext();
    READ_EPILOGUE
}

//! tailEnd (Tail line end style)
//! DrawingML ECMA-376 20.1.8.57, p.3232
/*!
  This element specifies decorations which can be added to the tail of
  a line.

  Parent Elements:
  - [done] ln (§20.1.2.2.24);
  - lnB (§21.1.3.5);
  - lnBlToTr (§21.1.3.6);
  - lnL (§21.1.3.7);
  - lnR (§21.1.3.8);
  - lnT (§21.1.3.9);
  - lnTlToBr (§21.1.3.10);
  - uLn (§21.1.2.3.14)

  Attributes:
  - len (Length of Head/End)
  - [done] type (Line Head/End Type)
  - w (Width of Head/End)

  TODO: merge the following two readers
*/
#undef CURRENT_EL
#define CURRENT_EL tailEnd
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_tailEnd()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(type)
    TRY_READ_ATTR_WITHOUT_NS(w)

    if (!type.isEmpty() && type != "none") {
        // draw:marker-end
        m_currentDrawStyle->addProperty("draw:marker-end", MSOOXML::Utils::defineMarkerStyle(*mainStyles, type));
        // draw:marker-end-center
        m_currentDrawStyle->addProperty("draw:marker-end-center", "false");
        // draw:marker-end-width
        m_currentDrawStyle->addPropertyPt("draw:marker-end-width",
                                          MSOOXML::Utils::defineMarkerWidth(w, m_currentLineWidth));
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL headEnd
KoFilter::ConversionStatus MSOOXML_CURRENT_CLASS::read_headEnd()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(type)
    TRY_READ_ATTR_WITHOUT_NS(w)

    if (!type.isEmpty() && type != "none") {
        // draw:marker-start
        m_currentDrawStyle->addProperty("draw:marker-start", MSOOXML::Utils::defineMarkerStyle(*mainStyles, type));
        // draw:marker-start-center
        m_currentDrawStyle->addProperty("draw:marker-start-center", "false");
        // draw:marker-start-width
        m_currentDrawStyle->addPropertyPt("draw:marker-start-width",
                                          MSOOXML::Utils::defineMarkerWidth(w, m_currentLineWidth));
    }

    readNext();
    READ_EPILOGUE
}

#endif
