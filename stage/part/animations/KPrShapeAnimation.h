/* This file is part of the KDE project
 * Copyright (C) 2010 Thorsten Zachmann <zachmann@kde.org>
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

#ifndef KPRSHAPEANIMATION_H
#define KPRSHAPEANIMATION_H

#include <QParallelAnimationGroup>
#include "KPrAnimationData.h"
#include <QPair>

#include "stage_export.h"

class KoShape;
class KoTextBlockData;
class KoXmlElement;
class KoShapeLoadingContext;
class KoShapeSavingContext;
class KPrAnimationCache;
class KPrAnimationStep;
class KPrAnimationSubStep;

class STAGE_EXPORT KPrShapeAnimation : public QParallelAnimationGroup, KPrAnimationData
{
    Q_OBJECT
public:

    /// Node Type (Trigger Event of the animation)
    enum Node_Type {
        On_Click,
        After_Previous,
        With_Previous
    };

    /// Animation class
    enum Preset_Class {
        None,
        Entrance,
        Exit,
        Emphasis,
        Custom,
        Motion_Path,
        Ole_Action,
        Media_Call
    };

    /// State of the animation
    enum Animation_State {
        Valid,
        Invalid
    };

    KPrShapeAnimation(KoShape *shape, KoTextBlockData *textBlockData);
    virtual ~KPrShapeAnimation();

    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);
    virtual bool saveOdf(KoPASavingContext &paContext, bool startStep, bool startSubStep) const;

    KoShape * shape() const;
    KoTextBlockData * textBlockData() const;

    virtual void init(KPrAnimationCache *animationCache, int step);

    virtual void deactivate();
    /**
     * @return true if this shape animation change the visibility
     */
    bool visibilityChange();

    /**
     * @return true if this shape animation enable visibility of the shape
     */
    bool visible();

    /**
     * Read the value from the first KPrAnimationBase object
     */
    //QPair<KoShape *, KoTextBlockData *> animationShape() const;

    /**
      * Return the begin and end time of the animation as a QPair
      */
    QPair<int, int> timeRange();
    /**
      * Return global duration of the shape animation
      */
    int globalDuration();

    /**
     * @brief Set the begin time for the animation
     *
     * @param timeMS time in miliseconds
     */
    void setBeginTime(int timeMS);

    /**
     * @brief Set duration time for the animation
     *
     * @param timeMS time in miliseconds
     */
    void setGlobalDuration(int timeMS);

    /**
     * @brief asign text block data
     *
     * @param textBlockData
     */
    void setKoTextBlockData(KoTextBlockData *textBlockData);

    /// The following data are just for quick access
    /// to different params of the animation
    /// Use set methods to mantain them in sync with the
    /// real animation data
    /**
     * @brief Set the node type of the animation
     * used just like quick access, Node Type is actually
     * determined using the step, substep structure
     *
     * @param type Node Type
     */
    void setNodeType(Node_Type type);

    /**
     * @brief Set class of the animation
     *
     * @param presetClass
     */
    void setPresetClass(Preset_Class presetClass);

    /**
     * @brief Set the id (name) of the animation
     *
     * @param id QString with the animation name
     */
    void setId(QString id);

    void setPresetSubType(QString subType);

    /**
     * @brief Returns the stored node type for the animation
     *
     * @return Node_Type
     */
    Node_Type NodeType() const;

    /**
     * @brief Returns stored class of the animation
     *
     * @return Preset_Class
     */
    Preset_Class presetClass() const;

    /**
     * @brief Returns the id (name) of the animation
     *
     * @return animation id
     */
    QString id() const;

    /**
     * @brief Returns the class of the animation as String
     *
     * @return animation class text
     */
    QString presetClassText() const;

    QString presetSubType() const;

    /**
     * @brief Set a pointer to the parent step of the animation
     *
     * @param step pointer to the step that holds the substep of the
     * animation
     */
    void setStep(KPrAnimationStep *step);

    /**
     * @brief Set a pointer to the parent substep of the animation
     *
     * @param step pointer to the substep that holds the animation
     */
    void setSubStep(KPrAnimationSubStep *subStep);

    /// The following pointers are used to reduce the necessity to
    /// look for animation in all the step-substep structure
    /// But they must be maintain in sync manually
    /**
     * @brief Returns the stored pointer of the animation step
     *
     * @return animation step
     */
    KPrAnimationStep *step();

    /**
     * @brief Returns the stored pointer of the animation substep
     *
     * @return animation substep
     */
    KPrAnimationSubStep *subStep();

    /// Store index for undo redo commands
    /// The indexes are used and updated with in the remove/move commands
    /// during other times they could be ou of sync
    void setStepIndex(int index);
    void setSubStepIndex(int index);
    void setAnimIndex(int index);
    int stepIndex();
    int subStepIndex();
    int animIndex();

signals:
    /// Notify if an animation stored property has been changed
    void timeChanged(int begin, int end);
    void triggerEventChanged(KPrShapeAnimation::Node_Type event);


private:
    KoShape *m_shape;
    KoTextBlockData *m_textBlockData;
    Node_Type m_triggerEvent;
    Preset_Class m_class;
    QString m_id;
    QString m_presetSubType;
    KPrAnimationStep *m_step;
    KPrAnimationSubStep *m_subStep;
    int m_stepIndex;
    int m_subStepIndex;
    int m_animIndex;
};

#endif /* KPRSHAPEANIMATION_H */
