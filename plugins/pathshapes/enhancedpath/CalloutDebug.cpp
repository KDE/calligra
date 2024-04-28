/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "CalloutDebug.h"

#include <KoPathPoint.h>
#include <KoProperties.h>

QDebug operator<<(QDebug dbg, KoPathPoint *p)
{
    if (p) {
        dbg << p->point();
    } else {
        dbg << (void *)p;
    }
    return dbg;
}

QDebug operator<<(QDebug dbg, KoSubpath *p)
{
    if (p) {
        dbg << *p;
    } else {
        dbg << (void *)p;
    }
    return dbg;
}

QDebug operator<<(QDebug dbg, KoProperties &p)
{
    dbg << p.store("Properties");
    return dbg;
}

const QLoggingCategory &CALLOUT_LOG()
{
    static const QLoggingCategory category("calligra.plugin.callout");
    return category;
}
