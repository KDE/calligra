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

#include "KPrShapeAnimationDocker.h"

//Stage Headers
#include "KPrPage.h"
#include "KPrView.h"
#include "KPrDocument.h"
#include "KPrViewModePreviewShapeAnimations.h"
#include "KPrCustomAnimationItem.h"
#include "KPrEditAnimationsWidget.h"
#include "KPrAnimationsTimeLineView.h"
#include "KPrAnimationGroupProxyModel.h"

//Qt Headers
#include <QToolButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTreeView>
#include <QMenu>
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
#include <KoPAPageBase.h>

//This class is needed so that the menu returns a sizehint based on the layout and not on the number (0) of menu items
class DialogMenu : public QMenu
{
    public:
        DialogMenu(QWidget * parent = 0);
        virtual QSize sizeHint() const;
};

DialogMenu::DialogMenu(QWidget * parent)
 : QMenu(parent)
{
}
QSize DialogMenu::sizeHint() const
{
    return layout()->sizeHint();
}

KPrShapeAnimationDocker::KPrShapeAnimationDocker(QWidget *parent)
    : QWidget(parent)
    , m_view(0)
    , m_previewMode(0)
{
    setObjectName("KPrShapeAnimationDocker");
    QHBoxLayout *hlayout = new QHBoxLayout;
    QHBoxLayout *hlayout2 = new QHBoxLayout;

    //Setup buttons
    m_editAnimation = new QToolButton();
    m_editAnimation->setText(i18n("Edit animation"));
    m_editAnimation->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_editAnimation->setIconSize(QSize(KIconLoader::SizeSmallMedium, KIconLoader::SizeSmallMedium));
    m_editAnimation->setIcon(KIcon("edit_animation"));
    m_editAnimation->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_editAnimation->setToolTip(i18n("Edit animation"));
    m_editAnimation->setEnabled(false);
    hlayout->addWidget(m_editAnimation);
    hlayout->addStretch();

    //TODO: Implement Edition Features
    DialogMenu *editMenu = new DialogMenu(this);
    m_editAnimationsPanel = new KPrEditAnimationsWidget(this);
    QGridLayout *containerLayout = new QGridLayout(editMenu);
    containerLayout->addWidget(m_editAnimationsPanel,0,0);
    m_editAnimation->setMenu(editMenu);
    m_editAnimation->setPopupMode(QToolButton::InstantPopup);

    m_buttonAddAnimation = new QToolButton();
    m_buttonAddAnimation->setIcon(SmallIcon("list-add", KIconLoader::SizeSmallMedium));
    m_buttonAddAnimation->setToolTip(i18n("Add new animation"));
    m_buttonAddAnimation->setEnabled(false);

    m_buttonRemoveAnimation = new QToolButton();
    m_buttonRemoveAnimation->setIcon(SmallIcon("list-remove", KIconLoader::SizeSmallMedium));
    m_buttonRemoveAnimation->setEnabled(false);
    m_buttonRemoveAnimation->setToolTip(i18n("Remove animation"));
    hlayout->addWidget(m_buttonAddAnimation);
    hlayout->addWidget(m_buttonRemoveAnimation);

    QLabel *orderLabel = new QLabel(i18n("Order: "));
    m_buttonAnimationOrderUp = new QToolButton();
    m_buttonAnimationOrderUp->setIcon(SmallIcon("arrow-up"));
    m_buttonAnimationOrderUp->setToolTip(i18n("Move animation up"));
    m_buttonAnimationOrderUp->setEnabled(false);

    m_buttonAnimationOrderDown = new QToolButton();
    m_buttonAnimationOrderDown->setIcon(SmallIcon("arrow-down"));
    m_buttonAnimationOrderDown->setToolTip(i18n("Move animation down"));
    m_buttonAnimationOrderDown->setEnabled(false);

    m_buttonPreviewAnimation = new QToolButton();
    m_buttonPreviewAnimation->setIcon(SmallIcon("media-playback-start"));
    m_buttonPreviewAnimation->setToolTip(i18n("Preview Shape Animation"));
    m_buttonPreviewAnimation->setEnabled(true);

    hlayout2->addWidget(m_buttonPreviewAnimation);
    hlayout2->addStretch();
    hlayout2->addWidget(orderLabel);
    hlayout2->addWidget(m_buttonAnimationOrderUp);
    hlayout2->addWidget(m_buttonAnimationOrderDown);

    //Connect Signals.
    connect(m_buttonPreviewAnimation, SIGNAL(clicked()), this, SLOT(slotAnimationPreview()));
    connect(m_buttonRemoveAnimation, SIGNAL(clicked()), this, SLOT(slotRemoveAnimations()));
    connect(m_buttonAnimationOrderUp, SIGNAL(clicked()), this, SLOT(moveAnimationUp()));
    connect(m_buttonAnimationOrderDown, SIGNAL(clicked()), this, SLOT(moveAnimationDown()));

    //load View and model
    m_animationsView = new QTreeView();
    m_animationsView->setAllColumnsShowFocus(true);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(hlayout);
    layout->addWidget(m_animationsView);
    layout->addLayout(hlayout2);
    setLayout(layout);

}

