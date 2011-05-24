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
class KoToolProxy;

class KPrSlidesManagerView : public QListView
{
    Q_OBJECT
public:
    explicit KPrSlidesManagerView(KoToolProxy *toolProxy, QWidget *parent = 0);

    ~KPrSlidesManagerView();

    virtual void paintEvent ( QPaintEvent * event);

    virtual void mouseDoubleClickEvent(QMouseEvent *event);

    virtual void contextMenuEvent(QContextMenuEvent *event);

    virtual void startDrag ( Qt::DropActions supportedActions );

    virtual void dropEvent(QDropEvent* ev);

    virtual void dragMoveEvent(QDragMoveEvent* ev);

    virtual void dragEnterEvent(QDragEnterEvent *event);

    virtual bool eventFilter(QObject *, QEvent *);

    int pageBefore(QPoint point);

    /**
     * Setter of the size with a rect
     *
     * @param size which is a QRect
     */
    void setItemSize(QRect size);


signals:

    void requestContextMenu(QContextMenuEvent *event);
    void slideDblClick();
    void indexChanged(QModelIndex index);

private:

    void setLastItemNumber(int number);

    int lastItemNumber();

    /**
     * The rect of an items, essentialy used to have the size of the full icon
     *
     * @return the rect of the item
     */
    QRect itemSize() const;

    /**
     * Setter for the draging flag
     *
     * @param flag boolean
     */
    void setDragingFlag(bool flag = true);

    /**
     * Permit to know if a slide is draging
     *
     * @return boolean
     */
    bool isDraging();

    int m_lastItemNumber;
    QRect m_itemSize;
    bool m_dragingFlag;
    KoToolProxy * m_toolProxy;

};

#endif // KPRSLIDESMANAGERVIEW_H
