/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2009-2010 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

// This is not a normal header, *don't* add include guards to it.
// This will cause the compiler to get wrong offsets and to corrupt the stack.

// included by DocxXmlDocumentReader and PptxXmlSlideReader

protected:
void initInternal(); //!< should be called from ctor
void doneInternal(); //!< should be called from dtor

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

bool m_choiceAccepted; // Whether choice provided something we implement

bool m_read_t_args;

// 2010 specific, meant to offer choice between paths based on what is supported
KoFilter::ConversionStatus read_AlternateContent();
KoFilter::ConversionStatus read_Choice();
KoFilter::ConversionStatus read_Fallback();
