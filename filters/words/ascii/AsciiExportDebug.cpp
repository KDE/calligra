/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "AsciiExportDebug.h"

const QLoggingCategory &ASCIIEXPORT_LOG()
{
    static const QLoggingCategory category("calligra.filter.odt2ascii");
    return category;
}
