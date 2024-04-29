/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2014 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoResourceItemChooserSync.h"

#include <QGlobalStatic>

Q_GLOBAL_STATIC(KoResourceItemChooserSync, s_instance)

struct Q_DECL_HIDDEN KoResourceItemChooserSync::Private {
    int baseLength;
};

KoResourceItemChooserSync::KoResourceItemChooserSync()
    : d(new Private)
{
    d->baseLength = 50;
}

KoResourceItemChooserSync::~KoResourceItemChooserSync() = default;

KoResourceItemChooserSync *KoResourceItemChooserSync::instance()
{
    return s_instance;
}

int KoResourceItemChooserSync::baseLength()
{
    return d->baseLength;
}

void KoResourceItemChooserSync::setBaseLength(int length)
{
    d->baseLength = qBound(50, length, 150);
    Q_EMIT baseLenghtChanged(d->baseLength);
}
