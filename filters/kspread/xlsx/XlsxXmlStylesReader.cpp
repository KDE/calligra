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

#include "XlsxXmlStylesReader.h"

#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <KoXmlWriter.h>
#include <styles/KoCharacterStyle.h>
#include <KoGenStyles.h>

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_CLASS XlsxXmlStylesReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>

#include <KGlobal>

#include <QMap>

#include <math.h>

//----------------------------------------------------------

#include "ColorConversions.h"
//! @todo only include for TINY target

//! @return tinted value for @a color
//! Alpha value is left unchanged.
/*! @param color to be converted
    @param tint color tint: from -1.0 .. 1.0, where -1.0 means 100% darken
           and 1.0 means 100% lighten; 0.0 means no change
*/
static QColor tintedColor(const QColor& color, qreal tint)
{
    const int HLSMAX = 255; // Used for computing tint
//kDebug() << "rgb:" << color.name() << "tint:" << tint;
    if (tint == 0.0 || !color.isValid()) {
        return color;
    }
    int h, l, s;
    rgb_to_hls(color.red(), color.green(), color.blue(), &h, &l, &s);
//    rgb_to_hls(0xec, 0xec, 0xec, &h, &l, &s);
//kDebug() << "hls before:" << h << l << s;
    if (tint < 0.0) {
        l = floor( l * (1.0 + tint) );
    }
    else {
        l = floor( l * (1.0 - tint) + (HLSMAX - HLSMAX * (1.0 - tint)) );
    }
//kDebug() << "hls after:" << h << l << s;
    quint8 r, g, b;
    hls_to_rgb(h, l, s, &r, &g, &b);
//kDebug() << "rgb:" << r << g << b << QColor(r, g, b, color.alpha()).name();
    return QColor(r, g, b, color.alpha());
}
#if 0
static QColor tintedColor( const QColor & color, qreal tintfactor )
{
    QColor retColor;
    const qreal  nonTindedPart = 1.0 - tintfactor;
    const int tintedColor = 255 * nonTindedPart;
    retColor.setRed( tintedColor + tintfactor * color.red() );
    retColor.setGreen( tintedColor + tintfactor * color.green() );
    retColor.setBlue( tintedColor + tintfactor * color.blue() );
    return retColor;
}
#endif

/*! @return color decoded from a "rgb" attribute of the current element
            or invalid QColor when reading was not possible.
Used by:
- color@rgb (§18.3.1.15)
- fgColor@rgb (§18.8.19)
- bgColor@rgb (§18.8.20) */
static QColor readRgbAttribute(const QXmlStreamAttributes& attrs)
{
    TRY_READ_ATTR_WITHOUT_NS(rgb)
//kDebug() << rgb;
    return ST_UnsignedIntHex_to_QColor(rgb);
}

/*! @return tint value (-1.0..1.0) decoded from a "tint" attribute of the current element
            or 0.0 when reading was not possible.
Used by:
- color@rgb (§18.3.1.15)
- fgColor@rgb (§18.8.19)
- bgColor@rgb (§18.8.20) */
static qreal readTintAttribute(const QXmlStreamAttributes& attrs, const char* debugElement)
{
    TRY_READ_ATTR_WITHOUT_NS(tint)
    qreal tintValue = 0.0;
    STRING_TO_QREAL(tint, tintValue, QLatin1String(debugElement) + "@tint")
kDebug() << tint << tintValue;
    return tintValue;
}

//----------------------------------------------------------

XlsxColorStyle::XlsxColorStyle()
{
    clear();
}

void XlsxColorStyle::clear()
{
    automatic = false;
    indexed = -1;
    tint = 0.0;
    theme = -1;
}

bool XlsxColorStyle::isValid(const /*QMap<QString, */MSOOXML::DrawingMLTheme/**>*/ *themes) const
{
kDebug() << "indexed:" << indexed << "rgb:" << rgb.name() << "tint:" << tint << "theme:" << theme;
    if (theme >= 0) {
kDebug() << themeColor(themes).isValid();
        return themeColor(themes).isValid();
    }
    return rgb.isValid();
}

QColor XlsxColorStyle::themeColor(const/* QMap<QString,*/ MSOOXML::DrawingMLTheme*/*> **/themes) const
{
    Q_ASSERT(themes);
//! @todo find proper theme, not just any
    const MSOOXML::DrawingMLTheme *themeObject = themes;
kDebug() << themeObject;
    if (themeObject) {
        MSOOXML::DrawingMLColorSchemeItemBase *colorItemBase = themeObject->colorScheme.value(theme);
kDebug() << colorItemBase;
//        MSOOXML::DrawingMLColorSchemeItem* colorItem = colorItemBase ? colorItemBase->toColorItem() : 0;
//kDebug() << colorItem;
//        if (colorItem)
        if (colorItemBase)
            return colorItemBase->value();
    }
    return QColor();
}

QColor XlsxColorStyle::value(const MSOOXML::DrawingMLTheme *themes) const
{
    QColor realColor;
kDebug() << "theme:" << theme;
    if (theme >= 0) {
        realColor = themeColor(themes);
kDebug() << "in theme found:" << realColor.name();
    }
    else {
        realColor = rgb;
kDebug() << "rgb found:" << realColor.name();
    }

    return tintedColor(realColor, tint);
}

KoFilter::ConversionStatus XlsxColorStyle::readAttributes(
    const QXmlStreamAttributes& attrs, const char* debugElement)
{
    automatic = MSOOXML::Utils::convertBooleanAttr(attrs.value("auto").toString());
    QString indexedStr;
    TRY_READ_ATTR_WITHOUT_NS_INTO(indexed, indexedStr)
    STRING_TO_INT(indexedStr, indexed, QLatin1String(debugElement) + "@indexed")
    // This is default array of colors from MS
    // TODO: Move to class in case something tries to override a value or two?
    const char* indexedArray[64] = {
        "000000", "FFFFFF", "FF0000", "00FF00", "0000FF",
        "FFFF00", "FF00FF", "00FFFF", "000000", "FFFFFF",
        "FF0000", "00FF00", "0000FF", "FFFF00", "FF00FF",
        "00FFFF", "800000", "008000", "000080", "808000",
        "800080", "008080", "C0C0C0", "808080", "9999FF",
        "993366", "FFFFCC", "CCFFFF", "660066", "FF8080",
        "0066CC", "CCCCFF", "000080", "FF00FF", "FFFF00",
        "00FFFF", "800080", "800000", "008080", "0000FF",
        "00CCFF", "CCFFFF", "CCFFCC", "FFFF99", "99CCFF",
        "FF99CC", "CC99FF", "FFCC99", "3366FF", "33CCCC",
        "99CC00", "FFCC00", "FF9900", "FF6600", "666699",
        "969696", "003366", "339966", "003300", "333300",
        "993300", "993366", "333399", "333333"
    };
    if (indexed >= 0 && indexed < 64) {
        rgb = QString("#%1").arg(indexedArray[indexed]);
    }
    else {
        rgb = readRgbAttribute(attrs);
    }
    tint = readTintAttribute(attrs, debugElement);
    QString themeStr;
    TRY_READ_ATTR_WITHOUT_NS_INTO(theme, themeStr)
    STRING_TO_INT(themeStr, theme, QLatin1String(debugElement) + "@theme")
kDebug() << "indexed:" << indexed << "rgb:" << rgb.name() << "tint:" << tint << "theme:" << theme;
    return KoFilter::OK;
}

//----------------------------------------------------------

XlsxFontStyle::XlsxFontStyle()
        : underline(NoUnderline),
        bold(false),
        italic(false),
        strike(false),
        m_size(12.0),
        m_defaultSize(true)
{
}

XlsxFillStyle::XlsxFillStyle()
        : patternType(NonePatternType),
          cachedRealBackgroundColor( 0 )
{
}

