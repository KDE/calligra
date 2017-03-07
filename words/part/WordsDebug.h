/*
 * Copyright (c) 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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
