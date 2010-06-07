/*
 * This file is part of Office 2007 Filters for KOffice
 * Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
 * Copyright (c) 2003 Lukas Tinkl <lukas@kde.org>
 * Copyright (C) 2003 David Faure <faure@kde.org>
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

#ifndef MSOOXML_UNITS_H
#define MSOOXML_UNITS_H

#include <QString>

// conversion from twips (http://en.wikipedia.org/wiki/Twip)
#define TWIP_TO_DM(tw) ((tw)*0.000176389)
#define TWIP_TO_CM(tw) ((tw)*0.001763889)
#define TWIP_TO_MM(tw) ((tw)*0.017638889)
#define TWIP_TO_POINT(tw) ((tw)*0.05)
#define TWIP_TO_INCH(tw) ((tw)*0.000694444)
#define TWIP_TO_PI(tw) ((tw)*0.004166667)
#define TWIP_TO_CC(tw) ((tw)*0.00389404975957)
#define TWIP_TO_PX(tw) ((tw)*0.066798611)

// conversion to twips
#define DM_TO_TWIP(dm) ((dm)*5669.28776738)
#define CM_TO_TWIP(cm) ((cm)*566.929098146)
#define MM_TO_TWIP(mm) ((mm)*56.6929130287)
#define POINT_TO_TWIP(pt) ((pt)*20.0)
#define INCH_TO_TWIP(in) ((in)*1440.0)
#define PI_TO_TWIP(pi) ((pi)*240.0)
#define CC_TO_TWIP(cc) ((cc)*256.80206)
#define PX_TO_TWIP(px) ((px)*14.970371)

// EMU conversion (ECMA-376, 20.1.2.1: EMU Unit of Measurement)
//! Converts emu value (integer or double) to cm
#define EMU_TO_CM(emu) ((emu)/360000.0)

//! Converts emu value (integer or double) to inches
#define EMU_TO_INCH(emu) ((emu)/914400.0)

//! Converts emu value (integer or double) to points
#define EMU_TO_POINT(emu) ((emu)/12700.0)

namespace MSOOXML
{

namespace Utils {

//! Performs EMU conversion and returns string.
inline QString cmString(qreal cm)
{
    QString res;
    return res.sprintf("%3.3fcm", cm);
}

//! Converts EMU Unit of Measurement to cm.
/*! Converts value expressed in EMU (ECMA-376, 20.1.2.1: EMU Unit of Measurement)
    to ODF-compliant "0.000cm" unit.
    "0" and "" is converted to "0cm".
    @return empty string on error. */
//! CASE #P505
MSOOXML_EXPORT QString EMU_to_ODF(const QString& emuValue);

//! Converts TWIP Unit of Measurement to cm.
/*! Converts value expressed in TWIPs  to ODF-compliant "0.000cm" unit.
    "0" and "" is converted to "0cm".
    @return empty string on error. */
MSOOXML_EXPORT QString TWIP_to_ODF(const QString& twipValue);

//! ECMA-376, 17.18.23 ST_EighthPointMeasure (Measurement in Eighths of a Point), p. 1540
/*! Converts eighths of a point (equivalent to 1/576th of an inch) to point
    to ODF-compliant "0.000pt" unit.
    @return empty string on failure. */
MSOOXML_EXPORT QString ST_EighthPointMeasure_to_ODF(const QString& value);

//! ECMA-376, 22.9.2.14 ST_TwipsMeasure (Measurement in Twentieths of a Point), p. 4339
/*! Converts:
    * Case 1: a positive number in twips (twentieths of a point, equivalent to 1/1440th of an inch), or
    * Case 2: a positive decimal number immediately followed by a unit identifier.
    The conversion's target is ODF-compliant "0.000xx" unit, where xx is "mm", "cm", "pt", etc.
    For case 1 it is always "pt".
    @return empty string on error. */
MSOOXML_EXPORT QString ST_TwipsMeasure_to_pt(const QString& value);

//! Like ST_TwipsMeasure_to_pt() but for case 1 always converts to "cm".
MSOOXML_EXPORT QString ST_TwipsMeasure_to_cm(const QString& value);

//! ECMA-376, 22.9.2.12 ST_PositiveUniversalMeasure (Positive Universal Measurement), p. 4340
/*! Converts number+unit of measurement into ODF-compliant number+unit.
    @a value should match the following regular expression pattern: [0-9]+(\.[0-9]+)?(mm|cm|in|pt|pc|pi).
    Values with units mm, cm, in, pt, pi are just copied.
    Values with "pc" (another name for Pica) are replaced with "pi".
    @return empty string on error. */
MSOOXML_EXPORT QString ST_PositiveUniversalMeasure_to_ODF(const QString& value);

//! Like ST_PositiveUniversalMeasure_to_ODF(const QString&) but always converts to cm.
MSOOXML_EXPORT QString ST_PositiveUniversalMeasure_to_cm(const QString& value);

} // Utils
} // MSOOXML

//! Performs EMU conversion and returns string.
inline QString EMU_TO_CM_STRING(int emu)
{
    return MSOOXML::Utils::cmString(EMU_TO_CM(qreal(emu)));
}

//! Performs EMU conversion and returns string.
inline QString EMU_TO_INCH_STRING(int emu)
{
    return MSOOXML::Utils::cmString(EMU_TO_INCH(qreal(emu)));
}

// px conversion
#define PT_TO_PX(pt) ((pt)*1.33597222222)
#define PX_TO_CM(px) ((px)*0.0264)

#endif /* MSOOXML_UNITS_H */
