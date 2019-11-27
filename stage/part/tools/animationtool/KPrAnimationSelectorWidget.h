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


#ifndef KPRANIMATIONSELECTORWIDGET_H
#define KPRANIMATIONSELECTORWIDGET_H

#include <QWidget>

class QModelIndex;
class QListWidget;
class QListView;
class KPrShapeAnimation;
class QListWidgetItem;
class KPrShapeAnimationDocker;
class KPrPredefinedAnimationsLoader;
class QToolButton;
class KoViewItemContextBar;
class QCheckBox;

/** Widget used to select predefined animations. Holds three views: categories, animations
    and subtypes (optional). This widget is used to add a new animation or change the type
    of an existing animation */
class KPrAnimationSelectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KPrAnimationSelectorWidget(KPrShapeAnimationDocker *docker, KPrPredefinedAnimationsLoader *animationsData,
                                        QWidget *parent = 0);
    ~KPrAnimationSelectorWidget() override;

    /**
     * @brief Loads data from data model (call this method before use the widget)
     */
    void init();

Q_SIGNALS:
    /// this signal is emitted when an animation is hovered
    void requestPreviewAnimation(KPrShapeAnimation *animation);
    /// this signal is emitted when an animation is selected
    void requestAcceptAnimation(KPrShapeAnimation *animation);
    /// signal emitted if automatic preview checkbox state is changed
    void previousStateChanged(bool state);

private Q_SLOTS:
    /**
     * @brief Request animation preview for the animation on index
     *
     * @param index of the animation to be displayed
     */
    void automaticPreviewRequested(const QModelIndex &index);

    /**
     * @brief Request animation preview for current animation
     * (animation selected on collection view)
     *
     */
    void automaticPreviewRequested();

    /**
     * @brief Changes the current shape collection
     */
    void activateShapeCollection(QListWidgetItem *item);

    /**
     * @brief Called if an animation was selected
     *
     * @param index of the animation selected
     */
    void setAnimation(const QModelIndex& index);

    /**
     * @brief Automatic preview checkbox state has changed
     *
     * @param true if automatic preview is going to be enabled
     */
    void setPreviewState(bool isEnable);

private:
    /// load / save automatic preview checkbox state
    bool loadPreviewConfig();
    void savePreviewConfig();
    /// Create on hover buttons for animations view
    void createCollectionContextBar();
    /// Create on hover buttons for subtype view
    void createSubTypeContextBar();

    QListWidget *m_collectionChooser;
    QListView *m_collectionView;
    QListView *m_subTypeView;
    KPrShapeAnimationDocker *m_docker;
    KPrShapeAnimation *m_previewAnimation;
    bool m_showAutomaticPreview;
    KPrPredefinedAnimationsLoader *m_animationsData;
    KoViewItemContextBar *m_collectionContextBar;
    QToolButton *m_collectionPreviewButton;
    KoViewItemContextBar *m_subTypeContextBar;
    QToolButton *m_subTypePreviewButton;
    QCheckBox *m_previewCheckBox;
};

#endif // KPRANIMATIONSELECTORWIDGET_H
