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

#include "KPrEditAnimationsWidget.h"

//Stage Headers
#include "KPrShapeAnimationDocker.h"
#include "KPrAnimationsTreeModel.h"
#include "KPrPage.h"
#include "KPrView.h"
#include "KPrDocument.h"
#include "KPrViewModePreviewShapeAnimations.h"
#include "animations/KPrShapeAnimation.h"
#include "KPrCustomAnimationItem.h"
#include "KPrEditAnimationsWidget.h"

#include "KPrAnimationsTimeLineView.h"
#include "KPrAnimationsDataModel.h"
#include "commands/KPrAnimationRemoveCommand.h"

//Qt Headers
#include <QToolButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTreeView>
#include <QMenu>
#include <QModelIndex>
#include <QDebug>
#include <QComboBox>
#include <QTimeEdit>

//KDE Headers
#include <KIcon>
#include <KLocale>
#include <KIconLoader>

//Calligra Headers
#include <KoToolManager.h>
#include <KoSelection.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoShapeManager.h>
#include <KoPAViewBase.h>
#include <KoPACanvasBase.h>




KPrEditAnimationsWidget::KPrEditAnimationsWidget(QWidget *parent)
    : QWidget(parent)
    , m_view(0)
{
    QVBoxLayout *layout = new QVBoxLayout;
    m_timeLineModel = new KPrAnimationsDataModel(this);
    m_timeLineView = new KPrAnimationsTimeLineView();
    m_timeLineView->setModel(m_timeLineModel);
    QLabel *label = new QLabel(i18n("Manage animation delay and duration: "));
    QLabel *startLabel = new QLabel(i18n("Start: "));
    m_triggerEventList = new QComboBox;
    m_triggerEventList->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_triggerEventList->setEditable(false);
    m_triggerEventList->addItem(KIcon("onclick"), i18n("on mouse click"));
    m_triggerEventList->addItem(KIcon("after_previous"), i18n("after previous animation"));
    m_triggerEventList->addItem(KIcon("with_previous"), i18n("with previous animation"));

    QLabel *delayLabel = new QLabel(i18n("Delay: "));
    m_delayEdit = new QTimeEdit;
    m_delayEdit->setTimeRange(QTime(0,0,0), QTime(0,30,0));
    m_delayEdit->setDisplayFormat("mm:ss.zzz");

    QLabel *durationLabel = new QLabel(i18n("Duration: "));
    m_durationEdit = new QTimeEdit;
    m_durationEdit->setTimeRange(QTime(0,0,0), QTime(1,0,0));
    m_durationEdit->setDisplayFormat("H:mm:ss.zzz");

    QToolButton *m_buttonPreviewAnimation = new QToolButton();
    m_buttonPreviewAnimation->setIcon(SmallIcon("media-playback-start"));
    m_buttonPreviewAnimation->setToolTip(i18n("Preview Shape Animation"));
    m_buttonPreviewAnimation->setEnabled(true);

    layout->addWidget(m_buttonPreviewAnimation);
    layout->addWidget(label);
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
}

void KPrEditAnimationsWidget::setView(KoPAViewBase *view)
{
    KPrView *n_view = dynamic_cast<KPrView *>(view);
    if (n_view) {
        m_view = n_view;
        m_timeLineModel->setDocumentView(m_view);
    }
    m_timeLineView->update();
}

void KPrEditAnimationsWidget::setParentItem(KPrCustomAnimationItem *item, KPrCustomAnimationItem *rootItem)
{
    m_timeLineModel->setParentItem(item, rootItem);
    m_timeLineView->update();
}

void KPrEditAnimationsWidget::setCurrentIndex(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    KPrCustomAnimationItem *item = m_timeLineModel->itemForIndex(index);
    if (item) {
        // Change tree model index to time line index
        QModelIndex newIndex = m_timeLineModel->indexByItem(item);
        if (newIndex.isValid()) {
            m_timeLineView->setCurrentIndex(index);
            updateIndex(index);
        }
    }
    m_timeLineView->update();
}

void KPrEditAnimationsWidget::setActiveShape(KoShape *shape)
{
    QModelIndex index = m_timeLineModel->indexByShape(shape);
    if (index.isValid()) {
        m_timeLineView->setCurrentIndex(index);
    }
    m_timeLineView->update();
}

void KPrEditAnimationsWidget::updateIndex(const QModelIndex &index)
{
    KPrCustomAnimationItem *item = m_timeLineModel->itemForIndex(index);
    if (item) {
        m_triggerEventList->setCurrentIndex((int)item->triggerEvent());
        m_delayEdit->setTime(QTime().addMSecs(item->startTime()));
        m_durationEdit->setTime(QTime().addMSecs(item->duration()));
    }
}
