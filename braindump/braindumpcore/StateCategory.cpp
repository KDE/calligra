/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QDebug>

#include "StateCategory.h"
#include "StateCategory_p.h"

#include "State.h"

StateCategory::StateCategory(const QString& _id, const QString& _name, int _priority) : d(new Private)
{
    d->id = _id;
    d->name = _name;
    d->priority = _priority;
}

StateCategory::~StateCategory()
{
    delete d;
}

const QString& StateCategory::name() const
{
    return d->name;
}

const QString& StateCategory::id() const
{
    return d->id;
}

QList<QString> StateCategory::stateIds() const
{
    return d->states.keys();
}

const State* StateCategory::state(const QString& _id) const
{
    if(d->states.contains(_id)) return d->states[_id];
    qWarning() << "No shape " << _id << " found in category " << name() << " choices: " << d->states.keys();
    return 0;
}

int StateCategory::priority() const
{
    return d->priority;
}

