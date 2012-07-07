/* This file is part of the KDE project
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

#include "KPrAnimationsDataModel.h"

#include <animations/KPrAnimationStep.h>
#include "animations/KPrAnimationSubStep.h"
#include <KPrPage.h>
#include <KPrView.h>
#include <KPrDocument.h>
#include <KPrCustomAnimationItem.h>
#include <commands/KPrEditAnimationTimeLineCommand.h>
#include <commands/KPrAnimationEditNodeTypeCommand.h>

//KDE HEADERS
#include <KIconLoader>
#include <KLocale>
#include "KoShape.h"
#include "KoShapeContainer.h"
#include "KoShapePainter.h"
#include <KoPADocument.h>
#include <KoShapeManager.h>

//QT HEADERS
#include <QImage>
#include <QPainter>
#include <QDebug>
#include <QAbstractAnimation>

const int REAL_COLUMN_COUNT = 3;
const int INVALID = -1;

enum ColumnNames {
    ShapeThumbnail = 0,
    AnimationIcon = 1,
    StartTime = 2,
    Duration = 3,
    AnimationClass = 4,
    TriggerEvent = 5
};

KPrAnimationsDataModel::KPrAnimationsDataModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_rootItem(0)
    , m_mainRoot(0)
    , m_oldBegin(INVALID)
    , m_oldDuration(INVALID)
    , m_firstEdition(true)
    , m_currentEditedItem(0)
{
}

QModelIndex KPrAnimationsDataModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!m_rootItem || row < 0 || column < 0 || column > TriggerEvent) {
        return QModelIndex();
    }
    KPrCustomAnimationItem *parentItem = itemForIndex(parent);
    Q_ASSERT(parentItem);
    if ((row == 0) && !m_rootItem->isDefaulAnimation()) {
        return createIndex(row, column, m_rootItem);
    }
    //Make sure of not display default init event
    if (KPrCustomAnimationItem *item = parentItem->childAt(
                parentItem->isDefaulAnimation() ? row : row - 1)) {
        return createIndex(row, column, item);
    }
    return QModelIndex();
}

QModelIndex KPrAnimationsDataModel::indexByShape(KoShape *shape)
{
    QModelIndex parent = QModelIndex();
    if (!shape) {
        return QModelIndex();
    }
    for (int row = 0; row < rowCount(parent); ++row) {
        QModelIndex thisIndex = index(row, 0, parent);
        if (thisIndex.isValid()) {
            KPrCustomAnimationItem *item = itemForIndex(thisIndex);
            if (item->shape()) {
                if (item->shape() == shape) {
                    return thisIndex;
                }
            }
        }
    }
    return QModelIndex();
}

QModelIndex KPrAnimationsDataModel::indexByItem(KPrCustomAnimationItem *item)
{
    QModelIndex parent = QModelIndex();
    if (!item) {
        return QModelIndex();
    }
    if (item->isDefaulAnimation()) {
        return QModelIndex();
    }
    for (int row = 0; row < rowCount(parent); ++row) {
        QModelIndex thisIndex = index(row, 0, parent);
        KPrCustomAnimationItem *thisItem = itemForIndex(thisIndex);
        if (item == thisItem) {
            return thisIndex;
        }
    }
    return QModelIndex();
}

int KPrAnimationsDataModel::rowCount(const QModelIndex &parent) const
{   
    if (parent.isValid()) {
        return 0;
    }
    KPrCustomAnimationItem *parentItem = itemForIndex(parent);
    if (parentItem) {
        return parentItem->isDefaulAnimation() ? parentItem->childCount() : parentItem->childCount() + 1;
    }
    else {
        return 0;
    }
}

int KPrAnimationsDataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return REAL_COLUMN_COUNT;
}

QVariant KPrAnimationsDataModel::data(const QModelIndex &index, int role) const
{
    if (!m_rootItem || !index.isValid() || index.column() < 0 ||
        index.column() > TriggerEvent) {
        return QVariant();
    }
    if (KPrCustomAnimationItem *item = itemForIndex(index)) {
        if (role == Qt::TextAlignmentRole) {
            return int(Qt::AlignRight | Qt::AlignVCenter);
        } else if (role == Qt::DisplayRole) {
            switch (index.column()) {
            case ShapeThumbnail:
                return QVariant();
            case AnimationIcon:
                return QVariant();
            case StartTime:
                return item->startTimeSeconds();
            case Duration:
                return item->durationSeconds();
            case AnimationClass:
                return item->type();
            case TriggerEvent:
                return item->triggerEvent();
            default:
                return QVariant();

            }
        } else if (role == Qt::DecorationRole) {
            switch (index.column()) {
            case ShapeThumbnail:
                return item->thumbnail();
            case AnimationIcon:
                return item->animationIcon();
            default:
                return QVariant();
            }
        } else if (role == Qt::TextAlignmentRole) {
                return Qt::AlignCenter;
        } else if (role == Qt::ToolTipRole) {
            switch (index.column()) {
            case ShapeThumbnail:
                return QVariant();
            case AnimationIcon:
                return item->animationName();
            case StartTime:
                return i18n("Start after %1 seconds. Duration of %2 seconds").
                        arg(item->startTimeSeconds()).arg(item->durationSeconds());
            case Duration:
            case AnimationClass:
                return item->type();
            default:
                return QVariant();

            }
        }
    }
    return QVariant();
}

QVariant KPrAnimationsDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Horizontal) {
        switch (section) {
        case ShapeThumbnail:
            return i18n("Seconds");
        default:
            return QVariant();
        }

    }
    return QVariant();
}

bool KPrAnimationsDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    //TODO: Edition features are not yet implemented
    if (!m_rootItem || !index.isValid() || index.column() < 0 ||
        index.column() > TriggerEvent) {
        return false;
    }
    if (KPrCustomAnimationItem *item = itemForIndex(index)) {
        if (role == Qt::EditRole) {
            switch (index.column()) {
            case ShapeThumbnail:
                return false;
            case AnimationIcon:
                return false;
            case StartTime:
                setTimeRangeIncrementalChange(item, value.toInt(), item->duration());
                emit dataChanged(index, index);
                return true;
            case Duration:
                setTimeRangeIncrementalChange(item, item->beginTime(), value.toInt());
                emit dataChanged(index, index);
                return true;
            case AnimationClass:
                return false;
            default:
                return false;

            }
        }
    }
    return false;
}

Qt::ItemFlags KPrAnimationsDataModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    return flags;
}

void KPrAnimationsDataModel::setParentItem(KPrCustomAnimationItem *item, KPrCustomAnimationItem *rootItem)
{
    qDebug() << "set Parent";
    emit layoutAboutToBeChanged();
    if (item) {
        m_mainRoot = rootItem;
        if (item->parent() == rootItem) {
            if (m_rootItem != item) {
                m_rootItem = item;
                reset();
                emit layoutChanged();
                qDebug() << "set parent success 1";
            }
        }
    }
    else{
        m_rootItem = 0;
        reset();
        emit layoutChanged();
        qDebug() << "set parent success 2";
    }
}

void KPrAnimationsDataModel::setDocumentView(KPrView *view)
{
    m_view = view;
    if (m_view->kopaDocument())
    {
        connect(m_view->kopaDocument(), SIGNAL(shapeRemoved(KoShape*)), this, SLOT(removeModel()));
        connect(m_view->kopaDocument(), SIGNAL(shapeAdded(KoShape*)), this, SLOT(removeModel()));
        KPrDocument *doc = dynamic_cast<KPrDocument*>(m_view->kopaDocument());
        if (doc) {
            connect(doc, SIGNAL(animationAdded(KPrShapeAnimation*)), this, SLOT(removeModel()));
            connect(doc, SIGNAL(animationRemoved(KPrShapeAnimation*)), this, SLOT(removeModel()));
        }
    }
    reset();
}

void KPrAnimationsDataModel::update()
{
    if (!m_rootItem->shape()) {
        m_rootItem = 0;
        emit layoutAboutToBeChanged();
        emit layoutChanged();
    }
}

void KPrAnimationsDataModel::setTimeRangeIncrementalChange(KPrCustomAnimationItem *item, const int begin, const int duration)
{
    if (m_firstEdition) {
        m_oldBegin = item->beginTime();
        m_oldDuration = item->duration();
        m_currentEditedItem = item;
        m_firstEdition = false;
    }
    if (item == m_currentEditedItem) {
        item->setStartTime(begin);
        item->setDuration(duration);
    }
    else {
        endTimeLineEdition();
    }
}

void KPrAnimationsDataModel::removeModel()
{
    m_rootItem = 0;
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

void KPrAnimationsDataModel::notifyAnimationEdited()
{
    if(KPrCustomAnimationItem *item = qobject_cast<KPrCustomAnimationItem*>(sender())) {
        QModelIndex index = indexByItem(item);
        if (index.isValid()) {
            emit dataChanged(index, index);
        }
    }
}

void KPrAnimationsDataModel::initializeNewParent()
{
    if (m_mainRoot) {
        if(KPrCustomAnimationItem *item = qobject_cast<KPrCustomAnimationItem*>(sender())) {
            if (item->triggerEvent() == KPrShapeAnimation::On_Click) {
                setParentItem(item, m_mainRoot);
            }
            else if (item->parent()->triggerEvent() == KPrShapeAnimation::On_Click) {
                setParentItem(item->parent(), m_mainRoot);
            }
        }
    }
    m_mainRoot->notifyRootModified();
}

void KPrAnimationsDataModel::setBeginTime(const QModelIndex &index, const int begin)
{
    KPrCustomAnimationItem *item = itemForIndex(index);
    if (item && (!item->isDefaulAnimation())) {
        setTimeRange(item, begin, item->duration());
        emit dataChanged(index, index);
    }
}

void KPrAnimationsDataModel::setDuration(const QModelIndex &index, const int duration)
{
    KPrCustomAnimationItem *item = itemForIndex(index);
    if (item && (!item->isDefaulAnimation())) {
        setTimeRange(item, item->beginTime(), duration);
        emit dataChanged(index, index);
    }
}

void KPrAnimationsDataModel::setTimeRange(KPrCustomAnimationItem *item, const int begin, const int duration)
{
    if (item && (!item->isDefaulAnimation())) {
        KPrDocument *doc = dynamic_cast<KPrDocument*>(m_view->kopaDocument());
        KPrEditAnimationTimeLineCommand *command = new KPrEditAnimationTimeLineCommand(item->animation(),
                                                                                     begin, duration);
        doc->addCommand(command);
        connect(item, SIGNAL(timeChanged(int,int)), this, SLOT(notifyAnimationEdited()));
    }
}

bool KPrAnimationsDataModel::setTriggerEvent(const QModelIndex &index, const KPrShapeAnimation::Node_Type type)
{
    bool success = false;
    KPrAnimationEditNodeTypeCommand *command = 0;
    qDebug() << "set Trigger Event" << type;
    KPrCustomAnimationItem *item = itemForIndex(index);
    if (item && (!item->isDefaulAnimation())) {
        QList<KPrShapeAnimation *> movedChildren = QList<KPrShapeAnimation *>();
        QList<KPrAnimationSubStep *>movedSubSteps = QList<KPrAnimationSubStep *>();
        qDebug() << "Item found and not is default" << item->animationName() << item->animation()->step()->animationCount();
        if (item->triggerEvent() == KPrShapeAnimation::After_Previous) {
             qDebug() << "Item is After previous";
            // After Previous to With Previous
            if (type == KPrShapeAnimation::With_Previous) {
                 qDebug() << "new type is Wit previous";
                KPrCustomAnimationItem *parent = item->parent();
                int currentIndex = parent->children().indexOf(item);
                qDebug() << "Current Index: " << currentIndex;
                //use previous animation to reparent current animation
                KPrCustomAnimationItem *previousItem = (currentIndex > 0) ? parent->childAt(currentIndex-1) :
                                                                            parent;

                KPrAnimationSubStep *newSubStep = previousItem->animation()->subStep();
                command =
                        new KPrAnimationEditNodeTypeCommand(item->animation(), item->animation()->step(),
                                                            newSubStep, type);
                connect(item, SIGNAL(triggerEventChanged(KPrShapeAnimation::Node_Type)), this, SLOT(notifyAnimationEdited()));
                success = true;
            }

            // After Previous to On Click
            if (type == KPrShapeAnimation::On_Click) {
                 qDebug() << "new type is On Click";
                 KPrCustomAnimationItem *parent = item->parent();
                 int currentIndex = parent->children().indexOf(item);
                 int childrenCount = parent->childCount();
                 qDebug() << "Current Index: " << currentIndex << childrenCount;
                 // Get index of current substep
                 int currentSubStepIndex = item->animation()->step()->indexOfAnimation(item->animation()->subStep());
                 int subStepCount = item->animation()->step()->animationCount();

                 qDebug() << "currentSubStepIndex: " << currentSubStepIndex << subStepCount;

                 //Create new step to reparent currrent item and all following items.
                 KPrAnimationStep *newStep = new KPrAnimationStep();

                 // Add step after original one
                 KPrPage *activePage = dynamic_cast<KPrPage *>(m_view->activePage());
                 if (!activePage) {
                     qDebug() << "active page missing";
                     return false;
                 }
                 int currentStepIndex = activePage->animationSteps().indexOf(item->animation()->step());
                 qDebug() << "currentStepIndex: " << currentStepIndex << activePage->animationSteps().count();
                 activePage->animations().insertStep(currentStepIndex + 1, newStep);
                 qDebug() << activePage->animationSteps().count();

                 //reparent children
                 connect(item, SIGNAL(triggerEventChanged(KPrShapeAnimation::Node_Type)), this, SLOT(initializeNewParent()));
                 qDebug() << "reparent main" << item->animationName();

                 if (currentSubStepIndex < subStepCount-1) {
                     for (int i =currentSubStepIndex + 1; i < subStepCount; i++) {
                         if (KPrAnimationSubStep *substep = dynamic_cast<KPrAnimationSubStep *>(item->animation()->step()->animationAt(i))) {
                             qDebug() << "move subStep " << i;
                            movedSubSteps.append(substep);
                         }
                     }

                 }

                 command =
                         new KPrAnimationEditNodeTypeCommand(item->animation(), newStep, item->animation()->subStep(),
                                                             type, movedChildren, movedSubSteps, activePage);
                 success = true;
            }
        }
        if (item->triggerEvent() == KPrShapeAnimation::With_Previous) {
            qDebug() << "Item is With previous";
           // With Previous to After Previous
           if (type == KPrShapeAnimation::After_Previous) {
                qDebug() << "new type is After previous";
               KPrCustomAnimationItem *parent = item->parent();
               int currentIndex = parent->children().indexOf(item);
               int childrenCount = parent->childCount();
               qDebug() << "Current Index: " << currentIndex << childrenCount;
               // Get index of current substep
               int currentSubStepIndex = item->animation()->step()->indexOfAnimation(item->animation()->subStep());
               qDebug() << "currentSubStepIndex: " << currentSubStepIndex;
               //Create new substep to reparent currrent item and all following items.
               KPrAnimationSubStep *newSubStep = new KPrAnimationSubStep();

               // Add substep after original one
               qDebug() << "substeps at step" << item->animation()->step()->animationCount();
               item->animation()->step()->insertAnimation(currentSubStepIndex+1, newSubStep);

               //reparent children
               connect(item, SIGNAL(triggerEventChanged(KPrShapeAnimation::Node_Type)), this, SLOT(notifyAnimationEdited()));
               qDebug() << "reparent main" << item->animationName();
               if (currentIndex < childrenCount-1) {
                   foreach (KPrCustomAnimationItem *subItem, parent->childrenAt(currentIndex + 1, childrenCount - 1)) {
                       if ((subItem->triggerEvent() == KPrShapeAnimation::After_Previous)) {
                           qDebug() << "break";
                           break;
                       }
                       qDebug() << "reparent" << subItem->animationName();
                       movedChildren.append(subItem->animation());
                       connect(subItem, SIGNAL(triggerEventChanged(KPrShapeAnimation::Node_Type)), this, SLOT(notifyAnimationEdited()));
                   }
               }
               command =
                       new KPrAnimationEditNodeTypeCommand(item->animation(), item->animation()->step(),
                                                           newSubStep, type, movedChildren);
               success = true;
           }
           // With Previous to On Click
           if (type == KPrShapeAnimation::On_Click) {
                qDebug() << "new type is On Click";
                KPrCustomAnimationItem *parent = item->parent();
                int currentIndex = parent->children().indexOf(item);
                int childrenCount = parent->childCount();
                qDebug() << "Current Index: " << currentIndex << childrenCount;
                // Get index of current substep
                int currentSubStepIndex = item->animation()->step()->indexOfAnimation(item->animation()->subStep());
                int subStepCount = item->animation()->step()->animationCount();

                qDebug() << "currentSubStepIndex: " << currentSubStepIndex << subStepCount;

                //Create new step to reparent currrent item and all following items.
                KPrAnimationStep *newStep = new KPrAnimationStep();

                //Create new substep to reparent currrent item and all following items.
                KPrAnimationSubStep *newSubStep = new KPrAnimationSubStep();

                // Add step after original one
                KPrPage *activePage = dynamic_cast<KPrPage *>(m_view->activePage());
                if (!activePage) {
                    qDebug() << "active page missing";
                    return false;
                }

                //insert new Step
                int currentStepIndex = activePage->animationSteps().indexOf(item->animation()->step());
                qDebug() << "currentStepIndex: " << currentStepIndex << activePage->animationSteps().count();
                activePage->animations().insertStep(currentStepIndex + 1, newStep);
                qDebug() << activePage->animationSteps().count();

                //reparent children
                connect(item, SIGNAL(triggerEventChanged(KPrShapeAnimation::Node_Type)), this, SLOT(initializeNewParent()));
                qDebug() << "reparent main" << item->animationName();

                if (currentSubStepIndex < subStepCount-1) {
                    for (int i =currentSubStepIndex + 1; i < subStepCount; i++) {
                        if (KPrAnimationSubStep *substep = dynamic_cast<KPrAnimationSubStep *>(item->animation()->step()->animationAt(i))) {
                           movedSubSteps.append(substep);
                        }
                    }

                }

                if (currentIndex < childrenCount-1) {
                    foreach (KPrCustomAnimationItem *subItem, parent->childrenAt(currentIndex + 1, childrenCount - 1)) {
                        if ((subItem->triggerEvent() == KPrShapeAnimation::After_Previous)) {
                            qDebug() << "break";
                            break;
                        }

                        qDebug() << "reparent" << subItem->animationName();
                        movedChildren.append(subItem->animation());
                    }
                }

                command =
                        new KPrAnimationEditNodeTypeCommand(item->animation(), newStep, newSubStep,
                                                            type, movedChildren, movedSubSteps, activePage);
                success = true;
           }
        }
        if (item->triggerEvent() == KPrShapeAnimation::On_Click) {
             qDebug() << "Item is On click";
             KPrCustomAnimationItem *parent = item->parent();
             qDebug() << "parent" << parent->animationName();
             if (parent->children().indexOf(item) < 2) {
                 qDebug() << "is first item";
                 // Resync trigger event edit widget
                 emit layoutChanged();
                 return false;
             }
            // On click to With Previous
            if (type == KPrShapeAnimation::With_Previous) {
                 qDebug() << "new type is Wit previous";
                int currentIndex = parent->children().indexOf(item);
                qDebug() << "Current Index: " << currentIndex << parent->childCount();
                qDebug() << "parent: " << parent->animationName() << parent->isRootAnimation();
                // Get previous animation
                KPrCustomAnimationItem *previousItemParent = parent->childAt(currentIndex - 1);
                KPrCustomAnimationItem *previousItem = previousItemParent;
                if (!previousItemParent->children().isEmpty()) {
                    previousItem = previousItemParent->children().last();
                }
                qDebug() << "Previous item" << previousItem->animationName();
                KPrAnimationStep *newStep = previousItem->animation()->step();
                KPrAnimationSubStep *newSubStep = previousItem->animation()->subStep();

                foreach (KPrCustomAnimationItem *subItem, item->children()) {
                    if ((subItem->triggerEvent() == KPrShapeAnimation::After_Previous)) {
                        qDebug() << "break";
                        break;
                    }
                    qDebug() << "reparent" << subItem->animationName();
                    movedChildren.append(subItem->animation());
                }
                int subStepCount = item->animation()->step()->animationCount();
                int currentSubStepIndex = item->animation()->step()->indexOfAnimation(item->animation()->subStep());
                qDebug() << "current sub step index" << currentSubStepIndex << subStepCount;
                if (subStepCount > 1) {
                    for (int i = currentSubStepIndex + 1; i < subStepCount; i++) {
                        qDebug() << "append substep " << i;
                        if (KPrAnimationSubStep *substep = dynamic_cast<KPrAnimationSubStep *>(item->animation()->step()->animationAt(i))) {
                           movedSubSteps.append(substep);
                        }
                    }

                }
                connect(item, SIGNAL(triggerEventChanged(KPrShapeAnimation::Node_Type)), this, SLOT(initializeNewParent()));
                command =
                        new KPrAnimationEditNodeTypeCommand(item->animation(), newStep, newSubStep,
                                                            type, movedChildren, movedSubSteps);
                success = true;
            }

            // On click to After Previous
            if (type == KPrShapeAnimation::After_Previous) {
                 qDebug() << "new type is After Previous";
                 int currentIndex = parent->children().indexOf(item);
                 qDebug() << "Current Index: " << currentIndex;
                 // Get previous animation
                 KPrCustomAnimationItem *previousItem = item->parent()->childAt(currentIndex - 1);
                 KPrAnimationStep *newStep = previousItem->animation()->step();

                 int subStepCount = item->animation()->step()->animationCount();
                 if (subStepCount > 1) {
                     for (int i = 1; i < subStepCount; i++) {
                         if (KPrAnimationSubStep *substep = dynamic_cast<KPrAnimationSubStep *>(item->animation()->step()->animationAt(i))) {
                            movedSubSteps.append(substep);
                         }
                     }

                 }
                 connect(item, SIGNAL(triggerEventChanged(KPrShapeAnimation::Node_Type)), this, SLOT(initializeNewParent()));
                 command =
                         new KPrAnimationEditNodeTypeCommand(item->animation(), newStep, item->animation()->subStep(),
                                                             type, movedChildren, movedSubSteps);
                 success = true;
            }
        }

        if (success) {
            KPrDocument *doc = dynamic_cast<KPrDocument*>(m_view->kopaDocument());
            doc->addCommand(command);
            return true;
        }

    }
    return false;
}

KPrCustomAnimationItem *KPrAnimationsDataModel::itemForIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        if (KPrCustomAnimationItem *item = static_cast<KPrCustomAnimationItem*>(
                index.internalPointer()))
            return item;
    }
    return m_rootItem;
}

qreal KPrAnimationsDataModel::previousItemEnd(const QModelIndex &index)
{
    if (m_rootItem && index.isValid()) {
        if (index.row() > 1) {
            KPrCustomAnimationItem *previousItem = m_rootItem->childAt(index.row() - 2);
            if (previousItem->triggerEvent() == KPrShapeAnimation::With_Previous) {
                //qDebug() << "return previous item end for " << index.row() << previousItem->durationSeconds() + previousItemBegin(indexByItem(previousItem)) << previousItem->animationName();
                return previousItem->durationSeconds() + previousItemBegin(indexByItem(previousItem));
            }
            else if (previousItem->triggerEvent() == KPrShapeAnimation::After_Previous) {
                //qDebug() << "return previous item end for " << index.row() << previousItem->durationSeconds() + previousItemEnd(indexByItem(previousItem)) << previousItem->animationName();
                return previousItem->durationSeconds() + previousItemEnd(indexByItem(previousItem));
            }
            else {
                //qDebug() << "return previous item end for " << index.row() << previousItem->durationSeconds() + previousItem->beginTime() << previousItem->animationName();
                return previousItem->durationSeconds() + previousItem->beginTime();
            }

        }
        else {
            //qDebug() << "root item end for" << index.row() << m_rootItem->durationSeconds() + m_rootItem->startTimeSeconds() << m_rootItem->animationName();
            return m_rootItem->durationSeconds() + m_rootItem->startTimeSeconds();
        }
    }
    return 0.0;
}

qreal KPrAnimationsDataModel::previousItemBegin(const QModelIndex &index)
{
    if (m_rootItem && index.isValid()) {
        if (index.row() > 1) {
            KPrCustomAnimationItem *previousItem = m_rootItem->childAt(index.row() - 2);
            if (previousItem->triggerEvent() == KPrShapeAnimation::With_Previous) {
                //qDebug() << "return previous item start for " << index.row() << previousItemBegin(indexByItem(previousItem)) << previousItem->animationName();
                return previousItemBegin(indexByItem(previousItem));
            }
            else if (previousItem->triggerEvent() == KPrShapeAnimation::After_Previous) {
                //qDebug() << "return previous item start for " << index.row() << previousItemEnd(indexByItem(previousItem)) << previousItem->animationName();
                return previousItemEnd(indexByItem(previousItem));
            }
            else {
                //qDebug() << "return previous item start for " << index.row() << previousItem->durationSeconds() + previousItem->beginTime() << previousItem->animationName();
                return previousItem->beginTime();
            }
        }
        else {
            //qDebug() << "root item start for" << index.row() << m_rootItem->startTimeSeconds() << m_rootItem->animationName();
            return m_rootItem->startTimeSeconds();
        }
    }
    return 0.0;
}

void KPrAnimationsDataModel::endTimeLineEdition()
{
    if (!m_firstEdition && m_currentEditedItem && (!m_currentEditedItem->isDefaulAnimation()) &&
            (m_oldBegin != INVALID) && (m_oldDuration != INVALID)) {
        int begin = m_currentEditedItem->beginTime();
        int duration = m_currentEditedItem->duration();
        if ((begin != m_oldBegin) || (duration != m_oldDuration)) {
            m_currentEditedItem->setStartTime(m_oldBegin);
            m_currentEditedItem->setDuration(m_oldDuration);
            setTimeRange(m_currentEditedItem, begin, duration);
        }
        m_oldBegin = INVALID;
        m_oldDuration = INVALID;
    }
    m_firstEdition = true;
    m_currentEditedItem = 0;
}

