/* This file is part of the KDE project
 * Copyright (C) 2014 Wenchao Li <manchiu.lee.9@gmail.com>
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

#include "VariableLineShape.h"

#include <KoColorBackground.h>
#include <QPainter>
#include <klocale.h>

VariableLineShape::VariableLineShape()
: m_widthPercentage(100)
{
    setFillRule(Qt::WindingFill);
    createPath();
}

VariableLineShape::~VariableLineShape()
{
}

bool VariableLineShape::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    return true;
}

void VariableLineShape::saveOdf(KoShapeSavingContext & context) const
{
}

void VariableLineShape::createPath()
{
    clear();
    moveTo(QPointF(50, 0));
    curveTo(QPointF(-20, 0), QPointF(-20, 50), QPointF(50, 50));
    curveTo(QPointF(120, 50), QPointF(120, 100), QPointF(50, 100));
    moveTo(QPointF(50, 100));
    lineTo(QPointF(50, 150));
    normalize();
}

void VariableLineShape::paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintContext)
{  
    applyConversion(painter, converter);
    QPainterPath path(outline());
    normalize();
    
    if (background()) {
        background()->paint(painter, converter, paintContext, path);
    }
}
/*
QPainterPath VariableLineShape::pathStroke(const QPen &pen) const
{
    return QPainterPath();
}  
*/
qreal VariableLineShape::widthPercentage() const
{
    return m_widthPercentage;
}

QString VariableLineShape::pathShapeId() const
{
    return VariableLineShapeId;
}