/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "PptxDebug.h"

const QLoggingCategory &PPTX_LOG()
{
    static const QLoggingCategory category("calligra.filter.pptx2odp");
    return category;
}
