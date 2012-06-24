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
public:

    enum Node_Type {
        On_Click,
        After_Previous,
        With_Previous
    };

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
    void setBeginTime(int timeMS);
    void setGlobalDuration(int timeMS);

    void setNodeType(Node_Type type);
    void setPresetClass(Preset_Class presetClass);
    void setId(QString id);
    Node_Type NodeType() const;
    Preset_Class presetClass() const;
    QString id() const;
    QString presetClassText() const;
    void setStep(KPrAnimationStep *step);
    void setSubStep(KPrAnimationSubStep *subStep);
    KPrAnimationStep *step();
    KPrAnimationSubStep *subStep();

    /// Store index for undo redo commands
    void setStepIndex(int index);
    void setSubStepIndex(int index);
    void setAnimIndex(int index);
    int stepIndex();
    int subStepIndex();
    int animIndex();


private:
    KoShape *m_shape;
    KoTextBlockData *m_textBlockData;
    Node_Type m_triggerEvent;
    Preset_Class m_class;
    QString m_id;
    KPrAnimationStep *m_step;
    KPrAnimationSubStep *m_subStep;
    int m_stepIndex;
    int m_subStepIndex;
    int m_animIndex;
};

#endif /* KPRSHAPEANIMATION_H */
