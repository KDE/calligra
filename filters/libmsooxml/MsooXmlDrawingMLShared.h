/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * Copyright (C) 2012 Matus Uzak (matus.uzak@gmail.com).
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

// This is not a normal header, *don't* add include guards to it.
// This will cause the compiler to get wrong offsets and to corrupt
// the stack.

KoFilter::ConversionStatus read_extLst();

KoFilter::ConversionStatus read_ln();
KoFilter::ConversionStatus read_solidFill();

#ifndef MSOOXML_THEMESREADER_H
KoFilter::ConversionStatus read_fillRef();
KoFilter::ConversionStatus read_fontRef();
#endif

KoFilter::ConversionStatus read_hslClr();
KoFilter::ConversionStatus read_prstClr();
KoFilter::ConversionStatus read_schemeClr();
KoFilter::ConversionStatus read_scrgbClr();
KoFilter::ConversionStatus read_srgbClr();
KoFilter::ConversionStatus read_sysClr();

KoFilter::ConversionStatus read_shade();
KoFilter::ConversionStatus read_tint();
KoFilter::ConversionStatus read_alpha();
KoFilter::ConversionStatus read_satMod();
KoFilter::ConversionStatus read_lumMod();
KoFilter::ConversionStatus read_lumOff();

KoFilter::ConversionStatus read_tailEnd();
KoFilter::ConversionStatus read_headEnd();

KoBorder::BorderData m_currentBorder;

KoFilter::ConversionStatus read_Table_generic(const QString& endElement);
KoFilter::ConversionStatus read_Table_lnL();
KoFilter::ConversionStatus read_Table_lnR();
KoFilter::ConversionStatus read_Table_lnT();
KoFilter::ConversionStatus read_Table_lnB();
KoFilter::ConversionStatus read_Table_ln();

//! set by one of the color readers, read by read_solidFill. Read and
//! set by one of the color transformations.
QColor m_currentColor;

int m_currentAlpha; // current alpha color value

qreal m_currentTint; // value of current tint
qreal m_currentShadeLevel;
qreal m_currentSatMod; //value of current saturation modulation
qreal m_currentLineWidth;

qreal* m_currentDoubleValue;

QString m_referredFontName;