XlsxFillStyle::~XlsxFillStyle()
{
    delete cachedRealBackgroundColor;
}

XlsxBorderStyle::XlsxBorderStyle()
{
}

QString XlsxBorderStyle::setupCellStyle(const /*QMap<QString, */MSOOXML::DrawingMLTheme/**>*/ *themes) const
{
    QString styleString = this->style;

    QColor c;
    if (color.isValid(themes)) {
        c = color.value(themes);
    }
    else if (!this->style.isEmpty()) {
        c = Qt::black;
    }
    if (c.isValid()) {
        if (!styleString.isEmpty())
            styleString.append(' ');
        styleString.append(c.name());
    }
    return styleString;
}

//! @see http://www.w3.org/TR/CSS2/box.html#value-def-border-style
//! @see http://www.w3.org/TR/CSS2/box.html#value-def-border-width
KoFilter::ConversionStatus XlsxBorderStyle::readAttributes(
    const QXmlStreamAttributes& attrs)
{
    QString s;
//! @todo more styles
    TRY_READ_ATTR_WITHOUT_NS_INTO(style, s)
    if (s == QLatin1String("dashed") || s == QLatin1String("dotted") || s == QLatin1String("double")) {
        style = s;
    }
    else if (s == QLatin1String("medium") || s == QLatin1String("thick")
             || s == QLatin1String("thin"))
    {
        style = s + " solid";
    }
    else if (s == QLatin1String("none"))
        style = QLatin1String("hidden");
    else if (!s.isEmpty())
        style = QLatin1String("solid"); // fallback
    kDebug() << "style:" << s << "set to:" << style;
    return KoFilter::OK;
}

XlsxBorderStyles::XlsxBorderStyles()
{
}

void XlsxBorderStyles::setupCellStyle(KoGenStyle* cellStyle, const /*QMap<QString,*/ MSOOXML::DrawingMLTheme/**>*/ *themes) const
{
//! @todo simplify if 2 or 4 sides are the same
    QString s;
    s = top.setupCellStyle(themes);
    if (!s.isEmpty())
        cellStyle->addProperty("fo:border-top", s);
    s = right.setupCellStyle(themes);
    if (!s.isEmpty())
        cellStyle->addProperty("fo:border-right", s);
    s = bottom.setupCellStyle(themes);
    if (!s.isEmpty())
        cellStyle->addProperty("fo:border-bottom", s);
    s = left.setupCellStyle(themes);
    if (!s.isEmpty())
        cellStyle->addProperty("fo:border-left", s);
    if (diagonalDirections) {
        s = diagonal.setupCellStyle(themes);
        if (diagonalDirections & DiagonalUp) {
            cellStyle->addProperty("style:diagonal-bl-tr", s);
        }
        if (diagonalDirections & DiagonalDown) {
            cellStyle->addProperty("style:diagonal-tl-br", s);
        }
    }
}

static QColor applyPatternDensity( const XlsxColorStyle& bg, const XlsxColorStyle& fg, qreal percent, const /*QMap<QString, */MSOOXML::DrawingMLTheme/**>*/ *themes )
{
    const QColor bgColor = bg.theme >= 0 ? bg.themeColor( themes ) : bg.rgb.isValid() ? bg.rgb : QColor( Qt::white );
    const QColor fgColor = fg.theme >= 0 ? fg.themeColor( themes ) : fg.rgb;//.isValid() ? fg.rgb : QColor( Qt::black );

    QColor result( Qt::white );
    if( bgColor.isValid() ) {
        result = QColor( bgColor.red() * percent,
                         bgColor.green() * percent,
                         bgColor.blue() * percent,
                         bgColor.alpha() );
    }
    if( fgColor.isValid() ) {
        result = QColor( result.red()   + fgColor.red() * ( 1.0 - percent ),
                         result.green() + fgColor.green() * ( 1.0 - percent ),
                         result.blue()  + fgColor.blue() * ( 1.0 - percent ),
                         bgColor.isValid() ? bgColor.alpha() : fgColor.alpha() );
    }
    return result;
}

const XlsxColorStyle* XlsxFillStyle::realBackgroundColor( const /*QMap<QString,*/ MSOOXML::DrawingMLTheme/**>*/ *themes) const
{
    delete cachedRealBackgroundColor;
    cachedRealBackgroundColor = new XlsxColorStyle;

kDebug() << "patternType:" << patternType;
    switch (patternType) {
    case NonePatternType:
        return 0;
    case SolidPatternType:
        return &fgColor;
    case DarkDownPatternType:
        cachedRealBackgroundColor->rgb = applyPatternDensity( bgColor, fgColor, 0.5, themes );
        return cachedRealBackgroundColor;
    case DarkGrayPatternType:
        cachedRealBackgroundColor->rgb = applyPatternDensity( bgColor, fgColor, 0.25, themes );
        return cachedRealBackgroundColor;
    case DarkGridPatternType: // fall through
    case DarkHorizontalPatternType:
        cachedRealBackgroundColor->rgb = applyPatternDensity( bgColor, fgColor, 0.5, themes );
        return cachedRealBackgroundColor;
    case DarkTrellisPatternType:
        cachedRealBackgroundColor->rgb = applyPatternDensity( bgColor, fgColor, 0.25, themes );
        return cachedRealBackgroundColor;
    case DarkUpPatternType:  // fall through
    case DarkVerticalPatternType:
        cachedRealBackgroundColor->rgb = applyPatternDensity( bgColor, fgColor, 0.50, themes );
        return cachedRealBackgroundColor;
    case LightPatternType:
        break; //??
    case LightDownPatternType:  // fall through
    case LightGrayPatternType:
        cachedRealBackgroundColor->rgb = applyPatternDensity( bgColor, fgColor, 0.75, themes );
        return cachedRealBackgroundColor;
    case LightGridPatternType:
        cachedRealBackgroundColor->rgb = applyPatternDensity( bgColor, fgColor, 0.5625, themes );
        return cachedRealBackgroundColor;
    case LightHorizontalPatternType:
        cachedRealBackgroundColor->rgb = applyPatternDensity( bgColor, fgColor, 0.75, themes );
        return cachedRealBackgroundColor;
    case LightTrellisPatternType:
        cachedRealBackgroundColor->rgb = applyPatternDensity( bgColor, fgColor, 0.625, themes );
        return cachedRealBackgroundColor;
    case LightUpPatternType:
        cachedRealBackgroundColor->rgb = applyPatternDensity( bgColor, fgColor, 0.75, themes );
        return cachedRealBackgroundColor;
    case LightVerticalPatternType: // fall through
    case MediumGrayPatternType:
        cachedRealBackgroundColor->rgb = applyPatternDensity( bgColor, fgColor, 0.50, themes );
        return cachedRealBackgroundColor;
    case Gray0625PatternType:
        cachedRealBackgroundColor->rgb = applyPatternDensity( bgColor, fgColor, 0.9375, themes );
        return cachedRealBackgroundColor;
    case Gray125PatternType:
        cachedRealBackgroundColor->rgb = applyPatternDensity( bgColor, fgColor, 0.875, themes );
        return cachedRealBackgroundColor;
    }
    return &bgColor;
}

