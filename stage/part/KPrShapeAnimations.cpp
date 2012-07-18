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

//Qt Headers
#include <QList>
#include <QSet>
#include <QPainter>

//Stage Headers
#include "KPrPage.h"
#include "KPrView.h"
#include "KPrDocument.h"
#include "animations/KPrAnimationSubStep.h"
#include "commands/KPrAnimationRemoveCommand.h"
#include "commands/KPrReorderAnimationCommand.h"
#include <commands/KPrEditAnimationTimeLineCommand.h>
#include <commands/KPrAnimationEditNodeTypeCommand.h>
#include <commands/KPrReplaceAnimationCommand.h>

//Calligra Headers
#include <KoShape.h>
#include <KoShapeManager.h>
#include <KoPADocument.h>
#include <KoShapePainter.h>
#include <KoShapeContainer.h>

//KDE Headers
#include <KIcon>
#include <KIconLoader>
#include <KLocale>
#include <kdebug.h>

const int COLUMN_COUNT = 10;
const int INVALID = -1;

KPrShapeAnimations::KPrShapeAnimations(QObject *parent)
    :QAbstractTableModel(parent)
    , m_currentEditedAnimation(0)
    , m_firstEdition(true)
    , m_oldBegin(INVALID)
    , m_oldDuration(INVALID)
    , m_document(0)
{
}

KPrShapeAnimations::~KPrShapeAnimations()
{
}

Qt::ItemFlags KPrShapeAnimations::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    if (index.isValid()) {
        theFlags |= Qt::ItemIsSelectable|Qt::ItemIsEnabled;
        //if (index.column() == Name)
            //theFlags |= Qt::ItemIsEditable;//|
                        //Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled;
    }
    return theFlags;
}

QVariant KPrShapeAnimations::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.column() < 0 ||
            index.column() >= COLUMN_COUNT || index.row() < 0
            || index.row() >= rowCount(QModelIndex())) {
        return QVariant();
    }

    // Read Data
    int groupCount = -1;
    int thisRow = index.row();
    KPrShapeAnimation *thisAnimation = animationByRow(thisRow, groupCount);
    if (!thisAnimation) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
            case Group: return groupCount;
            case StepCount:
                if (thisAnimation->NodeType() == KPrShapeAnimation::On_Click) {
                    return groupCount;
                }
                else {
                    return QVariant();
                }
            case TriggerEvent: return QVariant();
            case Name: return getAnimationName(thisAnimation);
            case ShapeThumbnail: return QVariant();
            case AnimationIcon: return QVariant();
            case StartTime: return thisAnimation->timeRange().first;
            case Duration: return thisAnimation->globalDuration();
            case AnimationClass: return thisAnimation->presetClass();
            case Node_Type: return thisAnimation->NodeType();
            default: Q_ASSERT(false);
        }
    }
    if (role == Qt::TextAlignmentRole) {
        if (index.column() == Name)
            return static_cast<int>(Qt::AlignVCenter|
                                    Qt::AlignLeft);
        return static_cast<int>(Qt::AlignVCenter|Qt::AlignRight);
    }
    if (role == Qt::DecorationRole) {
        switch (index.column()) {
            case Group: return QVariant();
            case StepCount: return QVariant();
            case TriggerEvent:
                if (thisAnimation->NodeType() == KPrShapeAnimation::On_Click)
                    return KIcon("onclick").pixmap(KIconLoader::SizeSmall,
                                                   KIconLoader::SizeSmall);
                if (thisAnimation->NodeType() == KPrShapeAnimation::After_Previous)
                    return KIcon("after_previous").pixmap(KIconLoader::SizeSmall,
                                                          KIconLoader::SizeSmall);
                if (thisAnimation->NodeType() == KPrShapeAnimation::With_Previous)
                    return KIcon("with_previous").pixmap(KIconLoader::SizeSmall,
                                                         KIconLoader::SizeSmall);
            case Name: return QVariant();
            case ShapeThumbnail: return getAnimationShapeThumbnail(thisAnimation);
            case AnimationIcon:  return getAnimationIcon(thisAnimation);
            case StartTime: return QVariant();
            case Duration: return QVariant();
            case AnimationClass: return QVariant();
            case Node_Type: return QVariant();
            default: Q_ASSERT(false);
        }
    }
    if (role == Qt::SizeHintRole) {
        switch (index.column()) {
            case Group:
            case StepCount: return QVariant();
            case TriggerEvent: return QSize(KIconLoader::SizeSmall, KIconLoader::SizeSmall);
            case Name: return QVariant();
            case ShapeThumbnail: return QSize(KIconLoader::SizeMedium, KIconLoader::SizeMedium);
            case AnimationIcon:
            case StartTime:
            case Duration:
            case AnimationClass: return QVariant();
            case Node_Type: return QVariant();
            default: Q_ASSERT(false);
        }
    }
    if (role == Qt::ToolTipRole) {
            switch (index.column()) {
            case Group:
            case StepCount: return QVariant();
            case TriggerEvent:
                if (thisAnimation->NodeType() == KPrShapeAnimation::On_Click)
                    return i18n("start on mouse click");
                if (thisAnimation->NodeType() == KPrShapeAnimation::After_Previous)
                    return i18n("start after previous animation");
                if (thisAnimation->NodeType() == KPrShapeAnimation::With_Previous)
                    return i18n("start with previous animation");
            case Name: return QVariant();
            case ShapeThumbnail: return thisAnimation->shape()->name();
            case AnimationIcon: return getAnimationName(thisAnimation);
            case StartTime: return i18n("Start after %1 seconds. Duration of %2 seconds").
                                    arg(thisAnimation->timeRange().first / 1000).
                                    arg(thisAnimation->globalDuration() / 1000);
            case Duration: return QVariant();
            case AnimationClass: return thisAnimation->presetClassText();
            case Node_Type: return QVariant();
            default: Q_ASSERT(false);
            }
        }
    return QVariant();
}

