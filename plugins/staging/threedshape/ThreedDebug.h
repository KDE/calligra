/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef THREED_DEBUG_H
#define THREED_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &THREED_LOG();

#define debugThreed qCDebug(THREED_LOG)
#define warnThreed qCWarning(THREED_LOG)
#define errorThreed qCCritical(THREED_LOG)

#endif
