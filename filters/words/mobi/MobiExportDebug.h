/*
  SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MOBIEXPORT_DEBUG_H
#define MOBIEXPORT_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>

extern const QLoggingCategory &MOBIEXPORT_LOG();

#define debugMobi qCDebug(MOBIEXPORT_LOG)
#define warnMobi qCWarning(MOBIEXPORT_LOG)
#define errorMobi qCCritical(MOBIEXPORT_LOG)

#endif
