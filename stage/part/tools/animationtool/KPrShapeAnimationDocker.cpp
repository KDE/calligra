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
#include "KPrEditAnimationsWidget.h"
#include "KPrAnimationGroupProxyModel.h"
#include "KPrAnimationSelectorWidget.h"
#include "KPrPredefinedAnimationsLoader.h"

//Qt Headers
#include <QToolButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTreeView>
#include <QHeaderView>
#include <QMenu>
#include <QActionGroup>
#include <QTimer>

//KF5 Headers
#include <klocalizedstring.h>
#include <kiconloader.h>

//Calligra Headers
#include <KoIcon.h>
#include <KoToolManager.h>
#include <KoSelection.h>
#include <KoCanvasController.h>
#include <KoShapeManager.h>
#include <KoPAViewBase.h>
#include <KoPACanvasBase.h>
#include <KoPAPageBase.h>

//This class is needed so that the menu returns a sizehint based on the layout and not on the number (0) of menu items
class DialogMenu : public QMenu
{
    public:
        DialogMenu(QWidget *parent = 0);
        QSize sizeHint() const override;
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
    , m_animationGroupModel(0)
    , m_previewMode(0)
    , m_lastSelectedShape(0)
{
    setObjectName("KPrShapeAnimationDocker");

    // load predefined animations data
    m_animationsData = new KPrPredefinedAnimationsLoader(this);

    QHBoxLayout *hlayout = new QHBoxLayout;
    QHBoxLayout *hlayout2 = new QHBoxLayout;

    //Setup buttons
    m_editAnimation = new QToolButton();
    m_editAnimation->setText(i18n("Edit animation"));
    m_editAnimation->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_editAnimation->setIcon(koIcon("edit_animation"));
    m_editAnimation->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_editAnimation->setToolTip(i18n("Edit animation"));
    m_editAnimation->setEnabled(false);
    hlayout->addWidget(m_editAnimation);
    hlayout->addStretch();

    m_editMenu = new DialogMenu(this);
    m_editAnimationsPanel = new KPrEditAnimationsWidget(this);
    QGridLayout *containerLayout = new QGridLayout(m_editMenu);
    containerLayout->addWidget(m_editAnimationsPanel,0,0);
    m_editAnimation->setMenu(m_editMenu);
    m_editAnimation->setPopupMode(QToolButton::InstantPopup);

    m_buttonAddAnimation = new QToolButton();
    m_buttonAddAnimation->setIcon(koIcon("list-add"));
    m_buttonAddAnimation->setToolTip(i18n("Add new animation"));

    m_addMenu = new DialogMenu(this);
    m_addDialog = new KPrAnimationSelectorWidget(this, m_animationsData);
    QGridLayout *addMenuLayout = new QGridLayout(m_addMenu);
    addMenuLayout->addWidget(m_addDialog,0,0);
    m_buttonAddAnimation->setMenu(m_addMenu);
    m_buttonAddAnimation->setPopupMode(QToolButton::InstantPopup);


    m_buttonRemoveAnimation = new QToolButton();
    m_buttonRemoveAnimation->setIcon(koIcon("list-remove"));
    m_buttonRemoveAnimation->setEnabled(false);
    m_buttonRemoveAnimation->setToolTip(i18n("Remove animation"));
    hlayout->addWidget(m_buttonAddAnimation);
    hlayout->addWidget(m_buttonRemoveAnimation);

    QLabel *orderLabel = new QLabel(i18n("Order: "));
    m_buttonAnimationOrderUp = new QToolButton();
    m_buttonAnimationOrderUp->setIcon(koIcon("arrow-up"));
    m_buttonAnimationOrderUp->setToolTip(i18n("Move animation up"));
    m_buttonAnimationOrderUp->setEnabled(false);

    m_buttonAnimationOrderDown = new QToolButton();
    m_buttonAnimationOrderDown->setIcon(koIcon("arrow-down"));
    m_buttonAnimationOrderDown->setToolTip(i18n("Move animation down"));
    m_buttonAnimationOrderDown->setEnabled(false);

    m_buttonPreviewAnimation = new QToolButton();
    m_buttonPreviewAnimation->setIcon(koIcon("media-playback-start"));
    m_buttonPreviewAnimation->setToolTip(i18n("Preview Shape Animation"));
    m_buttonPreviewAnimation->setEnabled(false);

    hlayout2->addWidget(m_buttonPreviewAnimation);
    hlayout2->addStretch();
    hlayout2->addWidget(orderLabel);
    hlayout2->addWidget(m_buttonAnimationOrderUp);
    hlayout2->addWidget(m_buttonAnimationOrderDown);

    //load View and model
    m_animationsView = new QTreeView();
    m_animationsView->setAllColumnsShowFocus(true);
    m_animationsView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_animationsView->installEventFilter(this);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(hlayout);
    layout->addWidget(m_animationsView);
    layout->addLayout(hlayout2);
    setLayout(layout);

    //Connect Signals.
    connect(m_buttonPreviewAnimation, SIGNAL(clicked()), this, SLOT(slotAnimationPreview()));
    connect(m_buttonRemoveAnimation, SIGNAL(clicked()), this, SLOT(slotRemoveAnimations()));
    connect(m_buttonAnimationOrderUp, SIGNAL(clicked()), this, SLOT(moveAnimationUp()));
    connect(m_buttonAnimationOrderDown, SIGNAL(clicked()), this, SLOT(moveAnimationDown()));
    connect(m_animationsView, SIGNAL(doubleClicked(QModelIndex)), m_editAnimation, SLOT(showMenu()));
    connect(m_animationsView, SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(showAnimationsCustomContextMenu(QPoint)));
    connect(m_addDialog, SIGNAL(requestPreviewAnimation(KPrShapeAnimation*)),
            this, SLOT(previewAnimation(KPrShapeAnimation*)));
    connect(m_addDialog, SIGNAL(requestAcceptAnimation(KPrShapeAnimation*)),
            this, SLOT(addNewAnimation(KPrShapeAnimation*)));
    connect(m_addDialog, SIGNAL(previousStateChanged(bool)), this, SIGNAL(previousStateChanged(bool)));
    connect(m_editAnimationsPanel, SIGNAL(previousStateChanged(bool)), this, SIGNAL(previousStateChanged(bool)));
    QTimer::singleShot(500, this, SLOT(initializeView()));

}

KPrShapeAnimationDocker::~KPrShapeAnimationDocker()
{
    delete m_animationGroupModel;
}

void KPrShapeAnimationDocker::setView(KoPAViewBase *view)
{
    KPrView *n_view = dynamic_cast<KPrView *>(view);
    if (n_view) {
        m_view = n_view;
        //load model
        slotActivePageChanged();
        m_editAnimationsPanel->setView(m_view);
        connect(m_animationsView, SIGNAL(clicked(QModelIndex)), this, SLOT(SyncWithAnimationsViewIndex(QModelIndex)));
        connect(m_animationsView, SIGNAL(clicked(QModelIndex)), this, SLOT(updateEditDialogIndex(QModelIndex)));
        connect(m_editAnimationsPanel, SIGNAL(itemClicked(QModelIndex)), this, SLOT(syncWithEditDialogIndex(QModelIndex)));
        connect(m_editAnimationsPanel, SIGNAL(requestAnimationPreview()), this, SLOT(slotAnimationPreview()));
    }
}

void KPrShapeAnimationDocker::checkAnimationSelected()
{
    KoCanvasController *canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    // If a shape is selected enable add animation button
    if (selection->selectedShapes().isEmpty() || !(selection->selectedShapes().first()->isPrintable())) {
        m_buttonAddAnimation->setEnabled(false);
    }
    else {
        m_buttonAddAnimation->setEnabled(true);
    }

    // If a valid animation is selected on main view, enable edition buttons
    QModelIndex index = m_animationsView->currentIndex();
    if (index.isValid()) {
        m_buttonAddAnimation->setEnabled(true);
        m_buttonRemoveAnimation->setEnabled(true);
        m_editAnimation->setEnabled(true);
        m_buttonAnimationOrderUp->setEnabled(true);
        m_buttonAnimationOrderDown->setEnabled(true);
        m_buttonPreviewAnimation->setEnabled(true);
        return;
    }
    m_buttonRemoveAnimation->setEnabled(false);
    m_editAnimation->setEnabled(false);
    m_buttonAnimationOrderUp->setEnabled(false);
    m_buttonAnimationOrderDown->setEnabled(false);
    m_buttonPreviewAnimation->setEnabled(false);
}

void KPrShapeAnimationDocker::moveAnimationUp()
{
    QModelIndex index = m_animationsView->currentIndex();
    m_animationsModel->moveUp(index);
    m_animationsView->setCurrentIndex(m_animationsModel->index(index.row() - 1, 0));
}

void KPrShapeAnimationDocker::moveAnimationDown()
{
    QModelIndex index = m_animationsView->currentIndex();
    m_animationsModel->moveDown(index);
    m_animationsView->setCurrentIndex(m_animationsModel->index(index.row() + 1, 0));
}

void KPrShapeAnimationDocker::addNewAnimation(KPrShapeAnimation *animation)
{
    if (!animation || !animation->shape()) {
        return;
    }
    QModelIndex index = m_animationsView->currentIndex();
    m_animationsModel->insertNewAnimation(animation, index);
    m_animationsView->setCurrentIndex(m_animationsModel->indexByAnimation(animation));
    m_addMenu->hide();
}

void KPrShapeAnimationDocker::verifyMotionPathChanged(const QModelIndex &index, const QModelIndex &indexEnd)
{
    Q_UNUSED(indexEnd);
    if (index.isValid()) {
        KPrShapeAnimation *animation = m_animationsModel->animationByRow(index.row());
        if (animation->presetClass() == KPrShapeAnimation::MotionPath) {
            emit motionPathAddedRemoved();
            return;
        }
        emit shapeAnimationsChanged(animation->shape());
    }
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
        m_animationsView->setModel(m_animationsModel);
        //Config columns
        m_animationsView->setColumnWidth(1, 3);
        m_animationsView->resizeColumnToContents(KPrShapeAnimations::ShapeThumbnail);
        m_animationsView->header()->moveSection(4, 3);
        m_animationsView->hideColumn(0);
        m_animationsView->hideColumn(5);
        m_animationsView->hideColumn(6);
        m_animationsView->hideColumn(7);
        m_animationsView->hideColumn(8);
        m_animationsView->hideColumn(9);

        if (!m_animationGroupModel) {
            m_animationGroupModel = new KPrAnimationGroupProxyModel;
        }
        m_animationGroupModel->setSourceModel(m_animationsModel);
        m_editAnimationsPanel->setProxyModel(m_animationGroupModel);

        m_animationsView->setColumnWidth(1, KIconLoader::SizeMedium + 6);
        m_animationsView->setColumnWidth(2, KIconLoader::SizeSmall + 6);
    }
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    connect(selection, SIGNAL(selectionChanged()), this, SLOT(syncWithCanvasSelectedShape()));
    connect(m_animationsModel, SIGNAL(onClickEventChanged()), this, SLOT(testEditPanelRoot()));
    connect(m_animationsModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(verifyMotionPathChanged(QModelIndex,QModelIndex)));
    connect(m_animationsModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SIGNAL(motionPathAddedRemoved()));
    connect(m_animationsModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SIGNAL(motionPathAddedRemoved()));
    getSelectedShape();
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
    if (!index.isValid())  {
        return;
    }
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
            m_animationsView->setCurrentIndex(index);
            if (index.isValid()) {
                if (m_animationGroupModel->setCurrentIndex(index)) {
                    m_editAnimationsPanel->updateView();
                }
            }
            updateEditDialogIndex(index);
        }
        checkAnimationSelected();
    }
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

    if (!m_previewMode) {
        m_previewMode = new KPrViewModePreviewShapeAnimations(m_view, m_view->kopaCanvas());
    }
    m_previewMode->setShapeAnimation(shapeAnimation);
    m_view->setViewMode(m_previewMode); // play the effect (it reverts to normal  when done)
}

