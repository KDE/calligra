/*  This file is part of the KDE project

    SPDX-FileCopyrightText: 2007 Sven Langkamp <sven.langkamp@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoResourceServerAdapter.h"

KoAbstractResourceServerAdapter::KoAbstractResourceServerAdapter(QObject *parent)
    : QObject(parent)
{
}

void KoAbstractResourceServerAdapter::emitResourceAdded(KoResource *resource)
{
    Q_EMIT resourceAdded(resource);
}

void KoAbstractResourceServerAdapter::emitRemovingResource(KoResource *resource)
{
    Q_EMIT removingResource(resource);
}

void KoAbstractResourceServerAdapter::emitResourceChanged(KoResource *resource)
{
    Q_EMIT resourceChanged(resource);
}

void KoAbstractResourceServerAdapter::emitTagsWereChanged()
{
    Q_EMIT tagsWereChanged();
}

void KoAbstractResourceServerAdapter::emitTagCategoryWasAdded(const QString &tag)
{
    Q_EMIT tagCategoryWasAdded(tag);
}

void KoAbstractResourceServerAdapter::emitTagCategoryWasRemoved(const QString &tag)
{
    Q_EMIT tagCategoryWasRemoved(tag);
}
