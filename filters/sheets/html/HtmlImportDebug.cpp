/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "HtmlImportDebug.h"

const QLoggingCategory &HTMLIMPORT_LOG()
{
    static const QLoggingCategory category("calligra.filter.html2ods");
    return category;
}
