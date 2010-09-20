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

// This is not a normail heder, *don't* add include guards to it.
// This will cause the compiler to get wrong offsets and to corrupt the stack.

// included by DocxXmlDocumentReader and PptxXmlSlideReader

protected:
void initInternal(); //!< should be called from ctor
void doneInternal(); //!< sould be called from dtor

KoFilter::ConversionStatus read_t();

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