void XlsxFillStyle::setupCellStyle(KoGenStyle* cellStyle, const /*QMap<QString, */MSOOXML::DrawingMLTheme/**>*/ *themes) const
{
//! @todo implement more styling;
//!       use XlsxColorStyle::automatic, XlsxColorStyle::indexed, XlsxColorStyle::theme...
    const XlsxColorStyle* realBackgroundColor = this->realBackgroundColor( themes );
    if ( realBackgroundColor )
    {
        // This is necessary because excel switches the indexes of 0 - 1 and 2 - 3 for theme colorindexes
        // looks like it has a different internal indexing table
        // i found the info here : http://blogs.msdn.com/b/excel/archive/2007/11/16/chart-pattern-fills.aspx
        // and verified it
        XlsxColorStyle changedColor( *realBackgroundColor );
        if ( changedColor.theme == 0 )
            changedColor.theme = 1;
        else if ( changedColor.theme == 1 )
            changedColor.theme = 0;
        else if ( changedColor.theme == 2 )
            changedColor.theme = 3;
        else if ( changedColor.theme == 3 )
            changedColor.theme = 2;
        kDebug() << patternType << changedColor.value(themes).name()
                << changedColor.tint << changedColor.isValid(themes);
        if (changedColor.isValid(themes)) {
            cellStyle->addProperty("fo:background-color", changedColor.value(themes).name());
        }
    }
}

//----------------------------------------------------------

class ST_UnderlineValue_fromStringMap : public QMap<QString, XlsxFontStyle::ST_UnderlineValue>
{
public:
    ST_UnderlineValue_fromStringMap() {
        insert(QLatin1String("single"), XlsxFontStyle::SingleUnderline);
        insert(QLatin1String("double"), XlsxFontStyle::DoubleUnderline);
        insert(QLatin1String("singleAccounting"), XlsxFontStyle::SingleAccountingUnderline);
        insert(QLatin1String("doubleAccounting"), XlsxFontStyle::DoubleAccountingUnderline);
        insert(QLatin1String("none"), XlsxFontStyle::NoUnderline);
    }
};

// static
XlsxFontStyle::ST_UnderlineValue XlsxFontStyle::ST_UnderlineValue_fromString(const QString& s)
{
    K_GLOBAL_STATIC(ST_UnderlineValue_fromStringMap, s_ST_UnderlineValues)
//    kDebug() << s;
    const ST_UnderlineValue v = s_ST_UnderlineValues->value(s);
//    kDebug() << v;
    if (v == NoUnderline && s != "none")
        return SingleUnderline; // default
    return v;
}

void XlsxFontStyle::setUnderline(const QString& s)
{
    underline = ST_UnderlineValue_fromString(s);
//    kDebug() << underline;
}

ST_VerticalAlignRun::ST_VerticalAlignRun(const QString& msooxmlName)
{
    if (msooxmlName == QLatin1String("subscript"))
        value = SubscriptVerticalAlignRun;
    else if (msooxmlName == QLatin1String("superscript"))
        value = SuperscriptVerticalAlignRun;
    else
        value = BaselineVerticalAlignRun;
}

void ST_VerticalAlignRun::setupCharacterStyle(KoCharacterStyle* characterStyle) const
{
    switch (value) {
    case SubscriptVerticalAlignRun:
        characterStyle->setVerticalAlignment(QTextCharFormat::AlignSubScript);
        break;
    case SuperscriptVerticalAlignRun:
        characterStyle->setVerticalAlignment(QTextCharFormat::AlignSuperScript);
        break;
    default:;
    }
}

void XlsxFontStyle::setupCharacterStyle(KoCharacterStyle* characterStyle) const
{
    // line
    switch (underline) {
    case SingleUnderline:
    case DoubleUnderline:
    case SingleAccountingUnderline:
    case DoubleAccountingUnderline:
        characterStyle->setUnderlineStyle(KoCharacterStyle::SolidLine);
        break;
    case NoUnderline:
    default:;
    }

    // # of lines
    switch (underline) {
    case SingleUnderline:
    case SingleAccountingUnderline:
        characterStyle->setUnderlineType(KoCharacterStyle::SingleLine);
        break;
    case DoubleUnderline:
    case DoubleAccountingUnderline:
        characterStyle->setUnderlineType(KoCharacterStyle::DoubleLine);
        break;
    case NoUnderline:
    default:;
    }

//! @todo underlineWidth
    switch (underline) {
    case SingleUnderline:
    case SingleAccountingUnderline:
    case DoubleUnderline:
    case DoubleAccountingUnderline:
        characterStyle->setUnderlineWidth(KoCharacterStyle::AutoLineWeight, 1.0);
        break;
    case NoUnderline:
    default:;
    }

    if (strike) {
        characterStyle->setStrikeOutStyle(KoCharacterStyle::SolidLine);
        characterStyle->setStrikeOutType(KoCharacterStyle::SingleLine);
    }

    {
        KoCharacterStyle::LineWeight weight;
        qreal width;
        characterStyle->underlineWidth(weight, width);

/*        kDebug() << "underlineStyle:" << characterStyle->underlineStyle()
        << "underlineType:" << characterStyle->underlineType()
        << "underlineWeight:" << weight;*/
    }

    if (bold)
        characterStyle->setFontWeight(QFont::Bold);
    if (italic)
        characterStyle->setFontItalic(true);
    if (!m_defaultSize)
        characterStyle->setFontPointSize(m_size);

    vertAlign.setupCharacterStyle(characterStyle);
}

void XlsxFontStyle::setupCellTextStyle(
    const /*QMap<QString,*/ MSOOXML::DrawingMLTheme/**>*/ *themes,
    KoGenStyle* cellStyle) const
{
    if (!name.isEmpty()) {
#ifdef __GNUC__
#warning TODO: we are saving with fo:font-family now because style:font-name is not properly supported by kotext; fix void KoCharacterStyle::loadOdf(KoOdfLoadingContext &context)...
#endif
//!@ todo reenable this        cellStyle->addProperty("style:font-name", name, KoGenStyle::TextType);
        cellStyle->addProperty("fo:font-family", name, KoGenStyle::TextType);
    }
    // This is necessary because excel switches the indexes of 0 - 1 and 2 - 3 for theme colorindexes
    // looks like it has a different internal indexing table
    // i found the info here : http://blogs.msdn.com/b/excel/archive/2007/11/16/chart-pattern-fills.aspx
    // and verified it
    XlsxColorStyle changedColor( color );
    if ( changedColor.theme == 0 )
        changedColor.theme = 1;
    else if ( changedColor.theme == 1 )
        changedColor.theme = 0;
    else if ( changedColor.theme == 2 )
        changedColor.theme = 3;
    else if ( changedColor.theme == 3 )
        changedColor.theme = 2;
    else if (changedColor.isValid(themes)) {
        const QColor c(changedColor.value(themes));
        cellStyle->addProperty("fo:color", c.name(), KoGenStyle::TextType);
    }
    //! @todo implement more styling
}

XlsxStyles::XlsxStyles()
{
    // fill the default number formats
    // from Office Open XML Part 4 - Markup Language Reference, p. 1974
    numberFormatStrings[ 1 ] = QLatin1String( "0" );
    numberFormatStrings[ 2 ] = QLatin1String( "0.00" );
    numberFormatStrings[ 3 ] = QLatin1String( "#,##0" );
    numberFormatStrings[ 4 ] = QLatin1String( "#,##0.00" );
    numberFormatStrings[ 9 ] = QLatin1String( "0%" );
    numberFormatStrings[ 10 ] = QLatin1String( "0.00%" );
    numberFormatStrings[ 11 ] = QLatin1String( "0.00E+00" );
    numberFormatStrings[ 12 ] = QLatin1String( "# ?/?" );
    numberFormatStrings[ 13 ] = QLatin1String( "# \?\?/\?\?" );
    numberFormatStrings[ 14 ] = QLatin1String( "mm-dd-yy" );
    numberFormatStrings[ 15 ] = QLatin1String( "d-mmm-yy" );
    numberFormatStrings[ 16 ] = QLatin1String( "d-mmm" );
    numberFormatStrings[ 17 ] = QLatin1String( "mmm-yy" );
    numberFormatStrings[ 18 ] = QLatin1String( "h:mm AM/PM" );
    numberFormatStrings[ 19 ] = QLatin1String( "h:mm:ss AM/PM" );
    numberFormatStrings[ 20 ] = QLatin1String( "h:mm" );
    numberFormatStrings[ 21 ] = QLatin1String( "h:mm:ss" );
    numberFormatStrings[ 22 ] = QLatin1String( "m/d/yy h:mm" );
    numberFormatStrings[ 37 ] = QLatin1String( "#,##0 ;(#,##0)" );
    numberFormatStrings[ 38 ] = QLatin1String( "#,##0 ;[Red](#,##0)" );
    numberFormatStrings[ 39 ] = QLatin1String( "#,##0.00;(#,##0.00)" );
    numberFormatStrings[ 40 ] = QLatin1String( "#,##0.00;[Red](#,##0.00)" );
    numberFormatStrings[ 45 ] = QLatin1String( "mm:ss" );
    numberFormatStrings[ 46 ] = QLatin1String( "[h]:mm:ss" );
    numberFormatStrings[ 47 ] = QLatin1String( "mmss.0" );
    numberFormatStrings[ 48 ] = QLatin1String( "##0.0E+0" );
    numberFormatStrings[ 49 ] = QLatin1String( "@" );
}