void KPrShapeAnimationDocker::previewAnimation(KPrShapeAnimation *animation)
{
    if (!animation) {
        return;
    }
    QModelIndex index = m_animationsView->currentIndex();
    index = m_animationsModel->index(index.row(), index.column(), index.parent());
    KPrShapeAnimation *currentAnimation = m_animationsModel->animationByRow(index.row());

    if (currentAnimation && (currentAnimation->shape() == animation->shape())) {
        animation->setTextBlockUserData(currentAnimation->textBlockUserData());
    }

    if (!previewMode()) {
        setPreviewMode(new KPrViewModePreviewShapeAnimations(m_view, m_view->kopaCanvas()));
    }
    previewMode()->setShapeAnimation(animation);
    m_view->setViewMode(previewMode()); // play the effect (it reverts to normal  when done)
}

void KPrShapeAnimationDocker::slotRemoveAnimations()
{
    QModelIndex index = m_animationsView->currentIndex();
    m_animationsModel->removeAnimationByIndex(index);
    syncCanvasWithIndex(index);
}

KPrShapeAnimations *KPrShapeAnimationDocker::mainModel()
{
    return m_animationsModel;
}

KPrViewModePreviewShapeAnimations *KPrShapeAnimationDocker::previewMode()
{
    return m_previewMode;
}

