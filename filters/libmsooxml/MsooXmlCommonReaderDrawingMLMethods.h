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

#ifndef MSOOXMLCOMMONREADERDRAWINGMLMETHODS_H
#define MSOOXMLCOMMONREADERDRAWINGMLMETHODS_H

// included by DocxXmlDocumentReader

protected:
KoFilter::ConversionStatus read_pic();
KoFilter::ConversionStatus read_nvPicPr();
KoFilter::ConversionStatus read_cNvPr();
KoFilter::ConversionStatus read_cNvPicPr();
KoFilter::ConversionStatus read_nvSpPr();
KoFilter::ConversionStatus read_cNvSpPr();
KoFilter::ConversionStatus read_sp();
KoFilter::ConversionStatus read_spPr();
KoFilter::ConversionStatus read_xfrm();
KoFilter::ConversionStatus read_off();
KoFilter::ConversionStatus read_ext();
KoFilter::ConversionStatus read_blip();
KoFilter::ConversionStatus read_stretch();
KoFilter::ConversionStatus read_fillRect();
KoFilter::ConversionStatus read_graphic();
KoFilter::ConversionStatus read_graphicData();
KoFilter::ConversionStatus read_blipFill();
KoFilter::ConversionStatus read_anchor();
KoFilter::ConversionStatus read_positionH();
KoFilter::ConversionStatus read_positionV();
KoFilter::ConversionStatus read_posOffset();
KoFilter::ConversionStatus read_align();
KoFilter::ConversionStatus read_inline();
KoFilter::ConversionStatus read_docPr();
KoFilter::ConversionStatus read_wrapSquare();
KoFilter::ConversionStatus read_wrapTight();
KoFilter::ConversionStatus read_wrapThrough();

KoFilter::ConversionStatus read_DrawingML_p();
KoFilter::ConversionStatus read_DrawingML_r();

//! Copies file to destination directory. @a destinationName is set.
KoFilter::ConversionStatus copyFile(
    const QString& sourceName, const QString& destinationDir, QString& destinationName);

KoGenStyle m_currentDrawStyle; //!< set by read_drawing(), used by read_pic()
bool m_drawing_anchor; //! set by read_drawing() to indicate if we have encountered drawing/anchor, used by read_pic()
bool m_drawing_inline; //! set by read_drawing() to indicate if we have encountered drawing/inline, used by read_pic()

int m_svgX; //!< set by read_off()
int m_svgY; //!< set by read_off()
int m_svgWidth; //! set by read_ext()
int m_svgHeight; //! set by read_ext()
int m_flipH; //! set by read_xfrm()
int m_flipV; //! set by read_xfrm()
int m_rot; //! set by read_xfrm()

//! true if no fill should be applied for element; used e.g. by pic:spPr/a:noFill elem.
bool m_noFill;

QString m_xlinkHref; //!< set by read_blip()
QString m_cNvPrId; //!< set by read_cNvPr()
QString m_cNvPrName; //!< set by read_cNvPr()
QString m_cNvPrDescr; //!< set by read_cNvPr()

QSet<QString> m_copiedFiles; //!< collects source names to avoid multiple copying of media files

#endif
