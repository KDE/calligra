/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoShapeBackground.h"

#include "KoShapeBackground_p.h"

KoShapeBackgroundPrivate::KoShapeBackgroundPrivate() = default;

KoShapeBackgroundPrivate::~KoShapeBackgroundPrivate() = default;

KoShapeBackground::KoShapeBackground(KoShapeBackgroundPrivate &dd)
    : d_ptr(&dd)
{
}

KoShapeBackground::KoShapeBackground()
    : d_ptr(new KoShapeBackgroundPrivate())
{
}

KoShapeBackground::~KoShapeBackground()
{
    delete d_ptr;
}

bool KoShapeBackground::hasTransparency() const
{
    return false;
}
