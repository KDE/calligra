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

KoGenStyle m_currentDrawStyle; //!< set by read_drawing(), used by read_pic()

int m_svgX, m_svgY; //!< set by read_off()
int m_svgWidth, m_svgHeight; //! set by read_ext()
#endif
