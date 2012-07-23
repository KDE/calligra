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
class KPrAnimationsTimeLineView;
class KPrViewModePreviewShapeAnimations;
class KPrEditAnimationsWidget;
class QModelIndex;
class KoPAPageBase;
class KoShape;
class DialogMenu;

/**
 * Shape animations docker widget: let's edition of animations.
 */
class KPrShapeAnimationDocker : public QWidget
{
    Q_OBJECT
public:
    explicit KPrShapeAnimationDocker(QWidget *parent = 0);
    void setView(KoPAViewBase *view);

    KPrShapeAnimations *mainModel();
    KPrViewModePreviewShapeAnimations *previewMode();
    void setPreviewMode(KPrViewModePreviewShapeAnimations *previewMode);

    KoShape *getSelectedShape();
    
signals:
    
public slots:
    /// Update widget with animations of the new active page
    void slotActivePageChanged();

    /// Update canvas with selected shape on Animations View
    void SyncWithAnimationsViewIndex(const QModelIndex &index);

    /// Update canvas with selected shape on Animations View
    void syncWithEditDialogIndex(const QModelIndex &index);

    void syncCanvasWithIndex(const QModelIndex &index);

    /// Update canvas with selected shape on Time Line View
    void updateEditDialogIndex(const QModelIndex &index);

    /// Update Time Line View with selected shape on canvas
    void syncWithCanvasSelectedShape();

    /// Plays a preview of the shape animation
    void slotAnimationPreview();

    void previewAnimation(KPrShapeAnimation *animation);

    /// Remove selected animations
    void slotRemoveAnimations();

    void checkAnimationSelected();

    void moveAnimationUp();
    void moveAnimationDown();

    void addNewAnimation(KPrShapeAnimation *animation);

private slots:
    void testEditPanelRoot();
    void showAnimationsCustomContextMenu(const QPoint &pos);
    void setTriggerEvent(QAction *action);
protected:
     bool eventFilter(QObject *ob, QEvent *ev);

private:

    KPrShapeAnimations *animationsByPage(KoPAPageBase *page);

    KPrView* m_view;
    QTreeView * m_animationsView;
    //KPrAnimationsTreeModel *m_animationsModel;
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
    
};

#endif // KPRSHAPEANIMATIONDOCKER_H
