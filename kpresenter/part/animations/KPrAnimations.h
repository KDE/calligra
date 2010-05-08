/* This file is part of the KDE project
 * Copyright (C) 2010 Thorsten Zachmann <zachmann@kde.org>
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

#ifndef KPRANIMATIONS_H
#define KPRANIMATIONS_H

class KPrAnimations
{
public:
    KPrAnimations();
    ~KPrAnimations();

    int steps();
    KPrShapeAnimation * take( int pos );
    int pos( KPrShapeAnimation * animation );

    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);
    void saveOdf(KoShapeSavingContext &context) const;

#if REMOVE
    /**
     * Add the animation to the current shape animation
     * if this fails create a new KPrShapeAnimation and put it in there
     * the new animation needs to be a with previous
     * should also write out a warning
     */
    void add( KPrAnimationBase * animation );

    QMap<QPair<KoShape *, KoTextBlockData *>, KPrShapeAnimation *> m_current;
#endif

private:
    QList<KPrShapeAnimation *> m_animations;
};

#endif /* KPRANIMATIONS_H */
