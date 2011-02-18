/* This file is part of the KDE project
   Copyright (C) 2004-2006 David Faure <faure@kde.org>
   Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>
   Copyright (C) 2011 Inge Wallin <inge@lysator.liu.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

// Own
#include "KoShapePaintingContext.h"

// Qt
#include <QMap>

// KDE
#include <kdebug.h>

// Calligra
#include "KoViewConverter.h"


class KoShapePaintingContextPrivate {
public:
    KoShapePaintingContextPrivate(KoViewConverter&);

    KoViewConverter& viewConverter;
    KoShapePaintingContext::ShapePaintingOptions paintingOptions;
};

KoShapePaintingContextPrivate::KoShapePaintingContextPrivate(KoViewConverter &v)
    : viewConverter(v)
{
}

// ----------------------------------------------------------------

KoShapePaintingContext::KoShapePaintingContext(KoViewConverter &viewConverter)
    : d(new KoShapePaintingContextPrivate(viewConverter))
{
}

KoShapePaintingContext::~KoShapePaintingContext()
{
}

KoViewConverter& KoShapePaintingContext::viewConverter() const
{
    return d->viewConverter;
}

void KoShapePaintingContext::setViewConverter(KoViewConverter &viewConverter)
{
    d->viewConverter = viewConverter;
}

bool KoShapePaintingContext::isSet(ShapePaintingOption option) const
{
    return d->paintingOptions & option;
}

void KoShapePaintingContext::setOptions(ShapePaintingOptions options)
{
    d->paintingOptions = options;
}

KoShapePaintingContext::ShapePaintingOptions KoShapePaintingContext::options() const
{
    return d->paintingOptions;
}

void KoShapePaintingContext::addOption(ShapePaintingOption option)
{
    d->paintingOptions = d->paintingOptions | option;
}

void KoShapePaintingContext::removeOption(ShapePaintingOption option)
{
    if (isSet(option))
        d->paintingOptions = d->paintingOptions ^ option; // xor to remove it.
}

