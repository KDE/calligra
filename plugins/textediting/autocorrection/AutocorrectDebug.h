/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef AUTOCORRECT_DEBUG_H
#define AUTOCORRECT_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &AUTOCORRECT_LOG();

#define debugAutocorrect qCDebug(AUTOCORRECT_LOG)
#define warnAutocorrect qCWarning(AUTOCORRECT_LOG)
#define errorAutocorrect qCCritical(AUTOCORRECT_LOG)

#endif
