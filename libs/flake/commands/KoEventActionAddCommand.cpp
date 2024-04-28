/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoEventActionAddCommand.h"
#include <KLocalizedString>

#include "KoEventAction.h"
#include "KoShape.h"

class KoEventActionAddCommandPrivate
{
public:
    KoEventActionAddCommandPrivate(KoShape *s, KoEventAction *a)
        : shape(s)
        , eventAction(a)
        , deleteEventAction(true)
    {
    }

    ~KoEventActionAddCommandPrivate()
    {
        if (deleteEventAction)
            delete eventAction;
    }
    KoShape *shape;
    KoEventAction *eventAction;
    bool deleteEventAction;
};

KoEventActionAddCommand::KoEventActionAddCommand(KoShape *shape, KoEventAction *eventAction, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new KoEventActionAddCommandPrivate(shape, eventAction))
{
}

KoEventActionAddCommand::~KoEventActionAddCommand()
{
    delete d;
}

void KoEventActionAddCommand::redo()
{
    d->shape->addEventAction(d->eventAction);
    d->deleteEventAction = false;
}

void KoEventActionAddCommand::undo()
{
    d->shape->removeEventAction(d->eventAction);
    d->deleteEventAction = true;
}
