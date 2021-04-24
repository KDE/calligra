/*  This file is part of the KDE project

    SPDX-FileCopyrightText: 2007 Sven Langkamp <sven.langkamp@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoResourceServerAdapter.h"

KoAbstractResourceServerAdapter::KoAbstractResourceServerAdapter(QObject *parent)
    : QObject(parent)
{
}

void KoAbstractResourceServerAdapter::emitResourceAdded(KoResource* resource)
{
    emit resourceAdded(resource);
}

void KoAbstractResourceServerAdapter::emitRemovingResource(KoResource* resource)
{
    emit removingResource(resource);
}

void KoAbstractResourceServerAdapter::emitResourceChanged(KoResource* resource)
{
    emit resourceChanged(resource);
}

void KoAbstractResourceServerAdapter::emitTagsWereChanged()
{
    emit tagsWereChanged();
}

void KoAbstractResourceServerAdapter::emitTagCategoryWasAdded(const QString& tag)
{
    emit tagCategoryWasAdded(tag);
}

void KoAbstractResourceServerAdapter::emitTagCategoryWasRemoved(const QString& tag)
{
    emit tagCategoryWasRemoved(tag);
}
