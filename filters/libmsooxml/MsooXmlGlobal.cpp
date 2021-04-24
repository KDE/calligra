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

#include "MsooXmlGlobal.h"

KOMSOOXML_EXPORT unsigned int MSOOXML::maximumSpreadsheetColumns()
{
    return 0x7FFF;
}

KOMSOOXML_EXPORT unsigned int MSOOXML::maximumSpreadsheetRows()
{
    return 0xFFFFF;
}
