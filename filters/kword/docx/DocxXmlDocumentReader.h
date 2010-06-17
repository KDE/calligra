/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef DOCXXMLDOCREADER_H
#define DOCXXMLDOCREADER_H

#include <QBuffer>
#include <QString>

#include <MsooXmlCommonReader.h>
#include <MsooXmlThemesReader.h>
#include "DocxXmlNotesReader.h"

#include <KoXmlWriter.h>
#include <KoGenStyle.h>
#include <styles/KoCharacterStyle.h>

//#define NO_DRAWINGML_PICTURE // disables pic:pic, etc. in MsooXmlCommonReader

class DocxImport;
class DocxXmlDocumentReaderContext;
namespace MSOOXML
{
class MsooXmlRelationships;
}

//! A class reading MSOOXML DOCX markup - document.xml part.
class DocxXmlDocumentReader : public MSOOXML::MsooXmlCommonReader
{
public:
    explicit DocxXmlDocumentReader(KoOdfWriters *writers);

    virtual ~DocxXmlDocumentReader();

    //! Reads/parses the file of format document.xml.
    //! The output goes mainly to KoXmlWriter* KoOdfWriters::body
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

#include <MsooXmlVmlReaderMethods.h> // separated as it is not a part of OOXML

protected:
    KoFilter::ConversionStatus read_body();
    KoFilter::ConversionStatus read_p();
    KoFilter::ConversionStatus read_r();
    enum rPrCaller {
        rPr_r,
        rPr_pPr,
        rPr_rPrDefault, // styles
        rPr_style // styles
    };
    KoFilter::ConversionStatus read_rPr(rPrCaller caller);
    KoFilter::ConversionStatus read_pPr();
    KoFilter::ConversionStatus read_numPr();
    KoFilter::ConversionStatus read_numId();
    KoFilter::ConversionStatus read_ilvl();
    KoFilter::ConversionStatus read_sectPr();
    KoFilter::ConversionStatus read_footerReference();
    KoFilter::ConversionStatus read_headerReference();
    KoFilter::ConversionStatus read_cols();
    KoFilter::ConversionStatus read_pgSz();
    KoFilter::ConversionStatus read_textDirection();
    KoFilter::ConversionStatus read_pgMar();
    KoFilter::ConversionStatus read_pgBorders();
    KoFilter::ConversionStatus read_object();
    KoFilter::ConversionStatus read_ind();
    KoFilter::ConversionStatus read_outline();
    KoFilter::ConversionStatus read_framePr();
    KoFilter::ConversionStatus read_OLEObject();
    KoFilter::ConversionStatus read_webHidden();
    KoFilter::ConversionStatus read_bookmarkStart();
    KoFilter::ConversionStatus read_bookmarkEnd();
    //KoFilter::ConversionStatus read_commentRangeEnd();
    KoFilter::ConversionStatus read_commentRangeStart();
    KoFilter::ConversionStatus read_endnoteReference();
    KoFilter::ConversionStatus read_footnoteReference();
    KoFilter::ConversionStatus read_footnotePr();
    KoFilter::ConversionStatus read_endnotePr();
    KoFilter::ConversionStatus read_numFmt();
    KoFilter::ConversionStatus read_hyperlink();
    KoFilter::ConversionStatus read_drawing();
    KoFilter::ConversionStatus read_ptab();
    KoFilter::ConversionStatus read_tabs();
    KoFilter::ConversionStatus read_tab();
    KoFilter::ConversionStatus read_i();
    KoFilter::ConversionStatus read_b();
    KoFilter::ConversionStatus read_u();
    KoFilter::ConversionStatus read_sz();
    KoFilter::ConversionStatus read_jc();
    KoFilter::ConversionStatus read_spacing();
    KoFilter::ConversionStatus read_trPr();
    KoFilter::ConversionStatus read_trHeight();
    enum shdCaller {
        shd_rPr,
        shd_pPr,
        shd_tcPr
    };
    KoFilter::ConversionStatus read_shd(shdCaller caller);
    KoFilter::ConversionStatus read_rFonts();
    KoFilter::ConversionStatus read_pStyle();
    KoFilter::ConversionStatus read_rStyle();
    KoFilter::ConversionStatus read_fldSimple();
    KoFilter::ConversionStatus read_lastRenderedPageBreak();
    KoFilter::ConversionStatus read_instrText();
    KoFilter::ConversionStatus read_fldChar();
    KoFilter::ConversionStatus read_strike();
    KoFilter::ConversionStatus read_dstrike();
    KoFilter::ConversionStatus read_caps();
    KoFilter::ConversionStatus read_smallCaps();
    KoFilter::ConversionStatus read_w();
    KoFilter::ConversionStatus read_color();
    KoFilter::ConversionStatus read_highlight();
    KoFilter::ConversionStatus read_vertAlign();
    KoFilter::ConversionStatus read_lang();
    KoFilter::ConversionStatus read_background();
    KoFilter::ConversionStatus read_pBdr();
    KoFilter::ConversionStatus read_tbl();
    KoFilter::ConversionStatus read_tblPr();
    KoFilter::ConversionStatus read_tblGrid();
    KoFilter::ConversionStatus read_gridCol();
    KoFilter::ConversionStatus read_tr();
    KoFilter::ConversionStatus read_tc();
    KoFilter::ConversionStatus read_tcPr();

    enum posOffsetCaller {
        posOffset_positionH,
        posOffset_positionV
    };

    enum alignCaller {
        align_positionH,
        align_positionV
    };
    KoFilter::ConversionStatus read_align(alignCaller caller);

