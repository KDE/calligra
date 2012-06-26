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
    m_editAnimationsPanel = new KPrEditAnimationsWidget;
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
    m_buttonAnimationOrderUp->setIcon(SmallIcon("arrow-down"));
    m_buttonAnimationOrderUp->setToolTip(i18n("Move animation down"));
    m_buttonAnimationOrderUp->setEnabled(false);

    m_buttonAnimationOrderDown = new QToolButton();
    m_buttonAnimationOrderDown->setIcon(SmallIcon("arrow-up"));
    m_buttonAnimationOrderDown->setToolTip(i18n("Move animation up"));
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


    //load View and model
    m_animationsModel = new KPrAnimationsTreeModel(this);
    m_animationsView = new QTreeView();
    m_animationsView->setAllColumnsShowFocus(true);
    m_animationsView->setModel(m_animationsModel);

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
        m_animationsModel->setDocumentView(m_view);
        m_editAnimationsPanel->setView(m_view);
        slotActivePageChanged();
        connect(m_view->proxyObject, SIGNAL(activePageChanged()),
                 this, SLOT(slotActivePageChanged()));
        connect(m_animationsView, SIGNAL(clicked(QModelIndex)), this, SLOT(SyncWithAnimationsViewIndex(QModelIndex)));
        connect(m_animationsView, SIGNAL(clicked(QModelIndex)), this, SLOT(updateEditDialogIndex(QModelIndex)));
        connect(m_editAnimationsPanel, SIGNAL(itemClicked(QModelIndex)), this, SLOT(syncWithEditDialogIndex(QModelIndex)));
        connect(m_editAnimationsPanel, SIGNAL(requestAnimationPreview()), this, SLOT(slotAnimationPreview()));
        connect(m_animationsModel, SIGNAL(rootChanged()), this, SLOT(checkAnimationSelected()));
    }
}

void KPrShapeAnimationDocker::checkAnimationSelected()
{
    QModelIndex index = m_animationsView->currentIndex();
    KPrCustomAnimationItem *item = itemByIndex(index);
    if (item && (!item->isDefaulAnimation())) {
        m_buttonRemoveAnimation->setEnabled(true);
        m_editAnimation->setEnabled(true);
        return;
    }
m_buttonRemoveAnimation->setEnabled(false);
m_editAnimation->setEnabled(false);
}

void KPrShapeAnimationDocker::slotActivePageChanged()
{
    Q_ASSERT( m_view );
    KPrPage *page = dynamic_cast<KPrPage*>(m_view->activePage());
    if (page) {
        m_animationsModel->setActivePage(page);
        m_animationsView->update();
        m_animationsView->setColumnWidth(1, KIconLoader::SizeMedium + 6);
        m_animationsView->setColumnWidth(2, KIconLoader::SizeSmall + 6);
        m_editAnimationsPanel->setParentItem(0, m_animationsModel->rootItem());
    }
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    connect(selection, SIGNAL(selectionChanged()), this, SLOT(syncWithCanvasSelectedShape()));
    checkAnimationSelected();
}

void KPrShapeAnimationDocker::SyncWithAnimationsViewIndex(const QModelIndex &index)
{
    syncCanvasWithIndex(index);
    m_editAnimationsPanel->setCurrentIndex(index);
    checkAnimationSelected();
}

void KPrShapeAnimationDocker::syncWithEditDialogIndex(const QModelIndex &index)
{
    syncCanvasWithIndex(index);
    KPrCustomAnimationItem *item = itemByIndex(index);
    if (item) {
        // Change time line index to tree model index
        QModelIndex newIndex = m_animationsModel->indexByItem(item);
        if (newIndex.isValid()) {
            m_animationsView->setCurrentIndex(index);
        }
    }
    checkAnimationSelected();
}

void KPrShapeAnimationDocker::syncCanvasWithIndex(const QModelIndex &index)
{
    KoShape *shape = itemByIndex(index)->shape();
    if (!shape) {
        return;
    }

    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    foreach (KoShape* shape, selection->selectedShapes()) {
        shape->update();
    }
    selection->deselectAll();
    selection->select(shape);
    selection->update();
    shape->update();
    checkAnimationSelected();
}

KPrCustomAnimationItem *KPrShapeAnimationDocker::itemByIndex(const QModelIndex &index)
{
    //Check if index is valid and it contains a shape with an animation
    if (!index.isValid()) {
        return 0;
    }
    //Update canvas with selected shape on Time Line View
    Q_ASSERT(index.internalPointer());

    KPrCustomAnimationItem *shapeAnimation = static_cast< KPrCustomAnimationItem*>(index.internalPointer());
    if (!shapeAnimation) {
        return 0;
    }
    return shapeAnimation;

}

void KPrShapeAnimationDocker::updateEditDialogIndex(const QModelIndex &index)
{
    //Update canvas with selected shape on Time Line View
    Q_ASSERT(index.internalPointer());

    //Check if index is valid and it contains a shape with an animation
    if (!index.isValid()) {
        return;
    }
    KPrCustomAnimationItem *rootAnimation = m_animationsModel->rootItem();
    KPrCustomAnimationItem *shapeAnimation = static_cast< KPrCustomAnimationItem*>(index.internalPointer());

    if (!shapeAnimation || !rootAnimation) {
        return;
    }

    if (shapeAnimation->parent() == rootAnimation) {
        m_editAnimationsPanel->setParentItem(shapeAnimation, rootAnimation);
        m_editAnimationsPanel->setCurrentIndex(index);
    }
    else if (shapeAnimation->parent()->parent() == rootAnimation) {
        m_editAnimationsPanel->setParentItem(shapeAnimation->parent(), rootAnimation);
        m_editAnimationsPanel->setCurrentIndex(index);
    }
    else {
        return;
    }
}

void KPrShapeAnimationDocker::syncWithCanvasSelectedShape()
{
    //Update View with selected shape on canvas
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    if (!selection->selectedShapes().isEmpty()) {
        if (selection->selectedShapes().first()) {
            KoShape *selectedShape = selection->selectedShapes().first();
            QModelIndex index = m_animationsModel->indexByShape(selectedShape);
            if (index.isValid()) {
                m_animationsView->setCurrentIndex(index);
                updateEditDialogIndex(index);
            }
            m_editAnimationsPanel->setActiveShape(selectedShape);
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
    KPrCustomAnimationItem *shapeAnimation = static_cast< KPrCustomAnimationItem*>(index.internalPointer());
    if (!shapeAnimation) {
        return;
    }

    if(!m_previewMode) {
        m_previewMode = new KPrViewModePreviewShapeAnimations(m_view, m_view->kopaCanvas());
    }

    m_previewMode->setShapeAnimation(shapeAnimation->animation());
    m_view->setViewMode(m_previewMode); // play the effect (it reverts to normal  when done)
}

void KPrShapeAnimationDocker::slotRemoveAnimations()
{
    QModelIndex index = m_animationsView->currentIndex();
    m_animationsModel->removeItemByIndex(index);
}
