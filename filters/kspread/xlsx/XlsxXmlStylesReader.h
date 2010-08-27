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

#ifndef XLSXXMLSTYLESREADER_H
#define XLSXXMLSTYLESREADER_H

#include <MsooXmlThemesReader.h>
#include <QtCore/QFlags>
#include <QColor>

class KoCharacterStyle;
class KoGenStyle;
class XlsxImport;
class XlsxStyles;
class XlsxCellFormat;
class XlsxXmlStylesReader;

//! Color information. Used by background and foregound color for XlsxFillStyle, by color of XlsxFontStyle
//! See MSOOXML's CT_Color content model
class XlsxColorStyle
{
public:
    XlsxColorStyle();

    //! Clears the style to default values.
    void clear();

    //! @return true if this color style is valid
    bool isValid(const /*QMap<QString, */MSOOXML::DrawingMLTheme/**>*/ *themes) const;

    //! @return value of this color style; for computing rgb, indexed, tint and theme attributes are used
//! @todo use indexed
    QColor value(const /*QMap<QString, */MSOOXML::DrawingMLTheme/**>*/ *themes) const;

    KoFilter::ConversionStatus readAttributes(const QXmlStreamAttributes& attrs, const char* debugElement);

    bool automatic; //!< default: false
    int indexed; //!< default: -1
    QColor rgb;
    qreal tint; //!< tint value applied to the color, default is -1
    int theme; //!< default: -1

    QColor themeColor(const /*QMap<QString, */MSOOXML::DrawingMLTheme/**>*/ *themes) const;
};

//! 22.9.2.17 ST_VerticalAlignRun (Vertical Positioning Location)
struct ST_VerticalAlignRun
{
    enum Value {
        BaselineVerticalAlignRun, //!< default
        SubscriptVerticalAlignRun,
        SuperscriptVerticalAlignRun
    };
    ST_VerticalAlignRun(const QString& msooxmlName = QString());
    //! Sets up @a characterStyle to match this setting.
    void setupCharacterStyle(KoCharacterStyle* characterStyle) const;

    Value value;
};

//! Single XLSX font style definition as specified in ECMA-376, 18.8.23 (Fonts), p. 1964.
/*! @see XlsxXmlStylesReader::read_fonts() */
class XlsxFontStyle
{
public:
    XlsxFontStyle();

    //! 18.18.85 ST_UnderlineValues (Underline Types)
    //! Represents the different types of possible underline formatting.
    //! Used by u@val (§18.4.13) - SpreadsheetML
    //! CASE #S1730
    enum ST_UnderlineValue {
        NoUnderline,
        SingleUnderline, //!< The default
        DoubleUnderline,
        SingleAccountingUnderline,
        DoubleAccountingUnderline
    };

    QString name;
    ST_UnderlineValue underline;
//! @todo charset
    XlsxColorStyle color;
//! @todo bool condense (Mac)
//! @todo bool shadow (Mac)
//! @todo extend
//! @todo family
//! @todo bool outline;
//! @todo QString scheme;

    ST_VerticalAlignRun vertAlign;

    bool bold;
    bool italic;
    bool strike;

    static ST_UnderlineValue ST_UnderlineValue_fromString(const QString& s);
    void setUnderline(const QString& s);

    void setSize(qreal size) {
        m_defaultSize = false;
        m_size = size;
    }

    qreal size() const { return m_size; }

    //! Sets up @a cellStyle to match this cell text style.
    //! @todo implement more styling
    void setupCellTextStyle(const /*QMap<QString,*/ MSOOXML::DrawingMLTheme/**>*/ *themes, KoGenStyle* cellStyle) const;

    //! Sets up @a characterStyle to match this font style.
    //! @todo implement more formatting
    void setupCharacterStyle(KoCharacterStyle* characterStyle) const;

private:
    qreal m_size;
    bool m_defaultSize;
};

//! @return QColor value for  ST_UnsignedIntHex (ARGB) (e.g. for 18.8.19 fgColor (Foreground Color) - SpreadsheetML only)
//!         or invalid color if @a color is not in the expected format.
//! @par val color value in AARRGGBB hexadecimal format
inline QColor ST_UnsignedIntHex_to_QColor(const QString& color)
{
    if (color.length() != 8)
        return QColor();
    bool ok;
    const uint rgb = color.toUInt(&ok, 16);
    if (!ok)
        return QColor();
    QColor c;
    c.setRgba(QRgb(rgb));
//kDebug() << color << c;
    return c;
}

