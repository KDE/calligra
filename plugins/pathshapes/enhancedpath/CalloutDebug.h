/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CALLOUTDEBUG_H
#define CALLOUTDEBUG_H

#include <QDebug>
#include <QLoggingCategory>

#include <KoPathShape.h>

class KoPathPoint;
class KoProperties;

QDebug operator<<(QDebug dbg, KoPathPoint *p);
QDebug operator<<(QDebug dbg, KoSubpath *p);

QDebug operator<<(QDebug dbg, KoProperties &p);

extern const QLoggingCategory &CALLOUT_LOG();
#define debugCallout qCDebug(CALLOUT_LOG)
#define debugCalloutF qCDebug(CALLOUT_LOG) << Q_FUNC_INFO
#define warnCallout qCWarning(CALLOUT_LOG)
#define errorCallout qCCritical(CALLOUT_LOG)

#endif // CALLOUTDEBUG_H