void KPrShapeAnimationDocker::setPreviewMode(KPrViewModePreviewShapeAnimations *previewMode)
{
    m_previewMode = previewMode;
}

KPrPredefinedAnimationsLoader *KPrShapeAnimationDocker::animationsLoader()
{
    return m_animationsData;
}

KoShape *KPrShapeAnimationDocker::getSelectedShape()
{
    KoCanvasController* canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    // Sync canvas with main view
    if (m_animationsView->currentIndex().isValid()) {
        syncCanvasWithIndex(m_animationsView->currentIndex());
    }
    // If a shape is already selected return it
    else if (!selection->selectedShapes().isEmpty()) {
        m_lastSelectedShape = selection->selectedShapes().first();
        return selection->selectedShapes().first();
    }
    // Restore last selected shape after an animation preview
    else if (m_lastSelectedShape) {
        foreach (KoShape* shape, selection->selectedShapes()) {
            shape->update();
        }
        selection->deselectAll();
        if (canvasController->canvas()->shapeManager()->shapes().contains(m_lastSelectedShape)) {
            selection->select(m_lastSelectedShape);
            selection->update();
            m_lastSelectedShape->update();
            if (selection->selectedShapes().contains(m_lastSelectedShape)) {
                return m_lastSelectedShape;
            }
        }
        m_lastSelectedShape = 0;
    }
    else if (!(canvasController->canvas()->shapeManager()->shapes().isEmpty())){
        foreach (KoShape* shape, selection->selectedShapes()) {
            shape->update();
        }
        selection->deselectAll();
        selection->select(canvasController->canvas()->shapeManager()->shapes().last());
        selection->update();
        m_view->kopaCanvas()->shapeManager()->shapes().first()->update();
    }
    //Return current shape selected on canvas
    if (!selection->selectedShapes().isEmpty()) {
        if (selection->selectedShapes().first()) {
            KoShape *selectedShape = selection->selectedShapes().first();
            m_lastSelectedShape = selectedShape;
            return selectedShape;
        }
    }
    return 0;
}

