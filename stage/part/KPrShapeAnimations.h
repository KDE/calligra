/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 * Copyright ( C ) 2010 Benjamin Port <port.benjamin@gmail.com>
 * Copyright ( C ) 2012 Paul Mendez <paulestebanms@gmail.com>
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

#ifndef KPRSHAPEANIMATIONS_H
#define KPRSHAPEANIMATIONS_H

#include <QList>
#include <QAbstractTableModel>
#include "animations/KPrShapeAnimation.h"
#include "animations/KPrAnimationStep.h"

#include "stage_export.h"

class KPrDocument;

/**
 * Model for Animations data of each KPrPage
 */
class STAGE_EXPORT KPrShapeAnimations : public QAbstractTableModel
{
    Q_OBJECT
public:

    /// Time to be updated
    enum TimeUpdated {
        BeginTime,
        DurationTime,
        BothTimes
    };

    /// column names
    enum ColumnNames {
        Group,
        StepCount,
        TriggerEvent,
        Name,
        ShapeThumbnail,
        AnimationIcon,
        StartTime,
        Duration,
        AnimationClass,
        NodeType
    };

    explicit KPrShapeAnimations(KPrDocument *document, QObject *parent = 0);
    ~KPrShapeAnimations() override;

    /// Model Methods
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index,
               int role=Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                     int role=Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent=QModelIndex()) const override;
    int columnCount(const QModelIndex &parent=QModelIndex()) const override;
    bool setHeaderData(int, Qt::Orientation, const QVariant&,
                       int=Qt::EditRole) override {return false;}
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role=Qt::EditRole) override;



    void init(const QList<KPrAnimationStep *> &animations);
    /**
     * Add animation to the animations
     *
     * It the animation for this shape/step already exists it is replaced by the given one
     *
     * @param animation the animation to insert
     */
    void add(KPrShapeAnimation *animation);

    /**
     * Remove  animation to the animations
     *
     * @param animation the animation to remove
     */
    void remove(KPrShapeAnimation *animation);

    /**
     * @brief Insert a new step on the steps list
     *
     * @param i position in which the step will be inserted
     * @param step step to be inserted
     */
    void insertStep(const int i, KPrAnimationStep *step);

    /**
     * @brief Remove a step on the steps list
     *
     * @param step step to be removed
     */
    void removeStep(KPrAnimationStep *step);

    /**
     * @brief Swap steps in positions i and j
     *
     * @param i position of the first step
     * @param j position of the second step
     */
    void swapSteps(int i, int j);

    void swapAnimations(KPrShapeAnimation *oldAnimation, KPrShapeAnimation *newAnimation);

    /**
     * @brief Replace old animation with new animation
     *
     * @param oldAnimation animation to be replaced
     * @param newAnimation
     */
    void replaceAnimation(KPrShapeAnimation *oldAnimation, KPrShapeAnimation *newAnimation);


    /**
     * Get the animations for the given step
     *
     * @param step the step for which the animations should be returned
     * @return A map of the shape -> animation if the animation is 0 the shape
     *         is not visible
     */