QVariant KPrShapeAnimations::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == Name) {
            return i18n("Animation");
        }
        else if (section == TriggerEvent) {
            return QString();
        }
        else if (section == ShapeThumbnail) {
            return i18n("Shape");
        }
    }
    return QVariant();
}

int KPrShapeAnimations::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    int rowCount = 0;
    foreach (KPrAnimationStep *step, m_shapeAnimations) {
        for (int i=0; i < step->animationCount(); i++) {
            QAbstractAnimation *animation = step->animationAt(i);
            if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(animation)) {
                for (int j=0; j < a->animationCount(); j++) {
                    QAbstractAnimation *shapeAnimation = a->animationAt(j);
                    if (KPrShapeAnimation *b = dynamic_cast<KPrShapeAnimation*>(shapeAnimation)) {
                        if ((b->presetClass() != KPrShapeAnimation::None) && (b->shape())) {
                            rowCount++;
                        }
                    }
                }
            }
        }
    }
    return rowCount;

}

int KPrShapeAnimations::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : COLUMN_COUNT;
}

bool KPrShapeAnimations::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.column() < 0 ||
            (index.column() > columnCount(QModelIndex()))) {
        return false;
    }
    // Read Data
    int groupCount = -1;
    KPrShapeAnimation *thisAnimation = animationByRow(index.row(), groupCount);
    if (!thisAnimation) {
        return false;
    }
    if (role == Qt::EditRole) {
        switch (index.column()) {
            case Group:
            case StepCount:
            case TriggerEvent:
            case Name:
            case ShapeThumbnail:
                return false;
            case AnimationIcon:
                return false;
            case StartTime:
                setTimeRangeIncrementalChange(thisAnimation, value.toInt(), thisAnimation->globalDuration(), BeginTime);
                emit dataChanged(index, index);
                return true;
            case Duration:
                setTimeRangeIncrementalChange(thisAnimation, thisAnimation->timeRange().first, value.toInt(), DurationTime);
                emit dataChanged(index, index);
                return true;
            case AnimationClass:
                return false;
            default:
                return false;

        }
    }
    return false;
}

void KPrShapeAnimations::init(const QList<KPrAnimationStep *> animations)
{
    m_shapeAnimations = animations;
}