void KPrShapeAnimationDocker::testEditPanelRoot()
{
    QModelIndex editPanelIndex = m_animationGroupModel->mapToSource(m_editAnimationsPanel->currentIndex());
    // Check if current root item on edit view is still valid
    if (!editPanelIndex.isValid()) {
        editPanelIndex = m_animationsView->currentIndex();
    }
    m_animationGroupModel->setCurrentIndex(editPanelIndex);
    m_animationGroupModel->forceUpdateModel();
    m_editAnimationsPanel->updateView();
    updateEditDialogIndex(editPanelIndex);
}

void KPrShapeAnimationDocker::showAnimationsCustomContextMenu(const QPoint &pos)
{
    QMenu menu(m_animationsView);
    menu.addAction(koIcon("document-new"), i18n("Add a new animation"), m_buttonAddAnimation, SLOT(showMenu()));
    menu.addAction(koIcon("edit-delete"), i18n("Delete current animation"), this, SLOT(slotRemoveAnimations()));
    menu.addAction(koIcon("edit_animation"), i18n("Edit animation"), m_editAnimation, SLOT(showMenu()));
    menu.addSeparator();
    if ((m_animationsView->selectionModel()->selectedRows().count() == 1) &&
            (m_animationsView->currentIndex().isValid())) {
        QActionGroup *actionGroup = new QActionGroup(m_animationsView);
        actionGroup->setExclusive(true);
        QAction *onClickAction = new QAction(koIcon("onclick"), i18n("start on mouse click"), m_animationsView);
        onClickAction->setCheckable(true);
        onClickAction->setData(KPrShapeAnimation::OnClick);
        QAction *afterAction = new QAction(koIcon("after_previous"), i18n("start after previous animation"), m_animationsView);
        afterAction->setCheckable(true);
        afterAction->setData(KPrShapeAnimation::AfterPrevious);
        QAction *withAction = new QAction(koIcon("with_previous"), i18n("start with previous animation"), m_animationsView);
        withAction->setCheckable(true);
        withAction->setData(KPrShapeAnimation::WithPrevious);

        actionGroup->addAction(onClickAction);
        actionGroup->addAction(afterAction);
        actionGroup->addAction(withAction);
        actionGroup->setExclusive(true);

        KPrShapeAnimation::NodeType currentNodeType = m_animationsModel->triggerEventByIndex(m_animationsView->currentIndex());
        if (currentNodeType == KPrShapeAnimation::OnClick) {
            onClickAction->setChecked(true);
        }
        else if (currentNodeType == KPrShapeAnimation::AfterPrevious) {
            afterAction->setChecked(true);
        }
        else {
            withAction->setChecked(true);
        }

        menu.addAction(onClickAction);
        menu.addAction(afterAction);
        menu.addAction(withAction);
        connect(actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(setTriggerEvent(QAction*)));
    }
    menu.exec(m_animationsView->mapToGlobal(pos));
}

