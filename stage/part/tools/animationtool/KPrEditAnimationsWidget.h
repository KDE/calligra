/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Paul Mendez <paulestebanms@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPREDITANIMATIONSWIDGET_H
#define KPREDITANIMATIONSWIDGET_H

#include <QWidget>

class KPrAnimationsTimeLineView;
class KPrView;
class KoPAViewBase;
class QModelIndex;
class QModelIndex;
class QComboBox;
class QTimeEdit;
class KPrShapeAnimation;
class KPrAnimationGroupProxyModel;
class KPrShapeAnimationDocker;
class KPrAnimationSelectorWidget;

/** This Widget holds the configuration popup dialog
    used to edit animation properties of existing or new
    animation.
    Animations on this view are displayed in groups that
    have as parent animations started by a click*/
class KPrEditAnimationsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KPrEditAnimationsWidget(KPrShapeAnimationDocker *docker, QWidget *parent = nullptr);
    ~KPrEditAnimationsWidget() override;

    void setView(KoPAViewBase *view);

    /**
     * @brief Verify if the parent (on click animation) for the given index has changed
     * If it has changed update the view
     *
     * @param index of the current animation
     */
    void setCurrentIndex(const QModelIndex &index);

    /**
     * @brief Set proxy model for animations Time line View
     *
     * @param model KPrAniamtionGroupProxyModel to be used as base model for the
     *        time line view
     */
    void setProxyModel(KPrAnimationGroupProxyModel *model);

    /**
     * @brief Update time line view
     */
    void updateView();

    /**
     * @brief Return current parent animation index (On click animation)
     * The index belongs to the proxy model
     */
    QModelIndex currentIndex();

Q_SIGNALS:
    /// emitted ed if an item on Time line view has been clicked
    void itemClicked(const QModelIndex &index);
    /// Request animation preview for the current index
    void requestAnimationPreview();
    /// Emitted if user has changed the state of automatic preview check box
    void previousStateChanged(bool isEnabled);

public Q_SLOTS:
    /**
     * @brief Sync node type and time combo boxes with the given index
     * @param index of the animation
     */
    void updateIndex(const QModelIndex &index);

protected Q_SLOTS:
    /**
     * @brief Change current animation begin time
     */
    void setBeginTime();

    /**
     * @brief Change current animation duration
     */
    void setDuration();

    /**
     * @brief Change current animation node type
     */
    void setTriggerEvent(int row);

    /**
     * @brief call update index
     */
    void syncCurrentItem();

    /**
     * @brief Helper method to set new node type
     * @param action that call the slot
     */
    void setTriggerEvent(QAction *action);

    /**
     * @brief Display context menu for the time line view
     * @param pos position where the context menu need to be displayed
     */
    void showTimeLineCustomContextMenu(const QPoint &pos);

    /**
     * @brief Use to sync with animations selected on main view or canvas
     * @param animation to be used as current animation
     */
    void changeCurrentAnimation(KPrShapeAnimation *animation);

    /**
     * @brief load predefined animations
     */
    void initializeView();

private:
    KPrView *m_view;
    KPrAnimationsTimeLineView *m_timeLineView;
    KPrAnimationGroupProxyModel *m_timeLineModel;
    QComboBox *m_triggerEventList;
    QTimeEdit *m_delayEdit;
    QTimeEdit *m_durationEdit;
    KPrShapeAnimationDocker *m_docker;
    KPrAnimationSelectorWidget *m_animationSelector;
};

#endif // KPREDITANIMATIONSWIDGET_H
