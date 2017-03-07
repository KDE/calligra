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

#include "State.h"

#include <QSvgRenderer>

struct Q_DECL_HIDDEN State::Private {
    QString id, name;
    StateCategory* category;
    QSvgRenderer* render;
    int priority;
};

State::State(const QString& _id, const QString& _name, StateCategory* _category, const QString& _fileName, int _priority) : d(new Private)
{
    d->id = _id;
    d->name = _name;
    d->category = _category;
    d->render = new QSvgRenderer(_fileName);
    d->priority = _priority;
}

State::~State()
{
    delete d->render;
    delete d;
}

const QString& State::name() const
{
    return d->name;
}

const QString& State::id() const
{
    return d->id;
}

const StateCategory* State::category() const
{
    return d->category;
}

QSvgRenderer* State::renderer() const
{
    return d->render;
}

int State::priority() const
{
    return d->priority;
}