//! Single XLSX fill style definition as specified in ECMA-376, ECMA-376, 18.8.21, p. 1963.
/*! @see XlsxXmlStylesReader::read_fills() */
class XlsxFillStyle
{
public:
    XlsxFillStyle();
    ~XlsxFillStyle();

    //! 18.18.55 ST_PatternType (Pattern Type), p. 2713
    /*! Indicates the style of fill pattern being used for a cell format.
        Compare to MSOOXML::DrawingMLPatternFillStyle which is different pattern type. */
    enum ST_PatternType {
        NonePatternType,
        SolidPatternType,
        DarkDownPatternType,
        DarkGrayPatternType,
        DarkGridPatternType,
        DarkHorizontalPatternType,
        DarkTrellisPatternType,
        DarkUpPatternType,
        DarkVerticalPatternType,
        LightPatternType,
        LightDownPatternType,
        LightGrayPatternType,
        LightGridPatternType,
        LightHorizontalPatternType,
        LightTrellisPatternType,
        LightUpPatternType,
        LightVerticalPatternType,
        MediumGrayPatternType,
        Gray0625PatternType,
        Gray125PatternType
    };

    ST_PatternType patternType;
    XlsxColorStyle bgColor;
    XlsxColorStyle fgColor;

    //! Sets up @a cellStyle to match this style.
    //! @todo implement more styling
    void setupCellStyle(KoGenStyle* cellStyle, const /*QMap<QString, */MSOOXML::DrawingMLTheme/**>*/ *themes) const;

    //! @return color style (bgColor or fgColor) depending on the pattern
    //! Can return 0 if no fill should be painted.
    const XlsxColorStyle* realBackgroundColor( const /*QMap<QString,*/ MSOOXML::DrawingMLTheme/**>*/ *themes) const;

private:
    mutable XlsxColorStyle* cachedRealBackgroundColor;
};

//! Single XLSX border style definition
/*! @see XlsxXmlStylesReader::read_borders() */
class XlsxBorderStyle
{
public:
    XlsxBorderStyle();

    KoFilter::ConversionStatus readAttributes(const QXmlStreamAttributes& attrs);

    QString setupCellStyle(const /*QMap<QString, */MSOOXML::DrawingMLTheme/**>*/ *themes) const;

    XlsxColorStyle color;
private:
    QString style;
};

//! Four XLSX border styles definition
/*! @see XlsxXmlStylesReader::read_borders() */
class XlsxBorderStyles
{
public:
    XlsxBorderStyles();

    void setupCellStyle(KoGenStyle* cellStyle, const /*QMap<QString,*/ MSOOXML::DrawingMLTheme/**>*/ *themes) const;

    XlsxBorderStyle top;
    XlsxBorderStyle right;
    XlsxBorderStyle bottom;
    XlsxBorderStyle left;
    enum DiagonalDirection {
        DiagonalUp = 1,
        DiagonalDown = 2
    };
    Q_DECLARE_FLAGS(DiagonalDirections, DiagonalDirection)
    DiagonalDirections diagonalDirections;

    XlsxBorderStyle diagonal;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(XlsxBorderStyles::DiagonalDirections)

//! Single XLSX cell format definition as specified in ECMA-376, 18.8.10 (Cell Formats), p. 1956.
/*! @see XlsxXmlStylesReader::read_cellXfs() */
class XlsxCellFormat
{
public:
    XlsxCellFormat();

    ~XlsxCellFormat();

    //! 18.18.40 ST_HorizontalAlignment (Horizontal Alignment Type), p. 2698
    /*! The enumeration value indicating the portion of Cell Alignment in a cell format (XF)
        that is horizontal alignment, i.e., whether it is aligned general, left, right,
        horizontally centered, filled (replicated), justified, centered across multiple cells,
        or distributed. */
    enum ST_HorizontalAlignment {
        GeneralHorizontalAlignment,
        CenterHorizontalAlignment,
        CenterContinuousHorizontalAlignment,
        DistributedHorizontalAlignment,
        FillHorizontalAlignment,
        JustifyHorizontalAlignment,
        LeftHorizontalAlignment,
        RightHorizontalAlignment
    };

