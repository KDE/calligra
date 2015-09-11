/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "FreeLayout.h"

#include <QRectF>

#include <KLocalizedString>

#include <KoShapeContainer.h>

FreeLayout::FreeLayout() : Layout("freelayout")
{
}

FreeLayout::~FreeLayout()
{
}

QRectF FreeLayout::boundingBox() const
{
    QRectF b = Layout::boundingBox();
    qreal margin = qMin<qreal>(200.0, 0.5 * (b.width() + b.height()));
    return b.adjusted(-margin, -margin, margin, margin);
}

void FreeLayout::shapeAdded(KoShape*)
{
}

void FreeLayout::shapeRemoved(KoShape*)
{
}

void FreeLayout::shapeGeometryChanged(KoShape*)
{
}

void FreeLayout::relayout()
{
    emit(boundingBoxChanged(boundingBox()));
}

FreeLayoutFactory::FreeLayoutFactory() : LayoutFactory("freelayout", i18n("Free"))
{
}

FreeLayoutFactory::~FreeLayoutFactory()
{
}

Layout* FreeLayoutFactory::createLayout() const
{
    return new FreeLayout;
}
