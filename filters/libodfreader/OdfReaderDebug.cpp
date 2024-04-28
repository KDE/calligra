/*
 *  SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "OdfReaderDebug.h"

const QLoggingCategory &ODFREADER_LOG()
{
    static const QLoggingCategory category("calligra.filter.odfreader");
    return category;
}
