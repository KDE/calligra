/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KarbonUiDebug.h"

const QLoggingCategory &KARBONUI_LOG()
{
    static const QLoggingCategory category("calligra.karbon.ui");
    return category;
}
