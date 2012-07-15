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

class STAGE_EXPORT KPrShapeAnimations : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum TimeUpdated {
        BeginTime,
        DurationTime,
        BothTimes
    };

    enum ColumnNames {
        Group = 0,
        StepCount = 1,
        TriggerEvent = 2,
        Name = 3,
        ShapeThumbnail = 4,
        AnimationIcon = 5,
        StartTime = 6,
        Duration = 7,
        AnimationClass = 8,
        Node_Type = 9
    };

    explicit KPrShapeAnimations(QObject *parent = 0);
    ~KPrShapeAnimations();

    // Model Methods
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index,
               int role=Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                     int role=Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    bool setHeaderData(int, Qt::Orientation, const QVariant&,
                       int=Qt::EditRole) { return false; }
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role=Qt::EditRole);



    void init(const QList<KPrAnimationStep *> animations);
    /**
     * Add animation to the animations
     *
     * It the animation for this shape/step already exists it is replaced by the given one
     *
     * @parama animation the animation to insert
     */
    void add( KPrShapeAnimation * animation );

    /**
     * Remove  animation to the animations
     *
     * @parama animation the animation to remove
     */
    void remove( KPrShapeAnimation * animation );

    void insertStep(const int i, KPrAnimationStep *step);
    void removeStep(KPrAnimationStep *step);

    void swapSteps(int i, int j);

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

    /// Set time range for item (times in miliseconds)
    void setTimeRange(KPrShapeAnimation *item, const int begin, const int duration);

    /// requiere to send commands
    void setDocument(KPrDocument *document);

    /// Return previous item end time in seconds
    qreal previousItemEnd(const QModelIndex &index);

    /// Return previous item begin time
    qreal previousItemBegin(const QModelIndex &index);

    QModelIndex replaceAnimation(const QModelIndex &index, KPrShapeAnimation *newAnimation);
    bool setTriggerEvent(const QModelIndex &index, const KPrShapeAnimation::Node_Type type);
    void recalculateStart(const QModelIndex &mIndex);

    QModelIndex moveUp(const QModelIndex &index);
    QModelIndex moveDown(const QModelIndex &index);
    QModelIndex moveItem(int oldRow, int newRow);

    QModelIndex removeItemByIndex(const QModelIndex &index);
    KoShape *shapeByIndex(const QModelIndex &index);

    /// Return the first animation index for the given shape
    QModelIndex indexByShape(KoShape* shape);

    void setBeginTime(const QModelIndex &index, const int begin);
    void setDuration(const QModelIndex &index, const int duration);

    KPrShapeAnimation *animationByRow(const int row) const;

public slots:
    /// Notify a external edition of begin or end time
    void notifyAnimationEdited();
    void notifyOnClickEventChanged();

signals:
    void timeScaleModified();
    void onClickEventChanged();

private:
    KPrShapeAnimation *animationByRow(const int row, int &groupCount) const;
    QString getAnimationName(KPrShapeAnimation *animation) const;
    QPixmap getAnimationShapeThumbnail(KPrShapeAnimation *animation) const;
    QPixmap getAnimationIcon(KPrShapeAnimation *animation) const;
    QImage createThumbnail(KoShape* shape, const QSize &thumbSize) const;
    void setTimeRangeIncrementalChange(KPrShapeAnimation *item, const int begin, const int duration, TimeUpdated updatedTimes);
    QModelIndex indexByAnimation(KPrShapeAnimation *animation);
    QList<KPrShapeAnimation *> getWithPreviousSiblings(KPrShapeAnimation *animation, bool connectItems);
    QList<KPrAnimationSubStep *> getSubSteps(int start, int end, KPrAnimationStep *step);
    bool createTriggerEventEditCmd(KPrShapeAnimation *animation, KPrAnimationStep *newStep, KPrAnimationSubStep *newSubStep,
                                   KPrShapeAnimation::Node_Type newType, QList<KPrShapeAnimation *> children,
                                   QList<KPrAnimationSubStep *> movedSubSteps, KPrShapeAnimations *shapeAnimations);

    QList<KPrAnimationStep *> m_shapeAnimations;
    KPrShapeAnimation *m_currentEditedAnimation;
    bool m_firstEdition;
    int m_oldBegin;
    int m_oldDuration;
    KPrDocument *m_document;
};

#endif /* KPRSHAPEANIMATIONS_H */
