/*
 * Interval.h - TaskJuggler
 *
 * Copyright (c) 2011 by Dag Andersen <danders@get2net.dk>
 * Copyright (c) 2011 by Dag Andersen <danders@get2net.dk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#include "Interval.h"

QDebug operator<<( QDebug dbg, const TJ::Interval *i )
{
    return i == 0 ? (dbg << (void*)i) : operator<<( dbg, *i );
}

QDebug operator<<( QDebug dbg, const TJ::Interval &i )
{
    dbg << "Interval[";
    if ( i.isNull() ) dbg << "Null";
    else dbg << TJ::time2ISO(i.getStart())<<"-"<<TJ::time2ISO(i.getEnd());
    dbg << "]";
    return dbg;
}