    //! 18.18.88 ST_VerticalAlignment (Vertical Alignment Types), p. 2755
    /*! This enumeration value indicates the type of vertical alignment for a cell,
        i.e., whether it is aligned top, bottom, vertically centered, justified or distributed. */
    enum ST_VerticalAlignment {
        NoVerticalAlignment,
        BottomVerticalAlignment,
        CenterVerticalAlignment,
        DistributedVerticalAlignment,
        JustifyVerticalAlignment,
        TopVerticalAlignment
    };

    //! Indicates whether the alignment formatting specified for this xf should be applied
//! @todo should be saved as metadata in ODF
    bool applyAlignment;

    //! Indicates whether the border formatting specified for this xf should be applied.
//! @todo should be saved as metadata in ODF
    bool applyBorder;

    //! Indicates whether the fill formatting specified for this xf should be applied.
//! @todo should be saved as metadata in ODF
    bool applyFill;

    //! Indicates whether the font formatting specified for this xf should be applied.
//! @todo should be saved as metadata in ODF
    bool applyFont;

    //! Indicates whether the number formatting specified for this xf should be applied.
    bool applyNumberFormat;

    //! Indicates whether the protection formatting specified for this xf should be applied.
    bool applyProtection;

    //! Zero-based index of the border record used by this cell format. Can be -1.
//! @todo set pointer directly here for optimization?
    int borderId;

    //! Zero-based index of the fill record used by this cell format. Can be -1.
//! @todo set pointer directly here for optimization?
    int fillId;

    //! Font id used in XlsXStyles::fontStyle(). Can be -1.
//! @todo set pointer directly here for optimization?
    int fontId;

    //! Id of the number format (numFmt) record used by this cell format. Can be -1.
//! @todo set pointer directly here for optimization?
    int numFmtId;

    //! Indicates whether the cell rendering includes a pivot table dropdown button.
    bool pivotButton;

    //! Indicates whether the text string in a cell should be prefixed by a single quote mark
    //! (e.g., 'text). In these cases, the quote is not stored in the Shared Strings Part.
    bool quotePrefix;

    //! For xf records contained in cellXfs this is the zero-based index of an xf record
    //! contained in cellStyleXfs corresponding to the cell style applied to the cell.
    //! Not present for xf records contained in cellStyleXfs. Can be -1.
//! @todo set pointer directly here for optimization?
    int xfId;

//! @todo should be saved as metadata in ODF if applyAlignment false
    ST_HorizontalAlignment horizontalAlignment;
//! @todo should be saved as metadata in ODF if applyAlignment false
    ST_VerticalAlignment verticalAlignment;

    //! Indicates whether if the text in the cell should be line-wrapped with the cell.
    bool wrapText;

    //! Inidicates whether the displayed text in a cell should be shrunk to fit the cell width.
    bool shrinkToFit;

    //! Text rotation in cells, expressed in degrees. (Undocumented? special case: 255 means
    //! vertical top-to-bottom-text without rotated characters
    uint textRotation;

    void setHorizontalAlignment(const QString& alignment);
    void setVerticalAlignment(const QString& alignment);

    //! Sets up @a cellStyle to match this cell style.
    //! @todo implement more styling
    bool setupCellStyle(
        const XlsxStyles *styles,
        const /*QMap<QString, */MSOOXML::DrawingMLTheme/**>*/ *themes,
        KoGenStyle* cellStyle) const;

    //! Sets up @a characterStyle to match this font style.
//! @todo implement more formatting
    bool setupCharacterStyle(const XlsxStyles *styles, KoCharacterStyle* characterStyle) const;

private:
    //! Used by setupCellStyle()
    void setupCellStyleAlignment(KoGenStyle* cellStyle) const;
};

class XlsxStyles
{
public:
    XlsxStyles();
    ~XlsxStyles();

    //! @return font style for id @a id (counted from 0)
    XlsxFontStyle* fontStyle(int id) const {
        if (id < 0 || id >= fontStyles.size())
            return 0;
        return fontStyles[id];
    }

