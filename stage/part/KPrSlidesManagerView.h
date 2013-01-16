/* This file is part of the KDE project
*
* Copyright (C) 2011 Paul Mendez <paulestebanms@gmail.com>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public License
* along with this library; see the file COPYING.LIB.  If not, write to
* the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
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
    explicit KPrSlidesManagerView(QWidget *parent = 0);

    ~KPrSlidesManagerView();

    virtual void paintEvent (QPaintEvent *event);

    //It emits a slideDblClick signal and then calls the parent
    //implementation
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

    virtual void contextMenuEvent(QContextMenuEvent *event);

    virtual void startDrag (Qt::DropActions supportedActions);

    virtual void dropEvent(QDropEvent *ev);

    virtual void dragMoveEvent(QDragMoveEvent *ev);

    virtual void dragEnterEvent(QDragEnterEvent *event);

    virtual void dragLeaveEvent(QDragLeaveEvent *e);

    //Reimplemented to provide suitable signals for selection and deselection of items
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    //Manage click events outside of items, to provide
    //a suitable active item for the context menu.
    virtual bool eventFilter(QObject *watched, QEvent *event);

    void focusOutEvent(QFocusEvent *event);

    void focusInEvent(QFocusEvent *event);

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
    virtual void wheelEvent(QWheelEvent *event);

signals:

    /** Is emitted if the user has request a context menu */
    void requestContextMenu(QContextMenuEvent *event);

    /** Is emitted if the user has double click an item */
    void slideDblClick();

    //lets update the active page when changing the current index
    //without a item selected event.
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
     * The rect of an items, essentialy used to have the size of the full icon
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
