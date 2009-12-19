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

#include <MsooXmlReader.h>

class KoCharacterStyle;
class KoGenStyle;
class XlsxImport;
class XlsxStyles;
class XlsxCellFormat;
class XlsxXmlStylesReader;

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
        NoUnderline = 0,
        SingleUnderline, //!< The default
        DoubleUnderline,
        SingleAccountingUnderline,
        DoubleAccountingUnderline
    };

    QString name;
    ST_UnderlineValue underline;
//! @todo charset
//! @todo color
//! @todo bool condense (Mac)
//! @todo bool shadow (Mac)
//! @todo extend
//! @todo family
//! @todo bool outline;
//! @todo QString scheme;
//! @todo vertAlign
    bool bold;
    bool italic;
    bool strike;

    static ST_UnderlineValue ST_UnderlineValue_fromString(const QString& s);
    void setUnderline(const QString& s);

    void setSize(qreal size) {
        m_defaultSize = false;
        m_size = size;
    }

    qreal size() const {
        return m_size;
    }
protected:
    //! Sets up @a cellStyle to match this cell text style.
    //! @todo implement more styling
    void setupCellTextStyle(KoGenStyle* cellStyle) const;

    //! Sets up @a characterStyle to match this font style.
    //! @todo implement more formatting
    void setupCharacterStyle(KoCharacterStyle* characterStyle) const;

private:
    qreal m_size;
    bool m_defaultSize;

    friend class XlsxCellFormat;
};

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
        GeneralHorizontalAlignment = 0,
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
        NoVerticalAlignment = 0,
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

    void setHorizontalAlignment(const QString& alignment);
    void setVerticalAlignment(const QString& alignment);

    //! Sets up @a cellStyle to match this cell style.
//! @todo implement more styling
    bool setupCellStyle(KoGenStyle* cellStyle) const;

    //! Sets up @a characterStyle to match this font style.
//! @todo implement more formatting
    bool setupCharacterStyle(KoCharacterStyle* characterStyle) const;

protected:
    XlsxStyles *styles;

    friend class XlsxStyles;
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

    //! @return cell format for id @a id (counted from 0)
    XlsxCellFormat* cellFormat(int id) const {
        if (id < 0 || id >= cellFormats.size())
            return 0;
        return cellFormats[id];
    }
protected:
    void setCellFormat(XlsxCellFormat *format, int cellFormatIndex);

    QVector<XlsxFontStyle*> fontStyles;
    QVector<XlsxCellFormat*> cellFormats;

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
    KoFilter::ConversionStatus read_fonts();
    KoFilter::ConversionStatus read_font();
    KoFilter::ConversionStatus read_sz();
    KoFilter::ConversionStatus read_name();
    KoFilter::ConversionStatus read_b();
    KoFilter::ConversionStatus read_i();
    KoFilter::ConversionStatus read_strike();
    KoFilter::ConversionStatus read_u();
    KoFilter::ConversionStatus read_cellXfs();
    KoFilter::ConversionStatus read_xf();
    KoFilter::ConversionStatus read_alignment();

    uint m_fontStyleIndex;
    uint m_cellFormatIndex;

    typedef KoFilter::ConversionStatus(XlsxXmlStylesReader::*ReadMethod)();
    QStack<ReadMethod> m_calls;

    XlsxXmlStylesReaderContext* m_context;

    XlsxFontStyle *m_currentFontStyle;
    XlsxCellFormat *m_currentCellFormat;

private:
    void init();

    class Private;
    Private* const d;
};

#endif //XLSXXMLSTYLESREADER_H
