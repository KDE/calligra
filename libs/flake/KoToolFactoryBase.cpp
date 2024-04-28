/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoToolFactoryBase.h"

#include "KoToolBase.h"

#include <QKeySequence>

class Q_DECL_HIDDEN KoToolFactoryBase::Private
{
public:
    Private(const QString &i)
        : priority(100)
        , id(i)
    {
    }
    int priority;
    QString toolType;
    QString tooltip;
    QString activationId;
    QString iconName;
    const QString id;
    QKeySequence shortcut;
};

KoToolFactoryBase::KoToolFactoryBase(const QString &id)
    : d(new Private(id))
{
}

KoToolFactoryBase::~KoToolFactoryBase()
{
    delete d;
}

QString KoToolFactoryBase::id() const
{
    return d->id;
}

int KoToolFactoryBase::priority() const
{
    return d->priority;
}

QString KoToolFactoryBase::toolType() const
{
    return d->toolType;
}

QString KoToolFactoryBase::toolTip() const
{
    return d->tooltip;
}

QString KoToolFactoryBase::iconName() const
{
    return d->iconName;
}

QString KoToolFactoryBase::activationShapeId() const
{
    return d->activationId;
}

QKeySequence KoToolFactoryBase::shortcut() const
{
    return d->shortcut;
}

void KoToolFactoryBase::setActivationShapeId(const QString &activationShapeId)
{
    d->activationId = activationShapeId;
}

void KoToolFactoryBase::setToolTip(const QString &tooltip)
{
    d->tooltip = tooltip;
}

void KoToolFactoryBase::setToolType(const QString &toolType)
{
    d->toolType = toolType;
}

void KoToolFactoryBase::setIconName(const QString &iconName)
{
    d->iconName = iconName;
}

void KoToolFactoryBase::setPriority(int newPriority)
{
    d->priority = newPriority;
}

void KoToolFactoryBase::setShortcut(const QKeySequence &shortcut)
{
    d->shortcut = shortcut;
}
