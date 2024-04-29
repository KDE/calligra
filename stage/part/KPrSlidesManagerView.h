/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRSLIDESMANAGERVIEW_H
#define KPRSLIDESMANAGERVIEW_H

#include <QListView>

/**
 * Class meant to hold a List View of slides thumbnails
 * This view lets display slides as thumbnails in a List view, using standard QT
 * view/model framework. It paint a line between thumbnails when dragging and
 * creates a pixmap the contains all icons of the items that are dragged.
 */
class KPrSlidesManagerView : public QListView
{
    Q_OBJECT
public:
    explicit KPrSlidesManagerView(QWidget *parent = nullptr);

    ~KPrSlidesManagerView() override;

    void paintEvent(QPaintEvent *event) override;

    // It emits a slideDblClick signal and then calls the parent
    // implementation
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void contextMenuEvent(QContextMenuEvent *event) override;

    void startDrag(Qt::DropActions supportedActions) override;

    void dropEvent(QDropEvent *ev) override;

    void dragMoveEvent(QDragMoveEvent *ev) override;

    void dragEnterEvent(QDragEnterEvent *event) override;

    void dragLeaveEvent(QDragLeaveEvent *e) override;

    // Reimplemented to provide suitable signals for selection and deselection of items
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;

    // Manage click events outside of items, to provide
    // a suitable active item for the context menu.
    bool eventFilter(QObject *watched, QEvent *event) override;

    void focusOutEvent(QFocusEvent *event) override;

    void focusInEvent(QFocusEvent *event) override;

    /**
     * Creates a pixmap the contains the all icons of the items
     * that are dragged.
     */
    QPixmap createDragPixmap() const;

    /**
     * Calculates the index of the nearest item to the cursor position
     */
    int cursorSlideIndex() const;

    /**
     * Calculates row and column of the nearest item to the cursor position
     * return a QPair with 0, 0 for the first item.
     */
    QPair<int, int> cursorRowAndColumn() const;

protected:
    void wheelEvent(QWheelEvent *event) override;

Q_SIGNALS:

    /** Is emitted if the user has request a context menu */
    void requestContextMenu(QContextMenuEvent *event);

    /** Is emitted if the user has double click an item */
    void slideDblClick();

    // lets update the active page when changing the current index
    // without a item selected event.
    /** Is emitted if the selection has been changed within a procedure code */
    void indexChanged(QModelIndex index);

    /** Is emitted when all items are deselected */
    void selectionCleared();

    /** Is emitted when an item is selected */
    void itemSelected();

    /** Is emitted when the view loses focus */
    void focusLost();

    /** Is emitted when the view get focus */
    void focusGot();

    /** Is emitted when Ctrl + Scrollwheel is used for zooming */
    void zoomIn();

    /** Is emitted when Ctrl + Scrollwheel is used for zooming */
    void zoomOut();

private:
    /**
     * The rect of an items, essentially used to have the size of the full icon
     *
     * @return the rect of the item
     */
    QRect itemSize() const;

    /**
     * Setter for the dragging flag
     *
     * @param flag boolean
     */
    void setDraggingFlag(bool flag = true);

    /**
     * Permit to know if a slide is dragging
     *
     * @return boolean
     */
    bool isDragging() const;

    QRect m_itemSize;
    bool m_draggingFlag;
    int margin;
};

#endif // KPRSLIDESMANAGERVIEW_H