XlsxStyles::~XlsxStyles()
{
    for (int i = 0; i < fontStyles.size(); i++)
        delete fontStyles[i];
    for (int i = 0; i < fillStyles.size(); i++)
        delete fillStyles[i];
    for (int i = 0; i < cellFormats.size(); i++)
        delete cellFormats[i];
}

void XlsxStyles::setCellFormat(XlsxCellFormat *format, int cellFormatIndex)
{
//    format->styles = this;
    delete cellFormats[cellFormatIndex];
    cellFormats[cellFormatIndex] = format;
}

//----------------------------------------------------------
//! default values based on Annex A, p. 4476
XlsxCellFormat::XlsxCellFormat()
 : applyAlignment(true), applyBorder(true), applyFill(true), applyFont(true),
   applyNumberFormat(true), applyProtection(true),
   borderId(-1), fillId(-1), fontId(-1), numFmtId(-1),
   pivotButton(false), quotePrefix(false), xfId(-1),
   horizontalAlignment(GeneralHorizontalAlignment),
   verticalAlignment(NoVerticalAlignment),
   wrapText(false),
   shrinkToFit(false),
   textRotation(0)
{
}

XlsxCellFormat::~XlsxCellFormat()
{
}

bool XlsxCellFormat::setupCharacterStyle(const XlsxStyles *styles, KoCharacterStyle* characterStyle) const
{
    XlsxFontStyle* fontStyle = styles->fontStyle(fontId);
    if (!fontStyle) {
        kWarning() << "No font with ID:" << fontId;
        return false;
    }
    fontStyle->setupCharacterStyle(characterStyle);
    return true;
}

class ST_HorizontalAlignment_fromStringMap : public QMap<QString, XlsxCellFormat::ST_HorizontalAlignment>
{
public:
    ST_HorizontalAlignment_fromStringMap() {
        insert(QString(), XlsxCellFormat::GeneralHorizontalAlignment);
        insert(QLatin1String("general"), XlsxCellFormat::GeneralHorizontalAlignment);
        insert(QLatin1String("center"), XlsxCellFormat::CenterHorizontalAlignment);
        insert(QLatin1String("centerContinuous"), XlsxCellFormat::CenterContinuousHorizontalAlignment);
        insert(QLatin1String("distributed"), XlsxCellFormat::DistributedHorizontalAlignment);
        insert(QLatin1String("fill"), XlsxCellFormat::FillHorizontalAlignment);
        insert(QLatin1String("justify"), XlsxCellFormat::JustifyHorizontalAlignment);
        insert(QLatin1String("left"), XlsxCellFormat::LeftHorizontalAlignment);
        insert(QLatin1String("right"), XlsxCellFormat::RightHorizontalAlignment);
    }
};

void XlsxCellFormat::setHorizontalAlignment(const QString& alignment)
{
    K_GLOBAL_STATIC(ST_HorizontalAlignment_fromStringMap, s_ST_HorizontalAlignmentValues)
    horizontalAlignment = s_ST_HorizontalAlignmentValues->value(alignment);
}

class ST_VerticalAlignment_fromStringMap : public QMap<QString, XlsxCellFormat::ST_VerticalAlignment>
{
public:
    ST_VerticalAlignment_fromStringMap() {
        insert(QString(), XlsxCellFormat::NoVerticalAlignment);
        insert(QLatin1String("bottom"), XlsxCellFormat::BottomVerticalAlignment);
        insert(QLatin1String("center"), XlsxCellFormat::CenterVerticalAlignment);
        insert(QLatin1String("distributed"), XlsxCellFormat::DistributedVerticalAlignment);
        insert(QLatin1String("justify"), XlsxCellFormat::JustifyVerticalAlignment);
        insert(QLatin1String("top"), XlsxCellFormat::TopVerticalAlignment);
    }
};

void XlsxCellFormat::setVerticalAlignment(const QString& alignment)
{
    K_GLOBAL_STATIC(ST_VerticalAlignment_fromStringMap, s_ST_VerticalAlignmentValues)
    verticalAlignment = s_ST_VerticalAlignmentValues->value(alignment);
}

//! CASE #S1600
void XlsxCellFormat::setupCellStyleAlignment(KoGenStyle* cellStyle) const
{
//! @todo FillHorizontalAlignment, JustifyHorizontalAlignment
    int wrapOption = -1; // "don't know"
    if (wrapText)
        wrapOption = 1;
    //special case: 255 indicates vertical rotation without rotated characters (I couldn't find that documented though)
    const bool verticalTtb = textRotation == 255;
    if (verticalTtb)
        cellStyle->addProperty("style:direction", "ttb");
    else if (textRotation != 0) {
        unsigned angle = textRotation;
        if (angle > 90) angle = 360 - (angle - 90);
        cellStyle->addProperty("style:rotation-angle", QString::number(angle));
    }

    if (shrinkToFit)
        cellStyle->addProperty("style:shrink-to-fit", "true");

    switch (horizontalAlignment) {
    case CenterHorizontalAlignment:
    case CenterContinuousHorizontalAlignment:
    case DistributedHorizontalAlignment:
        cellStyle->addProperty("fo:text-align", "center", KoGenStyle::ParagraphType);
        if (horizontalAlignment == DistributedHorizontalAlignment)
            wrapOption = 1;
        break;
    case GeneralHorizontalAlignment: // ok?
        if (verticalTtb) // Excel centers vertical text by default, so mimic that
            cellStyle->addProperty("fo:text-align", "center", KoGenStyle::ParagraphType);
        if (textRotation > 90 && textRotation < 180) // Excel right aligns rotated text for some angles
            cellStyle->addProperty("fo:text-align", "end", KoGenStyle::ParagraphType);
        break;
    case LeftHorizontalAlignment:
        cellStyle->addProperty("fo:text-align", "start", KoGenStyle::ParagraphType);
        break;
    case RightHorizontalAlignment:
        cellStyle->addProperty("fo:text-align", "end", KoGenStyle::ParagraphType);
        break;
    case JustifyHorizontalAlignment:
        cellStyle->addProperty("fo:text-align", "justify", KoGenStyle::ParagraphType);
        wrapOption = 1;
        break;
    case FillHorizontalAlignment:
    default:;
    }

    switch (verticalAlignment) {
    case CenterVerticalAlignment:
        cellStyle->addProperty("style:vertical-align", "middle");
        break;
    case TopVerticalAlignment:
        cellStyle->addProperty("style:vertical-align", "top");
        break;
    case JustifyVerticalAlignment: // ok?
    case DistributedVerticalAlignment:
        cellStyle->addProperty("style:vertical-align", "top");
        cellStyle->addProperty("koffice:vertical-distributed", "distributed");
        wrapOption = 1;
        break;
    case NoVerticalAlignment:
    case BottomVerticalAlignment:
        cellStyle->addProperty("style:vertical-align", "bottom");
        break;
    default:;
    }

    if (wrapOption == 0 || wrapOption == 1)
        cellStyle->addProperty("fo:wrap-option", wrapOption ? "wrap" : "no-wrap");
}

