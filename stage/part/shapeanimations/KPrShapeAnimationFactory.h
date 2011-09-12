/* This file is part of the KDE project
   Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>

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
#ifndef KPRSHAPEANIMATIONFACTORY_H
#define KPRSHAPEANIMATIONFACTORY_H

#include "KPrShapeAnimationOld.h"

/**
 * Base class for shape animation factories
 */
class KPrShapeAnimationFactory
{
public:

    /**
     * Constructor
     *
     * @param id The id of the page effect the factory is creating
     * @param name The name of the effect. This name is used in the UI
     */
    KPrShapeAnimationFactory( const QString & id, const QString & name );

    virtual ~KPrShapeAnimationFactory();

    /**
     * Create a shape animation
     *
     * @param properties The properties for creating a page effect
     */
    virtual KPrShapeAnimationOld * createShapeAnimation() const = 0;

    /**
     * Get the id of the page effect
     */
    QString id() const;

    /**
     * Get the name of the page effect
     */
    QString name() const;

private:
    struct Private;
    Private * const d;
};

#endif /* KPRSHAPEANIMATIONFACTORY_H */
