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

class QListWidget;
class QToolButton;
class KoPAViewBase;
class KPrView;
class QTreeView;
class KPrAnimationGroupProxyModel;
class KPrShapeAnimations;
class KPrAnimationsDataModel;
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
     * @param pointer to the new KPrViewModePreviewShapeAnimations class
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
    
signals:
    /// emited if the configuration for automatic preview is changed
    void previousStateChanged(bool isEnabled);
    /// emited if an animation of the given shape is changed
    void shapeAnimationsChanged(KoShape *shape);
    /// emited if a motion path animation is added or removed
    void motionPathAddedRemoved();
    
public slots:
    /// Update widget with animations of the new active page
    void slotActivePageChanged();

    /// Update canvas with selected shape on Animations View
    void SyncWithAnimationsViewIndex(const QModelIndex &index);

    /// Update canvas with selected shape on Animations View
    void syncWithEditDialogIndex(const QModelIndex &index);

    /// Update canvas with the given index (of the main model)
    void syncCanvasWithIndex(const QModelIndex &index);

    /// Update canvas with selected shape on Time Line View
    void updateEditDialogIndex(const QModelIndex &index);

    /// Update Time Line View with selected shape on canvas
    void syncWithCanvasSelectedShape();

    /// Plays a preview of the shape animation
    void slotAnimationPreview();

    /// Play animation preview for the given animation
    void previewAnimation(KPrShapeAnimation *animation);

    /// Remove selected animations
    void slotRemoveAnimations();

    /// Check type of animation
    void checkAnimationSelected();

    /// slot for moving animations up or down buttons
    void moveAnimationUp();
    void moveAnimationDown();

    /// slot for add animation button
    void addNewAnimation(KPrShapeAnimation *animation);

    /// Verify if animations on given index range are motion paths
    void verifyMotionPathChanged(const QModelIndex &index, const QModelIndex &indexEnd);

private slots:
    /// Test if the current on click animation displayed as root in edit panel has changed
    void testEditPanelRoot();
    /// Display context menu
    void showAnimationsCustomContextMenu(const QPoint &pos);
    /// Change current animation Node Type
    void setTriggerEvent(QAction *action);
    /// load predefined animations
    void initializeView();
protected:
     bool eventFilter(QObject *ob, QEvent *ev);

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
