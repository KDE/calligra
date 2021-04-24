/*
 * This file is part of Office 2007 Filters for Calligra
 * SPDX-FileCopyrightText: 2002 Laurent Montel <lmontel@mandrakesoft.com>
 * SPDX-FileCopyrightText: 2003 Lukas Tinkl <lukas@kde.org>
 * SPDX-FileCopyrightText: 2003 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2009 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#ifndef MSOOXML_GLOBAL_H
#define MSOOXML_GLOBAL_H

#include "komsooxml_export.h"

//! Common utilities for handling MSOOXML formats
namespace MSOOXML
{

//! @return maximum number of spreadsheet columns per worksheet. Currently set to 32767, which is Calligra Sheets maximum.
//! See http://en.wikipedia.org/wiki/OpenOffice.org_Calc#Specifications
KOMSOOXML_EXPORT unsigned int maximumSpreadsheetColumns();

//! @return maximum number of spreadsheet rows per worksheet. Currently set to 32767, which is Calligra Sheets maximum.
KOMSOOXML_EXPORT unsigned int maximumSpreadsheetRows();

} // MSOOXML namespace

#endif /* MSOOXML_GLOBAL_H */
