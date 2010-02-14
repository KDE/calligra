/* This file is part of the KDE project
 * Copyright ( C ) 2010 Casper Boemann <cbo@boemannn.dk>
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

#ifndef KPRANIMATIONCACHE_H
#define KPRANIMATIONCACHE_H

#include <QMap>
#include <QString>
#include <QVariant>

class KoShape;
class KoTextBlockData;

/**
 * This class represents a snapshot of the current state of animations.
 *
 * It relies on the Animation Framework to update it's state.
 *
 * It provides a central place for painters of text and shapes to get information
 * about any needed attributes (position, visibillity etc) of the animated objects
 * (shapes or paragraphs)
 **/
class KPrAnimationCache
{
public:
    KPrAnimationCache();

    virtual ~KPrAnimationCache();

    /**
     * Tells if a value has ever been set
     *
     * The purpose of this method is to help the AnimationFramework populate the
     * cache initially. Everyone else should just ask with @value for the value they want.
     *
     * @param shape The shape on which the value might be set
     * @param id The id of the value we are asking if is set
     * @return true if the value is already set.
     */
    bool hasValue(KoShape *shape, const QString &id);

    /**
     * Tells if a value has ever been set
     *
     * The purpose of this method is to help the AnimationFramework populate the
     * cache initially. Everyone else should just ask with @value for the value they want.
     *
     * @param textBlockData The KoTextBlockData which the value is set on
     * @param id The id of the value we are asking if is set
     * @return true if the value is already set.
     */
    bool hasValue(KoTextBlockData *textBlockData, const QString &id);

    /**
     * Sets a value, either initially or updating it
     *
     * The purpose of this method is to help the AnimationFramework set or update
     * values initially. Everyone else should just ask with @value for the value they want.
     *
     * @param shape The shape which the value is set on
     * @param id The id of the value
     * @param value The value is should have
     */
    void setValue(KoShape *shape, const QString &id, const QVariant &value);

    /**
     * Sets a value, either initially or updating it
     *
     * The purpose of this method is to help the AnimationFramework set or update
     * values initially. Everyone else should just ask with @value for the value they want.
     *
     * @param textBlockData The KoTextBlockData which the value is set on
     * @param id The id of the value
     * @param value The value is should have
     */
    void setValue(KoTextBlockData *textBlockData, const QString &id, const QVariant &value);

    /**
     * The value 
     *
     * Everyone can use this method to query a value.
     *
     * @param shape The shape on which the value might be set
     * @param id The id of the value we are asking if is set
     * @return the value as a QVariant
     */
    QVariant value(KoShape *shape, const QString &id, const QVariant &defaultValue);

    /**
     * The value
     *
     * Everyone can use this method to query a value.
     *
     * @param textBlockData The KoTextBlockData which the value is set on
     * @param id The id of the value we are asking if is set
     * @return the value as a QVariant
     */
    QVariant value(KoTextBlockData *textBlockData, const QString &id, const QVariant &defaultValue);

private:
    QMap<KoShape *, QMap<QString, QVariant> > m_shapeValues;
    QMap<KoTextBlockData *, QMap<QString, QVariant> > m_textBlockDataValues;
};


#endif /* KPRANIMATIONCACHE_H */