    //! @return fill style for id @a id (counted from 0)
    XlsxFillStyle* fillStyle(int id) const {
        if (id < 0 || id >= fillStyles.size())
            return 0;
        return fillStyles[id];
    }

    //! @return border style for id @a id (counted from 0)
    XlsxBorderStyles* borderStyle(int id) const {
        if (id < 0 || id >= borderStyles.size())
            return 0;
        return borderStyles[id];
    }

    //! @return cell format for id @a id (counted from 0)
    XlsxCellFormat* cellFormat(int id) const {
        if (id < 0 || id >= cellFormats.size())
            return 0;
        return cellFormats[id];
    }

    //! @return number format string for id @a (counted from 0)
    QString numberFormatString( int id ) const 
    {
        return numberFormatStrings[ id ];
    }

protected:
    void setCellFormat(XlsxCellFormat *format, int cellFormatIndex);

    QVector<XlsxFontStyle*> fontStyles;
    QVector<XlsxFillStyle*> fillStyles;
    QVector<XlsxBorderStyles*> borderStyles;
    QVector<XlsxCellFormat*> cellFormats;
    QMap< int, QString > numberFormatStrings;

    friend class XlsxXmlStylesReader;
};

class XlsxXmlStylesReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    XlsxXmlStylesReaderContext(XlsxStyles& _styles);
    XlsxStyles* styles;
};

//! A class reading MSOOXML XLSX markup - styles.xml part.
//! See ECMA-376, 12.3.20: Styles Part, p. 104
class XlsxXmlStylesReader : public MSOOXML::MsooXmlReader
{
public:
    explicit XlsxXmlStylesReader(KoOdfWriters *writers);

    virtual ~XlsxXmlStylesReader();

    //! Reads/parses the file of format document.xml.
    //! The output goes mainly to KoXmlWriter* KoOdfWriters::body
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

protected:
    KoFilter::ConversionStatus readInternal();
    KoFilter::ConversionStatus read_styleSheet();
    KoFilter::ConversionStatus read_numFmts();
    KoFilter::ConversionStatus read_numFmt();
    KoFilter::ConversionStatus read_fonts();
    KoFilter::ConversionStatus read_font();
    KoFilter::ConversionStatus read_sz();
    KoFilter::ConversionStatus read_name();
    KoFilter::ConversionStatus read_b();
    KoFilter::ConversionStatus read_i();
    KoFilter::ConversionStatus read_strike();
    KoFilter::ConversionStatus read_u();
    KoFilter::ConversionStatus read_vertAlign();
    KoFilter::ConversionStatus read_color();
    KoFilter::ConversionStatus read_cellXfs();
    KoFilter::ConversionStatus read_xf();
    KoFilter::ConversionStatus read_alignment();
    KoFilter::ConversionStatus read_fills();
    KoFilter::ConversionStatus read_fill();
//! @todo implement read_patternFill
    KoFilter::ConversionStatus read_patternFill();
    KoFilter::ConversionStatus read_bgColor();
    KoFilter::ConversionStatus read_fgColor();
//! @todo implement read_gradientFill
    KoFilter::ConversionStatus read_gradientFill();
//! @todo implement read_stop()
    KoFilter::ConversionStatus read_borders();
    KoFilter::ConversionStatus read_border();
    KoFilter::ConversionStatus read_top();
    KoFilter::ConversionStatus read_bottom();
    KoFilter::ConversionStatus read_left();
    KoFilter::ConversionStatus read_right();
    KoFilter::ConversionStatus read_diagonal();

    uint m_fontStyleIndex;
    uint m_fillStyleIndex;
    uint m_cellFormatIndex;
    uint m_borderStyleIndex;

    XlsxXmlStylesReaderContext* m_context;

    XlsxColorStyle *m_currentColorStyle; //!< set by read_color()
    XlsxFontStyle *m_currentFontStyle;
    XlsxFillStyle *m_currentFillStyle;
    XlsxCellFormat *m_currentCellFormat;
    XlsxBorderStyles *m_currentBorderStyle;

private:
    void init();

    void handlePatternType(const QString& patternType); //!< used by read_patternFill()

//! @todo readTintAttribute(const QXmlStreamAttributes& attrs) const;

    class Private;
    Private* const d;
};

#endif //XLSXXMLSTYLESREADER_H
