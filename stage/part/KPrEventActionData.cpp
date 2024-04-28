/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrEventActionData.h"

#include <QtGlobal>

class Q_DECL_HIDDEN KPrEventActionData::Private
{
public:
    Private(KoShape *s, KoEventAction *ea, KPrSoundCollection *sc)
        : shape(s)
        , eventAction(ea)
        , soundCollection(sc)
    {
    }

    KoShape *shape;
    KoEventAction *eventAction;
    KPrSoundCollection *soundCollection;
};

KPrEventActionData::KPrEventActionData(KoShape *shape, KoEventAction *eventAction, KPrSoundCollection *soundCollection)
    : d(new Private(shape, eventAction, soundCollection))
{
}

KPrEventActionData::~KPrEventActionData()
{
    delete d;
}

KPrSoundCollection *KPrEventActionData::soundCollection() const
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
