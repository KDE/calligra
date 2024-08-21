// SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <QDebug>

#include "StateCategory.h"
#include "StateCategory_p.h"

#include "State.h"

StateCategory::StateCategory(const QString &_id, const QString &_name, int _priority)
    : d(new Private)
{
    d->id = _id;
    d->name = _name;
    d->priority = _priority;
}

StateCategory::~StateCategory()
{
    delete d;
}

const QString &StateCategory::name() const
{
    return d->name;
}

const QString &StateCategory::id() const
{
    return d->id;
}

QList<QString> StateCategory::stateIds() const
{
    return d->states.keys();
}

const State *StateCategory::state(const QString &_id) const
{
    if (d->states.contains(_id))
        return d->states[_id];
    qWarning() << "No shape " << _id << " found in category " << name() << " choices: " << d->states.keys();
    return nullptr;
}

int StateCategory::priority() const
{
    return d->priority;
}
