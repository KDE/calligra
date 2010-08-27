/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "MsooXmlThemesReader.h"
#include "MsooXmlSchemas.h"
#include "MsooXmlUtils.h"
#include "KoXmlWriter.h"

#define MSOOXML_CURRENT_NS "a"
#define MSOOXML_CURRENT_CLASS MsooXmlThemesReader

#include "MsooXmlReader_p.h"

#include <QPalette>

#include <memory>

using namespace MSOOXML;

DrawingMLColorSchemeItem::DrawingMLColorSchemeItem()
{
}

DrawingMLColorSchemeSystemItem::DrawingMLColorSchemeSystemItem()
{
}

QColor DrawingMLColorSchemeSystemItem::value() const
{
    //kDebug() << systemColor;
    //! 20.1.10.58 ST_SystemColorVal (System Color Value)
    if (   systemColor == QLatin1String("windowText")
        || systemColor == QLatin1String("menuText"))
    {
//        return QPalette().color(QPalette::Active, QPalette::WindowText);
        return QPalette().color(QPalette::Active, QPalette::Window);
    }
    else if (    systemColor == QLatin1String("window")
              || systemColor == QLatin1String("menu")
              || systemColor == QLatin1String("menuBar"))
    {
//        return QPalette().color(QPalette::Active, QPalette::Window);
        return QPalette().color(QPalette::Active, QPalette::WindowText);
    }
    else if (systemColor == QLatin1String("highlightText")) {
        return QPalette().color(QPalette::Active, QPalette::HighlightedText);
    }
    else if (systemColor == QLatin1String("highlight")) {
        return QPalette().color(QPalette::Active, QPalette::Highlight);
    }
    else if (systemColor == QLatin1String("grayText")) {
        return QPalette().color(QPalette::Disabled, QPalette::WindowText);
    }
    else if (systemColor == QLatin1String("btnText")) {
        return QPalette().color(QPalette::Active, QPalette::ButtonText);
    }
    else if (systemColor == QLatin1String("btnFace")) {
        return QPalette().color(QPalette::Active, QPalette::Button);
    }
    else if (systemColor == QLatin1String("btnHighlight")) {
        return QPalette().color(QPalette::Active, QPalette::Light);
    }
    else if (systemColor == QLatin1String("btnShadow")) {
        return QPalette().color(QPalette::Active, QPalette::Dark);
    }
//! @todo Use more of systemColor
    return lastColor;
}

DrawingMLColorSchemeItemBase::DrawingMLColorSchemeItemBase()
{
}

DrawingMLColorSchemeItemBase::~DrawingMLColorSchemeItemBase()
{
}

DrawingMLColorSchemeItem* DrawingMLColorSchemeItemBase::toColorItem()
{
    return dynamic_cast<DrawingMLColorSchemeItem*>(this);
}

DrawingMLColorSchemeSystemItem* DrawingMLColorSchemeItemBase::toSystemItem()
{
    return dynamic_cast<DrawingMLColorSchemeSystemItem*>(this);
}

DrawingMLColorScheme::DrawingMLColorScheme()
{
}

DrawingMLColorScheme::~DrawingMLColorScheme()
{
    QSet<DrawingMLColorSchemeItemBase*> set(values().toSet()); // use set because values can be duplicated
    qDeleteAll(set);
}

DrawingMLColorSchemeItemBase* DrawingMLColorScheme::value(int index) const
{
    return DrawingMLColorSchemeItemHash::value( QString::number(index) );
}

DrawingMLFontScheme::DrawingMLFontScheme()
{
}

DrawingMLFontSet::DrawingMLFontSet()
{
}

DrawingMLFillStyleBase::DrawingMLFillStyleBase()
{
}

DrawingMLFillStyleBase::~DrawingMLFillStyleBase()
{
}

DrawingMLSolidFillStyle* DrawingMLFillStyleBase::toSolidFillStyle()
{
    return dynamic_cast<DrawingMLSolidFillStyle*>(this);
}

