/*
 *  SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "PageAppDebug.h"

const QLoggingCategory &PAGEAPP_LOG()
{
    static const QLoggingCategory category("calligra.lib.pageapp");
    return category;
}
