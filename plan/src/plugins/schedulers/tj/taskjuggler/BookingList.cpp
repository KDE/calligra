/*
 * BookingList.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */
#include "BookingList.h"

int
BookingList::compareItems(QCollection::Item i1, QCollection::Item i2)
{
    Booking* b1 = static_cast<Booking*>(i1);
    Booking* b2 = static_cast<Booking*>(i2);

    return b1->getInterval().compare(b2->getInterval());
}