DrawingMLPatternFillStyle* DrawingMLFillStyleBase::toPatternFillStyle()
{
    return dynamic_cast<DrawingMLPatternFillStyle*>(this);
}

DrawingMLNoFillStyle* DrawingMLFillStyleBase::toNoFillStyle()
{
    return dynamic_cast<DrawingMLNoFillStyle*>(this);
}

DrawingMLNoFillStyle::DrawingMLNoFillStyle()
{
}

DrawingMLFormatScheme::DrawingMLFormatScheme()
{
}

DrawingMLPatternFillStyle::DrawingMLPatternFillStyle()
        : backgroundColor(0), foregroundColor(0)
{
}

DrawingMLTheme::DrawingMLTheme()
{
}

// ---------------------------------------------------

MsooXmlThemesReaderContext::MsooXmlThemesReaderContext(DrawingMLTheme& t)
        : MsooXmlReaderContext()
        , theme(&t)
{
}

// ---------------------------------------------------

MsooXmlThemesReader::MsooXmlThemesReader(KoOdfWriters *writers)
        : MsooXmlReader(writers)
        , m_currentColor(0)
        , m_context(0)
{
    init();
}

MsooXmlThemesReader::~MsooXmlThemesReader()
{
}

void MsooXmlThemesReader::init()
{
    m_clrScheme_initialized = false;
    m_color_initialized = false;
}

KoFilter::ConversionStatus MsooXmlThemesReader::read(MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<MsooXmlThemesReaderContext*>(context);
    Q_ASSERT(m_context);
    *m_context->theme = DrawingMLTheme(); //clear
    const KoFilter::ConversionStatus result = readInternal();
    m_context = 0;
    if (result == KoFilter::OK)
        return KoFilter::OK;
    *dynamic_cast<MsooXmlThemesReaderContext*>(context)->theme = DrawingMLTheme(); //clear
    return result;
}