void KPrShapeAnimationDocker::setView(KoPAViewBase *view)
{
    KPrView *n_view = dynamic_cast<KPrView *>(view);
    if (n_view) {
        m_view = n_view;    
        //load model
        slotActivePageChanged();
        m_editAnimationsPanel->setView(m_view);
        connect(m_view->proxyObject, SIGNAL(activePageChanged()),
                 this, SLOT(slotActivePageChanged()));
        connect(m_animationsView, SIGNAL(clicked(QModelIndex)), this, SLOT(SyncWithAnimationsViewIndex(QModelIndex)));
        connect(m_animationsView, SIGNAL(clicked(QModelIndex)), this, SLOT(updateEditDialogIndex(QModelIndex)));
        connect(m_editAnimationsPanel, SIGNAL(itemClicked(QModelIndex)), this, SLOT(syncWithEditDialogIndex(QModelIndex)));
        connect(m_editAnimationsPanel, SIGNAL(requestAnimationPreview()), this, SLOT(slotAnimationPreview()));
        //connect(m_animationsModel, SIGNAL(rootChanged()), this, SLOT(checkAnimationSelected()));
        //connect(m_editAnimationsPanel, SIGNAL(rootRemoved()), this, SLOT(reParentEditDialog()));
    }
}

void KPrShapeAnimationDocker::checkAnimationSelected()
{
    QModelIndex index = m_animationsView->currentIndex();
    if (index.isValid()) {
        m_buttonRemoveAnimation->setEnabled(true);
        m_editAnimation->setEnabled(true);
        QModelIndex triggerEventIndex = m_animationsModel->index(index.row(), KPrShapeAnimations::Node_Type);
        if (static_cast<KPrShapeAnimation::Node_Type>(m_animationsModel->data(triggerEventIndex).toInt()) ==
                KPrShapeAnimation::On_Click) {
            m_buttonAnimationOrderUp->setEnabled(true);
            m_buttonAnimationOrderDown->setEnabled(true);
        }
        else {
            m_buttonAnimationOrderUp->setEnabled(false);
            m_buttonAnimationOrderDown->setEnabled(false);
        }
        return;
    }
    m_buttonRemoveAnimation->setEnabled(false);
    m_editAnimation->setEnabled(false);
}

void KPrShapeAnimationDocker::moveAnimationUp()
{
    QModelIndex index = m_animationsView->currentIndex();
    m_animationsModel->moveUp(index);
}

void KPrShapeAnimationDocker::moveAnimationDown()
{
    QModelIndex index = m_animationsView->currentIndex();
    m_animationsModel->moveDown(index);
}

void KPrShapeAnimationDocker::setBeginTime(const QModelIndex &index, const int begin)
{
    m_animationsModel->setBeginTime(index, begin);
}

void KPrShapeAnimationDocker::setDuration(const QModelIndex &index, const int duration)
{
    m_animationsModel->setDuration(index, duration);
}

bool KPrShapeAnimationDocker::setTriggerEvent(const QModelIndex &index, const KPrShapeAnimation::Node_Type type)
{
    return m_animationsModel->setTriggerEvent(index, type);
}

KPrShapeAnimations *KPrShapeAnimationDocker::animationsByPage(KoPAPageBase *page)
{
    KPrPageData * pageData = dynamic_cast<KPrPageData *>(page);
    Q_ASSERT(pageData);
    return &pageData->animations();
}


