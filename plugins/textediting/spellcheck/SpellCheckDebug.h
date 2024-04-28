/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SPELLCHECK_DEBUG_H
#define SPELLCHECK_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &SPELLCHECK_LOG();

#define debugSpellCheck qCDebug(SPELLCHECK_LOG)
#define warnSpellCheck qCWarning(SPELLCHECK_LOG)
#define errorSpellCheck qCCritical(SPELLCHECK_LOG)

#endif
