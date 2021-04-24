/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SpellCheckDebug.h"

const QLoggingCategory &SPELLCHECK_LOG()
{
    static const QLoggingCategory category("calligra.plugin.spellcheck");
    return category;
}