//! See http://www.w3.org/TR/2001/REC-xsl-20011015/slice7.html#text-align
bool XlsxCellFormat::setupCellStyle(
    const XlsxStyles *styles,
    const /*QMap<QString, */MSOOXML::DrawingMLTheme/**>*/ *themes,
    KoGenStyle* cellStyle) const
{
    kDebug() << "fontId:" << fontId << "fillId:" << fillId << "borderId:" << borderId;
    if (applyAlignment) {
        setupCellStyleAlignment(cellStyle);
    }
    if (applyFont && fontId >= 0) {
        XlsxFontStyle* fontStyle = styles->fontStyle(fontId);
        if (!fontStyle) {
            kWarning() << "No font with ID:" << fontId;
            return false;
        }
        fontStyle->setupCellTextStyle(themes, cellStyle);
    }
    if (applyFill && fillId >= 0) {
        XlsxFillStyle *fillStyle = styles->fillStyle(fillId);
        if (!fillStyle) {
            kWarning() << "No fill with ID:" << fillId;
            return false;
        }
        fillStyle->setupCellStyle(cellStyle, themes);
    }
    if (applyBorder && borderId >= 0) {
        XlsxBorderStyles *borderStyles = styles->borderStyle(borderId);
        if (borderStyles) {
            borderStyles->setupCellStyle(cellStyle, themes);
        }
    }
    return true;
}

//----------------------------------------------------------

XlsxXmlStylesReaderContext::XlsxXmlStylesReaderContext(XlsxStyles& _styles)
        : styles(&_styles)
{
}

class XlsxXmlStylesReader::Private
{
public:
    Private() {
    }
    ~Private() {
    }
private:
};

XlsxXmlStylesReader::XlsxXmlStylesReader(KoOdfWriters *writers)
        : MSOOXML::MsooXmlReader(writers)
        , m_context(0)
        , d(new Private)
{
    init();
}

XlsxXmlStylesReader::~XlsxXmlStylesReader()
{
    delete d;
    delete m_currentFontStyle;
    delete m_currentFillStyle;
    delete m_currentCellFormat;
}

void XlsxXmlStylesReader::init()
{
    m_defaultNamespace = "";
    m_fontStyleIndex = 0;
    m_fillStyleIndex = 0;
    m_cellFormatIndex = 0;
    m_borderStyleIndex = 0;
    m_currentColorStyle = 0;
    m_currentFontStyle = 0;
    m_currentFillStyle = 0;
    m_currentCellFormat = 0;
}

KoFilter::ConversionStatus XlsxXmlStylesReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<XlsxXmlStylesReaderContext*>(context);
    Q_ASSERT(m_context);
    const KoFilter::ConversionStatus result = readInternal();
    m_context = 0;
    if (result == KoFilter::OK)
        return KoFilter::OK;
    return result;
}

