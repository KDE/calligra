/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef MUSIC_DEBUG_H
#define MUSIC_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &MUSIC_LOG();

#define debugMusic qCDebug(MUSIC_LOG)
#define warnMusic qCWarning(MUSIC_LOG)
#define errorMusic qCCritical(MUSIC_LOG)

#endif
