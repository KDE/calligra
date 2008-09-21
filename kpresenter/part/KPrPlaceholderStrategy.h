/* This file is part of the KDE project
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
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

#ifndef KPRPLACEHOLDERSTRATEGY_H
#define KPRPLACEHOLDERSTRATEGY_H

#include <QMap>

class QString;
class KoShape;
class KoDataCenter;
class KoShapeControllerBase;
class KoShapeSavingContext;

class KPrPlaceholderStrategy
{
public:
    /**
     * Factory method to create a KPrPlaceholderStrategy
     *
     * @param presentationClass The presentation:class attribute of the placeholder
     */
    static KPrPlaceholderStrategy * create( const QString & presentationClass );

    virtual ~KPrPlaceholderStrategy();

    virtual KoShape * createShape( const QMap<QString, KoDataCenter *> & dataCenterMap );
    void saveOdf( KoShapeSavingContext & context );

protected:
    /**
     * @param shapeId The id of the shape used for creating a shape of that type
     * @param xmlElement The xml element used in saveOdf to write out the content of the frame
     */
    KPrPlaceholderStrategy( const char * shapeId, const char * xmlElement );

    const char * m_shapeId;
    const char * m_xmlElement;
};

#endif /* KPRPLACEHOLDERSTRATEGY_H */
