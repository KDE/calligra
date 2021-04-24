/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-only
 */

#include "WmfImportDebug.h"

const QLoggingCategory &WMFIMPORT_LOG()
{
    static const QLoggingCategory category("calligra.filter.wmf2svg");
    return category;
}
