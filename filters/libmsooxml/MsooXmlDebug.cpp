/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "MsooXmlDebug.h"

const QLoggingCategory &MSOOXML_LOG()
{
    static const QLoggingCategory category("calligra.filter.msooxml");
    return category;
}