/*    QMap<KoShape *, KPrShapeAnimation *> animations( int step ) const;
*/
    /**
     * Get all steps
     */
    QList<KPrAnimationStep *> steps() const;

    /// Save a edit command
    void endTimeLineEdition();

    /**
     * @brief Set animation begin and duration
     *
     * @param item the animation
     * @param begin time in milliseconds
     * @param duration time in milliseconds
     */
    void setTimeRange(KPrShapeAnimation *item, const int begin, const int duration);

    /**
     * Get the end time for the given animation
     *
     * @param index of the animation
     * @return the time in milliseconds of the animation end
     */
    int animationEnd(const QModelIndex &index) const;

    /**
     * Get the scale begin time for the given animation
     *
     * @param index of the animation
     * @return the time in milliseconds where the scale begin (relative to the parent onclik animation)
     */
    int animationStart(const QModelIndex &index) const;

    /**
     * @brief Replace animation in the given index
     *
     * @param index index of the animation
     * @param newAnimation animation to be used to replace
     */
    QModelIndex replaceAnimation(const QModelIndex &index, KPrShapeAnimation *newAnimation);

    /**
     * @brief Create command to change trigger event of the animation on index
     *
     * @param index index of the animation
     * @param type new Node Type for the animation
     */
    bool setTriggerEvent(const QModelIndex &index, const KPrShapeAnimation::NodeType type);

    /**
     * @brief Change trigger event of the animation
     *
     * @param animation
     * @param type new Node Type for the animation
     */
    bool setNodeType(KPrShapeAnimation *animation, const KPrShapeAnimation::NodeType type);

    /**
     * @brief Redefine start of the animation if is moved below the minimum limit
     * of its animation trigger event scale.
     *
     * @param mIndex index of the animation
     */
    void recalculateStart(const QModelIndex &mIndex);

    /**
     * @brief Move animation up in the animation list
     * Redefine trigger event if it's necessary
     *
     * @param index of the animation
     */
    QModelIndex moveUp(const QModelIndex &index);

    /**
     * @brief Move animation down in the animation list
     * Redefine trigger event if it's necessary
     *
     * @param index of the animation
     */
    QModelIndex moveDown(const QModelIndex &index);

    /**
     * @brief Move animation from @p oldRow to @p newRow
     * Redefine trigger event if it's necessary
     *
     * @param oldRow the old index of the animation
     * @param newRow the new index of the animation
     */
    QModelIndex moveAnimation(int oldRow, int newRow);

    /**
     * @brief remove animation on index
     *
     * @param index of the animation to be removed
     */
    QModelIndex removeAnimationByIndex(const QModelIndex &index);

    /**
     * @brief Return the shape of the animation on given index
     *
     * @param index of the animation
     */
    KoShape *shapeByIndex(const QModelIndex &index) const;

    /// Return the first animation index for the given shape
    QModelIndex indexByShape(KoShape* shape) const;

    /**
     * @brief Set begin time for the animation on index
     *
     * @param index index of the animation
     * @param begin the begin time
     */
    void setBeginTime(const QModelIndex &index, const int begin);

    /**
     * @brief Set duration for the animation on index
     *
     * @param index index of the animation
     * @param duration duration of the animation
     */
    void setDuration(const QModelIndex &index, const int duration);

    KPrShapeAnimation *animationByRow(int row, int *pGroup = 0, KPrShapeAnimation::NodeType *pNodeType = 0) const;

    /**
     * @brief add new animation after index
     *
     * @param newAnimation animation to be inserted
     * @param previousAnimation index of the previous animation
     */
    void insertNewAnimation(KPrShapeAnimation *newAnimation, const QModelIndex &previousAnimation);

    QModelIndex indexByAnimation(KPrShapeAnimation *animation) const;

    void resyncStepsWithAnimations();

    KPrShapeAnimation::NodeType triggerEventByIndex(const QModelIndex &index);

public Q_SLOTS:
    /// Notify a external edition of begin or end time
    void notifyAnimationEdited();

    /// Notify an external edition of an animation
    void notifyAnimationChanged(KPrShapeAnimation *animation);

    /// Notify if an animation set as OnClick has changed of trigger event
    void notifyOnClickEventChanged();

Q_SIGNALS:
    void timeScaleModified();
    void onClickEventChanged();

private:

    void dump() const;
    QString getAnimationName(KPrShapeAnimation *animation, bool omitSubType = false) const;
    QPixmap getAnimationShapeThumbnail(KPrShapeAnimation *animation) const;
    QPixmap getAnimationIcon(KPrShapeAnimation *animation) const;
    QImage createThumbnail(KoShape *shape, const QSize &thumbSize) const;
    void setTimeRangeIncrementalChange(KPrShapeAnimation *item, const int begin,
                                       const int duration, TimeUpdated updatedTimes);
    QList<KPrShapeAnimation *> getWithPreviousSiblings(KPrShapeAnimation *animation) const;
    QList<KPrAnimationSubStep *> getSubSteps(int start, int end, KPrAnimationStep *step) const;
    bool createTriggerEventEditCmd(KPrShapeAnimation *animation, KPrShapeAnimation::NodeType oldType,
                                   KPrShapeAnimation::NodeType newType);

    QList<KPrAnimationStep *> m_shapeAnimations;
    KPrShapeAnimation *m_currentEditedAnimation;
    bool m_firstEdition;
    int m_oldBegin;
    int m_oldDuration;
    KPrDocument *m_document;
};

#endif /* KPRSHAPEANIMATIONS_H */
