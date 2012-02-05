/* This file is part of the KDE project
 *
 * Copyright (C) 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

// Own
#include "ThreedShape.h"

// Qt
#include <QPainter>
#include <QTimer>
#include <QPixmapCache>
#include <QtCore/QBuffer>

// KDE
#include <KDebug>
#include <KJob>
#include <KIO/Job>

// Calligra
#include <KoViewConverter.h>
#include <KoImageCollection.h>
#include <KoImageData.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoStoreDevice.h>
#include <KoUnit.h>
#include <KoGenStyle.h>
#include <KoFilterEffectStack.h>


ThreedShape::ThreedShape()
    : KoShape()
{
}

ThreedShape::~ThreedShape()
{
}

void ThreedShape::paint(QPainter &painter, const KoViewConverter &converter,
                        KoShapePaintingContext &context)
{
}


void ThreedShape::saveOdf(KoShapeSavingContext &context) const
{
}

bool ThreedShape::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    return true;
}


void ThreedShape::waitUntilReady(const KoViewConverter &converter, bool asynchronous) const
{
}


#include <ThreedShape.moc>
