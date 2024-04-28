/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2008 Peter Penz <peter.penz19@gmail.com>
 * SPDX-FileCopyrightText: 2011 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOVIEWITEMCONTEXTBAR_H
#define KOVIEWITEMCONTEXTBAR_H

#include "kowidgets_export.h"
#include <QModelIndex>
#include <QObject>

class QAbstractItemView;
class QToolButton;
class QHBoxLayout;
class QRect;

/**
 * @brief Add context buttons to items of QAbstractView subclasses
 *
 * Whenever an item is hovered by the mouse, a toggle button is shown
 * which allows to select/deselect the current item, other buttons for
 * custom actions could be added using addContextButton method.
 */
class KOWIDGETS_EXPORT KoViewItemContextBar : public QObject
{
    Q_OBJECT

public:
    explicit KoViewItemContextBar(QAbstractItemView *parent);
    ~KoViewItemContextBar() override = default;
    bool eventFilter(QObject *watched, QEvent *event) override;

    /**
     * Add a button to the context bar
     * @param text to be used for button tool tip
     * @param iconName or name of the icon displayed on the button
     * @return a QToolButton, so it could be connected to a slot.
     */
    QToolButton *addContextButton(const QString &text, const QString &iconName);
    // Returns the index of the item under the mouse cursor
    QModelIndex currentIndex();

    int preferredWidth();
    void setShowSelectionToggleButton(bool enabled);

Q_SIGNALS:
    /** Is emitted if the selection has been changed by the toggle button. */
    void selectionChanged();

public Q_SLOTS:
    /** Hide context bar */
    void reset();
    void enableContextBar();
    void disableContextBar();
    /** Update Bar */
    void update();

private Q_SLOTS:
    void slotEntered(const QModelIndex &index);
    void slotViewportEntered();
    void setItemSelected();
    /** Hide context bar if the selectem item has been removed */
    void slotRowsRemoved(const QModelIndex &parent, int start, int end);
    /** Updates context bar buttons state*/
    void updateHoverUi(const QModelIndex &index);
    void showContextBar(const QRect &rect);
    /** Updates Selection Button state*/
    void updateToggleSelectionButton();
    /** Called when model resets */
    void slotModelReset();

private:
    QAbstractItemView *m_view;
    bool m_enabled;
    QModelIndex m_IndexUnderCursor;
    QWidget *m_ContextBar;
    QToolButton *m_ToggleSelectionButton;
    QHBoxLayout *m_Layout;
    QList<QToolButton *> m_contextBarButtons;
    bool m_showToggleButton;
};

#endif // KOVIEWITEMCONTEXTBAR_H
