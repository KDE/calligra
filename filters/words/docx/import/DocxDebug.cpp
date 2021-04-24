/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "DocxDebug.h"

const QLoggingCategory &DOCX_LOG()
{
    static const QLoggingCategory category("calligra.filter.docx2odt");
    return category;
}
