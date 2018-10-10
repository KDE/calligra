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

#include "KPrShapeAnimations.h"

//Qt Headers
#include <QList>
#include <QSet>
#include <QPainter>
#include <QPainterPath>

//Stage Headers
#include "KPrDocument.h"
#include "animations/KPrAnimationSubStep.h"
#include "animations/KPrAnimateMotion.h"
#include "commands/KPrAnimationRemoveCommand.h"
#include "commands/KPrReorderAnimationCommand.h"
#include <commands/KPrEditAnimationTimeLineCommand.h>
#include <commands/KPrAnimationEditNodeTypeCommand.h>
#include <commands/KPrReplaceAnimationCommand.h>
#include <commands/KPrAnimationCreateCommand.h>
#include "StageDebug.h"

//Calligra Headers
#include <KoShape.h>
#include <KoShapePainter.h>
#include <KoShapeContainer.h>
#include <KoPathShape.h>
#include <KoIcon.h>

//KF5 Headers
#include <kiconloader.h>
#include <klocalizedstring.h>

const int COLUMN_COUNT = 10;
const int INVALID = -1;

KPrShapeAnimations::KPrShapeAnimations(KPrDocument *document, QObject *parent)
    :QAbstractTableModel(parent)
    , m_currentEditedAnimation(0)
    , m_firstEdition(true)
    , m_oldBegin(INVALID)
    , m_oldDuration(INVALID)
    , m_document(document)
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
    KPrShapeAnimation::NodeType nodeType;
    int currentGroup = -1;
    KPrShapeAnimation *thisAnimation = animationByRow(index.row(), &currentGroup, &nodeType);
    if (!thisAnimation) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
            case Group: return currentGroup;
            case StepCount:
                if (nodeType == KPrShapeAnimation::OnClick) {
                    return currentGroup;
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
            case NodeType: return nodeType;
            default: Q_ASSERT(false);
        }
    }
    if (role == Qt::TextAlignmentRole) {
        if (index.column() == Name) {
            return static_cast<int>(Qt::AlignLeft|Qt::AlignVCenter);
        }
        return static_cast<int>(Qt::AlignCenter);
    }
    if (role == Qt::DecorationRole) {
        switch (index.column()) {
            case Group: return QVariant();
            case StepCount: return QVariant();
            case TriggerEvent:
                if (nodeType == KPrShapeAnimation::OnClick)
                    return koIcon("onclick");
                if (nodeType == KPrShapeAnimation::AfterPrevious)
                    return koIcon("after_previous");
                if (nodeType == KPrShapeAnimation::WithPrevious)
                    return koIcon("with_previous");
                return QVariant();
            case Name: return QVariant();
            case ShapeThumbnail: return getAnimationShapeThumbnail(thisAnimation);
            case AnimationIcon:  return getAnimationIcon(thisAnimation);
            case StartTime: return QVariant();
            case Duration: return QVariant();
            case AnimationClass: return QVariant();
            case NodeType: return QVariant();
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
            case NodeType: return QVariant();
            default: Q_ASSERT(false);
        }
    }
    if (role == Qt::ToolTipRole) {
            switch (index.column()) {
            case Group:
            case StepCount: return QVariant();
            case TriggerEvent:/// emitted if an item time range has changed (return the index of the item changed)
                if (nodeType == KPrShapeAnimation::OnClick)
                    return i18n("start on mouse click");
                if (nodeType == KPrShapeAnimation::AfterPrevious)
                    return i18n("start after previous animation");
                if (nodeType == KPrShapeAnimation::WithPrevious)
                    return i18n("start with previous animation");
                return QVariant();
            case Name: return QVariant();
            case ShapeThumbnail: return thisAnimation->shape()->name();
            case AnimationIcon: return getAnimationName(thisAnimation);
            case StartTime: {
                const float startDelay = thisAnimation->timeRange().first / 1000.0;
                const float duration = thisAnimation->globalDuration() / 1000.0;
                return i18n("Start after %1 seconds. Duration of %2 seconds.", startDelay, duration);
            }
            case Duration: return QVariant();
            case AnimationClass: return thisAnimation->presetClassText();
            case NodeType: return QVariant();
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

void KPrShapeAnimations::dump() const
{
    debugStageAnimation << "Share animations:";
    foreach (KPrAnimationStep *step, m_shapeAnimations) {
        debugStageAnimation << "  Step:";
        for (int i=0; i < step->animationCount(); i++) {
            QAbstractAnimation *animation = step->animationAt(i);
            if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(animation)) {
                debugStageAnimation << "    Substep" << a;
                for (int sub=0; sub < a->animationCount(); ++sub) {
                    QAbstractAnimation *baseAnim = a->animationAt(sub);
                    KPrShapeAnimation *anim = dynamic_cast<KPrShapeAnimation *>(baseAnim);
                    if (anim) {
                        debugStageAnimation << "      Animation" << anim << getAnimationName(anim);
                    } else {
                        debugStageAnimation << "      NOT a KPrShapeAnimation!" << anim;
                    }
                }
            } else {
                debugStageAnimation << "    NOT a KPrAnimationSubStep!" << animation;
            }
        }
    }
}

int KPrShapeAnimations::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    int rowCount = 0;
    foreach (KPrAnimationStep *step, m_shapeAnimations) {
        for (int i=0; i < step->animationCount(); i++) {
            QAbstractAnimation *animation = step->animationAt(i);
            if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(animation)) {
                rowCount = rowCount + a->animationCount();
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
    KPrShapeAnimation *thisAnimation = animationByRow(index.row());
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

void KPrShapeAnimations::init(const QList<KPrAnimationStep *> &animations)
{
    m_shapeAnimations = animations;
}

void KPrShapeAnimations::add(KPrShapeAnimation *animation)
{
    // TODO: what is the purpose of this empty KPrAnimationStep?
    if (m_shapeAnimations.isEmpty()) {
        m_shapeAnimations.append(new KPrAnimationStep());
    }
    if (!animation->step()) {
        KPrAnimationStep *newStep = new KPrAnimationStep();
        animation->setStep(newStep);
    }
    if (!animation->subStep()) {
        KPrAnimationSubStep *newSubStep = new KPrAnimationSubStep();
        animation->setSubStep(newSubStep);
    }
    if (!m_shapeAnimations.contains(animation->step())) {
        if ((animation->stepIndex() >= 0) && (animation->stepIndex() <= m_shapeAnimations.count())) {
            m_shapeAnimations.insert(animation->stepIndex(), animation->step());
        }
        else {
            m_shapeAnimations.append(animation->step());
        }
    }
    if (!(animation->step()->indexOfAnimation(animation->subStep()) >= 0)) {
        if ((animation->subStepIndex() >= 0) &&
                (animation->subStepIndex() <= animation->step()->animationCount())) {
            animation->step()->insertAnimation(animation->subStepIndex(), animation->subStep());
        }
        else {
            animation->step()->addAnimation(animation->subStep());
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
        animation->setSubStepIndex(step->indexOfAnimation(subStep));
        step->removeAnimation(subStep);
        if (step->animationCount() <= 0) {
            animation->setStepIndex(m_shapeAnimations.indexOf(step));
            m_shapeAnimations.removeAll(step);
        }
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

void KPrShapeAnimations::swapAnimations(KPrShapeAnimation *oldAnimation, KPrShapeAnimation *newAnimation)
{
    KPrAnimationStep *oldStep = oldAnimation->step();
    KPrAnimationSubStep *oldSubStep = oldAnimation->subStep();
    KPrAnimationSubStep *newSubStep = newAnimation->subStep();
    int oldIndex = oldSubStep->indexOfAnimation(oldAnimation);
    int newIndex = newSubStep->indexOfAnimation(newAnimation);
    if (oldSubStep != newSubStep) {
        oldSubStep->removeAnimation(oldAnimation);
        newSubStep->removeAnimation(newAnimation);
        oldSubStep->insertAnimation(oldIndex, newAnimation);
        newSubStep->insertAnimation(newIndex, oldAnimation);
    }
    else {
        if (oldIndex < newIndex) {
            oldSubStep->removeAnimation(newAnimation);
            oldSubStep->insertAnimation(oldIndex, newAnimation);
        }
        else {
            oldSubStep->removeAnimation(oldAnimation);
            oldSubStep->insertAnimation(newIndex, oldAnimation);
        }
    }

    oldAnimation->setStep(newAnimation->step());
    oldAnimation->setSubStep(newSubStep);
    newAnimation->setStep(oldStep);
    newAnimation->setSubStep(oldSubStep);
    QModelIndex indexOld = indexByAnimation(oldAnimation);
    QModelIndex indexNew = indexByAnimation(newAnimation);
    emit dataChanged(this->index(indexOld.row(), 0), this->index(indexOld.row(), COLUMN_COUNT));
    emit dataChanged(this->index(indexNew.row(), 0), this->index(indexNew.row(), COLUMN_COUNT));
}

void KPrShapeAnimations::replaceAnimation(KPrShapeAnimation *oldAnimation, KPrShapeAnimation *newAnimation)
{
    KPrAnimationSubStep *subStep = oldAnimation->subStep();
    int currentAnimationIndex = subStep->indexOfAnimation(oldAnimation);
    newAnimation->setStep(oldAnimation->step());
    newAnimation->setSubStep(oldAnimation->subStep());
    newAnimation->setTextBlockUserData(oldAnimation->textBlockUserData());
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

int KPrShapeAnimations::animationEnd(const QModelIndex &index) const
{
    if (index.isValid()) {
        KPrShapeAnimation *previousAnimation = animationByRow(index.row());
        KPrShapeAnimation::NodeType previousNodeType =
                static_cast<KPrShapeAnimation::NodeType>(data(this->index(index.row(),
                                                                           KPrShapeAnimations::NodeType)).toInt());
        if (previousNodeType == KPrShapeAnimation::OnClick) {
            return previousAnimation->timeRange().second;
        }
        if (previousNodeType == KPrShapeAnimation::WithPrevious) {
            return previousAnimation->timeRange().second +
                    animationStart(this->index(index.row() - 1, index.column(), QModelIndex()));
        }
        else if (previousNodeType == KPrShapeAnimation::AfterPrevious) {
            return previousAnimation->timeRange().second +
                    animationEnd(this->index(index.row() - 1, index.column(), QModelIndex()));
        }
    }
    return 0;
}

int KPrShapeAnimations::animationStart(const QModelIndex &index) const
{
    if (index.isValid()) {
        KPrShapeAnimation *previousAnimation = animationByRow(index.row());
        KPrShapeAnimation::NodeType previousNodeType =
                static_cast<KPrShapeAnimation::NodeType>(data(this->index(index.row(),
                                                                           KPrShapeAnimations::NodeType)).toInt());
        if (previousNodeType == KPrShapeAnimation::OnClick) {
            return previousAnimation->timeRange().first;
        }
        if (previousNodeType == KPrShapeAnimation::WithPrevious) {
            return animationStart(this->index(index.row() - 1, index.column(), QModelIndex()));
        }
        else if (previousNodeType == KPrShapeAnimation::AfterPrevious) {
            return animationEnd(this->index(index.row() - 1, index.column(), QModelIndex()));
        }
    }
    return 0;
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

bool KPrShapeAnimations::setTriggerEvent(const QModelIndex &index, const KPrShapeAnimation::NodeType type)
{
    KPrShapeAnimation *animation = animationByRow(index.row());
    if (animation) {
        KPrShapeAnimation::NodeType currentType =
                static_cast<KPrShapeAnimation::NodeType>(data(this->index(index.row(),
                                                                           KPrShapeAnimations::NodeType)).toInt());
        if (currentType == KPrShapeAnimation::AfterPrevious) {
            if (type == KPrShapeAnimation::WithPrevious) {
                Q_ASSERT(index.row() > 0);
            }
        }
        else if (currentType == KPrShapeAnimation::OnClick) {
             if (index.row() < 1) {
                 // Resync trigger event edit widget
                 emit layoutChanged();
                 return false;
             }
        }
        if (type != currentType) {
            return createTriggerEventEditCmd(animation, currentType, type);
        }
    }
    return false;
}

bool KPrShapeAnimations::setNodeType(KPrShapeAnimation *animation, const KPrShapeAnimation::NodeType type)
{
    resyncStepsWithAnimations();
    if (animation) {
        QModelIndex index = indexByAnimation(animation);
        if (!index.isValid()) {
            return false;
        }
        QList<KPrShapeAnimation *> movedChildren = QList<KPrShapeAnimation *>();
        QList<KPrAnimationSubStep *>movedSubSteps = QList<KPrAnimationSubStep *>();
        KPrAnimationSubStep *newSubStep = 0;
        KPrAnimationStep *newStep = 0;
        KPrShapeAnimation::NodeType currentType =
                static_cast<KPrShapeAnimation::NodeType>(data(this->index(index.row(),
                                                                           KPrShapeAnimations::NodeType)).toInt());
        if (currentType == KPrShapeAnimation::AfterPrevious) {
            // After Previous to With Previous
            if (type == KPrShapeAnimation::WithPrevious) {
                //use previous animation to reparent current animation
                Q_ASSERT(index.row() > 0);
                KPrShapeAnimation *previousAnimation = animationByRow(index.row() - 1);
                newSubStep = previousAnimation->subStep();
                movedChildren = getWithPreviousSiblings(animation);
            }

            // After Previous to On Click
            else if (type == KPrShapeAnimation::OnClick) {
                 // Get index of current substep
                 int currentSubStepIndex = animation->step()->indexOfAnimation(animation->subStep());
                 int subStepCount = animation->step()->animationCount();

                 //Create new step to reparent current item and all following items.
                 newStep = new KPrAnimationStep();

                 // Add step after original one
                 int currentStepIndex = m_shapeAnimations.indexOf(animation->step());
                 insertStep(currentStepIndex + 1, newStep);

                 //reparent children
                 if (currentSubStepIndex < subStepCount - 1) {
                     movedSubSteps = getSubSteps(currentSubStepIndex + 1, subStepCount, animation->step());
                 }
            }
            else {
                return false;
            }
        }
        else if (currentType == KPrShapeAnimation::WithPrevious) {
           // With Previous to After Previous
           if (type == KPrShapeAnimation::AfterPrevious) {
               // Get index of current substep
               int currentSubStepIndex = animation->step()->indexOfAnimation(animation->subStep());
               //Create new substep to reparent current item and all following items.
               newSubStep = new KPrAnimationSubStep();

               // Add substep after original one
               animation->step()->insertAnimation(currentSubStepIndex + 1, newSubStep);

               //reparent children
               movedChildren = getWithPreviousSiblings(animation);
           }
           // With Previous to On Click
           else if (type == KPrShapeAnimation::OnClick) {
                // Get index of current substep
                int currentSubStepIndex = animation->step()->indexOfAnimation(animation->subStep());
                int subStepCount = animation->step()->animationCount();

                //Create new step to reparent current item and all following items.
                newStep = new KPrAnimationStep();

                //Create new substep to reparent current item and all following items.
                newSubStep = new KPrAnimationSubStep();

                // Add step after original one
                //insert new Step
                int currentStepIndex = m_shapeAnimations.indexOf(animation->step());
                insertStep(currentStepIndex + 1, newStep);

                //reparent children
                if (currentSubStepIndex < subStepCount - 1) {
                    movedSubSteps = getSubSteps(currentSubStepIndex + 1, subStepCount, animation->step());
                }
                movedChildren = getWithPreviousSiblings(animation);
           }
           else {
               return false;
           }
        }
        else if (currentType == KPrShapeAnimation::OnClick) {
             if (index.row() < 1) {
                 // Resync trigger event edit widget
                 emit layoutChanged();
                 return false;
             }
            // On click to With Previous
            if (type == KPrShapeAnimation::WithPrevious) {
                // Get previous animation
                KPrShapeAnimation *previousAnimation = animationByRow(index.row() - 1);
                newStep = previousAnimation->step();
                newSubStep = previousAnimation->subStep();

                movedChildren = getWithPreviousSiblings(animation);

                int subStepCount = animation->step()->animationCount();
                int currentSubStepIndex = animation->step()->indexOfAnimation(animation->subStep());
                if (subStepCount > 1) {
                    movedSubSteps = getSubSteps(currentSubStepIndex + 1, subStepCount, animation->step());
                }
            }

            // On click to After Previous
            else if (type == KPrShapeAnimation::AfterPrevious) {
                 // Get previous animation
                 KPrShapeAnimation *previousAnimation = animationByRow(index.row() - 1);
                 newStep = previousAnimation->step();
                 int subStepCount = animation->step()->animationCount();
                 if (subStepCount > 1) {
                     movedSubSteps = getSubSteps(1, subStepCount, animation->step());
                 }
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }
        KPrAnimationSubStep *oldSubStep = animation->subStep();
        KPrAnimationStep *oldStep = animation->step();

        // if new subStep reparent main item and children
        if (newSubStep) {
            if (oldSubStep->indexOfAnimation(animation) >= 0) {
                newSubStep->addAnimation(oldSubStep->takeAnimation(oldSubStep->indexOfAnimation(animation)));
            }
            if (!movedChildren.isEmpty()) {
                foreach(KPrShapeAnimation *anim, movedChildren) {
                    if ((oldSubStep->indexOfAnimation(anim) >= 0) && (oldSubStep->indexOfAnimation(anim) <
                                                                      oldSubStep->animationCount())) {
                        newSubStep->addAnimation(oldSubStep->takeAnimation(oldSubStep->indexOfAnimation(anim)));
                    }
                }
            }
        }
        // If newStep reparent subSteps and children
        if (newStep) {
            if (!newSubStep) {
                newSubStep = oldSubStep;
            }
            if (movedSubSteps.isEmpty()) {
                movedSubSteps.append(newSubStep);
            }
            else {
                movedSubSteps.insert(0, newSubStep);
            }
            foreach(KPrAnimationSubStep *subStep, movedSubSteps) {
                newStep->addAnimation(subStep);
            }
        }
        // If old substep or step is empty remove from list;
        if (oldSubStep->children().isEmpty()) {
            oldSubStep->setParent(0);
        }
        if (oldStep->children().isEmpty()) {
            removeStep(oldStep);
        }

        if ((currentType == KPrShapeAnimation::OnClick) || (type == KPrShapeAnimation::OnClick)) {
            notifyOnClickEventChanged();
        }
        notifyAnimationChanged(animation);
        resyncStepsWithAnimations();
        return true;
    }
    return false;
}

void KPrShapeAnimations::recalculateStart(const QModelIndex &mIndex)
{
    if (!mIndex.isValid() || mIndex.row() < 1) {
        return;
    }
    KPrShapeAnimation *animation = animationByRow(mIndex.row());

    KPrShapeAnimation::NodeType type =
            static_cast<KPrShapeAnimation::NodeType>(data(this->index(mIndex.row(),
                                                                       KPrShapeAnimations::NodeType)).toInt());
    if (type == KPrShapeAnimation::AfterPrevious) {
        setTimeRange(animation, animationEnd(mIndex), animation->globalDuration());
        setTriggerEvent(mIndex, KPrShapeAnimation::WithPrevious);
    }
    else if (type == KPrShapeAnimation::WithPrevious) {
        recalculateStart(index(mIndex.row() - 1, 0));
    }
}

QModelIndex KPrShapeAnimations::moveUp(const QModelIndex &index)
{
    if (!index.isValid() || index.row() < 1) {
        return QModelIndex();
    }
    return moveAnimation(index.row(), index.row() - 1);
}

QModelIndex KPrShapeAnimations::moveDown(const QModelIndex &index)
{
    if (!index.isValid() || (index.row() >= (rowCount() - 1))) {
        return QModelIndex();
    }

    return moveAnimation(index.row(), index.row() + 1);
}

QModelIndex KPrShapeAnimations::moveAnimation(int oldRow, int newRow)
{
    Q_ASSERT(0 <= oldRow && oldRow < rowCount() &&
             0 <= newRow && newRow < rowCount());
    QModelIndex newIndex;
    // swap items
    KPrShapeAnimation *animationOld = animationByRow(oldRow);
    KPrShapeAnimation *animationNew = animationByRow(newRow);
    Q_ASSERT(animationOld);
    Q_ASSERT(animationNew);
    if (m_document) {
        newIndex = index(newRow, 0);
        KPrReorderAnimationCommand *cmd = new KPrReorderAnimationCommand(this, animationOld, animationNew);
        m_document->addCommand(cmd);
    }
    return newIndex;
}

QModelIndex KPrShapeAnimations::removeAnimationByIndex(const QModelIndex &index)
{
    if (!index.isValid()) {
        return index;
    }
    KPrShapeAnimation *animation = animationByRow(index.row());
    Q_ASSERT(animation);

    if (animation) {
        Q_ASSERT(m_document);
        KPrAnimationRemoveCommand *command = new KPrAnimationRemoveCommand(m_document, animation);
        m_document->addCommand(command);
    }
    return QModelIndex();
}

KoShape *KPrShapeAnimations::shapeByIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        KPrShapeAnimation *animation = animationByRow(index.row());
        if (animation) {
            return animation->shape();
        }
    }
    return 0;
}

QModelIndex KPrShapeAnimations::indexByShape(KoShape *shape) const
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
                            if (b->shape() == shape)
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
    if (KPrShapeAnimation *animation = qobject_cast<KPrShapeAnimation*>(sender())) {
        QModelIndex index = indexByAnimation(animation);
        if (index.isValid()) {
            emit dataChanged(index, index);
        }
    }
}

void KPrShapeAnimations::notifyAnimationChanged(KPrShapeAnimation *animation)
{
    QModelIndex index = indexByAnimation(animation);
    if (index.isValid()) {
        emit dataChanged(this->index(index.row(), 0), this->index(index.row(), COLUMN_COUNT));
    }
}

void KPrShapeAnimations::notifyOnClickEventChanged()
{
    emit onClickEventChanged();
}

KPrShapeAnimation *KPrShapeAnimations::animationByRow(int row, int *pGroup, KPrShapeAnimation::NodeType *pNodeType) const
{
    int rowCount = 0;
    int groupCount = 0;
    KPrShapeAnimation::NodeType currentNodeType = KPrShapeAnimation::OnClick;
    foreach (KPrAnimationStep *step, m_shapeAnimations) {
        int stepChild = -1;
        if (step->animationCount() > 0) {
            currentNodeType = KPrShapeAnimation::OnClick;
            ++groupCount;
        }
        for (int i=0; i < step->animationCount(); i++) {
            QAbstractAnimation *animation = step->animationAt(i);
            if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(animation)) {
                int subStepChild = -1;
                if (stepChild != -1) {
                    currentNodeType = KPrShapeAnimation::AfterPrevious;
                }
                if (rowCount + a->animationCount() < row) {
                    rowCount = rowCount + a->animationCount();
                    stepChild = stepChild + a->animationCount();
                    subStepChild = subStepChild + a->animationCount();
                    continue;
                }
                for (int j=0; j < a->animationCount(); j++) {
                    QAbstractAnimation *shapeAnimation = a->animationAt(j);
                    if (KPrShapeAnimation *b = dynamic_cast<KPrShapeAnimation*>(shapeAnimation)) {
                        stepChild++;
                        subStepChild++;
                        if (subStepChild > 0) {
                            currentNodeType = KPrShapeAnimation::WithPrevious;
                        }
                        if (rowCount == row) {
                            if (pGroup) {
                                *pGroup = groupCount;
                            }
                            if (pNodeType) {
                                *pNodeType = currentNodeType;
                            }
                            return b;
                        }
                        rowCount++;
                    }
                }
            }
        }
    }
    return 0;
}

void KPrShapeAnimations::insertNewAnimation(KPrShapeAnimation *newAnimation, const QModelIndex &previousAnimation)
{
    Q_ASSERT(newAnimation);
    // Create new Parent step and substep
    KPrAnimationStep *newStep = new KPrAnimationStep();
    KPrAnimationSubStep *newSubStep = new KPrAnimationSubStep();
    int stepIndex = -1;
    // insert step and substep
    if (previousAnimation.isValid()) {
        KPrShapeAnimation *previous = animationByRow(previousAnimation.row());
        stepIndex = m_shapeAnimations.indexOf(previous->step()) + 1;
    }
    else if (m_shapeAnimations.count() < 1) {
        stepIndex = -1;
    }
    else {
        stepIndex = m_shapeAnimations.count();
    }

    // Setup new Animation
    newAnimation->setStepIndex(stepIndex);
    newAnimation->setStep(newStep);
    newAnimation->setSubStep(newSubStep);
    newStep->addAnimation(newSubStep);
    Q_ASSERT(m_document);
    KPrAnimationCreateCommand *command = new KPrAnimationCreateCommand(m_document, newAnimation);
    m_document->addCommand(command);
}

QString KPrShapeAnimations::getAnimationName(KPrShapeAnimation *animation, bool omitSubType) const
{
    if (animation) {
        QStringList descriptionList = animation->id().split(QLatin1Char('-'));
        if (descriptionList.count() > 2) {
            descriptionList.removeFirst();
            descriptionList.removeFirst();
        }
        if (!omitSubType && (!animation->presetSubType().isEmpty())) {
            descriptionList.append(animation->presetSubType());
        }
        return descriptionList.join(QChar::fromLatin1(' '));
    }
    return QString();
}

QPixmap KPrShapeAnimations::getAnimationShapeThumbnail(KPrShapeAnimation *animation) const
{
    if (animation) {
        //TODO: Draw image file to load when shape thumbnail can't be created
       QPixmap thumbnail = koIcon("calligrastage").pixmap(KIconLoader::SizeMedium, KIconLoader::SizeMedium);

        if (
            thumbnail.convertFromImage(createThumbnail(animation->shape(),
                                                       QSize(KIconLoader::SizeMedium, KIconLoader::SizeMedium)))
        ) {
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
    QString name = getAnimationName(animation, true);
    // Return Path Motion Animation icon
    if (animation->presetClass() == KPrShapeAnimation::MotionPath) {
        QPainterPath m_path;
        for (int i = 0; i < animation->animationCount(); i++) {
            if (KPrAnimateMotion *motion = dynamic_cast<KPrAnimateMotion *>(animation->animationAt(i))) {
                m_path = motion->pathOutline();
                break;
            }
        }
        if (!m_path.isEmpty()) {
            const int margin = 8;
            const int width = 4;
            QImage thumb(QSize(KIconLoader::SizeHuge, KIconLoader::SizeHuge), QImage::Format_RGB32);
            // fill backgroung
            thumb.fill(QColor(Qt::white).rgb());
            QRect imageRect = thumb.rect();
            // adjust to left space for margins
            imageRect.adjust(margin, margin, -margin, -margin);
            //Center path
            m_path.translate(-m_path.boundingRect().x() + margin, -m_path.boundingRect().y() + margin);
            QTransform transform;
            transform.scale(thumb.width() / (m_path.boundingRect().width() + 2 * margin),
                            thumb.height() / (m_path.boundingRect().height() + 2 * margin));
            m_path = m_path * transform;
            QPainter painter(&thumb);
            painter.setRenderHints(QPainter::Antialiasing);
            painter.setPen(QPen(QColor(0, 100, 224), width, Qt::SolidLine,
                                Qt::FlatCap, Qt::MiterJoin));
            painter.drawPath(m_path);
            QPixmap iconPixmap;
            if (iconPixmap.convertFromImage(thumb)) {
                return iconPixmap;
            }
        }
    }
    // Return animation icon
    else if (!name.isEmpty()) {
        name = name.append("_animation");
        name.replace(QLatin1Char(' '), QLatin1Char('_'));
        QString path = KIconLoader::global()->iconPath(name, KIconLoader::Toolbar, true);
        if (!path.isNull()) {
            return QIcon::fromTheme(name).pixmap(KIconLoader::SizeHuge, KIconLoader::SizeHuge);
        }
    }
    return koIcon("unrecognized_animation").pixmap(KIconLoader::SizeMedium, KIconLoader::SizeMedium);
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

QModelIndex KPrShapeAnimations::indexByAnimation(KPrShapeAnimation *animation) const
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

void KPrShapeAnimations::resyncStepsWithAnimations()
{
    int row = -1;
    foreach (KPrAnimationStep *step, m_shapeAnimations) {
        row++;
        for (int i=0; i < step->animationCount(); i++) {
            QAbstractAnimation *subStep = step->animationAt(i);
            if (KPrAnimationSubStep *a = dynamic_cast<KPrAnimationSubStep*>(subStep)) {
                for (int j=0; j < a->animationCount(); j++) {
                    QAbstractAnimation *shapeAnimation = a->animationAt(j);
                    if (KPrShapeAnimation *b = dynamic_cast<KPrShapeAnimation*>(shapeAnimation)) {
                        if ((b->presetClass() != KPrShapeAnimation::None) && (b->shape())) {
                            b->setStep(step);
                            b->setSubStep(a);
                        }
                    }
                }
            }
        }
    }
}

KPrShapeAnimation::NodeType KPrShapeAnimations::triggerEventByIndex(const QModelIndex &index)
{
    Q_ASSERT(index.isValid());
    KPrShapeAnimation::NodeType nodeType = KPrShapeAnimation::OnClick;
    animationByRow(index.row(), 0, &nodeType);
    return nodeType;
}

QList<KPrShapeAnimation *> KPrShapeAnimations::getWithPreviousSiblings(KPrShapeAnimation *animation) const
{
    bool startAdding = false;
    QList<KPrShapeAnimation *> siblings = QList<KPrShapeAnimation *>();

    if (KPrAnimationSubStep *a = animation->subStep()) {
        for (int j=0; j < a->animationCount(); j++) {
            QAbstractAnimation *shapeAnimation = a->animationAt(j);
            if (KPrShapeAnimation *b = dynamic_cast<KPrShapeAnimation*>(shapeAnimation)) {
                if ((b->presetClass() != KPrShapeAnimation::None) && (b->shape())) {
                    if (startAdding) {
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

QList<KPrAnimationSubStep *> KPrShapeAnimations::getSubSteps(int start, int end, KPrAnimationStep *step) const
{
    QList<KPrAnimationSubStep *>movedSubSteps = QList<KPrAnimationSubStep *>();
    for (int i = start; i < end; i++) {
        if (KPrAnimationSubStep *substep = dynamic_cast<KPrAnimationSubStep *>(step->animationAt(i))) {
           movedSubSteps.append(substep);
        }
    }
    return movedSubSteps;
}

bool KPrShapeAnimations::createTriggerEventEditCmd(KPrShapeAnimation *animation, KPrShapeAnimation::NodeType oldType, KPrShapeAnimation::NodeType newType)
{
    KPrAnimationEditNodeTypeCommand *command =new KPrAnimationEditNodeTypeCommand(animation, oldType, newType, this);
    if (m_document) {
        m_document->addCommand(command);
        emit timeScaleModified();
        return true;
    }
    return false;
}
