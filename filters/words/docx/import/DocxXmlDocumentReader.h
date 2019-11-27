/*
 * This file is part of Office 2007 Filters for Calligra
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

#ifndef DOCXXMLDOCUMENTREADER_H
#define DOCXXMLDOCUMENTREADER_H

#include <QBuffer>
#include <QString>

#include <MsooXmlCommonReader.h>
#include <MsooXmlThemesReader.h>
#include <MsooXmlDrawingTableStyle.h>

#include <KoXmlWriter.h>
#include <KoGenStyle.h>
#include <styles/KoCharacterStyle.h>
#include <KoBorder.h>
#include <KoTblStyle.h>
#include <KoTable.h>

//#define NO_DRAWINGML_PICTURE // disables pic:pic, etc. in MsooXmlCommonReader

class DocxImport;
class DocxXmlDocumentReaderContext;
namespace MSOOXML
{
class MsooXmlRelationships;
struct TableStyleProperties;
class LocalTableStyles;
}

//! A class reading MSOOXML DOCX markup - document.xml part.
class DocxXmlDocumentReader : public MSOOXML::MsooXmlCommonReader
{
public:
    explicit DocxXmlDocumentReader(KoOdfWriters *writers);

    ~DocxXmlDocumentReader() override;

    //! Reads/parses the file of format document.xml.
    //! The output goes mainly to KoXmlWriter* KoOdfWriters::body
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0) override;

// Note: Do not move this, it just defines parts of this class
// It is separated out, as it is not part of the OOXML spec
#include <MsooXmlVmlReaderMethods.h>

protected:
    KoFilter::ConversionStatus read_body();
    KoFilter::ConversionStatus read_p();
    KoFilter::ConversionStatus read_r();
    KoFilter::ConversionStatus read_smartTag();
    KoFilter::ConversionStatus read_rPr();
    KoFilter::ConversionStatus read_pPr();
    KoFilter::ConversionStatus read_vanish();
    KoFilter::ConversionStatus read_numPr();
    KoFilter::ConversionStatus read_numId();
    KoFilter::ConversionStatus read_ilvl();
    KoFilter::ConversionStatus read_sectPr();
    bool m_footerActive;
    bool m_headerActive;
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
    KoFilter::ConversionStatus read_outlineLvl();
    KoFilter::ConversionStatus read_framePr();
    KoFilter::ConversionStatus read_OLEObject();
    KoFilter::ConversionStatus read_control();
    KoFilter::ConversionStatus read_webHidden();
    KoFilter::ConversionStatus read_bookmarkStart();
    KoFilter::ConversionStatus read_bookmarkEnd();
    //KoFilter::ConversionStatus read_commentRangeEnd();
    KoFilter::ConversionStatus read_commentRangeStart();
    KoFilter::ConversionStatus read_endnoteReference();
    KoFilter::ConversionStatus read_footnoteReference();
    KoFilter::ConversionStatus read_footnotePr();
    KoFilter::ConversionStatus read_endnotePr();
    KoFilter::ConversionStatus read_lnNumType();
    KoFilter::ConversionStatus read_numFmt();
    KoFilter::ConversionStatus read_numRestart();
    KoFilter::ConversionStatus read_numStart();
    KoFilter::ConversionStatus read_pos();
    KoFilter::ConversionStatus read_suppressLineNumbers();
    KoFilter::ConversionStatus read_hyperlink();
    KoFilter::ConversionStatus read_del();
    KoFilter::ConversionStatus read_ins();
    KoFilter::ConversionStatus read_delText();
    KoFilter::ConversionStatus read_drawing();
    KoFilter::ConversionStatus read_ptab();
    KoFilter::ConversionStatus read_tabs();
    KoFilter::ConversionStatus read_tab();
    KoFilter::ConversionStatus read_i();
    KoFilter::ConversionStatus read_b();
    KoFilter::ConversionStatus read_u();
    KoFilter::ConversionStatus read_sz();
    enum jcCaller {
       jc_tblPr,
       jc_pPr
    };
    KoFilter::ConversionStatus read_jc(jcCaller caller);
    KoFilter::ConversionStatus read_spacing();
    KoFilter::ConversionStatus read_trPr();
    KoFilter::ConversionStatus read_cnfStyle();
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
    KoFilter::ConversionStatus read_tblpPr();
    KoFilter::ConversionStatus read_tblStyle();
    KoFilter::ConversionStatus read_tblBorders();
    KoFilter::ConversionStatus read_tblCellMar();
    KoFilter::ConversionStatus read_fldSimple();
    KoFilter::ConversionStatus read_br();
    KoFilter::ConversionStatus read_lastRenderedPageBreak();
    KoFilter::ConversionStatus read_instrText();
    KoFilter::ConversionStatus read_fldChar();
    KoFilter::ConversionStatus read_strike();
    KoFilter::ConversionStatus read_dstrike();
    KoFilter::ConversionStatus read_caps();
    KoFilter::ConversionStatus read_smallCaps();
    KoFilter::ConversionStatus read_w();
    KoFilter::ConversionStatus read_txbxContent();
    KoFilter::ConversionStatus read_color();
    KoFilter::ConversionStatus read_highlight();
    KoFilter::ConversionStatus read_vertAlign();
    KoFilter::ConversionStatus read_lang();
    KoFilter::ConversionStatus read_background();
    KoFilter::ConversionStatus read_pBdr();
    KoFilter::ConversionStatus read_bdr();
    KoFilter::ConversionStatus read_tbl();
    KoFilter::ConversionStatus read_tblPr();
    KoFilter::ConversionStatus read_tblPrEx();
    KoFilter::ConversionStatus read_tblGrid();
    KoFilter::ConversionStatus read_gridCol();
    KoFilter::ConversionStatus read_tr();
    KoFilter::ConversionStatus read_tc();
    KoFilter::ConversionStatus read_tcPr();
    KoFilter::ConversionStatus read_vAlign();
    KoFilter::ConversionStatus read_textDirectionTc();
    KoFilter::ConversionStatus read_vMerge();
    KoFilter::ConversionStatus read_tcBorders();
    KoFilter::ConversionStatus read_tl2br();
    KoFilter::ConversionStatus read_tr2bl();
    KoFilter::ConversionStatus read_tcMar();
    KoFilter::ConversionStatus read_gridSpan();
    int m_gridSpan;

    KoFilter::ConversionStatus read_oMath();
    KoFilter::ConversionStatus read_oMathPara();
    KoFilter::ConversionStatus read_oMathParaPr();
    KoFilter::ConversionStatus read_jc_m();
    KoFilter::ConversionStatus read_r_m();
    KoFilter::ConversionStatus read_t_m();

    enum posOffsetCaller {
        posOffset_positionH,
        posOffset_positionV
    };

    enum alignCaller {
        align_positionH,
        align_positionV
    };
    KoFilter::ConversionStatus read_align(alignCaller caller);
    KoFilter::ConversionStatus read_pict();
    KoFilter::ConversionStatus read_sdt();
    KoFilter::ConversionStatus read_sdtContent();
    KoFilter::ConversionStatus read_inline();
    KoFilter::ConversionStatus read_extent();
    KoFilter::ConversionStatus read_docPr();
    KoFilter::ConversionStatus read_anchor();
    KoFilter::ConversionStatus read_positionH();
    KoFilter::ConversionStatus read_positionV();
    KoFilter::ConversionStatus read_posOffset(posOffsetCaller caller);
    KoFilter::ConversionStatus read_wrapPolygon();
    KoFilter::ConversionStatus read_wrapSquare();
    KoFilter::ConversionStatus read_wrapTight();
    KoFilter::ConversionStatus read_wrapThrough();

    //! Read wrapping related attributes.
    void readWrapAttrs();

    bool m_createSectionStyle;
    QString m_currentSectionStyleName;
    QByteArray m_currentSectionStyleFamily;
    bool m_createSectionToNext;
    KoGenStyle m_currentPageStyle;
    KoGenStyle m_masterPageStyle;

    DocxXmlDocumentReaderContext* m_context;

    KoOdfWriters *m_writers; // Needed to create new relationship for header/footer

    enum PageMargin {
        MarginTop, MarginBottom, MarginLeft, MarginRight
    };

    //! Used for setting up properties for page margins.
    QMap<PageMargin, qreal> m_pageMargins;
    QString m_pageBorderOffsetFrom;

    enum BorderSide {
        TopBorder, BottomBorder, LeftBorder, RightBorder, InsideH, InsideV
    };

    //! Whether the picture when run-through wrap is active should be in fore or background
    bool m_behindDoc;

    //! Used for setting up properties for pages
    QMap<BorderSide, QString> m_pageBorderStyles;

    //! Used for setting up properties for paragraphs.
    QMap<BorderSide, QString> m_borderStyles;

    //! Used for setting up properties r element
    QMap<BorderSide, QString> m_textBorderStyles;

    //! Used for setting up properties for pages
    QMap<BorderSide, qreal> m_pageBorderPaddings;

    //! Used for setting up properties for paragraphs
    QMap<BorderSide, qreal> m_borderPaddings;

    // ! Used for setting up properties for border padding
    QMap<BorderSide, qreal> m_textBorderPaddings;

    KoTable::Ptr m_table;
    QString m_currentTableStyleName;
    KoTblStyle::Ptr m_tableMainStyle;

    MSOOXML::LocalTableStyles* m_currentLocalTableStyles;

    MSOOXML::TableStyleProperties* m_currentTableStyleProperties;
    MSOOXML::TableStyleProperties* m_currentDefaultCellStyle;

    //! Name of the KoGenStyle style of type GraphicAutoStyle prepared for the
    //! parent <draw:frame> element containing the <table:table> element
    //! of a floating table.
    QString m_currentDrawStyleName;

    //! The complete XML snippet representing a floating table, which MUST be
    //! inserted into the following paragraph.
    QString m_floatingTable;

    //! The complete XML snippet representing a bookmark-start or
    //! bookmark-end, which MUST be inserted into the following paragraph.
    QString m_bookmarkSnippet;

    QList<MSOOXML::Utils::ParagraphBulletProperties> m_currentBulletList;

private:
    void init();

    //! Returns true if the field returned something that requires a closing element
    bool handleSpecialField();

    QString m_specialCharacters;

    QColor m_backgroundColor; //Document background color

    //! Reads CT_Border complex type (p.392), used by children of pgBorders and children of pBdr
    KoFilter::ConversionStatus readBorderElement(BorderSide borderSide, const char *borderSideName,
                                         QMap<BorderSide, QString> &sourceBorder, QMap<BorderSide, qreal> &sourcePadding);

    ///reads the border in a table style
    KoBorder::BorderData getBorderData();

    //! Creates border style for readBorderElement().
    //! Result is added to m_borderStyles and m_borderPaddings
    void createBorderStyle(const QString& size, const QString& color,
                           const QString& lineStyle, BorderSide borderSide, QMap<BorderSide, QString> &sourceBorder);

    //! Used by read_strike() and read_dstrike()
    void readStrikeElement(KoCharacterStyle::LineType type);

    //! Applies border styles and paddings obtained in readBorderElement()
    //! to style @a style (paragraph or page...)
    void applyBorders(KoGenStyle *style, QMap<BorderSide, QString> sourceBorder, QMap<BorderSide, qreal> sourcePadding);

    //! Applies border styles and paddings for page
    void applyPageBorders(KoGenStyle &style, QMap<PageMargin, qreal> &pageMargins, QMap<BorderSide,QString> &pageBorder,
                         QMap<BorderSide, qreal> &pagePadding, QString & offsetFrom);
    void defineTableStyles();

    enum ComplexFieldCharType {
       NoComplexFieldCharType, HyperlinkComplexFieldCharType, ReferenceComplexFieldCharType,
       ReferenceNextComplexFieldCharType, InternalHyperlinkComplexFieldCharType,
       MacroButtonFieldCharType
    };
    //! Type of complex field characters we have
    ComplexFieldCharType m_complexCharType;

    //! Value of the complex field char if applicable
    QString m_complexCharValue;

    enum ComplexCharStatus {
        NoneAllowed, InstrAllowed, InstrExecute, ExecuteInstrNow
    };
    //! State of fldChar
    ComplexCharStatus m_complexCharStatus;

    int m_z_index;

    enum DropCapStatus {
        NoDropCap, DropCapRead, DropCapDone
    };
    //! State of dropCap
    DropCapStatus m_dropCapStatus;

    //! Buffer where first letters of drop cap are read
    QBuffer* m_dropCapBuffer;
    KoXmlWriter* m_dropCapWriter;
    QString m_dropCapLines;
    qreal   m_dropCapDistance;

    QMap<QString, QString> m_bookmarks; //!< Bookmarks

    //!< Width of the object
    QString m_currentObjectWidthCm;
    QString m_currentObjectHeightCm;

    uint m_currentTableNumber; //!< table counter, from 0
    uint m_currentTableRowNumber; //!< row counter, from 0, initialized in read_tbl()
    uint m_currentTableColumnNumber; //!< column counter, from 0, initialized in read_tr()
    KoGenStyle m_currentTableRowStyle;
    QString m_currentTableName;
    qreal m_currentTableWidth; //!< in cm
    MSOOXML::DrawingTableStyleConverterProperties::Roles m_activeRoles;

    bool m_wasCaption; // bookkeeping to ensure next para is suppressed if a caption is encountered
    bool m_closeHyperlink; // should read_r close hyperlink
    bool m_listFound; // was there numPr element in ppr
    bool m_insideParagraph;

    QString m_currentNumId;

    //! Map of list styles encountered so far, reuse already created list style.
    QMap<QString, QString> m_usedListStyles;

    //TODO: Merge with m_continueListNumbering defined in MsooXmlCommenReaderDrawingMLMethods.h
    QMap<QString, QPair<int, bool> > m_continueListNum;

    //! Map of numId.level keys and list-item num./last xml:id pairs.
    QMap<QString, QPair<int, QString> > m_numIdXmlId;

    QMap<QString, QString> m_headers;
    QMap<QString, QString> m_footers;

    //processing the ins/del element (Inserted/Deleted Run Content)
    enum ChangeTrackingState { InsertedRunContent, DeletedRunContent };
    QStack<ChangeTrackingState> m_changeTrackingState;

    // ************************************************
    //  State
    // ************************************************
    struct DocumentReaderState {
        explicit DocumentReaderState(const QMap<QString, QString> &usedListStyles,
                                     const QMap <QString, QPair<int, bool> > &continueListNum,
                                     const QMap <QString, QPair<int, QString> > &numIdXmlId)
        : usedListStyles(usedListStyles),
          continueListNum(continueListNum),
          numIdXmlId(numIdXmlId) {}

        DocumentReaderState() {}

        QMap<QString, QString> usedListStyles;
        QMap<QString, QPair<int, bool> > continueListNum;
        QMap<QString, QPair<int, QString> > numIdXmlId;
    };

    void saveState();
    void restoreState();
    QStack<DocumentReaderState> m_statesBkp;

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
        MSOOXML::DrawingMLTheme* _themes
    );
    DocxImport* import;
    const QString path;
    const QString file;

    MSOOXML::DrawingMLTheme* themes;

    // Contains footnotes when read
    QMap<QString, QString> m_footnotes;

    QMap<QString, QString> m_endnotes;
    QMap<QString, QString> m_comments;
    QMap<QString, MSOOXML::DrawingTableStyle*> m_tableStyles;
    QMap<QString, QList<MSOOXML::Utils::ParagraphBulletProperties> > m_bulletStyles;

    // The map contains names of default styles applied to objects that do not
    // explicitly declare a style.  The object type (family) is the key.
    QMap<QString, QString> m_namedDefaultStyles;

    // The map contains abstractNumId of the abstract numbering definition that
    // is inherited by a numbering definition instance identified by numId (key).
    QMap<QString, QString> m_abstractNumIDs;

    QString m_defaultFontSizePt;

private:
};

#endif //DOCXXMLDOCUMENTREADER_H
