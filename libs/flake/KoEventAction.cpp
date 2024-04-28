/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoEventAction.h"

#include <QString>

class Q_DECL_HIDDEN KoEventAction::Private
{
public:
    QString id;
};

KoEventAction::KoEventAction()
    : d(new Private())
{
}

KoEventAction::~KoEventAction()
{
    delete d;
}

QString KoEventAction::id() const
{
    return d->id;
}

void KoEventAction::setId(const QString &id)
{
    d->id = id;
}
