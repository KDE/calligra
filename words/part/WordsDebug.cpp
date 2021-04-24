/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "WordsDebug.h"

const QLoggingCategory &WORDS_LOG()
{
    static const QLoggingCategory category("calligra.words");
    return category;
}

const QLoggingCategory &WORDSUI_LOG()
{
    static const QLoggingCategory category("calligra.words.ui");
    return category;
}
