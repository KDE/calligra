/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "PdfImportDebug.h"

const QLoggingCategory &PDFIMPORT_LOG()
{
    static const QLoggingCategory category("calligra.filter.pdf");
    return category;
}
