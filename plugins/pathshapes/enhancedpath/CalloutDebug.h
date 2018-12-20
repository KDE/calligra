/* This file is part of the KDE project
 * Copyright (C) 2018 Dag Andersen <danders@get2net.dk>
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
#define debugCalloutF qCDebug(CALLOUT_LOG)<<Q_FUNC_INFO
#define warnCallout qCWarning(CALLOUT_LOG)
#define errorCallout qCCritical(CALLOUT_LOG)

#endif // CALLOUTDEBUG_H
