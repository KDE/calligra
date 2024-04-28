/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PICTURE_DEBUG_H
#define PICTURE_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &PICTURE_LOG();

#define debugPicture qCDebug(PICTURE_LOG) << Q_FUNC_INFO
#define warnPicture qCWarning(PICTURE_LOG)
#define errorPicture qCCritical(PICTURE_LOG)

#endif
