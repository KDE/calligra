/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#include "KPrShapeManagerDisplayMasterStrategy.h"

#include "KPrPageSelectStrategyBase.h"
#include "KPrPlaceholderShape.h"
#include <KoPAPageBase.h>

KPrShapeManagerDisplayMasterStrategy::KPrShapeManagerDisplayMasterStrategy(KoShapeManager *shapeManager, KPrPageSelectStrategyBase *strategy)
    : KoShapeManagerPaintingStrategy(shapeManager)
    , m_strategy(strategy)
{
}

KPrShapeManagerDisplayMasterStrategy::~KPrShapeManagerDisplayMasterStrategy()
{
    delete m_strategy;
}

void KPrShapeManagerDisplayMasterStrategy::paint(KoShape *shape, QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintContext)
{
    if (!dynamic_cast<KPrPlaceholderShape *>(shape)) {
        if (m_strategy->page()->displayShape(shape)) {
            KoShapeManagerPaintingStrategy::paint(shape, painter, converter, paintContext);
        }
    }
}
