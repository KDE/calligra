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

#include "CalloutDebug.h"

#include <KoPathPoint.h>
#include <KoProperties.h>


QDebug operator<<(QDebug dbg, KoPathPoint *p)
{
    if (p) {
        dbg << p->point();
    } else {
        dbg << (void*)p;
    }
    return dbg;
}

QDebug operator<<(QDebug dbg, KoSubpath *p)
{
    if (p) {
        dbg << *p;
    } else {
        dbg << (void*)p;
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
