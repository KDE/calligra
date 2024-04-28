/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTextEditingFactory.h"

#include <QString>

class Q_DECL_HIDDEN KoTextEditingFactory::Private
{
public:
    Private(const QString &identifier)
        : id(identifier)
        , showInMenu(false)
    {
    }

    const QString id;
    bool showInMenu;
    QString title;
};

KoTextEditingFactory::KoTextEditingFactory(const QString &id)
    : d(new Private(id))
{
}

KoTextEditingFactory::~KoTextEditingFactory()
{
    delete d;
}

QString KoTextEditingFactory::id() const
{
    return d->id;
}

bool KoTextEditingFactory::showInMenu() const
{
    return d->showInMenu;
}

void KoTextEditingFactory::setShowInMenu(bool show)
{
    d->showInMenu = show;
}

QString KoTextEditingFactory::title() const
{
    return d->title;
}

void KoTextEditingFactory::setTitle(const QString &title)
{
    d->title = title;
}
