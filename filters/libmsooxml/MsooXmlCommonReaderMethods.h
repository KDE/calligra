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

#ifndef MSOOXMLCOMMONREADERMETHODS_H
#define MSOOXMLCOMMONREADERMETHODS_H

// included by DocxXmlDocumentReader and PptxXmlSlideReader

protected:
void initInternal();

KoFilter::ConversionStatus read_hyperlink();
KoFilter::ConversionStatus read_p();
KoFilter::ConversionStatus read_pPr();
KoFilter::ConversionStatus read_rPr();
KoFilter::ConversionStatus read_lang();
KoFilter::ConversionStatus read_r();
KoFilter::ConversionStatus read_t();
KoFilter::ConversionStatus read_i();
KoFilter::ConversionStatus read_b();
KoFilter::ConversionStatus read_u();
KoFilter::ConversionStatus read_sz();
KoFilter::ConversionStatus read_strike();
KoFilter::ConversionStatus read_dstrike();
KoFilter::ConversionStatus read_color();
KoFilter::ConversionStatus read_highlight();
KoFilter::ConversionStatus read_lstStyle();
KoFilter::ConversionStatus read_drawing();
KoFilter::ConversionStatus read_anchor();
KoFilter::ConversionStatus read_positionH();
KoFilter::ConversionStatus read_positionV();
KoFilter::ConversionStatus read_posOffset();
KoFilter::ConversionStatus read_align();
KoFilter::ConversionStatus read_inline();
KoFilter::ConversionStatus read_graphic();
KoFilter::ConversionStatus read_graphicData();
KoFilter::ConversionStatus read_blipFill();
KoFilter::ConversionStatus read_blip();
KoFilter::ConversionStatus read_stretch();
KoFilter::ConversionStatus read_fillRect();
KoFilter::ConversionStatus read_docPr();
KoFilter::ConversionStatus read_wrapSquare();
KoFilter::ConversionStatus read_wrapTight();
KoFilter::ConversionStatus read_wrapThrough();
KoFilter::ConversionStatus read_shd();
KoFilter::ConversionStatus read_jc();

void setParentParagraphStyleName(const QXmlStreamAttributes& attrs);

//! Used by read_strike() and read_dstrike()
void readStrikeValue(KoCharacterStyle::LineType type);

//! Sets fo:margin-* attribute of style:style/style:graphic-properties element. Used in read_anchor()
void distToODF(const char * odfEl, const QString emuValue);

//! Sets style:wrap attribute of style:style/style:graphic-properties element. Used in read_anchor()
void saveStyleWrap(const char * style);

//! Used by read_wrap*()
void readWrap();

//! ODF 1.1., 15.14.9 Fill Image Rendering Style
//! Set by read_stretch()
QString m_fillImageRenderingStyle;

QString m_xlinkHref; //!< set by read_blip()
QString m_cNvPrId; //!< set by read_cNvPr()
QString m_cNvPrName; //!< set by read_cNvPr()
QString m_cNvPrDescr; //!< set by read_cNvPr()
QString m_docPrName; //!< set by read_docPr()
QString m_docPrDescr; //!< set by read_docPr()

QSet<QString> m_copiedFiles; //!< collects source names to avoid multiple copying of media files

QString m_relativeFromV; //!< used by read_positionV()
QString m_relativeFromH; //!< used by read_positionH()
QString m_alignV; //!< used by read_align()
QString m_alignH; //!< used by read_align()
int m_posOffsetV; //!< used by read_posOffset()
bool m_hasPosOffsetV; //!< used by read_posOffset()
int m_posOffsetH; //!< used by read_posOffset()
bool m_hasPosOffsetH; //!< used by read_posOffset()

//! @todo set it
bool m_insideHdr; //!< used to indicate that we're parsing inside hdr (header)
//! @todo set it
bool m_insideFtr; //!< used to indicate that we're parsing inside ftr (footer)

#endif
