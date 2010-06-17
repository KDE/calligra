/* This file is part of the KDE project
 * Copyright ( C ) 2010 Casper Boemann <cbo@boemannn.dk>
 * Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>
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
#include <QList>

class KoShape;
class KoTextBlockData;

/**
 * This class represents a snapshot of the current state of animations.
 *
 * It relies on the Animation Framework to update it's state.
 *
 * It provides a central place for painters of text and shapes to get information
 * about any needed attributes (position, visibility etc) of the animated objects
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
    void setValue(int step, KoShape *shape, const QString &id, const QVariant &value);

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
    void setValue(int step, KoTextBlockData *textBlockData, const QString &id, const QVariant &value);

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

    // 1. Init cache with values that take effect even before the animation is started
    //    This information should be kept in the first stack entry
    //    I think this is only needed for visibility as the other take only effect in the presentation step
    // 2. For the animation on 1. step
    //    copy falues from previous step that are still there.
    //    update values with the values from the animations of that current step
    //    when all is finished start with the next step
    //    do the same for each time the effect is triggert until all effects in this step are finished.
    // go on with step 2 until there are no more steps left

    // start a new step
    // copy over all what is at the end of the last step

    // if we go backward in a presentation we need to setup all steps
    // if we go forward we can do it while we are at it but maybe it would be better to initilialize also
    // everything right away.

    // initialize step with values that will be there at the end of the step
    // step 0 is the value the object has before any animation is started
    // step n is the value of the object after the animation, only needed when there is a change to the real value of the object
    // e.g. the object has been moved to its original position, one the animation is done the value is removed
    void init(int step, KoShape *shape, const QString &id, const QVariant &value);

    // update step value by values
    // will do different things depending on type of QVariant
    // e.g. for QMatrix it will add the value to the matrix
    // the default action when there is no special handling is to copy the value over
    // if QVariant is emty it will remove the id from the step
    // the step in update must match the step of startStep
    // this will update the values used for the animation.
    // maybe have an internal method to also use it for updating the stack while init
    void update(KoShape *shape, const QString &id, const QVariant &value);

    // trigger copying of data from the last step to the current one
    // these values will be updated by update.
    // the step in update must match the step of startStep
    void startStep(int step);
    void endStep(int step);
    void next();
    QVariant value(int step, KoShape *shape, const QString &id, const QVariant &defaultValue);
    QVariant value(int step, KoShape *shape, const QString &id);

    bool hasValue(int step, KoShape *shape, const QString &id);

    // ending and animation will just activate the values of the step
private:
    QList<QMap<KoShape *, QMap<QString, QVariant> > > m_shapeValuesStack;
    QList<QMap<KoTextBlockData *, QMap<QString, QVariant> > > m_textBlockDataValuesStack;
    QMap<KoShape *, QMap<QString, QVariant> > m_currentShapeValues;
    QMap<KoTextBlockData *, QMap<QString, QVariant> > m_currentTextBlockDataValues;
    int m_step;
    bool m_next;
};


#endif /* KPRANIMATIONCACHE_H */