    KoFilter::ConversionStatus read_inline();
    KoFilter::ConversionStatus read_extent();
    KoFilter::ConversionStatus read_docPr();
    KoFilter::ConversionStatus read_anchor();
    KoFilter::ConversionStatus read_positionH();
    KoFilter::ConversionStatus read_positionV();
    KoFilter::ConversionStatus read_posOffset(posOffsetCaller caller);
    KoFilter::ConversionStatus read_wrapSquare();
    KoFilter::ConversionStatus read_wrapTight();
    KoFilter::ConversionStatus read_wrapThrough();

    KoGenStyle m_currentPageStyle;
    KoGenStyle m_masterPageStyle;

    DocxXmlDocumentReaderContext* m_context;

    KoOdfWriters *m_writers; // Needed to create new relationship for header/footer

private:
    void init();

    //! Writes text:p/draw:rect elements.
    //! Uses m_currentObjectWidthCm and m_currentObjectHeightCm for size (defaults to 2cm/2cm)
    void writeRect();

    enum BorderSide {
        TopBorder, BottomBorder, LeftBorder, RightBorder
    };
    //! Used for setting up properties for pages and paragraphs.
    //! It is reversed map, so detecting duplicates is easy in applyBorders().
    QMap<QString, BorderSide> m_borderStyles;
    QColor m_backgroundColor; //Documet background color

    //! Used for setting up properties for pages and paragraphs.
    //! It is reversed map, so detecting duplicates is easy in applyBorders().
    QMap<QString, BorderSide> m_borderPaddings;

    //! Reads CT_Border complex type (p.392), used by children of pgBorders and children of pBdr
    KoFilter::ConversionStatus readBorderElement(BorderSide borderSide, const char *borderSideName);

    //! Creates border style for readBorderElement().
    //! Result is added to m_borderStyles and m_borderPaddings
    void createBorderStyle(const QString& size, const QString& color,
                           const QString& lineStyle, BorderSide borderSide);

    //! Used by read_strike() and read_dstrike()
    void readStrikeElement(KoCharacterStyle::LineType type);

    void setParentParagraphStyleName(const QXmlStreamAttributes& attrs);

    //! Applies border styles and paddings obtained in readBorderElement()
    //! to style @a style (paragraph or page...)
    void applyBorders(KoGenStyle *style);

    enum ComplexFieldCharType {
       NoComplexFieldCharType, HyperlinkComplexFieldCharType, ReferenceComplexFieldCharType,
       ReferenceNextComplexFieldCharType
    };
    //! Type of complex field characters we have
    ComplexFieldCharType m_complexCharType;

    //! Value of the complex field char if applicable
    QString m_complexCharValue;

    enum ComplexCharStatus {
        NoneAllowed, InstrAllowed, InstrExecute
    };
    //! State of fldChar
    ComplexCharStatus m_complexCharStatus;

    enum DropCapStatus {
        NoDropCap, DropCapRead, DropCapDone
    };
    //! State of dropCap
    DropCapStatus m_dropCapStatus;

    //! Buffer where first letters of drop cap are read
    QBuffer m_dropCapBuffer;
    KoXmlWriter* m_dropCapWriter;
    QString m_dropCapLines;
    qreal   m_dropCapDistance;

    QMap<QString, QString> m_bookmarks; //!< Bookmarks

    uint m_currentTableNumber; //!< table counter, from 0
    uint m_currentTableRowNumber; //!< row counter, from 0, initialized in read_tbl()
    uint m_currentTableColumnNumber; //!< column counter, from 0, initialized in read_tr()
    KoGenStyle m_currentTableStyle;
    KoGenStyle m_currentTableRowStyle;
    KoGenStyle m_currentTableCellStyle;
    QString m_currentTableName;
    qreal m_currentTableWidth; //!< in cm
    bool m_wasCaption; // bookkeeping to ensure next para is suppressed if a caption is encountered

    bool m_closeHyperlink; // should read_r close hyperlink
    bool m_listFound; // was there numPr element in ppr
    QString m_currentListStyleName;

    /*! true if w:object/v:shape or w:object/o:OLEObject has been handled, .
     When w:object/o:OLEObject is visited and m_objectRectInitialized is true, handling
     w:object/o:OLEObject is (except for copying the OLE binary) skipped because
     w:object/v:shape is of higher priority.
     This flag is reset to false each time read_object() is called. */
    bool m_objectRectInitialized;

    //!< Width of the object. Set in read_OLEObject() or read_shape(). Used in writeRect().
    //! If both w:object/v:shape and w:object/o:OLEObject exist, information from v:shape is used.
    QString m_currentObjectWidthCm;

    QString m_currentObjectHeightCm; //!< See m_currentObjectWidthCm for description
    QString m_currentObjectXCm; //!< See m_currentObjectWidthCm for description
    QString m_currentObjectYCm; //!< See m_currentObjectWidthCm for description

    QMap<QString, QString> m_headers;
    QMap<QString, QString> m_footers;

#include <MsooXmlCommonReaderMethods.h>
#include <MsooXmlCommonReaderDrawingMLMethods.h>

    class Private;
    Private* const d;
};

//! Context for DocxXmlDocumentReader
class DocxXmlDocumentReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    //! Creates the context object.
    DocxXmlDocumentReaderContext(
        DocxImport& _import,
        const QString& _path, const QString& _file,
        MSOOXML::MsooXmlRelationships& _relationships,
        const QMap<QString, MSOOXML::DrawingMLTheme*>& _themes
    );
    DocxImport* import;
    const QString path;
    const QString file;

    const QMap<QString, MSOOXML::DrawingMLTheme*>* themes;

    // Contains footnotes when read, the styles of footnotes are already put to correct files.
    QMap<QString, QString> m_footnotes;

    QMap<QString, QString> m_comments;

    QMap<QString, QString> m_endnotes;

private:
};

#endif //DOCXXMLDOCREADER_H