void KPrShapeAnimations::add(KPrShapeAnimation * animation)
{
    if (!steps().contains(animation->step())) {
        if ((animation->stepIndex() >= 0) && (animation->stepIndex() <= steps().count())) {
            steps().insert(animation->stepIndex(), animation->step());
        }
        else {
            steps().append(animation->step());
        }
    }
    if (!(animation->step()->indexOfAnimation(animation->subStep()) > 0)) {
        if ((animation->subStepIndex() >= 0) &&
                (animation->subStepIndex() <= animation->step()->animationCount())) {
            animation->step()->insertAnimation(animation->subStepIndex(), animation->subStep());
        }
        else {
            animation->step()->addAnimation(animation);
        }
    }
    if ((animation->animIndex() >= 0) &&
            (animation->animIndex() <= animation->subStep()->animationCount())) {
        animation->subStep()->insertAnimation(animation->animIndex(), animation);
    }
    else {
        animation->subStep()->addAnimation(animation);
    }

    //updateModel
    QModelIndex index = indexByAnimation(animation);
    beginInsertRows(QModelIndex(), index.row(), index.row());
    endInsertRows();
    return;
}

void KPrShapeAnimations::remove(KPrShapeAnimation *animation)
{
    //updateModel
    QModelIndex index = indexByAnimation(animation);
    beginRemoveRows(QModelIndex(), index.row(), index.row());

    KPrAnimationStep *step = animation->step();
    KPrAnimationSubStep *subStep = animation->subStep();
    if (subStep->animationCount() <= 1) {
        if (step->animationCount() <= 1) {
            animation->setStepIndex(steps().indexOf(step));
            steps().removeAll(step);
        }
        animation->setSubStepIndex(step->indexOfAnimation(subStep));
        step->removeAnimation(subStep);
    }
    animation->setAnimIndex(subStep->indexOfAnimation(animation));
    subStep->removeAnimation(animation);
    endRemoveRows();
}

void KPrShapeAnimations::insertStep(const int i, KPrAnimationStep *step)
{
    if (step) {
        m_shapeAnimations.insert(i, step);
    }
}

void KPrShapeAnimations::removeStep(KPrAnimationStep *step)
{
    if (step) {
        m_shapeAnimations.removeAll(step);
    }
}

void KPrShapeAnimations::swapSteps(int i, int j)
{
    m_shapeAnimations.swap(i, j);
    emit dataChanged(this->index(i,0), this->index(i, COLUMN_COUNT));
    emit dataChanged(this->index(j,0), this->index(j, COLUMN_COUNT));
}

void KPrShapeAnimations::replaceAnimation(KPrShapeAnimation *oldAnimation, KPrShapeAnimation *newAnimation)
{
    KPrAnimationSubStep *subStep = oldAnimation->subStep();
    int currentAnimationIndex = subStep->indexOfAnimation(oldAnimation);
    newAnimation->setStep(oldAnimation->step());
    newAnimation->setSubStep(oldAnimation->subStep());
    newAnimation->setNodeType(oldAnimation->NodeType());
    newAnimation->setKoTextBlockData(oldAnimation->textBlockData());
    subStep->insertAnimation(currentAnimationIndex, newAnimation);
    subStep->removeAnimation(oldAnimation);
    QModelIndex indexModified = indexByAnimation(newAnimation);
    emit dataChanged(this->index(indexModified.row(), 0), this->index(indexModified.row(), COLUMN_COUNT));
}

QList<KPrAnimationStep *> KPrShapeAnimations::steps() const
{
    return m_shapeAnimations;
}

void KPrShapeAnimations::endTimeLineEdition()
{
    if (!m_firstEdition && m_currentEditedAnimation && (m_oldBegin != INVALID) && (m_oldDuration != INVALID)) {
        int begin = m_currentEditedAnimation->timeRange().first;
        int duration = m_currentEditedAnimation->globalDuration();
        if ((begin != m_oldBegin) || (duration != m_oldDuration)) {
            m_currentEditedAnimation->setBeginTime(m_oldBegin);
            m_currentEditedAnimation->setGlobalDuration(m_oldDuration);
            setTimeRange(m_currentEditedAnimation, begin, duration);
            emit timeScaleModified();
        }
        m_oldBegin = INVALID;
        m_oldDuration = INVALID;
    }
    m_firstEdition = true;
    m_currentEditedAnimation = 0;
}

void KPrShapeAnimations::setTimeRange(KPrShapeAnimation *item, const int begin, const int duration)
{
    if (item && m_document) {
        KPrEditAnimationTimeLineCommand *command = new KPrEditAnimationTimeLineCommand(item,
                                                                                     begin, duration);
        m_document->addCommand(command);
        connect(item, SIGNAL(timeChanged(int,int)), this, SLOT(notifyAnimationEdited()));
    }
}

