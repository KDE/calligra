/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoShapeStrokeModel.h"

#include <QAtomicInt>

class Q_DECL_HIDDEN KoShapeStrokeModel::Private
{
public:
    Private()
        : refCount(0)
    {
    }
    QAtomicInt refCount;
};

KoShapeStrokeModel::KoShapeStrokeModel()
    : d(new Private())
{
}

KoShapeStrokeModel::~KoShapeStrokeModel()
{
    delete d;
}

bool KoShapeStrokeModel::ref()
{
    return d->refCount.ref();
}

bool KoShapeStrokeModel::deref()
{
    return d->refCount.deref();
}

int KoShapeStrokeModel::useCount() const
{
    return d->refCount;
}
