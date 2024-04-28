/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoEventActionRemoveCommand.h"
#include <KLocalizedString>

#include "KoEventAction.h"
#include "KoShape.h"

class KoEventActionRemoveCommandPrivate
{
public:
    KoEventActionRemoveCommandPrivate(KoShape *s, KoEventAction *e)
        : shape(s)
        , eventAction(e)
        , deleteEventAction(false)
    {
    }

    ~KoEventActionRemoveCommandPrivate()
    {
        if (deleteEventAction)
            delete eventAction;
    }

    KoShape *shape;
    KoEventAction *eventAction;
    bool deleteEventAction;
};

KoEventActionRemoveCommand::KoEventActionRemoveCommand(KoShape *shape, KoEventAction *eventAction, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new KoEventActionRemoveCommandPrivate(shape, eventAction))
{
}

KoEventActionRemoveCommand::~KoEventActionRemoveCommand()
{
    delete d;
}

void KoEventActionRemoveCommand::redo()
{
    d->shape->removeEventAction(d->eventAction);
    d->deleteEventAction = true;
}

void KoEventActionRemoveCommand::undo()
{
    d->shape->addEventAction(d->eventAction);
    d->deleteEventAction = false;
}
