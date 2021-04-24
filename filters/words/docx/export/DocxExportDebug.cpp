/*
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "DocxExportDebug.h"

const QLoggingCategory &DOCXEXPORT_LOG()
{
    static const QLoggingCategory category("calligra.filter.odt2docx");
    return category;
}
