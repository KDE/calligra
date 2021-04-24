/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "AsciiImportDebug.h"

const QLoggingCategory &ASCIIIMPORT_LOG()
{
    static const QLoggingCategory category("calligra.filter.ascii2words");
    return category;
}