void KPrShapeAnimations::setDocument(KPrDocument *document)
{
    m_document = document;
    reset();
}

qreal KPrShapeAnimations::previousItemEnd(const QModelIndex &index)
{
    if (index.isValid()) {
        KPrShapeAnimation *previousAnimation = animationByRow(index.row());
        if (previousAnimation->NodeType() == KPrShapeAnimation::On_Click) {
            return previousAnimation->timeRange().second;
        }
        if (previousAnimation->NodeType() == KPrShapeAnimation::With_Previous) {
            return previousAnimation->timeRange().second +
                    previousItemBegin(this->index(index.row() - 1, index.column(), QModelIndex()));
        }
        else if (previousAnimation->NodeType() == KPrShapeAnimation::After_Previous) {
            return previousAnimation->timeRange().second +
                    previousItemEnd(this->index(index.row() - 1, index.column(), QModelIndex()));
        }
    }
    return 0.0;
}

qreal KPrShapeAnimations::previousItemBegin(const QModelIndex &index)
{
    if (index.isValid()) {
        KPrShapeAnimation *previousAnimation = animationByRow(index.row());
        if (previousAnimation->NodeType() == KPrShapeAnimation::On_Click) {
            return previousAnimation->timeRange().first;
        }
        if (previousAnimation->NodeType() == KPrShapeAnimation::With_Previous) {
            return previousItemBegin(this->index(index.row() - 1, index.column(), QModelIndex()));
        }
        else if (previousAnimation->NodeType() == KPrShapeAnimation::After_Previous) {
            return previousItemEnd(this->index(index.row() - 1, index.column(), QModelIndex()));
        }
    }
    return 0.0;
}

QModelIndex KPrShapeAnimations::replaceAnimation(const QModelIndex &index, KPrShapeAnimation *newAnimation)
{
    if (!index.isValid() || !m_document) {
        return QModelIndex();
    }
    KPrShapeAnimation *oldAnimation = animationByRow(index.row());
    Q_ASSERT(oldAnimation);
    KPrReplaceAnimationCommand *cmd = new KPrReplaceAnimationCommand(m_document, oldAnimation, newAnimation);
    m_document->addCommand(cmd);
    return index;
}

