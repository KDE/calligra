/*
 * This file is part of Office 2007 Filters for Calligra
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

#include <MsooXmlTheme.h>
#include <QFlags>
#include <QColor>

#include "XlsxXmlCommonReader.h"

class KoGenStyle;
class XlsxImport;
class XlsxStyles;
class XlsxCellFormat;
class XlsxXmlStylesReader;

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

    //! Indicates whether the displayed text in a cell should be shrunk to fit the cell width.
    bool shrinkToFit;

    //! Text rotation in cells, expressed in degrees. (Undocumented? special case: 255 means
    //! vertical top-to-bottom-text without rotated characters
    uint textRotation;

    void setHorizontalAlignment(const QString& alignment);
    void setVerticalAlignment(const QString& alignment);

    //! Sets up @a cellStyle to match this cell style.
    //! @todo implement more styling
    bool setupCellStyle(const XlsxStyles *styles, KoGenStyle* cellStyle) const;

    //! Used by setupCellStyle() & read_dxf
    void setupCellStyleAlignment(KoGenStyle* cellStyle) const;
};

class XlsxStyles
{
public:
    XlsxStyles();
    ~XlsxStyles();

    //! @return font style for id @a id (counted from 0)
    KoGenStyle* fontStyle(int id) const {
        if (id < 0 || id >= fontStyles.size())
            return 0;
        return fontStyles[id];
    }

    //! @return fill style for id @a id (counted from 0)
    KoGenStyle* fillStyle(int id) const {
        if (id < 0 || id >= fillStyles.size())
            return 0;
        return fillStyles[id];
    }

    //! @return border style for id @a id (counted from 0)
    KoGenStyle* borderStyle(int id) const {
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

//     //! @return the KoGenStyle the number-formatting style has
//     KoGenStyle numberFormatStyle( int id ) const {
//         return numberFormatStyles[ id ];
//     }

    //! @return the KoGenStyle styleName the number-formatting style has
    QString numberFormatStyleName( int id ) const {
        return numberFormatStyleNames[ id ];
    }

    //! @return conditional style name for the given index
    QString conditionalStyle( int index ) const
    {
        return conditionalStyles[ index ];
    }

protected:
    void setCellFormat(XlsxCellFormat *format, int cellFormatIndex);

    QVector<KoGenStyle*> fontStyles;
    QVector<KoGenStyle*> fillStyles;
    QVector<KoGenStyle*> borderStyles;
    QVector<XlsxCellFormat*> cellFormats;
    QMap< int, QString > numberFormatStrings;
//     QMap< int, KoGenStyle > numberFormatStyles;
    QMap< int, QString > numberFormatStyleNames;
    QMap< int, QString > conditionalStyles;

    friend class XlsxXmlStylesReader;
};

class XlsxXmlStylesReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    XlsxXmlStylesReaderContext(XlsxStyles& _styles, bool _skipFirstPart,
                               XlsxImport* _import,
                               MSOOXML::DrawingMLTheme* _themes);
    XlsxStyles* styles;
    bool skipFirstPart;
    XlsxImport* import;
    MSOOXML::DrawingMLTheme* themes;
    QVector<QString> colorIndices;
};

//! A class reading MSOOXML XLSX markup - styles.xml part.
//! See ECMA-376, 12.3.20: Styles Part, p. 104
class XlsxXmlStylesReader : public XlsxXmlCommonReader
{
public:
    explicit XlsxXmlStylesReader(KoOdfWriters *writers);

    ~XlsxXmlStylesReader() override;

    //! Reads/parses the file of format document.xml.
    //! The output goes mainly to KoXmlWriter* KoOdfWriters::body
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0) override;

    enum DiagonalDirection {
        DiagonalUp = 1,
        DiagonalDown = 2
    };
    Q_DECLARE_FLAGS(DiagonalDirections, DiagonalDirection)
    DiagonalDirections diagonalDirections;

protected:
    KoFilter::ConversionStatus readInternal();
    KoFilter::ConversionStatus read_styleSheet();
    KoFilter::ConversionStatus read_numFmts();
    KoFilter::ConversionStatus read_numFmt();
    KoFilter::ConversionStatus read_fonts();
    KoFilter::ConversionStatus read_font();
    KoFilter::ConversionStatus read_name();
    KoFilter::ConversionStatus read_cellXfs();
    KoFilter::ConversionStatus read_dxfs();
    KoFilter::ConversionStatus read_dxf();
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
    KoFilter::ConversionStatus read_colors();
    KoFilter::ConversionStatus read_indexedColors();
    KoFilter::ConversionStatus read_rgbColor();

    uint m_cellFormatIndex;

    XlsxXmlStylesReaderContext* m_context;

    QColor m_currentFgColor;
    QColor m_currentBgColor;

    KoGenStyle *m_currentFontStyle;
    KoGenStyle *m_currentFillStyle;
    XlsxCellFormat *m_currentCellFormat;
    KoGenStyle *m_currentBorderStyle;

    int m_colorIndex;

    KoFilter::ConversionStatus readAttributes(const QXmlStreamAttributes& attrs, QString& borderStyle);

private:
    void init();

    class Private;
    Private* const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(XlsxXmlStylesReader::DiagonalDirections)

#endif //XLSXXMLSTYLESREADER_H