KoFilter::ConversionStatus XlsxXmlStylesReader::readInternal()
{
    kDebug() << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    // sst
    readNext();
    kDebug() << *this << namespaceUri();

    if (!expectEl("styleSheet")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::spreadsheetml)) {
        return KoFilter::WrongFormat;
    }

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    for (int i = 0; i < namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration(QString(), MSOOXML::Schemas::spreadsheetml))) {
        raiseError(i18n("Namespace \"%1\" not found", MSOOXML::Schemas::spreadsheetml));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    TRY_READ(styleSheet)
    kDebug() << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL styleSheet
//! styleSheet handler (Style Sheet)
/*! ECMA-376, 18.8.39, p. 1993.
 This is the root element of the Styles part.

 Child elements:
 - [done] borders (Borders) §18.8.5
 - cellStyles (Cell Styles) §18.8.8
 - cellStyleXfs (Formatting Records) §18.8.9
 - [done] cellXfs (Cell Formats) §18.8.10
 - colors (Colors) §18.8.11
 - dxfs (Formats) §18.8.15
 - extLst (Future Feature Data Storage Area) §18.2.10
 - [done] fills (Fills) §18.8.21
 - [done] fonts (Fonts) §18.8.23
 - [done] numFmts (Number Formats) §18.8.31
 - tableStyles (Table Styles) §18.8.42
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_styleSheet()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(fonts)
            ELSE_TRY_READ_IF(fills)
            ELSE_TRY_READ_IF(numFmts)
            ELSE_TRY_READ_IF(cellXfs)
            ELSE_TRY_READ_IF(borders)
//! @todo add ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL fonts
//! fonts handler (Fonts)
/*! ECMA-376, 18.8.23, p. 1964.
 This element contains all font definitions for this workbook.

 Child elements:
 - [done] font (Font) §18.8.22
 Parent elements:
 - [done] styleSheet (§18.8.39)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_fonts()
{
    READ_PROLOGUE
    if (!m_context->styles->fontStyles.isEmpty()) {
        raiseUnexpectedSecondOccurenceOfElError(STRINGIFY(CURRENT_EL));
    }
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(count)
    uint countNumber = 0;
    STRING_TO_INT(count, countNumber, "styleSheet/fonts@count")
    m_context->styles->fontStyles.resize(countNumber);
    m_fontStyleIndex = 0;

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(font)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL numFmts
//! numFmts handler (Number formats)
/*! ECMA-376, 18.8.31, p. 1981.
 This element defines the number formats in this workbook, consisting of a sequence of numFmt records,
 where each numFmt record defines a particular number format, indicating how to format and render
 the numeric value of a cell.

 Child elements:
 - [done] numFmt (Format Definition) §18.8.30
 Parent elements:
 - [done] styleSheet (§18.8.39)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_numFmts()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS( count )
    int countNumber = 0;
    STRING_TO_INT( count, countNumber, "styleSheet/numFmts@count" );

    while( !atEnd() )
    {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if( isStartElement() )
        {
            TRY_READ_IF( numFmt )
            ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL numFmt
//! numFmt handler (Number format)
/*! ECMA-376, 18.8.30, p. 1973.
 This element specifies number format properties which indicate how to format and render
 the numeric value of a cell.

 No child elements.

 Parent elements:
 - dxf (§18.8.14)
 - ndxf (§18.11.1.4)
 - [done] numFmts (§18.8.31)
 - odxf (§18.11.1.6)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_numFmt()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS( numFmtId )
    int id = -1;
    STRING_TO_INT(numFmtId, id, "numFmt@numFmtId")

    TRY_READ_ATTR_WITHOUT_NS( formatCode );
    if (id >= 0 && !formatCode.isEmpty()) {
        m_context->styles->numberFormatStrings[ id ] = formatCode;
    }

    while( true )
    {
        readNext();
        BREAK_IF_END_OF( CURRENT_EL );
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL font
//! font handler (Font)
/*! ECMA-376, 18.8.22, p. 1964.
 This element defines the properties for one of the fonts used in this workbook.

 Child elements:
 - [done] b (Bold) §18.8.2
 - charset (Character Set) §18.4.1
 - color (Data Bar Color) §18.3.1.15
 - condense (Condense) §18.8.12
 - extend (Extend) §18.8.17
 - family (Font Family) §18.8.18
 - [done] i (Italic) §18.8.26
 - [done] name (Font Name) §18.8.29
 - outline (Outline) §18.4.2
 - scheme (Scheme) §18.8.35
 - shadow (Shadow) §18.8.36
 - [done] strike (Strike Through) §18.4.10
 - [done] sz (Font Size) §18.4.11
 - [done] u (Underline) §18.4.13
 - vertAlign (Vertical Alignment) §18.4.14
 Parent elements:
 - dxf (§18.8.14)
 - [done] fonts (§18.8.23)
 - ndxf (§18.11.1.4)
 - odxf (§18.11.1.6)

 @todo support all child elements
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_font()
{
    READ_PROLOGUE

    if (m_fontStyleIndex >= (uint)m_context->styles->fontStyles.size()) {
        raiseError(i18n("Declared number of font styles too small (%1)", m_context->styles->fontStyles.size()));
        return KoFilter::WrongFormat;
    }

    kDebug() << "font #" << m_fontStyleIndex;
    m_currentFontStyle = new XlsxFontStyle;
    MSOOXML::Utils::AutoPtrSetter<XlsxFontStyle> currentFontStyleSetter(m_currentFontStyle);

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(sz)
            ELSE_TRY_READ_IF(name)
            ELSE_TRY_READ_IF(b)
            ELSE_TRY_READ_IF(i)
            ELSE_TRY_READ_IF(strike)
            ELSE_TRY_READ_IF(u)
            else if (QUALIFIED_NAME_IS(color)) {
                m_currentColorStyle = &m_currentFontStyle->color;
                TRY_READ(color)
                m_currentColorStyle = 0;
            }
            ELSE_TRY_READ_IF(vertAlign)
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE_WITHOUT_RETURN

    currentFontStyleSetter.release();
    m_context->styles->fontStyles[m_fontStyleIndex] = m_currentFontStyle;
    m_currentFontStyle = 0;
    m_fontStyleIndex++;

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL sz
//! sz handler (Font Size)
/*! ECMA-376, 18.4.11, p. 1913.
 This element represents the point size (1/72 of an inch) of the Latin and East Asian text.

 No child elements.
 Parent elements:
 - [done] font (§18.8.22)
 - rPr (§18.4.7)

 @todo support all elements
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_sz()
{
    Q_ASSERT(m_currentFontStyle);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)
    bool ok;
    m_currentFontStyle->setSize(val.toDouble(&ok));
    if (!ok) {
        raiseUnexpectedAttributeValueError(val, "sz");
        return KoFilter::WrongFormat;
    }

    while (true) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL vertAlign
//! vertAlign handler (Vertical Alignment)
/*! ECMA-376, ???

 No child elements.
 Parent elements:
 - [done] font (§18.8.22)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_vertAlign()
{
    Q_ASSERT(m_currentFontStyle);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(val)
    m_currentFontStyle->vertAlign = ST_VerticalAlignRun( val );

    while (true) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}
#undef CURRENT_EL
#define CURRENT_EL name
//! name handler (Font Name)
/*! ECMA-376, 18.8.29, p. 1973.
 This element specifies the face name of this font.

 No child elements.
 Parent elements:
 - [done] font (§18.8.22)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_name()
{
    Q_ASSERT(m_currentFontStyle);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS_INTO(val, m_currentFontStyle->name)

    while (true) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL b
//! b handler (Bold)
/*! ECMA-376, 18.8.2, p. 1947.
 Displays characters in bold face font style.

 No child elements.
 Parent elements:
 - [done] font (§18.8.22)
 - rPr (§18.4.7)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_b()
{
    Q_ASSERT(m_currentFontStyle);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    m_currentFontStyle->bold = readBooleanAttr("val", true);

    while (true) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL i
//! i handler (Italic)
/*! ECMA-376, 18.8.26, p. 1969.
 Displays characters in italic font style.

 No child elements.
 Parent elements:
 - [done] font (§18.8.22)
 - rPr (§18.4.7)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_i()
{
    Q_ASSERT(m_currentFontStyle);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    m_currentFontStyle->italic = readBooleanAttr("val", true);

    while (true) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL strike
//! strike handler (Strike Through)
/*! ECMA-376, 18.4.10, p. 1913.
 This element draws a strikethrough line through the horizontal middle of the text.

 No child elements.
 Parent elements:
 - [done] font (§18.8.22)
 - rPr (§18.4.7)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_strike()
{
    Q_ASSERT(m_currentFontStyle);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    m_currentFontStyle->strike = readBooleanAttr("val", true);

    while (true) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL u
//! u handler (Underline)
/*! ECMA-376, 18.4.13, p. 1914.
 This element represents the underline formatting style.

 No child elements.
 Parent elements:
 - [done] font (§18.8.22)
 - rPr (§18.4.7)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_u()
{
    Q_ASSERT(m_currentFontStyle);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    m_currentFontStyle->setUnderline(attrs.value("val").toString());

    while (true) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL color
//! color handler (Data Bar Color)
/*! ECMA-376, 18.3.1.15, p. 1780.
 One of the colors associated with the data bar or color scale.
 The auto attribute shall not be used in the context of data bars.

 Child elements:
 - stop (Gradient Stop) §18.8.38
 Parent elements:
 - [done] bottom (§18.8.6)
 - colorScale (§18.3.1.16)
 - dataBar (§18.3.1.28)
 - [done] diagonal (§18.8.13)
 - end (§18.8.16)
 - [done] font (§18.8.22)
 - horizontal (§18.8.25)
 - mruColors (§18.8.28)
 - rPr (§18.4.7)
 - start (§18.8.37)
 - stop (§18.8.38)
 - [done] top (§18.8.43)
 - vertical (§18.8.44)
 - [done] left
 - [done] right

 @todo support all elements
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_color()
{
    Q_ASSERT(m_currentColorStyle);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    RETURN_IF_ERROR( m_currentColorStyle->readAttributes(attrs, "color") )

    while (true) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL cellXfs
//! cellXfs handler (Cell Formats)
/*! ECMA-376, 18.8.10, p. 1956.
 This element contains the master formatting records (xf) which define
 the formatting applied to cells in this workbook.

 Child elements:
 - [done] xf (Format) §18.8.45
 Parent elements:
 - [done] styleSheet (§18.8.39)

 @note cellStyleXfs: the standard states that both the cell style xf records and cell xf records
       must be read to understand the full set of formatting applied to a cell.
       In MSO, only the cell xf record defines the formatting applied to a cell.
       See <a href="http://www.documentinteropinitiative.org/implnotes/implementationnotelist.aspx?id=dd2615fe-aa8d-4a06-a415-13389919cf36&specname=ecma-376">here</a>.
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_cellXfs()
{
    READ_PROLOGUE
    if (!m_context->styles->cellFormats.isEmpty()) {
        raiseUnexpectedSecondOccurenceOfElError(STRINGIFY(CURRENT_EL));
    }
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(count)
    uint countNumber = 0;
    STRING_TO_INT(count, countNumber, "styleSheet/cellXfs@count")
    m_context->styles->cellFormats.resize(countNumber);
    m_cellFormatIndex = 0;

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(xf)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL xf
//! xf handler (Format)
/*! ECMA-376, 18.8.45, p. 1999.
 A single xf element describes all of the formatting for a cell.

 Child elements:
 - [done] alignment (Alignment) §18.8.1
 Parent elements:
 - cellStyleXfs (§18.8.9)
 - [done] cellXfs (§18.8.10)
 - extLst (Future Feature Data Storage Area) §18.2.10
 - protection (Protection Properties) §18.8.33

 Attributes:
 - applyAlignment
 - applyBorder
 - applyFill
 - applyFont
 - applyNumberFormat
 - applyProtection
 - [done] borderId
 - fillId
 - [done] fontId
 - numFmtId
 - pivotButton
 - quotePrefix
 - xfId

 @todo support all attributes and elements
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_xf()
{
    READ_PROLOGUE

    if (m_cellFormatIndex >= (uint)m_context->styles->cellFormats.size()) {
        raiseError(i18n("Declared number of cell formats too small (%1)", m_context->styles->cellFormats.size()));
        return KoFilter::WrongFormat;
    }

    kDebug() << "cell format #" << m_cellFormatIndex;
    m_currentCellFormat = new XlsxCellFormat;
    MSOOXML::Utils::AutoPtrSetter<XlsxCellFormat> currentCellFormatSetter(m_currentCellFormat);

    // -- read attrs --
    const QXmlStreamAttributes attrs(attributes());
    m_currentCellFormat->applyAlignment = readBooleanAttr("applyAlignment", true);
    m_currentCellFormat->applyBorder = readBooleanAttr("applyBorder", true);
    m_currentCellFormat->applyFill = readBooleanAttr("applyFill", true);
    m_currentCellFormat->applyFont = readBooleanAttr("applyFont", true);
    m_currentCellFormat->applyNumberFormat = readBooleanAttr("applyNumberFormat", true);
    m_currentCellFormat->applyProtection = readBooleanAttr("applyProtection", true);

    TRY_READ_ATTR_WITHOUT_NS(borderId)
    STRING_TO_INT(borderId, m_currentCellFormat->borderId, "xf@borderId")

    TRY_READ_ATTR_WITHOUT_NS(fillId)
    STRING_TO_INT(fillId, m_currentCellFormat->fillId, "xf@fillId")

    TRY_READ_ATTR_WITHOUT_NS(fontId)
    STRING_TO_INT(fontId, m_currentCellFormat->fontId, "xf@fontId")

    TRY_READ_ATTR_WITHOUT_NS(numFmtId)
    STRING_TO_INT(numFmtId, m_currentCellFormat->numFmtId, "xf@numFmtId")

    m_currentCellFormat->pivotButton = readBooleanAttr("pivotButton");
    m_currentCellFormat->quotePrefix = readBooleanAttr("quotePrefix");

    TRY_READ_ATTR_WITHOUT_NS(xfId)
    STRING_TO_INT(xfId, m_currentCellFormat->xfId, "xf@xfId")

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(alignment)
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE_WITHOUT_RETURN

    currentCellFormatSetter.release();
    m_context->styles->setCellFormat(m_currentCellFormat, m_cellFormatIndex);
    m_currentCellFormat = 0;
    m_cellFormatIndex++;

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL alignment
//! alignment handler (Alignment)
/*! ECMA-376, 18.8.1, p. 1944.
 Formatting information pertaining to text alignment in cells.

 No child elements.
 Parent elements:
 - dxf (§18.8.14)
 - ndxf (§18.11.1.4)
 - odxf (§18.11.1.6)
 - [done] xf (§18.8.45)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_alignment()
{
    Q_ASSERT(m_currentCellFormat);

    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    m_currentCellFormat->setHorizontalAlignment(attrs.value("horizontal").toString());
    kDebug() << "horizontalAlignment:" << m_currentCellFormat->horizontalAlignment;
    m_currentCellFormat->setVerticalAlignment(attrs.value("vertical").toString());
    kDebug() << "verticalAlignment:" << m_currentCellFormat->verticalAlignment;
    const bool wrap = readBooleanAttr("wrapText", false);
    m_currentCellFormat->wrapText = wrap;
    const bool shrinkToFit = readBooleanAttr("shrinkToFit", false);
    m_currentCellFormat->shrinkToFit = shrinkToFit;
    const uint textRotation = attributes().value("textRotation").toString().toUInt();
    m_currentCellFormat->textRotation = textRotation;

//! @todo more attributes

    while (true) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL fills
//! fills handler (Fills)
/*! ECMA-376, 18.8.21, p. 1963.
 This element defines the cell fills portion of the Styles part, consisting of a sequence of fill records.
 A cell fill consists of a background color, foreground color, and pattern to be applied across the cell.

 Child elements:
 - [done] fill (Fill) §18.8.20
 Parent elements:
 - [done] styleSheet (§18.8.39)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_fills()
{
    READ_PROLOGUE
    if (!m_context->styles->fillStyles.isEmpty()) {
        raiseUnexpectedSecondOccurenceOfElError(STRINGIFY(CURRENT_EL));
    }
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(count)
    uint countNumber = 0;
    STRING_TO_INT(count, countNumber, "styleSheet/fills@count")
    m_context->styles->fillStyles.resize(countNumber);
    m_fillStyleIndex = 0;

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(fill)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL fill
//! fill handler (Fill)
/*! ECMA-376, 18.8.20, p. 1962.
 This element specifies fill formatting.

 Child elements:
 - gradientFill (Gradient) §18.8.24
 - [done] patternFill (Pattern) §18.8.32
 Parent elements:
 - dxf (§18.8.14)
 - [done] fills (§18.8.21)
 - ndxf (§18.11.1.4)
 - odxf (§18.11.1.6)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_fill()
{
    READ_PROLOGUE

    if (m_fillStyleIndex >= (uint)m_context->styles->fillStyles.size()) {
        raiseError(i18n("Declared number of fill styles too small (%1)", m_context->styles->fillStyles.size()));
        return KoFilter::WrongFormat;
    }

    kDebug() << "fill #" << m_fillStyleIndex;
    m_currentFillStyle = new XlsxFillStyle;
    MSOOXML::Utils::AutoPtrSetter<XlsxFillStyle> currentFillStyleSetter(m_currentFillStyle);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(gradientFill)
            ELSE_TRY_READ_IF(patternFill)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE_WITHOUT_RETURN

    currentFillStyleSetter.release();
    m_context->styles->fillStyles[m_fillStyleIndex] = m_currentFillStyle;
    m_currentFillStyle = 0;
    m_fillStyleIndex++;

    return KoFilter::OK;
}

//! Handles patternType
void XlsxXmlStylesReader::handlePatternType(const QString& patternType)
{
    const QByteArray p(patternType.toLatin1());
//kDebug() << p;
    if (p.isEmpty() || p == MsooXmlReader::constNone) {
        // 100% background
        m_currentFillStyle->patternType = XlsxFillStyle::NonePatternType;
    }
    else if (p == "solid") {
        // 100% foreground
        m_currentFillStyle->patternType = XlsxFillStyle::SolidPatternType;
    }
    else if (p.startsWith("dark")) {
        if (p == "darkDown") {
            m_currentFillStyle->patternType = XlsxFillStyle::DarkDownPatternType;
        }
        else if (p == "darkGray") {
            m_currentFillStyle->patternType = XlsxFillStyle::DarkGrayPatternType;
        }
        else if (p == "darkGrid") {
            m_currentFillStyle->patternType = XlsxFillStyle::DarkGridPatternType;
        }
        else if (p == "darkHorizontal") {
            m_currentFillStyle->patternType = XlsxFillStyle::DarkGridPatternType;
        }
        else if (p == "darkTrellis") {
            m_currentFillStyle->patternType = XlsxFillStyle::DarkTrellisPatternType;
        }
        else if (p == "darkUp") {
            m_currentFillStyle->patternType = XlsxFillStyle::DarkUpPatternType;
        }
        else if (p == "darkVertical") {
            m_currentFillStyle->patternType = XlsxFillStyle::DarkVerticalPatternType;
        }
    }
    else if (p.startsWith("light")) {
        if (p == "lightDown") {
            m_currentFillStyle->patternType = XlsxFillStyle::LightDownPatternType;
        }
        else if (p == "lightGray") {
            m_currentFillStyle->patternType = XlsxFillStyle::LightGrayPatternType;
        }
        else if (p == "lightGrid") {
            m_currentFillStyle->patternType = XlsxFillStyle::LightGridPatternType;
        }
        else if (p == "lightHorizontal") {
            m_currentFillStyle->patternType = XlsxFillStyle::LightHorizontalPatternType;
        }
        else if (p == "lightTrellis") {
            m_currentFillStyle->patternType = XlsxFillStyle::LightTrellisPatternType;
        }
        else if (p == "lightUp") {
            m_currentFillStyle->patternType = XlsxFillStyle::LightUpPatternType;
        }
        else if (p == "lightVertical") {
            m_currentFillStyle->patternType = XlsxFillStyle::LightVerticalPatternType;
        }
    }
    else if (p == "mediumGray") {
        m_currentFillStyle->patternType = XlsxFillStyle::MediumGrayPatternType;
    }
    else if (p == "gray0625") {
        m_currentFillStyle->patternType = XlsxFillStyle::Gray0625PatternType;
    }
    else if (p == "gray125") {
        m_currentFillStyle->patternType = XlsxFillStyle::Gray125PatternType;
    }
    else {
        kWarning() << "unknown value" << p
            << "of patterFill@patternType; defaulting to \"none\"";
        m_currentFillStyle->patternType = XlsxFillStyle::NonePatternType;
    }
}

#undef CURRENT_EL
#define CURRENT_EL patternFill
//! patternFill handler (Pattern)
/*! ECMA-376, 18.8.22, p. 1990.
 This element is used to specify cell fill information for pattern and solid color cell fills.

 Child elements:
 - [done] bgColor (Background Color) §18.8.3
 - [done] fgColor (Foreground Color) §18.8.19
 Parent elements:
 - [done] fill (§18.8.20)
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_patternFill()
{
    Q_ASSERT(m_currentFillStyle);
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(patternType)
    handlePatternType(patternType);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(bgColor)
            ELSE_TRY_READ_IF(fgColor)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bgColor
//! bgColor handler (Background Color)
/*! ECMA-376, 18.8.3, p. 1948.
 Background color of the cell fill pattern. Cell fill patterns operate with two colors:
 a background color and a foreground color. These combine together to make a patterned cell fill.

 No child elements.
 Parent elements:
 - [done] patternFill (§18.8.20)

 @todo support all elements
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_bgColor()
{
    Q_ASSERT(m_currentFillStyle);
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    RETURN_IF_ERROR( m_currentFillStyle->bgColor.readAttributes(attrs, "bgColor") )

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL fgColor
//! fgColor handler (Foreground  Color)
/*! ECMA-376, 18.8.19, p. 1961.
 Foreground color of the cell fill pattern. Cell fill patterns operate with two colors:
 a background color and a foreground color. These combine together to make a patterned cell fill.

 No child elements.
 Parent elements:
 - [done] patternFill (§18.8.20)

 @todo support all elements
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_fgColor()
{
    Q_ASSERT(m_currentFillStyle);
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    RETURN_IF_ERROR( m_currentFillStyle->fgColor.readAttributes(attrs, "fgColor") )

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL gradientFill
//! gradientFill handler (Pattern)
/*! ECMA-376, 18.8.24, p. 1965.
 This element defines a gradient-style cell fill.
 Gradient cell fills can use one or two colors as the end points of color interpolation.

 Child elements:
 - stop (Gradient Stop) §18.8.38
 Parent elements:
 - [done] fill (§18.8.20)

 @todo support all elements
*/
KoFilter::ConversionStatus XlsxXmlStylesReader::read_gradientFill()
{
    Q_ASSERT(m_currentFillStyle);
    READ_PROLOGUE

//! @todo read attributes for gradientFill

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
//! @todo            TRY_READ_IF(stop)
//todo            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL borders
//! 18.8.5 borders (Borders), p. 1951
//! @todo support all elements
KoFilter::ConversionStatus XlsxXmlStylesReader::read_borders()
{
    READ_PROLOGUE
    if (!m_context->styles->borderStyles.isEmpty()) {
        raiseUnexpectedSecondOccurenceOfElError(STRINGIFY(CURRENT_EL));
    }
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(count)
    uint countNumber = 0;
    STRING_TO_INT(count, countNumber, "styleSheet/borders@count")
    m_context->styles->borderStyles.resize(countNumber);
    m_borderStyleIndex = 0;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(border)
//todo            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL border
//! 18.8.4 border (Border), p. 1949
//! @todo support all elements
KoFilter::ConversionStatus XlsxXmlStylesReader::read_border()
{
    READ_PROLOGUE
    if (m_borderStyleIndex >= (uint)m_context->styles->borderStyles.size()) {
        raiseError(i18n("Declared number of border styles too small (%1)", m_context->styles->borderStyles.size()));
        return KoFilter::WrongFormat;
    }

    kDebug() << "border #" << m_borderStyleIndex;
    m_currentBorderStyle = new XlsxBorderStyles;
    MSOOXML::Utils::AutoPtrSetter<XlsxBorderStyles> currentBorderStyleSetter(m_currentBorderStyle);

    if (readBooleanAttr("diagonalUp")) {
        m_currentBorderStyle->diagonalDirections |= XlsxBorderStyles::DiagonalUp;
    }
    if (readBooleanAttr("diagonalDown")) {
        m_currentBorderStyle->diagonalDirections |= XlsxBorderStyles::DiagonalDown;
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(bottom)
            ELSE_TRY_READ_IF(diagonal)
//! @todo            ELSE_TRY_READ_IF(end)
//! @todo (dxf only)            ELSE_TRY_READ_IF(horizontal)
            ELSE_TRY_READ_IF(left)
//! @todo            ELSE_TRY_READ_IF(start)
            ELSE_TRY_READ_IF(right)
            ELSE_TRY_READ_IF(top)
//! @todo (dxf only)            ELSE_TRY_READ_IF(vertical)
//todo            ELSE_WRONG_FORMAT
        }
    }

    currentBorderStyleSetter.release();
    m_context->styles->borderStyles[m_borderStyleIndex] = m_currentBorderStyle;
    m_currentBorderStyle = 0;
    m_borderStyleIndex++;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bottom
//! 18.8.6 bottom (Bottom Border), p. 1952
//! @todo support all elements
KoFilter::ConversionStatus XlsxXmlStylesReader::read_bottom()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    RETURN_IF_ERROR( m_currentBorderStyle->bottom.readAttributes(attrs) )

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(color)) {
                m_currentColorStyle = &m_currentBorderStyle->bottom.color;
                TRY_READ(color)
                m_currentColorStyle = 0;
            }
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL top
//! 18.8.6 bottom (Bottom Border), p. 1952
//! @todo support all elements
KoFilter::ConversionStatus XlsxXmlStylesReader::read_top()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    RETURN_IF_ERROR( m_currentBorderStyle->top.readAttributes(attrs) )

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(color)) {
                m_currentColorStyle = &m_currentBorderStyle->top.color;
                TRY_READ(color)
                m_currentColorStyle = 0;
            }
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL left
//! @todo support all elements
KoFilter::ConversionStatus XlsxXmlStylesReader::read_left()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    RETURN_IF_ERROR( m_currentBorderStyle->left.readAttributes(attrs) )

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(color)) {
                m_currentColorStyle = &m_currentBorderStyle->left.color;
                TRY_READ(color)
                m_currentColorStyle = 0;
            }
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL right
//! @todo support all elements
KoFilter::ConversionStatus XlsxXmlStylesReader::read_right()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    RETURN_IF_ERROR( m_currentBorderStyle->right.readAttributes(attrs) )

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(color)) {
                m_currentColorStyle = &m_currentBorderStyle->right.color;
                TRY_READ(color)
                m_currentColorStyle = 0;
            }
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL diagonal
//! @todo support all elements
KoFilter::ConversionStatus XlsxXmlStylesReader::read_diagonal()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    RETURN_IF_ERROR( m_currentBorderStyle->diagonal.readAttributes(attrs) )

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            if (QUALIFIED_NAME_IS(color)) {
                m_currentColorStyle = &m_currentBorderStyle->diagonal.color;
                TRY_READ(color)
                m_currentColorStyle = 0;
            }
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}
