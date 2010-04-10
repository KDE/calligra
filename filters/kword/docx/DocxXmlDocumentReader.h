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

#include <MsooXmlCommonReader.h>
#include <MsooXmlThemesReader.h>
#include "DocxXmlCommentsReader.h"
#include "DocxXmlNotesReader.h"

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
    KoFilter::ConversionStatus read_sectPr();
    KoFilter::ConversionStatus read_footerReference();
    KoFilter::ConversionStatus read_headerReference();
    KoFilter::ConversionStatus read_pgSz();
    KoFilter::ConversionStatus read_pgMar();
    KoFilter::ConversionStatus read_pgBorders();
    KoFilter::ConversionStatus read_object();
    KoFilter::ConversionStatus read_ind();
    KoFilter::ConversionStatus read_OLEObject();
    //KoFilter::ConversionStatus read_commentRangeEnd();
    KoFilter::ConversionStatus read_commentRangeStart();
    KoFilter::ConversionStatus read_endnoteReference();
    KoFilter::ConversionStatus read_footnoteReference();
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
    enum shdCaller {
        shd_rPr,
        shd_pPr
    };
    KoFilter::ConversionStatus read_shd(shdCaller caller);
    KoFilter::ConversionStatus read_rFonts();
    KoFilter::ConversionStatus read_pStyle();
    KoFilter::ConversionStatus read_rStyle();
    KoFilter::ConversionStatus read_lastRenderedPageBreak();
    KoFilter::ConversionStatus read_instrText();
    KoFilter::ConversionStatus read_fldChar();
    KoFilter::ConversionStatus read_strike();
    KoFilter::ConversionStatus read_dstrike();
    KoFilter::ConversionStatus read_caps();
    KoFilter::ConversionStatus read_smallCaps();
    KoFilter::ConversionStatus read_color();
    KoFilter::ConversionStatus read_highlight();
    KoFilter::ConversionStatus read_vertAlign();
    KoFilter::ConversionStatus read_lang();
    KoFilter::ConversionStatus read_background();
    KoFilter::ConversionStatus read_pBdr();

    KoGenStyle m_currentPageStyle;
    KoGenStyle m_masterPageStyle;

    DocxXmlDocumentReaderContext* m_context;

private:
    void init();

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
       NoComplexFieldCharType, HyperlinkComplexFieldCharType
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

    //! @return comment for identifier @a id. Use Comment::isNull() to check if the item was found.
    DocxComment comment(KoOdfWriters *writers, int id);

    //! @return endnote for identifier @a id. Use Comment::isNull() to check if the item was found.
    DocxNote endnote(KoOdfWriters *writers, int id);

    //! @return footnote for identifier @a id. Use Comment::isNull() to check if the item was found.
    DocxNote footnote(KoOdfWriters *writers, int id);

    const QMap<QString, MSOOXML::DrawingMLTheme*>* themes;
private:
    KoFilter::ConversionStatus loadComments(KoOdfWriters *writers);
    KoFilter::ConversionStatus loadEndnotes(KoOdfWriters *writers);
    KoFilter::ConversionStatus loadFootnotes(KoOdfWriters *writers);

    QMap<int, DocxComment> m_comments;
    QMap<int, DocxNote> m_endnotes;
    QMap<int, DocxNote> m_footnotes;
    bool m_commentsLoaded;
    bool m_endnotesLoaded;
    bool m_footnotesLoaded;
};

#endif //DOCXXMLDOCREADER_H
