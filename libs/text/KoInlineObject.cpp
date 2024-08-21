/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2009 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoInlineObject.h"
#include "KoInlineObject_p.h"
#include "KoInlineTextObjectManager.h"
#include "KoTextInlineRdf.h"

#include "TextDebug.h"

QDebug KoInlineObjectPrivate::printDebug(QDebug dbg) const
{
    dbg.nospace() << "KoInlineObject ManagerId: " << id;
    return dbg.space();
}

KoInlineObjectPrivate::~KoInlineObjectPrivate()
{
    delete rdf;
}

KoInlineObject::KoInlineObject(bool propertyChangeListener)
    : d_ptr(new KoInlineObjectPrivate)
{
    Q_D(KoInlineObject);
    d->propertyChangeListener = propertyChangeListener;
}

KoInlineObject::KoInlineObject(KoInlineObjectPrivate &priv, bool propertyChangeListener)
    : d_ptr(&priv)
{
    Q_D(KoInlineObject);
    d->propertyChangeListener = propertyChangeListener;
}

KoInlineObject::~KoInlineObject()
{
    if (d_ptr->manager) {
        d_ptr->manager->removeInlineObject(this);
    }
    delete d_ptr;
    d_ptr = nullptr;
}

void KoInlineObject::setManager(KoInlineTextObjectManager *manager)
{
    Q_D(KoInlineObject);
    d->manager = manager;
}

KoInlineTextObjectManager *KoInlineObject::manager() const
{
    Q_D(const KoInlineObject);
    return d->manager;
}

void KoInlineObject::propertyChanged(Property key, const QVariant &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

int KoInlineObject::id() const
{
    Q_D(const KoInlineObject);
    return d->id;
}

void KoInlineObject::setId(int id)
{
    Q_D(KoInlineObject);
    d->id = id;
}

bool KoInlineObject::propertyChangeListener() const
{
    Q_D(const KoInlineObject);
    return d->propertyChangeListener;
}

QDebug operator<<(QDebug dbg, const KoInlineObject *o)
{
    return o ? o->d_func()->printDebug(dbg) : dbg << "KoInlineObject 0";
    ;
}

void KoInlineObject::setInlineRdf(KoTextInlineRdf *rdf)
{
    Q_D(KoInlineObject);
    d->rdf = rdf;
}

KoTextInlineRdf *KoInlineObject::inlineRdf() const
{
    Q_D(const KoInlineObject);
    return d->rdf;
}
