/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef VIDEO_DEBUG_H
#define VIDEO_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &VIDEO_LOG();

#define debugVideo qCDebug(VIDEO_LOG)
#define warnVideo qCWarning(VIDEO_LOG)
#define errorVideo qCCritical(VIDEO_LOG)

#endif
