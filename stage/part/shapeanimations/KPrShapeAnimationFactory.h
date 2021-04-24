/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
