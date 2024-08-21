/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoFindOptionSet.h"
#include "KoFindOption.h"

#include <QHash>

class Q_DECL_HIDDEN KoFindOptionSet::Private
{
public:
    Private()
        : nextID(0)
    {
    }
    QHash<QString, KoFindOption *> options;

    int nextID;
};

KoFindOptionSet::KoFindOptionSet(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
}

KoFindOptionSet::~KoFindOptionSet()
{
    qDeleteAll(d->options);
    delete d;
}

KoFindOption *KoFindOptionSet::option(const QString &name) const
{
    if (d->options.contains(name)) {
        return d->options.value(name);
    }
    return nullptr;
}

QList<KoFindOption *> KoFindOptionSet::options() const
{
    return d->options.values();
}

KoFindOption *KoFindOptionSet::addOption(const QString &name)
{
    KoFindOption *newOption = new KoFindOption(name);
    d->options.insert(name, newOption);
    return newOption;
}

KoFindOption *KoFindOptionSet::addOption(const QString &name, const QString &title, const QString &description, const QVariant &value)
{
    KoFindOption *newOption = new KoFindOption(name);
    newOption->setTitle(title);
    newOption->setDescription(description);
    newOption->setValue(value);
    d->options.insert(name, newOption);
    return newOption;
}

void KoFindOptionSet::removeOption(const QString &name)
{
    if (d->options.contains(name)) {
        d->options.remove(name);
    }
}

void KoFindOptionSet::setOptionValue(const QString &name, const QVariant &value)
{
    if (d->options.contains(name)) {
        d->options.value(name)->setValue(value);
    }
}

void KoFindOptionSet::replaceOption(const QString &name, KoFindOption *newOption)
{
    Q_ASSERT(newOption);
    if (d->options.contains(name)) {
        d->options.insert(name, newOption);
    }
}
