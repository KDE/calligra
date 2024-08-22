/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoFindOption.h"

class Q_DECL_HIDDEN KoFindOption::Private
{
public:
    Private() = default;
    ~Private() = default;

    QString name;
    QString title;
    QString description;
    QVariant value;
};

KoFindOption::KoFindOption(const QString &name, QObject *parent)
    : QObject(parent)
    , d(new Private)
{
    d->name = name;
}

KoFindOption::~KoFindOption()
{
    delete d;
}

QString KoFindOption::name() const
{
    return d->name;
}

QString KoFindOption::title() const
{
    return d->title;
}

QString KoFindOption::description() const
{
    return d->description;
}

QVariant KoFindOption::value() const
{
    return d->value;
}

void KoFindOption::setTitle(const QString &newTitle)
{
    d->title = newTitle;
}

void KoFindOption::setDescription(const QString &newDescription)
{
    d->description = newDescription;
}

void KoFindOption::setValue(const QVariant &newValue)
{
    d->value = newValue;
}