bool KPrShapeAnimations::setTriggerEvent(const QModelIndex &index, const KPrShapeAnimation::Node_Type type)
{
    KPrShapeAnimation *item = animationByRow(index.row());
    if (item) {
        QList<KPrShapeAnimation *> movedChildren = QList<KPrShapeAnimation *>();
        QList<KPrAnimationSubStep *>movedSubSteps = QList<KPrAnimationSubStep *>();
        if (item->NodeType() == KPrShapeAnimation::After_Previous) {
            // After Previous to With Previous
            if (type == KPrShapeAnimation::With_Previous) {
                //use previous animation to reparent current animation
                Q_ASSERT(index.row() > 0);
                KPrShapeAnimation * previousAnimation = animationByRow(index.row() - 1);
                KPrAnimationSubStep *newSubStep = previousAnimation->subStep();
                movedChildren = getWithPreviousSiblings(item, true);
                return createTriggerEventEditCmd(item, item->step(),
                                                 newSubStep, type, movedChildren, movedSubSteps, this);
            }

            // After Previous to On Click
            if (type == KPrShapeAnimation::On_Click) {
                 // Get index of current substep
                 int currentSubStepIndex = item->step()->indexOfAnimation(item->subStep());
                 int subStepCount = item->step()->animationCount();

                 //Create new step to reparent currrent item and all following items.
                 KPrAnimationStep *newStep = new KPrAnimationStep();

                 // Add step after original one
                 int currentStepIndex = steps().indexOf(item->step());
                 insertStep(currentStepIndex + 1, newStep);

                 //reparent children
                 connect(item, SIGNAL(triggerEventChanged(KPrShapeAnimation::Node_Type)), this, SLOT(notifyAnimationEdited()));
                 if (currentSubStepIndex < subStepCount-1) {
                     movedSubSteps = getSubSteps(currentSubStepIndex + 1, subStepCount, item->step());
                 }
                 return createTriggerEventEditCmd(item, newStep, item->subStep(),
                                                  type, movedChildren, movedSubSteps, this);
            }
        }
        if (item->NodeType() == KPrShapeAnimation::With_Previous) {
           // With Previous to After Previous
           if (type == KPrShapeAnimation::After_Previous) {
               //int childrenCount = parent->childCount();
               // Get index of current substep
               int currentSubStepIndex = item->step()->indexOfAnimation(item->subStep());
               //Create new substep to reparent currrent item and all following items.
               KPrAnimationSubStep *newSubStep = new KPrAnimationSubStep();

               // Add substep after original one
               item->step()->insertAnimation(currentSubStepIndex + 1, newSubStep);

               //reparent children
               connect(item, SIGNAL(triggerEventChanged(KPrShapeAnimation::Node_Type)), this, SLOT(notifyAnimationEdited()));
               movedChildren = getWithPreviousSiblings(item, true);
               return createTriggerEventEditCmd(item, item->step(),
                                                newSubStep, type, movedChildren, movedSubSteps, this);
           }
           // With Previous to On Click
           if (type == KPrShapeAnimation::On_Click) {
                //int childrenCount = parent->childCount();
                // Get index of current substep
                int currentSubStepIndex = item->step()->indexOfAnimation(item->subStep());
                int subStepCount = item->step()->animationCount();

                //Create new step to reparent currrent item and all following items.
                KPrAnimationStep *newStep = new KPrAnimationStep();

                //Create new substep to reparent currrent item and all following items.
                KPrAnimationSubStep *newSubStep = new KPrAnimationSubStep();

                // Add step after original one
                //insert new Step
                int currentStepIndex = steps().indexOf(item->step());
                insertStep(currentStepIndex + 1, newStep);

                //reparent children
                connect(item, SIGNAL(triggerEventChanged(KPrShapeAnimation::Node_Type)), this, SLOT(notifyAnimationEdited()));
                if (currentSubStepIndex < subStepCount - 1) {
                    movedSubSteps = getSubSteps(currentSubStepIndex + 1, subStepCount, item->step());
                }
                movedChildren = getWithPreviousSiblings(item, false);
                return createTriggerEventEditCmd(item, newStep, newSubStep,
                                                 type, movedChildren, movedSubSteps, this);

           }
        }
        if (item->NodeType() == KPrShapeAnimation::On_Click) {
             if (index.row() < 1) {
                 // Resync trigger event edit widget
                 emit layoutChanged();
                 return false;
             }
            // On click to With Previous
            if (type == KPrShapeAnimation::With_Previous) {
                // Get previous animation
                KPrShapeAnimation *previousAnimation = animationByRow(index.row() - 1);
                KPrAnimationStep *newStep = previousAnimation->step();
                KPrAnimationSubStep *newSubStep = previousAnimation->subStep();

                movedChildren = getWithPreviousSiblings(item, false);

                int subStepCount = item->step()->animationCount();
                int currentSubStepIndex = item->step()->indexOfAnimation(item->subStep());
                if (subStepCount > 1) {
                    movedSubSteps = getSubSteps(currentSubStepIndex + 1, subStepCount, item->step());
                }
                connect(item, SIGNAL(triggerEventChanged(KPrShapeAnimation::Node_Type)), this, SLOT(notifyAnimationEdited()));
                return createTriggerEventEditCmd(item, newStep, newSubStep,
                                                 type, movedChildren, movedSubSteps, this);
            }

            // On click to After Previous
            if (type == KPrShapeAnimation::After_Previous) {
                 // Get previous animation
                 KPrShapeAnimation *previousAnimation = animationByRow(index.row() - 1);
                 KPrAnimationStep *newStep = previousAnimation->step();

                 int subStepCount = item->step()->animationCount();
                 if (subStepCount > 1) {
                     movedSubSteps = getSubSteps(1, subStepCount, item->step());
                 }
                 connect(item, SIGNAL(triggerEventChanged(KPrShapeAnimation::Node_Type)), this, SLOT(notifyAnimationEdited()));
                 return createTriggerEventEditCmd(item, newStep, item->subStep(),
                                                  type, movedChildren, movedSubSteps, this);
            }
        }
    }
    return false;
}

void KPrShapeAnimations::recalculateStart(const QModelIndex &mIndex)
{
    if (!mIndex.isValid()) {
        return;
    }
    if (mIndex.row() < 1) {
        return;
    }
    KPrShapeAnimation *animation = animationByRow(mIndex.row());

    KPrShapeAnimation::Node_Type type = animation->NodeType();
    if (type == KPrShapeAnimation::After_Previous) {
        setTimeRange(animation, previousItemEnd(mIndex), animation->globalDuration());
        setTriggerEvent(mIndex, KPrShapeAnimation::With_Previous);
    }
    else if (type == KPrShapeAnimation::With_Previous) {
        recalculateStart(index(mIndex.row() - 1, 0));
    }
}

