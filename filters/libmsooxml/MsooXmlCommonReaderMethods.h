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

#ifndef MSOOXMLCOMMONREADERMETHODS_H
#define MSOOXMLCOMMONREADERMETHODS_H

// included by DocxXmlDocumentReader and PptxXmlSlideReader

protected:
void initInternal(); //!< should be called from ctor
void doneInternal(); //!< sould be called from dtor

KoFilter::ConversionStatus read_pPr();
KoFilter::ConversionStatus read_rPr();
KoFilter::ConversionStatus read_lang();
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
KoFilter::ConversionStatus read_shd();
KoFilter::ConversionStatus read_jc();
KoFilter::ConversionStatus read_spacing();
KoFilter::ConversionStatus read_vertAlign();
KoFilter::ConversionStatus read_rFonts();
KoFilter::ConversionStatus read_pStyle();

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
bool m_fillImageRenderingStyleStretch;

QString m_relativeFromV; //!< used by read_positionV()
QString m_relativeFromH; //!< used by read_positionH()
QString m_alignV; //!< used by read_align()
QString m_alignH; //!< used by read_align()
int m_posOffsetV; //!< used by read_posOffset()
bool m_hasPosOffsetV; //!< used by read_posOffset()
int m_posOffsetH; //!< used by read_posOffset()
bool m_hasPosOffsetH; //!< used by read_posOffset()

QString m_docPrName; //!< set by read_docPr()
QString m_docPrDescr; //!< set by read_docPr()

//! @todo set it
bool m_insideHdr; //!< used to indicate that we're parsing inside hdr (header)
//! @todo set it
bool m_insideFtr; //!< used to indicate that we're parsing inside ftr (footer)

#endif
