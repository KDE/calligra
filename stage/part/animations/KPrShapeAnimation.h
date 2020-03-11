/* This file is part of the KDE project
 * Copyright (C) 2010 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>
 * Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>
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
class QTextBlockUserData;
class KoXmlElement;
class KoShapeLoadingContext;
class KoPASavingContext;
class KPrAnimationCache;
class KPrAnimationStep;
class KPrAnimationSubStep;

class STAGE_EXPORT KPrShapeAnimation : public QParallelAnimationGroup, KPrAnimationData
{
    Q_OBJECT
public:

    /// Node Type (Trigger Event of the animation)
    enum NodeType {
        OnClick,
        AfterPrevious,
        WithPrevious
    };

    /// Animation class
    enum PresetClass {
        None,
        Entrance,
        Exit,
        Emphasis,
        Custom,
        MotionPath,
        OleAction,
        MediaCall
    };

    KPrShapeAnimation(KoShape *shape, QTextBlockUserData *textBlockData);
    ~KPrShapeAnimation() override;

    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);
    virtual bool saveOdf(KoPASavingContext &paContext, bool startStep, bool startSubStep) const;

    KoShape *shape() const;
    QTextBlockUserData *textBlockUserData() const;

    // Do not mark this 'override', sub-classing will fail
    virtual void init(KPrAnimationCache *animationCache, int step); //NOLINT

    virtual void deactivate();

    /**
     * Read the value from the first KPrAnimationBase object
     */
    //QPair<KoShape *, KoTextBlockData *> animationShape() const;

    /**
      * Return the begin and end time of the animation as a QPair
      */
    virtual QPair<int, int> timeRange() const;
    /**
      * Return global duration of the shape animation
      */
    virtual int globalDuration() const;

    /**
     * @brief Set the begin time for the animation
     *
     * @param timeMS time in milliseconds
     */
    virtual void setBeginTime(int timeMS);

    /**
     * @brief Set duration time for the animation
     *
     * @param timeMS time in milliseconds
     */
    virtual void setGlobalDuration(int timeMS);

    /**
     * @brief assign text block data
     *
     * @param textBlockUserData
     */
    void setTextBlockUserData(QTextBlockUserData *textBlockUserData);

    /// The following data are just for quick access
    /// to different params of the animation
    /// Use set methods to maintain them in sync with the
    /// real animation data

    /**
     * @brief Set class of the animation
     *
     * @param presetClass
     */
    void setPresetClass(PresetClass presetClass);

    /**
     * @brief Set the id (name) of the animation
     *
     * @param id QString with the animation name
     */
    void setId(const QString &id);

    void setPresetSubType(const QString &subType);

    /**
     * @brief Returns stored class of the animation
     *
     * @return Preset_Class
     */
    PresetClass presetClass() const;

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
     * @param subStep pointer to the substep that holds the animation
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
    KPrAnimationStep *step() const;

    /**
     * @brief Returns the stored pointer of the animation substep
     *
     * @return animation substep
     */
    KPrAnimationSubStep *subStep() const;

    /// Store index for undo redo commands
    /// The indexes are used and updated with in the remove/move commands
    /// during other times they could be ou of sync
    void setStepIndex(int index);
    void setSubStepIndex(int index);
    void setAnimIndex(int index);
    int stepIndex() const;
    int subStepIndex() const;
    int animIndex() const;

Q_SIGNALS:
    /// Notify if an animation stored property has been changed
    void timeChanged(int begin, int end);

private:
    KoShape *m_shape;
    QTextBlockUserData *m_textBlockData;
    PresetClass m_class;
    QString m_id;
    QString m_presetSubType;
    KPrAnimationStep *m_step;
    KPrAnimationSubStep *m_subStep;
    int m_stepIndex;
    int m_subStepIndex;
    int m_animIndex;
};

#endif /* KPRSHAPEANIMATION_H */