KoFilter::ConversionStatus MsooXmlThemesReader::readInternal()
{
    //kDebug() << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    readNext();
    //kDebug() << *this;
    if (isStartElement()) {
        TRY_READ_IF(theme)
        ELSE_WRONG_FORMAT
    } else {
        return KoFilter::WrongFormat;
    }
    //kDebug() << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL theme
//! theme (Theme)
/*! ECMA-376, 20.1.6.9, p.3180.
 No parent elements.
 Child elements:
 - custClrLst (Custom Color List) §20.1.6.3
 - extLst (Extension List) §20.1.2.2.15
 - extraClrSchemeLst (Extra Color Scheme List) §20.1.6.5
 - objectDefaults (Object Defaults) §20.1.6.7
 - themeElements [done] (Theme Elements) §20.1.6.10
 CASE #850
*/
KoFilter::ConversionStatus MsooXmlThemesReader::read_theme()
{
    READ_PROLOGUE
    //kDebug() << namespaceUri();

    if (!expectNS(Schemas::drawingml::main)) {
        return KoFilter::WrongFormat;
    }

    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS_INTO(name, m_context->theme->name)
    //kDebug() << "name:" << m_context->theme->name;

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    //for (int i = 0; i < namespaces.count(); i++) {
        //kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    //}
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration(MSOOXML_CURRENT_NS, Schemas::drawingml::main))) {
        raiseError(i18n("Namespace \"%1\" not found", Schemas::drawingml::main));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    while (!atEnd()) {
        readNext();
        //kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(themeElements)
            ELSE_TRY_READ_IF(custClrLst)
            ELSE_TRY_READ_IF(extLst)
            ELSE_TRY_READ_IF(extraClrSchemeLst)
            ELSE_TRY_READ_IF(objectDefaults)
            ELSE_TRY_READ_IF(custClrLst)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL themeElements
//! themeElements (Theme Elements)
/*! ECMA-376, 20.1.6.10, p.3181.
 Parent elements:
 - theme
 Child elements:
 - clrScheme (Color Scheme) §20.1.6.2
 - extLst (Extension List) §20.1.2.2.15
 - fmtScheme (Format Scheme) §20.1.4.1.14
 - fontScheme (Font Scheme) §20.1.4.1.18
 CASE #850
*/
KoFilter::ConversionStatus MsooXmlThemesReader::read_themeElements()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        //kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(clrScheme)
            ELSE_TRY_READ_IF(extLst)
            ELSE_TRY_READ_IF(fmtScheme)
            ELSE_TRY_READ_IF(fontScheme)
            ELSE_WRONG_FORMAT
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL objectDefaults
//! objectDefaults (Object Defaults)
/*! ECMA-376, 20.1.6.7, p.3178.

 This element allows for the definition of default shape, line, and textbox formatting properties. An application
 can use this information to format a shape (or text) initially on insertion into a document.

 Parent elements:
 - [done] theme
 Child elements:
 - extLst (Extension List) §20.1.2.2.15
 - lnDef (Line Default) §20.1.4.1.20
 - spDef (Shape Default) §20.1.4.1.27
 - txDef (Text Default) §20.1.4.1.28

 @todo
*/
KoFilter::ConversionStatus MsooXmlThemesReader::read_objectDefaults()
{
    SKIP_EVERYTHING_AND_RETURN
}

#undef CURRENT_EL
#define CURRENT_EL custClrLst
//! custClrLst (Custom Color List))
/*! ECMA-376, 20.1.6.3, p.3175.

 @todo
*/
KoFilter::ConversionStatus MsooXmlThemesReader::read_custClrLst()
{
    SKIP_EVERYTHING_AND_RETURN
}

//! @todo !!!!!!!!!!!!!MERGE with Document Reader!!!!!!!!!!!!!
#undef CURRENT_EL
#define CURRENT_EL extLst
//! extLst (Extension List)
/*! ECMA-376, 20.1.2.2.15, p.3036.

 @todo implement
*/
KoFilter::ConversionStatus MsooXmlThemesReader::read_extLst()
{
    SKIP_EVERYTHING_AND_RETURN
}

#undef CURRENT_EL
#define CURRENT_EL clrMap
KoFilter::ConversionStatus MsooXmlThemesReader::read_clrMap()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    int index = 0;
    while (index < attrs.size()) {
        const QString handledAttr = attrs.at(index).name().toString();
        const QString attrValue = attrs.value(handledAttr).toString();
        if (handledAttr != attrValue)
        {
            // This is done to support masterslides properly, they may try to use colors
            // which are not yet mapped to correct ones, so we use these values.
            MSOOXML::DrawingMLColorSchemeItemBase *colorItem = 0;
            colorItem = m_context->theme->colorScheme.value(attrValue);
            QColor col = Qt::white;
            if (colorItem) {
                col = colorItem->value();
            }
            std::auto_ptr<DrawingMLColorSchemeItem> color(new DrawingMLColorSchemeItem);
            m_currentColor = 0;
            color.get()->color = col;
            m_currentColor = color.release();

            m_context->theme->colorScheme.insert(handledAttr, m_currentColor);
            colorItem = m_context->theme->colorScheme.value(handledAttr);
            if (colorItem) {
                col = colorItem->value();
            }
        }
        ++index;
    }

    SKIP_EVERYTHING

    READ_EPILOGUE
}

//! @todo !!!!!!!!!!!!!MERGE with Document Reader!!!!!!!!!!!!!
#undef CURRENT_EL
#define CURRENT_EL extraClrSchemeLst
//! extraClrSchemeLst (Extra Color Scheme List)
/*! ECMA-376, 20.1.6.5, p.3177.

 @todo implement
*/
KoFilter::ConversionStatus MsooXmlThemesReader::read_extraClrSchemeLst()
{
   READ_PROLOGUE

   while (!atEnd()) {
        readNext();
        //kDebug() << *this;
        if (isStartElement()) {
            TRY_READ_IF(clrMap)
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#define INIT_COLOR_ITEM(name, index) \
    BIND_READ_METHOD(name, color) \
    m_colorSchemeIndices.insert(name, QLatin1String(STRINGIFY(index)));

//! @todo !!!!!!!!!!!!!MERGE with Document Reader!!!!!!!!!!!!!
#undef CURRENT_EL
#define CURRENT_EL clrScheme
//! clrScheme (Color Scheme)
/*! ECMA-376, 20.1.6.2, p.3173.
 Parent elements:
 - extraClrScheme (§20.1.6.4)
 - [done] themeElements (§20.1.6.10)
 - themeOverride (§20.1.6.12)
 Child elements:
    - accent1 (Accent 1) §20.1.4.1.1
    - accent2 (Accent 2) §20.1.4.1.2
    - accent3 (Accent 3) §20.1.4.1.3
    - accent4 (Accent 4) §20.1.4.1.4
    - accent5 (Accent 5) §20.1.4.1.5
    - accent6 (Accent 6) §20.1.4.1.6
    - dk1 (Dark 1) §20.1.4.1.9
    - dk2 (Dark 2) §20.1.4.1.10
    - extLst (Extension List) §20.1.2.2.15
    - folHlink (Followed Hyperlink) §20.1.4.1.15
    - hlink (Hyperlink) §20.1.4.1.19
    - lt1 (Light 1) §20.1.4.1.22
    - lt2 (Light 2) §20.1.4.1.23
*/
KoFilter::ConversionStatus MsooXmlThemesReader::read_clrScheme()
{
    READ_PROLOGUE

    if (!m_clrScheme_initialized) {
        m_clrScheme_initialized = true;
        INIT_COLOR_ITEM("dk1", 0)
        INIT_COLOR_ITEM("lt1", 1)
        INIT_COLOR_ITEM("dk2", 2)
        INIT_COLOR_ITEM("lt2", 3)
        INIT_COLOR_ITEM("accent1", 4)
        INIT_COLOR_ITEM("accent2", 5)
        INIT_COLOR_ITEM("accent3", 6)
        INIT_COLOR_ITEM("accent4", 7)
        INIT_COLOR_ITEM("accent5", 8)
        INIT_COLOR_ITEM("accent6", 9)
        INIT_COLOR_ITEM("hlink", 10)
        INIT_COLOR_ITEM("folHlink", 11)
    }

    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_WITHOUT_NS_INTO(name, m_context->theme->colorScheme.name)

    while (!atEnd()) {
        readNext();
        //kDebug() << *this;
        if (isStartElement()) {
            ReadMethod readMethod = m_readMethods.value(this->name().toString());
            if (readMethod) {
                Q_ASSERT(!m_currentColor);
                RETURN_IF_ERROR( (this->*readMethod)() )
                Q_ASSERT(m_currentColor);
                if (!m_currentColor) {
                    return KoFilter::InternalError;
                }
                const QString colorName( this->name().toString() );
                //kDebug() << "inserting color for" << colorName;
                m_context->theme->colorScheme.insert(colorName, m_currentColor);
                const QString colorIndex(m_colorSchemeIndices.value(colorName));
                if (!colorIndex.isEmpty()) {
                    m_context->theme->colorScheme.insert(colorIndex, m_currentColor);
                }

                m_currentColor = 0;
            }
            ELSE_WRONG_FORMAT_DEBUG("!readMethod")
        }
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}
#undef INIT_COLOR_ITEM

//! @todo !!!!!!!!!!!!!MERGE with Document Reader!!!!!!!!!!!!!
#undef CURRENT_EL
//! accent[1-6] (Accent [1-6]), dk1/2, lt1/2
/*! ECMA-376, 20.1.4.1.1-6, p.3098, etc.
 Parent elements:
 - [done] clrScheme (§20.1.6.2)
 Child elements:
 - hslClr (Hue, Saturation, Luminance Color Model) §20.1.2.3.13
 - prstClr (Preset Color) §20.1.2.3.22
 - [unsupported by MSO] schemeClr (Scheme Color) §20.1.2.3.29
 - scrgbClr (RGB Color Model - Percentage Variant) §20.1.2.3.30
 - srgbClr (RGB Color Model - Hex Variant) §20.1.2.3.32
 - sysClr (System Color) §20.1.2.3.33
*/
KoFilter::ConversionStatus MsooXmlThemesReader::read_color()
{
    const QString qn(qualifiedName().toString());

    if (!m_color_initialized) {
        m_color_initialized = true;
//! @todo remove _SKIP
        BIND_READ_SKIP(hslClr)
//! @todo remove _SKIP
        BIND_READ_SKIP(prstClr)
//! @todo remove _SKIP
        BIND_READ_SKIP(schemeClr)
//! @todo remove _SKIP
        BIND_READ_SKIP(scrgbClr)

        BIND_READ(srgbClr)
        BIND_READ(sysClr)
    }

    while (!atEnd()) {
        readNext();
        //kDebug() << *this;
        if (isStartElement()) {
            ReadMethod readMethod = m_readMethods.value(this->name().toString());
            if (readMethod) {
                RETURN_IF_ERROR( (this->*readMethod)() )
            }
            ELSE_WRONG_FORMAT_DEBUG("!readMethod")
        }
        BREAK_IF_END_OF_QSTRING(qn)
    }

    if (!expectElEnd(qn)) {
        return KoFilter::WrongFormat;
    }
    return KoFilter::OK;
}


//! @todo !!!!!!!!!!!!!MERGE with Document Reader!!!!!!!!!!!!!
#undef CURRENT_EL
#define CURRENT_EL srgbClr
/*! ECMA-376
 - srgbClr (RGB Color Model - Hex Variant) §20.1.2.3.32

 Parent elements:
    [done] accent1 (§20.1.4.1.1); accent2 (§20.1.4.1.2); accent3 (§20.1.4.1.3); accent4 (§20.1.4.1.4);
    [done] accent5 (§20.1.4.1.5); accent6 (§20.1.4.1.6);
    [done] dk1 (§20.1.4.1.9); dk2 (§20.1.4.1.10);
    [done] lt1 (§20.1.4.1.22); lt2 (§20.1.4.1.23);
    [done] folHlink (§20.1.4.1.15); hlink (§20.1.4.1.19);

    todo:
    alphaInv (§20.1.8.4); bgClr (§20.1.8.10); bgRef (§19.3.1.3); buClr (§21.1.2.4.4); clrFrom
    (§20.1.8.17); clrMru (§19.2.1.4); clrRepl (§20.1.8.18); clrTo (§20.1.8.19); clrVal (§19.5.27); contourClr
    (§20.1.5.6); custClr (§20.1.4.1.8); duotone (§20.1.8.23); effectClrLst
    (§21.4.4.7); effectRef (§20.1.4.2.8); extrusionClr (§20.1.5.7); fgClr (§20.1.8.27); fillClrLst (§21.4.4.8); fillRef
    (§20.1.4.2.10); fontRef (§20.1.4.1.17); from (§19.5.43); glow (§20.1.8.32); gs
    (§20.1.8.36); highlight (§21.1.2.3.4); innerShdw (§20.1.8.40); linClrLst (§21.4.4.9); lnRef
    (§20.1.4.2.19); outerShdw (§20.1.8.45); penClr (§19.2.1.23); prstShdw
    (§20.1.8.49); solidFill (§20.1.8.54); tcTxStyle (§20.1.4.2.30); to (§19.5.90); txEffectClrLst (§21.4.4.12);
    txFillClrLst (§21.4.4.13); txLinClrLst (§21.4.4.14)
 Child elements:
    alpha (Alpha) §20.1.2.3.1
    alphaMod (Alpha Modulation) §20.1.2.3.2
    alphaOff (Alpha Offset) §20.1.2.3.3
    blue (Blue) §20.1.2.3.4
    blueMod (Blue Modification) §20.1.2.3.5
    blueOff (Blue Offset) §20.1.2.3.6
    comp (Complement) §20.1.2.3.7
    gamma (Gamma) §20.1.2.3.8
    gray (Gray) §20.1.2.3.9
    green (Green) §20.1.2.3.10
    greenMod (Green Modification) §20.1.2.3.11
    greenOff (Green Offset) §20.1.2.3.12
    hue (Hue) §20.1.2.3.14
    hueMod (Hue Modulate) §20.1.2.3.15
    hueOff (Hue Offset) §20.1.2.3.16
    inv (Inverse) §20.1.2.3.17
    invGamma (Inverse Gamma) §20.1.2.3.18
    lum (Luminance) §20.1.2.3.19
    lumMod (Luminance Modulation) §20.1.2.3.20
    lumOff (Luminance Offset) §20.1.2.3.21
    red (Red) §20.1.2.3.23
    redMod (Red Modulation) §20.1.2.3.24
    redOff (Red Offset) §20.1.2.3.25
    sat (Saturation) §20.1.2.3.26
    satMod (Saturation Modulation) §20.1.2.3.27
    satOff (Saturation Offset) §20.1.2.3.28
    shade (Shade) §20.1.2.3.31
    tint (Tint) §20.1.2.3.34

    @todo these child elements are common for all *Clr() methods, so create common function for parsing them
*/
KoFilter::ConversionStatus MsooXmlThemesReader::read_srgbClr()
{
    std::auto_ptr<DrawingMLColorSchemeItem> color(new DrawingMLColorSchemeItem);
    m_currentColor = 0;
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    READ_ATTR_WITHOUT_NS(val)
    color.get()->color = Utils::ST_HexColorRGB_to_QColor(val);
    //kDebug() << color.get()->color;

    readNext();
    READ_EPILOGUE_WITHOUT_RETURN
    m_currentColor = color.release();

    return KoFilter::OK;
}

//! @todo !!!!!!!!!!!!!MERGE with Document Reader!!!!!!!!!!!!!
#undef CURRENT_EL
#define CURRENT_EL sysClr
/*! ECMA-376
 - sysClr (System Color) §20.1.2.3.33

 Parent elements:
    [done] accent1 (§20.1.4.1.1); accent2 (§20.1.4.1.2); accent3 (§20.1.4.1.3); accent4 (§20.1.4.1.4);
    [done] accent5 (§20.1.4.1.5); accent6 (§20.1.4.1.6);
    [done] dk1 (§20.1.4.1.9); dk2 (§20.1.4.1.10);
    [done] lt1 (§20.1.4.1.22); lt2 (§20.1.4.1.23);
    [done] folHlink (§20.1.4.1.15); hlink (§20.1.4.1.19);

    todo:
    alphaInv (§20.1.8.4); bgClr (§20.1.8.10); bgRef (§19.3.1.3); buClr (§21.1.2.4.4); clrFrom
    (§20.1.8.17); clrMru (§19.2.1.4); clrRepl (§20.1.8.18); clrTo (§20.1.8.19); clrVal (§19.5.27); contourClr
    (§20.1.5.6); custClr (§20.1.4.1.8); duotone (§20.1.8.23); effectClrLst
    (§21.4.4.7); effectRef (§20.1.4.2.8); extrusionClr (§20.1.5.7); fgClr (§20.1.8.27); fillClrLst (§21.4.4.8); fillRef
    (§20.1.4.2.10); fontRef (§20.1.4.1.17); from (§19.5.43); glow (§20.1.8.32); gs
    (§20.1.8.36); highlight (§21.1.2.3.4); innerShdw (§20.1.8.40); linClrLst (§21.4.4.9); lnRef
    (§20.1.4.2.19); outerShdw (§20.1.8.45); penClr (§19.2.1.23); prstShdw
    (§20.1.8.49); solidFill (§20.1.8.54); tcTxStyle (§20.1.4.2.30); to (§19.5.90); txEffectClrLst (§21.4.4.12);
    txFillClrLst (§21.4.4.13); txLinClrLst (§21.4.4.14)
 Child elements:
    alpha (Alpha) §20.1.2.3.1
    alphaMod (Alpha Modulation) §20.1.2.3.2
    alphaOff (Alpha Offset) §20.1.2.3.3
    blue (Blue) §20.1.2.3.4
    blueMod (Blue Modification) §20.1.2.3.5
    blueOff (Blue Offset) §20.1.2.3.6
    comp (Complement) §20.1.2.3.7
    gamma (Gamma) §20.1.2.3.8
    gray (Gray) §20.1.2.3.9
    green (Green) §20.1.2.3.10
    greenMod (Green Modification) §20.1.2.3.11
    greenOff (Green Offset) §20.1.2.3.12
    hue (Hue) §20.1.2.3.14
    hueMod (Hue Modulate) §20.1.2.3.15
    hueOff (Hue Offset) §20.1.2.3.16
    inv (Inverse) §20.1.2.3.17
    invGamma (Inverse Gamma) §20.1.2.3.18
    lum (Luminance) §20.1.2.3.19
    lumMod (Luminance Modulation) §20.1.2.3.20
    lumOff (Luminance Offset) §20.1.2.3.21
    red (Red) §20.1.2.3.23
    redMod (Red Modulation) §20.1.2.3.24
    redOff (Red Offset) §20.1.2.3.25
    sat (Saturation) §20.1.2.3.26
    satMod (Saturation Modulation) §20.1.2.3.27
    satOff (Saturation Offset) §20.1.2.3.28
    shade (Shade) §20.1.2.3.31
    tint (Tint) §20.1.2.3.34
*/
KoFilter::ConversionStatus MsooXmlThemesReader::read_sysClr()
{
    std::auto_ptr<DrawingMLColorSchemeSystemItem> color(new DrawingMLColorSchemeSystemItem);
    m_currentColor = 0;
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    READ_ATTR_WITHOUT_NS(lastClr)
    color.get()->lastColor = Utils::ST_HexColorRGB_to_QColor(lastClr);
    //kDebug() << "lastClr:" << color.get()->lastColor;

    // System color value. This color is based upon the value that this color currently has
    // within the system on which the document is being viewed.
    READ_ATTR_WITHOUT_NS_INTO(val, color.get()->systemColor)
    //kDebug() << "val:" << color.get()->systemColor;

    readNext();
    READ_EPILOGUE_WITHOUT_RETURN
    m_currentColor = color.release();

    return KoFilter::OK;
}

//! @todo !!!!!!!!!!!!!MERGE with Document Reader!!!!!!!!!!!!!
#undef CURRENT_EL
#define CURRENT_EL fmtScheme
//! fmtScheme (Format Scheme)
/*! ECMA-376, 20.1.4.1.14, p.3110.
 Parent elements:
 - [done] themeElements (§20.1.6.10)
 - themeOverride (§20.1.6.12)
 Child elements:
 - bgFillStyleLst (Background Fill Style List) §20.1.4.1.7
 - effectStyleLst (Effect Style List) §20.1.4.1.12
 - fillStyleLst (Fill Style List) §20.1.4.1.13
 - lnStyleLst (Line Style List) §20.1.4.1.21*/
KoFilter::ConversionStatus MsooXmlThemesReader::read_fmtScheme()
{
    SKIP_EVERYTHING_AND_RETURN
}

//! @todo !!!!!!!!!!!!!MERGE with Document Reader!!!!!!!!!!!!!
#undef CURRENT_EL
#define CURRENT_EL fontScheme
//! fontScheme (Font Scheme)
/*! ECMA-376, 20.1.4.1.18, p.3114.
 Parent elements:
 - [done] themeElements (§20.1.6.10)
 - themeOverride (§20.1.6.12)
 Child elements:
 - extLst (Extension List) §20.1.2.2.15
 - majorFont (Major Font) §20.1.4.1.24
 - minorFont (Minor fonts) §20.1.4.1.25*/
KoFilter::ConversionStatus MsooXmlThemesReader::read_fontScheme()
{
    SKIP_EVERYTHING_AND_RETURN
}

KoFilter::ConversionStatus MsooXmlThemesReader::read_SKIP()
{
    SKIP_EVERYTHING_AND_RETURN
}
