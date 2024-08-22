/*
 *  SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <KoColorSpaceEngine.h>
#include <QGlobalStatic>
#include <QString>

Q_GLOBAL_STATIC(KoColorSpaceEngineRegistry, s_instance);

struct Q_DECL_HIDDEN KoColorSpaceEngine::Private {
    QString id;
    QString name;
};

KoColorSpaceEngine::KoColorSpaceEngine(const QString &id, const QString &name)
    : d(new Private)
{
    d->id = id;
    d->name = name;
}

KoColorSpaceEngine::~KoColorSpaceEngine()
{
    delete d;
}

const QString &KoColorSpaceEngine::id() const
{
    return d->id;
}

const QString &KoColorSpaceEngine::name() const
{
    return d->name;
}

KoColorSpaceEngineRegistry::KoColorSpaceEngineRegistry() = default;

KoColorSpaceEngineRegistry::~KoColorSpaceEngineRegistry()
{
    foreach (KoColorSpaceEngine *item, values()) {
        delete item;
    }
}

KoColorSpaceEngineRegistry *KoColorSpaceEngineRegistry::instance()
{
    return s_instance;
}
