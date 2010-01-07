/*
 * This file is part of Office 2007 Filters for KOffice
 * Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
 * Copyright (c) 2003 Lukas Tinkl <lukas@kde.org>
 * Copyright (C) 2003 David Faure <faure@kde.org>
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

//! Performs EMU conversion and returns string.
inline QString EMU_TO_CM_STRING(int emu)
{
    QString res;
    return res.sprintf("%3.3fcm", EMU_TO_CM(double(emu)));
}

//! Converts emu value (integer or double) to inches
#define EMU_TO_INCH(emu) ((emu)/914400.0)

//! Performs EMU conversion and returns string.
inline QString EMU_TO_INCH_STRING(int emu)
{
    QString res;
    return res.sprintf("%3.3fin", EMU_TO_INCH(double(emu)));
}

// px conversion
#define PT_TO_PX(pt) ((pt)*1.33597222222)
#define PX_TO_CM(px) ((px)*0.0264)

#endif /* MSOOXML_UNITS_H */
