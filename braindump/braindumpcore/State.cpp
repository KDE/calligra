// SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "State.h"

#include <QSvgRenderer>

struct Q_DECL_HIDDEN State::Private {
    QString id, name;
    StateCategory *category;
    QSvgRenderer *render;
    int priority;
};

State::State(const QString &_id, const QString &_name, StateCategory *_category, const QString &_fileName, int _priority)
    : d(new Private)
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

const QString &State::name() const
{
    return d->name;
}

const QString &State::id() const
{
    return d->id;
}

const StateCategory *State::category() const
{
    return d->category;
}

QSvgRenderer *State::renderer() const
{
    return d->render;
}

int State::priority() const
{
    return d->priority;
}
