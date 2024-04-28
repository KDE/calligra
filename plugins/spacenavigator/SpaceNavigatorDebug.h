/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SPACENAVIGATOR_DEBUG_H
#define SPACENAVIGATOR_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &SPACENAVIGATOR_LOG();

#define debugSpaceNavigator qCDebug(SPACENAVIGATOR_LOG)
#define warnSpaceNavigator qCWarning(SPACENAVIGATOR_LOG)
#define errorSpaceNavigator qCCritical(SPACENAVIGATOR_LOG)

#endif