QModelIndex KPrShapeAnimations::moveUp(const QModelIndex &index)
{
    if (!index.isValid() || index.row() < 1) {
        return QModelIndex();
    }
    KPrShapeAnimation *animationOld = animationByRow(index.row());
    if (animationOld->NodeType() == KPrShapeAnimation::On_Click) {
        if (steps().indexOf(animationOld->step()) < 1) {
            return QModelIndex();
        }
        int oldRow = -1;
        for (int i = index.row() - 1; i >= 0; i--) {
            if (animationByRow(i)->NodeType() == KPrShapeAnimation::On_Click) {
                oldRow = i;
                break;
            }
        }
        return moveItem(index.row(), oldRow);
    }
    return QModelIndex();
}

QModelIndex KPrShapeAnimations::moveDown(const QModelIndex &index)
{
    if (!index.isValid() || (index.row() >= (rowCount() - 1))) {
        return QModelIndex();
    }
    KPrShapeAnimation *animationOld = animationByRow(index.row());
    if (animationOld->NodeType() == KPrShapeAnimation::On_Click) {
        if (steps().indexOf(animationOld->step()) > (steps().count() - 1)) {
            return QModelIndex();
        }
        int oldRow = -1;
        for (int i = index.row() + animationOld->subStep()->animationCount(); i < rowCount(); i++) {
            if (animationByRow(i)->NodeType() == KPrShapeAnimation::On_Click) {
                oldRow = i;
                break;
            }
        }
        return moveItem(index.row(), oldRow);
    }
    return QModelIndex();
}

QModelIndex KPrShapeAnimations::moveItem(int oldRow, int newRow)
{
    Q_ASSERT(0 <= oldRow && oldRow < rowCount() &&
             0 <= newRow && newRow < rowCount());
    QModelIndex newIndex;
    // swap top level items
    KPrShapeAnimation *animationOld = animationByRow(oldRow);
    Q_ASSERT(animationOld);
    if (animationOld->NodeType() == KPrShapeAnimation::On_Click) {
        KPrShapeAnimation *animationNew = animationByRow(newRow);
        if (animationOld && animationNew) {
            if (m_document) {
                KPrReorderAnimationCommand *cmd = new KPrReorderAnimationCommand(this, animationOld->step(), animationNew->step());
                m_document->addCommand(cmd);
            }
        }
    }
    return newIndex;
}

QModelIndex KPrShapeAnimations::removeItemByIndex(const QModelIndex &index)
{
    if (!index.isValid()) {
        return index;
    }
    KPrShapeAnimation *animation = animationByRow(index.row());
    Q_ASSERT(animation);

    if (animation) {
        beginRemoveRows(index.parent(), index.row(), index.row());
        Q_ASSERT(m_document);
        KPrAnimationRemoveCommand *command = new KPrAnimationRemoveCommand(m_document, animation);
        m_document->addCommand(command);
        endRemoveRows();
    }
    return QModelIndex();
}

KoShape *KPrShapeAnimations::shapeByIndex(const QModelIndex &index)
{
    if (index.isValid()) {
        KPrShapeAnimation *animation = animationByRow(index.row());
        return animation->shape();
    }
    return 0;
}

QModelIndex KPrShapeAnimations::indexByShape(KoShape *shape)
{
    int rowCount = 0;
    foreach (KPrAnimationStep *step, m_shapeAnimations) {
        for (int i=0; i < step->animationCount(); i++) {
            QAbstractAnimation *animation = step->animationAt(i);
            if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(animation)) {
                for (int j=0; j < a->animationCount(); j++) {
                    QAbstractAnimation *shapeAnimation = a->animationAt(j);
                    if (KPrShapeAnimation *b = dynamic_cast<KPrShapeAnimation*>(shapeAnimation)) {
                        if ((b->presetClass() != KPrShapeAnimation::None) && (b->shape())) {
                            if(b->shape() == shape)
                                return this->index(rowCount, 0);
                        }
                        rowCount++;
                    }
                }
            }
        }
    }
    return QModelIndex();
}

