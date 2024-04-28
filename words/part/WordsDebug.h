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

#define debugWords qCDebug(WORDS_LOG) << Q_FUNC_INFO
#define warnWords qCWarning(WORDS_LOG) << Q_FUNC_INFO
#define errorWords qCCritical(WORDS_LOG) << Q_FUNC_INFO

extern const WORDS_EXPORT QLoggingCategory &WORDSUI_LOG();

#define debugWordsUI qCDebug(WORDSUI_LOG) << Q_FUNC_INFO
#define warnWordsUI qCWarning(WORDSUI_LOG) << Q_FUNC_INFO
#define errorWordsUI qCCritical(WORDSUI_LOG) << Q_FUNC_INFO

#endif
