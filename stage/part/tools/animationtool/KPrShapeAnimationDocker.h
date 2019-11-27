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

#ifndef KPRSHAPEANIMATIONDOCKER_H
#define KPRSHAPEANIMATIONDOCKER_H

#include <QWidget>
#include <animations/KPrShapeAnimation.h>

class QToolButton;
class KoPAViewBase;
class KPrView;
class QTreeView;
class KPrAnimationGroupProxyModel;
class KPrShapeAnimations;
class KPrViewModePreviewShapeAnimations;
class KPrEditAnimationsWidget;
class QModelIndex;
class KoPAPageBase;
class KoShape;
class DialogMenu;
class KPrPredefinedAnimationsLoader;
class KPrAnimationSelectorWidget;

/**
 * Shape animations docker widget: let's edition of animations.
 */
class KPrShapeAnimationDocker : public QWidget
{
    Q_OBJECT
public:
    explicit KPrShapeAnimationDocker(QWidget *parent = 0);
    ~KPrShapeAnimationDocker() override;
    void setView(KoPAViewBase *view);

    /**
      * Return the Animations data Model
      *
      * @return a KPrShapeAnimations pointer
      */
    KPrShapeAnimations *mainModel();

    /**
      * Return the preview animations view mode
      *
      * @return a KPrViewModePreviewShapeAnimations pointer
      */
    KPrViewModePreviewShapeAnimations *previewMode();

    /**
     * Set the current preview animations view mode
     * @param previewMode pointer to the new KPrViewModePreviewShapeAnimations class
     */
    void setPreviewMode(KPrViewModePreviewShapeAnimations *previewMode);

    /**
      * Return the predefined animations loader class
      *
      * @return a KPrPredefinedAnimationsLoader pointer
      */
    KPrPredefinedAnimationsLoader *animationsLoader();

    /**
      * Return the current selected shape (the animation selected on animations list
      * view if the shape has an animation or just the selected shape on canvas)
      *
      * @return a KPrPredefinedAnimationsLoader pointer
      */
    KoShape *getSelectedShape();

Q_SIGNALS:
    /// emitted if the configuration for automatic preview is changed
    void previousStateChanged(bool isEnabled);
    /// emitted if an animation of the given shape is changed
    void shapeAnimationsChanged(KoShape *shape);
    /// emitted if a motion path animation is added or removed
    void motionPathAddedRemoved();

public Q_SLOTS:
    /**
     * @brief Update widget with animations of the new active page
     */
    void slotActivePageChanged();

    /**
     * @brief Update canvas with shape of selected animation on Animations View
     * @param index of selected animation
     */
    void SyncWithAnimationsViewIndex(const QModelIndex &index);

    /**
     * @brief Update canvas with shape of animation selected on edition view
     * @param index of selected animation on edition view
     */
    void syncWithEditDialogIndex(const QModelIndex &index);

    /**
     * @brief Update canvas with the given index (of the main model)
     * @param index of animation
     */
    void syncCanvasWithIndex(const QModelIndex &index);

    /**
     * @brief Update canvas with selected shape on Time Line View
     * @param index (main model index)
     */
    void updateEditDialogIndex(const QModelIndex &index);

    /**
     * @brief Update Time Line View with selected shape on canvas
     */
    void syncWithCanvasSelectedShape();

    /**
     * @brief Plays a preview of the shape animation
     */
    void slotAnimationPreview();

    /**
     * @brief Play animation preview for the given animation
     * @param animation to be played
     */
    void previewAnimation(KPrShapeAnimation *animation);

    /**
     * @brief Remove selected animations
     */
    void slotRemoveAnimations();

    /**
     * @brief Check type of animation
     */
    void checkAnimationSelected();

    /**
     * @brief slot for moving current animation up on animation view
     */
    void moveAnimationUp();

    /**
     * @brief slot for moving current animation down on animation view
     */
    void moveAnimationDown();

    /**
     * @brief add new animation
     * @param animation to be added
     */
    void addNewAnimation(KPrShapeAnimation *animation);

    /**
     * @brief Verify if animations on given index range are motion paths
     * @param index where range to be verified starts
     * @param indexEnd where range to be verified ends
     */
    void verifyMotionPathChanged(const QModelIndex &index, const QModelIndex &indexEnd);

private Q_SLOTS:
    /// Test if the current on click animation displayed as root in edit panel has changed
    void testEditPanelRoot();
    /// Display context menu
    void showAnimationsCustomContextMenu(const QPoint &pos);
    /// Change current animation Node Type
    void setTriggerEvent(QAction *action);
    /// load predefined animations
    void initializeView();
protected:
     bool eventFilter(QObject *ob, QEvent *ev) override;

private:
    /// Return a pointer to the KPrShapeAnimations for the given page
    KPrShapeAnimations *animationsByPage(KoPAPageBase *page);

    KPrView *m_view;
    QTreeView *m_animationsView;
    KPrShapeAnimations *m_animationsModel;
    KPrAnimationGroupProxyModel *m_animationGroupModel;
    QToolButton *m_editAnimation;
    KPrEditAnimationsWidget *m_editAnimationsPanel;
    QToolButton *m_buttonAddAnimation;
    QToolButton *m_buttonRemoveAnimation;
    QToolButton *m_buttonAnimationOrderUp;
    QToolButton *m_buttonAnimationOrderDown;
    QToolButton *m_buttonPreviewAnimation;
    KPrViewModePreviewShapeAnimations *m_previewMode;
    KoShape *m_lastSelectedShape;
    DialogMenu *m_addMenu;
    DialogMenu *m_editMenu;
    KPrPredefinedAnimationsLoader *m_animationsData;
    KPrAnimationSelectorWidget *m_addDialog;
};

#endif // KPRSHAPEANIMATIONDOCKER_H