void KPrShapeAnimationDocker::slotActivePageChanged()
{
    Q_ASSERT( m_view );
    if (m_view->activePage()) {
        m_animationsModel = animationsByPage(m_view->activePage());
        KPrDocument *doc = dynamic_cast<KPrDocument *>(m_view->kopaDocument());
        Q_ASSERT(doc);
        m_animationsModel->setDocument(doc);
        m_animationsView->setModel(m_animationsModel);
        //Config columns
        m_animationsView->hideColumn(0);
        m_animationsView->setColumnWidth(1, 3);
        m_animationsView->hideColumn(5);
        m_animationsView->hideColumn(6);
        m_animationsView->hideColumn(7);
        m_animationsView->hideColumn(8);
        m_animationsView->hideColumn(9);

        m_animationGroupModel = new KPrAnimationGroupProxyModel;
        m_animationGroupModel->setSourceModel(m_animationsModel);
        m_editAnimationsPanel->setProxyModel(m_animationGroupModel);

        m_animationsView->setColumnWidth(1, KIconLoader::SizeMedium + 6);
        m_animationsView->setColumnWidth(2, KIconLoader::SizeSmall + 6);
    }
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    connect(selection, SIGNAL(selectionChanged()), this, SLOT(syncWithCanvasSelectedShape()));
    checkAnimationSelected();
}

void KPrShapeAnimationDocker::SyncWithAnimationsViewIndex(const QModelIndex &index)
{
    syncCanvasWithIndex(index);
    if (m_animationGroupModel->setCurrentIndex(index)) {
        m_editAnimationsPanel->updateView();
    }
    checkAnimationSelected();
}

void KPrShapeAnimationDocker::syncWithEditDialogIndex(const QModelIndex &index)
{
    QModelIndex newIndex = m_animationGroupModel->mapToSource(index);
    syncCanvasWithIndex(newIndex);
    m_animationsView->setCurrentIndex(newIndex);
    checkAnimationSelected();
}

void KPrShapeAnimationDocker::syncCanvasWithIndex(const QModelIndex &index)
{
    KoShape *shape = m_animationsModel->shapeByIndex(index);
    if (!shape) {
        return;
    }

    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    // Don't update if shape is already selected
    if (selection->selectedShapes().contains(shape)) {
        return;
    }

    foreach (KoShape* shape, selection->selectedShapes()) {
        shape->update();
    }

    selection->deselectAll();
    selection->select(shape);
    selection->update();
    shape->update();
    checkAnimationSelected();
}

void KPrShapeAnimationDocker::updateEditDialogIndex(const QModelIndex &index)
{
    //Sync selected animation in Animations View with time line View
    QModelIndex newIndex = m_animationGroupModel->mapFromSource(index);
    m_editAnimationsPanel->setCurrentIndex(newIndex);

}

void KPrShapeAnimationDocker::syncWithCanvasSelectedShape()
{
    //Update View with selected shape on canvas
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    if (!selection->selectedShapes().isEmpty()) {
        if (selection->selectedShapes().first()) {
            KoShape *selectedShape = selection->selectedShapes().first();
            QModelIndex currentIndex = m_animationsView->currentIndex();
            if (currentIndex.isValid()) {
                KoShape *currentSelectedShape = m_animationsModel->shapeByIndex(currentIndex);
                if (currentSelectedShape == selectedShape) {
                    return;
                }
            }
            QModelIndex index = m_animationsModel->indexByShape(selectedShape);
            if (index.isValid()) {
                m_animationsView->setCurrentIndex(index);
                if (m_animationGroupModel->setCurrentIndex(index)) {
                    m_editAnimationsPanel->updateView();
                }
                updateEditDialogIndex(index);
            }
        }
    }
    checkAnimationSelected();
}

void KPrShapeAnimationDocker::slotAnimationPreview()
{
    QModelIndex index = m_animationsView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    // Sometimes index is not updated fast enough
    index = m_animationsModel->index(index.row(), index.column(), index.parent());

    KPrShapeAnimation *shapeAnimation = m_animationsModel->animationByRow(index.row());
    if (!shapeAnimation) {
        return;
    }

    if(!m_previewMode) {
        m_previewMode = new KPrViewModePreviewShapeAnimations(m_view, m_view->kopaCanvas());
    }

    m_previewMode->setShapeAnimation(shapeAnimation);
    m_view->setViewMode(m_previewMode); // play the effect (it reverts to normal  when done)
}

void KPrShapeAnimationDocker::slotRemoveAnimations()
{
    QModelIndex index = m_animationsView->currentIndex();
    m_animationsModel->removeItemByIndex(index);
}

KPrShapeAnimations *KPrShapeAnimationDocker::mainModel()
{
    return m_animationsModel;
}
