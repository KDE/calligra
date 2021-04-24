/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "StencilBoxDebug.h"

const QLoggingCategory &STENCILBOX_LOG()
{
    static const QLoggingCategory category("calligra.plugin.stencilboxdock");
    return category;
}
