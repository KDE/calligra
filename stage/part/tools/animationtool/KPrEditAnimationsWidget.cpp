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
    QLabel label(i18n("Manage time line delay and duration: "));
    layout->addWidget(&label);
    layout->addWidget(m_timeLineView);
    setLayout(layout);

    connect(m_timeLineView, SIGNAL(clicked(QModelIndex)), this, SIGNAL(itemClicked(QModelIndex)));
}

void KPrEditAnimationsWidget::setView(KoPAViewBase *view)
{
    qDebug() << "Set View";
    KPrView *n_view = dynamic_cast<KPrView *>(view);
    if (n_view) {
        m_view = n_view;
        m_timeLineModel->setDocumentView(m_view);
    }
    m_timeLineView->update();
}

void KPrEditAnimationsWidget::setParentItem(KPrCustomAnimationItem *item, KPrCustomAnimationItem *rootItem)
{
    qDebug() << "Set Parent Item";
    m_timeLineModel->setParentItem(item, rootItem);
    m_timeLineView->update();
}

void KPrEditAnimationsWidget::setCurrentIndex(const QModelIndex &index)
{
    qDebug() << "Set Current Item";
    if (!index.isValid()) {
        return;
    }
    KPrCustomAnimationItem *item = m_timeLineModel->itemForIndex(index);
    if (item) {
        // Change tree model index to time line index
        QModelIndex newIndex = m_timeLineModel->indexByItem(item);
        if (newIndex.isValid()) {
            m_timeLineView->setCurrentIndex(index);
        }
    }
    m_timeLineView->update();
}

void KPrEditAnimationsWidget::setActiveShape(KoShape *shape)
{
    qDebug() << "SetActiveShape";
    QModelIndex index = m_timeLineModel->indexByShape(shape);
    if (index.isValid()) {
        m_timeLineView->setCurrentIndex(index);
    }
    m_timeLineView->update();
}
