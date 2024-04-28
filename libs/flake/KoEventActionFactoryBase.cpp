/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoEventActionFactoryBase.h"

class Q_DECL_HIDDEN KoEventActionFactoryBase::Private
{
public:
    Private(const QString &id, const QString &action)
        : id(id)
        , action(action)
    {
    }

    const QString id;
    const QString action;
};

KoEventActionFactoryBase::KoEventActionFactoryBase(const QString &id, const QString &action)
    : d(new Private(id, action))
{
}

KoEventActionFactoryBase::~KoEventActionFactoryBase()
{
    delete d;
}

QString KoEventActionFactoryBase::action() const
{
    return d->action;
}

QString KoEventActionFactoryBase::id() const
{
    return d->id;
}
