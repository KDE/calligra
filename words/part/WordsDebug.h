/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef WORDS_DEBUG_H
#define WORDS_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

#include <words_export.h>

extern const WORDS_EXPORT QLoggingCategory &WORDS_LOG();

#define debugWords qCDebug(WORDS_LOG)
#define warnWords qCWarning(WORDS_LOG)
#define errorWords qCCritical(WORDS_LOG)

extern const WORDS_EXPORT QLoggingCategory &WORDSUI_LOG();

#define debugWordsUI qCDebug(WORDSUI_LOG)
#define warnWordsUI qCWarning(WORDSUI_LOG)
#define errorWordsUI qCCritical(WORDSUI_LOG)

#endif
