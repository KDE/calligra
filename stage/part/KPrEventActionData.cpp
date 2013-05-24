/* This file is part of the KDE project
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
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

#include "KPrEventActionData.h"

class KPrEventActionData::Private
{
public:
    Private(KoShape *s, KoEventAction *ea, KPrSoundCollection *sc)
            : shape(s),
            eventAction(ea),
            soundCollection(sc)
    {
    }

    KoShape *shape;
    KoEventAction *eventAction;
    KPrSoundCollection *soundCollection;
};

KPrEventActionData::KPrEventActionData( KoShape * shape, KoEventAction * eventAction , KPrSoundCollection * soundCollection )
    : d(new Private(shape, eventAction, soundCollection))
{
}

KPrEventActionData::~KPrEventActionData()
{
    delete d;
}

KPrSoundCollection * KPrEventActionData::soundCollection() const
{
    return d->soundCollection;
}

KoShape *KPrEventActionData::shape() const
{
    return d->shape;
}

KoEventAction *KPrEventActionData::eventAction() const
{
    return d->eventAction;
}
