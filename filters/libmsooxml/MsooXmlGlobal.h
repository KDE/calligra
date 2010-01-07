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

#ifndef MSOOXML_GLOBAL_H
#define MSOOXML_GLOBAL_H

#include "msooxml_export.h"

//! Common utilities for handling MSOOXML formats
namespace MSOOXML
{

//! @return maximum number of spreadsheet columns per worksheet. Currently set to 32767, which is KSpread maximum.
//! See http://en.wikipedia.org/wiki/OpenOffice.org_Calc#Specifications
MSOOXML_EXPORT uint maximumSpreadsheetColumns();

//! @return maximum number of spreadsheet rows per worksheet. Currently set to 32767, which is KSpread maximum.
MSOOXML_EXPORT uint maximumSpreadsheetRows();

} // MSOOXML namespace

#endif /* MSOOXML_GLOBAL_H */