void KPrShapeAnimations::setBeginTime(const QModelIndex &index, const int begin)
{
    if (!index.isValid()) {
        return;
    }
    KPrShapeAnimation *item = animationByRow(index.row());
    if (item) {
        setTimeRange(item, begin, item->globalDuration());
        emit dataChanged(index, index);
    }

}

void KPrShapeAnimations::setDuration(const QModelIndex &index, const int duration)
{
    if (!index.isValid()) {
        return;
    }
    KPrShapeAnimation *item = animationByRow(index.row());
    if (item) {
        setTimeRange(item, item->timeRange().first, duration);
        emit dataChanged(index, index);
    }
}

void KPrShapeAnimations::notifyAnimationEdited()
{
    if(KPrShapeAnimation *animation = qobject_cast<KPrShapeAnimation*>(sender())) {
        QModelIndex index = indexByAnimation(animation);
        if (index.isValid()) {
            emit dataChanged(index, index);
        }
    }
}

void KPrShapeAnimations::notifyOnClickEventChanged()
{
    emit onClickEventChanged();
}

KPrShapeAnimation *KPrShapeAnimations::animationByRow(const int row, int &groupCount) const
{
    int rowCount = 0;
    groupCount = 0;
    foreach (KPrAnimationStep *step, m_shapeAnimations) {
        for (int i=0; i < step->animationCount(); i++) {
            QAbstractAnimation *animation = step->animationAt(i);
            if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(animation)) {
                for (int j=0; j < a->animationCount(); j++) {
                    QAbstractAnimation *shapeAnimation = a->animationAt(j);
                    if (KPrShapeAnimation *b = dynamic_cast<KPrShapeAnimation*>(shapeAnimation)) {
                        if ((b->presetClass() != KPrShapeAnimation::None) && (b->shape())) {
                            if (b->NodeType() == KPrShapeAnimation::On_Click) {
                                groupCount = groupCount + 1;
                            }
                            if (rowCount == row) {
                                return b;
                            }
                            rowCount++;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

KPrShapeAnimation *KPrShapeAnimations::animationByRow(const int row) const
{
    int groupCount = 0;
    return animationByRow(row, groupCount);
}

QString KPrShapeAnimations::getAnimationName(KPrShapeAnimation *animation) const
{
    if (animation) {
        QStringList descriptionList = animation->id().split("-");
        if (descriptionList.count() > 2) {
            descriptionList.removeFirst();
            descriptionList.removeFirst();
        }
        return descriptionList.join(QString(" "));
    }
    return QString();
}

QPixmap KPrShapeAnimations::getAnimationShapeThumbnail(KPrShapeAnimation *animation) const
{
    if (animation) {
        //TODO: Draw image file to load when shape thumbnail can't be created
       QPixmap thumbnail = KIcon("stage").pixmap(KIconLoader::SizeMedium, KIconLoader::SizeMedium);

        if (thumbnail.convertFromImage(createThumbnail(animation->shape(),
                                                       QSize(KIconLoader::SizeMedium, KIconLoader::SizeMedium)))) {
            thumbnail.scaled(QSize(KIconLoader::SizeMedium, KIconLoader::SizeMedium), Qt::KeepAspectRatio);
        }
        return thumbnail;
    }
    return QPixmap();
}

QPixmap KPrShapeAnimations::getAnimationIcon(KPrShapeAnimation *animation) const
{
    if (!animation) {
        return QPixmap();
    }
    QString name = getAnimationName(animation);
    if (!name.isEmpty()) {
        name = name.append("_animation");
        name.replace(" ", "_");
        QString path = KIconLoader::global()->iconPath(name, KIconLoader::Toolbar, true);
        if (!path.isNull()) {
            return KIcon(name).pixmap(KIconLoader::SizeHuge, KIconLoader::SizeHuge);
        }
    }
    return KIcon("unrecognized_animation").pixmap(KIconLoader::SizeMedium, KIconLoader::SizeMedium);
}

QImage KPrShapeAnimations::createThumbnail(KoShape *shape, const QSize &thumbSize) const
{
    KoShapePainter painter;
    QList<KoShape*> shapes;
    shapes.append(shape);
    KoShapeContainer * container = dynamic_cast<KoShapeContainer*>(shape);
    if (container) {
        shapes.append(container->shapes());
    }

    painter.setShapes(shapes);

    QImage thumb(thumbSize, QImage::Format_RGB32);
    // draw the background of the thumbnail
    thumb.fill(QColor(Qt::white).rgb());

    QRect imageRect = thumb.rect();
    // use 2 pixel border around the content
    imageRect.adjust(2, 2, -2, -2);

    QPainter p(&thumb);
    painter.paint(p, imageRect, painter.contentRect());

    return thumb;
}

void KPrShapeAnimations::setTimeRangeIncrementalChange(KPrShapeAnimation *item, const int begin, const int duration, TimeUpdated updatedTimes)
{
    if (m_firstEdition) {
        m_oldBegin = item->timeRange().first;
        m_oldDuration = item->timeRange().second;
        m_currentEditedAnimation = item;
        m_firstEdition = false;
    }
    if (item == m_currentEditedAnimation) {
        if ((updatedTimes == BothTimes) || (updatedTimes == BeginTime)) {
            item->setBeginTime(begin);
        }
        if ((updatedTimes == BothTimes) || (updatedTimes == DurationTime)) {
            item->setGlobalDuration(duration);
        }
    }
    else {
        endTimeLineEdition();
    }
}

QModelIndex KPrShapeAnimations::indexByAnimation(KPrShapeAnimation *animation)
{
    int rowCount = 0;
    foreach (KPrAnimationStep *step, m_shapeAnimations) {
        for (int i=0; i < step->animationCount(); i++) {
            QAbstractAnimation *subStep = step->animationAt(i);
            if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(subStep)) {
                for (int j=0; j < a->animationCount(); j++) {
                    QAbstractAnimation *shapeAnimation = a->animationAt(j);
                    if (KPrShapeAnimation *b = dynamic_cast<KPrShapeAnimation*>(shapeAnimation)) {
                        if ((b->presetClass() != KPrShapeAnimation::None) && (b->shape())) {
                            if (b == animation) {
                                return this->index(rowCount, 0, QModelIndex());
                            }
                            rowCount++;
                        }
                    }
                }
            }
        }
    }
    return QModelIndex();
}

QList<KPrShapeAnimation *> KPrShapeAnimations::getWithPreviousSiblings(KPrShapeAnimation *animation, bool connectItems)
{
    bool startAdding = false;
    QList<KPrShapeAnimation *> siblings = QList<KPrShapeAnimation *>();

    if (KPrAnimationSubStep *a = animation->subStep()) {
        for (int j=0; j < a->animationCount(); j++) {
            QAbstractAnimation *shapeAnimation = a->animationAt(j);
            if (KPrShapeAnimation *b = dynamic_cast<KPrShapeAnimation*>(shapeAnimation)) {
                if ((b->presetClass() != KPrShapeAnimation::None) && (b->shape())) {
                    if (startAdding) {
                        if (connectItems) {
                            connect(b, SIGNAL(triggerEventChanged(KPrShapeAnimation::Node_Type)), this, SLOT(notifyAnimationEdited()));
                        }
                        siblings.append(b);
                    }
                    if (b == animation) {
                        startAdding = true;
                    }
                }
            }
        }
    }
    return siblings;
}

QList<KPrAnimationSubStep *> KPrShapeAnimations::getSubSteps(int start, int end, KPrAnimationStep *step)
{
    QList<KPrAnimationSubStep *>movedSubSteps = QList<KPrAnimationSubStep *>();
    for (int i = start; i < end; i++) {
        if (KPrAnimationSubStep *substep = dynamic_cast<KPrAnimationSubStep *>(step->animationAt(i))) {
           movedSubSteps.append(substep);
        }
    }
    return movedSubSteps;
}

bool KPrShapeAnimations::createTriggerEventEditCmd(KPrShapeAnimation *animation, KPrAnimationStep *newStep, KPrAnimationSubStep *newSubStep, KPrShapeAnimation::Node_Type newType, QList<KPrShapeAnimation *> children, QList<KPrAnimationSubStep *> movedSubSteps, KPrShapeAnimations *shapeAnimations)
{
    KPrAnimationEditNodeTypeCommand *command =
            new KPrAnimationEditNodeTypeCommand(animation, newStep, newSubStep,
                                                newType, children, movedSubSteps, shapeAnimations);
    if (m_document) {
        m_document->addCommand(command);
        emit timeScaleModified();
        return true;
    }
    return false;
}
