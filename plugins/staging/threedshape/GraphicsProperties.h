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

#ifndef GRAPHICSPROPERTIES_H
#define GRAPHICSPROPERTIES_H

// Qt
#include <QColor>
//#include <QVector3D>
//#include <QString>
//#include <QList>

// Calligra
//#include <KoShape.h>

// Shape
//#include "Lightsource.h"

class KoXmlElement;
class KoXmlWriter;
class KoShapeSavingContext;

/**
 * GraphicsProperties hold the graphics properties of an ODF style.
 */
class GraphicsProperties
{
public:
    enum Attribute {
        AttributeFill               = 0x0001, 
        AttributeFillColor          = 0x0002,
        AttributeSecondaryFillColor = 0x0004,
        // Many many more here.
    };

    enum Fill {
        None,
        Solid
        // More here
    };
        

    GraphicsProperties();
    ~GraphicsProperties();

    /**
     * Load the style from the element
     *
     * @param style  the element containing the style to read from
     * @return true when border attributes were found
     */
    bool loadOdf(const KoXmlElement &properties);
    void saveOdf(KoXmlWriter &writer) const;

    // getters
    Fill   fill() const               { return m_fill; }
    QColor fillColor() const          { return m_fillColor; }
    QColor secondaryFillColor() const { return m_secondaryFillColor; }

private:
    qint32  m_attributes;

    Fill    m_fill;
    QColor  m_fillColor;
    QColor  m_secondaryFillColor;
};


#endif