void KPrShapeAnimationDocker::setTriggerEvent(QAction *action)
{
    if (!m_animationsView->currentIndex().isValid())
        return;

    int row = action->data().toInt();
    QModelIndex triggerIndex = m_animationsModel->index(m_animationsView->currentIndex().row(),
                                                        KPrShapeAnimations::NodeType);
    if (row != m_animationsModel->data(triggerIndex).toInt()) {
        KPrShapeAnimation::NodeType newType;
        if (row == 0) newType = KPrShapeAnimation::OnClick;
        else if (row == 1) newType = KPrShapeAnimation::AfterPrevious;
        else newType = KPrShapeAnimation::WithPrevious;
        m_animationsModel->setTriggerEvent(m_animationsView->currentIndex(), newType);
    }
}

void KPrShapeAnimationDocker::initializeView()
{
    m_addDialog->init();
}

bool KPrShapeAnimationDocker::eventFilter(QObject *ob, QEvent *ev)
{
    if (ob == m_animationsView && ev->type() == QEvent::KeyPress) {
        if (QKeyEvent *keyEvent = static_cast<QKeyEvent*>(ev)) {
            if (keyEvent->key() == Qt::Key_Delete) {
                slotRemoveAnimations();
            }
        }
    }
    return QWidget::eventFilter(ob, ev);
}
