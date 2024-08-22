/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoFindMatch.h"

#include <QVariant>

class Q_DECL_HIDDEN KoFindMatch::Private : public QSharedData
{
public:
    Private() = default;
    ~Private() = default;
    Private(const Private &other)

        = default;

    QVariant container;
    QVariant location;
};

KoFindMatch::KoFindMatch()
    : d(new Private)
{
}

KoFindMatch::KoFindMatch(const QVariant &container, const QVariant &location)
    : d(new Private)
{
    d->container = container;
    d->location = location;
}

KoFindMatch::KoFindMatch(const KoFindMatch &other)

    = default;

KoFindMatch::~KoFindMatch() = default;

KoFindMatch &KoFindMatch::operator=(const KoFindMatch &other) = default;

bool KoFindMatch::operator==(const KoFindMatch &other) const
{
    return d->container == other.d->container && d->location == other.d->location;
}

bool KoFindMatch::isValid() const
{
    return d->container.isValid() && d->location.isValid();
}

QVariant KoFindMatch::container() const
{
    return d->container;
}

void KoFindMatch::setContainer(const QVariant &container)
{
    d->container = container;
}

QVariant KoFindMatch::location() const
{
    return d->location;
}

void KoFindMatch::setLocation(const QVariant &location)
{
    d->location = location;
}
