/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoInlineObjectFactoryBase.h"
#include <KoProperties.h>

#include <QStringList>

class InlineObjectFactoryPrivate
{
public:
    InlineObjectFactoryPrivate(const QString &identifier)
        : id(identifier)
    {
    }

    ~InlineObjectFactoryPrivate()
    {
        foreach (const KoInlineObjectTemplate &t, templates)
            delete t.properties;
        templates.clear();
    }

    const QString id;
    QString iconName;
    QString odfNameSpace;
    QStringList odfElementNames;
    QVector<KoInlineObjectTemplate> templates;
    KoInlineObjectFactoryBase::ObjectType type;
};

KoInlineObjectFactoryBase::KoInlineObjectFactoryBase(const QString &id, ObjectType type)
    : d(new InlineObjectFactoryPrivate(id))
{
    d->type = type;
}

KoInlineObjectFactoryBase::~KoInlineObjectFactoryBase()
{
    delete d;
}

QString KoInlineObjectFactoryBase::id() const
{
    return d->id;
}

QVector<KoInlineObjectTemplate> KoInlineObjectFactoryBase::templates() const
{
    return d->templates;
}

void KoInlineObjectFactoryBase::addTemplate(const KoInlineObjectTemplate &params)
{
    d->templates.append(params);
}

QStringList KoInlineObjectFactoryBase::odfElementNames() const
{
    return d->odfElementNames;
}

QString KoInlineObjectFactoryBase::odfNameSpace() const
{
    return d->odfNameSpace;
}

void KoInlineObjectFactoryBase::setOdfElementNames(const QString &nameSpace, const QStringList &names)
{
    d->odfNameSpace = nameSpace;
    d->odfElementNames = names;
}

KoInlineObjectFactoryBase::ObjectType KoInlineObjectFactoryBase::type() const
{
    return d->type;
}
