/* This file is part of the KDE project
   Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

// Animations Collection based in ShapeCollectionDocker

#include "KPrEditAnimationsWidget.h"

//Stage Headers
#include "KPrShapeAnimationDocker.h"
#include "KPrView.h"
#include "animations/KPrShapeAnimation.h"
#include "KPrAnimationGroupProxyModel.h"
#include "KPrAnimationsTimeLineView.h"
#include "KPrAnimationSelectorWidget.h"
#include "KPrShapeAnimations.h"
#include "StageDebug.h"

//Qt Headers
#include <QToolButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTreeView>
#include <QMenu>
#include <QModelIndex>
#include <QComboBox>
#include <QTimeEdit>
#include <QTime>
#include <QTimer>
#include <QActionGroup>

//KF5 Headers
#include <klocalizedstring.h>

//Calligra Headers
#include <KoIcon.h>
#include <KoSelection.h>
#include <KoCanvasController.h>
#include <KoPAViewBase.h>

KPrEditAnimationsWidget::KPrEditAnimationsWidget(KPrShapeAnimationDocker *docker, QWidget *parent)
    : QWidget(parent)
    , m_view(0)
    , m_docker(docker)
{
    QVBoxLayout *layout = new QVBoxLayout;
    m_timeLineView = new KPrAnimationsTimeLineView();
    QLabel *label = new QLabel(i18n("Manage animation delay and duration: "));
    QLabel *startLabel = new QLabel(i18n("Start: "));
    m_triggerEventList = new QComboBox;
    m_triggerEventList->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_triggerEventList->setEditable(false);
    m_triggerEventList->addItem(koIcon("onclick"), i18n("on mouse click"));
    m_triggerEventList->addItem(koIcon("after_previous"), i18n("after previous animation"));
    m_triggerEventList->addItem(koIcon("with_previous"), i18n("with previous animation"));

    QLabel *delayLabel = new QLabel(i18n("Delay: "));
    m_delayEdit = new QTimeEdit;
    m_delayEdit->setTimeRange(QTime(0,0,0), QTime(0,30,0));
    m_delayEdit->setDisplayFormat("mm:ss.zzz");

    QLabel *durationLabel = new QLabel(i18n("Duration: "));
    m_durationEdit = new QTimeEdit;
    m_durationEdit->setTimeRange(QTime(0,0,0), QTime(1,0,0));
    m_durationEdit->setDisplayFormat("H:mm:ss.zzz");

    QToolButton *m_buttonPreviewAnimation = new QToolButton();
    m_buttonPreviewAnimation->setIcon(koIcon("media-playback-start"));
    m_buttonPreviewAnimation->setToolTip(i18n("Preview Shape Animation"));
    m_buttonPreviewAnimation->setEnabled(true);

    m_animationSelector = new KPrAnimationSelectorWidget(docker, docker->animationsLoader());

    // layout widgets
    layout->addWidget(m_animationSelector);

    QHBoxLayout *playLayout = new QHBoxLayout;
    playLayout->addWidget(label);
    playLayout->addStretch();
    playLayout->addWidget(m_buttonPreviewAnimation);
    layout->addLayout(playLayout);
    layout->addWidget(m_timeLineView);
    layout->addWidget(startLabel);
    layout->addWidget(m_triggerEventList);
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(delayLabel);
    hlayout->addWidget(m_delayEdit);
    hlayout->addWidget(durationLabel);
    hlayout->addWidget(m_durationEdit);
    layout->addLayout(hlayout);
    setLayout(layout);

    //Connect Signals.
    connect(m_buttonPreviewAnimation, SIGNAL(clicked()), this, SIGNAL(requestAnimationPreview()));
    connect(m_timeLineView, SIGNAL(clicked(QModelIndex)), this, SIGNAL(itemClicked(QModelIndex)));
    connect(m_timeLineView, SIGNAL(clicked(QModelIndex)), this, SLOT(updateIndex(QModelIndex)));
    connect(m_timeLineView, SIGNAL(timeValuesChanged(QModelIndex)), this, SLOT(updateIndex(QModelIndex)));
    connect(m_timeLineView, SIGNAL(layoutChanged()), this, SLOT(syncCurrentItem()));
    connect(m_delayEdit, SIGNAL(editingFinished()), this, SLOT(setBeginTime()));
    connect(m_durationEdit, SIGNAL(editingFinished()), this, SLOT(setDuration()));
    connect(m_triggerEventList, SIGNAL(currentIndexChanged(int)), this, SLOT(setTriggerEvent(int)));
    connect(m_animationSelector, SIGNAL(requestPreviewAnimation(KPrShapeAnimation*)),
            docker, SLOT(previewAnimation(KPrShapeAnimation*)));
    connect(m_animationSelector, SIGNAL(requestAcceptAnimation(KPrShapeAnimation*)),
            this, SLOT(changeCurrentAnimation(KPrShapeAnimation*)));
    connect(m_timeLineView, SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(showTimeLineCustomContextMenu(QPoint)));
    connect(m_animationSelector, SIGNAL(previousStateChanged(bool)), this, SIGNAL(previousStateChanged(bool)));
    QTimer::singleShot(700, this, SLOT(initializeView()));
}

KPrEditAnimationsWidget::~KPrEditAnimationsWidget()
{
}

void KPrEditAnimationsWidget::setView(KoPAViewBase *view)
{
    KPrView *n_view = dynamic_cast<KPrView *>(view);
    if (n_view) {
        m_view = n_view;
    }
}

void KPrEditAnimationsWidget::setCurrentIndex(const QModelIndex &index)
{
    Q_ASSERT(m_timeLineModel);
    m_timeLineView->setCurrentIndex(index);
    updateIndex(index);
}

void KPrEditAnimationsWidget::setProxyModel(KPrAnimationGroupProxyModel *model)
{
    m_timeLineModel = model;
    m_timeLineView->setModel(m_timeLineModel);
}

void KPrEditAnimationsWidget::updateView()
{
    m_timeLineView->update();
    updateGeometry();
}

QModelIndex KPrEditAnimationsWidget::currentIndex()
{
    return m_timeLineView->currentIndex();
}

void KPrEditAnimationsWidget::updateIndex(const QModelIndex &index)
{
    if (index.isValid() && (index.row() == m_timeLineView->currentIndex().row())) {
        QModelIndex triggerIndex = m_timeLineModel->index(index.row(), KPrShapeAnimations::NodeType);
        QModelIndex beginTimeIndex = m_timeLineModel->index(index.row(), KPrShapeAnimations::StartTime);
        QModelIndex durationIndex = m_timeLineModel->index(index.row(), KPrShapeAnimations::Duration);
        m_triggerEventList->setCurrentIndex(m_timeLineModel->data(triggerIndex).toInt());
        m_delayEdit->setTime(QTime().addMSecs(m_timeLineModel->data(beginTimeIndex).toInt()));
        m_durationEdit->setTime(QTime().addMSecs(m_timeLineModel->data(durationIndex).toInt()));
    }
}

void KPrEditAnimationsWidget::setBeginTime()
{
    if (m_timeLineView->currentIndex().isValid()) {
        m_docker->mainModel()->setBeginTime(m_timeLineModel->mapToSource(m_timeLineView->currentIndex()), -m_delayEdit->time().msecsTo(QTime()));
    }
}

void KPrEditAnimationsWidget::setDuration()
{
    if (m_timeLineView->currentIndex().isValid()) {
        m_docker->mainModel()->setDuration(m_timeLineModel->mapToSource(m_timeLineView->currentIndex()), -m_durationEdit->time().msecsTo(QTime()));
    }
}

void KPrEditAnimationsWidget::setTriggerEvent(int row)
{
    QModelIndex index = m_timeLineView->currentIndex();
    if ((row >= 0) && index.isValid()) {
        QModelIndex triggerIndex = m_timeLineModel->index(index.row(), KPrShapeAnimations::NodeType);
        if (row != m_timeLineModel->data(triggerIndex).toInt()) {
            KPrShapeAnimation::NodeType newType;
            if (row == 0) newType = KPrShapeAnimation::OnClick;
            else if (row == 1) newType = KPrShapeAnimation::AfterPrevious;
            else newType = KPrShapeAnimation::WithPrevious;
            m_docker->mainModel()->setTriggerEvent(m_timeLineModel->mapToSource(m_timeLineView->currentIndex()), newType);
        }
    }
}

void KPrEditAnimationsWidget::setTriggerEvent(QAction *action)
{
    int row = action->data().toInt();
    setTriggerEvent(row);
}

void KPrEditAnimationsWidget::syncCurrentItem()
{
    QModelIndex index = m_timeLineView->currentIndex();
    if (index.isValid()) {
        updateIndex(index);
    }
}

void KPrEditAnimationsWidget::showTimeLineCustomContextMenu(const QPoint &pos)
{
    if (m_timeLineView->currentIndex().isValid()) {
        QMenu menu(m_timeLineView);
        QModelIndex index = m_timeLineView->currentIndex();
        // get animation data
        QModelIndex triggerIndex = m_timeLineModel->index(index.row(), KPrShapeAnimations::NodeType);
        KPrShapeAnimation::NodeType currentType = static_cast <KPrShapeAnimation::NodeType>(m_timeLineModel->data(triggerIndex).toInt());

        // Populate context menu
        QActionGroup *actionGroup = new QActionGroup(m_timeLineView);
        actionGroup->setExclusive(true);
        QAction *onClickAction = new QAction(koIcon("onclick"), i18n("start on mouse click"), m_timeLineView);
        onClickAction->setCheckable(true);
        onClickAction->setData(KPrShapeAnimation::OnClick);
        QAction *afterAction = new QAction(koIcon("after_previous"), i18n("start after previous animation"), m_timeLineView);
        afterAction->setCheckable(true);
        afterAction->setData(KPrShapeAnimation::AfterPrevious);
        QAction *withAction = new QAction(koIcon("with_previous"), i18n("start with previous animation"), m_timeLineView);
        withAction->setCheckable(true);
        withAction->setData(KPrShapeAnimation::WithPrevious);

        actionGroup->addAction(onClickAction);
        actionGroup->addAction(afterAction);
        actionGroup->addAction(withAction);
        actionGroup->setExclusive(true);

        // Select current nodetype
        if (currentType == KPrShapeAnimation::OnClick) {
            onClickAction->setChecked(true);
        }
        else if (currentType == KPrShapeAnimation::AfterPrevious) {
            afterAction->setChecked(true);
        }
        else {
            withAction->setChecked(true);
        }

        menu.addAction(onClickAction);
        menu.addAction(afterAction);
        menu.addAction(withAction);
        connect(actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(setTriggerEvent(QAction*)));
        menu.exec(m_timeLineView->mapToGlobal(pos));
    }
}

void KPrEditAnimationsWidget::changeCurrentAnimation(KPrShapeAnimation *animation)
{
    QModelIndex itemIndex = m_timeLineModel->mapToSource(m_timeLineView->currentIndex());
    KPrShapeAnimation *currentAnimation = m_docker->mainModel()->animationByRow(itemIndex.row());
    if (!itemIndex.isValid() || !animation || (animation->shape() != currentAnimation->shape())) {
        return;
    }
    //if it's the same animation do anything
    if ((currentAnimation->id() == animation->id()) && (currentAnimation->presetSubType() == animation->presetSubType())) {
        return;
    }
    m_docker->mainModel()->replaceAnimation(itemIndex, animation);
}

void KPrEditAnimationsWidget::initializeView()
{
    m_animationSelector->init();
}
