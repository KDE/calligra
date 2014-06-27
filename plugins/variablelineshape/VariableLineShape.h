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

#ifndef VARIABLELINESHAPE_H
#define VARIABLELINESHAPE_H

#include "KoPathShape.h"

#define VariableLineShapeId "VariableLineShape"

typedef QMap<KoPathPoint *, qreal> pointsWidth;

/**
 * The VariableLineShape is a shape that represents a variable line.
 * The variable line can have nodes to set width in percentage.
 */
class VariableLineShape : public KoPathShape
{
public:
    VariableLineShape();
    ~VariableLineShape();

    // Returns the percentage
    qreal widthPercentage() const;
    
    // Sets the percentage of current width
    void setWidthePercentage(qreal percentage);
    
    void initDefaultPointWidth();
    
    qreal getPointStokeWidth(KoPathPoint *pathPoint);
    
    // creates outline of variable width
    QPainterPath createVariableWidthOutline() const;
    
    // reimplemented
    virtual bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);

    // reimplemented
    virtual void saveOdf(KoShapeSavingContext &context) const;
    
    // reimplemented
    virtual QPainterPath pathStroke(const QPen &pen) const;

    // reimplemented
    virtual QString pathShapeId() const;

private:
    qreal m_widthPercentage;
    QString m_mimetype;
    pointsWidth m_pointsWidth;
};

#endif // VARIABLELINESHAPE_H
